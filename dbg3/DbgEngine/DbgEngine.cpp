#include "DbgEngine.h"


DbgEngine::DbgEngine()
{
	m_hBreakpointEvent = CreateEvent(NULL , FALSE , FALSE , NULL);
	m_hUserIputEvent = CreateEvent(NULL , FALSE,FALSE , NULL);
}

DbgEngine::~DbgEngine()
{
}

E_Status DbgEngine::Exec()
{
	static bool bIsSystemBreakpoint = true;
	DEBUG_EVENT dbgEvent = { 0 };
	DWORD dwStatus = 0;
	DWORD bRet = 0;

	// 等待调试事件
	bRet = WaitForDebugEvent(&dbgEvent , 30);
	if(bRet == FALSE)
		return e_s_sucess;

	m_pid = dbgEvent.dwProcessId;
	m_tid = dbgEvent.dwThreadId;

	// 关闭旧的句柄
	CloseHandle(m_hCurrProcess);
	CloseHandle(m_hCurrThread);
	m_hCurrThread = m_hCurrProcess = 0;
	// 获取本轮异常使用到的句柄
	m_hCurrProcess = OpenProcess(PROCESS_ALL_ACCESS,/*所有权限*/
								 FALSE ,
								 m_pid
								 );
	m_hCurrThread = OpenThread(THREAD_ALL_ACCESS,
							   FALSE ,
							   m_tid
							   );

	dwStatus = DBG_CONTINUE;

	// 分析调试事件
	switch(dbgEvent.dwDebugEventCode)
	{
		case EXCEPTION_DEBUG_EVENT: /*异常调试事件*/
		{
			if(bIsSystemBreakpoint)
			{
				// 初始化符号服务器
				BreakpointEngine::InitSymbol(m_hCurrProcess);
				bIsSystemBreakpoint = false;

				// 在OEP处下断
				BPObject* pBp = AddBreakPoint(m_oep , breakpointType_soft);
				if(pBp == nullptr)
				{
					TerminateProcess(m_hCurrProcess , 0);
					return e_s_processQuit;
				}
				pBp->SetCondition(true);

				if(m_bStopOnSystemBreakpoint)
				{
					//调用处理断点的回调函数
					SetEvent(m_hBreakpointEvent);
					// 等待用户输入完成的信号
					WaitForSingleObject(m_hUserIputEvent , -1);
					dwStatus = DBG_CONTINUE;
				}
				goto _SUCESS;
			}

			// 重新安装已失效的断点
			ReInstallBreakpoint();

			// 根据异常信息查找断点
			
			BpItr itr = FindBreakpoint(dbgEvent.u.Exception);
			// 判断迭代器是否有效, 如果无效,说明没有对应的断点
			if(IsInvalidIterator(itr))
			{
				printf("\n>>>>>>> 目标进程自身触发了异常 <<<<<<\n");
				// 设置断点事件为有信号状态
				SetEvent(m_hBreakpointEvent);
				// 等待用户输入完成的信号
				WaitForSingleObject(m_hUserIputEvent , -1);
				dwStatus = DBG_EXCEPTION_NOT_HANDLED;
			}
			else
			{
				// 修复异常,如果能够成功修正断点, 则调用用户的处理函数
				if(true == FixException(itr))
				{
					// 设置断点事件为有信号状态
					SetEvent(m_hBreakpointEvent);
					// 等待用户输入完成的信号
					WaitForSingleObject(m_hUserIputEvent,-1);
				}
			}
			break;
		}
		




		case CREATE_PROCESS_DEBUG_EVENT: /*创建进程事件*/
			// 保存oep和加载基址
			m_oep = (uaddr)dbgEvent.u.CreateProcessInfo.lpStartAddress;
			m_imgBase = (uaddr)dbgEvent.u.CreateProcessInfo.lpBaseOfImage;
			break;
		case CREATE_THREAD_DEBUG_EVENT:/*创建线程事件*/
			break;
		case EXIT_PROCESS_DEBUG_EVENT:/*进程退出事件*/
			bIsSystemBreakpoint = true;
			return e_s_processQuit;
			break;
		case EXIT_THREAD_DEBUG_EVENT:
			break;
		case LOAD_DLL_DEBUG_EVENT:
			break;
		case UNLOAD_DLL_DEBUG_EVENT:
			break;
		case OUTPUT_DEBUG_STRING_EVENT:
			break;
	}


_SUCESS:
	ContinueDebugEvent(dbgEvent.dwProcessId ,
					   dbgEvent.dwThreadId ,
					   dwStatus);

	return e_s_sucess;
}


void DbgEngine::Close()
{
	DbgObject::Close();
	BreakpointEngine::Clear();
}


BOOL DbgEngine::WaitForBreakpointEvent(DWORD nTime)
{
	// 等待断点事件的信号
	return WaitForSingleObject(m_hBreakpointEvent , nTime) == WAIT_OBJECT_0;
}

void DbgEngine::FinishBreakpointEvnet()
{
	// 设置用户输入事件完成信号
	SetEvent(m_hUserIputEvent);
}
