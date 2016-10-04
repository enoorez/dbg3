#include "dbgUi.h"
#include "hightLight.h"
#include <cstdio>
#include "../DbgObject/RegStruct.h"

HANDLE DbgUi::m_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
DbgUi::DbgUi(BreakpointEngine* pBpEngine)
	:m_pBpEngine(pBpEngine)
{

}

DbgUi::~DbgUi()
{

}


void DbgUi::Show(E_ShowType type , const CStringA& pszData)
{
	const char* pStr = pszData;
	switch(type)
	{
		case e_st_DisAsmInfo:
			break;
		case e_st_log:
			printf("%s\n" , pStr);
			break;
		case e_st_memInfo:
			break;
		case e_st_regInfo:
			break;
		default:
			break;
	}

}


void DbgUi::Show(const char* pszStr , int x , int y , int color)
{
	SetConsoleCursorPosition(m_hStdOut , COORD{ x , y });
	SetConsoleTextAttribute(m_hStdOut , color);
	printf("%s" , pszStr);
}

void DbgUi::ShowEx(int x , int y , ...)
{
	va_list argptr;
	va_start(argptr , y);
	int nColor = 1;
	SetConsoleCursorPosition(m_hStdOut , COORD{ x , y });
	while(nColor)
	{
		SetConsoleTextAttribute(m_hStdOut , va_arg(argptr,int));
		printf("%s" , va_arg(argptr , char*));
	}
	va_end(argptr);
}



