#include "DBGSymbol.h"
#include <atlstr.h>

DbgSymbol::DbgSymbol()
{
}



DbgSymbol::~DbgSymbol( )
{
}


void DbgSymbol::InitSymbol(HANDLE hProcess)
{
	DWORD Options = SymGetOptions( );
	Options |= SYMOPT_DEBUG;
	::SymSetOptions(Options);

	::SymInitialize(hProcess ,
					0 ,
					TRUE
					);
	return;
}

SIZE_T DbgSymbol::FindApiAddress(HANDLE hProcess,const char* pszName)
{
	DWORD64  dwDisplacement = 0;
	char buffer[sizeof(SYMBOL_INFO)+MAX_SYM_NAME * sizeof(TCHAR)];
	PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbol->MaxNameLen = MAX_SYM_NAME;

	if(!SymFromName(hProcess , pszName , pSymbol))
	{
		return 0;
	}
	return (SIZE_T)pSymbol->Address;
}

BOOL DbgSymbol::GetFunctionName(HANDLE hProcess , SIZE_T nAddress , CString& strName)
{
	DWORD64  dwDisplacement = 0;
	char buffer[sizeof(SYMBOL_INFO)+MAX_SYM_NAME * sizeof(TCHAR)];
	PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbol->MaxNameLen = MAX_SYM_NAME;

	if(!SymFromAddr(hProcess , nAddress , &dwDisplacement , pSymbol))
		return FALSE;
	
	strName = pSymbol->Name;
	return TRUE;
}
