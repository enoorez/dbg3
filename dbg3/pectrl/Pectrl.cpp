#include "Pectrl.h"
#include <stdio.h>

/** 求一个值经过一定粒度对齐后的值 */
DWORD ToAligentSize(DWORD nSize , DWORD nAligent)
{
	// 超出了多少倍的内存对齐,超出多少倍,就有多少倍内存对齐单位 ;  
	// 零头是否超出内存对齐,超出则是一个内存对齐单位
	if(nSize%nAligent != 0)
		return (nSize / nAligent + 1)*nAligent;
	return nSize;
}

/** 虚拟内存偏移转文件偏移 */
DWORD RVAToOfs(const LPVOID pDosHdr , DWORD dwRVA)
{

	//开始遍历区段查找包含RVA地址的区段
	//获取标准头指针,以获取区段数目
	//获取区段数目
	DWORD	dwSecTotal = GetPEFileHdr(pDosHdr)->NumberOfSections;

	//获取第一个区段
	PIMAGE_SECTION_HEADER	pScn = GetPEFirScnHdr(pDosHdr);

	//遍历区段
	for(DWORD i = 0; i < dwSecTotal; i++)
	{
		if(dwRVA >= pScn->VirtualAddress
		   && dwRVA < pScn->VirtualAddress + pScn->Misc.VirtualSize)
		{
			// rva 转 文件偏移公式:
			// rva - 区段所在rva + 区段所在文件偏移
			return dwRVA - pScn->VirtualAddress + pScn->PointerToRawData;
		}
		++pScn;
	}
	return 0;
}

/** 获取PE的NT头 */
PIMAGE_NT_HEADERS GetPENtHdr(const LPVOID pDosHdr)
{
	return (PIMAGE_NT_HEADERS)((((PIMAGE_DOS_HEADER)(pDosHdr))->e_lfanew + (LPBYTE)(pDosHdr)));
}

/** 获取PE文件头 */
PIMAGE_FILE_HEADER GetPEFileHdr(const LPVOID pDosHdr)
{
	return &((PIMAGE_NT_HEADERS)((((PIMAGE_DOS_HEADER)(pDosHdr))->e_lfanew + (LPBYTE)(pDosHdr))))->FileHeader;
}

/** 获取PE扩展头 */
PIMAGE_OPTIONAL_HEADER32 GetPEOptHdr32(const LPVOID pDosHdr)
{
	return &((PIMAGE_NT_HEADERS)((((PIMAGE_DOS_HEADER)(pDosHdr))->e_lfanew + (LPBYTE)(pDosHdr))))->OptionalHeader;
}

/** 获取PE扩展头 */
PIMAGE_OPTIONAL_HEADER64 GetPEOptHdr64(const LPVOID pDosHdr)
{
	return &((PIMAGE_NT_HEADERS64)((((PIMAGE_DOS_HEADER)(pDosHdr))->e_lfanew + (LPBYTE)(pDosHdr))))->OptionalHeader;
}

/** 获取PE文件的第一个区段头 */
PIMAGE_SECTION_HEADER GetPEFirScnHdr(const LPVOID pDosHdr)
{
	return IMAGE_FIRST_SECTION(GetPENtHdr(pDosHdr));
}

PIMAGE_SECTION_HEADER GetPELastScnHdr(const LPVOID pDosHdr)
{
	DWORD	dwNumOfScn = GetPEFileHdr(pDosHdr)->NumberOfSections;
	return &GetPEFirScnHdr(pDosHdr)[ dwNumOfScn - 1 ];
}


/** 判断是否是一个有效的PE文件 */
bool isPeFile(const LPVOID pDosHdr)
{
	return (*((WORD*)pDosHdr) == 'ZM') && (*((WORD*)GetPENtHdr(pDosHdr)) == 'EP');
}

/** 获取PE文件的映像大小 */
DWORD GetPEImageSize(const LPVOID pDosHdr)
{
	DWORD dwSize = 0 ;
	PIMAGE_OPTIONAL_HEADER32 pOptHdr = NULL;
	pOptHdr = GetPEOptHdr32(pDosHdr);

	if(pOptHdr->Magic == 0x10B)
	{
		dwSize = pOptHdr->SizeOfImage;
	}
	else
	{
		PIMAGE_OPTIONAL_HEADER64 pOptHdr64 = NULL;
		pOptHdr64 = GetPEOptHdr64(pDosHdr);
		dwSize = pOptHdr64->SizeOfImage;
	}

	return dwSize;
}


/** 获取PE文件的整个头部的大小(包括DOS,PE头,区段表头) */
DWORD GetPEHdrSize(const LPVOID pDosHdr)
{
	if(pDosHdr == NULL)
		return 0;

	// 获取PE文件头部大小
	PIMAGE_OPTIONAL_HEADER32 pOptHdr = NULL;/*option header 扩展头*/
	pOptHdr = GetPEOptHdr32(pDosHdr);
	DWORD dwHdrSize = 0;
	if(pOptHdr->Magic == 0x10B) //32位的程序
	{
		dwHdrSize = pOptHdr->SizeOfHeaders;
	}
	else
	{
		PIMAGE_OPTIONAL_HEADER64 pOptHdr64 = NULL;
		pOptHdr64 = GetPEOptHdr64(pDosHdr);
		dwHdrSize = pOptHdr64->SizeOfHeaders;
	}
	return dwHdrSize;

}