void DbgUi::showAsm(SIZE_T Addr , 
					const WCHAR* ShowOpc , 
					const WCHAR* pszDiAsm , 
					const WCHAR* pszCom ,
					const WCHAR* pszLineHeader)
{
	//�����ַ�Ƕϵ� ��ʾ�ϵ����ɫ
	if(nullptr != m_pBpEngine->FindBreakpoint(Addr,breakpointType_soft))
	{
		SetConsoleTextAttribute(m_hStdOut , B_H_RED );//��ɫ
		wprintf(L"%s0x%08X" , pszLineHeader,Addr);
	}
	else
	{
		wprintf(L"%s0x%08X" ,pszLineHeader, Addr);
	}

	//���OPCODE
	SetConsoleTextAttribute(m_hStdOut , F_WHITE );//�ָ�Ĭ����ɫ
	wprintf(L" | %24s | " , ShowOpc);

	//wprintf(L"  %08X | %-24s | ", Addr, ShowOpc);
	SIZE_T nLen = wcslen(pszDiAsm);
	//��ʼ������������������
	for(SIZE_T i = 0; i < nLen; i++)
	{
		SetConsoleTextAttribute(m_hStdOut , F_WHITE );		//Ĭ�ϵİ�ɫ
		//��ת������J��
		if(pszDiAsm[ i ] == 'j')
		{
			if(pszDiAsm[ i + 1 ] == 'm'&&pszDiAsm[ i + 2 ] == 'p')
			{
				SetConsoleTextAttribute(m_hStdOut , B_H_GREEN );//��ɫ
				while(pszDiAsm[ i ] != ' '&&pszDiAsm[ i ] != '\t')
				{
					wprintf(L"%c" , pszDiAsm[ i ]);
					i++;
				}
				i--;
				continue;
			}
			else
			{
				SetConsoleTextAttribute(m_hStdOut , B_H_YELLOW );//��ɫ
				while(pszDiAsm[ i ] != ' '&&pszDiAsm[ i ] != '\t')
				{
					wprintf(L"%c" , pszDiAsm[ i ]);
					i++;
				}
				i--;
				continue;
			}
		}
		//CALL����
		if(pszDiAsm[ i ] == 'c'&&pszDiAsm[ i + 1 ] == 'a'&&pszDiAsm[ i + 2 ] == 'l'&&pszDiAsm[ i + 3 ] == 'l')
		{
			SetConsoleTextAttribute(m_hStdOut , B_H_RED );//��ɫ
			while(pszDiAsm[ i ] != ' '&&pszDiAsm[ i ] != '\t')
			{
				wprintf(L"%c" , pszDiAsm[ i ]);
				i++;
			}
			i--;
			continue;
		}
		//RET����
		if((pszDiAsm[ i ] == 'r'&&pszDiAsm[ i + 1 ] == 'e'&&pszDiAsm[ i + 2 ] == 't') ||
		   (pszDiAsm[ i ] == 'i'&&pszDiAsm[ i + 1 ] == 'r'&&pszDiAsm[ i + 2 ] == 'e'))
		{
			SetConsoleTextAttribute(m_hStdOut , F_H_LIGHTBLUE  /* |REVERSE */ );//��ɫ
			while(pszDiAsm[ i ] != ' '&&pszDiAsm[ i ] != '\t')
			{
				wprintf(L"%c" , pszDiAsm[ i ]);
				i++;
			}
			i--;
			continue;
		}
		//PUSH POP����
		if((pszDiAsm[ i ] == 'p'&&pszDiAsm[ i + 1 ] == 'u'&&pszDiAsm[ i + 2 ] == 's') ||
		   (pszDiAsm[ i ] == 'p'&&pszDiAsm[ i + 1 ] == 'o'&&pszDiAsm[ i + 2 ] == 'p'))
		{
			SetConsoleTextAttribute(m_hStdOut , F_H_PURPLE );//��ѡ�񣡣���
			while(pszDiAsm[ i ] != ' '&&pszDiAsm[ i ] != '\t')
			{
				wprintf(L"%c" , pszDiAsm[ i ]);
				i++;
			}
			i--;
			continue;
		}
		//����������
		if((pszDiAsm[ i + 8 ] == 'h') && (pszDiAsm[ i - 1 ] == ' ' || pszDiAsm[ i - 1 ] == ',')
		   && i > 5)
		{
			SetConsoleTextAttribute(m_hStdOut , F_H_YELLOW );//��ɫ
			while(pszDiAsm[ i ] != ' '&&pszDiAsm[ i ] != '\0')
			{
				wprintf(L"%c" , pszDiAsm[ i ]);
				i++;
			}
			i--;
			continue;
		}
		//�ڴ��ַ����
		if((pszDiAsm[ i ] == 'b'&&pszDiAsm[ i + 1 ] == 'y'&&pszDiAsm[ i + 2 ] == 't'
			&&pszDiAsm[ i + 3 ] == 'e') ||
			(pszDiAsm[ i ] == 'w'&&pszDiAsm[ i + 1 ] == 'o'&&pszDiAsm[ i + 2 ] == 'r'
			&&pszDiAsm[ i + 3 ] == 'd') ||
			(pszDiAsm[ i ] == 'd'&&pszDiAsm[ i + 1 ] == 'w'&&pszDiAsm[ i + 2 ] == 'o'
			&&pszDiAsm[ i + 3 ] == 'r'&&pszDiAsm[ i + 4 ] == 'd') ||
			(pszDiAsm[ i ] == 'f'&&pszDiAsm[ i + 1 ] == 'w'&&pszDiAsm[ i + 2 ] == 'o'
			&&pszDiAsm[ i + 3 ] == 'r'&&pszDiAsm[ i + 4 ] == 'd') ||
			(pszDiAsm[ i ] == 'q'&&pszDiAsm[ i + 1 ] == 'w'&&pszDiAsm[ i + 2 ] == 'o'
			&&pszDiAsm[ i + 3 ] == 'r'&&pszDiAsm[ i + 4 ] == 'd'))
		{
			if((pszDiAsm[ i - 2 ] == 'l'&&pszDiAsm[ i - 3 ] == 'l') &&
			   (pszDiAsm[ i - 4 ] == 'a'&&pszDiAsm[ i - 5 ] == 'c'))
			{
				//CALL��ַ����
				SetConsoleTextAttribute(m_hStdOut , F_H_PURPLE );
				while(pszDiAsm[ i ] != ',' && pszDiAsm[ i ] != '\0')
				{
					wprintf(L"%c" , pszDiAsm[ i ]);
					i++;
				}
				i--;
				continue;
			}
			else
			{
				//DS����
				SetConsoleTextAttribute(m_hStdOut , F_H_LIGHTBLUE );
				while(pszDiAsm[ i ] != ',' && pszDiAsm[ i ] != '\0')
				{
					wprintf(L"%c" , pszDiAsm[ i ]);
					i++;
				}
				i--;
				continue;
			}
		}
		//�ڴ��ַ����������
		if((pszDiAsm[ i - 6 ] == 'j' || pszDiAsm[ i - 4 ] == 'j' || pszDiAsm[ i - 3 ] == 'j') ||
		   (pszDiAsm[ i - 5 ] == 'c'&&pszDiAsm[ i - 4 ] == 'a' &&pszDiAsm[ i - 3 ] == 'l' &&pszDiAsm[ i - 2 ] == 'l') ||
		   (pszDiAsm[ i - 5 ] == 'l'&& pszDiAsm[ i - 4 ] == 'o'&&pszDiAsm[ i - 3 ] == 'o'&&pszDiAsm[ i - 1 ] == '\t') ||
		   (pszDiAsm[ i - 6 ] == 'l'&&pszDiAsm[ i - 5 ] == 'o'&&pszDiAsm[ i - 4 ] == 'o'&&pszDiAsm[ i - 1 ] == '\t') ||
		   (pszDiAsm[ i - 7 ] == 'l'&&pszDiAsm[ i - 6 ] == 'o'&&pszDiAsm[ i - 5 ] == 'o'&&pszDiAsm[ i - 1 ] == '\t'))
		{
			SetConsoleTextAttribute(m_hStdOut , F_H_GREEN );//��ɫ
			while(pszDiAsm[ i ] != ',' && pszDiAsm[ i ] != '\0')
			{
				wprintf(L"%c" , pszDiAsm[ i ]);
				i++;
			}
			i--;
			continue;
		}
		SetConsoleTextAttribute(m_hStdOut , F_H_WHITE );
		wprintf(L"%c" , pszDiAsm[ i ]);//���û�и������ַ�
	}

	if(pszCom != NULL)
	{
		SetConsoleTextAttribute(m_hStdOut , F_H_GREEN );//��ɫ
		wprintf(L"%*c %s" , 40-wcslen(pszDiAsm) , '|' , pszCom);
	}
	SetConsoleTextAttribute(m_hStdOut , F_WHITE );//�ָ�Ĭ����ɫ
	wprintf(L"\n");//�������
}




