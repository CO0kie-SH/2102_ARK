#pragma once
#include <ntifs.h>

// 注册表遍历子键信息结构体
typedef struct _FIND_REG_SUBKEY_INFO
{
	ULONG serialNumber;  // 序号
	WCHAR regParent[266];  // 主键
	WCHAR regValue[266];  // 子键名称

	ULONG status;  // 用以反馈
}FIND_REG_SUBKEY_INFO, * PFIND_REG_SUBKEY_INFO;

// 注册表遍历键值信息结构体
typedef struct _FIND_REG_VALUE_INFO
{
	ULONG serialNumber;  // 序号
	WCHAR regParent[266];  // 主键
	WCHAR regValue[266];  // 值
	ULONG regType;  // 类型

	ULONG status;  // 用以反馈
}FIND_REG_VALUE_INFO, * PFIND_REG_VALUE_INFO;



/************************************************************************
* 函数名称:ZwQueryKey
* 功能描述:获得某注册表项究竟有多少个子项
* 参数列表:
        KeyHandle:获得的注册表句柄
        KeyInformationClass:查询的类别，一般选择KeyFullInformation
        KeyInformation:查询的数据指针。
            如果KeyInformationClass是KeyFullInformation
            则该指针指向一个KEY_FULL_INFORMATION的数据结构
        Length:数据长度
        ResultLength:返回的数据长度
* 返回值:查询是否成功
*************************************************************************/
//NTSTATUS ZwQueryKey(
//    IN HANDLE KeyHandle,
//    IN KEY_INFORMATION_CLASS KeyInformationClass,
//    OUT PVOID KeyInformation,
//    IN ULONG Length,
//    OUT PULONG ResultLength
//);
/*
*备注：
*    1、使用ZwQueryKey时，可以将参数KeyInformationClass指定为KeyFullInformation
* 这样参数KeyInformation就对应一个KEY_FULL_INFORMATION数据结构，该数据结构中的
* SubKeys指明了项中有多少个子项
*    2、KEY_FULL_INFORMATION数据结构的大小是变长的，所有要调用两次ZwQueryKey。
* 第一次获取KEY_FULL_INFORMATION数据的长度，第二次真正获取KEY_FULL_INFORMATION
* 数据
*/

/************************************************************************
* 函数名称:ZwEnumerateKey
* 功能描述:针对第几个子项获取该子项的具体信息
* 参数列表:
        KeyHandle:获得的注册表句柄
        Index:很好用到，一般为0
        KeyInformationClass:查询的类别，一般选择KeyFullInformation
        KeyInformation:该子项的信息
        Length:子项信息的长度
        ResultLength:返回子键信息的长度
* 返回值:查询是否成功
*************************************************************************/
//NTSTATUS ZwEnumerateKey(
//    IN HANDLE KeyHandle,
//    IN ULONG Index,
//    IN KEY_INFORMATION_CLASS KeyInformationClass,
//    OUT PVOID KeyInformation,
//    IN ULONG Length,
//    OUT PULONG ResultLength
//);
/*
*备注：
*    1、使用ZwEnumerateKey时，可以将参数KeyInformationClass指定为
* KeybasicInformation
* 这样参数KeyInformation就对应一个KEY_BASIC_INFORMATION数据结构
*    2、KEY_BASIC_INFORMATION数据结构的大小是变长的，所有要调用两次
* ZwEnumerateKey。第一次获取KEY_BASIC_INFORMATION数据的长度，第二
* 次真正获取KEY_BASIC_INFORMATION数据
*/


/*
    测试注册表
*/
void EnumRegedit();