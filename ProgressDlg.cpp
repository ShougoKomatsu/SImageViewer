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
	, m_sStatic(_T(""))
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
	DDX_Text(pDX, IDC_PROGRESS_STATIC, m_sStatic);
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
	m_sStatic.Format(_T("中断処理をしています"));
	UpdateData(FALSE);
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
	this->GetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CProgressDlg::OnBnClickedCancel()
{
	m_iStatus=0;
	m_sStatic.Format(_T("中断処理をしています"));
	UpdateData(FALSE);
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
}

void CProgressDlg::SetText(const CString sText)
{
	if(m_iStatus==0){return;}
	m_sStatic.Format(_T("%s"), sText);
	UpdateData(FALSE);
}


void CProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialogEx::OnTimer(nIDEvent);
}
