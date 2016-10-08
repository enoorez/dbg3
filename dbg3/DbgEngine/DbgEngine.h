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

class DbgEngine : public BreakpointEngine
{

	bool	m_bStopOnSystemBreakpoint; // 调试器引擎配置,是否断在系统断点处
public:
	DbgEngine();
	~DbgEngine();
public:
	HANDLE			m_hBreakpointEvent;
	HANDLE			m_hUserIputEvent;
public:
	// 运行引擎
	E_Status Exec();
	void	 Close();

	BOOL	WaitForBreakpointEvent(DWORD nTime);
	void	FinishBreakpointEvnet();

};

