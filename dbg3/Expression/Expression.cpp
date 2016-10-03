#include "Expression.h"


Expression::Expression(DbgObject* pDbgObj)
	: m_pDdbgObj(pDbgObj)
{

}


Expression::~Expression()
{
}


// 忽略大小写判断
// int strincmp(const char* left , int nLSize , const char* right , int nRSize)
// {
// 	if(left == right) // 是否是相同的字符串(地址相等)
// 		return true;
// 	if(left == NULL || right == NULL || nRSize > nLSize)
// 		return false;
// 
// 	for(; nRSize >= 0; --nRSize)
// 	{
// 		// 把字符转换成
// 		if('A' <= left[ nRSize ] && left[ nRSize ] <= 'Z')
// 		{
// 			if((left[ nRSize ] | 0x20) != (right[ nRSize ] | 0x20))
// 				return false;
// 		}
// 		
// 	}
// 	strnicmp()
// 	return true;
// }

/*
  : 碰到BYTE/WORD/DWORD/QWORD [ 时,先获取[]表达式中的值
	碰到寄存器,获取寄存器的值.
优先级号: 0~9, 数值越低优先级越高.
	每次函数的调用都会传入一个优先级数值,低优先级的逻辑分支
	不能处理高优先级传入的参数
*/
const char *skipspace(const char *p)
{
	for(; *p == ' ' || *p == '\t'; p++);
	return p;
}


bool isReg(const char* pStr , const char** pEnd)
{
	if(*pStr == 'e')
	{
		switch(*((WORD*)(pStr + 1)))
		{
			case 'xa'://eax
			case 'xc'://ecx
			case 'xd'://edx
			case 'xb'://ebx
			case 'is'://esi
			case 'id'://edi
			case 'ps'://esp
			case 'pb'://ebp
			case 'pi'://eip
				if(pEnd)
					*pEnd = pStr + 3;
				return true;
		}
	}
	else
	{
		switch(*((WORD*)(pStr + 1)))
		{
			case 'xa'://ax
			case 'cx'://cx
			case 'dx'://dx
			case 'bx'://bx
			case 'si'://si
			case 'di'://di
			case 'sp'://sp
			case 'bp'://bp
			case 'la'://al
			case 'ha'://ah
			case 'lc'://cl
			case 'hc'://ch
			case 'ld'://dl
			case 'hd'://dh
			case 'lb'://bl
			case 'hb'://bh
				if(pEnd)
					*pEnd = pStr + 2;
				return true;
		}
	}
	return false;
}

SSIZE_T Expression::readProcMem(LPVOID lpAddr , DWORD dwSize)
{
	SSIZE_T nValue = 0;
	if(dwSize==m_pDdbgObj->ReadMemory((uaddr)lpAddr , (pbyte)&nValue , dwSize))
		return nValue;
	return 0;
}


