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
	ON_BN_CLICKED(IDC_PASTE_AS_BUTTON_TSV, &CPasteAsDlg::OnBnClickedPasteAsButtonTsv)
	ON_BN_CLICKED(IDC_PASTE_AS_BUTTON_CSV, &CPasteAsDlg::OnBnClickedPasteAsButtonCsv)
	ON_BN_CLICKED(IDC_PASTE_AS_BUTTON_IMAGE, &CPasteAsDlg::OnBnClickedPasteAsButtonImage)
END_MESSAGE_MAP()


// CPasteAsDlg メッセージ ハンドラー


void CPasteAsDlg::OnBnClickedPasteAsButtonTsv()
{
	m_enumCopyMode=PASTE_AS_TSV;
	CDialogEx::OnOK();
}


void CPasteAsDlg::OnBnClickedPasteAsButtonCsv()
{
	m_enumCopyMode=PASTE_AS_CSV;
	CDialogEx::OnOK();
}

void CPasteAsDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


void CPasteAsDlg::OnBnClickedPasteAsButtonImage()
{
	m_enumCopyMode=PASTE_AS_IMAGE;
	CDialogEx::OnOK();
}