#include "CDisp.h"

/*
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
*/

PDRIVER_OBJECT gDriverObject = 0;
PVOID gSysFirst = 0;

ULONG_PTR WriteDisp(LPCH FunName)
{
	ULONG_PTR uRet = 0;
	if (10 == (uRet = RtlCompareMemory(FunName, "InitDevice", 10)))
	{
		KdPrint(("�Ƚ�: %s %s %lu->��ʼ������\n", FunName, "InitDevice", uRet));
		//ListCurrentProcessAndThread();
		// ժ��
		//current->InLoadOrderLinks.Flink->Blink = current->InLoadOrderLinks.Blink;
		//current->InLoadOrderLinks.Blink->Flink = current->InLoadOrderLinks.Flink;
		// ����ļ����ڣ�����ֻ���ķ�ʽ��ָ�����ļ�������ʹ�������·���� \\??\\...
		//WCHAR* Path = L"\\??\\D:\\data.txt";
		//HANDLE FileHandle = CreateFile(Path, GENERIC_WRITE, TRUE);
		//// ���ļ���д���ַ���: "hello world"
		//KdPrint(("д���ļ�[%p]->[%d]\n", FileHandle,
		//	WriteFile(FileHandle, "hello world", 12, 0)));
		//// �ļ����Ժ����ʹ�� ZwClose ���йرգ����������ļ�ռ��
		//ZwClose(FileHandle);


		//FileHandle = CreateFile(Path, GENERIC_READ, TRUE);
		//// ��ѯ�ļ��Ĵ�С
		//ULONGLONG FileSize = GetFileSize(FileHandle);
		//CHAR Buff[40] = { 0 };
		//ReadFile(FileHandle, Buff, 0x10, 0);
		//ZwClose(FileHandle);
		//KdPrint(("��ȡ�ļ�[%p]->[%lld]\n%s\nɾ���ļ�[%u]\n", 
		//	FileHandle, FileSize, Buff,
		//	DeleteFile(Path)));
		//UNICODE_STRING ustrQueryFile;
		//RtlInitUnicodeString(&ustrQueryFile, L"\\??\\D:\\Lucy");
		//MyQueryFileAndFileFolder(ustrQueryFile);
		
	}
	return uRet;
}


ULONG_PTR ReadDisp(LPCH FunName, LPMyInfoSend pInfo)
{
	ULONG_PTR uRet = 0;
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
		return GetMods(pInfo->ulBuff, pInfo->ulNum1, pInfo->ulNum2, pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "GetSyss", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->��������\n", FunName, "GetSyss", uRet));
		return GetSyss(pInfo->ulBuff, pInfo->ulNum1, pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "GetPath", 8)))
	{
		KdPrint(("�Ƚ�: %s %s %lu->����Ŀ¼\n", FunName, "GetPath", uRet));
		return GetPath(pInfo->ulBuff, pInfo->ulNum1,
			(PIO_STATUS_BLOCK)pInfo->byBuf2, pInfo);
	}
	KdPrint(("�Ƚ�: [%s] [%p] [%lu]->û�ж�Ӧ\n", FunName, pInfo, uRet));
	return 0;
}

