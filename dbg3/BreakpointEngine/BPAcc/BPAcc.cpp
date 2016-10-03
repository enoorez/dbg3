
/**
  * ���cpp�Ĵ���δ��������
  */

#include "BPAcc.h"
#include "../../Expression/Expression.h"
#include "../../pectrl/Pectrl.h"

BPAcc::BPAcc(DbgObject& dbgObj , uaddr uAddr , E_BPType eType , uint uLen)
	:BPObject(dbgObj)
	, m_eType(eType)
	, m_uLen(uLen)
	, m_oldProtect1()
	, m_oldProtect2()
	, m_currentHitAddress()
	, m_currentHitAccType()
{
	m_uAddress = uAddr;
	Install();
}

BPAcc::~BPAcc()
{
	Remove();
}

bool BPAcc::Install()
{
	if(m_uLen >= 0x1000)
		return false;

	uaddr uAligAddr = ToAligentSize(m_uAddress + m_uLen , 0x1000);

	DWORD	dwTemp = 0;
	if(uAligAddr > ToAligentSize(m_uAddress , 0x1000))
	{
		dwTemp = VirtualProtectEx(m_dbgObj.m_hCurrProcess ,
								  (LPVOID)uAligAddr ,
								  0x1000 ,
								  PAGE_NOACCESS ,
								  &m_oldProtect2
								  );
	}
	// �趨�ϵ��ʱ�򣬲��ܳ���������ҳ
	dwTemp = VirtualProtectEx(m_dbgObj.m_hCurrProcess ,
							  (LPVOID)m_uAddress ,
							  m_uLen ,
							  PAGE_NOACCESS ,
							  &m_oldProtect1
							  );
	m_bOnce = false;
	return dwTemp==TRUE;
}


bool BPAcc::Remove()
{
	uaddr uAligAddr = ToAligentSize(m_uAddress + m_uLen , 0x1000);

	DWORD	dwTemp = 0;
	DWORD	dwOldProtect = 0;
	if(uAligAddr > ToAligentSize(m_uAddress , 0x1000))
	{
		dwTemp = VirtualProtectEx(m_dbgObj.m_hCurrProcess ,
								  (LPVOID)uAligAddr ,
								  0x1000 ,
								  m_oldProtect2 ,
								  &dwOldProtect
								  );
	}

	// �趨�ϵ��ʱ�򣬲��ܳ���������ҳ
	dwTemp = VirtualProtectEx(m_dbgObj.m_hCurrProcess ,
							  (LPVOID)m_uAddress ,
							  m_uLen ,
							  m_oldProtect1 ,
							  &dwOldProtect
							  );
	return dwTemp==TRUE;
}

bool BPAcc::IsHit()const
{
	// �ж��Ƿ��б��ʽ
	if(m_currentHitAddress != m_uAddress)
		return false;

	switch(m_eType)
	{
		case e_bt_acc_r:
			if(m_currentHitAccType != 0)
				return false;
		case e_bt_acc_w:
			if(m_currentHitAccType != 1)
				return false;
		case e_bt_acc_e:
			if(m_currentHitAccType != 8)
				return false;
	}


	if(!m_condition.IsEmpty()){
		Expression exp(&m_dbgObj);
		return exp.getValue(m_condition) != 0;
	}
	else
		return m_bOnce;
	return false;
}

bool BPAcc::IsMe(const EXCEPTION_DEBUG_INFO& ExcDebInf)const
{
	*(DWORD*)m_currentHitAccType = ExcDebInf.ExceptionRecord.ExceptionInformation[ 0 ];
	*(DWORD*)m_currentHitAddress = ExcDebInf.ExceptionRecord.ExceptionInformation[ 1 ];
	
	// �жϷ����쳣�ĵ�ַ
	if(ExcDebInf.ExceptionRecord.ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
	{
		if(m_currentHitAddress >= m_uAddress && m_currentHitAddress <= m_uAddress + m_uLen)
			return true;
	}
	return false;
}

E_BPType BPAcc::Type()const
{
	return m_eType;
}
