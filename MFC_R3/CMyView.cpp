#include "pch.h"
#include "CMyView.h"

CMyView::CMyView(CDialogEx* pDlg)
	:mListLastId(0)
{
	//��ʼ��
	puts("CMyView()");
	this->pcData = &gcData;
	this->pcData->hMFC = pDlg->GetSafeHwnd();
	this->pcDlg = pDlg;

	//��ȡ�ؼ�
	this->pvTree = (CTreeCtrl*)pDlg->GetDlgItem(IDC_TREE1);
	this->pvList = (CListCtrl*)pDlg->GetDlgItem(IDC_LIST1);
	this->pvEdit = (CEditView*)pDlg->GetDlgItem(IDC_EDIT1);
	this->pvBttn = (CButton*)pDlg->GetDlgItem(IDC_BUTTON1);

	//��ʼ���ؼ�
	mStr.Format(L"MFC_R3 PID[%lu]TID[%lu]HWND[%lu]",
		this->pcData->mPID,
		this->pcData->mTID,
		(DWORD)this->pcData->hMFC);
	::SetWindowTextW(this->pcData->hMFC, mStr);
	this->pvList->SetExtendedStyle(
		/*LVS_EX_CHECKBOXES |*/ //���Ǹ�ѡ��
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
		puts("������ʼ����ϡ�");
		return;
	}

	switch (nowTree.uiNumb)
	{
	case 1: {	//���̹�����
		for (DWORD i = 0; i < gnbTreeFunctions1; i++)
		{
			tmp = { nowTree.uiDeep + 1,i + 1,gszTreeFunctions1[i], hTree };
			tmp.htTree = vTree.InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}return;
	default:
		break;
	}
}

void CMyView::InitList(DWORD ID)
{
	if (mListLastId == ID)	return;
	mListLastId = ID;

	while (this->pvList->DeleteColumn(0));		//�����
	this->pvList->InsertColumn(0, _T("��ע"), LVCFMT_LEFT, 222);
	switch (ID) {
	case 11:									//��������
		this->pvList->InsertColumn(0, _T("����·��"), LVCFMT_LEFT, 222);
		this->pvList->InsertColumn(0, _T("������"), LVCFMT_LEFT, 123);
		this->pvList->InsertColumn(0, _T("PPID"), LVCFMT_LEFT, 111);
		this->pvList->InsertColumn(0, _T("PID"), LVCFMT_LEFT, 111);
		break;
	default:
		break;
	}
	this->pvList->InsertColumn(0, _T("��"), LVCFMT_LEFT, 55);
}

void CMyView::InitList(vector<MyProcess3>& vPIDs)
{
	CString str;
	this->pvList->DeleteAllItems();		//�����
	for (DWORD i = 0,max = vPIDs.size(); i < max; i++)
	{
		auto& Info = vPIDs[i];
		str.Format(_T("%lu"), i + 1);
		this->pvList->InsertItem(i, str);
		str.Format(_T("%lu"), Info.tPID);
		this->pvList->SetItemText(i, 1, str);
		str.Format(_T("%lu"), Info.pPID);
		this->pvList->SetItemText(i, 2, str);
		this->pvList->SetItemText(i, 4, Info.Path);
	}
}
