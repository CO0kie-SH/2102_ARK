
// MFC_R3Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFC_R3.h"
#include "MFC_R3Dlg.h"
#include "afxdialogex.h"
#include "CCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCR3Dlg 对话框



CMFCR3Dlg::CMFCR3Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_R3_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCR3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCR3Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CMFCR3Dlg::OnNMDblclkTree1)
END_MESSAGE_MAP()


// CMFCR3Dlg 消息处理程序

BOOL CMFCR3Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	gcCtrl.InitCtrl(new CMyView(this));
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCR3Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCR3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCR3Dlg::OnNMDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	if (pNMHDR->idFrom == IDC_TREE1) {
		CTreeCtrl* pTree = (CTreeCtrl*)GetDlgItem(IDC_TREE1);
		ASSERT_VALID(pTree);
		HTREEITEM hLeaf = pTree->GetSelectedItem();
		bool bHasLeaf = pTree->ItemHasChildren(hLeaf), bExpand = false;
		if (bHasLeaf) {
			bExpand = TVIS_EXPANDED &
				pTree->GetItemState(hLeaf, TVIS_EXPANDED);
		}
		gcCtrl.TreeClick(pTree, hLeaf);
	}
	*pResult = 0;
}