ULONG_PTR GetPIDs(ULONG MaxBuff,ULONG MaxPID, LPMyProcess pPID)
{
	PEPROCESS Process = NULL;
	//KdPrint(("[%lu][%04lu][%p]\n", MaxBuff, MaxPID, (char*)pPID));
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

ULONG_PTR GetMods(ULONG MaxBuff, ULONG PID, ULONG List, LPMyInfoSend pInfo)
{
	UNREFERENCED_PARAMETER(MaxBuff);
	PEPROCESS Process = NULL;
	if (!NT_SUCCESS(PsLookupProcessByProcessId(ULongToHandle(PID), &Process)))
	{
		KdPrint(("�޷���ȡEPROCESS[%lu]��\n", PID));
		return 0;
	}

	//��ȡ PEB��Ϣ
	PPEB_EX peb = PsGetProcessPeb(Process);
	if (!peb)
	{
		KdPrint(("�޷���ȡpeb[%lu][%p]��\n", PID, Process));
		return 0;
	}
	//KdPrint(("peb[%lu][%p]��\n", PID, peb));
	ULONG uRet = 0;
	KAPC_STATE ks;
	KeStackAttachProcess(Process, &ks);
	__try
	{
		PPEB_LDR_DATA_EX peb_LDR_data = (PPEB_LDR_DATA_EX)peb->Ldr;
		PLIST_ENTRY list_entry = &peb_LDR_data->InLoadOrderModuleList;
		//�Ȼ�ȡ��һ��
		PLIST_ENTRY currentList = List ? (PLIST_ENTRY)List : list_entry->Flink;
		while (currentList != list_entry)
		{
			PLDR_DATA_TABLE_ENTRY_EX ldr_data_table_entry =
				(PLDR_DATA_TABLE_ENTRY_EX)currentList;

			uRet = ldr_data_table_entry->FullDllName.Length;
			pInfo->ulNum2 = (ULONG)ldr_data_table_entry->DllBase;
			RtlCopyMemory(
				pInfo->byBuf3,
				ldr_data_table_entry->FullDllName.Buffer,
				uRet
			);
			pInfo->byBuf3[uRet] = 0x0;
			uRet = (ULONG)currentList->Flink;
			break;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		//DbgPrint("Can not Modules...");
	}
	KeUnstackDetachProcess(&ks);
	return uRet;
}

ULONG_PTR GetSyss(ULONG MaxBuff, ULONG List, LPMyInfoSend pInfo)
{
	UNREFERENCED_PARAMETER(MaxBuff);
	ULONG uRet = 0;
	PLDR_DATA_TABLE_ENTRY first = gSysFirst, next = List
		? (PLDR_DATA_TABLE_ENTRY)List
		: (PLDR_DATA_TABLE_ENTRY)first->InLoadOrderLinks.Flink;
	//do {
	//	// ���Ԫ�صĻ�����Ϣ
	//	//KdPrint(("%d: %lu %wZ %wZ\n", ++index, (ULONG)next->BaseDllName.Length,
	//	//	&next->BaseDllName, &next->FullDllName));
	//} while (first != next);

	uRet = next->BaseDllName.Length;
	if (uRet == 0)	return 0;
	pInfo->ulNum1 = uRet;
	pInfo->ulNum2 = (ULONG)next->DllBase;
	pInfo->byBuf2[uRet] = 0x0;
	RtlCopyMemory(
		pInfo->byBuf2,
		next->BaseDllName.Buffer,
		uRet
	);
	pInfo->byBuf3[next->FullDllName.Length] = 0x0;
	RtlCopyMemory(
		pInfo->byBuf3,
		next->FullDllName.Buffer,
		next->FullDllName.Length
	);
	// ��ȡ��������Ԫ�ص���һ��Ԫ��
	next = (PLDR_DATA_TABLE_ENTRY)next->InLoadOrderLinks.Flink;
	return (ULONG)next;
}

ULONG_PTR GetPath(ULONG MaxBuff, ULONG Count, PIO_STATUS_BLOCK pIoStatusBlock, LPMyInfoSend pInfo)
{
	HANDLE hFile = (HANDLE)pInfo->ulNum2;
	PFILE_BOTH_DIR_INFORMATION pDir = (PFILE_BOTH_DIR_INFORMATION)pInfo->byBuf3;
	NTSTATUS status;
	if (hFile == 0)
	{
		UNICODE_STRING ustrPath;
		RtlInitUnicodeString(&ustrPath, (PCWCHAR)pInfo->byBuf2);
		OBJECT_ATTRIBUTES objectAttributes = { 0 };
		InitializeObjectAttributes(&objectAttributes, &ustrPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
		status = ZwCreateFile(&hFile, FILE_LIST_DIRECTORY | SYNCHRONIZE | FILE_ANY_ACCESS,
			&objectAttributes, pIoStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE,
			FILE_OPEN, FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT,
			NULL, 0);
		if (NT_SUCCESS(status))
		{
			//��ʼ������
			pInfo->ulNum2 = (ULONG)hFile;
			RtlZeroMemory(pIoStatusBlock, sizeof(IO_STATUS_BLOCK));
			KdPrint(("%d: ����Ŀ¼���%lX\n", Count, pInfo->ulNum2));
			// ��ȡ��Ϣ
			status = ZwQueryDirectoryFile(hFile, NULL, NULL, NULL, pIoStatusBlock, pDir, MaxBuff,
				FileBothDirectoryInformation, TRUE, NULL, TRUE);
			if (!NT_SUCCESS(status))
			{
				ZwClose(hFile);
				KdPrint(("ZwQueryDirectoryFile[ERROR][%u]", status));
				return 0;
			}
			pInfo->ulNum1 = ++Count;
			KdPrint(("%d: [%2d]%S\n", Count, pDir->FileNameLength, pDir->FileName));
			return pInfo->ulNum1;
		}
	}
	else
	{
		KdPrint(("%d: ���Ŀ¼���%lX\n", Count, pInfo->ulNum2));
		// ����ָ��Ŀ¼�µĵ�һ���ļ��������ر������Ľ��
		status = ZwQueryDirectoryFile(hFile, NULL, NULL, NULL, pIoStatusBlock, pDir, MaxBuff,
			FileBothDirectoryInformation, TRUE, NULL, FALSE);
		if (!NT_SUCCESS(status))
		{
			ZwClose(hFile);
			KdPrint(("ZwQueryDirectoryFile[ERROR][%u]", status));
			return 0;
		}
		KdPrint(("%d: [%2d]%S\n", Count, pDir->FileNameLength, pDir->FileName));
		// �����������ж��� IoStatusBlock �����Ǻ����ķ���ֵ
		if (pIoStatusBlock->Status != STATUS_NO_MORE_FILES)
		{
			pInfo->ulNum1 = ++Count;
			return pInfo->ulNum1;
		}
	}
	ZwClose(hFile);
	return 0;
}
