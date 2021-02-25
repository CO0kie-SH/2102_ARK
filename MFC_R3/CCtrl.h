#pragma once
#include "CMyView.h"


class CCtrl
{
public:
	CMyView* pcView;
public:
	CCtrl();
	~CCtrl();

	BOOL InitCtrl(CMyView* pcView);
private:

};

extern CCtrl gcCtrl;