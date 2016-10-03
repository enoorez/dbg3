#pragma once
#include "../DbgObject/DbgObject.h"
class Expression 
{
	DbgObject *m_pDdbgObj;
protected:
	// ��ȡ�����ڴ�
	SSIZE_T readProcMem(LPVOID lpAddr , DWORD dwSize);
	// ��ȡ�̼߳Ĵ�����ֵ
	bool    readRegValue(const char* pReg , const char** pEnd , SSIZE_T& uRegValue);
	bool	WriteRegValue(const char* pReg , const char** pEnd , SSIZE_T& uRegValue);

	// ��ȡ���ʽ���յ�ֵ
	bool    getValue(SSIZE_T& uValue , const char* pExpression , const char** pEnd , int nPriorty);

public:
	Expression(DbgObject* pDbgObj);
	~Expression();

	SSIZE_T getValue(const char* pExpression);
};

