#pragma once
#include "../DbgObject/DbgObject.h"
class Expression 
{
	DbgObject *m_pDdbgObj;
protected:
	// 读取进程内存
	SSIZE_T readProcMem(LPVOID lpAddr , DWORD dwSize);
	// 获取线程寄存器的值
	bool    readRegValue(const char* pReg , const char** pEnd , SSIZE_T& uRegValue);
	bool	WriteRegValue(const char* pReg , const char** pEnd , SSIZE_T& uRegValue);

	// 获取表达式最终的值
	bool    getValue(SSIZE_T& uValue , const char* pExpression , const char** pEnd , int nPriorty);

public:
	Expression(DbgObject* pDbgObj);
	~Expression();

	SSIZE_T getValue(const char* pExpression);
};

