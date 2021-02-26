#include "pch.h"
#include "CR3R0.h"

CR3R0::CR3R0()
	:pMem(0)
{
	puts("CR3R0()");
	DWORD bRet = 0;

	// ����ʹ�� CreateFile ȥ��һ���豸����Ҫ�����ԱȨ��
	DeviceHandle = CreateFile(SYMBOLICLINE_NAMER3, GENERIC_ALL,
		NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (DeviceHandle == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(0, "���豸����ʧ�ܡ�", 0, 0);
		ExitProcess(1);
	}
	// ���豸����д������
	WriteFile(DeviceHandle, "InitDevice", 10, &bRet, NULL);
	printf("WriteFile(10) > Bytes[%d]\n", bRet);

	this->pcData = &gcData;
	this->pMem = (LPCH)VirtualAlloc(NULL, sizeof(MyInfoSend), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pMem == NULL)
	{
		MessageBoxA(0, "�޷�������ҳ��", 0, 0);
		ExitProcess(2);
	}
}

CR3R0::~CR3R0()
{
}

BOOL CR3R0::GetPIDs(vector<MyProcess3>& vPIDs)
{
	return 0;
}

BOOL CR3R0::GettPID(vector<MyProcess3>& vPIDs)
{
	return 0;
}

BOOL CR3R0::GetTIDs(vector<MyProcess3>& vPIDs)
{
	DWORD ulRet = 0, ths = 0, pMax = vPIDs.size();
	LPMyInfoSend pInfo = (LPMyInfoSend)this->pMem;
	ZeroMemory(pInfo, 4096);
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
			return FALSE;
		}
		else if (ulRet == 0)	//β������
		{
			printf("�߳�β�������������߳���[%lu]��\n", ths);
			break;
		}
		++ths;
		//printf("%3lu [%6lu][%6lu][%6lu]->%p\n", ths,
		//	pTh->TID, pTh->PID, ulRet, pTh->pETHREAD);
		vTHs.push_back(*pTh);
		pInfo->ulNum2 = ulRet;
	}

	for (ULONG i = 0; i < pMax; i++)
	{
		auto& Pro = vPIDs[i];
		DWORD PID = Pro.tPID;
		printf("����PID[%6lu]\n", PID);
		for (DWORD j = 0; j < ths; j++)
		{
			auto& th = vTHs[j];
			if (th.PID == PID)
			{
				printf("%lu ", th.TID);
				Pro.vTHs.push_back(th);
			}
		}
		printf("\n");
	}
	return TRUE;
}

BOOL CR3R0::GetMODs(vector<MyProcess3>& vPIDs, DWORD PID /*= 0*/)
{
	DWORD i = 0, count = 0, dwRet = 0,
		dwSize = vPIDs.size(), dwPID;
	LPMyInfoSend pInfo = (LPMyInfoSend)this->pMem;
	PWSTR szExe = (PWSTR)pInfo->byBuf3;
	//ZeroMemory(pInfo, 4096);
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	pInfo->ulNum1 = 999999;
	strcpy_s(pInfo->byBuf1, "GetMODs");

	do{
		auto& Info = vPIDs[i];
		dwPID = Info.tPID;
		if (PID && PID != dwPID)	//�жϴ����PID
			continue;
		pInfo->ulNum1 = dwPID;		//PID
		pInfo->ulNum2 = 0;			//��
		count = 0;
		if (PID)	Info.vMDs.clear();
		do {
			//printf("׼����ѯģ�� PID[%lu]\n", dwPID);
			ZeroMemory(pInfo->byBuf3, pInfo->ulBuff);
			StrCpyW(szExe, Info.szExe.GetBuffer());
			if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &dwRet, NULL)
				|| dwRet == 0)
			{
				wprintf(L"��ȡģ����Ϣ%s��%2lu PID[%lu] Next[%lu]%s\n",
					dwRet == 0 ? L"��β" : L"ʧ��",
					count, dwPID, dwRet, szExe);
				break;
			}

			if (++count > 200)
			{
				wprintf(L"��ȡģ����Ϣ���ޡ�%2lu PID[%lu] Next[%lu]%s\n",
					count, dwPID, dwRet, szExe);
				return FALSE;
			}

			if (count == 1 && PID == 0)		//��¼·��
			{
				Info.szPath = szExe;
				Info.pNextMod = dwRet;
				break;
			}
			Info.vMDs.push_back({ pInfo->ulNum2,szExe });
			//wprintf(L"%3lu %lu Next[%08lX] Base[%08lX] %s\n",
			//	count, dwPID, dwRet, pInfo->ulNum2, szExe);
			pInfo->ulNum2 = dwRet;	//��һ��������
		} while (PID);
	} while (++i < dwSize);
	return TRUE;
}
