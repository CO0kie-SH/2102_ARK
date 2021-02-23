#include "CDevice.h"


CDevice::CDevice() :DeviceHandle(0)
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
}

CDevice::~CDevice()
{
	CloseHandle(DeviceHandle);
}