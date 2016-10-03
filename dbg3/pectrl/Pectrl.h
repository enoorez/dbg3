#pragma once
#include "windows.h"



// 求经过一定粒度对齐后的值
DWORD ToAligentSize(DWORD nSize , DWORD nAligent);

// 虚拟内存偏移转文件偏移
DWORD RVAToOfs(const LPVOID pDosHdr ,DWORD dwRVA);

// 判断是否有效PE文件
bool  isPeFile(const LPVOID pDosHdr);

// 获取PE文件头
inline PIMAGE_FILE_HEADER	GetPEFileHdr(const LPVOID pDosHdr);

// 获取PE扩展头
// get pe option head 获取pe文件的扩展头
inline PIMAGE_OPTIONAL_HEADER32 GetPEOptHdr32(const LPVOID pDosHdr);
inline PIMAGE_OPTIONAL_HEADER64 GetPEOptHdr64(const LPVOID pDosHdr);

// 获取区段表头
// get pe first section head 获取pe文件的第一个区段头
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
// get pe head size 
DWORD	GetPEHdrSize(const LPVOID pDosHdr);

// 获取一个RVA所属的区段
PIMAGE_SECTION_HEADER	GetPESection(const LPVOID pDosHdr , DWORD dwRVA);

// 获取PE文件的大小
// get pe head size
DWORD	GetPEImgSize(const LPVOID pDosHdr );

// 设置PE文件的大小
// set pe head size
void	SetPEImgSize(const LPVOID pDosHdr , DWORD dwSize);

// 根据区段信息修正映像大小
void	FixPEImgSize(const LPVOID pDosHdr);

// 添加一个区段
// add pe section 在pe文件中添加一个区段
PIMAGE_SECTION_HEADER	AddPEScn(const LPVOID pDosHdr , const PIMAGE_SECTION_HEADER pScnHdr);

// 获取模块的函数 返回函数的VA
LPVOID GetPEProcAddress(LPVOID hModule , LPVOID pExpTab , const char* pszName);