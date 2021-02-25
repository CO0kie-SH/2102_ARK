#include "pch.h"
#include "CR3R0.h"

CR3R0::CR3R0()
	:pMem(0)
{
	puts("CR3R0()");
	DWORD bRet = 0;

	// 可以使用 CreateFile 去打开一个设备对象，要求管理员权限
	DeviceHandle = CreateFile(SYMBOLICLINE_NAMER3, GENERIC_ALL,
		NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (DeviceHandle == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(0, "打开设备对象失败。", 0, 0);
		ExitProcess(1);
	}
	// 向设备对象写入数据
	WriteFile(DeviceHandle, "InitDevice", 10, &bRet, NULL);
	printf("WriteFile(10) > Bytes[%d]\n", bRet);

	this->pcData = &gcData;
	this->pMem = (LPCH)VirtualAlloc(NULL, sizeof(MyInfoSend), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pMem == NULL)
	{
		MessageBoxA(0, "无法创建分页。", 0, 0);
		ExitProcess(2);
	}
}

CR3R0::~CR3R0()
{
}

BOOL CR3R0::GetPIDs(vector<MyProcess3>& vPIDs)
{
	ULONG ulRet = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)this->pMem;
	ZeroMemory(pInfo, 4096);
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	pInfo->ulNum1 = 999999;
	strcpy_s(pInfo->byBuf1, "GetPIDs");
	if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL))
	{
		puts("读取进程数失败");
		return 0;
	}
	printf("读取进程数[%d]\n", ulRet);
	LPMyProcess pPro = (LPMyProcess)pInfo->byBuf3;
	vPIDs.clear();
	for (ULONG i = 0; i < ulRet; i++)
	{
		//printf("%lu [%lu][%lu]\n", i + 1, pPro[i].tPID, pPro[i].pPID);
		vPIDs.push_back({ pPro[i].tPID,pPro[i].pPID });
	}
	return this->GettPID(vPIDs);
}

BOOL CR3R0::GettPID(vector<MyProcess3>& vPIDs)
{
	ULONG ulRet = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)this->pMem;
	ZeroMemory(pInfo, 4096);
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	pInfo->ulNum1 = 999999;
	strcpy_s(pInfo->byBuf1, "GettPID");

	for (ULONG i = 0, max = vPIDs.size(); i < max; i++)
	{
		auto& Info = vPIDs[i];
		//if (Info.tPID != GetCurrentProcessId())
		//	continue;

		pInfo->ulNum1 = Info.tPID;
		if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL)
			|| ulRet == 0)
		{
			printf("读取进程信息失败。%lu\n", Info.tPID);
			continue;
		}
		printf("读取进程信息。[%lu]", Info.tPID);
		wprintf(L"%s\n", (PWCH)pInfo->byBuf3);
		ZeroMemory(pInfo->byBuf3, pInfo->ulBuff);
	}
	return TRUE;
}

BOOL CR3R0::GetTIDs(vector<MyProcess3>& vPIDs)
{
	return 0;
}

BOOL CR3R0::GetMODs(vector<MyProcess3>& vPIDs)
{
	return 0;
}
