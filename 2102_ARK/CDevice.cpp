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
	//WriteFile(DeviceHandle, "InitDevice", 10, &Bytes, NULL);
	//printf("\nWriteFile(10) > Bytes[%d]\n", Bytes);

	this->pMem = (LPCH)VirtualAlloc(NULL, sizeof(MyInfoSend), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pMem == NULL)	ExitProcess(2);
}

CDevice::~CDevice()
{
	CloseHandle(DeviceHandle);
	VirtualFree(pMem, 0, 0);
}

void CDevice::Test()
{
	ULONG Bytes = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)pMem;
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	pInfo->ulNum1 = 999999;
	strcpy_s(pInfo->byBuf1, "GetPIDs");

	if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &Bytes, NULL))
	{
		printf("读取进程数失败\n");
		return;
	}
	printf("读取进程数[%d]\n", Bytes);
	LPMyProcess pPro = (LPMyProcess)pInfo->byBuf3;
	mProcess.clear();
	for (ULONG i = 0; i < Bytes; i++)
	{
		mProcess.push_back({ pPro[i].tPID,pPro[i].pPID });
	}

}

void CDevice::ShowPID(DWORD Num)
{
	for (ULONG i = 0; i < Num; i++)
	{
		MyProcess2& Pro = mProcess[i];
		auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE, Pro.tPID);

		CloseHandle(hProcess);
	}
}
