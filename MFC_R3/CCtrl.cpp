#include "pch.h"
#include "CCtrl.h"
CCtrl gcCtrl;

CCtrl::CCtrl()
	:pcView(0)
{
}

CCtrl::~CCtrl()
{
	OutputDebugString(L"~CCtrl()\n");
	delete this->pcView;
}

BOOL CCtrl::InitCtrl(CMyView* pcView)
{
	this->pcView = pcView;
	return TRUE;
}
