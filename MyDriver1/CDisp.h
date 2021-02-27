#pragma once
#include <ntifs.h>
#include <ntddk.h>
#include "CMyFile.h"
#include "CReNT.h"
#include "CReg.h"


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

// LDR 链表中的每一项都是这个结构体，保存了驱动的基本信息
typedef struct _LDR_DATA_TABLE_ENTRY
{
	struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
	struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x8
	struct _LIST_ENTRY InInitializationOrderLinks;                          //0x10
	VOID* DllBase;                                                          //0x18
	VOID* EntryPoint;                                                       //0x1c
	ULONG SizeOfImage;                                                      //0x20
	struct _UNICODE_STRING FullDllName;                                     //0x24
	struct _UNICODE_STRING BaseDllName;                                     //0x2c
	// ... 后面还有一些字段，由于用不到，为了节省代码量，直接不考虑
} *PLDR_DATA_TABLE_ENTRY;
#pragma pack(1)
typedef  struct  _KSYSTEM_SERVICE_TABLE
{
	PULONG  ServiceTableBase;   //函数地址表的首地址
	PULONG  ServiceCounterTableBase;// 函数表中每个函数被调用的次数
	ULONG   NumberOfService;// 服务函数的个数, NumberOfService * 4 就是整个地址表的大小
	UCHAR* ParamTableBase; // 参数个数表首地址
} KSYSTEM_SERVICE_TABLE, * PKSYSTEM_SERVICE_TABLE;
typedef  struct  _KSERVICE_TABLE_DESCRIPTOR
{
	KSYSTEM_SERVICE_TABLE   ntoskrnl;// ntoskrnl.exe的服务函数，即SSDT
	KSYSTEM_SERVICE_TABLE   win32k; // win32k.sys的服务函数(GDI32.dll/User32.dll 的内核支持)，即ShadowSSDT
	KSYSTEM_SERVICE_TABLE   notUsed1; // 不使用
	KSYSTEM_SERVICE_TABLE   notUsed2; // 不使用
}KSERVICE_TABLE_DESCRIPTOR, * PKSERVICE_TABLE_DESCRIPTOR;
#pragma pack()

#pragma region SSDTHOOK
#pragma pack(1)
typedef struct _ServiceDesriptorEntry
{
	ULONG* ServiceTableBase;        // 服务表基址
	ULONG* ServiceCounterTableBase; // 计数表基址
	ULONG NumberOfServices;         // 表中项的个数
	UCHAR* ParamTableBase;          // 参数表基址
}SSDTEntry, * PSSDTEntry;
#pragma pack()
// 函数原型
typedef NTSTATUS(NTAPI* FnZwOpenProcess)(PHANDLE,
	ACCESS_MASK,
	POBJECT_ATTRIBUTES,
	PCLIENT_ID);
extern FnZwOpenProcess g_OldZwOpenProcess;

#pragma endregion


#define MAKELONG(a,b) ((LONG)(((UINT16)(((DWORD_PTR)(a))&0xffff)) | ((ULONG)((UINT16)(((DWORD_PTR)(b))& 0xffff)))<<16))
NTKERNELAPI PPEB_EX PsGetProcessPeb(PEPROCESS Process);

// 接受R3指令分发
ULONG_PTR WriteDisp(LPCH FunName);
// 向R3发送数据包
ULONG_PTR ReadDisp(LPCH FunName, LPMyInfoSend pInfo);

// 遍历PIDs
ULONG_PTR GetPIDs(ULONG MaxBuff,ULONG MaxPID, LPMyProcess pPID);
// 遍历tPID
ULONG_PTR GettPID(ULONG PID, LPMyInfoSend pInfo);
// 遍历线程
ULONG_PTR GetThID(ULONG MaxBuff, ULONG MaxTID, ULONG TID, LPMyThread pTID);
// 遍历模块
ULONG_PTR GetMods(ULONG MaxBuff, ULONG PID, ULONG List, LPMyInfoSend pInfo);
// 遍历模块2
ULONG_PTR GetMODs(LPMyInfoSend pInfo);
// 遍历驱动
ULONG_PTR GetSyss(ULONG MaxBuff, ULONG List, LPMyInfoSend pInfo);
// 遍历文件
ULONG_PTR GetPath(ULONG MaxBuff, ULONG Count, PIO_STATUS_BLOCK pIoStatusBlock, LPMyInfoSend pInfo);
// 遍历IDT
ULONG_PTR GetIDTs(LPMyInfoSend pInfo);
// 遍历GDT
ULONG_PTR GetGDTs(LPMyInfoSend pInfo);
// 遍历SSDT
ULONG_PTR GetSSDT(LPMyInfoSend pInfo);
// 隐藏驱动
ULONG_PTR SetSYSf(LPMyInfoSend pInfo);
// HOOK
ULONG_PTR SYSHOOK(LPMyInfoSend pInfo);
// 创建文件
ULONG_PTR CrtFile(LPMyInfoSend pInfo);
// 删除文件
ULONG_PTR DelFile(LPMyInfoSend pInfo);
// 隐藏进程
ULONG_PTR HidePID(LPMyInfoSend pInfo);
// 结束进程
ULONG_PTR ExitPID(LPMyInfoSend pInfo);
// 重载内核
ULONG_PTR RLoadNT(LPMyInfoSend pInfo);
// SSDTHOOK
ULONG_PTR SSDTHOK(LPMyInfoSend pInfo);

extern PDRIVER_OBJECT gDriverObject;
extern PVOID gSysFirst;
extern ULONG OriginalAddress;
VOID UnHook();
void UnInstallHook();