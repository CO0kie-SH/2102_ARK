#include "CReg.h"

void EnumRegeditSub(LPCWSTR Path)
{
    UNICODE_STRING RegUnicodeString;
    HANDLE hRegister;

    //��ʼ��UNICODE_STRING�ַ���
    RtlInitUnicodeString(&RegUnicodeString,
        Path);

    OBJECT_ATTRIBUTES objectAttributes;
    //��ʼ��objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &RegUnicodeString,
        OBJ_CASE_INSENSITIVE, //�Դ�Сд����
        NULL,
        NULL);

    //��ע���
    NTSTATUS ntStatus = ZwOpenKey(&hRegister,
        KEY_ALL_ACCESS,
        &objectAttributes);

    if (!NT_SUCCESS(ntStatus))
    {
        KdPrint(("Open register ʧ��\n"));
        return;
    }

    ULONG ulSize;
    //��һ�ε���ZwQueryKey,Ϊ�˻�ȡKEY_FULL_INFORMATION���ݵĳ���
    ZwQueryKey(hRegister,
        KeyFullInformation,
        NULL,
        0,
        &ulSize);

    PKEY_FULL_INFORMATION pfi =
        (PKEY_FULL_INFORMATION)ExAllocatePool(PagedPool, ulSize);

    //�ڶ��ε���ZwQueryKey,Ϊ�˻�ȡKEY_FULL_INFORMATION����
    ZwQueryKey(hRegister,
        KeyFullInformation,
        pfi,
        ulSize,
        &ulSize);

    KdPrint(("�Ӽ�����%3lu %S\n", pfi->SubKeys, Path));
    for (ULONG i = 0; i < pfi->Values; i++)
    {
        //ö��ע���
        ZwEnumerateValueKey(hRegister,
            i,
            KeyValueBasicInformation,
            NULL,
            0,
            &ulSize);

        PKEY_VALUE_BASIC_INFORMATION pvbi =
            (PKEY_VALUE_BASIC_INFORMATION)ExAllocatePool(PagedPool, ulSize);
        if (pvbi == 0)
        {
            KdPrint(("�����Ӽ��ڴ����[%lu] %S\n", i, Path));
            continue;
        }
        //ö��ע���
        ZwEnumerateValueKey(hRegister,
            i,
            KeyValueBasicInformation,
            pvbi,
            ulSize,
            &ulSize);
        
        UNICODE_STRING uniKeyName;
        uniKeyName.Length =
            uniKeyName.MaximumLength =
            (USHORT)pvbi->NameLength;
        uniKeyName.Buffer = pvbi->Name;

        char* buff = "NULL";
        if (pvbi->Type == REG_SZ)
            buff = "REG_SZ";
        else if (pvbi->Type == REG_MULTI_SZ)
            buff = "REG_MULTI_SZ";
        else if (pvbi->Type == REG_DWORD)
            buff = "REG_DWORD";
        else if (pvbi->Type == REG_QWORD)
            buff = "REG_QWORD";
        else if (pvbi->Type == REG_BINARY)
            buff = "REG_BINARY";


        KdPrint(("����%3lu [   %-10s] %wZ\n", pfi->SubKeys, buff, &uniKeyName));
        //�����ڴ�
        ExFreePool(pvbi);
    }

    //�����ڴ�
    ExFreePool(pfi);
    //�رվ��
    ZwClose(hRegister);
}


