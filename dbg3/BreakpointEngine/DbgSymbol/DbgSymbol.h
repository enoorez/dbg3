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
	// 查找函数名对应的地址
	SIZE_T	FindApiAddress(HANDLE hProcess,const char* pszName);
	// 查找地址对应的函数名
	BOOL	GetFunctionName(HANDLE hProcess , SIZE_T nAddress , CString& strName);
};

