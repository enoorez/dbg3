/**
  * 利用DbgEngine类的Open()打开一个进程进行调试.
  * 利用DbgEngine类的Exec()接收被调试进程的调试事件
  * 利用DbgEngine类的AddBreakpoint()设置断点
  * 利用DbgEngine类的pfnBreakpointProc函数指针去处理断点事件
  * 利用DbgEngine类的GetRegInfo()获取寄存器信息
  * 利用DbgEngine类的SetRegInfo()设置寄存器信息
  * 利用DbgEngine类的ReadMomory()获取指定地址的内存
  * 利用DbgEngine类的WriteMomory()设置指定地址的内存
  * 利用DisAssambly的DiAsm()来将一段opcode转换成汇编代码
  * 利用XEDParse的XEDParseAssemble()来将一段汇编代码转换成opcode
  */

#include "DbgEngine/DbgEngine.h" // 调试引擎
#include "dbgUi/dbgUi.h" // 用户界面
#include "Expression/Expression.h" // 表达式模块
#include "DisAssambly/DiAsmEngine.h" // 反汇编引擎
#include "AssamblyEngine/XEDParse.h" // 汇编引擎
#pragma comment(lib,"AssamblyEngine\\XEDParse.lib")

#include <iostream>
using namespace std;

// 显示调试器命令行帮助信息
void showHelp(); 
// 将字符串分割成两个字符串(将第一次遇到的空格替换成字符串结束符)
char* GetSecondArg(char* pBuff);
inline char* SkipSpace(char* pBuff);

// 调试器引擎的断点处理回调函数
// 断点被命中时,调试器引擎会调用此函数
unsigned int __stdcall DebugEvent(void* uParam);

int main()
{
	// 设置代码页,以支持中文
	setlocale(LC_ALL , "chs");
	cout << "\t\t---------< Dbg >---------\n";
	cout << "\t\t---< 按h查看完整命令 >---\n";



	DbgEngine		dbgEng; // 调试器引擎对象

	// 注册断点处理的回调函数
	dbgEng.m_pfnBreakpointProc = (fnExceptionProc)DebugEvent;
	
	char szPath[ MAX_PATH ];
	bool bCreateThread = false;

	while(true)
	{
		while(true)
		{
			cout << "输入路径以打开调试进程:";
			cin.getline(szPath , MAX_PATH);

			// 打开调试进程
			if(dbgEng.Open(szPath))
				break;

			cout << "程序打开失败\n";
		}
		cout << "调试进程创建成功, 可以进行调试\n";

		while(1)
		{
			// 运行调试器,Exec不处于阻塞状态,因此需要放在while循环中.
			if(e_s_processQuit == dbgEng.Exec())
			{
				dbgEng.Close();
				system("cls");
				cout << "进程已退出\n";
				break;
			}
		}
	}
}


// 显示反汇编
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



