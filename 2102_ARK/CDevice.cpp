#include "CDevice.h"


CDevice::CDevice() :pMem(0)
{
	DWORD Bytes = 0;

	// ����ʹ�� CreateFile ȥ��һ���豸����Ҫ�����ԱȨ��
	DeviceHandle = CreateFile(SYMBOLICLINE_NAME, GENERIC_ALL,
		NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	printf("���豸����(%p)",DeviceHandle);
	if (DeviceHandle == INVALID_HANDLE_VALUE)
	{
		printf("ʧ�ܡ�\n");
		return;
	}
	// ���豸����д������
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
		printf("��ȡ������ʧ��\n");
		return;
	}
	printf("��ȡ������[%d]\n", Bytes);
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
