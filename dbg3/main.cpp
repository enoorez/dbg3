/**
  * ����DbgEngine���Open()��һ�����̽��е���.
  * ����DbgEngine���Exec()���ձ����Խ��̵ĵ����¼�
  * ����DbgEngine���AddBreakpoint()���öϵ�
  * ����DbgEngine���pfnBreakpointProc����ָ��ȥ����ϵ��¼�
  * ����DbgEngine���GetRegInfo()��ȡ�Ĵ�����Ϣ
  * ����DbgEngine���SetRegInfo()���üĴ�����Ϣ
  * ����DbgEngine���ReadMomory()��ȡָ����ַ���ڴ�
  * ����DbgEngine���WriteMomory()����ָ����ַ���ڴ�
  * ����DisAssambly��DiAsm()����һ��opcodeת���ɻ�����
  * ����XEDParse��XEDParseAssemble()����һ�λ�����ת����opcode
  */

#include "DbgEngine/DbgEngine.h" // ��������
#include "dbgUi/dbgUi.h" // �û�����
#include "Expression/Expression.h" // ���ʽģ��
#include "DisAssambly/DiAsmEngine.h" // ���������
#include "AssamblyEngine/XEDParse.h" // �������
#pragma comment(lib,"AssamblyEngine\\XEDParse.lib")

#include <iostream>
using namespace std;

// ��ʾ�����������а�����Ϣ
void showHelp(); 
// ���ַ����ָ�������ַ���(����һ�������Ŀո��滻���ַ���������)
char* GetSecondArg(char* pBuff);
inline char* SkipSpace(char* pBuff);

// ����������Ķϵ㴦��ص�����
// �ϵ㱻����ʱ,�������������ô˺���
unsigned int __stdcall DebugEvent(void* uParam);

int main()
{
	// ���ô���ҳ,��֧������
	setlocale(LC_ALL , "chs");
	cout << "\t\t---------< Dbg >---------\n";
	cout << "\t\t---< ��h�鿴�������� >---\n";



	DbgEngine		dbgEng; // �������������

	// ע��ϵ㴦��Ļص�����
	dbgEng.m_pfnBreakpointProc = (fnExceptionProc)DebugEvent;
	
	char szPath[ MAX_PATH ];
	bool bCreateThread = false;

	while(true)
	{
		while(true)
		{
			cout << "����·���Դ򿪵��Խ���:";
			cin.getline(szPath , MAX_PATH);

			// �򿪵��Խ���
			if(dbgEng.Open(szPath))
				break;

			cout << "�����ʧ��\n";
		}
		cout << "���Խ��̴����ɹ�, ���Խ��е���\n";

		while(1)
		{
			// ���е�����,Exec����������״̬,�����Ҫ����whileѭ����.
			if(e_s_processQuit == dbgEng.Exec())
			{
				dbgEng.Close();
				system("cls");
				cout << "�������˳�\n";
				break;
			}
		}
	}
}


// ��ʾ�����
void ShowAsm(DbgEngine& dbgEngine,
			 DbgUi& ui , 
			 DisAsmEngine& disAsmEng,
			 int nLine = 30,
			 SIZE_T Addr=0)
{
	static CONTEXT ct = { CONTEXT_CONTROL };
	if(Addr == 0)
	{
		dbgEngine.GetRegInfo(ct);
		Addr= ct.Eip;
	}

	vector<DISASMSTRUST> vecDisAsm;
	disAsmEng.diAsm(Addr , vecDisAsm , nLine);

	for(vector<DISASMSTRUST>::iterator i = vecDisAsm.begin();
		i != vecDisAsm.end();
		++i)
	{
		ui.showAsm(Addr ,
				   (*i).strOpCode ,
				   (*i).strAsm ,
				   (*i).strCom
				   );
		Addr += (*i).dwCodeLen;
	}
}



