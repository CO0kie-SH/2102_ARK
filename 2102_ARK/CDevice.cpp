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
	WriteFile(DeviceHandle, "InitDevice", 10, &Bytes, NULL);
	printf("\nWriteFile(10) > Bytes[%d]\n", Bytes);

	this->pMem = (LPCH)VirtualAlloc(NULL, sizeof(MyInfoSend), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pMem == NULL)	ExitProcess(2);
	//this->GetPIDs();
	//this->GetSyss();
	//this->EnumPath();
	//this->GetIDTs();
	//this->GetGDTs();
}

CDevice::~CDevice()
{
	CloseHandle(DeviceHandle);
	VirtualFree(pMem, 0, MEM_RELEASE);
}

void CDevice::Test()
{
	
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
	//GetMods(Bytes, pInfo);
	//GetThId(Bytes, pInfo);
}

void CDevice::GetMods(DWORD Num, LPMyInfoSend pInfo)
{
	ULONG ulRet = 0, ths = 0;
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	strcpy_s(pInfo->byBuf1, "GetMods");

	for (ULONG i = 0, count = 0; i < Num; i++)
	{
		MyProcess2& Pro = mProcess[i];
		printf("����PID[%06lu]\n", Pro.tPID);
		if (Pro.tPID != GetCurrentProcessId())
			continue;
		pInfo->ulNum1 = Pro.tPID;
		pInfo->ulNum2 = 0;			//��ȡ����
		while (true)
		{
			if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL))
			{
				printf("��ȡģ��ʧ��\n");
				return;
			}
			else if (ulRet == 0)	//β������
			{
				printf("ģ��β������������ģ����[%lu]��\n", count);
				break;
			}
			printf("%lu [%08lX][%p] ->%S\n", ++count,
				pInfo->ulNum2, (PCH)ulRet, (PWCHAR)pInfo->byBuf3);
			pInfo->ulNum2 = ulRet;
		}
	}
}

void CDevice::GetThId(DWORD Num, LPMyInfoSend pInfo)
{
	ULONG ulRet = 0, ths = 0;
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
			printf("�߳�β�������������߳���[%lu]��\n", ths);
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
				Pro.vTHs.push_back(th);
			}
		}
		printf("\n");
	}
}

void CDevice::GetSyss()
{
	ULONG ulRet = 0, count = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)pMem;
	ZeroMemory(pInfo, 4096);
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	strcpy_s(pInfo->byBuf1, "GetSyss");

	while (true)
	{
		if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL))
		{
			printf("��ȡģ��ʧ��\n");
			return;
		}
		else if (ulRet == 0)	//β������
		{
			printf("����β������������������[%lu]��\n", count);
			break;
		}
		printf("%3lu [%lX][%p][%2d]%20S %S\n", ++count, ulRet, (PCH)pInfo->ulNum2,
			pInfo->ulNum1, (PWCH)pInfo->byBuf2, (PWCH)pInfo->byBuf3);
		pInfo->ulNum1 = ulRet;
	}
}

void CDevice::EnumPath()
{
	ULONG ulRet = 0, count = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)pMem;
	auto pDir = (PFILE_BOTH_DIR_INFORMATION)pInfo->byBuf3;
	ZeroMemory(pInfo, 4096);
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	strcpy_s(pInfo->byBuf1, "GetPath");
	wsprintfW((LPWSTR)pInfo->byBuf2, L"%s", L"\\??\\C:\\Users\\");

	while (true)
	{
		if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL))
		{
			printf("��ȡĿ¼ʧ��\n");
			return;
		}
		else if (ulRet == 0)	//β������
		{
			printf("Ŀ¼β������������Ŀ¼��[%lu]��\n", pInfo->ulNum1);
			break;
		}
		printf("%lu [%lX]->", ulRet, pInfo->ulNum2);
		wprintf(L"%s\n", pDir->FileName);
		ZeroMemory(pInfo->byBuf3, pInfo->ulBuff);
	}
}

void CDevice::GetIDTs()
{
	ULONG ulRet = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)pMem;
	auto pIDT = (LPMyIDT)pInfo->byBuf3;
	ZeroMemory(pInfo, sizeof(MyInfoSend));
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	strcpy_s(pInfo->byBuf1, "GetIDTs");

	if (ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL)
		&& ulRet == 0x100)
	{
		for (ULONG i = 0; i < 0x100; i++)
		{
			printf("%3lu Addr[%p] selector: %d, GateType:%d, DPL: %d\n", 
				i + 1, pIDT->Addr[i],
				pIDT->IDT[i].uSelector,// ��ѡ����
				pIDT->IDT[i].GateType,//����
				pIDT->IDT[i].DPL);//��Ȩ�ȼ�
		}
		return;
	}
	printf("��ȡIDT��ʧ��\n");
}

void CDevice::GetGDTs()
{
	ULONG ulRet = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)pMem;
	auto pGDT = (LPMyGDT)pInfo->byBuf3;
	ZeroMemory(pInfo, sizeof(MyInfoSend));
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	strcpy_s(pInfo->byBuf1, "GetGDTs");

	if (ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL)
		&& ulRet > 0)
	{
		for (ULONG i = 0; i < ulRet; i++)
		{
			PSHORT sGDT= (PSHORT)&pGDT->uGDT[i];
			PGDT_ENTRY eGDT = (PGDT_ENTRY)sGDT;

			ULONG Addr = MAKELONG(sGDT[0], sGDT[3]);
			UCHAR P = eGDT->P,
				S = eGDT->S,
				G = eGDT->G;
			printf("%3lu [%p] %016llX ��ƫ�� 0x%08X P=%d S=%d G=%d\n",
				i + 1, pGDT->Addr[i], pGDT->uGDT[i],
				Addr, P, S, G);
		}
		return;
	}
	printf("��ȡGDT��ʧ��\n");
}
