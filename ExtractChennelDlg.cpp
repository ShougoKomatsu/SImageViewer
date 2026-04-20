// ExtractChennelDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "ExtractChennelDlg.h"
#include "afxdialogex.h"


// CExtractChennelDlg ダイアログ

IMPLEMENT_DYNAMIC(CExtractChennelDlg, CDialogEx)

CExtractChennelDlg::CExtractChennelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CExtractChennelDlg::IDD, pParent)
{

}

CExtractChennelDlg::~CExtractChennelDlg()
{
}

void CExtractChennelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExtractChennelDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CExtractChennelDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CExtractChennelDlg メッセージ ハンドラー


void CExtractChennelDlg::OnBnClickedOk()
{
	
	if(((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_R))->GetCheck() == true){m_enumColor=COLOR_RED;};
	if(((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_G))->GetCheck() == true){m_enumColor=COLOR_GREEN;};
	if(((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_B))->GetCheck() == true){m_enumColor=COLOR_BLUE;};
	CDialogEx::OnOK();
}


BOOL CExtractChennelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_R))->SetCheck(true); 
	((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_G))->SetCheck(false); 
	((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_B))->SetCheck(false); 

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
