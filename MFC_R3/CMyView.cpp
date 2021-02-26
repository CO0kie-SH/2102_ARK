#include "pch.h"
#include "CMyView.h"

CMyView::CMyView(CDialogEx* pDlg)
	:mListLastId(0)
{
	//初始化
	puts("CMyView()");
	this->pcData = &gcData;
	this->pcData->hMFC = pDlg->GetSafeHwnd();
	this->pcDlg = pDlg;

	//获取控件
	this->pvTree = (CTreeCtrl*)pDlg->GetDlgItem(IDC_TREE1);
	this->pvList = (CListCtrl*)pDlg->GetDlgItem(IDC_LIST1);
	this->pvEdit = (CEditView*)pDlg->GetDlgItem(IDC_EDIT1);
	this->pvBttn = (CButton*)pDlg->GetDlgItem(IDC_BUTTON1);

	//初始化控件
	mStr.Format(L"MFC_R3 PID[%lu]TID[%lu]HWND[%lu]",
		this->pcData->mPID,
		this->pcData->mTID,
		(DWORD)this->pcData->hMFC);
	::SetWindowTextW(this->pcData->hMFC, mStr);
	this->pvList->SetExtendedStyle(
		/*LVS_EX_CHECKBOXES |*/ //这是复选框
		LVS_EX_GRIDLINES |
		LVS_EX_FULLROWSELECT |
		LVS_EX_GRIDLINES);
	this->InitTree(0);
}

CMyView::~CMyView()
{
}


