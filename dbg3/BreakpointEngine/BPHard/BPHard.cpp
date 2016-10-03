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

	// 为所有线程的线程设置硬件断点
	for(auto& i : m_dbgObj.m_hThreadList)
	{
		if(!GetThreadContext(i , &ct))
			continue;
		
		// DR0 ~ DR3 保存着断点地址
		// DR7 保存着中断方式和数据长度
		// 中断方式的表现形式为:
		//	RWX == 0 : 执行时中断
		//	RWX == 1 : 写入时中断
		//	RWX == 2 : I/O 操作时中断
		//	RWX == 3 : 读取或写入时中断
		// 数据长度的表现形式为(当RWX==0时,LENX只能为0):
		//	LENX == 0 : 1字节,中断地址不用对齐
		//	LENX == 1 : 2字节,中断地址必须能整除2
		//  LENX == 2 : 未定义
		//	LENX == 3 : 4字节,中断地址必须能整除4
		PDBG_REG7 pDbgReg7 = (PDBG_REG7)&ct.Dr7;

		// 设置断点类型,执行:00,写入:01, 读写:11 
		// 如果Dr0寄存器没有保存地址, 则使用Dr0
		if(ct.Dr0 == 0)
		{
			// 将断点地址保存到Dr0寄存器
			ct.Dr0 = m_uAddress;
			// 激活断点的中断为本任务
			pDbgReg7->L0 = 1;
			// 设置地址中断长度
			pDbgReg7->LEN0 = 0;

			// 根据断点的来设置RW寄存器
			if(m_eType == e_bt_hard_e)
				pDbgReg7->RW0 = 0; // 执行断点
			else // 读写断点
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
		// DR0 ~ DR3 保存着断点地址
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
	// 硬件断点的中断方式保存在DR7寄存器中
	// 硬件断点表保存在DR0~DR3寄存器中
	// DR6寄存器保存着DR0~DR3哪个寄存器被启用

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

