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
	DWORD ulRet = 0, ths = 0, pMax = vPIDs.size();
	LPMyInfoSend pInfo = (LPMyInfoSend)this->pMem;
	ZeroMemory(pInfo, 4096);
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	pInfo->ulNum1 = 999999;
	pInfo->ulNum2 = 0;		//当前要查询的线程ID
	strcpy_s(pInfo->byBuf1, "GetThID");

	LPMyThread pTh = (LPMyThread)pInfo->byBuf3;
	vector<MyThread> vTHs;
	while (true)
	{
		if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL))
		{
			printf("读取线程失败\n");
			return FALSE;
		}
		else if (ulRet == 0)	//尾部结束
		{
			printf("线程尾部结束。共有线程数[%lu]。\n", ths);
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
		//printf("进程PID[%6lu]\n", PID);
		for (DWORD j = 0; j < ths; j++)
		{
			auto& th = vTHs[j];
			if (th.PID == PID)
			{
				//printf("%lu ", th.TID);
				Pro.vTHs.push_back(th);
			}
		}
		//printf("\n");
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
		if (PID && PID != dwPID)	//判断传入的PID
			continue;
		pInfo->ulNum1 = dwPID;		//PID
		pInfo->ulNum2 = 0;			//链
		count = 0;
		if (PID)	Info.vMDs.clear();
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
			Info.vMDs.push_back({ pInfo->ulNum2,szExe });
			//wprintf(L"%3lu %lu Next[%08lX] Base[%08lX] %s\n",
			//	count, dwPID, dwRet, pInfo->ulNum2, szExe);
			pInfo->ulNum2 = dwRet;	//下一个链继续
		} while (PID);
	} while (++i < dwSize);
	return TRUE;
}

BOOL CR3R0::GetSyss(vector<MySys>& vSYSs)
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
			printf("读取模块失败\n");
			return FALSE;
		}
		else if (ulRet == 0)	//尾部结束
		{
			printf("驱动尾部结束。共有驱动数[%lu]。\n", count);
			break;
		}
		vSYSs.push_back({ pInfo->ulNum2,ulRet });
		auto& SYS = vSYSs[count];
		SYS.szExe = (PWCH)pInfo->byBuf2;
		SYS.szPath = (PWCH)pInfo->byBuf3;
		count++;
		//printf("%3lu [%lX][%p][%2d]%20S %S\n", count, ulRet, (PCH)pInfo->ulNum2,
		//	pInfo->ulNum1, (PWCH)pInfo->byBuf2, (PWCH)pInfo->byBuf3);
		pInfo->ulNum1 = ulRet;
	}
	return TRUE;
}

BOOL CR3R0::GetPath(vector<MyPath>& vPATHs)
{
	ULONG ulRet = 0, count = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)pMem;
	auto pDir = (PFILE_BOTH_DIR_INFORMATION)pInfo->byBuf3;
	ZeroMemory(pInfo, 4096);
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	strcpy_s(pInfo->byBuf1, "GetPath");
	wsprintfW((LPWSTR)pInfo->byBuf2, L"%s", L"\\??\\C:\\Windows\\");

	while (true)
	{
		if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL))
		{
			printf("读取目录失败\n");
			return FALSE;
		}
		else if (ulRet == 0)	//尾部结束
		{
			printf("目录尾部结束。共有目录数[%lu]。\n", pInfo->ulNum1);
			break;
		}
		//printf("%lu [%lX]->", ulRet, pInfo->ulNum2);
		//wprintf(L"%s\n", pDir->FileName);
		vPATHs.push_back({
			pDir->FileAttributes,
			pDir->CreationTime,
			pDir->FileName});
		ZeroMemory(pInfo->byBuf3, pInfo->ulBuff);
	}
	return TRUE;
}

BOOL CR3R0::GetIDTs()
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
				pIDT->IDT[i].uSelector,// 段选择子
				pIDT->IDT[i].GateType,//类型
				pIDT->IDT[i].DPL);//特权等级
		}
		return;
	}
	printf("读取IDT表失败\n");
}
