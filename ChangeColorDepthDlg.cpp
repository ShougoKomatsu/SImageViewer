// ChangeColorDepthDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "ChangeColorDepthDlg.h"
#include "afxdialogex.h"


// CChangeColorDepthDlg ダイアログ

IMPLEMENT_DYNAMIC(CChangeColorDepthDlg, CDialogEx)

CChangeColorDepthDlg::CChangeColorDepthDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CChangeColorDepthDlg::IDD, pParent)
{

}

CChangeColorDepthDlg::~CChangeColorDepthDlg()
{
}

void CChangeColorDepthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CChangeColorDepthDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CChangeColorDepthDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CChangeColorDepthDlg メッセージ ハンドラー


void CChangeColorDepthDlg::OnBnClickedOk()
{
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_8_LOSSLESS))->GetCheck()==TRUE){m_iMode=0;}
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_8_AREA))->GetCheck()==TRUE){m_iMode=1;}
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_8_DEVIATION))->GetCheck()==TRUE){m_iMode=2;}
	CDialogEx::OnOK();
}
