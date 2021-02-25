#include "pch.h"
#include "CCtrl.h"

CCtrl gcCtrl;
CCtrl::CCtrl()
	:pcView(0),pcR3R0(0)
{
	pcData = &gcData;
}

CCtrl::~CCtrl()
{
	delete this->pcR3R0;
	delete this->pcView;
}

BOOL CCtrl::InitCtrl(CMyView* pcView)
{
	this->pcView = pcView;
	this->pcR3R0 = new CR3R0();
	
	return TRUE;
}

void CCtrl::TreeClick(CTreeCtrl* cTree, HTREEITEM hTree)
{
	auto& tLeafs = this->pcView->m_tLeafs;
	auto& tInfo = tLeafs[hTree];
	if (tInfo.uiDeep != 1)	return;
	else if (tInfo.str == gszTreeCtrl遍历进程)	//遍历进程
	{
		puts("用户点击了遍历进程。");
		auto& vPIDs = this->pcData->vPIDs;
		if (this->pcR3R0->GetPIDs(this->pcData->vPIDs))
		{
			this->pcView->InitList(11);
			this->pcView->InitList(vPIDs);
		}
		else
		{
			MessageBoxA(0, "获取进程失败。", 0, 0);
		}
	}
}
