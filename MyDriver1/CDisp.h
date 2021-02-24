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


typedef struct _PEB_LDR_DATA_EX
{
	ULONG Length; // +0x00  
	BOOLEAN Initialized; // +0x04  
	PVOID SsHandle; // +0x08  
	LIST_ENTRY InLoadOrderModuleList; // +0x0c 
	LIST_ENTRY InMemoryOrderModuleList; // +0x14
	LIST_ENTRY InInitializationOrderModuleList;// +0x1c  
}PEB_LDR_DATA_EX, * PPEB_LDR_DATA_EX;

typedef struct _LDR_DATA_TABLE_ENTRY_EX {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	//union {
	//	LIST_ENTRY HashLinks;
	//	struct {
	//		PVOID SectionPointer;
	//		ULONG CheckSum;
	//	};
	//};
	//union {
	//	ULONG TimeDateStamp;
	//	PVOID LoadedImports;
	//};
	//PVOID EntryPointActivationContext;
	//PVOID PatchInformation;
	//LIST_ENTRY ForwarderLinks;
	//LIST_ENTRY ServiceTagLinks;
	//LIST_ENTRY StaticLinks;
	//PVOID ContextInformation;
	//PVOID OriginalBase;
	//LARGE_INTEGER LoadTime;
} LDR_DATA_TABLE_ENTRY_EX, * PLDR_DATA_TABLE_ENTRY_EX;

typedef struct _CURDIR {
	UNICODE_STRING DosPath;
	PVOID Handle;
}CURDIR, * PCURDIR;

typedef struct _RTL_DRIVE_LETTER_CURDIR {
	USHORT Flags;
	USHORT Length;
	ULONG TimeStamp;
	STRING DosPath;
}RTL_DRIVE_LETTER_CURDIR, * PRTL_DRIVE_LETTER_CURDIR;

//进程参数

typedef struct _RTL_USER_PROCESS_PARAMETERS {
	ULONG MaximumLength;
	ULONG Length;
	ULONG Flags;
	ULONG DebugFlags;
	PVOID ConsoleHandle;
	ULONG ConsoleFlags;
	PVOID StandardInput;
	PVOID StandardOutput;
	PVOID StandardError;
	CURDIR CurrentDirectory;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
	PVOID Environment;
	ULONG StartingX;
	ULONG StartingY;
	ULONG CountX;
	ULONG CountY;
	ULONG CountCharsX;
	ULONG CountCharsY;
	ULONG FillAttribute;
	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING DesktopInfo;
	UNICODE_STRING ShellInfo;
	UNICODE_STRING RuntimeData;
	RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];
}RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

//进程环境块（因为Windows内核有一个机构PEB，为了不重定义，所以就另起一个名字）
typedef struct _PEB_EX {
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	UCHAR SpareBool;
	PVOID Mutant;
	PVOID ImageBaseAddress;
	PPEB_LDR_DATA_EX Ldr;
	PRTL_USER_PROCESS_PARAMETERS  ProcessParameters;
	UCHAR Reserved4[104];
	PVOID Reserved5[52];
	PVOID PostProcessInitRoutine;
	PVOID Reserved7;
	UCHAR Reserved6[128];
	ULONG SessionId;
} PEB_EX, * PPEB_EX;


NTKERNELAPI PPEB_EX PsGetProcessPeb(PEPROCESS Process);


// 接受R3指令分发
ULONG_PTR WriteDisp(LPCH FunName);
// 向R3发送数据包
ULONG_PTR ReadDisp(LPCH FunName, LPMyInfoSend pInfo);

// 遍历PIDs
ULONG_PTR GetPIDs(ULONG MaxBuff,ULONG MaxPID, LPMyProcess pPID);
// 遍历线程
ULONG_PTR GetThID(ULONG MaxBuff, ULONG MaxTID, ULONG TID, LPMyThread pTID);
// 遍历模块
ULONG_PTR GetMods(ULONG MaxBuff, ULONG PID, ULONG List, LPMyInfoSend pInfo);



