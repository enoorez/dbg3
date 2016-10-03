#pragma once
#include "../BPObjcet/BPObjcet.h"

class BPSoft : public BPObject
{
public:
	BPSoft(DbgObject& dbgObject,uaddr uAddr);
	virtual ~BPSoft();

	unsigned char m_uData;
public:
	// 插入断点
	virtual bool Install();
	// 移除断点
	virtual bool Remove();

	// 返回本断点的类型
	virtual	E_BPType Type()const;

	// 判断断点是否是自己
	virtual bool IsMe(const EXCEPTION_DEBUG_INFO& ExcDebInf)const;
};
