#pragma once
#include "../DbgObject/DbgObject.h"
#include "BPTF/BPTF.h"
#include "DbgSymbol/DbgSymbol.h"
#include <list>
using std::list;

typedef list<BPObject*>::iterator BpItr;  //������

class BreakpointEngine : public DbgObject , public DbgSymbol
{
	
	list<BPObject*>	m_bpList;	// �ϵ��б�
	list<BPObject*> m_bpRecList;// ���ָ��Ķϵ��б�
public:
	BreakpointEngine();
	~BreakpointEngine();


protected:
	// �����쳣��Ϣ�����Ҷϵ�, ���ضϵ��������еĵ�����
	BpItr FindBreakpoint(const EXCEPTION_DEBUG_INFO& ExceptionInfo);
	// �����ϵ�
	bool FixBreakpoint(BpItr FindItr);
	// �ָ�ʧЧ�Ķϵ�
	bool ReInstallBreakpoint();
	// ���ϵ��Ƿ��ظ�
	BPObject* CheckRepeat(uaddr uAddress , E_BPType eType);
public:
	// ���ݵ�ַ�����Ͳ���һ���ϵ�
	BPObject* FindBreakpoint(uaddr uAddress , E_BPType eType = e_bt_none);
	// ���һ���ϵ㵽�ϵ�����(��ָ���ϵ�ĵ�ַ,�ϵ�����,�ϵ�����ݳ���(һ��ֻ����Ӳ���ϵ���ڴ���ʶϵ�))
	BPObject* AddBreakPoint(uaddr uAddress , E_BPType eType , uint uLen = 0);
	// ���һ���ϵ㵽�ϵ��б�(ָ��һ��API��(���ִ�Сд),�ϵ�ֻ������Ϊ����ϵ�)
	BPObject* AddBreakPoint(const char* pszApiName);
	// �Ƴ�һ���ϵ�(uIndex�Ƕϵ��ڶϵ��б��е�λ��)
	bool DeleteBreakpoint(uint uIndex);


	// �ж�һ���ϵ�������Ƿ�����Ч��
	bool IsInvalidIterator(const BpItr& itr)const;
	// ��ȡ�ϵ��б�Ŀ�ʼ������
	list<BPObject*>::const_iterator GetBPListBegin()const;
	// ��ȡ�ϵ��б�Ľ���������
	list<BPObject*>::const_iterator GetBPListEnd()const;

	// ���ϵ�����һ���������ʽ
	static void SetExp(BPObject* pBp , const CStringA& strExp);
	// ���ϵ����ó�һ���Զϵ�
	static void SetOnce(BPObject* pBp , bool bOnce);
	// ��նϵ�����
	void Clear();
};