#pragma once
#include <iostream>
#include <windows.h>
#include <vector>
#include <map>
using std::vector;
using std::map;

#include "Psapi.h"
#pragma comment (lib,"Psapi.lib")
#include "ZwQuerySystemInformation.h"

typedef struct _MyInfoSend
{
	ULONG ulSize;
	ULONG ulBuff;
	ULONG ulNum1;
	ULONG ulNum2;
	CHAR byBuf1[40];
	CHAR byBuf2[40];
	CHAR byBuf3[4000];
}MyInfoSend, * LPMyInfoSend;

typedef struct _MyProcess
{
	ULONG tPID;
	ULONG pPID;
}MyProcess, * LPMyProcess;


typedef struct _MyThread
{
	ULONG TID;
	ULONG PID;
	PCHAR pETHREAD;
}MyThread, * LPMyThread;

typedef struct _MyProcess2
{
	ULONG tPID;		//����ID
	ULONG pPID;		//������
	ULONG ulThs;	//�߳�����
	ULONG ulMod;	//ģ������
	PCHAR szExe;	//Exe��
	CHAR  Path[MAX_PATH];	//����·��
	vector<MyThread> vTHs;	//�߳�����
}MyProcess2, * LPMyProcess2;

typedef struct _FILE_BOTH_DIR_INFORMATION {
	ULONG NextEntryOffset;
	ULONG FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG FileAttributes;
	ULONG FileNameLength;
	ULONG EaSize;
	CCHAR ShortNameLength;
	WCHAR ShortName[12];
	_Field_size_bytes_(FileNameLength) WCHAR FileName[1];
} FILE_BOTH_DIR_INFORMATION, * PFILE_BOTH_DIR_INFORMATION;

#define DEVICE_NAME L"\\Device\\MyDevice"
#define SYMBOLICLINE_NAME L"\\\\.\\MyTestDriver"
//ring3��CreateFile���豸ʱ,��"\\\\.\\MyTestDriver"//�൱����ı���

class CDevice
{
public:
	CDevice();
	~CDevice();

	void Test();
	void GetSyss();
	void GetPIDs();
	void GetThId(DWORD Num, LPMyInfoSend pInfo);
	void GetMods(DWORD Num, LPMyInfoSend pInfo);
	void EnumPath();
private:
	HANDLE DeviceHandle;
	LPCH	pMem;
	vector<MyProcess2> mProcess;
};