void CMyView::InitTree(HTREEITEM hTree)
{
	CTreeCtrl &vTree = *this->pvTree;
	MyTreeInfo& nowTree = m_tLeafs[hTree];
	MyTreeInfo tmp;

	if (hTree == 0)
	{
		m_tLeafs.clear();
		for (DWORD i = 0; i < gnbTreeFunctions; i++)
		{
			tmp = { 0,i + 1,gszTreeFunctions[i] };
			tmp.htTree = vTree.InsertItem(tmp.str);
			m_tLeafs[tmp.htTree] = tmp;
			this->InitTree(tmp.htTree);
		}
		puts("树根初始化完毕。");
		return;
	}

	switch (nowTree.uiNumb)
	{
	case 1: {	//进程功能区
		for (DWORD i = 0; i < gnbTreeFunctions1; i++)
		{
			tmp = { nowTree.uiDeep + 1,i + 1,gszTreeFunctions1[i], hTree };
			tmp.htTree = vTree.InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}break;
	case 2: {	//驱动功能区
		for (DWORD i = 0; i < gnbTreeFunctions2; i++)
		{
			tmp = { nowTree.uiDeep + 1,i + 1,gszTreeFunctions2[i], hTree };
			tmp.htTree = vTree.InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}break;
	case 3: {	//文件功能区
		for (DWORD i = 0; i < gnbTreeFunctions3; i++)
		{
			tmp = { nowTree.uiDeep + 1,i + 1,gszTreeFunctions3[i], hTree };
			tmp.htTree = vTree.InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}break;
	case 5: {	//IDT功能区
		for (DWORD i = 0; i < gnbTreeFunctions5; i++)
		{
			tmp = { nowTree.uiDeep + 1,i + 1,gszTreeFunctions5[i], hTree };
			tmp.htTree = vTree.InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}break;
	case 6: {	//GDT功能区
		for (DWORD i = 0; i < gnbTreeFunctions6; i++)
		{
			tmp = { nowTree.uiDeep + 1,i + 1,gszTreeFunctions6[i], hTree };
			tmp.htTree = vTree.InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}break;
	case 7: {	//SSDT功能区
		for (DWORD i = 0; i < gnbTreeFunctions7; i++)
		{
			tmp = { nowTree.uiDeep + 1,i + 1,gszTreeFunctions7[i], hTree };
			tmp.htTree = vTree.InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}break;
	case 8: {	//HOOK功能区
		for (DWORD i = 0; i < gnbTreeFunctions8; i++)
		{
			tmp = { nowTree.uiDeep + 1,i + 1,gszTreeFunctions8[i], hTree };
			tmp.htTree = vTree.InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}break;
	}
}

void CMyView::InitList(DWORD ID)
{
	if (mListLastId == ID)	return;
	mListLastId = ID;

	while (this->pvList->DeleteColumn(0));		//清空列
	this->pvList->InsertColumn(0, _T("备注"), LVCFMT_LEFT, 222);
	switch (ID) {
	case 11:									//遍历进程
		this->pvList->InsertColumn(0, _T("进程路径"), LVCFMT_LEFT, 222);
		this->pvList->InsertColumn(0, _T("进程名"), LVCFMT_LEFT, 123);
		this->pvList->InsertColumn(0, _T("PPID"), LVCFMT_LEFT, 55);
		this->pvList->InsertColumn(0, _T("PID"), LVCFMT_LEFT, 55);
		break;
	case 12:									//遍历线程
		this->pvList->InsertColumn(0, _T("线程ETHREAD"), LVCFMT_LEFT, 123);
		//this->pvList->InsertColumn(0, _T("模块名"), LVCFMT_LEFT, 123);
		this->pvList->InsertColumn(0, _T("TID"), LVCFMT_LEFT, 88);
		this->pvList->InsertColumn(0, _T("PID"), LVCFMT_LEFT, 66);
		break;
	case 13:									//遍历模块
		this->pvList->InsertColumn(0, _T("模块路径"), LVCFMT_LEFT, 222);
		//this->pvList->InsertColumn(0, _T("模块名"), LVCFMT_LEFT, 123);
		this->pvList->InsertColumn(0, _T("BASE"), LVCFMT_LEFT, 123);
		this->pvList->InsertColumn(0, _T("PID"), LVCFMT_LEFT, 66);
		break;
	case 21:									//遍历驱动
		this->pvList->InsertColumn(0, _T("驱动路径"), LVCFMT_LEFT, 222);
		this->pvList->InsertColumn(0, _T("驱动名"), LVCFMT_LEFT, 123);
		this->pvList->InsertColumn(0, _T("BASE"), LVCFMT_LEFT, 123);
		this->pvList->InsertColumn(0, _T("nLdr"), LVCFMT_LEFT, 123);
		break;
	case 31:									//遍历文件
		this->pvList->InsertColumn(0, _T("创建日期"), LVCFMT_LEFT, 222);
		this->pvList->InsertColumn(0, _T("修改日期"), LVCFMT_LEFT, 222);
		this->pvList->InsertColumn(0, _T("文件大小"), LVCFMT_LEFT, 100);
		this->pvList->InsertColumn(0, _T("文件名"), LVCFMT_LEFT, 123);
		this->pvList->InsertColumn(0, _T("文件属性"), LVCFMT_LEFT, 88);
		this->pvList->InsertColumn(0, _T("类型"), LVCFMT_LEFT, 55);
		break;
	case 51:									//遍历IDT
		this->pvList->DeleteAllItems();			//清空行
		this->pvList->InsertColumn(0, _T("DPL"), LVCFMT_LEFT, 50);
		this->pvList->InsertColumn(0, _T("GateType"), LVCFMT_LEFT, 99);
		this->pvList->InsertColumn(0, _T("selector"), LVCFMT_LEFT, 99);
		this->pvList->InsertColumn(0, _T("BASE"), LVCFMT_LEFT, 123);
		break;
	case 61:									//遍历GDT
		this->pvList->DeleteAllItems();			//清空行
		this->pvList->InsertColumn(0, _T("G"), LVCFMT_LEFT, 22);
		this->pvList->InsertColumn(0, _T("S"), LVCFMT_LEFT, 22);
		this->pvList->InsertColumn(0, _T("P"), LVCFMT_LEFT, 22);
		this->pvList->InsertColumn(0, _T("偏移"), LVCFMT_LEFT, 99);
		this->pvList->InsertColumn(0, _T("类型"), LVCFMT_LEFT, 77);
		this->pvList->InsertColumn(0, _T("INFO"), LVCFMT_LEFT, 177);
		this->pvList->InsertColumn(0, _T("Addr"), LVCFMT_LEFT, 123);
		break;
	case 71:									//遍历GDT
		this->pvList->DeleteAllItems();			//清空行
		this->pvList->InsertColumn(0, _T("Addr"), LVCFMT_LEFT, 123);
		break;
	default:
		break;
	}
	this->pvList->InsertColumn(0, _T("序"), LVCFMT_LEFT, 55);
}

void CMyView::InitList(vector<MyProcess3>& vPIDs, DWORD ID, DWORD PID)
{
	DWORD pMax = vPIDs.size();
	CString str;
	this->pvList->DeleteAllItems();		//清空行

	switch (ID)
	{
	//遍历进程
	case 11: {
		for (DWORD i = 0; i < pMax; i++)
		{
			auto& Info = vPIDs[i];
			str.Format(_T("%lu"), i + 1);
			this->pvList->InsertItem(i, str);
			str.Format(_T("%lu"), Info.tPID);
			this->pvList->SetItemText(i, 1, str);
			str.Format(_T("%lu"), Info.pPID);
			this->pvList->SetItemText(i, 2, str);
			this->pvList->SetItemText(i, 3, Info.szExe);
			this->pvList->SetItemText(i, 4, Info.szPath);
		}
	}break;
	//遍历线程
	case 12: {
		for (DWORD i = 0; i < pMax; i++)
		{
			auto& Info = vPIDs[i];
			if (Info.tPID != PID)	continue;
			auto& Ths = Info.vTHs;
			printf("线程数：%lu\n", Ths.size());
			for (DWORD j = 0, max = Ths.size(); j < max; j++)
			{
				auto& Th = Ths[j];
				str.Format(_T("%lu"), j + 1);
				this->pvList->InsertItem(j, str);
				str.Format(_T("%lu"), PID);
				this->pvList->SetItemText(j, 1, str);
				str.Format(_T("%lu"), Th.TID);
				this->pvList->SetItemText(j, 2, str);
				str.Format(_T("0x%p"), Th.pETHREAD);
				this->pvList->SetItemText(j, 3, str);
			}
			break;
		}
	}break;
	//遍历模块
	case 13: {
		for (DWORD i = 0; i < pMax; i++)
		{
			auto& Info = vPIDs[i];
			if (Info.tPID != PID)	continue;
			auto& Mds = Info.vMDs;
			for (DWORD j = 0, max = Mds.size(); j < max; j++)
			{
				auto& Md = Mds[j];
				str.Format(_T("%lu"), j + 1);
				this->pvList->InsertItem(j, str);
				str.Format(_T("%lu"), PID);
				this->pvList->SetItemText(j, 1, str);
				str.Format(_T("0x%08lX"), Md.Base);
				this->pvList->SetItemText(j, 2, str);
				this->pvList->SetItemText(j, 3, Md.str);
			}
		}
	}
	default:
		break;
	}

}

void CMyView::InitList(vector<MySys>& vSYSs)
{
	DWORD sMax = vSYSs.size();
	CString str;
	this->pvList->DeleteAllItems();		//清空行

	for (DWORD i = 0; i < sMax; i++)
	{
		auto& SYS = vSYSs[i];
		str.Format(_T("%lu"), i + 1);
		this->pvList->InsertItem(i, str);
		str.Format(_T("0x%08lX"), SYS.pNextLdr);
		this->pvList->SetItemText(i, 1, str);
		str.Format(_T("0x%08lX"), SYS.Base);
		this->pvList->SetItemText(i, 2, str);
		this->pvList->SetItemText(i, 3, SYS.szExe);
		this->pvList->SetItemText(i, 4, SYS.szPath);
	}
}

void SetStrTime(LARGE_INTEGER* pTime, CString& str)
{
	//char buf[128];
		//SYSTEMTIME st;
		//GetSystemTime(&st);
		//FILETIME ft;
		//SystemTimeToFileTime(&st, &ft);
		//FILETIME转SYSTEMTIME
	PFILETIME pft = (PFILETIME)pTime;
	SYSTEMTIME st2;
	FileTimeToSystemTime(pft, &st2);
	st2.wHour += 8;
	//memset(buf, 0, 128);
	//sprintf_s(buf, "%04d-%02d-%02d %02d:%02d:%02d:%03d", st2.wYear, st2.wMonth, st2.wDay, st2.wHour, st2.wMinute, st2.wSecond, st2.wMilliseconds);
	//printf("时间：%s\n", buf);
	str.Format(L"%04d-%02d-%02d %02d:%02d:%02d:%03d", st2.wYear, st2.wMonth, st2.wDay, st2.wHour, st2.wMinute, st2.wSecond, st2.wMilliseconds);
	//str.Format(_T("%llX"), PATH.CreationTime.QuadPart);
}

void CMyView::InitList(vector<MyPath> vPATHs)
{
	DWORD pMax = vPATHs.size();
	CString str;
	this->pvList->DeleteAllItems();		//清空行

	for (DWORD i = 0; i < pMax; i++)
	{
		auto& PATH = vPATHs[i];
		str.Format(_T("%lu"), i + 1);
		this->pvList->InsertItem(i, str);
		str.Format(_T("%s"),
			PATH.FileAttributes & FILE_ATTRIBUTE_DIRECTORY
			? L"目录" : L"文件");
		this->pvList->SetItemText(i, 1, str);
		str.Format(_T("%lX"), PATH.FileAttributes);
		this->pvList->SetItemText(i, 2, str);
		this->pvList->SetItemText(i, 3, PATH.szPath);
		SetStrTime(&PATH.LastWriteTime, str);
		this->pvList->SetItemText(i, 5, str);
		SetStrTime(&PATH.CreationTime, str);
		this->pvList->SetItemText(i, 6, str);
		if (PATH.EndOfFile.LowPart)
		{
			str.Format(_T("%llu字节"), PATH.EndOfFile.QuadPart);
			this->pvList->SetItemText(i, 4, str);
		}
	}
}
