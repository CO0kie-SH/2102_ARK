#include "pch.h"
#include "CR3R0.h"

CR3R0::CR3R0()
	:pMem(0)
{
	puts("CR3R0()");
	DWORD bRet = 0;

	// ����ʹ�� CreateFile ȥ��һ���豸����Ҫ�����ԱȨ��
	DeviceHandle = CreateFile(SYMBOLICLINE_NAMER3, GENERIC_ALL,
		NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (DeviceHandle == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(0, "���豸����ʧ�ܡ�", 0, 0);
		ExitProcess(1);
	}
	// ���豸����д������
	WriteFile(DeviceHandle, "InitDevice", 10, &bRet, NULL);
	printf("WriteFile(10) > Bytes[%d]\n", bRet);

	this->pcData = &gcData;
	this->pMem = (LPCH)VirtualAlloc(NULL, sizeof(MyInfoSend), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pMem == NULL)
	{
		MessageBoxA(0, "�޷�������ҳ��", 0, 0);
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
	pInfo->ulNum2 = 0;		//��ǰҪ��ѯ���߳�ID
	strcpy_s(pInfo->byBuf1, "GetThID");

	LPMyThread pTh = (LPMyThread)pInfo->byBuf3;
	vector<MyThread> vTHs;
	while (true)
	{
		if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL))
		{
			printf("��ȡ�߳�ʧ��\n");
			return FALSE;
		}
		else if (ulRet == 0)	//β������
		{
			printf("�߳�β�������������߳���[%lu]��\n", ths);
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
		//printf("����PID[%6lu]\n", PID);
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
		if (PID && PID != dwPID)	//�жϴ����PID
			continue;
		pInfo->ulNum1 = dwPID;		//PID
		pInfo->ulNum2 = 0;			//��
		count = 0;
		if (PID)	Info.vMDs.clear();
		do {
			//printf("׼����ѯģ�� PID[%lu]\n", dwPID);
			ZeroMemory(pInfo->byBuf3, pInfo->ulBuff);
			StrCpyW(szExe, Info.szExe.GetBuffer());
			if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &dwRet, NULL)
				|| dwRet == 0)
			{
				wprintf(L"��ȡģ����Ϣ%s��%2lu PID[%lu] Next[%lu]%s\n",
					dwRet == 0 ? L"��β" : L"ʧ��",
					count, dwPID, dwRet, szExe);
				break;
			}

			if (++count > 200)
			{
				wprintf(L"��ȡģ����Ϣ���ޡ�%2lu PID[%lu] Next[%lu]%s\n",
					count, dwPID, dwRet, szExe);
				return FALSE;
			}

			if (count == 1 && PID == 0)		//��¼·��
			{
				Info.szPath = szExe;
				Info.pNextMod = dwRet;
				break;
			}
			Info.vMDs.push_back({ pInfo->ulNum2,szExe });
			//wprintf(L"%3lu %lu Next[%08lX] Base[%08lX] %s\n",
			//	count, dwPID, dwRet, pInfo->ulNum2, szExe);
			pInfo->ulNum2 = dwRet;	//��һ��������
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
			printf("��ȡģ��ʧ��\n");
			return FALSE;
		}
		else if (ulRet == 0)	//β������
		{
			printf("����β������������������[%lu]��\n", count);
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
			printf("��ȡĿ¼ʧ��\n");
			return FALSE;
		}
		else if (ulRet == 0)	//β������
		{
			printf("Ŀ¼β������������Ŀ¼��[%lu]��\n", pInfo->ulNum1);
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

BOOL CR3R0::GetIDTs(CListCtrl* pvList)
{
	ULONG ulRet = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)pMem;
	auto pIDT = (LPMyIDT)pInfo->byBuf3;
	ZeroMemory(pInfo, sizeof(MyInfoSend));
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	strcpy_s(pInfo->byBuf1, "GetIDTs");
	CString str;

	if (ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL)
		&& ulRet == 0x100)
	{
		for (ULONG i = 0; i < 0x100; i++)
		{
			str.Format(_T("%lu"), i + 1);
			pvList->InsertItem(i, str);
			str.Format(_T("0x%p"), pIDT->Addr[i]);
			pvList->SetItemText(i, 1, str);
			str.Format(_T("%d"), pIDT->IDT[i].uSelector);
			pvList->SetItemText(i, 2, str);
			// ����
			switch (pIDT->IDT[i].GateType)
			{
			case 05: str = L"������"; break;
			case 14: str = L"�ж���"; break;
			case 15: str = L"������"; break;
			default:
				str = L"δ֪��";
				break;
			}
			str.AppendFormat(L"%d", pIDT->IDT[i].GateType);
			//str.Format(_T("%d"), pIDT->IDT[i].GateType);
			pvList->SetItemText(i, 3, str);
			str.Format(_T("%d"), pIDT->IDT[i].DPL);
			pvList->SetItemText(i, 4, str);
			//printf("%3lu Addr[%p] selector: %d, GateType:%d, DPL: %d\n",
			//	i + 1, pIDT->Addr[i],
			//	pIDT->IDT[i].uSelector,//��ѡ����
			//	pIDT->IDT[i].GateType,//����
			//	pIDT->IDT[i].DPL);//��Ȩ�ȼ�
		}
		return TRUE;
	}
	printf("��ȡIDT��ʧ��\n");
	return FALSE;
}

