#include "CDevice.h"


CDevice::CDevice() :pMem(0)
{
	DWORD Bytes = 0;

	// 可以使用 CreateFile 去打开一个设备对象，要求管理员权限
	DeviceHandle = CreateFile(SYMBOLICLINE_NAME, GENERIC_ALL,
		NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	printf("打开设备对象(%p)",DeviceHandle);
	if (DeviceHandle == INVALID_HANDLE_VALUE)
	{
		printf("失败。\n");
		return;
	}
	// 向设备对象写入数据
	WriteFile(DeviceHandle, "InitDevice", 10, &Bytes, NULL);
	printf("\nWriteFile(10) > Bytes[%d]\n", Bytes);

	this->pMem = (LPCH)VirtualAlloc(NULL, sizeof(MyInfoSend), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pMem == NULL)	ExitProcess(2);
}

CDevice::~CDevice()
{
	CloseHandle(DeviceHandle);
}

void CDevice::Test()
{
	DWORD Bytes = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)pMem;
	pInfo->ulSize = 4096;
	strcpy_s(pInfo->byBuf1, "GetPIDs");

	if (ReadFile(DeviceHandle, pMem, 4096, &Bytes, NULL))
	{
		printf("ReadFile > %s Bytes[%d]\n", pInfo->byBuf1, Bytes);
	}
}
