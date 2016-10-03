#pragma once
#include <Windows.h>
#include <atlstr.h>
#include <list>
using std::list;
#include "../BreakpointEngine/BreakpointEngine.h"


#define SHOW_TEXT_HEX		0x000000001
#define SHOW_TEXT_DEC		0x000000002
#define SHOW_LEN_BYTE		0x000000004
#define SHOW_LEN_WORD		0x000000008
#define SHOW_LEN_DWORD		0x000000010
#define SHOW_LEN_QWORD		0x000000020
#define	SHOW_TEXT_ANSI		0x000000040
#define	SHOW_TEXT_UNICODE	0x000000080

typedef enum
{
	e_st_log=0,
	e_st_regInfo ,
	e_st_memInfo ,
	e_st_DisAsmInfo
}E_ShowType;

class DbgUi
{
	static HANDLE	m_hStdOut;
	BreakpointEngine* m_pBpEngine;
public:
	DbgUi(BreakpointEngine* pBpEngine);
	~DbgUi();

	void Show(E_ShowType type , const CStringA& pszData);
	void Show(const char* pszStr , int x , int y , int color);

	// 格式: 颜色 字符串,颜色 , 字符串 .... ... 
	// 以0结尾.
	void ShowEx(int x , int y ,...);


	inline void printReg(SIZE_T dwReg , WORD color);
	inline void printEflag(DWORD dwFlag , WORD color);
public:
	void showAsm(SIZE_T Addr ,
				 const WCHAR* ShowOpc ,
				 const WCHAR* pszDiAsm ,
				 const WCHAR* pszCom,
				 const WCHAR* pszLineHeader = L"  "
				 );

	void showReg(const CONTEXT& ct);
	void showMem(SIZE_T virtualAddress , const LPBYTE lpBuff , int nSize , DWORD dwShowFlag);
	void showStack(SIZE_T virtualAddress , const LPBYTE lpBuff , int nSize);

	void showBreakPointList(list<BPObject*>::const_iterator beginItr , list<BPObject*>::const_iterator endItr);
};
