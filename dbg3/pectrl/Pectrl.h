#pragma once
#include "windows.h"



// �󾭹�һ�����ȶ�����ֵ
DWORD ToAligentSize(DWORD nSize , DWORD nAligent);

// �����ڴ�ƫ��ת�ļ�ƫ��
DWORD RVAToOfs(const LPVOID pDosHdr ,DWORD dwRVA);

// �ж��Ƿ���ЧPE�ļ�
bool  isPeFile(const LPVOID pDosHdr);

// ��ȡPE�ļ�ͷ
inline PIMAGE_FILE_HEADER	GetPEFileHdr(const LPVOID pDosHdr);

// ��ȡPE��չͷ
// get pe option head ��ȡpe�ļ�����չͷ
inline PIMAGE_OPTIONAL_HEADER32 GetPEOptHdr32(const LPVOID pDosHdr);
inline PIMAGE_OPTIONAL_HEADER64 GetPEOptHdr64(const LPVOID pDosHdr);

// ��ȡ���α�ͷ
// get pe first section head ��ȡpe�ļ��ĵ�һ������ͷ
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
// get pe head size 
DWORD	GetPEHdrSize(const LPVOID pDosHdr);

// ��ȡһ��RVA����������
PIMAGE_SECTION_HEADER	GetPESection(const LPVOID pDosHdr , DWORD dwRVA);

// ��ȡPE�ļ��Ĵ�С
// get pe head size
DWORD	GetPEImgSize(const LPVOID pDosHdr );

// ����PE�ļ��Ĵ�С
// set pe head size
void	SetPEImgSize(const LPVOID pDosHdr , DWORD dwSize);

// ����������Ϣ����ӳ���С
void	FixPEImgSize(const LPVOID pDosHdr);

// ���һ������
// add pe section ��pe�ļ������һ������
PIMAGE_SECTION_HEADER	AddPEScn(const LPVOID pDosHdr , const PIMAGE_SECTION_HEADER pScnHdr);

// ��ȡģ��ĺ��� ���غ�����VA
LPVOID GetPEProcAddress(LPVOID hModule , LPVOID pExpTab , const char* pszName);