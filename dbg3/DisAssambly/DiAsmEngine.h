#pragma once
#include "../BreakpointEngine/BreakpointEngine.h"
#include <vector>
using std::vector;
#include <map>
using std::map;
using std::pair;
#include <list>
using std::list;

#define	MAX_OPCODESTRING	64

#ifndef DEASM_STRUCT
#define DEASM_STRUCT 
typedef	struct DISASMSTRUST
{
	SIZE_T	address;
	DWORD	dwCodeLen;

	CString	strOpCode;
	CString	strAsm;
	CString strCom; // ע��
}DISASMSTRUST;
#endif



void byteArr2HexStr(const LPBYTE& lpbArr , DWORD dwBSize , TCHAR* pszHexStr , const TCHAR wcSpace);
void byteArr2HexStr(const LPBYTE& lpbArr , DWORD dwBSize , char* pszHexStr , const TCHAR wcSpace);


typedef struct MODULEDISASM
{
	LONG32	uImgBase;
	LONG32	uSize;
	map<LONG32 , DISASMSTRUST> mapDisAsm;
}MODULEDISASM , *PMODULEDISASM;

// Ϊ����ģ�鱣�淴�����Ϣ,��ַ�ͷ��������ӳ���ϵ
class DisAsmEngine 
{
	list<MODULEDISASM> m_moduleInfo;
	BreakpointEngine* m_BPEngine;
public:
	DisAsmEngine(BreakpointEngine* pBPEngine);
	~DisAsmEngine( );
public:

	// ��ȡԶ�̽���ָ����ַ��ָ��ĳ���
	int getCoodeLen(const SIZE_T lpAddress);

	// ��ȡԶ�̽���ָ���opcode
	BYTE getOpCode(SIZE_T uAddress);
	// ������Զ�̽��̵�ָ����ַ����һ�����ָ��
	int diAsm(const SIZE_T lpAddress,CString& strOpCode,CString& strAsm,CString & strCom);
	int diAsm(const SIZE_T lpAddress,vector<DISASMSTRUST>& vecAsm,DWORD dwLine);


	// ��ȡ��ǰ��ַ��ǰx�еķ����ָ���ַ
	void AnalysisDisAsm();
	bool GetDisAsm(const LPBYTE pData,
				   LONG32 begAddr , 
				   LONG32 size,
				   LONG32 addr , 
				   map<LONG32 , 
				   DISASMSTRUST>& mapDisAsm);
};

