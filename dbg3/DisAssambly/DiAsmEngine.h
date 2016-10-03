#pragma once
#include "../BreakpointEngine/BreakpointEngine.h"
#include <vector>
using std::vector;
#include <map>
using std::map;
using std::pair;
#include <list>
using std::list;

#define	MAX_OPCODESTRING	64

#ifndef DEASM_STRUCT
#define DEASM_STRUCT 
typedef	struct DISASMSTRUST
{
	SIZE_T	address;
	DWORD	dwCodeLen;

	CString	strOpCode;
	CString	strAsm;
	CString strCom; // 注释
}DISASMSTRUST;
#endif



void byteArr2HexStr(const LPBYTE& lpbArr , DWORD dwBSize , TCHAR* pszHexStr , const TCHAR wcSpace);
void byteArr2HexStr(const LPBYTE& lpbArr , DWORD dwBSize , char* pszHexStr , const TCHAR wcSpace);


typedef struct MODULEDISASM
{
	LONG32	uImgBase;
	LONG32	uSize;
	map<LONG32 , DISASMSTRUST> mapDisAsm;
}MODULEDISASM , *PMODULEDISASM;

// 为各个模块保存反汇编信息,地址和反汇编代码的映射关系
class DisAsmEngine 
{
	list<MODULEDISASM> m_moduleInfo;
	BreakpointEngine* m_BPEngine;
public:
	DisAsmEngine(BreakpointEngine* pBPEngine);
	~DisAsmEngine( );
public:

	// 获取远程进程指定地址的指令的长度
	int getCoodeLen(const SIZE_T lpAddress);

	// 获取远程进程指令的opcode
	BYTE getOpCode(SIZE_T uAddress);
	// 解析出远程进程的指定地址出的一条汇编指令
	int diAsm(const SIZE_T lpAddress,CString& strOpCode,CString& strAsm,CString & strCom);
	int diAsm(const SIZE_T lpAddress,vector<DISASMSTRUST>& vecAsm,DWORD dwLine);


	// 获取当前地址的前x行的反汇编指令地址
	void AnalysisDisAsm();
	bool GetDisAsm(const LPBYTE pData,
				   LONG32 begAddr , 
				   LONG32 size,
				   LONG32 addr , 
				   map<LONG32 , 
				   DISASMSTRUST>& mapDisAsm);
};

