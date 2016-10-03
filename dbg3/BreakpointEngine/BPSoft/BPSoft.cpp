#include "BPSoft.h"
#include "../../Expression/Expression.h"

BPSoft::BPSoft(DbgObject& dbgObject , uaddr uAddress)
	:BPObject(dbgObject)
	, m_uData()
{
	BPObject::m_uAddress = uAddress;
	Install();
}

BPSoft::~BPSoft()
{
}

bool BPSoft::Install()
{
	// ���ϵ��ַ����1�ֽ����ݱ���
	// ��0xccд�뵽�ϵ��ַ��
	DWORD dwFlag = 0;
	DWORD dwProtect = 0;
	char c = '\xcc';
	dwFlag = VirtualProtectEx(m_dbgObj.m_hCurrProcess ,
							  (LPVOID)m_uAddress ,
							  1 ,
							  PAGE_EXECUTE_READWRITE ,
							  &dwProtect
							  );
	if(dwFlag != TRUE)
		return false;
	m_dbgObj.ReadMemory(m_uAddress , &m_uData , 1);

	m_dbgObj.WriteMemory(m_uAddress , (pbyte)&c , 1);
	VirtualProtectEx(m_dbgObj.m_hCurrProcess ,
					 (LPVOID)m_uAddress ,
					 1 ,
					 dwProtect ,
					 &dwProtect
					 );
	m_bOnce = false;
	return true;
}

bool BPSoft::Remove()
{
	// �ָ��ϵ��ַ��������
	// ��eip--;
	DWORD dwFlag = 0;
	DWORD dwProtect = 0;

	dwFlag = VirtualProtectEx(m_dbgObj.m_hCurrProcess ,
							  (LPVOID)m_uAddress ,
							  1 ,
							  PAGE_EXECUTE_READWRITE ,
							  &dwProtect
							  );
	if(dwFlag != TRUE)
		return false;

	m_dbgObj.WriteMemory(m_uAddress , (pbyte)&m_uData , 1);

	CONTEXT ct = { CONTEXT_CONTROL };
	if(!m_dbgObj.GetRegInfo(ct))
		return false;

	// ��eip--;
	ct.Eip--;

	if(!m_dbgObj.SetRegInfo(ct))
		return false;

	VirtualProtectEx(m_dbgObj.m_hCurrProcess ,
					 (LPVOID)m_uAddress ,
					 1 ,
					 dwProtect ,
					 &dwProtect
					 );

	return true;
}


E_BPType BPSoft::Type()const
{
	return e_bt_soft;
}

bool BPSoft::IsMe(const EXCEPTION_DEBUG_INFO& ExcDebInf)const
{
	if(ExcDebInf.ExceptionRecord.ExceptionCode == EXCEPTION_BREAKPOINT)
	{
		return ExcDebInf.ExceptionRecord.ExceptionAddress == (LPVOID)m_uAddress;
	}
	return false;
}


