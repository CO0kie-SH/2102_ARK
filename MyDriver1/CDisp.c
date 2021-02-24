#include "CDisp.h"



// �����е���������û����������Ҫ�Լ��ṩ
NTKERNELAPI UCHAR* PsGetProcessImageFileName(__in PEPROCESS Process);

VOID ListCurrentProcessAndThread()
{
	// �ṩһ�� EPROCESS ��ָ�룬���ڽ��ܲ�ѯ��������
	//PETHREAD Thread = NULL;
	PEPROCESS Process = NULL;

	// �ṩһ�����ڱ����ķ�Χ���� 4 Ϊ����ֵ�������������еĽ��̣�����
	//	���̺��̱߳���������ͬһ��λ�ã��������ߵ� id �Ǵ���ͬ�����е�
	for (ULONG uid = 0, id = 4, *PPID, *pDbg; id <= 5000; id += 4)
	{
		// ����ʹ�� pid �ҵ���Ӧ�� EOROCESS �ṹ�壬����ҵ��������Ϣ
		if (NT_SUCCESS(PsLookupProcessByProcessId(ULongToHandle(id), &Process)))
		{
			PPID = (ULONG*)((char*)Process + 0x140);
			pDbg = (ULONG*)((char*)Process + 0x0ec);
			// ͨ�� windows �ṩ�����ú�����ȡ����
			KdPrint(("[%lu][%04lu][%04lu][P]DBG[%lu]EPS[%p]: %s\n", ++uid, id, *PPID,
				*pDbg, Process, PsGetProcessImageFileName(Process)));

			// �������ʹָ�����ü��� +1 �ˣ���ô����Ҫ -1
			ObDereferenceObject(Process);
		}

		// ������ǽ��̣����п������̣߳��ٽ���һ���ж�
		//else if (NT_SUCCESS(PsLookupThreadByThreadId(ULongToHandle(id), &Thread)))
		//{
		//	// ͨ������������ȡ��ǰ�̵߳���������
		//	PEPROCESS Process2 = IoThreadToProcess(Thread);
		//	KdPrint(("[%d][T]:%d\n", id, PsGetProcessId(Process2)));

		//	// ��������Ժ���Ҫ�ֶ��ļ������ü���
		//	ObDereferenceObject(Thread);
		//}

	}
}



ULONG_PTR WriteDisp(LPCH FunName)
{
	ULONG_PTR uRet = 0;
	if (10 == (uRet = RtlCompareMemory(FunName, "InitDevice", 10)))
	{
		KdPrint(("�Ƚ�: %s %s %lu->��ʼ������\n", FunName, "InitDevice", uRet));
		ListCurrentProcessAndThread();
	}
	return uRet;
}


ULONG_PTR ReadDisp(LPCH FunName, LPMyInfoSend pInfo)
{
	ULONG_PTR uRet = RtlCompareMemory(FunName, "GetPIDs", 8);
	if (8 == (uRet = RtlCompareMemory(FunName, "GetPIDs", 8)))
	{
		KdPrint(("�Ƚ�: %s %s %lu->��������\n", FunName, "GetPIDs", uRet));
		return GetPIDs(pInfo->ulBuff, pInfo->ulNum1, (LPMyProcess)pInfo->byBuf3);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "GetThID", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->�����߳�\n", FunName, "GetThID", uRet));
		return GetThID(pInfo->ulBuff,
			pInfo->ulNum1,pInfo->ulNum2, (LPMyThread)pInfo->byBuf3);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "GetMods", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->����ģ��\n", FunName, "GetMods", uRet));
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


	// �ṩһ�����ڱ����ķ�Χ���� 4 Ϊ����ֵ�������������еĽ��̣�����
	//	���̺��̱߳���������ͬһ��λ�ã��������ߵ� id �Ǵ���ͬ�����е�
	//for (, *PPID, *pDbg; id <= MaxPID; id += 4)
	//{
	//	// ����ʹ�� pid �ҵ���Ӧ�� EOROCESS �ṹ�壬����ҵ��������Ϣ
	//	if (NT_SUCCESS(PsLookupProcessByProcessId(ULongToHandle(id), &Process)))
	//	{
	//		PPID = (ULONG*)((char*)Process + 0x140);
	//		pDbg = (ULONG*)((char*)Process + 0x0ec);
	//		// ͨ�� windows �ṩ�����ú�����ȡ����
	//		KdPrint(("[%lu][%04lu][%04lu][P]DBG[%lu]EPS[%p]: %s\n", ++uid, id, *PPID,
	//			*pDbg, Process, PsGetProcessImageFileName(Process)));

	//		// �������ʹָ�����ü��� +1 �ˣ���ô����Ҫ -1
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
				// ͨ������������ȡ��ǰ�̵߳���������
				PEPROCESS Process2 = IoThreadToProcess(Thread);
				pTID->TID = i;
				pTID->pETHREAD = (PCHAR)Thread;
				pTID->PID = (ULONG)PsGetProcessId(Process2);
				//KdPrint(("[%lu][T][%lu]:%p \n", TID, pTID->PID, pTID->pETHREAD));
			}
			else
			{
				return i;	//������һ��TID
			}
		}
	}
	return 0;
}


ULONG_PTR GetMods(ULONG MaxBuff, ULONG PID, LPMyProcess pPID)
{
	KdPrint(("�Ƚ�: %lu %lu %p->����ģ��\n", MaxBuff, PID, pPID));
	return 0;
}
