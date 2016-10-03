#include "BreakpointEngine.h"
#include "BPSoft/BPSoft.h"
#include "BPHard/BPHard.h"
#include "BPAcc/BPAcc.h"

BreakpointEngine::BreakpointEngine()
{
}

BreakpointEngine::~BreakpointEngine()
{
	Clear();
}

/**
  * 功  能: 查找断点 返回list的一个迭代器
  * 返回值: BpItr
  * 形  参: const EXCEPTION_DEBUG_INFO & ExceptionInfo
  */
BpItr BreakpointEngine::FindBreakpoint(const EXCEPTION_DEBUG_INFO& ExceptionInfo)
{
	BPObject* pBp = nullptr;
	for(BpItr i = m_bpList.begin();
		i != m_bpList.end();
		++i)
	{
		// 利用断点对象自己提供的方法来判断
		// 异常信息是否对应这个断点
		if((*i)->IsMe(ExceptionInfo))
			return i;
	}
	return m_bpList.end();
}


/**
  * 功  能: 根据提供的地址和类型查找断点,返回断点对象
  * 返回值: BPObject*
  * 形  参: uaddr uAddress
  * 形  参: E_BPType eType
  */
BPObject* BreakpointEngine::FindBreakpoint(uaddr uAddress , E_BPType eType)
{
	for(auto& i : m_bpList)
	{
		// 判断地址是否一致
		if(i->GetAddress() == uAddress)
		{
			// 判断类型是否一致, 如果是一次性断点,即使类型相同也判断
			// 为是不一样的断点
			if(eType != e_bt_none && i->Type() == eType)
				return i;
			else
				return nullptr;
			return i;
		}
	}
	return nullptr;
}



/**
  * 功  能: 修复异常
  * 返回值: bool ,则返回true,否则返回false
  * 形  参: BpItr FindItr
  */
bool BreakpointEngine::FixException(BpItr FindItr)
{
	BPObject* pBp = *FindItr;

	// 从被调试进程中移除断点(使断点失效)
	pBp->Remove();

	// 获取断点是否被命中,内部会有一些条件表达式,
	// 如果条件表达式为true才被命中,否则不被命中
	bool bHit = pBp->IsHit();

	// 判断断点是否需要从断点列表中移除
	if(pBp->NeedRemove())
	{
		// 释放空间
		delete pBp;
		// 从断点表中删除断点记录
		m_bpList.erase(FindItr);
	}
	else // 没有被命中.
	{
		// 如果是tf断点，则不需要插入待恢复列表
		if(pBp->Type() == e_bt_tf)
		{
			pBp->Install();
			return bHit;
		}

		// 因为断点已经被移除, 断点已经失效,因此,需要恢复断点的有效性
		// 将断点放入待恢复断点表中
		m_bpRecList.push_back(pBp);

		// 插入tf断点,触发一个异常,用于恢复失效的断点
		m_bpList.push_front(new BPTF(*this , false));
	}

	// 返回是否被命中
	return bHit;
}

// 重新插入断点
bool BreakpointEngine::ReInstallBreakpoint()
{
	if(m_bpRecList.empty())
		return false;
	// 遍历链表
	for(auto& i : m_bpRecList)
	{
		// 调用断点本身提供的方法去恢复断点的有效性
		i->Install();
	}
	// 清空链表
	m_bpRecList.clear();
	return true; 
}

/**
  * 功  能: 添加断点到断点列表中
  * 返回值: BPObject*
  * 形  参: uaddr uAddress
  * 形  参: E_BPType eType
  * 形  参: uint uLen
  */
BPObject* BreakpointEngine::AddBreakPoint(uaddr uAddress , E_BPType eType , uint uLen)
{
	// 添加断点
	BPObject	*pBp = nullptr;

	// 判断是否含有重复断点
	pBp = CheckRepeat(uAddress , eType);
	if(pBp!=nullptr) // 有重复的TF断点
	{
		
		if(pBp->Type() != e_bt_tf)
			return pBp;
	
		// 如果重复的断点是TF断点,则需要将TF断点转换
		// 转换成用户断点（否则无法断在用户界面上）
		((BPTF*)pBp)->ConverToUserBreakpoint();
		return pBp;
	}

	if(eType == e_bt_tf)// TF断点
		pBp = new BPTF(*this,true);
	else if(eType == e_bt_soft) // 软件断点
		pBp = (new BPSoft(*this , uAddress));
	else if(eType >= e_bt_acc && eType <= e_bt_acc_rw)//内存访问断点
		pBp = (new BPAcc(*this , uAddress , eType , uLen));
	else if(eType >= e_bt_hard && eType <= e_bt_hard_rw)// 硬件断点
		pBp = (new BPHard(*this , uAddress , eType , uLen));
	else
		return nullptr;
	if(pBp->Install() == false)
		return false;

	// 将断点插入到断点链表中
	m_bpList.push_front(pBp);
	return pBp;
}

BPObject* BreakpointEngine::AddBreakPoint(const char* pszApiName)
{
	// 查找API的地址
	uaddr address = FindApiAddress(m_hCurrProcess , pszApiName);
	if(address == 0)
		return nullptr;
	// 添加一个软甲断点
	return AddBreakPoint(address , e_bt_soft);
}

/**
  * 功  能: 清空断点列表
  * 返回值: void
  */
void BreakpointEngine::Clear()
{
	for(auto& i : m_bpList)
	{
		i->Remove();
		delete i;
	}
	m_bpList.clear();
	m_bpRecList.clear();

}

/**
  * 功  能: 移除断点
  * 返回值: bool
  * 形  参: uint uIndex
  */
bool BreakpointEngine::DeleteBreakpoint(uint uIndex)
{
	if(uIndex >= m_bpList.size())
		return false;

	for(BpItr i = m_bpList.begin();
		i != m_bpList.end();
		++i)
	{
		if(uIndex-- == 0)
		{
			m_bpList.erase(i);
			return true;
		}
	}
	return false;
}

void BreakpointEngine::SetExp(BPObject* pBp , const CStringA& strExp)
{
	if(pBp != nullptr)
		pBp->SetCondition(strExp);
}

void BreakpointEngine::SetOnce(BPObject* pBp , bool bOnce)
{
	if(pBp!=nullptr)
		pBp->SetCondition(bOnce);
}

bool BreakpointEngine::IsInvalidIterator(const BpItr& itr)const
{
	return itr == m_bpList.end();
}

list<BPObject*>::const_iterator BreakpointEngine::GetBPListBegin()const
{
	return m_bpList.begin();
}

list<BPObject*>::const_iterator BreakpointEngine::GetBPListEnd() const
{
	return m_bpList.end();
}

BPObject* BreakpointEngine::CheckRepeat(uaddr uAddress , E_BPType eType)
{
	for (auto& i : m_bpList)
	{
		if(i->Type() == eType && i->m_bOnce == false)
		{
			if(i->m_uAddress == uAddress)
				return i;
		}
	}
	return nullptr;
}