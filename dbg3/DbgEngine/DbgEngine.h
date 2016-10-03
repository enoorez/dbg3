#pragma once
#include "../DbgObject/DbgObject.h"
#include "../BreakpointEngine/BreakpointEngine.h"
class DbgUi;

/**
  * DbgEngine 会随时改变DbgObject中的m_hThread和m_hProcess的值.因为不同的
  * 异常可能是由不同的线程和进程引发的.
  * 调试引擎所管理断点模块.
  * 断点模块 : 提供对调试对象的控制.
  *	断点模块管理调试对象.
  * 
  */
typedef enum
{
	e_s_sucess = 0 ,
	e_s_processQuit,
}E_Status;


class DbgEngine;
typedef uint(__stdcall* fnExceptionProc)(DbgEngine* pDbg);
typedef DWORD(__stdcall *fnOtherException)(const EXCEPTION_DEBUG_INFO& exceptionInfo);


class DbgEngine : public BreakpointEngine
{

	bool	m_bStopOnSystemBreakpoint; // 调试器引擎配置,是否断在系统断点处
public:
	DbgEngine();
	~DbgEngine();
public:
	fnExceptionProc m_pfnBreakpointProc; // 断点处理函数
	fnOtherException m_pfnOtherException; // 其他异常处理函数
public:
	// 运行引擎
	E_Status Exec();
	void	 Close();
};