void DbgUi::printReg(SIZE_T dwReg , WORD color)
{
	SetConsoleTextAttribute(m_hStdOut , color);
	printf(" %08X" , dwReg);
	SetConsoleTextAttribute(m_hStdOut , F_WHITE);
	printf(" |");
}

void DbgUi::printEflag(DWORD dwFlag , WORD color)
{
	SetConsoleTextAttribute(m_hStdOut , color);
	printf("%3d " , dwFlag);
	SetConsoleTextAttribute(m_hStdOut , F_WHITE);
	printf("|");
}

void DbgUi::showReg(const CONTEXT& ct)
{


	static	bool bFirst = true;
	static	CONTEXT		oldCt = { 0 };
	if(bFirst)
	{
		bFirst = false;
		oldCt = ct;
	}

	printf("------------------------------------------------------------------\n");
	printf("    eax   |    ecx   |    edx   |    ebx   |    esi   |    edi   |\n");
	// 	printf(" %08X | %08X | %08X | %08X | %08X | %08X |\n" , 
	// 		   ct.Eax , ct.Ecx , ct.Edx , ct.Ebx , ct.Esi , ct.Edi);
	if(oldCt.Eax != ct.Eax)
		printReg(ct.Eax , F_H_RED);
	else
		printReg(ct.Eax , F_WHITE);
	if(oldCt.Ecx != ct.Ecx)
		printReg(ct.Ecx , F_H_RED);
	else
		printReg(ct.Ecx , F_WHITE);

	if(oldCt.Edx != ct.Edx)
		printReg(ct.Edx , F_H_RED);
	else
		printReg(ct.Edx , F_WHITE);

	if(oldCt.Ebx != ct.Ebx)
		printReg(ct.Ebx , F_H_RED);
	else
		printReg(ct.Ebx , F_WHITE);

	if(oldCt.Esi != ct.Esi)
		printReg(ct.Esi , F_H_RED);
	else
		printReg(ct.Esi , F_WHITE);

	if(oldCt.Edi != ct.Edi)
		printReg(ct.Edi , F_H_RED);
	else
		printReg(ct.Edi , F_WHITE);
	printf("\n");





	printf("    esp   |    ebp   ||||| CF | PF | AF | ZF | SF | TF | DF | OF |\n");
	PEFLAGS pEflgas = (PEFLAGS)&ct.EFlags;
	PEFLAGS pOldEflags = (PEFLAGS)&oldCt.EFlags;

	// 	printf(" %08X | %08X |||||%3d |%3d |%3d |%3d |%3d |%3d |%3d |%3d |\n",
	// 		   ct.Esp , ct.Ebp , pEflgas->CF , 
	// 		   pEflgas->PF , pEflgas->AF , pEflgas->ZF , 
	// 		   pEflgas->SF , pEflgas->TF , pEflgas->DF , pEflgas->OF
	// 		   );
	if(oldCt.Esp != ct.Esp)
		printReg(ct.Esp , F_H_RED);
	else
		printReg(ct.Esp , F_WHITE);
	if(oldCt.Ebp != ct.Ebp)
		printReg(ct.Ebp , F_H_RED);
	else
		printReg(ct.Ebp , F_WHITE);

	printf("||||");

	if(pOldEflags->CF != pEflgas->CF)
		printEflag(pEflgas->CF , F_H_RED);
	else
		printEflag(pEflgas->CF , F_WHITE);
	if(pOldEflags->PF != pEflgas->PF)
		printEflag(pEflgas->PF , F_H_RED);
	else
		printEflag(pEflgas->PF , F_WHITE);
	if(pOldEflags->AF != pEflgas->AF)
		printEflag(pEflgas->AF , F_H_RED);
	else
		printEflag(pEflgas->AF , F_WHITE);
	if(pOldEflags->ZF != pEflgas->ZF)
		printEflag(pEflgas->ZF , F_H_RED);
	else
		printEflag(pEflgas->ZF , F_WHITE);
	if(pOldEflags->SF != pEflgas->SF)
		printEflag(pEflgas->SF , F_H_RED);
	else
		printEflag(pEflgas->SF , F_WHITE);
	if(pOldEflags->TF != pEflgas->TF)
		printEflag(pEflgas->TF , F_H_RED);
	else
		printEflag(pEflgas->TF , F_WHITE);
	if(pOldEflags->DF != pEflgas->DF)
		printEflag(pEflgas->DF , F_H_RED);
	else
		printEflag(pEflgas->DF , F_WHITE);
	if(pOldEflags->OF != pEflgas->OF)
		printEflag(pEflgas->OF , F_H_RED);
	else
		printEflag(pEflgas->OF , F_WHITE);
	printf("\n");

	printf("--> eip  %08X\n" , ct.Eip);
	oldCt = ct;
	printf("------------------------------------------------------------------\n");
}


