#include "CDevice.h"


CDevice::CDevice() :pMem(0)
{
	DWORD Bytes = 0;

	// 可以使用 CreateFile 去打开一个设备对象，要求管理员权限
	DeviceHandle = CreateFile(SYMBOLICLINE_NAME, GENERIC_ALL,
		NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	printf("打开设备对象(%p)",DeviceHandle);
	//Test();
	if (DeviceHandle == INVALID_HANDLE_VALUE)
	{
		printf("失败。\n");
		return;
	}
	printf("成功。\n");
	// 向设备对象写入数据
	WriteFile(DeviceHandle, "InitDevice", 10, &Bytes, NULL);
	printf("\nWriteFile(10) > Bytes[%d]\n", Bytes);

	this->pMem = (LPCH)VirtualAlloc(NULL, sizeof(MyInfoSend), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pMem == NULL)	ExitProcess(2);
	//this->GetPIDs();
	//this->GetSyss();
	this->EnumFiles();
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
		printf("进程PID[%06lu]\n", Pro.tPID);
		if (Pro.tPID != GetCurrentProcessId())
			continue;
		pInfo->ulNum1 = Pro.tPID;
		pInfo->ulNum2 = 0;			//获取的链
		while (true)
		{
			if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL))
			{
				printf("读取模块失败\n");
				return;
			}
			else if (ulRet == 0)	//尾部结束
			{
				printf("模块尾部结束。共有模块数[%lu]。\n", count);
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
	pInfo->ulNum2 = 0;		//当前要查询的线程ID
	strcpy_s(pInfo->byBuf1, "GetThID");

	LPMyThread pTh = (LPMyThread)pInfo->byBuf3;
	vector<MyThread> vTHs;
	while (true)
	{
		if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL))
		{
			printf("读取线程失败\n");
			return;
		}
		else if (ulRet == 0)	//尾部结束
		{
			printf("线程尾部结束。共有线程数[%lu]。\n", ths);
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
		printf("进程PID[%6lu]\n", Pro.tPID);
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
			printf("读取模块失败\n");
			return;
		}
		else if (ulRet == 0)	//尾部结束
		{
			printf("驱动尾部结束。共有驱动数[%lu]。\n", count);
			break;
		}
		printf("%3lu [%lX][%p][%2d]%20S %S\n", ++count, ulRet, (PCH)pInfo->ulNum2,
			pInfo->ulNum1, (PWCH)pInfo->byBuf2, (PWCH)pInfo->byBuf3);
		pInfo->ulNum1 = ulRet;
	}
}

void CDevice::EnumFiles()
{

}