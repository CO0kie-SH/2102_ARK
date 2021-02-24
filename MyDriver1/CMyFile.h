#pragma once
#include <ntifs.h>
#include <ntddk.h>



// ʹ��ָ���ķ�ʽ�������һ��ָ�����ļ���Ŀ¼
HANDLE CreateFile(LPCWSTR pFilePath, ACCESS_MASK Access, BOOLEAN IsFile);

// ��ָ���ļ���д��ָ����С��ָ������
NTSTATUS WriteFile(HANDLE FileHandle, PVOID Buffer, ULONG Length, ULONG pOffset);

// ��ȡָ���ļ��Ļ�����Ϣ���ú�����Ҫ��ȡ�ļ��Ĵ�С
LONGLONG GetFileSize(HANDLE FileHandle);

// ��ָ���ļ���ȡָ����С��ָ������
NTSTATUS ReadFile(HANDLE FileHandle, PVOID Buffer, ULONG Length, ULONG pOffset);

// ɾ���ļ�������Ҫָ���ļ����
NTSTATUS DeleteFile(LPCWSTR pFilePath);