#pragma once
#include <ntifs.h>
#include <ntddk.h>

typedef struct _MyInfoSend
{
	ULONG ulSize;
	ULONG ulBuff;
	ULONG ulNum1;
	ULONG ulNum2;
	CHAR byBuf1[40];
	CHAR byBuf2[40];
	CHAR byBuf3[4000];
}MyInfoSend, * LPMyInfoSend;

typedef struct _MyProcess
{
	ULONG tPID;
	ULONG pPID;
}MyProcess, * LPMyProcess;

typedef struct _MyThread
{
	ULONG TID;
	ULONG PID;
	PCHAR pETHREAD;
}MyThread, * LPMyThread;


//����ZwQuerySystemInformationС��_sas???�Ĳ���-CSDN����
//https://blog.csdn.net/weixin_33906657/article/details/91860580
typedef struct _SYSTEM_PROCESSES {
	ULONG          NextEntryDelta;          //���ɽṹ���е�ƫ������       
	ULONG          ThreadCount;             //�߳���Ŀ��
	ULONG          Reserved1[6];
	LARGE_INTEGER  CreateTime;				//����ʱ��
	LARGE_INTEGER  UserTime;				//�û�ģʽ(Ring 3)��CPUʱ�䣻
	LARGE_INTEGER  KernelTime;				//�ں�ģʽ(Ring 0)��CPUʱ�䣻
	UNICODE_STRING ProcessName;				//�������ƣ�       
	KPRIORITY      BasePriority;            //��������Ȩ��       
	HANDLE         ProcessId;              	//���̱�ʶ����       
	HANDLE         InheritedFromProcessId;  //�����̵ı�ʶ����       
	ULONG          HandleCount;				//�����Ŀ��       
	ULONG          Reserved2[2];
	VM_COUNTERS    VmCounters;              //����洢���Ľṹ��       
	IO_COUNTERS    IoCounters;              //IO�����ṹ��       
	//SYSTEM_THREADS Threads[1];              //��������̵߳Ľṹ���飻   
}SYSTEM_PROCESSES, * PSYSTEM_PROCESSES;

//typedef NTSTATUS(WINAPI* NTQUERYSYSTEMINFORMATION)(INSYSTEM_INFORMATION_CLASS, IN OUT PVOID, INULONG, OUTPULONG);
////����NTDLL.DLL����ȡ������ַ��
//NTQUERYSYSTEMINFORMATIONZwQuerySystemInformation = NULL;
//ZwQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)GetProcAddress(ntdll.dll, "ZwQuerySystemInfromation");

// ����R3ָ��ַ�
ULONG_PTR WriteDisp(LPCH FunName);
// ��R3�������ݰ�
ULONG_PTR ReadDisp(LPCH FunName, LPMyInfoSend pInfo);

// ����PIDs
ULONG_PTR GetPIDs(ULONG MaxBuff,ULONG MaxPID, LPMyProcess pPID);
// �����߳�
ULONG_PTR GetThID(ULONG MaxBuff, ULONG MaxTID, ULONG TID, LPMyThread pTID);
// ����ģ��
ULONG_PTR GetMods(ULONG MaxBuff,ULONG PID, LPMyProcess pPID);


