#include "BPTF.h"
#include "../../DbgObject/RegStruct.h"
#include "../../Expression/Expression.h"

BPTF::BPTF(DbgObject& dbgObj , bool bIsTFBP)
	:BPObject(dbgObj)
	, m_bIsUserBP(bIsTFBP)
{
	Install();
}

BPTF::~BPTF()
{
	Remove();
}


bool BPTF::Install()
{
	// 设置TF标志位等于1即可

	CONTEXT	ct = { CONTEXT_CONTROL };
	if(!m_dbgObj.GetRegInfo(ct))
		return false;

	PEFLAGS pEflags = (PEFLAGS)&ct.EFlags;
	pEflags->TF = 1;

	// m_bOnce = true;
	return m_dbgObj.SetRegInfo(ct);
}


bool BPTF::Remove()
{
	return true;
}

bool BPTF::IsHit()const
{
	// 记录命中次数
	// 判断是否含有表达式
	if(!m_condition.IsEmpty()){
		Expression exp(&m_dbgObj);
		return exp.getValue(m_condition) != 0;
	}

	// 如果TF断点是用来恢复其他断点的,那么永远返回false
// 	if(m_bIsUserBP == TRUE)
// 		return m_bOnce;
// 	else
// 		return false;
	return m_bIsUserBP && m_bOnce;
}

bool BPTF::IsMe(const EXCEPTION_DEBUG_INFO& ExcDebInf)const
{
	// 判断断点类型是否匹配则
	if(ExcDebInf.ExceptionRecord.ExceptionCode != EXCEPTION_SINGLE_STEP)
		return false;

	// 获取调试寄存器
	CONTEXT ct = { CONTEXT_DEBUG_REGISTERS };
	if(!GetThreadContext(m_dbgObj.m_hCurrThread , &ct))
		return false;

	PDBG_REG6 pDr6 = (PDBG_REG6)&ct.Dr6;
	// 判断调试寄存器是否有值
	return !(pDr6->B0 || pDr6->B1 || pDr6->B2 || pDr6->B3);
}

E_BPType BPTF::Type()const
{
	return e_bt_tf;
}

bool BPTF::NeedRemove() const
{
	return m_bOnce;
}

void BPTF::ConverToUserBreakpoint()
{
	m_bIsUserBP = true;
}
