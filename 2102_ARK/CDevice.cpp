#include "CDevice.h"


CDevice::CDevice() :DeviceHandle(0)
{
	DWORD Bytes = 0;

	// ����ʹ�� CreateFile ȥ��һ���豸����Ҫ�����ԱȨ��
	DeviceHandle = CreateFile(L"\\\\.\\.min", GENERIC_ALL,
		NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	printf("���豸����(%p)",DeviceHandle);
	if (DeviceHandle == INVALID_HANDLE_VALUE)
	{
		printf("ʧ�ܡ�\n");
		return;
	}
	// ���豸����д������
	WriteFile(DeviceHandle, "123456 r3", 10, &Bytes, NULL);
	printf("WriteFile(10) > Bytes[%d]\n", Bytes);
}

CDevice::~CDevice()
{
	CloseHandle(DeviceHandle);
}