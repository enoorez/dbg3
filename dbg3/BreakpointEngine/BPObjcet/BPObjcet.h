#pragma once
#include "../../DbgObject/DbgObject.h"
#include <atlstr.h>

typedef enum
{
	e_bt_none = 0 ,
	breakpointType_tf ,/*单步断点*/
	breakpointType_soft ,/*软件断点*/
	
	breakpointType_acc ,/*内存访问断点*/
	breakpointType_acc_e = breakpointType_acc ,
	breakpointType_acc_r ,
	breakpointType_acc_w ,
	breakpointType_acc_rw,

	breakpointType_hard ,/*硬件断点*/
	breakpointType_hard_e = breakpointType_hard ,
	breakpointType_hard_r ,
	breakpointType_hard_w ,
	breakpointType_hard_rw
}E_BPType;


class BPObject
{
public:
	DbgObject&  m_dbgObj;  // 调试对象
	CStringA	m_condition;//条件表达式
	bool		m_bOnce; // 是否是一次性断点
	uaddr		m_uAddress;
public:
	BPObject(DbgObject& dbgObj);
	virtual ~BPObject();

public:
	virtual uaddr GetAddress()const;
	virtual const char* GetCondition()const;


	// 插入断点
	virtual bool Install() = 0;

	// 移除短点的
	virtual bool Remove() = 0;

	// 判断断点是否被命中
	virtual bool IsHit()const ;

	// 判断断点是否该删除
	virtual bool NeedRemove()const;

	// 判断当前异常触发的断点是否是自己
	virtual bool IsMe(const EXCEPTION_DEBUG_INFO& ExcDebInf)const = 0;

	// 返回断点自己的类型
	virtual	E_BPType Type()const = 0;

	// 设置断点的命中表达式
	virtual void SetCondition(const char* strConditoion);
	// 将断点设置为一次性
	virtual void SetCondition(bool bOnce);
};

