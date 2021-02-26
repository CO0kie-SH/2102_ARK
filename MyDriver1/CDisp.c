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
ULONG gSYSf = 0, kID = 0, kPID = 0, hookPID = 0;
ULONG OriginalAddress = 0;

//1.ʹ�û��ķ�����ȡ�ں˺����ĵ�ַ
ULONG GetFunctionAddr_ASM(PKSYSTEM_SERVICE_TABLE KeServiceDescriptorTable, LONG lgSsdtIndex)
{
	ULONG lgSsdtFunAddr = 0;
	//lgSsdtFunAddr = [[KeServiceDescriptorTable]+lgSsdtIndex*4]
	__asm
	{
		push ebx
		push eax
		mov ebx, KeServiceDescriptorTable
		mov ebx, [ebx]	//SSDT��Ļ���ַ
		mov eax, lgSsdtIndex
		shl eax, 2
		add ebx, eax
		mov ebx, [ebx]
		mov lgSsdtFunAddr, ebx
		pop  eax
		pop  ebx
	}
	return lgSsdtFunAddr;
}

void EnumRegedit()
{
}

ULONG_PTR WriteDisp(LPCH FunName)
{
	ULONG_PTR uRet = 0;
	if (10 == (uRet = RtlCompareMemory(FunName, "InitDevice", 10)))
	{
		KdPrint(("�Ƚ�: %s %s %lu->��ʼ������\n", FunName, "InitDevice", uRet));
		EnumRegedit();
	}
	return uRet;
}


