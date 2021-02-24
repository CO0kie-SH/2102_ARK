#pragma once
#include <ntifs.h>
#include <ntddk.h>



// 使用指定的方式创建或打开一个指定的文件或目录
HANDLE CreateFile(LPCWSTR pFilePath, ACCESS_MASK Access, BOOLEAN IsFile);

// 向指定文件内写入指定大小的指定数据
NTSTATUS WriteFile(HANDLE FileHandle, PVOID Buffer, ULONG Length, ULONG pOffset);

// 获取指定文件的基本信息，该函数主要获取文件的大小
LONGLONG GetFileSize(HANDLE FileHandle);

// 从指定文件读取指定大小的指定数据
NTSTATUS ReadFile(HANDLE FileHandle, PVOID Buffer, ULONG Length, ULONG pOffset);

// 删除文件，不需要指定文件句柄
NTSTATUS DeleteFile(LPCWSTR pFilePath);