BOOL CR3R0::GetGDTs(CListCtrl* pvList)
{
	ULONG ulRet = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)pMem;
	auto uInfo = (PULONGLONG)pInfo->byBuf3;
	ZeroMemory(pInfo, sizeof(MyInfoSend));
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	strcpy_s(pInfo->byBuf1, "GetGDTs");
	CString str;

	if (ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL)
		&& ulRet > 0)
	{
		for (ULONG i = 0; i < ulRet; i++)
		{
			PSHORT sGDT = (PSHORT)&uInfo[i];
			PGDT_ENTRY eGDT = (PGDT_ENTRY)sGDT;
			ULONG Base = pInfo->ulNum1 + i * 4,
				Addr = MAKELONG(sGDT[0], sGDT[3]);
			UCHAR P = eGDT->P,
				S = eGDT->S,
				G = eGDT->G;
			str.Format(_T("%lu"), i + 1);
			pvList->InsertItem(i, str);
			str.Format(_T("%08lX"), Base);
			pvList->SetItemText(i, 1, str);
			str.Format(_T("%016llX"), uInfo[i]);
			pvList->SetItemText(i, 2, str);
			str.Format(_T("%08lX"), Addr);
			pvList->SetItemText(i, 4, str);
			str.Format(_T("%d"), P);
			pvList->SetItemText(i, 5, str);
			str.Format(_T("%d"), S);
			pvList->SetItemText(i, 6, str);
			str.Format(_T("%d"), G);
			pvList->SetItemText(i, 7, str);
			if (uInfo[i] != 0)
			{
				// ����
				if (S == 0)
				{
					if (eGDT->Type == 12)
						str = L"������";  // ������
					else
						str = L"δ֪��";  // δ֪����
				}
				else
				{
					if (eGDT->Type < 8)
						str = L"���ݶ�";  // ���ݶ�
					else
						str = L"�����";  // �����
				}
				pvList->SetItemText(i, 3, str);
			}
			//printf("%3lu [%p] %016llX ��ƫ�� 0x%08X P=%d S=%d G=%d\n",
			//	i + 1, pGDT->Addr[i], pGDT->uGDT[i],
			//	Addr, P, S, G);
		}
		return TRUE;
	}
	printf("��ȡGDT��ʧ��\n");
	return 0;
}

BOOL CR3R0::GetSSDT(CListCtrl* pvList)
{
	ULONG ulRet = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)pMem;
	ZeroMemory(pInfo, sizeof(MyInfoSend));
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	strcpy_s(pInfo->byBuf1, "GetSSDT");
	CString str;

	if (ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL)
		&& ulRet > 0)
	{
		for (ULONG i = 0, *Addr = (ULONG*)pInfo->byBuf3;
			i < ulRet; i++)
		{

			str.Format(_T("%lu"), i + 1);
			pvList->InsertItem(i, str);
			str.Format(_T("%lX"), Addr[i]);
			pvList->SetItemText(i, 1, str);
			//printf("%3lu [%08lX]\n",
			//	i + 1, Addr[i]);
		}
		return TRUE;
	}
	printf("��ȡSSDT��ʧ��\n");
	return 0;
}

BOOL CR3R0::SetSYSf()
{
	ULONG ulRet = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)pMem;
	ZeroMemory(pInfo, sizeof(MyInfoSend));
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	strcpy_s(pInfo->byBuf1, "SetSYSf");

	if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL)
		&& ulRet != 0)
	{
		printf("��������ʧ�ܡ��������Ѿ�������[%lu]\n", ulRet);
		return 0;
	}
	return TRUE;
}

BOOL CR3R0::SYSHOOK()
{
	ULONG ulRet = 0;
	LPMyInfoSend pInfo = (LPMyInfoSend)pMem;
	LPMyHOOKs pHOOKs = (LPMyHOOKs)pInfo->byBuf3;
	ZeroMemory(pInfo, sizeof(MyInfoSend));
	pInfo->ulSize = 4096;
	pInfo->ulBuff = 4000;
	pInfo->ulNum1 = pcData->mPID;
	strcpy_s(pInfo->byBuf1, "SYSHOOK");

	if (!ReadFile(DeviceHandle, pMem, pInfo->ulSize, &ulRet, NULL)
		|| ulRet == 0)
	{
		printf("����HOOKʧ��[%lu]\n", ulRet);
		return 0;
	}
	return TRUE;
}
