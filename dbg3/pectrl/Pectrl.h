#pragma once
#include "windows.h"
#include <list>
using std::list;



typedef BOOL (WINAPI *fnVirtualProtect)(LPVOID lpAddress ,SIZE_T dwSize ,DWORD flNewProtect ,PDWORD lpflOldProtect);
typedef HMODULE (WINAPI *fnLoadLibraryA)(LPCSTR lpLibFileName);
typedef	LPVOID(WINAPI *fnGetProcAddr)(HMODULE , const char* pszName);

// �󾭹�һ�����ȶ�����ֵ
DWORD ToAligentSize(DWORD nSize , DWORD nAligent);

// �����ڴ�ƫ��ת�ļ�ƫ��
DWORD RVAToOfs(const LPVOID pDosHdr ,DWORD dwRVA);

// �ж��Ƿ���ЧPE�ļ�
bool  isPeFile(const LPVOID pDosHdr);

// ��ȡPE�ļ�ͷ
inline PIMAGE_FILE_HEADER	GetPEFileHdr(const LPVOID pDosHdr);

// ��ȡPE��չͷ
// get pe option header ��ȡpe�ļ�����չͷ
inline PIMAGE_OPTIONAL_HEADER32 GetPEOptHdr32(const LPVOID pDosHdr);
inline PIMAGE_OPTIONAL_HEADER64 GetPEOptHdr64(const LPVOID pDosHdr);

// ��ȡ���α�ͷ
// get pe first section header ��ȡpe�ļ��ĵ�һ������ͷ
PIMAGE_SECTION_HEADER	GetPEFirScnHdr(const LPVOID pDosHdr);
PIMAGE_SECTION_HEADER	GetPELastScnHdr(const LPVOID pDosHdr);

// ��ȡ�����
// get pe import table
DWORD GetPEImpTab(const LPVOID pDosHdr , unsigned int *puSize=NULL);

// ��ȡ����Ŀ¼��
PIMAGE_DATA_DIRECTORY GetPEDataDirTab(const LPVOID pDosHdr);

// ��ȡ������
// get pe export table
DWORD GetPEExpTab(const LPVOID pDosHdr , unsigned int *puSize = NULL);

// ��ȡӳ���С
// get pe image size ��ȡpe�ļ���ӳ���С
DWORD	GetPEImageSize(const LPVOID pDosHdr);

// ��ȡ����PEͷ�Ĵ�С
// get pe header size 
DWORD	GetPEHdrSize(const LPVOID pDosHdr);

// ��ȡһ��RVA����������
PIMAGE_SECTION_HEADER	GetPESection(const LPVOID pDosHdr , DWORD dwRVA);

// ��ȡPE�ļ��Ĵ�С
// get pe header size
DWORD	GetPEImgSize(const LPVOID pDosHdr );

// ����PE�ļ��Ĵ�С
// set pe header size
void	SetPEImgSize(const LPVOID pDosHdr , DWORD dwSize);

// ����������Ϣ����ӳ���С
void	FixPEImgSize(const LPVOID pDosHdr);

// ��PE�ļ���ȡ���ڴ���
LPBYTE	LoadPEFile(const wchar_t* pszPath,unsigned int* puSize=NULL);

// ��PE�ļ����ڴ����ͷ�
void	FreePEFile(const LPBYTE lpData);

// ��PE�ļ����ڴ汣�浽�����ļ���
bool	SavePEFile(const LPBYTE lpData ,DWORD dwSize , const TCHAR* pszFilePath);

// ���һ������
// add pe section ��pe�ļ������һ������
PIMAGE_SECTION_HEADER	AddPEScn(const LPVOID pDosHdr , const PIMAGE_SECTION_HEADER pScnHdr);

/** �޸��Ѿ����ص��ڴ���ļ����ض�λ- */
bool FixPERel(LPVOID pDosHdr ,
			  DWORD uOldImgBase ,
			  DWORD uNewImgBase ,
			  DWORD	dwBaseVirtualAddress ,
			  DWORD dwNewVirtualAddress ,
			  LPVOID uRelTabAddr ,
			  fnVirtualProtect pfnVirPro
			  );

/** �޸�δ���ص��ڴ���ļ����ض�λ- */
bool FixPERel(LPVOID pDosHdr ,
			  DWORD uOldImgBase ,
			  DWORD uNewImgBase ,
			  DWORD	dwBaseVirtualAddress ,
			  DWORD dwNewVirtualAddress ,
			  LPVOID uRelTabAddr
			  );

/** �޸�IAT */
bool FixPEIAT(LPVOID pDosHdr ,
			  LPVOID pImgTabs , 
			  fnVirtualProtect pfnVirtualProtect ,
			  fnLoadLibraryA pfnLoadLibraryA ,
			  fnGetProcAddr pfnGetProcAddr
			  );


// ��ȡģ��ĺ���/�ṹ�� ����VA
LPVOID GetPEProcAddress(LPVOID hModule ,  LPVOID pExpTab , const char* pszName);
