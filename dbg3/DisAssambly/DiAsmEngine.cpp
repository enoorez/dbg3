#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "BeaEngine_4.1/Win32/headers/BeaEngine.h"


#ifndef _WIN64
//#pragma comment(lib, "BeaEngine_4.1/Win32/Win32/Lib/BeaEngine.lib")
#pragma comment(linker, "/NODEFAULTLIB:\"crt.lib\"")
#else
#pragma comment(lib, "../BeaEngine_4.1/Win64/Win64/Lib/BeaEngine64.lib")
#pragma comment(linker, "/NODEFAULTLIB:\"crt64.lib\"")
#endif
#include "DiAsmEngine.h"
#include <strsafe.h>
#include "../BreakpointEngine/BPSoft/BPSoft.h"
#include "../pectrl/Pectrl.h"



DisAsmEngine::DisAsmEngine(BreakpointEngine* pBPEngine) 
	:m_BPEngine(pBPEngine)
{
	m_BPEngine->InitSymbol(pBPEngine->m_hCurrProcess);
}




/*
※※ 函数名: CDiAsmEngine::GetCoodeLen
※※ 功  能: 获取指令长度
※※ 返回值: int
※※ 形  参: const SIZE_T lpAddress
*/
int DisAsmEngine::getCoodeLen(const SIZE_T lpAddress)
{
	// 1. 将调试程序的内存(OPCode)复制到本地
	SIZE_T  dwRetSize = 0;
	LPBYTE  lpRemote_Buf[32];
	ZeroMemory(lpRemote_Buf,32);
	ReadProcessMemory(m_BPEngine->m_hCurrProcess,
					  (LPVOID)lpAddress , 
					  lpRemote_Buf , 
					  32 , 
					  &dwRetSize);
	
	// 2. 初始化反汇编引擎
	DISASM objDiasm;
	objDiasm.EIP = (UIntPtr)lpRemote_Buf; // 起始地址
	objDiasm.VirtualAddr = (UINT64)lpAddress;     // 虚拟内存地址（反汇编引擎用于计算地址）
	objDiasm.Archi = 0;                     // AI-X86
	objDiasm.Options = 0x000;                 // MASM

	// 3. 反汇编代码
	int nLen = Disasm(&objDiasm);
	return nLen;
}




/*
※※ 函数名: byteArr2HexStr
※※ 功  能: 将整形的数组转换成十六进制字符串
※※ 返回值: void
※※ 形  参: const LPBYTE & lpbArr
※※ 形  参: DWORD dwBSize
※※ 形  参: TCHAR * pszHexStr
※※ 形  参: const TCHAR wcSpace
*/
void byteArr2HexStr(const LPBYTE& lpbArr , DWORD dwBSize , TCHAR* pszHexStr , const TCHAR wcSpace)
{
	// 一个字节转换成一个TCHAR
	DWORD	i = 0;
	TCHAR	ucNum[ 3 ] = { 0 };
	BYTE	byteNum = 0;
	DWORD	dwIndex = wcSpace == 0 ? 2 : 3;
	DWORD	j = 0;
	while(j < dwBSize)
	{
		byteNum = *((PBYTE)(lpbArr + j));
        
		// 转成字符串
		swprintf_s(pszHexStr + i , 3 + 1 , L"%02X%c" , byteNum , wcSpace);
		i += dwIndex;
		++j;
	}
}