// 处理断点的回调函数
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

	// 清屏
	system("cls");
	// 获取寄存器信息
	pDbg->GetRegInfo(ct);
	// 使用ui模块将寄存器信息输出
	ui.showReg(ct);

	// 输出反汇编
	ShowAsm(*pDbg , ui , disAsm , 20 , ct.Eip);

	while(1)
	{
		do
		{
			cout <<  "> ";
			// 接收用户输入的命令
			gets_s(szCmdLine , 64);
		} while(*szCmdLine == '\0');

		// 跳过行头空格
		pCmdLine = SkipSpace(szCmdLine);

		// 判断是否需要退出调试器
		if(*(DWORD*)pCmdLine == 'tixe')
			exit(0);


		// 解析用户输入的命令
		switch(*pCmdLine)
		{
			/*查看反汇编*/
			case 'u': // 查看反汇编
			{
				// 格式: u 地址(可选) 行数(可选)
				SIZE_T  uAddr = 0;
				DWORD	dwLineCount = 0;

				// 判断是否是只有u没有其他参数
				if(pCmdLine[ 1 ] == 0 || pCmdLine[ 2 ] == 0) // if(strlen(szCmdLine) < 2)
				{
					// 设置默认的反汇编地址和反汇编指令数量
					pDbg->GetRegInfo(ct);
					uAddr = ct.Eip;
					dwLineCount = 10;
				}
				else // 至少包含一个参数时
				{
					char* pAddress = SkipSpace(pCmdLine + 1);
					char* pLineCount = GetSecondArg(pAddress);
					// 使用表达式模块获取值以获取反汇编地址
					uAddr = exp.getValue(pAddress);
					// 获取反汇编指令数量
					dwLineCount = exp.getValue(pLineCount);
					// 如果没有行数,则默认显示10行
					dwLineCount = dwLineCount == 0 ? 10 : dwLineCount;
				}
				// 显示反汇编
				ShowAsm(*pDbg , ui , disAsm , dwLineCount , uAddr);
				break;
			}
			/*输入汇编*/
			case 'a': /*汇编*/
			{
				// 获取开始地址
				XEDPARSE xpre = { 0 };
				xpre.x64 = false; // 是否转换成64位的opCode

				pCmdLine = SkipSpace(pCmdLine + 1);
				if(*pCmdLine == 0)
				{
					printf("指令格式错误, 格式为: a 地址\n");
					continue;// 结束本次while循环
				}
				uaddr address = exp.getValue(pCmdLine);
				if(address == 0)
					continue;// 结束本次while循环

				while(true)
				{
					printf("%08X: ",address);
					cin.getline(xpre.instr , XEDPARSE_MAXBUFSIZE);
					if(strcmp(xpre.instr , "quit") == 0)
						break;
					
					xpre.cip = address;// 指令所在的地址
					if(false == XEDParseAssemble(&xpre))
					{
						printf("%s\n" , xpre.error);
						continue;// 结束本次while循环
					}
					// 将代码写入到目标进程
					if(!pDbg->WriteMemory(address , xpre.dest , xpre.dest_size))
						continue;// 结束本次while循环
					// 地址++
					address += xpre.dest_size;
				}
				break;
			}
			/*查看栈*/
			case 'k':
			{
				pDbg->GetRegInfo(ct);
				BYTE	buff[ sizeof(SIZE_T) * 20 ];
				pDbg->ReadMemory(ct.Esp , buff , sizeof(SIZE_T) * 20);
				ui.showStack(ct.Esp , buff , sizeof(SIZE_T) * 20);
				break;
			}
			/*查看和修改寄存器*/
			case 'r':/*寄存器读写*/
			{
				// 获取寄存器的值:
				// r 寄存器名 
				// 设置寄存器的值
				// r 寄存器名 = 表达式
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
			/*查看内存*/
			case 'd':/*查看数据*/
			{
				char *p = &szCmdLine[ 1 ];
				// 筛选数据格式
				switch(*p)
				{
					case 'u':/*unicode字符串*/
					{
						SSIZE_T uAddr = exp.getValue(szCmdLine + 2);
						BYTE lpBuff[ 16 * 6 ];
						pDbg->ReadMemory(uAddr , lpBuff , 16 * 6);
						ui.showMem(uAddr , lpBuff , 16 * 6 , 1);
					}
					break;
					case 'a':/*ansi字符串*/
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
			/*单步步入*/
			case 't': // 步入
			{
				// 使用调试器引擎的函数来添加一个TF断点
				BPObject *pBp = pDbg->AddBreakPoint(0 , e_bt_tf);
				if(pBp == nullptr)
					return 0;

				// 获取条件
				char* pCondition = SkipSpace(pCmdLine + 1);

				// 设置断点的中断条件
				if(*pCondition != 0)
				{
					pBp->SetCondition(pCondition);
				}
				else
					pBp->SetCondition(true);
				return 0;
			}
			/*单步步过*/
			case 'p': // 步过
			{
				BPObject *pBp = nullptr;
				// 判断当前是否是call指令
				pDbg->GetRegInfo(ct);
				SIZE_T uEip = ct.Eip;
				BYTE c[ 2 ] = { 0 };
				pDbg->ReadMemory(uEip , c , 2);
				DWORD dwCodeLen = 5;
				/**
				* call 的机器码有:
				* 0xe8 : 5byte,
				* 0x9a : 7byte,
				* 0xff :
				*	 0x10ff ~ 0x1dff
				* rep 前缀的指令也可以步过
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
				// 获取条件
				char* pCondition = SkipSpace(pCmdLine + 1);
				if(*pCondition != 0)
				{
					pBp->SetCondition(pCondition);
				}
				else
					pBp->SetCondition(true);

				return 0;
			}
			/*查看加载的模块*/
			case 'm':
			{
				if(*SkipSpace(pCmdLine + 1) == 'l')
				{
					list<MODULEFULLINFO> modList;
					pDbg->GetModuleList(modList);
					printf("+------------------+----------+----------------------------------------------------+\n");
					printf("|     加载基址     + 模块大小 |                    模块名                          |\n");
					printf("+------------------+----------+----------------------------------------------------+\n");
					for(auto &i : modList)
					{
						printf("| %016I64X | %08X | %-50s |\n" , i.uStart , i.uSize , (LPCSTR)i.name);
					}
					printf("+------------------+----------+----------------------------------------------------+\n");
				}
				continue;// 结束本次while循环
			}
			/*设置断点*/
			case 'b':/*下断*/
			{
				char* p = SkipSpace(szCmdLine + 1);
				E_BPType bpType = e_bt_none;
				SIZE_T uAddr = 0; // 下断地址
				char*  pRule = 0; // 断点命中规则
				char   cType = *p;// 断点类型
				uint   uBPLen = 0;
				switch(cType)
				{
					case 'l':/*断点列表*/
					{
						ui.showBreakPointList(pDbg->GetBPListBegin() , pDbg->GetBPListEnd());
						continue;// 结束本次while循环
					}
					case 'c':/*删除断点*/
					{
						DWORD	dwIndex = 0;
						sscanf_s(szCmdLine + 2 , "%d" , &dwIndex);
						pDbg->DeleteBreakpoint(dwIndex);
						continue;// 结束本次while循环
					}
					case 'n':/*函数名断点*/
					{
						if(nullptr == pDbg->AddBreakPoint(SkipSpace(pCmdLine + 2)))
							cout << "找不到符号\n";
						cout << "设置成功!\n";
						continue;// 结束本次while循环
					}
					case 'h':/*硬件断点*/
					case 'm': /*内存访问断点*/
					{
						uBPLen = 1;
						// 得到地址
						for(p = p + 1; *p == ' '; ++p);
						char *pType = 0;

						// 得到断点类型
						pType = GetSecondArg(p);

						// 通过表达式类的处理来获取地址
						uAddr = exp.getValue(p);

						// 判断有没有指定断点的类型:
						if(*pType == 0)
						{
							printf("bm/bh 地址 类型(r/w/e) 条件(可选)\n");
							continue;// 结束本次while循环
						}
						switch(*pType) // 筛选断点的类型
						{
							case 'r':bpType = cType == 'm' ? e_bt_acc_r : e_bt_hard_r; break;
							case 'w':bpType = cType == 'm' ? e_bt_acc_w: e_bt_hard_w; break;
							case 'e':bpType = cType == 'm' ? e_bt_acc_e: e_bt_hard_e; break;
							default:
								printf("访问断点的类型有: r(读),w(写),e(执行)\n");
								continue;// 结束本次while循环
						}
						// 获取断点数据的长度
						char* pLen = GetSecondArg(pType);
						uBPLen = exp.getValue(pLen);

						// 定位到条件表达式(第一个不是空格的地方)
						pRule = SkipSpace(pLen + 1);
						break;
					}
					case 'p':/*普通断点*/
					{
 						bpType = e_bt_soft;
						// 地址 条件						
						p = SkipSpace(p + 1);
						pRule = GetSecondArg(p);

						// 得到地址
						uAddr = exp.getValue(p);
						// 得到条件
						pRule = SkipSpace(pRule);
						break;
					}
					default: 
						cout << "没有该类型的断点\n";
						continue;// 结束本次while循环
					
				}

				// 筛选完断点后, 进行下断.
				BPObject* pBp = pDbg->AddBreakPoint(uAddr , bpType , uBPLen);
				if(pBp == nullptr)
				{
					printf("设置断点失败\n");
					continue;// 结束本次while循环
				}
				// 如果断点携带表达式, 则把表达式设置到断点上
				if(*pRule != 0)
					BreakpointEngine::SetExp(pBp , pRule);
			}
			break;
			/*运行程序*/
			case 'g':
				return 0;
			/*查看帮助*/
			case 'h':
				showHelp();
				continue;// 结束本次while循环
			default:
				continue;// 结束本次while循环
		}
	}
	

	return 0;
}


// 显示帮助信息
void showHelp()
{
	printf("----------------------------------------------------\n");
	printf("h : 查看帮助\n");
	//printf("o : 打开调试进程\n");
	//printf("    格式为: o 可执行程序路径\n");
	printf("exit: 退出调试器\n");
	printf("ml: 显示模块列表\n");
	printf("g : 运行程序\n");
	printf("p : 单步\n");
	printf("t : 步过\n");
	printf("a : 进入汇编模式\n");
	printf("    格式为 : a 开始地址\n");
	printf("    输入quit结束汇编模式\n");
	printf("u : 查看反汇编\n");
	printf("    格式为 : u 开始地址(表达式) 指令条数\n");
	printf("    例如   : u eip\n");
	printf("    例如   : u eax 100\n");
	printf("    例如   : u 0x401000 100\n");
	printf("d : 查看内存数据\n");
	printf("    格式为 : d 开始地址\n");
	printf("    格式为 : da 开始地址(显示字符串时使用ANSIII字符)\n");
	printf("    格式为 : du 开始地址(显示字符串时使用Unicode字符)\n");
	printf("r : 查看/设置寄存器\n");
	printf("    格式为 : r 寄存器名\n");
	printf("    r eax = 0x1000\n");
	printf("b : 设置断点\n");
	printf("    格式:\n");
	printf("    bp 地址 条件表达式 => 软件断点\n");
	printf("    例如: bp 0x401000 eax==0 && byte[0x403000]==97\n");
	printf("    bh 地址 断点属性 条件表达式 => 硬件断点\n");
	printf("    例如: bh 0x401000 e \n");
	printf("    bm 地址 断点属性 条件表达式 => 内存断点\n");
	printf("    例如: bm 0x401000 e \n");
	printf("    bl 列出所有断点\n");
	printf("    bc 序号 删除指定序号的断点\n");
	printf("k : 查看栈\n");
	printf("----------------------------------------------------\n");

}

// 获取第二个参数(参数之间以空格间隔开
char* GetSecondArg(char* pBuff)
{
	for(; *pBuff != 0; ++pBuff)
	{
		if(*pBuff == ' ')//找到第一个空格
		{
			*pBuff = 0; // 把空格变成字符串结束符,分隔两个参数
			return pBuff + 1;//返回第二个参数的开始地址
		}
	}
	return pBuff;
}

// 跳过空格(包括换行符,tab符)
inline char* SkipSpace(char* pBuff)
{
	for(; *pBuff == ' ' || *pBuff == '\t' || *pBuff == '\r' || *pBuff == '\n' ; ++pBuff);
	return pBuff;
}
