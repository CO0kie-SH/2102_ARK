#pragma once
#include <iostream>
#include <windows.h>
#include <vector>
using std::vector;

#include "Psapi.h"
#pragma comment (lib,"Psapi.lib")


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

typedef struct _MyProcess2
{
	ULONG tPID;
	ULONG pPID;
	CHAR  Path[MAX_PATH];
}MyProcess2, * LPMyProcess2;

#define DEVICE_NAME L"\\Device\\MyDevice"
#define SYMBOLICLINE_NAME L"\\\\.\\MyTestDriver"
//ring3用CreateFile打开设备时,用"\\\\.\\MyTestDriver"//相当于起的别名

class CDevice
{
public:
	CDevice();
	~CDevice();

	void Test();
	void ShowPID(DWORD Num);
private:
	HANDLE DeviceHandle;
	LPCH	pMem;
	vector<MyProcess2> mProcess;
};

