#include "ZwQuerySystemInformation.h"

typedef NTSTATUS(WINAPI* PFN_NTQUERYSYSTEMINFORMATION)(SYSTEM_INFORMATION_CLASS, PVOID, ULONG, PULONG);

//************************************
// Method:    ZwQuerySystemInformation
// Parameter: SYSTEM_INFORMATION_CLASS SystemInformationClass
// Parameter: PVOID SystemInformation
// Parameter: ULONG SystemInformationLength
// Parameter: PULONG ReturnLength
//************************************
NTSTATUS WINAPI ZwQuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength)
{
	HMODULE hNtDll = LoadLibraryA("ntdll.dll");
	if (hNtDll == 0)	ExitProcess(0);
	PFN_NTQUERYSYSTEMINFORMATION pfnZwQuerySystemInformation = NULL;	// ZwQuerySystemInformation
	pfnZwQuerySystemInformation = (PFN_NTQUERYSYSTEMINFORMATION)GetProcAddress(hNtDll, "ZwQuerySystemInformation");
	if (pfnZwQuerySystemInformation == NULL)
	{
		DWORD dwErrCode = GetLastError();
		return -1;
	}
	return pfnZwQuerySystemInformation(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
}