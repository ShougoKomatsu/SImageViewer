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
	m_dZoom = 1.0;
	return TRUE;  
}


void CResampleDlg::OnBnClickedOk()
{
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_1))->GetCheck() == true){m_dZoom=1.0;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_2))->GetCheck() == true){m_dZoom=2.0;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_3))->GetCheck() == true){m_dZoom=3.0;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_4))->GetCheck() == true){m_dZoom=4.0;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_2ND))->GetCheck() == true){m_dZoom=0.50;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_3RD))->GetCheck() == true){m_dZoom=0.33;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_4TH))->GetCheck() == true){m_dZoom=0.25;CDialogEx::OnOK();return;}

	CDialogEx::OnOK();
}


void CResampleDlg::OnBnClickedCancel()
{
	m_dZoom = 1.0;
	CDialogEx::OnCancel();
}
