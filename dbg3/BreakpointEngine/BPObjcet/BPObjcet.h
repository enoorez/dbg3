#pragma once
#include "../../DbgObject/DbgObject.h"
#include <atlstr.h>

typedef enum
{
	e_bt_none = 0 ,
	e_bt_tf ,/*�����ϵ�*/
	e_bt_soft ,/*����ϵ�*/
	
	e_bt_acc ,/*�ڴ���ʶϵ�*/
	e_bt_acc_e = e_bt_acc ,
	e_bt_acc_r ,
	e_bt_acc_w ,
	e_bt_acc_rw,

	e_bt_hard ,/*Ӳ���ϵ�*/
	e_bt_hard_e = e_bt_hard ,
	e_bt_hard_r ,
	e_bt_hard_w ,
	e_bt_hard_rw
}E_BPType;

class BPObject
{
public:
	DbgObject&  m_dbgObj;  // ���Զ���
	CStringA	m_condition;//�������ʽ
	bool		m_bOnce; // �Ƿ���һ���Զϵ�
	uaddr		m_uAddress;
public:
	BPObject(DbgObject& dbgObj);
	virtual ~BPObject();

public:
	virtual uaddr GetAddress()const;
	virtual const char* GetCondition()const;


	// ����ϵ�
	virtual bool Install() = 0;

	// �Ƴ��̵��
	virtual bool Remove() = 0;

	// �ж϶ϵ��Ƿ�����
	virtual bool IsHit()const ;

	// �ж϶ϵ��Ƿ��ɾ��
	virtual bool NeedRemove()const;

	// �жϵ�ǰ�쳣�����Ķϵ��Ƿ����Լ�
	virtual bool IsMe(const EXCEPTION_DEBUG_INFO& ExcDebInf)const = 0;

	// ���ضϵ��Լ�������
	virtual	E_BPType Type()const = 0;

	// ���öϵ�����б��ʽ
	virtual void SetCondition(const char* strConditoion);
	// ���ϵ�����Ϊһ����
	virtual void SetCondition(bool bOnce);
};

