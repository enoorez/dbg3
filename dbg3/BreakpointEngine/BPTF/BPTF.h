#pragma once
#include "../BPObjcet/BPObjcet.h"

class BPTF : public BPObject
{
	uint	m_bIsUserBP; // 记录是否是用户下的tf断点
public:
	BPTF(DbgObject& dbgObj , bool bIsTFBP = true);
	virtual ~BPTF();

public:
	// 插入断点
	virtual bool Install();
	
	virtual bool NeedRemove()const;
	// 移除断点
	virtual bool Remove();
	// 判断断点是否被命中
	virtual bool IsHit()const;
	// 返回本断点的类型
	virtual	E_BPType Type()const;
	// 判断断点是否是自己
	virtual bool IsMe(const EXCEPTION_DEBUG_INFO& ExcDebInf)const;

	void ConverToUserBreakpoint();
};
