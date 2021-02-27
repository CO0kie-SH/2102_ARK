#pragma once
#include <ntifs.h>

// ע�������Ӽ���Ϣ�ṹ��
typedef struct _FIND_REG_SUBKEY_INFO
{
	ULONG serialNumber;  // ���
	WCHAR regParent[266];  // ����
	WCHAR regValue[266];  // �Ӽ�����

	ULONG status;  // ���Է���
}FIND_REG_SUBKEY_INFO, * PFIND_REG_SUBKEY_INFO;

// ע��������ֵ��Ϣ�ṹ��
typedef struct _FIND_REG_VALUE_INFO
{
	ULONG serialNumber;  // ���
	WCHAR regParent[266];  // ����
	WCHAR regValue[266];  // ֵ
	ULONG regType;  // ����

	ULONG status;  // ���Է���
}FIND_REG_VALUE_INFO, * PFIND_REG_VALUE_INFO;



/************************************************************************
* ��������:ZwQueryKey
* ��������:���ĳע�������ж��ٸ�����
* �����б�:
        KeyHandle:��õ�ע�����
        KeyInformationClass:��ѯ�����һ��ѡ��KeyFullInformation
        KeyInformation:��ѯ������ָ�롣
            ���KeyInformationClass��KeyFullInformation
            ���ָ��ָ��һ��KEY_FULL_INFORMATION�����ݽṹ
        Length:���ݳ���
        ResultLength:���ص����ݳ���
* ����ֵ:��ѯ�Ƿ�ɹ�
*************************************************************************/
//NTSTATUS ZwQueryKey(
//    IN HANDLE KeyHandle,
//    IN KEY_INFORMATION_CLASS KeyInformationClass,
//    OUT PVOID KeyInformation,
//    IN ULONG Length,
//    OUT PULONG ResultLength
//);
/*
*��ע��
*    1��ʹ��ZwQueryKeyʱ�����Խ�����KeyInformationClassָ��ΪKeyFullInformation
* ��������KeyInformation�Ͷ�Ӧһ��KEY_FULL_INFORMATION���ݽṹ�������ݽṹ�е�
* SubKeysָ���������ж��ٸ�����
*    2��KEY_FULL_INFORMATION���ݽṹ�Ĵ�С�Ǳ䳤�ģ�����Ҫ��������ZwQueryKey��
* ��һ�λ�ȡKEY_FULL_INFORMATION���ݵĳ��ȣ��ڶ���������ȡKEY_FULL_INFORMATION
* ����
*/

/************************************************************************
* ��������:ZwEnumerateKey
* ��������:��Եڼ��������ȡ������ľ�����Ϣ
* �����б�:
        KeyHandle:��õ�ע�����
        Index:�ܺ��õ���һ��Ϊ0
        KeyInformationClass:��ѯ�����һ��ѡ��KeyFullInformation
        KeyInformation:���������Ϣ
        Length:������Ϣ�ĳ���
        ResultLength:�����Ӽ���Ϣ�ĳ���
* ����ֵ:��ѯ�Ƿ�ɹ�
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
*��ע��
*    1��ʹ��ZwEnumerateKeyʱ�����Խ�����KeyInformationClassָ��Ϊ
* KeybasicInformation
* ��������KeyInformation�Ͷ�Ӧһ��KEY_BASIC_INFORMATION���ݽṹ
*    2��KEY_BASIC_INFORMATION���ݽṹ�Ĵ�С�Ǳ䳤�ģ�����Ҫ��������
* ZwEnumerateKey����һ�λ�ȡKEY_BASIC_INFORMATION���ݵĳ��ȣ��ڶ�
* ��������ȡKEY_BASIC_INFORMATION����
*/


/*
    ����ע���
*/
void EnumRegedit();