#include "DbgObject.h"
#include <algorithm>
#include "..\\pectrl\\Pectrl.h"
#include <psapi.h>



DbgObject::DbgObject()
	:m_imgBase()
	, m_oep()
	, m_pid()
	, m_tid()
	, m_hCurrProcess()
	, m_hCurrThread()
{
}


DbgObject::~DbgObject()
{
}

bool DbgObject::Open(const char* pszFile)
{
	if(pszFile == nullptr)
		return false;
	STARTUPINFOA         stcStartupInfo = { sizeof(STARTUPINFOA) };
	PROCESS_INFORMATION  stcProcInfo = { 0 };     // 进程信息

	/* 创建调试进程程 */
	BOOL bRet = FALSE;
	bRet = CreateProcessA(pszFile,                // 可执行模块路径
						  NULL ,                   // 命令行
						  NULL ,                   // 安全描述符
						  NULL ,                   // 线程属性是否可继承
						  FALSE ,                  // 否从调用进程处继承了句柄
						  DEBUG_ONLY_THIS_PROCESS ,// 以调试的方式启动
						  NULL ,                   // 新进程的环境块
						  NULL ,                   // 新进程的当前工作路径（当前目录）
						  &stcStartupInfo ,        // 指定进程的主窗口特性
						  &stcProcInfo             // 接收新进程的识别信息
						  );
	if(bRet == FALSE)
		return false;

	m_hCurrProcess = stcProcInfo.hProcess;
	m_hCurrThread = stcProcInfo.hThread;
	m_pid = stcProcInfo.dwProcessId;
	m_tid = stcProcInfo.dwThreadId;
	
	return true;
}

bool DbgObject::Open(const uint	  uPid)
{
	// 未实现
	return true;
}

bool DbgObject::IsOpen()
{
	return m_hCurrProcess != 0;
}

void DbgObject::Close()
{
	TerminateProcess(m_hCurrProcess , 0);
	m_hCurrProcess = 0;
}


bool DbgObject::IsClose()
{
	return m_hCurrProcess == 0;
}

uint DbgObject::ReadMemory(uaddr  uAddress , pbyte pBuff , uint uSize)
{
	DWORD	dwRead = 0;
	ReadProcessMemory(m_hCurrProcess , (LPVOID)uAddress , pBuff , uSize , &dwRead);
	return dwRead;
}

uint DbgObject::WriteMemory(uaddr uAddress , const pbyte pBuff , uint uSize)
{
	DWORD	dwWrite = 0;
	WriteProcessMemory(m_hCurrProcess , (LPVOID)uAddress , pBuff , uSize , &dwWrite);
	return dwWrite;
}

bool DbgObject::GetRegInfo(CONTEXT& ct)
{
	return GetThreadContext(m_hCurrThread , &ct)==TRUE;
}

bool DbgObject::SetRegInfo(CONTEXT& ct)
{
	return SetThreadContext(m_hCurrThread , &ct) == TRUE;
}

void DbgObject::AddThread(HANDLE hThread)
{
	list<HANDLE>::iterator i = m_hThreadList.begin();
	for(; i != m_hThreadList.end(); ++i)
	{
		if(*i == hThread)
			return;
	}
	m_hThreadList.push_back(hThread);
}

void DbgObject::AddProcess(HANDLE hProcess , HANDLE hThread)
{
	AddThread(hThread);
	list<HANDLE>::iterator i = m_hProcessList.begin();
	for(; i != m_hProcessList.end(); ++i)
	{
		if(*i == hProcess)
			return;
	}
	m_hProcessList.push_back(hProcess);
}

bool DbgObject::RemoveThread(HANDLE hThread)
{
	list<HANDLE>::iterator i = m_hThreadList.begin();
	for(;i != m_hThreadList.end();++i)
	{
		if(*i == hThread)
		{
			m_hThreadList.erase(i);
			return true;
		}
	}
	return false;
}

bool DbgObject::RemoveProcess(HANDLE hProcess)
{
	list<HANDLE>::iterator i = m_hProcessList.begin();
	for(; i != m_hProcessList.end(); ++i)
	{
		if(*i == hProcess)
		{
			m_hProcessList.erase(i);
			return true;
		}
	}
	return false;
}



void DbgObject::GetModuleList(list<MODULEFULLINFO>& moduleList)
{
	// 枚举进程模块
	 DWORD dwNeed = 0;
	 EnumProcessModulesEx(m_hCurrProcess , 
	 					 nullptr , 
	 					 0 , 
	 					 &dwNeed , 
	 					 LIST_MODULES_ALL);
	 DWORD	dwModuleCount = dwNeed / sizeof(HMODULE);
	 HMODULE *phModule = new HMODULE[ dwModuleCount ];
	 EnumProcessModulesEx(m_hCurrProcess , 
	 					 phModule , 
	 					 dwNeed , 
	 					 &dwNeed , 
	 					 LIST_MODULES_ALL);
	 
	 MODULEINFO		moif = { 0 };
	 char path[ MAX_PATH ];
	 moduleList.resize(dwModuleCount);
	 list<MODULEFULLINFO>::iterator itr = moduleList.begin();
	 // 循环获取模块信息
	 for(SIZE_T i = 0; i < dwModuleCount; ++i)
	 {
	 	// 获取模块路径
	 	GetModuleFileNameExA(m_hCurrProcess ,
	 						 phModule[ i ] , 
	 						 path , 
	 						 MAX_PATH);
	 
	 	// 获取模块其他信息
	 	GetModuleInformation(m_hCurrProcess , 
	 						 phModule[ i ] , 
	 						 &moif , sizeof(MODULEINFO));
	 	itr->name = PathFindFileNameA(path);
	 	itr->uStart = (LONG64)moif.lpBaseOfDll; // dll 基址
	 	itr->uSize = moif.SizeOfImage; // dll 大小
	 	++itr;
	 }
	 
	 delete[] phModule;
}
