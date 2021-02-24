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


//函数ZwQuerySystemInformation小结_sas???的博客-CSDN博客
//https://blog.csdn.net/weixin_33906657/article/details/91860580
typedef struct _SYSTEM_PROCESSES {
	ULONG          NextEntryDelta;          //构成结构序列的偏移量；       
	ULONG          ThreadCount;             //线程数目；
	ULONG          Reserved1[6];
	LARGE_INTEGER  CreateTime;				//创建时间
	LARGE_INTEGER  UserTime;				//用户模式(Ring 3)的CPU时间；
	LARGE_INTEGER  KernelTime;				//内核模式(Ring 0)的CPU时间；
	UNICODE_STRING ProcessName;				//进程名称；       
	KPRIORITY      BasePriority;            //进程优先权；       
	HANDLE         ProcessId;              	//进程标识符；       
	HANDLE         InheritedFromProcessId;  //父进程的标识符；       
	ULONG          HandleCount;				//句柄数目；       
	ULONG          Reserved2[2];
	VM_COUNTERS    VmCounters;              //虚拟存储器的结构；       
	IO_COUNTERS    IoCounters;              //IO计数结构；       
	//SYSTEM_THREADS Threads[1];              //进程相关线程的结构数组；   
}SYSTEM_PROCESSES, * PSYSTEM_PROCESSES;

//typedef NTSTATUS(WINAPI* NTQUERYSYSTEMINFORMATION)(INSYSTEM_INFORMATION_CLASS, IN OUT PVOID, INULONG, OUTPULONG);
////加载NTDLL.DLL，获取函数地址。
//NTQUERYSYSTEMINFORMATIONZwQuerySystemInformation = NULL;
//ZwQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)GetProcAddress(ntdll.dll, "ZwQuerySystemInfromation");

// 接受R3指令分发
ULONG_PTR WriteDisp(LPCH FunName);
// 向R3发送数据包
ULONG_PTR ReadDisp(LPCH FunName, LPMyInfoSend pInfo);

// 遍历PIDs
ULONG_PTR GetPIDs(ULONG MaxBuff,ULONG MaxPID, LPMyProcess pPID);
// 遍历线程
ULONG_PTR GetThID(ULONG MaxBuff, ULONG MaxTID, ULONG TID, LPMyThread pTID);
// 遍历模块
ULONG_PTR GetMods(ULONG MaxBuff,ULONG PID, LPMyProcess pPID);



