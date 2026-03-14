// CopyAsDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "CopyAsDlg.h"
#include "afxdialogex.h"


// CCopyAsDlg ダイアログ

IMPLEMENT_DYNAMIC(CCopyAsDlg, CDialogEx)

CCopyAsDlg::CCopyAsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCopyAsDlg::IDD, pParent)
{

}

CCopyAsDlg::~CCopyAsDlg()
{
}

void CCopyAsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCopyAsDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CCopyAsDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_CSV, &CCopyAsDlg::OnBnClickedButtonCsv)
	ON_BN_CLICKED(IDC_BUTTON_TSV, &CCopyAsDlg::OnBnClickedButtonTsv)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE, &CCopyAsDlg::OnBnClickedButtonImage)
END_MESSAGE_MAP()


// CCopyAsDlg メッセージ ハンドラー


void CCopyAsDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


void CCopyAsDlg::OnBnClickedButtonCsv()
{
	m_enumCopyMode=COPY_AS_CSV;
	CDialogEx::OnOK();
}


void CCopyAsDlg::OnBnClickedButtonTsv()
{
	m_enumCopyMode=COPY_AS_TSV;
	CDialogEx::OnOK();
}


void CCopyAsDlg::OnBnClickedButtonImage()
{
	m_enumCopyMode=COPY_AS_IMAGE;
}
