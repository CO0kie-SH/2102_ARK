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
	BOOL GetSyss(vector<MySys>& vSYSs);
	BOOL GetPath(vector<MyPath>& vPATHs);
	BOOL GetIDTs(CListCtrl* pvList);
	BOOL GetGDTs(CListCtrl* pvList);
	BOOL GetSSDT(CListCtrl* pvList);
	BOOL SetSYSf();
	BOOL SYSHOOK();
	BOOL HidePID();
	BOOL ExitPID(CEditView* pvEdit);
	BOOL CrtFile();
	BOOL DelFile();
	BOOL RLoadNT();
	BOOL SSDTHOK();
private:
};

