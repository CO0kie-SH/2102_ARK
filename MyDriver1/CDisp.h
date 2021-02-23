#pragma once
#include <ntifs.h>
#include <ntddk.h>

typedef struct _MyInfoSend
{
	ULONG ulSize;
	ULONG ulNum1;
	ULONG ulNum2;
	ULONG ulNum3;
	CHAR byBuf1[40];
	CHAR byBuf2[40];
	CHAR byBuf3[4000];
}MyInfoSend, * LPMyInfoSend;


ULONG_PTR WriteDisp(LPCH FunName);
ULONG_PTR ReadDisp(LPCH FunName, LPMyInfoSend pInfo);



