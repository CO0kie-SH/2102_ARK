#pragma once
#include <ntifs.h>

extern PDRIVER_OBJECT g_pDriver;
extern ULONG g_ReNt;
void UnHookNt();
void KernelReload();