// ����ϵ�Ļص�����
unsigned int __stdcall DebugEvent(void* uParam)
{
	DbgEngine* pDbg = (DbgEngine*)uParam;
	DbgUi ui(pDbg);
	Expression exp(pDbg);
	DisAsmEngine disAsm(pDbg);

	char szCmdLine[ 64 ] ;
	CONTEXT ct = { CONTEXT_ALL };
	vector<DISASMSTRUST> vecDisAsm;
	char* pCmdLine = 0;

	// ����
	system("cls");
	// ��ȡ�Ĵ�����Ϣ
	pDbg->GetRegInfo(ct);
	// ʹ��uiģ�齫�Ĵ�����Ϣ���
	ui.showReg(ct);

	// ��������
	ShowAsm(*pDbg , ui , disAsm , 20 , ct.Eip);

	while(1)
	{
		do
		{
			cout <<  "> ";
			// �����û����������
			gets_s(szCmdLine , 64);
		} while(*szCmdLine == '\0');

		// ������ͷ�ո�
		pCmdLine = SkipSpace(szCmdLine);

		// �ж��Ƿ���Ҫ�˳�������
		if(*(DWORD*)pCmdLine == 'tixe')
			exit(0);


		// �����û����������
		switch(*pCmdLine)
		{
			/*�鿴�����*/
			case 'u': // �鿴�����
			{
				// ��ʽ: u ��ַ(��ѡ) ����(��ѡ)
				SIZE_T  uAddr = 0;
				DWORD	dwLineCount = 0;

				// �ж��Ƿ���ֻ��uû����������
				if(pCmdLine[ 1 ] == 0 || pCmdLine[ 2 ] == 0) // if(strlen(szCmdLine) < 2)
				{
					// ����Ĭ�ϵķ�����ַ�ͷ����ָ������
					pDbg->GetRegInfo(ct);
					uAddr = ct.Eip;
					dwLineCount = 10;
				}
				else // ���ٰ���һ������ʱ
				{
					char* pAddress = SkipSpace(pCmdLine + 1);
					char* pLineCount = GetSecondArg(pAddress);
					// ʹ�ñ��ʽģ���ȡֵ�Ի�ȡ������ַ
					uAddr = exp.getValue(pAddress);
					// ��ȡ�����ָ������
					dwLineCount = exp.getValue(pLineCount);
					// ���û������,��Ĭ����ʾ10��
					dwLineCount = dwLineCount == 0 ? 10 : dwLineCount;
				}
				// ��ʾ�����
				ShowAsm(*pDbg , ui , disAsm , dwLineCount , uAddr);
				break;
			}
			/*������*/
			case 'a': /*���*/
			{
				// ��ȡ��ʼ��ַ
				XEDPARSE xpre = { 0 };
				xpre.x64 = false; // �Ƿ�ת����64λ��opCode

				pCmdLine = SkipSpace(pCmdLine + 1);
				if(*pCmdLine == 0)
				{
					printf("ָ���ʽ����, ��ʽΪ: a ��ַ\n");
					continue;// ��������whileѭ��
				}
				uaddr address = exp.getValue(pCmdLine);
				if(address == 0)
					continue;// ��������whileѭ��

				while(true)
				{
					printf("%08X: ",address);
					cin.getline(xpre.instr , XEDPARSE_MAXBUFSIZE);
					if(strcmp(xpre.instr , "quit") == 0)
						break;
					
					xpre.cip = address;// ָ�����ڵĵ�ַ
					if(false == XEDParseAssemble(&xpre))
					{
						printf("%s\n" , xpre.error);
						continue;// ��������whileѭ��
					}
					// ������д�뵽Ŀ�����
					if(!pDbg->WriteMemory(address , xpre.dest , xpre.dest_size))
						continue;// ��������whileѭ��
					// ��ַ++
					address += xpre.dest_size;
				}
				break;
			}
			/*�鿴ջ*/
			case 'k':
			{
				pDbg->GetRegInfo(ct);
				BYTE	buff[ sizeof(SIZE_T) * 20 ];
				pDbg->ReadMemory(ct.Esp , buff , sizeof(SIZE_T) * 20);
				ui.showStack(ct.Esp , buff , sizeof(SIZE_T) * 20);
				break;
			}
			/*�鿴���޸ļĴ���*/
			case 'r':/*�Ĵ�����д*/
			{
				// ��ȡ�Ĵ�����ֵ:
				// r �Ĵ����� 
				// ���üĴ�����ֵ
				// r �Ĵ����� = ���ʽ
				char* p = szCmdLine + 1;
				p = SkipSpace(p);
				if(*p == 0)
				{
					ct.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
					pDbg->GetRegInfo(ct);
					ui.showReg(ct);
					break;
				}
				SSIZE_T nValue = exp.getValue(szCmdLine + 1);

				char* pSecArg = SkipSpace(szCmdLine + 1);
				pSecArg = GetSecondArg(pSecArg);
				if(*pSecArg == 0)
					printf("%s = 0x%X\n" , szCmdLine + 1 , nValue);
				break;
			}
			/*�鿴�ڴ�*/
			case 'd':/*�鿴����*/
			{
				char *p = &szCmdLine[ 1 ];
				// ɸѡ���ݸ�ʽ
				switch(*p)
				{
					case 'u':/*unicode�ַ���*/
					{
						SSIZE_T uAddr = exp.getValue(szCmdLine + 2);
						BYTE lpBuff[ 16 * 6 ];
						pDbg->ReadMemory(uAddr , lpBuff , 16 * 6);
						ui.showMem(uAddr , lpBuff , 16 * 6 , 1);
					}
					break;
					case 'a':/*ansi�ַ���*/
						p = &szCmdLine[ 2 ];
					default:
					{
						SSIZE_T uAddr = exp.getValue(p);
						BYTE lpBuff[ 16 * 6 ];
						pDbg->ReadMemory(uAddr , lpBuff , 16 * 6);
						ui.showMem(uAddr , lpBuff , 16 * 6 , 0);
					}
					break;
				}
				break;
			}
			/*��������*/
			case 't': // ����
			{
				// ʹ�õ���������ĺ��������һ��TF�ϵ�
				BPObject *pBp = pDbg->AddBreakPoint(0 , e_bt_tf);
				if(pBp == nullptr)
					return 0;

				// ��ȡ����
				char* pCondition = SkipSpace(pCmdLine + 1);

				// ���öϵ���ж�����
				if(*pCondition != 0)
				{
					pBp->SetCondition(pCondition);
				}
				else
					pBp->SetCondition(true);
				return 0;
			}
			/*��������*/
			case 'p': // ����
			{
				BPObject *pBp = nullptr;
				// �жϵ�ǰ�Ƿ���callָ��
				pDbg->GetRegInfo(ct);
				SIZE_T uEip = ct.Eip;
				BYTE c[ 2 ] = { 0 };
				pDbg->ReadMemory(uEip , c , 2);
				DWORD dwCodeLen = 5;
				/**
				* call �Ļ�������:
				* 0xe8 : 5byte,
				* 0x9a : 7byte,
				* 0xff :
				*	 0x10ff ~ 0x1dff
				* rep ǰ׺��ָ��Ҳ���Բ���
				*/
				if(c[ 0 ] == 0xe8/*call*/
				   || c[ 0 ] == 0xf3/*rep*/
				   || c[ 0 ] == 0x9a/*call*/
				   || (c[ 0 ] == 0xff && 0x10 <= c[ 1 ] && c[ 1 ] <= 0x1d)/*call*/
				   )
				{
					dwCodeLen = disAsm.getCoodeLen(uEip);
					pBp = pDbg->AddBreakPoint(uEip + dwCodeLen , e_bt_soft);
				}
				else
					pBp=pDbg->AddBreakPoint(0 , e_bt_tf);

				if(pBp == nullptr)
					return 0;
				// ��ȡ����
				char* pCondition = SkipSpace(pCmdLine + 1);
				if(*pCondition != 0)
				{
					pBp->SetCondition(pCondition);
				}
				else
					pBp->SetCondition(true);

				return 0;
			}
			/*�鿴���ص�ģ��*/
			case 'm':
			{
				if(*SkipSpace(pCmdLine + 1) == 'l')
				{
					list<MODULEFULLINFO> modList;
					pDbg->GetModuleList(modList);
					printf("+------------------+----------+----------------------------------------------------+\n");
					printf("|     ���ػ�ַ     + ģ���С |                    ģ����                          |\n");
					printf("+------------------+----------+----------------------------------------------------+\n");
					for(auto &i : modList)
					{
						printf("| %016I64X | %08X | %-50s |\n" , i.uStart , i.uSize , (LPCSTR)i.name);
					}
					printf("+------------------+----------+----------------------------------------------------+\n");
				}
				continue;// ��������whileѭ��
			}
			/*���öϵ�*/
			case 'b':/*�¶�*/
			{
				char* p = SkipSpace(szCmdLine + 1);
				E_BPType bpType = e_bt_none;
				SIZE_T uAddr = 0; // �¶ϵ�ַ
				char*  pRule = 0; // �ϵ����й���
				char   cType = *p;// �ϵ�����
				uint   uBPLen = 0;
				switch(cType)
				{
					case 'l':/*�ϵ��б�*/
					{
						ui.showBreakPointList(pDbg->GetBPListBegin() , pDbg->GetBPListEnd());
						continue;// ��������whileѭ��
					}
					case 'c':/*ɾ���ϵ�*/
					{
						DWORD	dwIndex = 0;
						sscanf_s(szCmdLine + 2 , "%d" , &dwIndex);
						pDbg->DeleteBreakpoint(dwIndex);
						continue;// ��������whileѭ��
					}
					case 'n':/*�������ϵ�*/
					{
						if(nullptr == pDbg->AddBreakPoint(SkipSpace(pCmdLine + 2)))
							cout << "�Ҳ�������\n";
						cout << "���óɹ�!\n";
						continue;// ��������whileѭ��
					}
					case 'h':/*Ӳ���ϵ�*/
					case 'm': /*�ڴ���ʶϵ�*/
					{
						uBPLen = 1;
						// �õ���ַ
						for(p = p + 1; *p == ' '; ++p);
						char *pType = 0;

						// �õ��ϵ�����
						pType = GetSecondArg(p);

						// ͨ�����ʽ��Ĵ�������ȡ��ַ
						uAddr = exp.getValue(p);

						// �ж���û��ָ���ϵ������:
						if(*pType == 0)
						{
							printf("bm/bh ��ַ ����(r/w/e) ����(��ѡ)\n");
							continue;// ��������whileѭ��
						}
						switch(*pType) // ɸѡ�ϵ������
						{
							case 'r':bpType = cType == 'm' ? e_bt_acc_r : e_bt_hard_r; break;
							case 'w':bpType = cType == 'm' ? e_bt_acc_w: e_bt_hard_w; break;
							case 'e':bpType = cType == 'm' ? e_bt_acc_e: e_bt_hard_e; break;
							default:
								printf("���ʶϵ��������: r(��),w(д),e(ִ��)\n");
								continue;// ��������whileѭ��
						}
						// ��ȡ�ϵ����ݵĳ���
						char* pLen = GetSecondArg(pType);
						uBPLen = exp.getValue(pLen);

						// ��λ���������ʽ(��һ�����ǿո�ĵط�)
						pRule = SkipSpace(pLen + 1);
						break;
					}
					case 'p':/*��ͨ�ϵ�*/
					{
 						bpType = e_bt_soft;
						// ��ַ ����						
						p = SkipSpace(p + 1);
						pRule = GetSecondArg(p);

						// �õ���ַ
						uAddr = exp.getValue(p);
						// �õ�����
						pRule = SkipSpace(pRule);
						break;
					}
					default: 
						cout << "û�и����͵Ķϵ�\n";
						continue;// ��������whileѭ��
					
				}

				// ɸѡ��ϵ��, �����¶�.
				BPObject* pBp = pDbg->AddBreakPoint(uAddr , bpType , uBPLen);
				if(pBp == nullptr)
				{
					printf("���öϵ�ʧ��\n");
					continue;// ��������whileѭ��
				}
				// ����ϵ�Я�����ʽ, ��ѱ��ʽ���õ��ϵ���
				if(*pRule != 0)
					BreakpointEngine::SetExp(pBp , pRule);
			}
			break;
			/*���г���*/
			case 'g':
				return 0;
			/*�鿴����*/
			case 'h':
				showHelp();
				continue;// ��������whileѭ��
			default:
				continue;// ��������whileѭ��
		}
	}
	

	return 0;
}


