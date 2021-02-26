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
	return 0;
}

BOOL CR3R0::GettPID(vector<MyProcess3>& vPIDs)
{
	return 0;
}

BOOL CR3R0::GetTIDs(vector<MyProcess3>& vPIDs)
{
	return 0;
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
		if (PID && PID != dwPID)	//判断传入的PID
			continue;
		pInfo->ulNum1 = dwPID;		//PID
		pInfo->ulNum2 = 0;			//链
		count = 0;
		if (PID)
		{
			Info.vMDs.clear();
		}
		//if (dwPID != GetCurrentProcessId())
		//	continue;
		do {
			//printf("准备查询模块 PID[%lu]\n", dwPID);
			ZeroMemory(pInfo->byBuf3, pInfo->ulBuff);
			StrCpyW(szExe, Info.szExe.GetBuffer());
			if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &dwRet, NULL)
				|| dwRet == 0)
			{
				wprintf(L"读取模块信息%s。%2lu PID[%lu] Next[%lu]%s\n",
					dwRet == 0 ? L"结尾" : L"失败",
					count, dwPID, dwRet, szExe);
				break;
			}

			if (++count > 200)
			{
				wprintf(L"读取模块信息超限。%2lu PID[%lu] Next[%lu]%s\n",
					count, dwPID, dwRet, szExe);
				return FALSE;
			}

			if (count == 1 && PID == 0)		//记录路径
			{
				Info.szPath = szExe;
				Info.pNextMod = dwRet;
				break;
			}
			
			wprintf(L"%3lu %lu Next[%08lX] Base[%08lX] %s\n",
				count, dwPID, dwRet, pInfo->ulNum2, szExe);
			pInfo->ulNum2 = dwRet;	//下一个链继续
		} while (PID);
	} while (++i < dwSize);
	return TRUE;
}
