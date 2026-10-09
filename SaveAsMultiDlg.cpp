// SaveAsMultiDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "SaveAsMultiDlg.h"
#include "afxdialogex.h"


// CSaveAsMultiDlg ダイアログ

IMPLEMENT_DYNAMIC(CSaveAsMultiDlg, CDialogEx)

CSaveAsMultiDlg::CSaveAsMultiDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSaveAsMultiDlg::IDD, pParent)
	, m_sComboExt(_T(""))
{

}

CSaveAsMultiDlg::~CSaveAsMultiDlg()
{
}

void CSaveAsMultiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_SAVE_AS_MULTI_COMBO_EXY, m_sComboExt);
}


BEGIN_MESSAGE_MAP(CSaveAsMultiDlg, CDialogEx)
END_MESSAGE_MAP()


// CSaveAsMultiDlg メッセージ ハンドラー


BOOL CSaveAsMultiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	((CComboBox*)GetDlgItem(IDD_DLG_SAVE_AS_MULTI))->ResetContent();
	((CComboBox*)GetDlgItem(IDD_DLG_SAVE_AS_MULTI))->AddString(_T("元ファイルと同じ"));
	((CComboBox*)GetDlgItem(IDD_DLG_SAVE_AS_MULTI))->AddString(_T("png"));
	((CComboBox*)GetDlgItem(IDD_DLG_SAVE_AS_MULTI))->AddString(_T("bmp"));
	// TODO:  ここに初期化を追加してください

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
