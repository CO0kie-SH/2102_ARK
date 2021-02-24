#include "CDisp.h"



// 函数有导出，但是没有声明，需要自己提供
NTKERNELAPI UCHAR* PsGetProcessImageFileName(__in PEPROCESS Process);

VOID ListCurrentProcessAndThread()
{
	// 提供一个 EPROCESS 的指针，用于接受查询到的内容
	//PETHREAD Thread = NULL;
	PEPROCESS Process = NULL;

	// 提供一个用于遍历的范围，以 4 为递增值，暴力遍历所有的进程，由于
	//	进程和线程被放置在了同一个位置，所以两者的 id 是处于同意序列的
	for (ULONG uid = 0, id = 4, *PPID, *pDbg; id <= 5000; id += 4)
	{
		// 尝试使用 pid 找到相应的 EOROCESS 结构体，如果找到就输出信息
		if (NT_SUCCESS(PsLookupProcessByProcessId(ULongToHandle(id), &Process)))
		{
			PPID = (ULONG*)((char*)Process + 0x140);
			pDbg = (ULONG*)((char*)Process + 0x0ec);
			// 通过 windows 提供的内置函数获取名称
			KdPrint(("[%lu][%04lu][%04lu][P]DBG[%lu]EPS[%p]: %s\n", ++uid, id, *PPID,
				*pDbg, Process, PsGetProcessImageFileName(Process)));

			// 如果操作使指针引用计数 +1 了，那么就需要 -1
			ObDereferenceObject(Process);
		}

		// 如果不是进程，还有可能是线程，再进行一次判断
		//else if (NT_SUCCESS(PsLookupThreadByThreadId(ULongToHandle(id), &Thread)))
		//{
		//	// 通过函数函数获取当前线程的所属进程
		//	PEPROCESS Process2 = IoThreadToProcess(Thread);
		//	KdPrint(("[%d][T]:%d\n", id, PsGetProcessId(Process2)));

		//	// 操作完毕以后，需要手动的减少引用计数
		//	ObDereferenceObject(Thread);
		//}

	}
}



ULONG_PTR WriteDisp(LPCH FunName)
{
	ULONG_PTR uRet = 0;
	if (10 == (uRet = RtlCompareMemory(FunName, "InitDevice", 10)))
	{
		KdPrint(("比较: %s %s %lu->初始化环境\n", FunName, "InitDevice", uRet));
		ListCurrentProcessAndThread();
	}
	return uRet;
}


ULONG_PTR ReadDisp(LPCH FunName, LPMyInfoSend pInfo)
{
	ULONG_PTR uRet = RtlCompareMemory(FunName, "GetPIDs", 8);
	if (8 == (uRet = RtlCompareMemory(FunName, "GetPIDs", 8)))
	{
		KdPrint(("比较: %s %s %lu->遍历进程\n", FunName, "GetPIDs", uRet));
		return GetPIDs(pInfo->ulBuff, pInfo->ulNum1, (LPMyProcess)pInfo->byBuf3);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "GetThID", 8)))
	{
		//KdPrint(("比较: %s %s %lu->遍历线程\n", FunName, "GetThID", uRet));
		return GetThID(pInfo->ulBuff,
			pInfo->ulNum1,pInfo->ulNum2, (LPMyThread)pInfo->byBuf3);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "GetMods", 8)))
	{
		//KdPrint(("比较: %s %s %lu->遍历模块\n", FunName, "GetMods", uRet));
		return GetMods(pInfo->ulBuff, pInfo->ulNum1, (LPMyProcess)pInfo->byBuf3);
	}
	return 0;
}

ULONG_PTR GetPIDs(ULONG MaxBuff,ULONG MaxPID, LPMyProcess pPID)
{
	PEPROCESS Process = NULL;
	KdPrint(("[%lu][%04lu][%p]\n", MaxBuff, MaxPID, (char*)pPID));
	ULONG count = 0, tid = 4, maxID = MaxBuff / 4 / 2;
	while (tid <= MaxPID && count <= maxID)
	{
		if (NT_SUCCESS(PsLookupProcessByProcessId(ULongToHandle(tid), &Process)))
		{
			pPID[count].tPID = tid;
			pPID[count].pPID = *((ULONG*)((char*)Process + 0x140));
			count++;
			ObDereferenceObject(Process);
		}
		tid += 4;
	}


	// 提供一个用于遍历的范围，以 4 为递增值，暴力遍历所有的进程，由于
	//	进程和线程被放置在了同一个位置，所以两者的 id 是处于同意序列的
	//for (, *PPID, *pDbg; id <= MaxPID; id += 4)
	//{
	//	// 尝试使用 pid 找到相应的 EOROCESS 结构体，如果找到就输出信息
	//	if (NT_SUCCESS(PsLookupProcessByProcessId(ULongToHandle(id), &Process)))
	//	{
	//		PPID = (ULONG*)((char*)Process + 0x140);
	//		pDbg = (ULONG*)((char*)Process + 0x0ec);
	//		// 通过 windows 提供的内置函数获取名称
	//		KdPrint(("[%lu][%04lu][%04lu][P]DBG[%lu]EPS[%p]: %s\n", ++uid, id, *PPID,
	//			*pDbg, Process, PsGetProcessImageFileName(Process)));

	//		// 如果操作使指针引用计数 +1 了，那么就需要 -1
	//		ObDereferenceObject(Process);
	//	}
	//}
	return count;
}

ULONG_PTR GetThID(ULONG MaxBuff, ULONG MaxTID, ULONG TID, LPMyThread pTID)
{
	PETHREAD Thread = NULL;
	UNREFERENCED_PARAMETER(MaxBuff);
	for (ULONG i = TID, tmp = 0; i < MaxTID; i += 4)
	{
		if (NT_SUCCESS(PsLookupThreadByThreadId(ULongToHandle(i), &Thread)))
		{
			ObDereferenceObject(Thread);
			if (tmp == 0)
			{
				tmp = i;
				// 通过函数函数获取当前线程的所属进程
				PEPROCESS Process2 = IoThreadToProcess(Thread);
				pTID->TID = i;
				pTID->pETHREAD = (PCHAR)Thread;
				pTID->PID = (ULONG)PsGetProcessId(Process2);
				//KdPrint(("[%lu][T][%lu]:%p \n", TID, pTID->PID, pTID->pETHREAD));
			}
			else
			{
				return i;	//返回下一个TID
			}
		}
	}
	return 0;
}


ULONG_PTR GetMods(ULONG MaxBuff, ULONG PID, LPMyProcess pPID)
{
	KdPrint(("比较: %lu %lu %p->遍历模块\n", MaxBuff, PID, pPID));
	return 0;
}
