#pragma once
#include "../BPObjcet/BPObjcet.h"


class BPHard : public BPObject
{
	uint			m_uLen;
	E_BPType		m_eType;
	uint			m_uDbgRegNum;
public:	
	BPHard(DbgObject& dbgObj , uaddr uAddress , E_BPType eType , uint uLen);
	virtual ~BPHard();

	// ����ϵ�
	virtual bool Install();
	// �Ƴ��ϵ�
	virtual bool Remove();
	
	// ���ر��ϵ������
	virtual	E_BPType Type()const;
	// �ж϶ϵ��Ƿ����Լ�
	virtual bool IsMe(const EXCEPTION_DEBUG_INFO& ExcDebInf)const;
};
