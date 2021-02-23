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
	for (ULONG uid = 0, id = 0, *PPID, *pDbg; id <= 5000; id += 4)
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
	if (8 == uRet)
	{
		KdPrint(("�Ƚ�: %s %s %lu->��������\n", FunName, "GetPIDs", uRet));
		pInfo->ulNum1 = 1;
		return 4096;
	}
	return uRet;
}