ULONG_PTR ReadDisp(LPCH FunName, LPMyInfoSend pInfo)
{
	ULONG_PTR uRet = 0;
	if (8 == (uRet = RtlCompareMemory(FunName, "GetPIDs", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->��������\n", FunName, "GetPIDs", uRet));
		return GetPIDs(pInfo->ulBuff, pInfo->ulNum1, (LPMyProcess)pInfo->byBuf3);
	}
	if (8 == (uRet = RtlCompareMemory(FunName, "GettPID", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->��ȡ������Ϣ\n", FunName, "GettPID", uRet));
		return GettPID(pInfo->ulNum1, (LPMyInfoSend)pInfo->byBuf3);
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
	else if (8 == (uRet = RtlCompareMemory(FunName, "GetMODs", 8)))
	{
		//KdPrint(("�Ƚ�: %s %lu->����ģ�� %lu\n", FunName, uRet, pInfo->ulNum1));
		return GetMODs(pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "GetSyss", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->��������\n", FunName, "GetSyss", uRet));
		return GetSyss(pInfo->ulBuff, pInfo->ulNum1, pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "GetPath", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->����Ŀ¼->%S\n",
		//	FunName, "GetPath", uRet, (PWCHAR)pInfo->byBuf2));
		return GetPath(pInfo->ulBuff, pInfo->ulNum1,
			(PIO_STATUS_BLOCK)pInfo->byBuf2, pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "GetIDTs", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->����IDTs\n", FunName, "GetIDTs", uRet));
		return GetIDTs(pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "GetGDTs", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->����GDTs\n", FunName, "GetGDTs", uRet));
		return GetGDTs(pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "GetSSDT", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->����SSDT\n", FunName, "GetSSDT", uRet));
		return GetSSDT(pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "SetSYSf", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->��������\n", FunName, "SetSYSf", uRet));
		return SetSYSf(pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "SYSHOOK", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->HOOK\n", FunName, "SYSHOOK", uRet));
		return SYSHOOK(pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "CrtFile", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->�����ļ�\n", FunName, "CrtFile", uRet));
		return CrtFile(pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "DelFile", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->ɾ���ļ�\n", FunName, "DelFile", uRet));
		return DelFile(pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "ExitPID", 8)))
	{
		//KdPrint(("�Ƚ�: %s %s %lu->��������\n", FunName, "ExitPID", uRet));
		return ExitPID(pInfo);
	}
	else if (8 == (uRet = RtlCompareMemory(FunName, "RLoadNT", 8)))
	{
		KdPrint(("�Ƚ�: %s %s %lu->�����ں�\n", FunName, "RLoadNT", uRet));
		return RLoadNT(pInfo);
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

ULONG_PTR GettPID(ULONG PID, LPMyInfoSend pInfo)
{
	ULONG uRet = 0;
	PEPROCESS Process = NULL;
	if (!NT_SUCCESS(PsLookupProcessByProcessId(ULongToHandle(PID), &Process)))
	{
		KdPrint(("�޷���ȡEPROCESS PID=[%lu]��\n", PID));
		return 0;
	}
	//KdPrint(("PID[%lu] EPROCESS[%p]\n", PID, Process));
	//��ȡ PEB��Ϣ
	//ULONG peb1 = *((ULONG*)((char*)Process + 0x1A8));
	PPEB_EX peb = PsGetProcessPeb(Process);
	if (!peb)
	{
		KdPrint(("�޷���ȡpeb PID[%lu]EPS[%p]��\n", PID, Process));
		return 0;
	}
	//KdPrint(("PID[%lu] PEB[%p]\n", PID, peb));
	__try
	{
		KAPC_STATE ks;
		KeStackAttachProcess(Process, &ks);
		ULONG* uUser = ((ULONG*)((char*)peb + 0x10));
		//KdPrint(("PID[%lu] PEB2[%p]\n", PID, uUser));
		PRTL_USER_PROCESS_PARAMETERS pUser =
			(PRTL_USER_PROCESS_PARAMETERS)*uUser;
		uRet = pUser->ImagePathName.Length;
		RtlCopyMemory(pInfo->byBuf3, pUser->ImagePathName.Buffer, uRet);
		pInfo->byBuf3[uRet] = 0x0;
		KdPrint(("PID[%lu] [%lu]%S\n", PID, uRet, (PWCH)pInfo->byBuf3));
		KeUnstackDetachProcess(&ks);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("Can not Modules...PID[%lu]\n", PID));
	}
	return uRet;
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

/*
	����
*/
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
		KdPrint(("�޷���ȡpeb PID[%lu]EPS[%p]��\n", PID, Process));
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

ULONG_PTR GetMODs(LPMyInfoSend pInfo)
{
	ULONG PID = pInfo->ulNum1, ulNext = pInfo->ulNum2,
		uRet = 0;
	PWCH szExe = (PWCH)pInfo->byBuf3;
	PEPROCESS Process = NULL;
	if (!NT_SUCCESS(PsLookupProcessByProcessId(ULongToHandle(PID), &Process)))
	{
		KdPrint(("�޷���ȡEPROCESS PID=[%lu]��\n", PID));
		return 0;
	}
	//KdPrint(("PID[%lu] EPROCESS[%p] %S\n", PID, Process, szExe));
	//��ȡ PEB��Ϣ
	//ULONG peb1 = *((ULONG*)((char*)Process + 0x1A8));
	PPEB_EX peb = PsGetProcessPeb(Process);
	if (!peb)
	{
		KdPrint(("�޷���ȡpeb PID[%lu]EPS[%p]��\n", PID, Process));
		return 0;
	}
	//KdPrint(("PID[%lu] PEB[%p]\n", PID, peb));
	KAPC_STATE ks;
	__try
	{
		KeStackAttachProcess(Process, &ks);
		PPEB_LDR_DATA_EX peb_LDR_data = (PPEB_LDR_DATA_EX)peb->Ldr;
		PLIST_ENTRY list_entry = &peb_LDR_data->InLoadOrderModuleList;
		PLIST_ENTRY List = ulNext ? (PLIST_ENTRY)ulNext : list_entry->Flink;
		//��ȡLDR��
		PLDR_DATA_TABLE_ENTRY_EX ldr = 
			(PLDR_DATA_TABLE_ENTRY_EX)List;
		//KdPrint(("PID[%lu] FullName[%wZ]\n", PID, &ldr->FullDllName));
		pInfo->ulNum2 = (ULONG)ldr->DllBase;
		RtlCopyMemory(szExe, ldr->FullDllName.Buffer, ldr->FullDllName.Length);
		if (List->Flink != list_entry->Flink)
			uRet = (ULONG)List->Flink;
		KeUnstackDetachProcess(&ks);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		KdPrint(("Can not Modules...PID[%lu]\n", PID));
		KeUnstackDetachProcess(&ks);
	}
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
		//KdPrint(("%d: ����Ŀ¼���%lX->[%u]\n", Count, pInfo->ulNum2, status));
		if (NT_SUCCESS(status))
		{
			//��ʼ������
			pInfo->ulNum2 = (ULONG)hFile;
			RtlZeroMemory(pIoStatusBlock, sizeof(IO_STATUS_BLOCK));
			// ��ȡ��Ϣ
			status = ZwQueryDirectoryFile(hFile, NULL, NULL, NULL, pIoStatusBlock, pDir, MaxBuff,
				FileBothDirectoryInformation, TRUE, NULL, TRUE);
			if (!NT_SUCCESS(status))
			{
				ZwClose(hFile);
				KdPrint(("ZwQueryDirectoryFile[ERROR1][%u]", status));
				return 0;
			}
			pInfo->ulNum1 = ++Count;
			//KdPrint(("%d: [%2d]%S\n", Count, pDir->FileNameLength, pDir->FileName));
			return pInfo->ulNum1;
		}
	}
	else
	{
		//KdPrint(("%d: ���Ŀ¼���%lX\n", Count, pInfo->ulNum2));
		// ����ָ��Ŀ¼�µĵ�һ���ļ��������ر������Ľ��
		status = ZwQueryDirectoryFile(hFile, NULL, NULL, NULL, pIoStatusBlock, pDir, MaxBuff,
			FileBothDirectoryInformation, TRUE, NULL, FALSE);
		if (!NT_SUCCESS(status))
		{
			ZwClose(hFile);
			KdPrint(("ZwQueryDirectoryFile[ERROR2][%u]\n", status));
			return 0;
		}
		//KdPrint(("%d: [%2d]%S\n", Count, pDir->FileNameLength, pDir->FileName));
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

ULONG_PTR GetIDTs(LPMyInfoSend pInfo)
{
	LPMyIDT myIDT = (LPMyIDT)pInfo->byBuf3;
	IDT_INFO IDT = { 0,0,0 };
	PIDT_ENTRY pIDTEntry = NULL;
	// ��ȡIDT���ַ
	__asm sidt IDT;

	// ��ȡIDT�������ַ
	pIDTEntry = (PIDT_ENTRY)MAKELONG(IDT.uLowIdtBase, IDT.uHighIdtBase);
	if (!pIDTEntry)	return 0;

	// ��ȡIDT��Ϣ
	for (ULONG i = 0, Idt_address; i < 0x100; i++)
	{
		Idt_address = MAKELONG(pIDTEntry[i].uOffsetLow, pIDTEntry[i].uOffsetHigh);
		myIDT->Addr[i] = (LPCH)Idt_address;

		RtlCopyMemory(&myIDT->IDT[i], &pIDTEntry[i], sizeof(IDT_ENTRY));
		//KdPrint(("addr: %08X, int: %d, selector: %d, GateType:%d, DPL: %d\n",
		//	Idt_address,// �жϵ�ַ
		//	i,// �жϺ�
		//	pIDTEntry[i].uSelector,// ��ѡ����
		//	pIDTEntry[i].GateType,//����
		//	pIDTEntry[i].DPL//��Ȩ�ȼ�
		//	));
	}
	return 0x100;
}

ULONG_PTR GetGDTs(LPMyInfoSend pInfo)
{
	GDT_INFO GDT = { 0,0,0 };
	PGDT_ENTRY pGDTEntry = NULL, pUser = (PGDT_ENTRY)pInfo->byBuf3;
	// ��ȡGDT���ַ
	_asm sgdt GDT;
	// ��ȡGDT�������ַ
	pGDTEntry = (PGDT_ENTRY)MAKELONG(GDT.uLowGdtBase, GDT.uHighGdtBase);
	if (!pGDTEntry)	return 0;

	// ��ȡGDT��Ϣ
	//KdPrint(("---------------GDT��������---------\n"));
	ULONG i = 0, ulMax = pInfo->ulBuff / 8;
	pInfo->ulNum1 = (ULONG)pGDTEntry;
	while (i < ulMax)
	{
		ULONGLONG* uGDT = (ULONGLONG*)&pGDTEntry[i];
		//KdPrint(("%3lu addr: %p %016llX\n", i + 1, uGDT, *uGDT));
		if (*uGDT == -1)	return i;
		pUser[i] = pGDTEntry[i];
		//myGDT->Addr[i] = (LPCH)uGDT;
		//myGDT->uGDT[i] = *uGDT;
		++i;
	}
	/*for (; i < 0x100; i++)
	{
		Gdt_address = MAKELONG(pGDTEntry[i].Base16_31, pGDTEntry[i].Base0_7);
		Gdt_address = MAKELONG(Gdt_address, pGDTEntry[i].Base24_31);
		Gdt_limit = MAKELONG(pGDTEntry[i].Limit_0_15, pGDTEntry[i].Limit_16_19);
		myGDT->Addr[i] = (LPCH)Gdt_address;
		myGDT->limit[i] = Gdt_limit;

		RtlCopyMemory(&myGDT->GDT[i], &pGDTEntry[i], sizeof(GDT_ENTRY));
		 ��ӡ
		KdPrint(("%3lu addr: %08X, limit: %d, P: %d, G:%d, S:%d,Type:%d,D/B:%d,DPL:%d\n",
			i+1,
			Gdt_address, Gdt_limit,
			pGDTEntry[i].P,
			pGDTEntry[i].G,
			pGDTEntry[i].S,
			pGDTEntry[i].Type,
			pGDTEntry[i].D_B,
			pGDTEntry[i].DPL
			));
	}*/
	return i;
}

ULONG_PTR GetSSDT(LPMyInfoSend pInfo)
{
	// ��ȡϵͳ������������
	PETHREAD pCurThread = PsGetCurrentThread();
	KSERVICE_TABLE_DESCRIPTOR* pServiceTable = (KSERVICE_TABLE_DESCRIPTOR*)
		(*(ULONG*)((ULONG_PTR)pCurThread + 0xBC));
	PKSYSTEM_SERVICE_TABLE pSSDT = (PKSYSTEM_SERVICE_TABLE)pServiceTable;
	//KdPrint(("SSDT %lu\n", pSSDT->NumberOfService));
	ULONG i = 0, Addr, * lpSave = (ULONG*)pInfo->byBuf3,
		max1 = pInfo->ulBuff / sizeof(ULONG),
		max2 = pSSDT->NumberOfService;
	while (i < max2)
	{
		if (i > max1)	return 0;
		Addr = GetFunctionAddr_ASM(pSSDT, i);
		lpSave[i] = Addr;
		i++;
	}
	return i;
}

ULONG_PTR SetSYSf(LPMyInfoSend pInfo)
{
	UNREFERENCED_PARAMETER(pInfo);
	// LDR ��һ��˫���������������������ǣ��������Ĳ����Լ�
	PLDR_DATA_TABLE_ENTRY current = gDriverObject->DriverSection;
	PLDR_DATA_TABLE_ENTRY item = current;

	// ����һ����������ʾ��ǰ���������ǵڼ���
	//int index = 1;
	do {
		// ���Ԫ�صĻ�����Ϣ
		//KdPrint(("%d: %wZ %wZ\n", index++, &item->BaseDllName, &item->FullDllName));

		// ��ȡ��������Ԫ�ص���һ��Ԫ��
		item = (PLDR_DATA_TABLE_ENTRY)item->InLoadOrderLinks.Flink;
	} while (current != item);

	// ժ��
	current->InLoadOrderLinks.Flink->Blink = current->InLoadOrderLinks.Blink;
	current->InLoadOrderLinks.Blink->Flink = current->InLoadOrderLinks.Flink;
	return gSYSf++;
}



void kOpen()
{
	__asm
	{
		mov eax, [edx + 0x14];	// ��ȡ���ĸ����� ClientId
		mov eax, [eax];
		mov kPID, eax;
	}
	if (hookPID && hookPID == kPID)
	{
		_asm mov[edx + 0x0c], 0;
		//KdPrint(("Open [%lu]\n", kPID));
	}
}

__declspec(naked)FakeAddress()
{
	__asm
	{
		mov kID, eax
		pushad
		push fs
		push 0x30
		pop fs
	}
	if (kID == 0x0BE)
		kOpen();
	_asm
	{
		pop fs
		popad
		jmp[OriginalAddress]
	}
}

VOID
Hook()
{
	_asm
	{
		mov ecx, 0x176
		rdmsr
		mov OriginalAddress, eax
		mov eax, FakeAddress
		wrmsr
	}
	KdPrint(("HOOK��װ(%lX)\n", OriginalAddress));
}

VOID
UnHook()
{
	KdPrint(("HOOKж��(%lX)\n", OriginalAddress));
	KIRQL oldIrql = KeRaiseIrqlToDpcLevel();
	_asm
	{
		mov ecx, 0x176
		mov eax, OriginalAddress
		wrmsr
	}
	KeLowerIrql(oldIrql);
}

ULONG_PTR ExitPID(LPMyInfoSend pInfo)
{

	HANDLE hProcess = 0;
	OBJECT_ATTRIBUTES Obj = {sizeof(OBJECT_ATTRIBUTES)};
	CLIENT_ID Cid = { (HANDLE)pInfo->ulNum1 };

	NTSTATUS status = ZwOpenProcess(
		&hProcess,
		1,
		&Obj,
		&Cid);
	if (hProcess)
	{
		status = ZwTerminateProcess(hProcess, 0);
		ZwClose(hProcess);
		return NT_SUCCESS(status);
	}
	return 0;
}

ULONG_PTR SYSHOOK(LPMyInfoSend pInfo)
{
	//UNREFERENCED_PARAMETER(pInfo);
	hookPID = pInfo->ulNum1;
	if (OriginalAddress == 0)
		Hook();
	return hookPID;
}

ULONG_PTR CrtFile(LPMyInfoSend pInfo)
{
	//UNREFERENCED_PARAMETER(pInfo);
#pragma region �ļ�����
	
	// ����ļ����ڣ�����ֻ���ķ�ʽ��ָ�����ļ�������ʹ�������·���� \\??\\...
	WCHAR* Path = L"\\??\\D:\\data.txt";
	HANDLE FileHandle = CreateFile(Path, GENERIC_WRITE, TRUE);
	// ���ļ���д���ַ���: "hello world"
	KdPrint(("д���ļ�[%p]->[%d]\n", FileHandle,
		WriteFile(FileHandle, "hello world", 12, 0)));
	// �ļ����Ժ����ʹ�� ZwClose ���йرգ����������ļ�ռ��
	ZwClose(FileHandle);


	FileHandle = CreateFile(Path, GENERIC_READ, TRUE);
	// ��ѯ�ļ��Ĵ�С
	ULONGLONG FileSize = GetFileSize(FileHandle);
	//CHAR Buff[40] = { 0 };
	LPCH Buff = pInfo->byBuf3;
	ReadFile(FileHandle, Buff, 0x10, 0);
	ZwClose(FileHandle);
	KdPrint(("��ȡ�ļ�[%p]->[%lld]\n%s\n",
		FileHandle, FileSize, Buff));
	//UNICODE_STRING ustrQueryFile;
	//RtlInitUnicodeString(&ustrQueryFile, L"\\??\\D:\\Lucy");
	//MyQueryFileAndFileFolder(ustrQueryFile);
#pragma endregion
	return TRUE;
}

ULONG_PTR DelFile(LPMyInfoSend pInfo)
{
	UNREFERENCED_PARAMETER(pInfo);
	WCHAR* Path = L"\\??\\D:\\data.txt";
	return NT_SUCCESS(DeleteFile(Path));
}