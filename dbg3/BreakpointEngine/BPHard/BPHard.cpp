#include "BPHard.h"
#include "../../DbgObject/RegStruct.h"
#include "../../Expression/Expression.h"

BPHard::BPHard(DbgObject& dbgObj , uaddr uAddress , E_BPType eType , uint uLen)
	: m_uLen(uLen)
	, m_eType(eType)
	, BPObject(dbgObj)
	, m_uDbgRegNum()
{
	BPObject::m_uAddress=uAddress;
	//Install();
}

BPHard::~BPHard()
{
}

bool BPHard::Install()
{
	CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };

	// Ϊ�����̵߳��߳�����Ӳ���ϵ�
	for(auto& i : m_dbgObj.m_hThreadList)
	{
		if(!GetThreadContext(i , &ct))
			continue;
		
		// DR0 ~ DR3 �����Ŷϵ��ַ
		// DR7 �������жϷ�ʽ�����ݳ���
		// �жϷ�ʽ�ı�����ʽΪ:
		//	RWX == 0 : ִ��ʱ�ж�
		//	RWX == 1 : д��ʱ�ж�
		//	RWX == 2 : I/O ����ʱ�ж�
		//	RWX == 3 : ��ȡ��д��ʱ�ж�
		// ���ݳ��ȵı�����ʽΪ(��RWX==0ʱ,LENXֻ��Ϊ0):
		//	LENX == 0 : 1�ֽ�,�жϵ�ַ���ö���
		//	LENX == 1 : 2�ֽ�,�жϵ�ַ����������2
		//  LENX == 2 : δ����
		//	LENX == 3 : 4�ֽ�,�жϵ�ַ����������4
		PDBG_REG7 pDbgReg7 = (PDBG_REG7)&ct.Dr7;

		// ���öϵ�����,ִ��:00,д��:01, ��д:11 
		// ���Dr0�Ĵ���û�б����ַ, ��ʹ��Dr0
		if(ct.Dr0 == 0)
		{
			// ���ϵ��ַ���浽Dr0�Ĵ���
			ct.Dr0 = m_uAddress;
			// ����ϵ���ж�Ϊ������
			pDbgReg7->L0 = 1;
			// ���õ�ַ�жϳ���
			pDbgReg7->LEN0 = 0;

			// ���ݶϵ��������RW�Ĵ���
			if(m_eType == e_bt_hard_e)
				pDbgReg7->RW0 = 0; // ִ�жϵ�
			else // ��д�ϵ�
			{
				pDbgReg7->RW0 = 3;
			}
			m_uDbgRegNum = 0;
		}
		else if(ct.Dr1 == 0)
		{
			ct.Dr1 = m_uAddress;
			pDbgReg7->G1 = 0;
			pDbgReg7->L1 = 1;
			pDbgReg7->LEN1 =0;
			if(m_eType == e_bt_hard_e)
				pDbgReg7->RW1 = 0;
			else
			{
				pDbgReg7->RW1 = 3;
			}
			m_uDbgRegNum = 1;
		}
		else if(ct.Dr2 == 0)
		{
			ct.Dr2 = m_uAddress;
			pDbgReg7->G2 = 0;
			pDbgReg7->L2 = 1;
			pDbgReg7->LEN2 = 0;
			if(m_eType == e_bt_hard_e)
				pDbgReg7->RW2 = 0;
			else
			{
				pDbgReg7->RW2 = 3;
			}
			m_uDbgRegNum = 2;
		}
		else if(ct.Dr3 == 0)
		{
			ct.Dr3 = m_uAddress;
			pDbgReg7->G3 = 0;
			pDbgReg7->L3 = 1;
			pDbgReg7->LEN3 = 0;
			if(m_eType == e_bt_hard_e)
				pDbgReg7->RW3 = 0;
			else
			{
				pDbgReg7->RW3 = 3;
			}
			m_uDbgRegNum = 3;
		}
		else
			return false;

		SetThreadContext(i , &ct);
		ZeroMemory(&ct , sizeof(CONTEXT));
	}
	m_bOnce = false;
	return true;
}

bool BPHard::Remove()
{
	CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
	PDBG_REG7  pDr7 = nullptr;

	for(auto& i : m_dbgObj.m_hThreadList)
	{
		if(!GetThreadContext(i , &ct))
			continue;
		
		pDr7 = (PDBG_REG7)&ct.Dr7;
		// DR0 ~ DR3 �����Ŷϵ��ַ
		switch(m_uDbgRegNum)
		{
			case 0:
				ct.Dr0 = 0;
				pDr7->L0 = 0;
				break;
			case 1:
				ct.Dr1 = 0;
				pDr7->L1 = 0;
				break;
			case 2:
				ct.Dr2 = 0;
				pDr7->L2 = 0;
				break;
			case 3:
				ct.Dr3 = 0;
				pDr7->L3 = 0;
				break;
		}
		m_dbgObj.SetRegInfo(ct);
	}
	return true;
}


E_BPType BPHard::Type()const
{
	return m_eType;
}

bool BPHard::IsMe(const EXCEPTION_DEBUG_INFO& ExcDebInf)const
{
	if(ExcDebInf.ExceptionRecord.ExceptionCode != EXCEPTION_SINGLE_STEP)
		return false;
	
	CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
	PDBG_REG7  pDr7 = nullptr;
	PDBG_REG6  pDr6 = nullptr;
	// Ӳ���ϵ���жϷ�ʽ������DR7�Ĵ�����
	// Ӳ���ϵ������DR0~DR3�Ĵ�����
	// DR6�Ĵ���������DR0~DR3�ĸ��Ĵ���������

	for(auto& i : m_dbgObj.m_hThreadList)
	{
		if(!GetThreadContext(i , &ct))
			continue;
		pDr6 = (PDBG_REG6)&ct.Dr6;
		pDr7 = (PDBG_REG7)&ct.Dr7;
		if(pDr6->B0)
		{
			if(pDr7->RW0 == 0)
			{
				if(m_eType == e_bt_hard_e)
					return (LPVOID)ct.Dr0 == ExcDebInf.ExceptionRecord.ExceptionAddress;
				else
					return false;
			}
			return m_eType != e_bt_hard_e;
		}
		else if(pDr6->B1)
		{
			if(pDr7->RW1 == 0)
			{
				if(m_eType == e_bt_hard_e)
					return (LPVOID)ct.Dr1 == ExcDebInf.ExceptionRecord.ExceptionAddress;
				else
					return false;
			}
			return m_eType != e_bt_hard_e;
		}
		else if(pDr6->B2)
		{
			if(pDr7->RW2 == 0)
			{
				if(m_eType == e_bt_hard_e)
					return (LPVOID)ct.Dr2 == ExcDebInf.ExceptionRecord.ExceptionAddress;
				else
					return false;
			}
			return m_eType != e_bt_hard_e;
		}
		else if(pDr6->B3)
		{
			if(pDr7->RW3 == 0)
			{
				if(m_eType == e_bt_hard_e)
					return (LPVOID)ct.Dr3 == ExcDebInf.ExceptionRecord.ExceptionAddress;
				else
					return false;
			}
			return m_eType != e_bt_hard_e;
		}
	}
	return false;
}

