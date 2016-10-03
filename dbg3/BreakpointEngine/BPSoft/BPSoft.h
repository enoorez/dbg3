#pragma once
#include "../BPObjcet/BPObjcet.h"

class BPSoft : public BPObject
{
public:
	BPSoft(DbgObject& dbgObject,uaddr uAddr);
	virtual ~BPSoft();

	unsigned char m_uData;
public:
	// ����ϵ�
	virtual bool Install();
	// �Ƴ��ϵ�
	virtual bool Remove();

	// ���ر��ϵ������
	virtual	E_BPType Type()const;

	// �ж϶ϵ��Ƿ����Լ�
	virtual bool IsMe(const EXCEPTION_DEBUG_INFO& ExcDebInf)const;
};
