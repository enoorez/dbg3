#pragma once
#include <atlstr.h>
#include "..\\DbgObject\\DbgObject.h"


class HOOKObject
{
	DbgObject*	m_pDbgObj;
	CStringA	m_name;
	uaddr		m_uAddress;
	byte		m_uData[ 8 ];

	pbyte		m_MyFunctionCodeAddr;
	pbyte		m_pFuntionData;
public:
	HOOKObject();
	~HOOKObject();


};
