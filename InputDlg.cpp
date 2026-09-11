// InputDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "InputDlg.h"
#include "afxdialogex.h"


// CInputDlg ダイアログ

IMPLEMENT_DYNAMIC(CInputDlg, CDialogEx)

CInputDlg::CInputDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInputDlg::IDD, pParent)
	, m_sEditInput(_T(""))
{

}

CInputDlg::~CInputDlg()
{
}

void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INPUT_EDIT_INPUT, m_sEditInput);
}


BEGIN_MESSAGE_MAP(CInputDlg, CDialogEx)
END_MESSAGE_MAP()


// CInputDlg メッセージ ハンドラー


BOOL CInputDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_INPUT_EDIT_INPUT)->SetFocus();
	((CEdit*)GetDlgItem(IDC_INPUT_EDIT_INPUT))->SetSel(0, -1);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


BOOL CInputDlg::PreTranslateMessage(MSG* pMsg)
{
	

		if (pMsg->message == WM_KEYDOWN)
		{	
			if(pMsg->wParam == VK_RETURN)
			{
				this->OnOK();
			}
		}

	return CDialogEx::PreTranslateMessage(pMsg);
}
