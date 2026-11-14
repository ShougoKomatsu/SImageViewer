// FilterSettingDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "FilterSettingDlg.h"
#include "afxdialogex.h"


// CFilterSettingDlg ダイアログ

IMPLEMENT_DYNAMIC(CFilterSettingDlg, CDialogEx)

CFilterSettingDlg::CFilterSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFilterSettingDlg::IDD, pParent)
	, m_sEditW(_T(""))
	, m_sEditH(_T(""))
{

}

CFilterSettingDlg::~CFilterSettingDlg()
{
}

void CFilterSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FILTER_EDIT_W, m_sEditW);
	DDX_Text(pDX, IDC_FILTER_EDIT_H, m_sEditH);
    DDX_Control(pDX, IDC_FILTER_EDIT_W, m_editW);
    DDX_Control(pDX, IDC_FILTER_EDIT_H, m_editH);
}


BEGIN_MESSAGE_MAP(CFilterSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CFilterSettingDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CFilterSettingDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CFilterSettingDlg メッセージ ハンドラー


void CFilterSettingDlg::OnBnClickedCancel()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnCancel();
}


void CFilterSettingDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnOK();
}
