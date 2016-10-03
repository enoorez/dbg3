#pragma once
#include "../../DbgObject/DbgObject.h"
#include <windows.h>
#include <Dbghelp.h>
#pragma comment(lib,"Dbghelp.lib")
#include <atlstr.h>
using namespace std;
class DbgSymbol
{
public:
	DbgSymbol();
	virtual ~DbgSymbol( );
	void	InitSymbol(HANDLE hProcess);
	// ���Һ�������Ӧ�ĵ�ַ
	SIZE_T	FindApiAddress(HANDLE hProcess,const char* pszName);
	// ���ҵ�ַ��Ӧ�ĺ�����
	BOOL	GetFunctionName(HANDLE hProcess , SIZE_T nAddress , CString& strName);
};