void byteArr2HexStr(const LPBYTE& lpbArr , DWORD dwBSize , char* pszHexStr , const TCHAR wcSpace)
{
	DWORD	i = 0;
	TCHAR	ucNum[ 3 ] = { 0 };
	BYTE	byteNum = 0;
	DWORD	dwIndex = wcSpace == 0 ? 2 : 3;
	DWORD	j = 0;
	while(j < dwBSize)
	{
		byteNum = *((PBYTE)(lpbArr + j));
		// ת���ַ���
		sprintf_s(pszHexStr + i , 3 + 1 , "%02X%c" , byteNum , wcSpace);
		i += dwIndex;
		++j;
	}
}



void DbgUi::showMem(SIZE_T virtualAddress , const LPBYTE lpBuff , int nSize , DWORD dwShowFlag)
{
	printf("--------+--------------------------------------------------+------------------+\n");
	printf("        |  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  |                  |\n");
	printf("--------+--------------------------------------------------+------------------+\n");
	unsigned char ch =0;
	const char *pAnsi = (char*)lpBuff;
	const wchar_t* pWchar = (wchar_t*)lpBuff;
	if(dwShowFlag)
	{
		printf("%08X| ",virtualAddress);
		virtualAddress += 16;
		unsigned short wch = 0;
		for(; nSize > 0; nSize -= 32)
		{
			for(int i = 0 ; i < 16; ++i)
			{
				wch = pWchar[ i ];
				if(dwShowFlag == 0)
					printf("%02X " , wch); 
			}
			printf(" | ");
			for(int i = 0 ; i < 16; ++i)
			{
				if(ch < 33 || ch>126)
					ch = '.';
				wprintf(L"%c" , wch);
			}
			printf(" |\n");

			pWchar += 16;
		}
	}
	else
	{
		for(; nSize > 0; nSize -= 16)
		{
			printf("%08X| ",virtualAddress);
			virtualAddress += 16; 
			for(int i = 0 ; i < 16; ++i)
			{
				ch = pAnsi[ i ];
				printf("%02X " , ch);
			}
			printf(" | ");
			for(int i = 0 ; i < 16;++i)
			{
				printf("%c" , pAnsi[ i ] < 33 || pAnsi[ i ]>126 ? '.' : pAnsi[ i ]);
			}
			printf(" |\n");

			pAnsi += 16;
		}
	}
	printf("--------+--------------------------------------------------+------------------+\n");
}

