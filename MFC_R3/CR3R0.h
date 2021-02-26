#pragma once
#include "CData.h"

class CR3R0
{
public:
	CData*	pcData;
	HANDLE	DeviceHandle;
	LPCH	pMem;
public:
	CR3R0();
	~CR3R0();

	BOOL GetPIDs(vector<MyProcess3>& vPIDs);
	BOOL GettPID(vector<MyProcess3>& vPIDs);
	BOOL GetTIDs(vector<MyProcess3>& vPIDs);
	BOOL GetMODs(vector<MyProcess3>& vPIDs, DWORD PID = 0);
private:
};

