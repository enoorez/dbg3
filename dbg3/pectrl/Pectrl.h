#pragma once
#include "windows.h"
#include <list>
using std::list;



typedef BOOL (WINAPI *fnVirtualProtect)(LPVOID lpAddress ,SIZE_T dwSize ,DWORD flNewProtect ,PDWORD lpflOldProtect);
typedef HMODULE (WINAPI *fnLoadLibraryA)(LPCSTR lpLibFileName);
typedef	LPVOID(WINAPI *fnGetProcAddr)(HMODULE , const char* pszName);

// 求经过一定粒度对齐后的值
DWORD ToAligentSize(DWORD nSize , DWORD nAligent);

// 虚拟内存偏移转文件偏移
DWORD RVAToOfs(const LPVOID pDosHdr ,DWORD dwRVA);

// 判断是否有效PE文件
bool  isPeFile(const LPVOID pDosHdr);

// 获取PE文件头
inline PIMAGE_FILE_HEADER	GetPEFileHdr(const LPVOID pDosHdr);

// 获取PE扩展头
// get pe option header 获取pe文件的扩展头
inline PIMAGE_OPTIONAL_HEADER32 GetPEOptHdr32(const LPVOID pDosHdr);
inline PIMAGE_OPTIONAL_HEADER64 GetPEOptHdr64(const LPVOID pDosHdr);

// 获取区段表头
// get pe first section header 获取pe文件的第一个区段头
PIMAGE_SECTION_HEADER	GetPEFirScnHdr(const LPVOID pDosHdr);
PIMAGE_SECTION_HEADER	GetPELastScnHdr(const LPVOID pDosHdr);

// 获取导入表
// get pe import table
DWORD GetPEImpTab(const LPVOID pDosHdr , unsigned int *puSize=NULL);

// 获取数据目录表
PIMAGE_DATA_DIRECTORY GetPEDataDirTab(const LPVOID pDosHdr);

// 获取导出表
// get pe export table
DWORD GetPEExpTab(const LPVOID pDosHdr , unsigned int *puSize = NULL);

// 获取映像大小
// get pe image size 获取pe文件的映像大小
DWORD	GetPEImageSize(const LPVOID pDosHdr);

// 获取整个PE头的大小
// get pe header size 
DWORD	GetPEHdrSize(const LPVOID pDosHdr);

// 获取一个RVA所属的区段
PIMAGE_SECTION_HEADER	GetPESection(const LPVOID pDosHdr , DWORD dwRVA);

// 获取PE文件的大小
// get pe header size
DWORD	GetPEImgSize(const LPVOID pDosHdr );

// 设置PE文件的大小
// set pe header size
void	SetPEImgSize(const LPVOID pDosHdr , DWORD dwSize);

// 根据区段信息修正映像大小
void	FixPEImgSize(const LPVOID pDosHdr);

// 将PE文件读取到内存中
LPBYTE	LoadPEFile(const wchar_t* pszPath,unsigned int* puSize=NULL);

// 将PE文件从内存中释放
void	FreePEFile(const LPBYTE lpData);

// 将PE文件从内存保存到磁盘文件中
bool	SavePEFile(const LPBYTE lpData ,DWORD dwSize , const TCHAR* pszFilePath);

// 添加一个区段
// add pe section 在pe文件中添加一个区段
PIMAGE_SECTION_HEADER	AddPEScn(const LPVOID pDosHdr , const PIMAGE_SECTION_HEADER pScnHdr);

/** 修复已经加载到内存的文件的重定位- */
bool FixPERel(LPVOID pDosHdr ,
			  DWORD uOldImgBase ,
			  DWORD uNewImgBase ,
			  DWORD	dwBaseVirtualAddress ,
			  DWORD dwNewVirtualAddress ,
			  LPVOID uRelTabAddr ,
			  fnVirtualProtect pfnVirPro
			  );

/** 修复未加载到内存的文件的重定位- */
bool FixPERel(LPVOID pDosHdr ,
			  DWORD uOldImgBase ,
			  DWORD uNewImgBase ,
			  DWORD	dwBaseVirtualAddress ,
			  DWORD dwNewVirtualAddress ,
			  LPVOID uRelTabAddr
			  );

/** 修改IAT */
bool FixPEIAT(LPVOID pDosHdr ,
			  LPVOID pImgTabs , 
			  fnVirtualProtect pfnVirtualProtect ,
			  fnLoadLibraryA pfnLoadLibraryA ,
			  fnGetProcAddr pfnGetProcAddr
			  );


// 获取模块的函数/结构体 返回VA
LPVOID GetPEProcAddress(LPVOID hModule ,  LPVOID pExpTab , const char* pszName);
