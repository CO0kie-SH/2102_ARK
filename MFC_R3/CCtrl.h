#pragma once
#include "CMyView.h"

class CCtrl
{
public:
	CMyView*	pcView;
	CData*		pcData;
	CR3R0*		pcR3R0;
public:
	CCtrl();
	~CCtrl();

	BOOL InitCtrl(CMyView* pcView);
	void TreeClick(CTreeCtrl* cTree, HTREEITEM hTree);
private:

};

extern CCtrl gcCtrl;
