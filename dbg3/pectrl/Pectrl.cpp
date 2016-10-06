#include "Pectrl.h"
#include <stdio.h>

/** ��һ��ֵ����һ�����ȶ�����ֵ */
DWORD ToAligentSize(DWORD nSize , DWORD nAligent)
{
	// �����˶��ٱ����ڴ����,�������ٱ�,���ж��ٱ��ڴ���뵥λ ;  
	// ��ͷ�Ƿ񳬳��ڴ����,��������һ���ڴ���뵥λ
	if(nSize%nAligent != 0)
		return (nSize / nAligent + 1)*nAligent;
	return nSize;
}

/** �����ڴ�ƫ��ת�ļ�ƫ�� */
DWORD RVAToOfs(const LPVOID pDosHdr , DWORD dwRVA)
{

	//��ʼ�������β��Ұ���RVA��ַ������
	//��ȡ��׼ͷָ��,�Ի�ȡ������Ŀ
	//��ȡ������Ŀ
	DWORD	dwSecTotal = GetPEFileHdr(pDosHdr)->NumberOfSections;

	//��ȡ��һ������
	PIMAGE_SECTION_HEADER	pScn = GetPEFirScnHdr(pDosHdr);

	//��������
	for(DWORD i = 0; i < dwSecTotal; i++)
	{
		if(dwRVA >= pScn->VirtualAddress
		   && dwRVA < pScn->VirtualAddress + pScn->Misc.VirtualSize)
		{
			// rva ת �ļ�ƫ�ƹ�ʽ:
			// rva - ��������rva + ���������ļ�ƫ��
			return dwRVA - pScn->VirtualAddress + pScn->PointerToRawData;
		}
		++pScn;
	}
	return 0;
}

/** ��ȡPE��NTͷ */
PIMAGE_NT_HEADERS GetPENtHdr(const LPVOID pDosHdr)
{
	return (PIMAGE_NT_HEADERS)((((PIMAGE_DOS_HEADER)(pDosHdr))->e_lfanew + (LPBYTE)(pDosHdr)));
}

/** ��ȡPE�ļ�ͷ */
PIMAGE_FILE_HEADER GetPEFileHdr(const LPVOID pDosHdr)
{
	return &((PIMAGE_NT_HEADERS)((((PIMAGE_DOS_HEADER)(pDosHdr))->e_lfanew + (LPBYTE)(pDosHdr))))->FileHeader;
}

/** ��ȡPE��չͷ */
PIMAGE_OPTIONAL_HEADER32 GetPEOptHdr32(const LPVOID pDosHdr)
{
	return &((PIMAGE_NT_HEADERS)((((PIMAGE_DOS_HEADER)(pDosHdr))->e_lfanew + (LPBYTE)(pDosHdr))))->OptionalHeader;
}

/** ��ȡPE��չͷ */
PIMAGE_OPTIONAL_HEADER64 GetPEOptHdr64(const LPVOID pDosHdr)
{
	return &((PIMAGE_NT_HEADERS64)((((PIMAGE_DOS_HEADER)(pDosHdr))->e_lfanew + (LPBYTE)(pDosHdr))))->OptionalHeader;
}

/** ��ȡPE�ļ��ĵ�һ������ͷ */
PIMAGE_SECTION_HEADER GetPEFirScnHdr(const LPVOID pDosHdr)
{
	return IMAGE_FIRST_SECTION(GetPENtHdr(pDosHdr));
}

PIMAGE_SECTION_HEADER GetPELastScnHdr(const LPVOID pDosHdr)
{
	DWORD	dwNumOfScn = GetPEFileHdr(pDosHdr)->NumberOfSections;
	return &GetPEFirScnHdr(pDosHdr)[ dwNumOfScn - 1 ];
}


/** �ж��Ƿ���һ����Ч��PE�ļ� */
bool isPeFile(const LPVOID pDosHdr)
{
	return (*((WORD*)pDosHdr) == 'ZM') && (*((WORD*)GetPENtHdr(pDosHdr)) == 'EP');
}

/** ��ȡPE�ļ���ӳ���С */
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


