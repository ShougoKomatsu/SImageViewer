// SettingDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "SettingDlg.h"
#include "afxdialogex.h"


// CSettingDlg ダイアログ

IMPLEMENT_DYNAMIC(CSettingDlg, CDialogEx)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingDlg::IDD, pParent)
	, m_sEditMaxFile(_T(""))
	, m_sEditMaxDepth(_T(""))
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SETTING_EDIT_MAX_FILE, m_sEditMaxFile);
	DDX_Text(pDX, IDC_SETTING_EDIT_MAX_DEPTH, m_sEditMaxDepth);
    DDX_Control(pDX, IDC_SETTING_EDIT_MAX_FILE, m_editMaxFile);
    DDX_Control(pDX, IDC_SETTING_EDIT_MAX_DEPTH, m_editMaxDepth);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSettingDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSettingDlg メッセージ ハンドラー


BOOL CSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	((CButton*)(GetDlgItem(IDC_SETTING_CHECK_EXIT_BY_ESC)))->SetCheck(m_bExitByEsc);
	((CButton*)(GetDlgItem(IDC_SETTING_CHECK_CENTERIZE_FULLSCREEN)))->SetCheck(m_bCenteredWhenFullScreen);
	((CButton*)(GetDlgItem(IDC_SETTING_CHECK_PPFW_WITHOUT_CTRL)))->SetCheck(m_bPPFWwithoutCtrlWhenFullScreen);
	m_sEditMaxFile.Format(_T("%d"),m_iMaxFile);
	m_sEditMaxDepth.Format(_T("%d"),m_iMaxDepth);
	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CSettingDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	m_bExitByEsc=(((CButton*)(GetDlgItem(IDC_SETTING_CHECK_EXIT_BY_ESC)))->GetCheck() == TRUE) ? true : false;
	m_bCenteredWhenFullScreen=(((CButton*)(GetDlgItem(IDC_SETTING_CHECK_CENTERIZE_FULLSCREEN)))->GetCheck() == TRUE) ? true : false;
	m_bPPFWwithoutCtrlWhenFullScreen=(((CButton*)(GetDlgItem(IDC_SETTING_CHECK_PPFW_WITHOUT_CTRL)))->GetCheck() == TRUE) ? true : false;
	m_iMaxFile=_ttoi(m_sEditMaxFile);
	m_iMaxDepth=_ttoi(m_sEditMaxDepth);

	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnOK();
}
