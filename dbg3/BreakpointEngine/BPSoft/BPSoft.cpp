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
	// 将断点地址处的1字节内容保存
	// 将0xcc写入到断点地址处
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
	// 恢复断点地址处的内容
	// 将eip--;
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

	// 将eip--;
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