/** ��ȡPE�ļ�������ͷ���Ĵ�С(����DOS,PEͷ,���α�ͷ) */
DWORD GetPEHdrSize(const LPVOID pDosHdr)
{
	if(pDosHdr == NULL)
		return 0;

	// ��ȡPE�ļ�ͷ����С
	PIMAGE_OPTIONAL_HEADER32 pOptHdr = NULL;/*option header ��չͷ*/
	pOptHdr = GetPEOptHdr32(pDosHdr);
	DWORD dwHdrSize = 0;
	if(pOptHdr->Magic == 0x10B) //32λ�ĳ���
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

/** ��ȡӳ���С */
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

/** ����PE�ļ�����ͷ���Ĵ�С(�ı���չͷ�е�SizeOfImage�ֶ� */
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


/** ���һ������ */
PIMAGE_SECTION_HEADER AddPEScn(const LPVOID pDosHdr , const PIMAGE_SECTION_HEADER pScnHdr)
{
	if(pDosHdr == NULL || pScnHdr == NULL)
		return NULL;

	//		+--------------------+ <-+
	//		|		DOSͷ		 |	 |
	//		+--------------------+	 |
	//		|		�ļ�ͷ   	 |	 |
	//		+--------------------+	 |
	//		|		��չͷ		 |	 |>> ͷ����С
	//		+--------------------+	 |
	//		|		����ͷ1		 |	 |
	//		|		����ͷ2		 |	 |
	//		+--------------------+   |
	//      |		����ֽ�		 |   |
	//		+--------------------+ <-+
	//		|		��������		 |

	DWORD	dwHdrSize = 0;/*header size ͷ����С*/
	DWORD	dwAllScnSize = 0;/*all section size �������εĴ�С*/

	// ��ȡPE�ļ�ͷ����С
	dwHdrSize = GetPEHdrSize(pDosHdr);
	dwHdrSize =
		dwHdrSize
		- sizeof(IMAGE_DOS_HEADER)
		- sizeof(IMAGE_FILE_HEADER)
		- GetPEFileHdr(pDosHdr)->SizeOfOptionalHeader;

	// �õ���������ͷ�������ʵ��ռ���ֽ���
	// ��ʽ : 
	//	 ���������ֽ��� = ��������� * ������������ֽ���
	DWORD	dwNumOfScn = GetPEFileHdr(pDosHdr)->NumberOfSections;
	dwAllScnSize = dwNumOfScn * sizeof(IMAGE_SECTION_HEADER);
	
	// �ж�ͷ���Ƿ��㹻�����µ�����
	// ���ֽ��� + һ����������ֽ��� �� PE�ļ�ͷ���ܴ�С�Ƚ�
	if((dwAllScnSize + sizeof(IMAGE_SECTION_HEADER) > dwHdrSize))
		return NULL;

	// ��������д�뵽���һ������֮��
	//		+--------------------+
	//		|		����ͷ1		 |
	//		|		����ͷ2		 |
	//		|		������ͷ		 |
	//		+--------------------+
	// �ҵ����һ������,�õ����һ������ͷ������
	PIMAGE_SECTION_HEADER pLastScnHdr = NULL;
	// ���һ������ = ��һ�����ε�ַ + ���θ��� - 1 
	pLastScnHdr = GetPEFirScnHdr(pDosHdr) + (dwNumOfScn - 1) ;


/**
  * ����ͷ�������еĸ��������С��ƫ�Ƶ��ֶεĽ���:
  * ���δ�С : 
  *  Misc.VirtualSize : ���ܴ���SizeOfRowData
  *  SizeOfRawData	  : �����ļ����ȶ���������ֽ���
  * ����ƫ��:
  *	 PointerToRawData : ���ε��ļ�ƫ��, ���ֵ������һ�������ļ����ȶ����ֵ 
  *  VirtualAddress   : ���ε������ڴ�ƫ��, ���ֵ������һ�������ڴ����ȶ����ֵ
  */

	// ���������ε��ļ�ƫ�ƺ������ڴ�ƫ��
	// �����ε��ļ�ƫ�� = ���һ�����ε��ļ�ƫ�� + ���һ�����ε��ֽ���
	pScnHdr->PointerToRawData 
		= pLastScnHdr->PointerToRawData/*���һ�����ε����ļ��е�ƫ�ƿ�ʼ*/
		  + pLastScnHdr->SizeOfRawData/*���һ�����ξ����ļ����ȶ������ֽ���*/;


	// �����ε������ڴ�ƫ�� = ���һ�����ε������ڴ�ƫ��+���һ������
	// �����ڴ����ȶ������ֽ���
	pScnHdr->VirtualAddress 
		= pLastScnHdr->VirtualAddress /*���һ�����������ڴ�ƫ��*/
		  + ToAligentSize(pLastScnHdr->SizeOfRawData,0x1000)/*�����ڴ����ȶ�������δ�С*/;

	// ���������εľ����ļ����ȶ�������δ�С
	pScnHdr->SizeOfRawData = ToAligentSize(pScnHdr->Misc.VirtualSize , 0x200);

	// �������ο��������һ������֮��
	memcpy_s(pLastScnHdr + 1 ,/*���һ������֮��*/
			 sizeof(IMAGE_SECTION_HEADER) ,
			 pScnHdr ,/*������*/
			 sizeof(IMAGE_SECTION_HEADER)
			 );

	// ����������Ŀ
	++GetPEFileHdr(pDosHdr)->NumberOfSections;

	// �޸���չͷ�е�ӳ���С
	// �µ�����ͷ�Ǳ�����ͷ��ԭ�еĿռ���,��û������ͷ���Ĵ�С.
	// �µ�ӳ���С = ���һ�����ε������ڴ�RVA + ���δ�С(�ļ�����)
	SetPEImgSize(pDosHdr ,
				 (pLastScnHdr+1)->VirtualAddress + (pLastScnHdr+1)->SizeOfRawData
				 );

	// �����µ�����
	return (pLastScnHdr+1);
}


/** ��ȡ����Ŀ¼�� */
PIMAGE_DATA_DIRECTORY GetPEDataDirTab(const LPVOID pDosHdr)
{
	// �õ��������ļ�ƫ��
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


// ��ȡģ��ĺ��� ���غ�����VA
LPVOID GetPEProcAddress(LPVOID hModule , LPVOID pExpTab , const char* pszName)
{
	DWORD	uNumOfName;/*�������Ƹ���*/
	DWORD	uNumOfFun;/*�����ܸ���*/
	PSIZE_T	puFunAddr ;/*������ַ��*/
	PSIZE_T	puFunName;/*�������Ʊ�*/
	PWORD	pwFunOrd;/*������ű�*/
	PIMAGE_EXPORT_DIRECTORY pExp = (PIMAGE_EXPORT_DIRECTORY)pExpTab;
	if(pExpTab == 0)
		return 0;

	/*��ȡ���Ʊ�*/
	puFunName = (PSIZE_T)(RVAToOfs(hModule , pExp->AddressOfNames) + (DWORD)hModule);
	/*��ȡ��ַ��*/
	puFunAddr = (PSIZE_T)(RVAToOfs(hModule , pExp->AddressOfFunctions) + (DWORD)hModule);
	/*��ȡ��ű�*/
	pwFunOrd = (PWORD)(RVAToOfs(hModule , pExp->AddressOfNameOrdinals) + (DWORD)hModule);

	// ��ȡ��������
	uNumOfName = pExp->NumberOfNames;
	uNumOfFun = pExp->NumberOfFunctions;

	char* pName = 0;;
	// ������ű�
	for(DWORD i = 0 ; i < uNumOfFun; ++i)
	{
		DWORD j = 0;
		for(; j < uNumOfName; ++j)
		{
			// ��������
			if(i == pwFunOrd[ j ])
				break;
		}
		if(j < uNumOfFun)
		{
			pName = (char*)(RVAToOfs(hModule , puFunName[ j ]) + (DWORD)hModule);
			if(strcmp(pName , pszName) == 0)
			{
				// ����һ��VA
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