#pragma once
#include <iostream>
#include <windows.h>

typedef struct _MyInfoSend
{
	ULONG ulSize;
	ULONG ulNum1;
	ULONG ulNum2;
	ULONG ulNum3;
	BYTE byBuf1[40];
	BYTE byBuf2[40];
	BYTE byBuf3[4000];
}MyInfoSend, * LPMyInfoSend;

#define DEVICE_NAME L"\\Device\\MyDevice"
#define SYMBOLICLINE_NAME L"\\??\\MyTestDriver"
//ring3��CreateFile���豸ʱ,��"\\\\.\\MyTestDriver"//�൱����ı���

class CDevice
{
public:
	CDevice();
	~CDevice();

private:
	HANDLE DeviceHandle;
};

