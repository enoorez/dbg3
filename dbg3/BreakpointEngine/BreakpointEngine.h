#pragma once
#include "../DbgObject/DbgObject.h"
#include "BPTF/BPTF.h"
#include "DbgSymbol/DbgSymbol.h"
#include <list>
using std::list;

typedef list<BPObject*>::iterator BpItr;  //迭代器

class BreakpointEngine : public DbgObject , public DbgSymbol
{
	
	list<BPObject*>	m_bpList;	// 断点列表
	list<BPObject*> m_bpRecList;// 待恢复的断点列表
public:
	BreakpointEngine();
	~BreakpointEngine();


protected:
	// 根据异常信息来查找断点, 返回断点在链表中的迭代器
	BpItr FindBreakpoint(const EXCEPTION_DEBUG_INFO& ExceptionInfo);
	// 修整断点
	bool FixBreakpoint(BpItr FindItr);
	// 恢复失效的断点
	bool ReInstallBreakpoint();
	// 检查断点是否重复
	BPObject* CheckRepeat(uaddr uAddress , E_BPType eType);
public:
	// 根据地址和类型查找一个断点
	BPObject* FindBreakpoint(uaddr uAddress , E_BPType eType = e_bt_none);
	// 添加一个断点到断点链表(可指定断点的地址,断点类型,断点的数据长度(一般只用于硬件断点和内存访问断点))
	BPObject* AddBreakPoint(uaddr uAddress , E_BPType eType , uint uLen = 0);
	// 添加一个断点到断点列表(指定一个API名(区分大小写),断点只能设置为软件断点)
	BPObject* AddBreakPoint(const char* pszApiName);
	// 移除一个断点(uIndex是断点在断点列表中的位置)
	bool DeleteBreakpoint(uint uIndex);


	// 判断一个断点迭代器是否是无效的
	bool IsInvalidIterator(const BpItr& itr)const;
	// 获取断点列表的开始迭代器
	list<BPObject*>::const_iterator GetBPListBegin()const;
	// 获取断点列表的结束迭代器
	list<BPObject*>::const_iterator GetBPListEnd()const;

	// 给断点设置一个条件表达式
	static void SetExp(BPObject* pBp , const CStringA& strExp);
	// 将断点设置成一次性断点
	static void SetOnce(BPObject* pBp , bool bOnce);
	// 清空断点链表
	void Clear();
};