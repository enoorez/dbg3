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
  * ��  ��: ���Ҷϵ� ����list��һ��������
  * ����ֵ: BpItr
  * ��  ��: const EXCEPTION_DEBUG_INFO & ExceptionInfo
  */
BpItr BreakpointEngine::FindBreakpoint(const EXCEPTION_DEBUG_INFO& ExceptionInfo)
{
	BPObject* pBp = nullptr;
	for(BpItr i = m_bpList.begin();
		i != m_bpList.end();
		++i)
	{
		// ���öϵ�����Լ��ṩ�ķ������ж�
		// �쳣��Ϣ�Ƿ��Ӧ����ϵ�
		if((*i)->IsMe(ExceptionInfo))
			return i;
	}
	return m_bpList.end();
}


/**
  * ��  ��: �����ṩ�ĵ�ַ�����Ͳ��Ҷϵ�,���ضϵ����
  * ����ֵ: BPObject*
  * ��  ��: uaddr uAddress
  * ��  ��: E_BPType eType
  */
BPObject* BreakpointEngine::FindBreakpoint(uaddr uAddress , E_BPType eType)
{
	for(auto& i : m_bpList)
	{
		// �жϵ�ַ�Ƿ�һ��
		if(i->GetAddress() == uAddress)
		{
			// �ж������Ƿ�һ��, �����һ���Զϵ�,��ʹ������ͬҲ�ж�
			// Ϊ�ǲ�һ���Ķϵ�
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
  * ��  ��: �޸��쳣
  * ����ֵ: bool ,�򷵻�true,���򷵻�false
  * ��  ��: BpItr FindItr
  */
bool BreakpointEngine::FixException(BpItr FindItr)
{
	BPObject* pBp = *FindItr;

	// �ӱ����Խ������Ƴ��ϵ�(ʹ�ϵ�ʧЧ)
	pBp->Remove();

	// ��ȡ�ϵ��Ƿ�����,�ڲ�����һЩ�������ʽ,
	// ����������ʽΪtrue�ű�����,���򲻱�����
	bool bHit = pBp->IsHit();

	// �ж϶ϵ��Ƿ���Ҫ�Ӷϵ��б����Ƴ�
	if(pBp->NeedRemove())
	{
		// �ͷſռ�
		delete pBp;
		// �Ӷϵ����ɾ���ϵ��¼
		m_bpList.erase(FindItr);
	}
	else // û�б�����.
	{
		// �����tf�ϵ㣬����Ҫ������ָ��б�
		if(pBp->Type() == e_bt_tf)
		{
			pBp->Install();
			return bHit;
		}

		// ��Ϊ�ϵ��Ѿ����Ƴ�, �ϵ��Ѿ�ʧЧ,���,��Ҫ�ָ��ϵ����Ч��
		// ���ϵ������ָ��ϵ����
		m_bpRecList.push_back(pBp);

		// ����tf�ϵ�,����һ���쳣,���ڻָ�ʧЧ�Ķϵ�
		m_bpList.push_front(new BPTF(*this , false));
	}

	// �����Ƿ�����
	return bHit;
}

// ���²���ϵ�
bool BreakpointEngine::ReInstallBreakpoint()
{
	if(m_bpRecList.empty())
		return false;
	// ��������
	for(auto& i : m_bpRecList)
	{
		// ���öϵ㱾���ṩ�ķ���ȥ�ָ��ϵ����Ч��
		i->Install();
	}
	// �������
	m_bpRecList.clear();
	return true; 
}

/**
  * ��  ��: ��Ӷϵ㵽�ϵ��б���
  * ����ֵ: BPObject*
  * ��  ��: uaddr uAddress
  * ��  ��: E_BPType eType
  * ��  ��: uint uLen
  */
BPObject* BreakpointEngine::AddBreakPoint(uaddr uAddress , E_BPType eType , uint uLen)
{
	// ��Ӷϵ�
	BPObject	*pBp = nullptr;

	// �ж��Ƿ����ظ��ϵ�
	pBp = CheckRepeat(uAddress , eType);
	if(pBp!=nullptr) // ���ظ���TF�ϵ�
	{
		
		if(pBp->Type() != e_bt_tf)
			return pBp;
	
		// ����ظ��Ķϵ���TF�ϵ�,����Ҫ��TF�ϵ�ת��
		// ת�����û��ϵ㣨�����޷������û������ϣ�
		((BPTF*)pBp)->ConverToUserBreakpoint();
		return pBp;
	}

	if(eType == e_bt_tf)// TF�ϵ�
		pBp = new BPTF(*this,true);
	else if(eType == e_bt_soft) // ����ϵ�
		pBp = (new BPSoft(*this , uAddress));
	else if(eType >= e_bt_acc && eType <= e_bt_acc_rw)//�ڴ���ʶϵ�
		pBp = (new BPAcc(*this , uAddress , eType , uLen));
	else if(eType >= e_bt_hard && eType <= e_bt_hard_rw)// Ӳ���ϵ�
		pBp = (new BPHard(*this , uAddress , eType , uLen));
	else
		return nullptr;
	if(pBp->Install() == false)
		return false;

	// ���ϵ���뵽�ϵ�������
	m_bpList.push_front(pBp);
	return pBp;
}

BPObject* BreakpointEngine::AddBreakPoint(const char* pszApiName)
{
	// ����API�ĵ�ַ
	uaddr address = FindApiAddress(m_hCurrProcess , pszApiName);
	if(address == 0)
		return nullptr;
	// ���һ����׶ϵ�
	return AddBreakPoint(address , e_bt_soft);
}

/**
  * ��  ��: ��նϵ��б�
  * ����ֵ: void
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
  * ��  ��: �Ƴ��ϵ�
  * ����ֵ: bool
  * ��  ��: uint uIndex
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