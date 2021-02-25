#pragma once

class CMyView
{
public:
	CData* pcData;
	CDialogEx* pcDlg;
	CTreeCtrl* pvTree;
	CListCtrl* pvList;
	CEditView* pvEdit;
	CButton* pvBttn;

	map<HTREEITEM, MyTreeInfo> m_tLeafs;	//定义树节点索引
public:
	CMyView(CDialogEx* pDlg);
	~CMyView();

	void InitTree(HTREEITEM hTree);
private:

};

