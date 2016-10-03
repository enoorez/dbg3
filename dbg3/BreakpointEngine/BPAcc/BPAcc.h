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
	// 插入断点
	virtual bool Install();
	// 移除断点
	virtual bool Remove();
	// 判断断点是否被命中
	virtual bool IsHit()const;
	// 返回本断点的类型
	virtual	E_BPType Type()const;
	// 判断断点是否是自己
	virtual bool IsMe(const EXCEPTION_DEBUG_INFO& ExcDebInfconst)const;
};
