#ifndef _XEDPARSE_H
#define _XEDPARSE_H

#include <windows.h>

//XEDParse defines
#ifdef XEDPARSE_BUILD
#define XEDPARSE_EXPORT __declspec(dllexport)
#else
#define XEDPARSE_EXPORT __declspec(dllimport)
#endif //XEDPARSE_BUILD

#define XEDPARSE_CALL //calling convention

#define XEDPARSE_MAXBUFSIZE 256
#define XEDPARSE_MAXASMSIZE 16

//typedefs
// ����δʶ��Ĳ�����ʱ,�����һ���ص�����, �������������ǻص�������ԭ��
typedef bool (XEDPARSE_CALL* CBXEDPARSE_UNKNOWN)(const char* text, ULONGLONG* value);

//XEDParse enums
enum XEDPARSE_STATUS
{
    XEDPARSE_ERROR = 0, 
    XEDPARSE_OK = 1
};

//XEDParse structs
#pragma pack(push,8)
struct XEDPARSE
{
    bool x64; // ����trueʱ,���ɵ�opCode��64λ��,����falseʱ,���ɵ�opCode��32λ��
    ULONGLONG cip; //ָ��ĵ�ǰ��ַ
    unsigned int dest_size; //���ɵ�opCode�ĳ���(���ֵ�ɺ��������)
    CBXEDPARSE_UNKNOWN cbUnknown; //����δ����Ĳ�����ʱ�Ļص�����
    unsigned char dest[XEDPARSE_MAXASMSIZE]; //���ɵ�opCode(�������óɹ���,opCode�ͱ�������)
    char instr[XEDPARSE_MAXBUFSIZE]; // ��Ҫת����opCode�Ļ��ָ��
    char error[XEDPARSE_MAXBUFSIZE]; // ������Ϣ, ��������falseʱ,˵��ת��ʧ��,���ﱣ����ǳ����ԭ��
};
#pragma pack(pop)

#ifdef __cplusplus
extern "C"
{
#endif

XEDPARSE_EXPORT XEDPARSE_STATUS XEDPARSE_CALL XEDParseAssemble(XEDPARSE* XEDParse);

#ifdef __cplusplus
}
#endif

#endif // _XEDPARSE_H

