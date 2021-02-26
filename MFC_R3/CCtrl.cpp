#include "pch.h"
#include "CCtrl.h"
#include <bcrypt.h>
#include "..\2102_ARK\ZwQuerySystemInformation.h"

typedef NTSTATUS(WINAPI* PFN_NTQUERYSYSTEMINFORMATION)(SYSTEM_INFORMATION_CLASS, PVOID, ULONG, PULONG);
PFN_NTQUERYSYSTEMINFORMATION pfnZwQuerySystemInformation = NULL;	// ZwQuerySystemInformation

CCtrl gcCtrl;
CCtrl::CCtrl()
	:pcView(0),pcR3R0(0)
{
	pcData = &gcData;
}

CCtrl::~CCtrl()
{
	delete this->pcR3R0;
	delete this->pcView;
}

BOOL CCtrl::InitCtrl(CMyView* pcView)
{
	this->pcView = pcView;
	this->pcR3R0 = new CR3R0();
	HMODULE hNtDll = LoadLibraryA("ntdll.dll");
	if (hNtDll == 0)
		ExitProcess(4);
	pfnZwQuerySystemInformation = (PFN_NTQUERYSYSTEMINFORMATION)GetProcAddress(hNtDll, "ZwQuerySystemInformation");
	if (pfnZwQuerySystemInformation == 0)
		ExitProcess(5);
	this->TreeFun(21);
	return TRUE;
}

//************************************
// Method:    ZwQuerySystemInformation
// Parameter: SYSTEM_INFORMATION_CLASS SystemInformationClass
// Parameter: PVOID SystemInformation
// Parameter: ULONG SystemInformationLength
// Parameter: PULONG ReturnLength
//************************************
NTSTATUS WINAPI ZwQuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength)
{
	if (pfnZwQuerySystemInformation == NULL)
	{
		//DWORD dwErrCode = GetLastError();
		return -1;
	}
	return pfnZwQuerySystemInformation(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
}

void CCtrl::TreeClick(CTreeCtrl* cTree, HTREEITEM hTree)
{
	auto& tLeafs = this->pcView->m_tLeafs;
	auto& tInfo = tLeafs[hTree];
	if (tInfo.uiDeep != 1)	return;
	printf("�û������[%d][%d]\t", tInfo.uiDeep, tInfo.uiNumb);
	wprintf(L"%s\n", tInfo.str);
	if (tInfo.str == gszTreeCtrl��������)
	{
		//if (this->pcR3R0->GetPIDs(this->pcData->vPIDs))
		//{
		//	this->pcView->InitList(11);
		//	this->pcView->InitList(vPIDs);
		//}
		//else
		//{
		//	MessageBoxA(0, "��ȡ����ʧ�ܡ�", 0, 0);
		//}
		this->TreeFun(11);
	}
	else if (tInfo.str == gszTreeCtrl�̲߳���)
	{
		this->TreeFun(12);
	}
	else if (tInfo.str == gszTreeCtrlģ�����)
	{
		this->TreeFun(13);
	}
	else if (tInfo.str == gszTreeCtrl��������)
	{
		this->TreeFun(21);
	}
}

void CCtrl::TreeFun(DWORD ID, DWORD dwC1 /*= 0*/)
{
	auto& vPIDs = this->pcData->vPIDs;
	DWORD dwNeedSize = 0, dwCount = 0, vSize = vPIDs.size(), tmp;
	switch (ID) {
	//ö�ٽ���
	case 11: {
		vPIDs.clear();
		PSYSTEM_PROCESSES psp = NULL;
		NTSTATUS status = ZwQuerySystemInformation(
			SystemProcessesAndThreadsInformation, NULL, 0, &dwNeedSize);
		if (status == STATUS_INFO_LENGTH_MISMATCH)
		{
			PVOID pBuffer = new BYTE[dwNeedSize];
			status = ZwQuerySystemInformation(
				SystemProcessesAndThreadsInformation, pBuffer, dwNeedSize, NULL);
			if (NT_SUCCESS(status))
			{
				psp = (PSYSTEM_PROCESSES)pBuffer;
				do {
					//printf("PID=%-4d", psp->ProcessId);
					//wprintf(L" %s\n", psp->ProcessName.Buffer);
					vPIDs.push_back({ psp->ProcessId,psp->InheritedFromProcessId });
					auto& Info = vPIDs[dwCount++];
					Info.szExe = psp->ProcessName.Buffer;

					psp = (PSYSTEM_PROCESSES)((ULONG)psp + psp->NextEntryDelta);
				} while (psp->NextEntryDelta != 0);
			}
			delete[] pBuffer; pBuffer = NULL;

			this->pcR3R0->GetTIDs(vPIDs);
			this->pcR3R0->GetMODs(vPIDs, 0);
			this->pcView->InitList(ID);
			this->pcView->InitList(vPIDs, ID, 0);
		}
	}break;
	//�����߳�
	case 12: {
		tmp = pcData->mPID;
		printf("׼����ʾ�߳� PID[%lu]\n", tmp);
		this->pcView->InitList(ID);
		this->pcView->InitList(vPIDs, ID, tmp);
	}break;
	//����ģ��
	case 13: {
		tmp = pcData->mPID;
		printf("׼����ȡģ�� PID[%lu]\n", tmp);
		if (this->pcR3R0->GetMODs(vPIDs, tmp))
		{
			this->pcView->InitList(ID);
			this->pcView->InitList(vPIDs, ID, tmp);
		}
		else
		{
			MessageBoxA(0, "��ȡģ��ʧ�ܡ�", 0, 0);
		}
	}break;
	//��������
	case 21: {
		vector<MySys> vSYSs;
		if (this->pcR3R0->GetSyss(vSYSs))
		{
			this->pcView->InitList(ID);
			this->pcView->InitList(vSYSs);
		}
	}break;
	default: break;
	}
}