bool Expression::readRegValue(const char* pReg ,const char** pEnd, SSIZE_T& uRegValue)
{
	if(pReg == NULL)
		return false;
	CONTEXT ct = { 0 };
	ct.ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL;
	if(!m_pDdbgObj->GetRegInfo(ct))
		return false;

	pReg = skipspace(pReg);
	if(*pReg == 'e')
	{
		*pEnd = pReg + 3;
		switch(*((WORD*)(pReg + 1)))
		{
			case 'xa'://eax
				uRegValue= ct.Eax;
				break;
			case 'xc'://ecx
				uRegValue = ct.Ecx;
				break;
			case 'xd'://edx
				uRegValue = ct.Edx;
				break;
			case 'xb'://ebx
				uRegValue = ct.Ebx;
				break;
			case 'is'://esi
				uRegValue = ct.Esi;
				break;
			case 'id'://edi
				uRegValue = ct.Edi;
				break;
			case 'ps'://esp
				uRegValue = ct.Esp;
				break;
			case 'pb'://ebp
				uRegValue = ct.Ebp;
				break;
			case 'pi':// eip
				uRegValue = ct.Eip;
				break;
			default:
				uRegValue = 0;
				return false;
		}
	}
	else
	{
		*pEnd = pReg + 2;
		switch(*((WORD*)(pReg + 1)))
		{
			case 'xa'://ax
				uRegValue = ct.Eax & 0xFFFF;
				break;
			case 'xc'://cx
				uRegValue = ct.Ecx & 0xFFFF;
				break;
			case 'xd'://dx
				uRegValue = ct.Edx & 0xFFFF;
				break;
			case 'xb'://bx
				uRegValue = ct.Ebx & 0xFFFF;
				break;
			case 'is'://si
				uRegValue = ct.Esi & 0xFFFF;
				break;
			case 'id'://di
				uRegValue = ct.Edi & 0xFFFF;
				break;
			case 'ps'://sp
				uRegValue = ct.Esp & 0xFFFF;
				break;
			case 'pb'://bp
				uRegValue = ct.Ebp & 0xFFFF;
				break;
			case 'la'://al
				uRegValue = ct.Eax & 0x0f;
				break;
			case 'ha'://ah
				uRegValue = ct.Eax & 0xf0;
				break;
			case 'lc'://cl
				uRegValue = ct.Ecx & 0x0f;
				break;
			case 'hc'://ch
				uRegValue = ct.Ecx & 0xf0;
				break;
			case 'ld'://dl
				uRegValue = ct.Edx & 0x0f;
				break;
			case 'hd'://dh
				uRegValue = ct.Edx & 0xf0;
				break;
			case 'lb'://bl
				uRegValue = ct.Ebx & 0x0f;
				break;
			case 'hb'://bh
				uRegValue = ct.Ebx & 0xf0;
				break;
			default:
				uRegValue = 0;
				return false;
		}
	}
	return true;
}

bool Expression::WriteRegValue(const char* pReg , const char** pEnd , SSIZE_T& uRegValue)
{
	if(pReg == NULL)
		return false;
	CONTEXT ct = { 0 };
	ct.ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL;
	if(!m_pDdbgObj->GetRegInfo(ct))
		return false;
	pReg = skipspace(pReg);
	if(*pReg == 'e')
	{
		*pEnd = pReg + 3;
		switch(*((WORD*)(pReg + 1)))
		{
			case 'xa'://eax
				ct.Eax = uRegValue;
				break;
			case 'xc'://ecx
				ct.Ecx = uRegValue;
				break;
			case 'xd'://edx
				ct.Edx = uRegValue;
				break;
			case 'xb'://ebx
				ct.Ebx = uRegValue;
				break;
			case 'is'://esi
				ct.Esi = uRegValue;
				break;
			case 'id'://edi
				ct.Edi = uRegValue;
				break;
			case 'ps'://esp
				ct.Esp = uRegValue;
				break;
			case 'pb'://ebp
				ct.Ebp = uRegValue;
				break;
			case 'pi':// eip
				ct.Eip = uRegValue;
				break;
			default:
				 0;
				return false;
		}
	}
	else
	{
		*pEnd = pReg + 2;
		switch(*((WORD*)(pReg + 1)))
		{
			case 'xa'://ax
				ct.Eax = uRegValue & 0xFFFF;
				break;
			case 'xc'://cx
				ct.Ecx = uRegValue & 0xFFFF;
				break;
			case 'xd'://dx
				ct.Edx = uRegValue & 0xFFFF;
				break;
			case 'xb'://bx
				ct.Ebx = uRegValue & 0xFFFF;
				break;
			case 'is'://si
				ct.Esi = uRegValue & 0xFFFF;
				break;
			case 'id'://di
				ct.Edi = uRegValue & 0xFFFF;
				break;
			case 'ps'://sp
				ct.Esp = uRegValue & 0xFFFF;
				break;
			case 'pb'://bp
				ct.Ebp = uRegValue & 0xFFFF;
				break;
			case 'la'://al
				ct.Eax = uRegValue & 0x0f;
				break;
			case 'ha'://ah
				ct.Eax = uRegValue & 0xf0;
				break;
			case 'lc'://cl
				ct.Ecx = uRegValue & 0x0f;
				break;
			case 'hc'://ch
				ct.Ecx = uRegValue & 0xf0;
				break;
			case 'ld'://dl
				ct.Edx = uRegValue & 0x0f;
				break;
			case 'hd'://dh
				ct.Edx = uRegValue & 0xf0;
				break;
			case 'lb'://bl
				ct.Ebx = uRegValue & 0x0f;
				break;
			case 'hb'://bh
				ct.Ebx = uRegValue & 0xf0;
				break;
			default:
				 0;
				return false;
		}
	}

	if(!m_pDdbgObj->SetRegInfo(ct))
		return false;
	return true;
}


