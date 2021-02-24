#pragma once

/*
	转载来自：ZwQuerySystemInformation_friendan的专栏-CSDN博客
	https://blog.csdn.net/friendan/article/details/52503730
*/

#ifndef ZwQuerySystemInformation_H_
#define ZwQuerySystemInformation_H_
#include <Windows.h>

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation,                 //  0 Y N   
	SystemProcessorInformation,             //  1 Y N   
	SystemPerformanceInformation,           //  2 Y N   
	SystemTimeOfDayInformation,             //  3 Y N   
	SystemNotImplemented1,                  //  4 Y N   
	SystemProcessesAndThreadsInformation,   //  5 Y N   
	SystemCallCounts,                       //  6 Y N   
	SystemConfigurationInformation,         //  7 Y N   
	SystemProcessorTimes,                   //  8 Y N   
	SystemGlobalFlag,                       //  9 Y Y   
	SystemNotImplemented2,                  // 10 Y N   
	SystemModuleInformation,                // 11 Y N   
	SystemLockInformation,                  // 12 Y N   
	SystemNotImplemented3,                  // 13 Y N   
	SystemNotImplemented4,                  // 14 Y N   
	SystemNotImplemented5,                  // 15 Y N   
	SystemHandleInformation,                // 16 Y N   
	SystemObjectInformation,                // 17 Y N   
	SystemPagefileInformation,              // 18 Y N   
	SystemInstructionEmulationCounts,       // 19 Y N   
	SystemInvalidInfoClass1,                // 20   
	SystemCacheInformation,                 // 21 Y Y   
	SystemPoolTagInformation,               // 22 Y N   
	SystemProcessorStatistics,              // 23 Y N   
	SystemDpcInformation,                   // 24 Y Y   
	SystemNotImplemented6,                  // 25 Y N   
	SystemLoadImage,                        // 26 N Y   
	SystemUnloadImage,                      // 27 N Y   
	SystemTimeAdjustment,                   // 28 Y Y   
	SystemNotImplemented7,                  // 29 Y N   
	SystemNotImplemented8,                  // 30 Y N   
	SystemNotImplemented9,                  // 31 Y N   
	SystemCrashDumpInformation,             // 32 Y N   
	SystemExceptionInformation,             // 33 Y N   
	SystemCrashDumpStateInformation,        // 34 Y Y/N   
	SystemKernelDebuggerInformation,        // 35 Y N   
	SystemContextSwitchInformation,         // 36 Y N   
	SystemRegistryQuotaInformation,         // 37 Y Y   
	SystemLoadAndCallImage,                 // 38 N Y   
	SystemPrioritySeparation,               // 39 N Y   
	SystemNotImplemented10,                 // 40 Y N   
	SystemNotImplemented11,                 // 41 Y N   
	SystemInvalidInfoClass2,                // 42   
	SystemInvalidInfoClass3,                // 43   
	SystemTimeZoneInformation,              // 44 Y N   
	SystemLookasideInformation,             // 45 Y N   
	SystemSetTimeSlipEvent,                 // 46 N Y   
	SystemCreateSession,                    // 47 N Y   
	SystemDeleteSession,                    // 48 N Y   
	SystemInvalidInfoClass4,                // 49   
	SystemRangeStartInformation,            // 50 Y N   
	SystemVerifierInformation,              // 51 Y Y   
	SystemAddVerifier,                      // 52 N Y   
	SystemSessionProcessesInformation       // 53 Y N   
} SYSTEM_INFORMATION_CLASS;



NTSTATUS WINAPI ZwQuerySystemInformation(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength);

typedef struct _LSA_UNICODE_STRING
{
	USHORT  Length;
	USHORT  MaximumLength;
	PWSTR   Buffer;
}LSA_UNICODE_STRING, * PLSA_UNICODE_STRING;
typedef LSA_UNICODE_STRING UNICODE_STRING, * PUNICODE_STRING;