/*
※※ 函数名: CDiAsmEngine::DiAsm
※※ 功  能: 获取opcode和汇编指令
※※ 返回值: int
※※ 形  参: const SIZE_T lpAddress
※※ 形  参: CString & strOpCode
※※ 形  参: CString & strAsm
※※ 形  参: CString & strCom
*/
int DisAsmEngine::diAsm(const SIZE_T lpAddress,CString& strOpCode,CString& strAsm,CString & strCom)
{
	strOpCode.Empty( );// 清空
	strAsm.Empty( );// 清空
	strCom.Empty( );// 清空

	// 1. 将调试程序的内存(OPCode)复制到本地
	BYTE lpRemote_Buf[ 64 ] = {0};

	if(m_BPEngine->ReadMemory(lpAddress , lpRemote_Buf , 64) != 64)
		return -1;

	// 软件断点上有0xcc,不是正确的opcode,
	// 判断这个地址上有没有软件断点,有则将原来的数据保存到缓冲区的首字节
	// 否则无法正常反汇编
	BPObject* pBP = m_BPEngine->FindBreakpoint((uaddr)lpAddress,breakpointType_soft);
	if(pBP!=NULL)
	{
		*lpRemote_Buf = *(char*)&((BPSoft*)pBP)->m_uData;
	}

	// 2. 初始化反汇编引擎
	DISASM objDiasm;
	objDiasm.EIP = (UIntPtr)lpRemote_Buf; // 起始地址
	objDiasm.VirtualAddr = (UINT64)lpAddress;// 虚拟内存地址（反汇编引擎用于计算地址）
	objDiasm.Archi = 0;                   // AI-X86
	objDiasm.Options = 0x000;             // MASM

	// 3. 反汇编代码
	UINT unLen = Disasm(&objDiasm);
	if(-1 == unLen) 
		return unLen;

	// 4. 将机器码转码为字符串
	strOpCode.GetBufferSetLength(unLen * 2 + 1);/*一个字节得到两个字符最后加上字符串结束符*/
	byteArr2HexStr((LPBYTE)lpRemote_Buf,unLen,(LPWSTR)(LPCTSTR)strOpCode,0);

	// 6. 保存反汇编出的指令
	strAsm = objDiasm.CompleteInstr;

	// 查看指令是否调用了函数, 如果调用了函数, 则使用调试符号服务器解析出符号名
	// call 的机器码有 :
	//  0xe8	: 5byte ,
	//  0x15ff	: 6byte
	// jmp 的机器码是:
	//	0xe9	: 5byte,
	//  0x25ff	: 6byte
	SIZE_T uDesAddress = 0;
	if(*lpRemote_Buf == 0xe8 || *lpRemote_Buf == 0xe9)
	{
		// 获取跳转偏移,获取当前指令地址, 计算出目标地址
		// 计算公式: 偏移 = 目标地址 - 当前地址 - 5
		DWORD dwDispAddr = *(DWORD*)(lpRemote_Buf + 1);
		// 目标地址 = 偏移 + 当前地址 + 5
		uDesAddress = dwDispAddr + 5 + lpAddress;
	}
	else if(*(WORD*)lpRemote_Buf == 0x15ff || *(WORD*)lpRemote_Buf == 0x25ff)
	{
		uDesAddress = *(DWORD*)(lpRemote_Buf + 2);
	}
	if(uDesAddress)
	{
		m_BPEngine->GetFunctionName(m_BPEngine->m_hCurrProcess,
									uDesAddress , 
									strCom);
	}

	return unLen;
}




/*
※※ 函数名: CDiAsmEngine::DiAsm
※※ 功  能: 将指定地址的opcode反汇编,保存到传进的vector中
※※ 返回值: int
※※ 形  参: const SIZE_T lpAddress
※※ 形  参: vector<DIASM> & vecAsm
※※ 形  参: DWORD dwLine
*/
int  DisAsmEngine::diAsm(const SIZE_T lpAddress,vector<DISASMSTRUST>& vecAsm,DWORD dwLine)
{
	TCHAR szOpCode[MAX_OPCODESTRING] = {0};
	TCHAR szAsm[MAX_OPCODESTRING] = {0};
	DISASMSTRUST	stcDeAsm = {0};
	vecAsm.clear();
	SIZE_T	address = (SIZE_T)lpAddress;

	vecAsm.resize(dwLine);
	for(DWORD i = 0; i < dwLine; ++i)
	{
		vecAsm[ i ].dwCodeLen = diAsm(address ,
									  vecAsm[ i ].strOpCode ,
									  vecAsm[ i ].strAsm ,
									  vecAsm[ i ].strCom
									  );
		vecAsm[ i ].address = address;
		if(vecAsm[ i ].dwCodeLen == -1)
		{
			return vecAsm[ i ].dwCodeLen;
		}
		address += vecAsm[ i ].dwCodeLen;
	}
	return TRUE;
}



