// 2102_ARK.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "CDevice.h"

int main()
{
    std::cout << "Hello World!\n";
    printf("size=%d\n", sizeof(MyInfoSend));
    CDevice cDevice;

	//PSYSTEM_PROCESSES psp = NULL;
	//DWORD dwNeedSize = 0;
	//NTSTATUS status = ZwQuerySystemInformation(
	//	SystemProcessesAndThreadsInformation,
	//	NULL, 0, &dwNeedSize);
	//if (status == STATUS_INFO_LENGTH_MISMATCH)
	//{
	//	BYTE* pBuffer = new BYTE[dwNeedSize];
	//	status = ZwQuerySystemInformation(
	//		SystemProcessesAndThreadsInformation,
	//		(PVOID)pBuffer, dwNeedSize, NULL);
	//	if (status == STATUS_SUCCESS)
	//	{
	//		psp = (PSYSTEM_PROCESSES)pBuffer;
	//		do {
	//			printf("PID=%-4d", psp->ProcessId);
	//			wprintf(L" %s\n", psp->ProcessName.Buffer);
	//			psp = (PSYSTEM_PROCESSES)((ULONG)psp + psp->NextEntryDelta);
	//		} while (psp->NextEntryDelta != 0);
	//	}
	//	delete[]pBuffer;
	//	pBuffer = NULL;
	//}

    system("pause");
}