void DbgUi::showBreakPointList(list<BPObject*>::const_iterator beginItr ,
							   list<BPObject*>::const_iterator endItr)
{
	printf("------+------------+----------------+---------------\n");
	printf(" ��� |    ��ַ    |      ����      |     ����\n");
	printf("------+------------+----------------+---------------\n");
	int j = 0;


	for(;beginItr != endItr ; ++beginItr)
	{
		E_BPType Type = (*beginItr)->Type();
		if(Type == breakpointType_tf)
			continue;

		printf("%5d |" , j++);
		printf(" 0x%08X |" , (*beginItr)->GetAddress());

		switch(Type)
		{
			case breakpointType_hard_r:
				printf("%14s  |" , "Ӳ�����ϵ�");
				break;
			case breakpointType_hard_w:
				printf("%14s  |" , "Ӳ��д�ϵ�");
				break;
			case breakpointType_hard_e:
				printf("%14s  |" , "Ӳ��ִ�жϵ�");
				break;
			case breakpointType_hard_rw:
				printf("%14s  |" , "Ӳ����д�ϵ�");
				break;
			case breakpointType_acc_r:
				printf("%14s  |" , "�ڴ���ϵ�");
				break;
			case breakpointType_acc_w:
				printf("%14s  |" , "�ڴ�д�ϵ�");
				break;
			case breakpointType_acc_e:
				printf("%14s  |" , "�ڴ�ִ�жϵ�");
				break;
			case breakpointType_soft:
				printf("%14s  |" , "����ϵ�");
				break;
			default:
				printf("%14s  |" , "������");
		}
		printf(" %s\n" , (*beginItr)->GetCondition());
	}
	printf("------+------------+----------------+---------------\n");
}

void DbgUi::showStack(SIZE_T virtualAddress , const LPBYTE lpBuff , int nSize)
{
	SIZE_T	*p = (SIZE_T*)lpBuff;
	CString	buff;
	while(nSize >= 0)
	{
		printf(" %08X | %08X\n" , virtualAddress , *p);
		++p;
		virtualAddress += sizeof(SIZE_T);
		nSize -= sizeof(SIZE_T);
	}

}