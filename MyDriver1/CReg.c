#include "CReg.h"

void EnumRegeditSub(LPCWSTR Path)
{
    UNICODE_STRING RegUnicodeString;
    HANDLE hRegister;

    //初始化UNICODE_STRING字符串
    RtlInitUnicodeString(&RegUnicodeString,
        Path);

    OBJECT_ATTRIBUTES objectAttributes;
    //初始化objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &RegUnicodeString,
        OBJ_CASE_INSENSITIVE, //对大小写敏感
        NULL,
        NULL);

    //打开注册表
    NTSTATUS ntStatus = ZwOpenKey(&hRegister,
        KEY_ALL_ACCESS,
        &objectAttributes);

    if (!NT_SUCCESS(ntStatus))
    {
        KdPrint(("Open register 失败\n"));
        return;
    }

    ULONG ulSize;
    //第一次调用ZwQueryKey,为了获取KEY_FULL_INFORMATION数据的长度
    ZwQueryKey(hRegister,
        KeyFullInformation,
        NULL,
        0,
        &ulSize);

    PKEY_FULL_INFORMATION pfi =
        (PKEY_FULL_INFORMATION)ExAllocatePool(PagedPool, ulSize);

    //第二次调用ZwQueryKey,为了获取KEY_FULL_INFORMATION数据
    ZwQueryKey(hRegister,
        KeyFullInformation,
        pfi,
        ulSize,
        &ulSize);

    KdPrint(("子键计数%3lu %S\n", pfi->SubKeys, Path));
    for (ULONG i = 0; i < pfi->Values; i++)
    {
        //枚举注册表
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
            KdPrint(("分配子键内存错误。[%lu] %S\n", i, Path));
            continue;
        }
        //枚举注册表
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


        KdPrint(("计数%3lu [   %-10s] %wZ\n", pfi->SubKeys, buff, &uniKeyName));
        //回收内存
        ExFreePool(pvbi);
    }

    //回收内存
    ExFreePool(pfi);
    //关闭句柄
    ZwClose(hRegister);
}


void EnumRegeditDir(PCWSTR Path)
{
    UNICODE_STRING RegUnicodeString;
    HANDLE hRegister;

    //初始化UNICODE_STRING字符串
    RtlInitUnicodeString(&RegUnicodeString,
        Path);

    OBJECT_ATTRIBUTES objectAttributes;
    //初始化objectAttributes
    InitializeObjectAttributes(&objectAttributes,
        &RegUnicodeString,
        OBJ_CASE_INSENSITIVE, //对大小写敏感
        NULL,
        NULL);

    //打开注册表
    NTSTATUS ntStatus = ZwOpenKey(&hRegister,
        KEY_ALL_ACCESS,
        &objectAttributes);

    if (!NT_SUCCESS(ntStatus))
    {
        KdPrint(("Open register 失败\n"));
        return;
    }
    KdPrint(("Open register 成功[%p]\n", hRegister));

    ULONG ulSize;
    //第一次调用ZwQueryKey,为了获取KEY_FULL_INFORMATION数据的长度
    ZwQueryKey(hRegister,
        KeyFullInformation,
        NULL,
        0,
        &ulSize);

    PKEY_FULL_INFORMATION pfi =
        (PKEY_FULL_INFORMATION)ExAllocatePool(PagedPool, ulSize);

    //第二次调用ZwQueryKey,为了获取KEY_FULL_INFORMATION数据
    ZwQueryKey(hRegister,
        KeyFullInformation,
        pfi,
        ulSize,
        &ulSize);
    if (pfi == 0)
    {
        ZwClose(hRegister);
        KdPrint(("查询 register 失败\n"));
        return;
    }
    KdPrint(("子项计数%3lu %S\n", pfi->SubKeys, Path));
    for (ULONG i = 0; i < pfi->SubKeys; i++)
    {
        //第一次调用ZwEnumerateKey，为了获取KEY_BASIC_INFORMATION数据的长度
        ZwEnumerateKey(hRegister,
            i,
            KeyBasicInformation,
            NULL,
            0,
            &ulSize);

        PKEY_BASIC_INFORMATION pbi =
            (PKEY_BASIC_INFORMATION)ExAllocatePool(PagedPool, ulSize);

        //第二次调用ZwEnumerateKey，为了获取KEY_BASIC_INFORMATION数据
        ZwEnumerateKey(hRegister,
            i,
            KeyBasicInformation,
            pbi,
            ulSize,
            &ulSize);
        if (pbi == 0)
        {
            ExFreePool(pbi);
            KdPrint(("查询 register Key 失败\n"));
            break;
        }
        UNICODE_STRING uniKeyName;
        uniKeyName.Length =
            uniKeyName.MaximumLength =
            (USHORT)pbi->NameLength;
        uniKeyName.Buffer = pbi->Name;

        KdPrint(("计次%3lu %S%wZ\n", i + 1, Path, &uniKeyName));
        //回收内存
        ExFreePool(pbi);
    }
    KdPrint(("\n"));

    //回收内存
    ExFreePool(pfi);
    //关闭句柄
    ZwClose(hRegister);
}

void EnumRegeditCreat(PCWCHAR Path)
{
    // 1. 初始化变量

    UNICODE_STRING KeyName;
    RtlInitUnicodeString(&KeyName, L"\\Registry\\Machine\\Software\\0Test\\Creat");

    // 2. 创建子项
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

    KdPrint(("创建子项%s\n\n", NT_SUCCESS(status) ? "成功" : "失败"));
    if (hRegister)  ZwClose(hRegister);
    EnumRegeditDir(Path);
}

void EnumRegeditDel(PCWCHAR Path)
{
    // 1. 初始化变量
    UNICODE_STRING usKeyName;
    RtlInitUnicodeString(&usKeyName, L"\\Registry\\Machine\\Software\\0Test\\Creat");
    OBJECT_ATTRIBUTES objectAttributes;
    InitializeObjectAttributes(&objectAttributes, &usKeyName, OBJ_CASE_INSENSITIVE, NULL, NULL);

    // 2. 打开注册表,获取句柄
    HANDLE hRegister = 0;
    NTSTATUS status = ZwOpenKey(&hRegister, KEY_ALL_ACCESS, &objectAttributes);
    if (status != STATUS_SUCCESS) return;

    // 3. 删除指定键
    status = ZwDeleteKey(hRegister);
    KdPrint(("删除子项%s\n\n", NT_SUCCESS(status) ? "成功" : "失败"));

    // 4. 清理资源
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