bool Expression::getValue(SSIZE_T& uValue , const char* pStr , const char** pEnd , int nPriorty)
{
	// 判断表达式是求内存地址的表达式还是普通求值表达式
	bool   bFalg = true;
	pStr = skipspace(pStr);

	if(*pStr == 0)
		return 0;


	// 判断表达式是否是内存寻址
	// 如果是内存寻址, 则把内存寻址的表达式作为子表达式,子表达式需要独立求值
	if(*pStr == '[')
	{
		// 得到子表达式的值
		if(!getValue(uValue , pStr + 1 , &pStr , 9))
		{
			bFalg = false;
		}
	}
	else if(_strnicmp(pStr , "BYTE" , 4) == 0)
	{
		// 得到子表达式的值
		pStr = skipspace(pStr + 4);
		getValue(uValue , pStr , &pStr , 9);
		if(*pStr == ']')	
		{
			++pStr;
			// uValue 是内存地址,读取出内存地址处的值
			uValue = readProcMem((LPVOID)uValue , sizeof(BYTE));
		}
		else
		{
			bFalg = false;
		}
	}
	else if(_strnicmp(pStr , "WORD" , 4) == 0)
	{
		pStr = skipspace(pStr + 4);
		// 得到子表达式的值
		getValue(uValue , pStr , &pStr , 9);
		// uValue 是内存地址,读取出内存地址处的值
		if(*pStr == ']')
		{
			++pStr;
			// uValue 是内存地址,读取出内存地址处的值
			uValue = readProcMem((LPVOID)uValue , sizeof(WORD));
		}
		else
		{
			bFalg = false;
		}
	}
	else if(_strnicmp(pStr , "DWORD" , 5)==0)
	{
		pStr = skipspace(pStr + 5);
		// 得到子表达式的值
		getValue(uValue , pStr , &pStr , 9);
		// uValue 是内存地址,读取出内存地址处的值
		if(*pStr == ']')
		{
			++pStr;
			// uValue 是内存地址,读取出内存地址处的值
			uValue = readProcMem((LPVOID)uValue , sizeof(DWORD));
		}
		else
		{
			bFalg = false;
		}
	}
	else if(_strnicmp(pStr , "QWORD" , 5) == 0)
	{
		pStr = skipspace(pStr + 5);
		// 得到子表达式的值
		getValue(uValue , pStr , &pStr , 9);
		// uValue 是内存地址,读取出内存地址处的值
		if(*pStr == ']')
		{
			++pStr;
			// uValue 是内存地址,读取出内存地址处的值
			uValue = readProcMem((LPVOID)uValue , sizeof(__int64));
		}
		else
		{
			bFalg = false;
		}
	}
	else if(*pStr == '0'&&pStr[ 1 ] == 'x')
	{
		uValue = strtol(pStr , (char**)&pStr , 16);
	}
	else if(isReg(pStr , NULL)) // 可能是给寄存器赋值
	{
		
		const char *pReg = pStr;
		isReg(pStr , &pReg);
		pReg = skipspace(pReg);
		if(*pReg == '=' && *(pReg+1) != '=') // 给寄存器赋值
		{
			
			// 获取操作数
			SSIZE_T nlValue = 0;
			if(getValue(nlValue , pReg + 1 , &pReg , 9))
			{
				uValue = nlValue;
				WriteRegValue(pStr , &pStr , nlValue);
				pStr = pReg;
			}
			else
				bFalg = false;
		}
		else
		{
			if(!readRegValue(pStr , &pStr , uValue))
				bFalg = false;
		}
	}
	else if('0' <= *pStr && *pStr <= '9')
	{
		uValue = strtol(pStr , (char**)&pStr , 10);
	}
	else if(*pStr == '+')
	{
		pStr = skipspace(pStr + 1);
		uValue = getValue(uValue , pStr , &pStr , 0);
	}
	else if(*pStr == '-')
	{
		pStr = skipspace(pStr + 1);
		if(getValue(uValue , pStr , &pStr , 0))
		{
			uValue = -uValue;
		}
	}
	else if(*pStr == '(') /*括号的优先级最低*/
	{
		pStr = skipspace(pStr + 1);
		getValue(uValue , pStr , &pStr , 9);
		pStr = skipspace(pStr);
		if(*pStr == ')')
			pStr = skipspace(pStr + 1);
		else
			bFalg = false;
	}
	else
	{
		bFalg = false;
	}



	pStr = skipspace(pStr);

	/* 得到运算符 */
	if(bFalg == false)
		return false;

	if(*pStr == '+' && nPriorty > 3)
	{
		// 获取第二个操作数
		SSIZE_T right = 0;
		if(!getValue(right , pStr + 1 , &pStr , 3))
			return false;
		uValue += right;
	}
	else if(*pStr == '-'  && nPriorty > 3)
	{
		// 获取第二个操作数
		SSIZE_T right = 0;
		if(!getValue(right , pStr + 1 , &pStr , 3))
			return false;
		uValue -= right;
	}
	else if(*pStr == '*'  && nPriorty > 2)
	{
		// 获取第二个操作数
		SSIZE_T right = 0;
		if(!getValue(right , pStr + 1 , &pStr , 2))
			return false;
		uValue *= right;
	}
	else if(*pStr == '/'  && nPriorty > 2)
	{
		// 获取第二个操作数
		SSIZE_T right = 0;
		if(!getValue(right , pStr + 1 , &pStr , 2))
			return false;
		uValue /= right;
	}
	else if(*pStr == '%'  && nPriorty > 2)
	{
		// 获取第二个操作数
		SSIZE_T right = 0;
		if(!getValue(right , pStr + 1 , &pStr , 2))
			return false;
		uValue %= right;
	}
	else if(*pStr == '<' && nPriorty>9)
	{
		// 获取第二个操作数
		SSIZE_T right = 0;
		if(!getValue(right , pStr + 1 , &pStr , 9))
			return false;
		uValue = uValue < right;
	}
	else if(*pStr == '>'&& nPriorty>9)
	{
		// 获取第二个操作数
		SSIZE_T right = 0;
		if(!getValue(right , pStr + 1 , &pStr , 9))
			return false;
		uValue = uValue > right;
	}
	else if(*pStr == '='&&pStr[ 1 ] == '='&& nPriorty>=9)
	{
		// 获取第二个操作数
		SSIZE_T right = 0;
		if(!getValue(right , pStr + 2 , &pStr , 9))
			return false;
		uValue = uValue == right;
	}
	else if(*pStr == '&'&&pStr[ 1 ] == '&' && nPriorty>=9)
	{
		// 获取第二个操作数
		SSIZE_T right = 0;
		if(!getValue(right , pStr +2 , &pStr , 9))
			return false;
		uValue = uValue && right;
	}
	else if(*pStr == '|'&&pStr[ 1 ] == '|'&& nPriorty>=9)
	{
		// 获取第二个操作数
		SSIZE_T right = 0;
		if(!getValue(right , pStr + 2 , &pStr , 9))
			return false;
		uValue = uValue || right;
	}
	else if(*pStr == '<'&&pStr[ 1 ] == '='&& nPriorty>=9)
	{
		// 获取第二个操作数
		SSIZE_T right = 0;
		if(!getValue(right , pStr + 2 , &pStr , 9))
			return false;
		uValue = uValue <= right;
	}
	else if(*pStr == '>'&&pStr[ 1 ] == '='&& nPriorty>=9)
	{
		// 获取第二个操作数
		SSIZE_T right = 0;
		if(!getValue(right , pStr + 2 , &pStr , 9))
			return false;
		uValue = uValue >= right;
	}
	else if(*pStr == '!'&&pStr[ 1 ] == '='&& nPriorty>=9)
	{
		// 获取第二个操作数
		SSIZE_T right = 0;
		if(!getValue(right , pStr + 2 , &pStr , 9))
			return false;
		uValue = uValue != right;
	}


	*pEnd = pStr;
	return bFalg;
}


SSIZE_T Expression::getValue(const char* pStr)
{
	const char* pEnd = pStr;
	SSIZE_T uValue = 0;
	if(getValue(uValue , pStr , &pEnd , 9))
		return uValue;
	return 0;
}

