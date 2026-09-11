// ColorizeDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "ColorizeDlg.h"
#include "afxdialogex.h"


// CColorizeDlg ダイアログ

IMPLEMENT_DYNAMIC(CColorizeDlg, CDialogEx)

CColorizeDlg::CColorizeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CColorizeDlg::IDD, pParent)
{

}

CColorizeDlg::~CColorizeDlg()
{
}

void CColorizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CColorizeDlg, CDialogEx)
END_MESSAGE_MAP()


// CColorizeDlg メッセージ ハンドラー
