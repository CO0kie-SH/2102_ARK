#include "pch.h"
#include "CMyView.h"

CMyView::CMyView(CDialogEx* pDlg)
{
	//��ʼ��
	OutputDebugString(L"CMyView()\n");
	this->pcData = &gcData;
	this->pcData->hMFC = pDlg->GetSafeHwnd();
	this->pcDlg = pDlg;

	//��ȡ�ؼ�
	this->pvTree = (CTreeCtrl*)pDlg->GetDlgItem(IDC_TREE1);
	this->pvList = (CListCtrl*)pDlg->GetDlgItem(IDC_LIST1);
	this->pvEdit = (CEditView*)pDlg->GetDlgItem(IDC_EDIT1);
	this->pvBttn = (CButton*)pDlg->GetDlgItem(IDC_BUTTON1);

	//��ʼ���ؼ�
	this->InitTree(0);
}

CMyView::~CMyView()
{
	OutputDebugString(L"~CMyView()\n");
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
			tmp = { 0,i + 1,gszTreeFunctions1[i], hTree };
			tmp.htTree = vTree.InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
	}return;
	default:
		break;
	}
}
