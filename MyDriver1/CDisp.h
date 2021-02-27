#pragma once
#include <ntifs.h>
#include <ntddk.h>
#include "CMyFile.h"
#include "CReNT.h"
#include "CReg.h"


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

//���̲���
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

//���̻����飨��ΪWindows�ں���һ������PEB��Ϊ�˲��ض��壬���Ծ�����һ�����֣�
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

// LDR �����е�ÿһ�������ṹ�壬�����������Ļ�����Ϣ
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
	// ... ���滹��һЩ�ֶΣ������ò�����Ϊ�˽�ʡ��������ֱ�Ӳ�����
} *PLDR_DATA_TABLE_ENTRY;
#pragma pack(1)
typedef  struct  _KSYSTEM_SERVICE_TABLE
{
	PULONG  ServiceTableBase;   //������ַ����׵�ַ
	PULONG  ServiceCounterTableBase;// ��������ÿ�����������õĴ���
	ULONG   NumberOfService;// �������ĸ���, NumberOfService * 4 ����������ַ��Ĵ�С
	UCHAR* ParamTableBase; // �����������׵�ַ
} KSYSTEM_SERVICE_TABLE, * PKSYSTEM_SERVICE_TABLE;
typedef  struct  _KSERVICE_TABLE_DESCRIPTOR
{
	KSYSTEM_SERVICE_TABLE   ntoskrnl;// ntoskrnl.exe�ķ���������SSDT
	KSYSTEM_SERVICE_TABLE   win32k; // win32k.sys�ķ�����(GDI32.dll/User32.dll ���ں�֧��)����ShadowSSDT
	KSYSTEM_SERVICE_TABLE   notUsed1; // ��ʹ��
	KSYSTEM_SERVICE_TABLE   notUsed2; // ��ʹ��
}KSERVICE_TABLE_DESCRIPTOR, * PKSERVICE_TABLE_DESCRIPTOR;
#pragma pack()

#pragma region SSDTHOOK
#pragma pack(1)
typedef struct _ServiceDesriptorEntry
{
	ULONG* ServiceTableBase;        // ������ַ
	ULONG* ServiceCounterTableBase; // �������ַ
	ULONG NumberOfServices;         // ������ĸ���
	UCHAR* ParamTableBase;          // �������ַ
}SSDTEntry, * PSSDTEntry;
#pragma pack()
// ����ԭ��
typedef NTSTATUS(NTAPI* FnZwOpenProcess)(PHANDLE,
	ACCESS_MASK,
	POBJECT_ATTRIBUTES,
	PCLIENT_ID);
extern FnZwOpenProcess g_OldZwOpenProcess;

#pragma endregion


#define MAKELONG(a,b) ((LONG)(((UINT16)(((DWORD_PTR)(a))&0xffff)) | ((ULONG)((UINT16)(((DWORD_PTR)(b))& 0xffff)))<<16))
NTKERNELAPI PPEB_EX PsGetProcessPeb(PEPROCESS Process);

// ����R3ָ��ַ�
ULONG_PTR WriteDisp(LPCH FunName);
// ��R3�������ݰ�
ULONG_PTR ReadDisp(LPCH FunName, LPMyInfoSend pInfo);

// ����PIDs
ULONG_PTR GetPIDs(ULONG MaxBuff,ULONG MaxPID, LPMyProcess pPID);
// ����tPID
ULONG_PTR GettPID(ULONG PID, LPMyInfoSend pInfo);
// �����߳�
ULONG_PTR GetThID(ULONG MaxBuff, ULONG MaxTID, ULONG TID, LPMyThread pTID);
// ����ģ��
ULONG_PTR GetMods(ULONG MaxBuff, ULONG PID, ULONG List, LPMyInfoSend pInfo);
// ����ģ��2
ULONG_PTR GetMODs(LPMyInfoSend pInfo);
// ��������
ULONG_PTR GetSyss(ULONG MaxBuff, ULONG List, LPMyInfoSend pInfo);
// �����ļ�
ULONG_PTR GetPath(ULONG MaxBuff, ULONG Count, PIO_STATUS_BLOCK pIoStatusBlock, LPMyInfoSend pInfo);
// ����IDT
ULONG_PTR GetIDTs(LPMyInfoSend pInfo);
// ����GDT
ULONG_PTR GetGDTs(LPMyInfoSend pInfo);
// ����SSDT
ULONG_PTR GetSSDT(LPMyInfoSend pInfo);
// ��������
ULONG_PTR SetSYSf(LPMyInfoSend pInfo);
// HOOK
ULONG_PTR SYSHOOK(LPMyInfoSend pInfo);
// �����ļ�
ULONG_PTR CrtFile(LPMyInfoSend pInfo);
// ɾ���ļ�
ULONG_PTR DelFile(LPMyInfoSend pInfo);
// ���ؽ���
ULONG_PTR HidePID(LPMyInfoSend pInfo);
// ��������
ULONG_PTR ExitPID(LPMyInfoSend pInfo);
// �����ں�
ULONG_PTR RLoadNT(LPMyInfoSend pInfo);
// SSDTHOOK
ULONG_PTR SSDTHOK(LPMyInfoSend pInfo);

extern PDRIVER_OBJECT gDriverObject;
extern PVOID gSysFirst;
extern ULONG OriginalAddress;
VOID UnHook();
void UnInstallHook();