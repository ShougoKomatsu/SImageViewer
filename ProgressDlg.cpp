// ProgressDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "ProgressDlg.h"
#include "afxdialogex.h"


// CProgressDlg ダイアログ

IMPLEMENT_DYNAMIC(CProgressDlg, CDialogEx)

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProgressDlg::IDD, pParent)
{
	
	m_iStatus=0;
     Create(CProgressDlg::IDD,pParent); 
}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
}

BEGIN_MESSAGE_MAP(CProgressDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CProgressDlg::OnBnClickedCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CProgressDlg メッセージ ハンドラー


void CProgressDlg::OnOK()
{
	DestroyWindow();
}
void CProgressDlg::OnCancel()
{
	m_iStatus=0;
	DestroyWindow();
}
void CProgressDlg::PostNcDestroy()
{
	delete this;
}
	void CProgressDlg::SetPos(int i0to100)
	{

		m_progress.SetPos(min(100, max(0, i0to100)));
		Invalidate();
	}
BOOL CProgressDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	m_iStatus=1;
	m_progress.SetRange(0, 100);
	m_progress.SetPos(0);

	ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CProgressDlg::OnBnClickedCancel()
{
	m_iStatus=0;
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
}


void CProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialogEx::OnTimer(nIDEvent);
}
