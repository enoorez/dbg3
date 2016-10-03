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
// 出现未识别的操作数时,会调用一个回调函数, 下面这个定义就是回调函数的原型
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
    bool x64; // 等于true时,生成的opCode是64位的,等于false时,生成的opCode是32位的
    ULONGLONG cip; //指令的当前地址
    unsigned int dest_size; //生成的opCode的长度(这个值由函数来输出)
    CBXEDPARSE_UNKNOWN cbUnknown; //出现未定义的操作数时的回调函数
    unsigned char dest[XEDPARSE_MAXASMSIZE]; //生成的opCode(函数调用成功后,opCode就保存在这)
    char instr[XEDPARSE_MAXBUFSIZE]; // 需要转换成opCode的汇编指令
    char error[XEDPARSE_MAXBUFSIZE]; // 错误信息, 函数返回false时,说明转换失败,这里保存的是出错的原因
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

