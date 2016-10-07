#include "BPObjcet.h"
#include "../../Expression/Expression.h"

BPObject::BPObject(DbgObject& dbgObj)
	:m_dbgObj(dbgObj)
	, m_bOnce()
{
}

BPObject::~BPObject()
{
}

void BPObject::SetCondition(const  char* strConditoion)
{
	m_condition = strConditoion	;
	m_bOnce = false;
}

void BPObject::SetCondition(bool bOnce)
{
	m_bOnce = bOnce;
	m_condition.Empty();
}

uaddr BPObject::GetAddress()const
{
	return m_uAddress;
}

const char* BPObject::GetCondition() const
{
	if(m_condition.IsEmpty())
		return nullptr;
	return m_condition;
}

bool BPObject::NeedRemove() const
{
	return m_bOnce;
}

bool BPObject::IsHit() const
{
	if(!m_condition.IsEmpty()){
		Expression exp(&m_dbgObj);
		return exp.getValue(m_condition) != 0;
	}
	return true;
}

