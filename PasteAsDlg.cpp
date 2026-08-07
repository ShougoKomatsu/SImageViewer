// PasteAsDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "PasteAsDlg.h"
#include "afxdialogex.h"

// CPasteAsDlg ダイアログ

IMPLEMENT_DYNAMIC(CPasteAsDlg, CDialogEx)

CPasteAsDlg::CPasteAsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPasteAsDlg::IDD, pParent)
{

}

CPasteAsDlg::~CPasteAsDlg()
{
}

void CPasteAsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPasteAsDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CPasteAsDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CPasteAsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPasteAsDlg メッセージ ハンドラー


void CPasteAsDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

void CPasteAsDlg::OnBnClickedOk()
{
	if( ((CButton*)GetDlgItem(IDC_RADIO_PASTE_AS_TSV))->GetCheck() == TRUE)
	{
		m_enumPasteFrom=PASTE_FROM_TSV;
	}
	
	if( ((CButton*)GetDlgItem(IDC_RADIO_PASTE_AS_CSV))->GetCheck() == TRUE)
	{
		m_enumPasteFrom=PASTE_FROM_CSV;
	}
	if( ((CButton*)GetDlgItem(IDC_RADIO_PASTE_AS_0TO255))->GetCheck() == TRUE)
	{
		m_enumPasteAs=VALUE_IMAGE_0_TO_255;
	}
	if( ((CButton*)GetDlgItem(IDC_RADIO_PASTE_AS_RAINBOW))->GetCheck() == TRUE)
	{
		m_enumPasteAs=VALUE_IMAGE_RAINBOW;
	}
	CDialogEx::OnOK();
}


BOOL CPasteAsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	((CButton*)GetDlgItem(IDC_RADIO_PASTE_AS_TSV))->SetCheck(true);
	((CButton*)GetDlgItem(IDC_RADIO_PASTE_AS_0TO255))->SetCheck(true);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
