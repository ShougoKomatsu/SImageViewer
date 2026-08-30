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
	, m_sEditImageType(_T(""))
{

}

CPasteAsDlg::~CPasteAsDlg()
{
}

void CPasteAsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PASTE_AS_EDIT_IMAGE_TYPE, m_sEditImageType);
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
	if( ((CButton*)GetDlgItem(IDC_PASTE_AS_RADIO_CLIP_0TO255))->GetCheck() == TRUE)
	{
		m_enumPasteAs=VALUE_IMAGE_CLIP_0_TO_255;
	}
	if( ((CButton*)GetDlgItem(IDC_PASTE_AS_RADIO_RAINBOW))->GetCheck() == TRUE)
	{
		m_enumPasteAs=VALUE_IMAGE_RAINBOW;
	}
	if( ((CButton*)GetDlgItem(IDC_PASTE_AS_RADIO_RESCALE_0TO255))->GetCheck() == TRUE)
	{
		m_enumPasteAs=VALUE_IMAGE_RESCALE_0_TO_255;
	}
	CDialogEx::OnOK();
}


BOOL CPasteAsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if(m_sEditImageType.Compare(_T("Value"))==0)
	{
		((CButton*)GetDlgItem(IDC_PASTE_AS_RADIO_CLIP_0TO255))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_PASTE_AS_RADIO_RESCALE_0TO255))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_PASTE_AS_RADIO_RAINBOW))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_PASTE_AS_RADIO_CLIP_0TO255))->SetCheck(true);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_PASTE_AS_RADIO_CLIP_0TO255))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_PASTE_AS_RADIO_RESCALE_0TO255))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_PASTE_AS_RADIO_RAINBOW))->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
