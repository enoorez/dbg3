#pragma once
#include "../BPObjcet/BPObjcet.h"

typedef enum E_BPAcc_Type
{
	e_bpa_e = 0 ,
	e_bpa_r  ,
	e_bpa_w ,
	e_bpa_rw,
};

class BPAcc : public BPObject
{
	E_BPType m_eType;
	uint		 m_uLen;
	DWORD		 m_oldProtect1;
	DWORD		 m_oldProtect2;
	uaddr		 m_currentHitAddress;
	DWORD		 m_currentHitAccType;
public:
	BPAcc(DbgObject& dbgObj , uaddr uAddr ,E_BPType eType , uint uLen);
	virtual ~BPAcc();

public:
	// ����ϵ�
	virtual bool Install();
	// �Ƴ��ϵ�
	virtual bool Remove();
	// �ж϶ϵ��Ƿ�����
	virtual bool IsHit()const;
	// ���ر��ϵ������
	virtual	E_BPType Type()const;
	// �ж϶ϵ��Ƿ����Լ�
	virtual bool IsMe(const EXCEPTION_DEBUG_INFO& ExcDebInfconst)const;
};
