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
	m_sMessage=_T("");
	m_bDispButton[0]=true;
	m_bDispButton[1]=false;
	m_bDispButton[2]=true;
m_sButton[0]=_T("OK");
m_sButton[1]=_T("");
m_sButton[2]=_T("Cancel");
m_bDispInput=true;

}

CInputDlg::~CInputDlg()
{
}

void CInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INPUT_EDIT_INPUT, m_sEditInput);
    DDX_Control(pDX, IDC_INPUT_EDIT_INPUT, m_editInput);
}


BEGIN_MESSAGE_MAP(CInputDlg, CDialogEx)
	ON_BN_CLICKED(IDC_INPUT_BUTTON_1, &CInputDlg::OnBnClickedInputButton1)
	ON_BN_CLICKED(IDC_INPUT_BUTTON_2, &CInputDlg::OnBnClickedInputButton2)
	ON_BN_CLICKED(IDC_INPUT_BUTTON_3, &CInputDlg::OnBnClickedInputButton3)
END_MESSAGE_MAP()


// CInputDlg メッセージ ハンドラー


BOOL CInputDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GetDlgItem(IDC_INPUT_EDIT_INPUT)->ShowWindow(m_bDispInput);
	GetDlgItem(IDC_INPUT_BUTTON_1)->ShowWindow(m_bDispButton[0]);
	GetDlgItem(IDC_INPUT_BUTTON_2)->ShowWindow(m_bDispButton[1]);
	GetDlgItem(IDC_INPUT_BUTTON_3)->ShowWindow(m_bDispButton[2]);
	GetDlgItem(IDC_INPUT_BUTTON_1)->SetWindowText((LPCTSTR)(m_sButton[0]));
	GetDlgItem(IDC_INPUT_BUTTON_2)->SetWindowText((LPCTSTR)(m_sButton[1]));
	GetDlgItem(IDC_INPUT_BUTTON_3)->SetWindowText((LPCTSTR)(m_sButton[2]));

	GetDlgItem(IDC_INPUT_STATIC_MESSAGE)->SetWindowText((LPCTSTR)(m_sMessage));

	m_editInput.SelectAll();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


BOOL CInputDlg::PreTranslateMessage(MSG* pMsg)
{
	

		if (pMsg->message == WM_KEYDOWN)
		{	
			if(pMsg->wParam == VK_RETURN)
			{
				m_iReturnCode=1;
				this->OnOK();
			}
			if(pMsg->wParam == VK_RETURN)
			{
				m_iReturnCode=3;
				this->OnCancel();
			}
		}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CInputDlg::OnBnClickedInputButton1(){m_iReturnCode=1;this->OnOK();}
void CInputDlg::OnBnClickedInputButton2(){m_iReturnCode=2;this->OnOK();}
void CInputDlg::OnBnClickedInputButton3(){m_iReturnCode=3;this->OnOK();}