// ��ʾ������Ϣ
void showHelp()
{
	printf("----------------------------------------------------\n");
	printf("h : �鿴����\n");
	//printf("o : �򿪵��Խ���\n");
	//printf("    ��ʽΪ: o ��ִ�г���·��\n");
	printf("exit: �˳�������\n");
	printf("ml: ��ʾģ���б�\n");
	printf("g : ���г���\n");
	printf("p : ����\n");
	printf("t : ����\n");
	printf("a : ������ģʽ\n");
	printf("    ��ʽΪ : a ��ʼ��ַ\n");
	printf("    ����quit�������ģʽ\n");
	printf("u : �鿴�����\n");
	printf("    ��ʽΪ : u ��ʼ��ַ(���ʽ) ָ������\n");
	printf("    ����   : u eip\n");
	printf("    ����   : u eax 100\n");
	printf("    ����   : u 0x401000 100\n");
	printf("d : �鿴�ڴ�����\n");
	printf("    ��ʽΪ : d ��ʼ��ַ\n");
	printf("    ��ʽΪ : da ��ʼ��ַ(��ʾ�ַ���ʱʹ��ANSIII�ַ�)\n");
	printf("    ��ʽΪ : du ��ʼ��ַ(��ʾ�ַ���ʱʹ��Unicode�ַ�)\n");
	printf("r : �鿴/���üĴ���\n");
	printf("    ��ʽΪ : r �Ĵ�����\n");
	printf("    r eax = 0x1000\n");
	printf("b : ���öϵ�\n");
	printf("    ��ʽ:\n");
	printf("    bp ��ַ �������ʽ => ����ϵ�\n");
	printf("    ����: bp 0x401000 eax==0 && byte[0x403000]==97\n");
	printf("    bh ��ַ �ϵ����� �������ʽ => Ӳ���ϵ�\n");
	printf("    ����: bh 0x401000 e \n");
	printf("    bm ��ַ �ϵ����� �������ʽ => �ڴ�ϵ�\n");
	printf("    ����: bm 0x401000 e \n");
	printf("    bl �г����жϵ�\n");
	printf("    bc ��� ɾ��ָ����ŵĶϵ�\n");
	printf("k : �鿴ջ\n");
	printf("----------------------------------------------------\n");

}

// ��ȡ�ڶ�������(����֮���Կո�����
char* GetSecondArg(char* pBuff)
{
	for(; *pBuff != 0; ++pBuff)
	{
		if(*pBuff == ' ')//�ҵ���һ���ո�
		{
			*pBuff = 0; // �ѿո����ַ���������,�ָ���������
			return pBuff + 1;//���صڶ��������Ŀ�ʼ��ַ
		}
	}
	return pBuff;
}

// �����ո�(�������з�,tab��)
inline char* SkipSpace(char* pBuff)
{
	for(; *pBuff == ' ' || *pBuff == '\t' || *pBuff == '\r' || *pBuff == '\n' ; ++pBuff);
	return pBuff;
}
