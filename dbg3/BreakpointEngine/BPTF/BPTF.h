#pragma once
#include "../BPObjcet/BPObjcet.h"

class BPTF : public BPObject
{
	uint	m_bIsUserBP; // ��¼�Ƿ����û��µ�tf�ϵ�
public:
	BPTF(DbgObject& dbgObj , bool bIsTFBP = true);
	virtual ~BPTF();

public:
	// ����ϵ�
	virtual bool Install();
	
	virtual bool NeedRemove()const;
	// �Ƴ��ϵ�
	virtual bool Remove();
	// �ж϶ϵ��Ƿ�����
	virtual bool IsHit()const;
	// ���ر��ϵ������
	virtual	E_BPType Type()const;
	// �ж϶ϵ��Ƿ����Լ�
	virtual bool IsMe(const EXCEPTION_DEBUG_INFO& ExcDebInf)const;

	void ConverToUserBreakpoint();
};