typedef LONG KPRIORITY;

typedef enum _THREAD_STATE
{
	StateInitialized,
	StateReady,
	StateRunning,
	StateStandby,
	StateTerminated,
	StateWait,
	StateTransition,
	StateUnknown
}THREAD_STATE;

typedef enum _KWAIT_REASON
{
	Executive,
	FreePage,
	PageIn,
	PoolAllocation,
	DelayExecution,
	Suspended,
	UserRequest,
	WrExecutive,
	WrFreePage,
	WrPageIn,
	WrPoolAllocation,
	WrDelayExecution,
	WrSuspended,
	WrUserRequest,
	WrEventPair,
	WrQueue,
	WrLpcReceive,
	WrLpcReply,
	WrVertualMemory,
	WrPageOut,
	WrRendezvous,
	Spare2,
	Spare3,
	Spare4,
	Spare5,
	Spare6,
	WrKernel
}KWAIT_REASON;

typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
}CLIENT_ID;
typedef CLIENT_ID* PCLIENT_ID;

typedef LONG KPRIORITY;

typedef struct _VM_COUNTERS
{
	ULONG PeakVirtualSize;
	ULONG VirtualSize;
	ULONG PageFaultCount;
	ULONG PeakWorkingSetSize;
	ULONG WorkingSetSize;
	ULONG QuotaPeakPagedPoolUsage;
	ULONG QuotaPagedPoolUsage;
	ULONG QuotaPeakNonPagedPoolUsage;
	ULONG QuotaNonPagedPoolUsage;
	ULONG PagefileUsage;
	ULONG PeakPagefileUsage;
}VM_COUNTERS, * PVM_COUNTERS;

typedef struct _SYSTEM_THREADS
{
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG         WaitTime;
	PVOID         StartAddress;
	CLIENT_ID     ClientId;
	KPRIORITY     Priority;
	KPRIORITY     BasePriority;
	ULONG         ContextSwitchCount;
	THREAD_STATE  State;
	KWAIT_REASON  WaitReason;
}SYSTEM_THREADS, * PSYSTEM_THREADS;

typedef struct _SYSTEM_PROCESSES
{
	ULONG          NextEntryDelta;
	ULONG          ThreadCount;
	ULONG          Reserved1[6];
	LARGE_INTEGER  CreateTime;
	LARGE_INTEGER  UserTime;
	LARGE_INTEGER  KernelTime;
	UNICODE_STRING ProcessName;
	KPRIORITY      BasePriority;
	ULONG          ProcessId;
	ULONG          InheritedFromProcessId;
	ULONG          HandleCount;
	ULONG          Reserved2[2];
	VM_COUNTERS    VmCounters;
	IO_COUNTERS    IoCounters;
	SYSTEM_THREADS Threads[1];
}SYSTEM_PROCESSES, * PSYSTEM_PROCESSES;

typedef struct _SYSTEM_MODULE_INFORMATION_ENTRY {
	HANDLE Section;
	PVOID MappedBase;
	PVOID Base;
	ULONG Size;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT PathLength;
	CHAR ImageName[256];
} SYSTEM_MODULE_INFORMATION_ENTRY, * PSYSTEM_MODULE_INFORMATION_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION {
	ULONG Count;
	SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

#define NT_SUCCESS(Status)((NTSTATUS)(Status)>=0)
#define STATUS_SUCCESS 0x00000000
#define STATUS_UNSUCCESSFUL 0xC0000001
#define STATUS_NOT_IMPLEMENTED 0xC0000002
#define STATUS_INFO_LENGTH_MISMATCH 0xC0000004
//#define STATUS_INVALID_PARAMETER 0xC000000D
#define STATUS_ACCESS_DENIED 0xC0000022
#define STATUS_BUFFER_TOO_SMALL 0xC0000023
#define OBJ_KERNEL_HANDLE 0x00000200

#endif // ZwQuerySystemInformation_H_