#pragma once


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



#pragma region IDT表结构体
#pragma warning(disable:4214)

typedef struct _IDT_INFO
{
	UINT16 uIdtLimit;
	UINT16 uLowIdtBase;
	UINT16 uHighIdtBase;
}IDT_INFO, * PIDT_INFO;
typedef struct _IDT_ENTRY
{
	UINT16 uSelector;
	UINT16 uOffsetLow;
	UINT16 uOffsetHigh;
	UINT8 GateType : 4;
	UINT8 DPL : 2;
}IDT_ENTRY, * PIDT_ENTRY;

typedef struct _MyIDT
{
	LPCH		Addr[0x100];
	IDT_ENTRY	IDT[0x100];
}MyIDT, * LPMyIDT;
#pragma endregion
