// SetSelectionDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "SetSelectionDlg.h"
#include "afxdialogex.h"


// CSetSelectionDlg ダイアログ

IMPLEMENT_DYNAMIC(CSetSelectionDlg, CDialogEx)

CSetSelectionDlg::CSetSelectionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetSelectionDlg::IDD, pParent)
	, m_sEditTop(_T(""))
	, m_sEditLeft(_T(""))
	, m_sEditRight(_T(""))
	, m_sEditBottom(_T(""))
{
	m_iR0=0;
	m_iC0=0;
	m_iR1=0;
	m_iC1=0;
}

CSetSelectionDlg::~CSetSelectionDlg()
{
}

void CSetSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SET_SELECTION_EDIT_TOP, m_sEditTop);
	DDX_Text(pDX, IDC_SET_SELECTION_EDIT_LEFT, m_sEditLeft);
	DDX_Text(pDX, IDC_SET_SELECTION_EDIT_RIGHT, m_sEditRight);
	DDX_Text(pDX, IDC_SET_SELECTION_EDIT_BOTTOM, m_sEditBottom);
}


BEGIN_MESSAGE_MAP(CSetSelectionDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSetSelectionDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSetSelectionDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSetSelectionDlg メッセージ ハンドラー


void CSetSelectionDlg::OnBnClickedOk()
{
	
	UpdateData(TRUE);
	m_iR0=min(_ttoi(m_sEditTop),_ttoi(m_sEditBottom));
	m_iR1=max(_ttoi(m_sEditTop),_ttoi(m_sEditBottom));

	m_iC0=min(_ttoi(m_sEditLeft),_ttoi(m_sEditRight));
	m_iC1=max(_ttoi(m_sEditLeft),_ttoi(m_sEditRight));

	CDialogEx::OnOK();
}


void CSetSelectionDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


BOOL CSetSelectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_sEditTop.Format(_T("%d"), m_iR0);
	m_sEditLeft.Format(_T("%d"), m_iC0);
	m_sEditBottom.Format(_T("%d"), m_iR1);
	m_sEditRight.Format(_T("%d"), m_iC1);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
