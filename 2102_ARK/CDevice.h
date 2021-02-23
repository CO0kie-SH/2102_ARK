#pragma once
#include <iostream>
#include <windows.h>

typedef struct _MyInfoSend
{
	ULONG ulSize;
	ULONG ulNum1;
	ULONG ulNum2;
	ULONG ulNum3;
	CHAR byBuf1[40];
	CHAR byBuf2[40];
	CHAR byBuf3[4000];
}MyInfoSend, * LPMyInfoSend;

#define DEVICE_NAME L"\\Device\\MyDevice"
#define SYMBOLICLINE_NAME L"\\\\.\\MyTestDriver"
//ring3用CreateFile打开设备时,用"\\\\.\\MyTestDriver"//相当于起的别名

class CDevice
{
public:
	CDevice();
	~CDevice();

	void Test();
private:
	HANDLE DeviceHandle;
	LPCH	pMem;
};

