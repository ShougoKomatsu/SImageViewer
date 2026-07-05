// ExtractChennelDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "ExtractChannelDlg.h"
#include "afxdialogex.h"


// CExtractChannelDlg ダイアログ

IMPLEMENT_DYNAMIC(CExtractChannelDlg, CDialogEx)

CExtractChannelDlg::CExtractChannelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CExtractChannelDlg::IDD, pParent)
{

}

CExtractChannelDlg::~CExtractChannelDlg()
{
}

void CExtractChannelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExtractChannelDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CExtractChannelDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CExtractChannelDlg メッセージ ハンドラー


void CExtractChannelDlg::OnBnClickedOk()
{
	
	if(((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_R))->GetCheck() == TRUE){m_enumColor=COLOR_RED;};
	if(((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_G))->GetCheck() == TRUE){m_enumColor=COLOR_GREEN;};
	if(((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_B))->GetCheck() == TRUE){m_enumColor=COLOR_BLUE;};
	
	if(((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_R_GRAY))->GetCheck() == TRUE){m_enumColor=COLOR_RED_GRAY;};
	if(((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_G_GRAY))->GetCheck() == TRUE){m_enumColor=COLOR_GREEN_GRAY;};
	if(((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_B_GRAY))->GetCheck() == TRUE){m_enumColor=COLOR_BLUE_GRAY;};


	if(((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_HUE))->GetCheck() == TRUE){m_enumColor=COLOR_HUE;};
	if(((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_HUE180))->GetCheck() == TRUE){m_enumColor=COLOR_HUE180;};
	if(((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_SATURATION))->GetCheck() == TRUE){m_enumColor=COLOR_SATURATION;};
	if(((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_BRIGHTNESS))->GetCheck() == TRUE){m_enumColor=COLOR_BRIGHTNESS;};

	CDialogEx::OnOK();
}


BOOL CExtractChannelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_R))->SetCheck(TRUE); 
	((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_G))->SetCheck(FALSE); 
	((CButton*)GetDlgItem(IDC_EXTRACT_CHANNEL_RADIO_B))->SetCheck(FALSE); 

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
