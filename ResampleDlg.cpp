// ResampleDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "ResampleDlg.h"
#include "afxdialogex.h"


// CResampleDlg ダイアログ

IMPLEMENT_DYNAMIC(CResampleDlg, CDialogEx)

CResampleDlg::CResampleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CResampleDlg::IDD, pParent)
{

}

CResampleDlg::~CResampleDlg()
{
}

void CResampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CResampleDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CResampleDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CResampleDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CResampleDlg メッセージ ハンドラー


BOOL CResampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_1))->SetCheck(true);
	m_resample = RESAMPLE_NONE;
	return TRUE;  
}


void CResampleDlg::OnBnClickedOk()
{
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_1))->GetCheck() == true){m_resample = RESAMPLE_NONE;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_2))->GetCheck() == true){m_resample = RESAMPLE_2;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_3))->GetCheck() == true){m_resample = RESAMPLE_3;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_4))->GetCheck() == true){m_resample = RESAMPLE_4;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_2ND))->GetCheck() == true){m_resample = RESAMPLE_2ND;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_3RD))->GetCheck() == true){m_resample = RESAMPLE_3RD;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_4TH))->GetCheck() == true){m_resample = RESAMPLE_4TH;CDialogEx::OnOK();return;}

	CDialogEx::OnOK();
}


void CResampleDlg::OnBnClickedCancel()
{
	m_resample = RESAMPLE_NONE;
	CDialogEx::OnCancel();
}
