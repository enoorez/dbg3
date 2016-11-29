#pragma once
#include <atlstr.h>

/**
  * HOOKEngine
  * 
  */
class HOOKEngine
{
	CStringA	m_functionName;
	void*		m_pAddress;
	char		m_uData[ 8 ];

	char*		m_MyFunctionCodeAddr;
	char*		m_pFuntionData;
public:
	HOOKEngine();
	~HOOKEngine();
};

