#pragma once
#include "../DbgObject/DbgObject.h"
#include "../BreakpointEngine/BreakpointEngine.h"
class DbgUi;

/**
  * DbgEngine ����ʱ�ı�DbgObject�е�m_hThread��m_hProcess��ֵ.��Ϊ��ͬ��
  * �쳣�������ɲ�ͬ���̺߳ͽ���������.
  * ��������������ϵ�ģ��.
  * �ϵ�ģ�� : �ṩ�Ե��Զ���Ŀ���.
  *	�ϵ�ģ�������Զ���.
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

	bool	m_bStopOnSystemBreakpoint; // ��������������,�Ƿ����ϵͳ�ϵ㴦
public:
	DbgEngine();
	~DbgEngine();
public:
	fnExceptionProc m_pfnBreakpointProc; // �ϵ㴦����
	fnOtherException m_pfnOtherException; // �����쳣������
public:
	// ��������
	E_Status Exec();
	void	 Close();
};

