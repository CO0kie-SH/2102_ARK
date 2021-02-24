#include "CDisp.h"

/*
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
*/

PDRIVER_OBJECT gDriverObject = 0;
PVOID gSysFirst = 0;

ULONG_PTR WriteDisp(LPCH FunName)
{
	ULONG_PTR uRet = 0;
	if (10 == (uRet = RtlCompareMemory(FunName, "InitDevice", 10)))
	{
		KdPrint(("比较: %s %s %lu->初始化环境\n", FunName, "InitDevice", uRet));
		//ListCurrentProcessAndThread();
		// 摘链
		//current->InLoadOrderLinks.Flink->Blink = current->InLoadOrderLinks.Blink;
		//current->InLoadOrderLinks.Blink->Flink = current->InLoadOrderLinks.Flink;
		// 如果文件存在，则以只读的方式打开指定的文件，否则就创建它，路径是 \\??\\...
		//WCHAR* Path = L"\\??\\D:\\data.txt";
		//HANDLE FileHandle = CreateFile(Path, GENERIC_WRITE, TRUE);
		//// 向文件内写入字符串: "hello world"
		//KdPrint(("写入文件[%p]->[%d]\n", FileHandle,
		//	WriteFile(FileHandle, "hello world", 12, 0)));
		//// 文件打开以后必须使用 ZwClose 进行关闭，否则会产生文件占用
		//ZwClose(FileHandle);


		//FileHandle = CreateFile(Path, GENERIC_READ, TRUE);
		//// 查询文件的大小
		//ULONGLONG FileSize = GetFileSize(FileHandle);
		//CHAR Buff[40] = { 0 };
		//ReadFile(FileHandle, Buff, 0x10, 0);
		//ZwClose(FileHandle);
		//KdPrint(("读取文件[%p]->[%lld]\n%s\n删除文件[%u]\n", 
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
		return GetMods(pInfo->ulBuff, pInfo->ulNum1, pInfo->ulNum2, pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "GetSyss", 8)))
	{
		//KdPrint(("比较: %s %s %lu->遍历驱动\n", FunName, "GetSyss", uRet));
		return GetSyss(pInfo->ulBuff, pInfo->ulNum1, pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "GetPath", 8)))
	{
		KdPrint(("比较: %s %s %lu->遍历目录\n", FunName, "GetPath", uRet));
		return GetPath(pInfo->ulBuff, pInfo->ulNum1,
			(PIO_STATUS_BLOCK)pInfo->byBuf2, pInfo);
	}
	KdPrint(("比较: [%s] [%p] [%lu]->没有对应\n", FunName, pInfo, uRet));
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

ULONG_PTR GetMods(ULONG MaxBuff, ULONG PID, ULONG List, LPMyInfoSend pInfo)
{
	UNREFERENCED_PARAMETER(MaxBuff);
	PEPROCESS Process = NULL;
	if (!NT_SUCCESS(PsLookupProcessByProcessId(ULongToHandle(PID), &Process)))
	{
		KdPrint(("无法获取EPROCESS[%lu]！\n", PID));
		return 0;
	}

	//获取 PEB信息
	PPEB_EX peb = PsGetProcessPeb(Process);
	if (!peb)
	{
		KdPrint(("无法获取peb[%lu][%p]！\n", PID, Process));
		return 0;
	}
	//KdPrint(("peb[%lu][%p]！\n", PID, peb));
	ULONG uRet = 0;
	KAPC_STATE ks;
	KeStackAttachProcess(Process, &ks);
	__try
	{
		PPEB_LDR_DATA_EX peb_LDR_data = (PPEB_LDR_DATA_EX)peb->Ldr;
		PLIST_ENTRY list_entry = &peb_LDR_data->InLoadOrderModuleList;
		//先获取第一个
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
	//	// 输出元素的基本信息
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
	// 获取遍历到的元素的下一个元素
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
			//初始化变量
			pInfo->ulNum2 = (ULONG)hFile;
			RtlZeroMemory(pIoStatusBlock, sizeof(IO_STATUS_BLOCK));
			KdPrint(("%d: 初次目录句柄%lX\n", Count, pInfo->ulNum2));
			// 获取信息
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
		KdPrint(("%d: 多次目录句柄%lX\n", Count, pInfo->ulNum2));
		// 遍历指定目录下的第一个文件，并返回遍历到的结果
		status = ZwQueryDirectoryFile(hFile, NULL, NULL, NULL, pIoStatusBlock, pDir, MaxBuff,
			FileBothDirectoryInformation, TRUE, NULL, FALSE);
		if (!NT_SUCCESS(status))
		{
			ZwClose(hFile);
			KdPrint(("ZwQueryDirectoryFile[ERROR][%u]", status));
			return 0;
		}
		KdPrint(("%d: [%2d]%S\n", Count, pDir->FileNameLength, pDir->FileName));
		// 结束条件的判断是 IoStatusBlock 而不是函数的返回值
		if (pIoStatusBlock->Status != STATUS_NO_MORE_FILES)
		{
			pInfo->ulNum1 = ++Count;
			return pInfo->ulNum1;
		}
	}
	ZwClose(hFile);
	return 0;
}
