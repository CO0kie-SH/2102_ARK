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

// 接受R3指令分发
ULONG_PTR WriteDisp(LPCH FunName);
// 向R3发送数据包
ULONG_PTR ReadDisp(LPCH FunName, LPMyInfoSend pInfo);

// 遍历PIDs
ULONG_PTR GetPIDs(ULONG MaxBuff,ULONG MaxPID, LPMyProcess pPID);