void EnumRegeditDir(PCWSTR Path)
{
    UNICODE_STRING RegUnicodeString;
    HANDLE hRegister;

    //��ʼ��UNICODE_STRING�ַ���
    RtlInitUnicodeString(&RegUnicodeString,
        Path);

    OBJECT_ATTRIBUTES objectAttributes;
    //��ʼ��objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &RegUnicodeString,
        OBJ_CASE_INSENSITIVE, //�Դ�Сд����
        NULL,
        NULL);

    //��ע���
    NTSTATUS ntStatus = ZwOpenKey(&hRegister,
        KEY_ALL_ACCESS,
        &objectAttributes);

    if (!NT_SUCCESS(ntStatus))
    {
        KdPrint(("Open register ʧ��\n"));
        return;
    }
    KdPrint(("Open register �ɹ�[%p]\n", hRegister));

    ULONG ulSize;
    //��һ�ε���ZwQueryKey,Ϊ�˻�ȡKEY_FULL_INFORMATION���ݵĳ���
    ZwQueryKey(hRegister,
        KeyFullInformation,
        NULL,
        0,
        &ulSize);

    PKEY_FULL_INFORMATION pfi =
        (PKEY_FULL_INFORMATION)ExAllocatePool(PagedPool, ulSize);

    //�ڶ��ε���ZwQueryKey,Ϊ�˻�ȡKEY_FULL_INFORMATION����
    ZwQueryKey(hRegister,
        KeyFullInformation,
        pfi,
        ulSize,
        &ulSize);
    if (pfi == 0)
    {
        ZwClose(hRegister);
        KdPrint(("��ѯ register ʧ��\n"));
        return;
    }
    KdPrint(("�������%3lu %S\n", pfi->SubKeys, Path));
    for (ULONG i = 0; i < pfi->SubKeys; i++)
    {
        //��һ�ε���ZwEnumerateKey��Ϊ�˻�ȡKEY_BASIC_INFORMATION���ݵĳ���
        ZwEnumerateKey(hRegister,
            i,
            KeyBasicInformation,
            NULL,
            0,
            &ulSize);

        PKEY_BASIC_INFORMATION pbi =
            (PKEY_BASIC_INFORMATION)ExAllocatePool(PagedPool, ulSize);

        //�ڶ��ε���ZwEnumerateKey��Ϊ�˻�ȡKEY_BASIC_INFORMATION����
        ZwEnumerateKey(hRegister,
            i,
            KeyBasicInformation,
            pbi,
            ulSize,
            &ulSize);
        if (pbi == 0)
        {
            ExFreePool(pbi);
            KdPrint(("��ѯ register Key ʧ��\n"));
            break;
        }
        UNICODE_STRING uniKeyName;
        uniKeyName.Length =
            uniKeyName.MaximumLength =
            (USHORT)pbi->NameLength;
        uniKeyName.Buffer = pbi->Name;

        KdPrint(("�ƴ�%3lu %S%wZ\n", i + 1, Path, &uniKeyName));
        //�����ڴ�
        ExFreePool(pbi);
    }
    KdPrint(("\n"));

    //�����ڴ�
    ExFreePool(pfi);
    //�رվ��
    ZwClose(hRegister);
}

void EnumRegeditCreat(PCWCHAR Path)
{
    // 1. ��ʼ������

    UNICODE_STRING KeyName;
    RtlInitUnicodeString(&KeyName, L"\\Registry\\Machine\\Software\\0Test\\Creat");

    // 2. ��������
    OBJECT_ATTRIBUTES objectAttributes;
    InitializeObjectAttributes(&objectAttributes, &KeyName, OBJ_CASE_INSENSITIVE/*| OBJ_KERNEL_HANDLE*/, NULL, NULL);

    //////////////////
    objectAttributes.Length = sizeof(OBJECT_ATTRIBUTES);
    objectAttributes.Attributes = OBJ_CASE_INSENSITIVE;
    objectAttributes.ObjectName = &KeyName;
    objectAttributes.RootDirectory = NULL;
    objectAttributes.SecurityDescriptor = NULL;
    objectAttributes.SecurityQualityOfService = NULL;
    /////////////////

    HANDLE hRegister = 0;
    NTSTATUS status = ZwCreateKey(
        &hRegister, KEY_READ, &objectAttributes, 0, NULL, 
        REG_OPTION_NON_VOLATILE, NULL);

    KdPrint(("��������%s\n\n", NT_SUCCESS(status) ? "�ɹ�" : "ʧ��"));
    if (hRegister)  ZwClose(hRegister);
    EnumRegeditDir(Path);
}

void EnumRegeditDel(PCWCHAR Path)
{
    // 1. ��ʼ������
    UNICODE_STRING usKeyName;
    RtlInitUnicodeString(&usKeyName, L"\\Registry\\Machine\\Software\\0Test\\Creat");
    OBJECT_ATTRIBUTES objectAttributes;
    InitializeObjectAttributes(&objectAttributes, &usKeyName, OBJ_CASE_INSENSITIVE, NULL, NULL);

    // 2. ��ע���,��ȡ���
    HANDLE hRegister = 0;
    NTSTATUS status = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);
    if (status != STATUS_SUCCESS) return;

    // 3. ɾ��ָ����
    status = ZwDeleteKey(hRegister);
    KdPrint(("ɾ������%s\n\n", NT_SUCCESS(status) ? "�ɹ�" : "ʧ��"));

    // 4. ������Դ
    if (hRegister)ZwClose(hRegister);
    EnumRegeditDir(Path);
}
void EnumRegedit()
{
    PCWCHAR Path = L"\\Registry\\Machine\\Software\\0Test\\";
    EnumRegeditDir(Path);
    EnumRegeditSub(Path);
    EnumRegeditCreat(Path);
    EnumRegeditDel(Path);
}