/** 获取映像大小 */
DWORD GetPEImgSize(const LPVOID pDosHdr )
{
	if(pDosHdr == NULL)
		return 0;

	PIMAGE_OPTIONAL_HEADER32 pOptHdr = NULL;
	pOptHdr = GetPEOptHdr32(pDosHdr);

	if(pOptHdr->Magic == 0x10B)
	{
		return pOptHdr->SizeOfImage;
	}
	else
	{
		PIMAGE_OPTIONAL_HEADER64 pOptHdr64 = NULL;
		pOptHdr64 = GetPEOptHdr64(pDosHdr);
		return pOptHdr64->SizeOfImage;
	}
	return 0;
}

/** 设置PE文件整个头部的大小(改变扩展头中的SizeOfImage字段 */
void SetPEImgSize(const LPVOID pDosHdr , DWORD dwSize)
{
	if(pDosHdr == NULL)
		return ;

	PIMAGE_OPTIONAL_HEADER32 pOptHdr = NULL;
	pOptHdr = GetPEOptHdr32(pDosHdr);

	if(pOptHdr->Magic == 0x10B)
	{
		pOptHdr->SizeOfImage = dwSize;
	}
	else
	{
		PIMAGE_OPTIONAL_HEADER64 pOptHdr64 = NULL;
		pOptHdr64 = GetPEOptHdr64(pDosHdr);
		pOptHdr64->SizeOfImage = dwSize;
	}
	
}


/** 添加一个区段 */
PIMAGE_SECTION_HEADER AddPEScn(const LPVOID pDosHdr , const PIMAGE_SECTION_HEADER pScnHdr)
{
	if(pDosHdr == NULL || pScnHdr == NULL)
		return NULL;

	//		+--------------------+ <-+
	//		|		DOS头		 |	 |
	//		+--------------------+	 |
	//		|		文件头   	 |	 |
	//		+--------------------+	 |
	//		|		扩展头		 |	 |>> 头部大小
	//		+--------------------+	 |
	//		|		区段头1		 |	 |
	//		|		区段头2		 |	 |
	//		+--------------------+   |
	//      |		填充字节		 |   |
	//		+--------------------+ <-+
	//		|		区段数据		 |

	DWORD	dwHdrSize = 0;/*header size 头部大小*/
	DWORD	dwAllScnSize = 0;/*all section size 所有区段的大小*/

	// 获取PE文件头部大小
	dwHdrSize = GetPEHdrSize(pDosHdr);
	dwHdrSize =
		dwHdrSize
		- sizeof(IMAGE_DOS_HEADER)
		- sizeof(IMAGE_FILE_HEADER)
		- GetPEFileHdr(pDosHdr)->SizeOfOptionalHeader;

	// 得到所有区段头描述表的实际占用字节数
	// 公式 : 
	//	 描述表总字节数 = 描述表个数 * 单个描述表的字节数
	DWORD	dwNumOfScn = GetPEFileHdr(pDosHdr)->NumberOfSections;
	dwAllScnSize = dwNumOfScn * sizeof(IMAGE_SECTION_HEADER);
	
	// 判断头部是否足够容纳新的区段
	// 总字节数 + 一个描述表的字节数 和 PE文件头部总大小比较
	if((dwAllScnSize + sizeof(IMAGE_SECTION_HEADER) > dwHdrSize))
		return NULL;

	// 将新区段写入到最后一个区段之后
	//		+--------------------+
	//		|		区段头1		 |
	//		|		区段头2		 |
	//		|		新区段头		 |
	//		+--------------------+
	// 找到最后一个区段,得到最后一个区段头描述表
	PIMAGE_SECTION_HEADER pLastScnHdr = NULL;
	// 最后一个区段 = 第一个区段地址 + 区段个数 - 1 
	pLastScnHdr = GetPEFirScnHdr(pDosHdr) + (dwNumOfScn - 1) ;


/**
  * 区段头描述表中的各个保存大小和偏移的字段的解释:
  * 区段大小 : 
  *  Misc.VirtualSize : 不能大于SizeOfRowData
  *  SizeOfRawData	  : 经过文件粒度对齐的区段字节数
  * 区段偏移:
  *	 PointerToRawData : 区段的文件偏移, 这个值必须是一个经过文件粒度对齐的值 
  *  VirtualAddress   : 区段的虚拟内存偏移, 这个值必须是一个经过内存粒度对齐的值
  */

	// 设置新区段的文件偏移和虚拟内存偏移
	// 新区段的文件偏移 = 最后一个区段的文件偏移 + 最后一个区段的字节数
	pScnHdr->PointerToRawData 
		= pLastScnHdr->PointerToRawData/*最后一个区段的在文件中的偏移开始*/
		  + pLastScnHdr->SizeOfRawData/*最后一个区段经过文件粒度对齐后的字节数*/;


	// 新区段的虚拟内存偏移 = 最后一个区段的虚拟内存偏移+最后一个区段
	// 经过内存粒度对齐后的字节数
	pScnHdr->VirtualAddress 
		= pLastScnHdr->VirtualAddress /*最后一个区段虚拟内存偏移*/
		  + ToAligentSize(pLastScnHdr->SizeOfRawData,0x1000)/*经过内存粒度对齐的区段大小*/;

	// 设置新区段的经过文件粒度对齐的区段大小
	pScnHdr->SizeOfRawData = ToAligentSize(pScnHdr->Misc.VirtualSize , 0x200);

	// 将新区段拷贝到最后一个区段之后
	memcpy_s(pLastScnHdr + 1 ,/*最后一个区段之后*/
			 sizeof(IMAGE_SECTION_HEADER) ,
			 pScnHdr ,/*新区段*/
			 sizeof(IMAGE_SECTION_HEADER)
			 );

	// 增加区段数目
	++GetPEFileHdr(pDosHdr)->NumberOfSections;

	// 修改扩展头中的映像大小
	// 新的区段头是保存在头部原有的空间中,并没有扩从头部的大小.
	// 新的映像大小 = 最后一个区段的虚拟内存RVA + 区段大小(文件对齐)
	SetPEImgSize(pDosHdr ,
				 (pLastScnHdr+1)->VirtualAddress + (pLastScnHdr+1)->SizeOfRawData
				 );

	// 返回新的区段
	return (pLastScnHdr+1);
}


