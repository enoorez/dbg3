#pragma once
#include <windows.h>
#include <list>
using std::list;
#include <map>
using std::map;
using std::pair;
#include <atlstr.h>

/**
  * 调试对象
  * 1. 加载基址
  * 2. PID
  * 3. TID
  * 4. hProcess
  * 5. hThread
  * 6. OEP
  */

typedef unsigned int	 uint;
typedef unsigned int     uaddr;
typedef unsigned char	 byte,*pbyte;

typedef struct MODULEFULLINFO
{
	CStringA name;
	LONG64	 uStart;
	LONG32	 uSize;
}MODULEFULLINFO , *PMODULEFULLINFO;


class DbgObject
{

public:
	DbgObject();
	~DbgObject();

public:
	uaddr			m_imgBase;		// 加载基址
	uint			m_oep;			// 程序入口地址
	uint			m_pid;			// 进程id
	uint			m_tid;			// 线程id
	HANDLE			m_hCurrProcess;	// 当前发生异常的进程句柄
	HANDLE			m_hCurrThread;	// 当前发生异常的线程句柄
	list<HANDLE>	m_hProcessList;	// 进程句柄表
	list<HANDLE>	m_hThreadList;	// 线程句柄表
public:
	bool Open(const char* pszFile);
	bool Open(const uint  uPid);
	bool IsOpen();
	bool IsClose();
	void Close();

	// 读取内存
	uint ReadMemory(uaddr  uAddress , pbyte pBuff , uint uSize);
	// 写入内存
	uint WriteMemory(uaddr uAddress , const pbyte pBuff , uint uSize);

	// 读取寄存器信息
	bool GetRegInfo(CONTEXT& ct);
	// 设置寄存器信息
	bool SetRegInfo(CONTEXT& ct);

	// 添加新的线程到线程列表
	void AddThread(HANDLE hThread);
	// 添加新的进程到进程列表
	void AddProcess(HANDLE hProcess , HANDLE hThread);
	// 从线程列表中移除一个线程
	bool RemoveThread(HANDLE hThread);
	// 从进程列表中移除一个进程
	bool RemoveProcess(HANDLE hProcess);

	// 获取模块列表
	void GetModuleList(list<MODULEFULLINFO>& moduleList);
};