DisAsmEngine::~DisAsmEngine( )
{

}



void DisAsmEngine::AnalysisDisAsm()
{
	// 获取模块个数
	list<MODULEFULLINFO> moduleInfo;
	m_BPEngine->GetModuleList(moduleInfo);

	m_moduleInfo.resize(moduleInfo.size());

	// 获取每一个模块的反汇编
	// 并建立地址和反汇编代码的映射关系
	// 
	// 采用递归下降分析opcode
	list<MODULEDISASM>::iterator itr = m_moduleInfo.begin();
	BYTE*	pHdrData = new BYTE[ 0x200 ];
	PIMAGE_SECTION_HEADER pScnHdr = nullptr;
	BYTE*   pCode = nullptr;
	for(auto& i : moduleInfo)
	{
		itr->uImgBase = (LONG32)i.uStart;
		itr->uSize = i.uSize;

		if(0x200 != m_BPEngine->ReadMemory(itr->uImgBase , pHdrData , 0x200))
			goto _ERROR;

		// 获取代码段数据
		pScnHdr = GetPEFirScnHdr(pHdrData);
		for(; !(pScnHdr->Characteristics&IMAGE_SCN_MEM_EXECUTE); ++pScnHdr);
		if(pScnHdr->Characteristics&IMAGE_SCN_MEM_EXECUTE)
		{
			pCode = new BYTE[ pScnHdr->SizeOfRawData ];
			if(pScnHdr->SizeOfRawData 
			   != m_BPEngine->ReadMemory(pScnHdr->VirtualAddress , pCode , pScnHdr->SizeOfRawData))
				goto _ERROR;

			// 获取代码入口地址
			LONG32 oep = GetPEOptHdr32(pHdrData)->AddressOfEntryPoint + itr->uImgBase;

			// 开始递归下降分析

			delete[] pCode;
		}
	}


	goto _SUCESS;
_ERROR:
	for (auto& i : m_moduleInfo)
		i.mapDisAsm.clear();
	m_moduleInfo.clear();

_SUCESS :
	if(pHdrData!=nullptr)
		delete[] pHdrData;
	if(pCode != nullptr)
		delete[] pCode;
}

bool DisAsmEngine::GetDisAsm(const LPBYTE pData , 
							 LONG32 begAddr , 
							 LONG32 size , 
							 LONG32 addr , 
							 map<LONG32 , DISASMSTRUST>& mapDisAsm)
{
	// 判断opcode是否含有call指令,jmp指令,jcc指令
	// call 指令的字节码有:
	//  0xe8 : 5byte ,
	// 	*0xff :
	// 
	LONG32	dwDisp = 0;
	DISASMSTRUST	disAsmInfo;
	DWORD	dwNextCodeAddress = addr;
	if(*pData == 0xe8 || *pData == 0xe9) // 函数调用和断跳转
	{
		// 将当条指令加入表中,并到跳转或者调用的目标地址处继续反汇编
		// 计算出目标地址的地址
		// 公式: 
		//  目标地址  = 跳转偏移 + 当前指令地址 + 5
		dwDisp = *(LONG32*)(pData + 1);
		dwNextCodeAddress += dwDisp;
	}
	else if(*(WORD*)pData == 0x15ff || *(WORD*)pData == 0x25ff)
	{
		dwDisp = *(LONG32*)(pData + 2);
	}

	disAsmInfo.address = addr;
	disAsmInfo.dwCodeLen = diAsm(addr ,
								 disAsmInfo.strOpCode ,
								 disAsmInfo.strAsm ,
								 disAsmInfo.strCom
								 );
	pair<map<LONG32 , DISASMSTRUST>::iterator , bool> Ret =
		mapDisAsm.insert(pair<LONG32 , DISASMSTRUST>(addr , disAsmInfo));
	if(Ret.second == false)
		return false;

	GetDisAsm(pData + disAsmInfo.dwCodeLen ,
			  begAddr ,
			  size ,
			  dwNextCodeAddress + disAsmInfo.dwCodeLen ,
			  mapDisAsm);
	
	return 0;
}





