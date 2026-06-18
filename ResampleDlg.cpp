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
END_MESSAGE_MAP()


// CResampleDlg メッセージ ハンドラー


BOOL CResampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ここに初期化を追加してください
	((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_1))->SetCheck(true);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
