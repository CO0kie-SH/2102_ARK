#include "CDevice.h"


CDevice::CDevice() :pMem(0)
{
	DWORD Bytes = 0;

	// ����ʹ�� CreateFile ȥ��һ���豸����Ҫ�����ԱȨ��
	DeviceHandle = CreateFile(SYMBOLICLINE_NAME, GENERIC_ALL,
		NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	printf("���豸����(%p)",DeviceHandle);
	//Test();
	if (DeviceHandle == INVALID_HANDLE_VALUE)
	{
		printf("ʧ�ܡ�\n");
		return;
	}
	printf("�ɹ���\n");
	// ���豸����д������
	//WriteFile(DeviceHandle, "InitDevice", 10, &Bytes, NULL);
	//printf("\nWriteFile(10) > Bytes[%d]\n", Bytes);

	this->pMem = (LPCH)VirtualAlloc(NULL, sizeof(MyInfoSend), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pMem == NULL)	ExitProcess(2);
	GetPIDs();
}

CDevice::~CDevice()
{
	CloseHandle(DeviceHandle);
	VirtualFree(pMem, 0, MEM_RELEASE);
}

void CDevice::Test()
{
	ULONG Bytes = 0, dwNeedSize = 0;
	if (!NT_SUCCESS(ZwQuerySystemInformation(
		SystemModuleInformation,
		NULL, 0, &dwNeedSize)))
	{

	}
	BYTE* pBuffer = new BYTE[dwNeedSize];
	if (!NT_SUCCESS(ZwQuerySystemInformation(
		SystemModuleInformation,
		(PVOID)pBuffer, dwNeedSize, NULL)))
	{

	}
	PSYSTEM_MODULE_INFORMATION psp= (PSYSTEM_MODULE_INFORMATION)pBuffer;
	for (ULONG i = 0; i < psp->Count; i++)
	{
		auto& Info = psp->Module[i];
		Bytes = 1;
	}
	Bytes = 1;
}

void CDevice::GetPIDs()
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
	ShowPID(Bytes);
}

void CDevice::ShowPID(DWORD Num)
{
	ULONG ulRet = 0, ths = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)pMem;
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	pInfo->ulNum1 = 999999;
	pInfo->ulNum2 = 0;		//��ǰҪ��ѯ���߳�ID
	strcpy_s(pInfo->byBuf1, "GetThID");

	LPMyThread pTh = (LPMyThread)pInfo->byBuf3;
	vector<MyThread> vTHs;
	while (true)
	{
		if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL))
		{
			printf("��ȡ�߳�ʧ��\n");
			return;
		}
		else if (ulRet == 0)	//β������
		{
			printf("β�������������߳���[%lu]��\n", ths);
			break;
		}
		printf("%3lu [%6lu][%6lu][%6lu]->%p\n", ++ths,
			pTh->TID, pTh->PID, ulRet, pTh->pETHREAD);
		vTHs.push_back(*pTh);
		pInfo->ulNum2 = ulRet;
	}

	

	for (ULONG i = 0; i < Num; i++)
	{
		MyProcess2& Pro = mProcess[i];
		printf("����PID[%6lu]\n", Pro.tPID);
		for (ULONG i = 0,PID=Pro.tPID; i < ths; i++)
		{
			auto& th = vTHs[i];
			if (th.PID == PID)
			{
				printf("%6lu ", th.TID);
			}
		}
		printf("\n");
		//auto hProcess = OpenProcess(
		//	PROCESS_QUERY_INFORMATION,
		//	FALSE, Pro.tPID);
		//char filePath[MAX_PATH] = {};
		//DWORD read = MAX_PATH;
		//QueryFullProcessImageNameA(hProcess, 0, filePath, &read);
		//printf("%lu %lu %lu %s\n", i, Pro.tPID, Pro.pPID, filePath);
		//GetMods(Pro);
		//CloseHandle(hProcess);
	}
}

void CDevice::GetMods(MyProcess2& Info)
{
	ULONG Bytes = 0,dwNeedSize = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)pMem;
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	pInfo->ulNum1 = Info.tPID;
	strcpy_s(pInfo->byBuf1, "GetMods");

	if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &Bytes, NULL))
	{
		printf("��ȡģ����ʧ��\n");
		return;
	}
	printf("��ȡģ����[%d]\n", Bytes);
}