/** 获取数据目录表 */
PIMAGE_DATA_DIRECTORY GetPEDataDirTab(const LPVOID pDosHdr)
{
	// 得到导入表的文件偏移
	PIMAGE_OPTIONAL_HEADER32 pOptHdr = NULL;
	pOptHdr = GetPEOptHdr32(pDosHdr);

	PIMAGE_DATA_DIRECTORY pDataDir = NULL;

	if(pOptHdr->Magic == 0x10B)
	{
		pDataDir = pOptHdr->DataDirectory;
	}
	else
	{
		PIMAGE_OPTIONAL_HEADER64 pOptHdr64 = NULL;
		pOptHdr64 = GetPEOptHdr64(pDosHdr);
		pDataDir = pOptHdr64->DataDirectory;
	}
	return pDataDir;
}


// 获取模块的函数 返回函数的VA
LPVOID GetPEProcAddress(LPVOID hModule , LPVOID pExpTab , const char* pszName)
{
	DWORD	uNumOfName;/*函数名称个数*/
	DWORD	uNumOfFun;/*函数总个数*/
	PSIZE_T	puFunAddr ;/*函数地址表*/
	PSIZE_T	puFunName;/*函数名称表*/
	PWORD	pwFunOrd;/*函数序号表*/
	PIMAGE_EXPORT_DIRECTORY pExp = (PIMAGE_EXPORT_DIRECTORY)pExpTab;
	if(pExpTab == 0)
		return 0;

	/*获取名称表*/
	puFunName = (PSIZE_T)(RVAToOfs(hModule , pExp->AddressOfNames) + (DWORD)hModule);
	/*获取地址表*/
	puFunAddr = (PSIZE_T)(RVAToOfs(hModule , pExp->AddressOfFunctions) + (DWORD)hModule);
	/*获取序号表*/
	pwFunOrd = (PWORD)(RVAToOfs(hModule , pExp->AddressOfNameOrdinals) + (DWORD)hModule);

	// 获取名称数量
	uNumOfName = pExp->NumberOfNames;
	uNumOfFun = pExp->NumberOfFunctions;

	char* pName = 0;;
	// 遍历序号表
	for(DWORD i = 0 ; i < uNumOfFun; ++i)
	{
		DWORD j = 0;
		for(; j < uNumOfName; ++j)
		{
			// 含有名称
			if(i == pwFunOrd[ j ])
				break;
		}
		if(j < uNumOfFun)
		{
			pName = (char*)(RVAToOfs(hModule , puFunName[ j ]) + (DWORD)hModule);
			if(strcmp(pName , pszName) == 0)
			{
				// 返回一个VA
				return (LPVOID)(puFunAddr[ i ] + (DWORD)hModule);
			}
		}
	}
	return 0;
}


void FixPEImgSize(const LPVOID pDosHdr)
{
	PIMAGE_SECTION_HEADER pLastScn = GetPELastScnHdr(pDosHdr);
	if(pLastScn == NULL)
		return;
	SetPEImgSize(pDosHdr , pLastScn->VirtualAddress + pLastScn->SizeOfRawData);
}

PIMAGE_SECTION_HEADER GetPESection(const LPVOID pDosHdr,DWORD dwRVA)
{
	DWORD	dwNumOfScn = GetPEFileHdr(pDosHdr)->NumberOfSections;
	PIMAGE_SECTION_HEADER pScn = GetPEFirScnHdr(pDosHdr);
	for(DWORD i = 0; i < dwNumOfScn; i++,++pScn)
	{
		if(pScn->VirtualAddress <= dwRVA && dwRVA <= pScn->VirtualAddress + pScn->SizeOfRawData)
			return pScn;
	}
	return NULL;
}