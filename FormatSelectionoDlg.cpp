// FormatSelectionoDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "FormatSelectionoDlg.h"
#include "afxdialogex.h"


// CFormatSelectionoDlg ダイアログ

IMPLEMENT_DYNAMIC(CFormatSelectionoDlg, CDialogEx)

CFormatSelectionoDlg::CFormatSelectionoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFormatSelectionoDlg::IDD, pParent)
	, m_sEditIsMonochrome(_T(""))
{

}

CFormatSelectionoDlg::~CFormatSelectionoDlg()
{
}

void CFormatSelectionoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FORMAT_SELECT_MONO, m_sEditIsMonochrome);
}


BEGIN_MESSAGE_MAP(CFormatSelectionoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CFormatSelectionoDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CFormatSelectionoDlg メッセージ ハンドラー


BOOL CFormatSelectionoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if(m_bImageIsMonochrome == true)
	{
		m_sEditIsMonochrome.Format(_T("Yes"));
	}
	else
	{
		m_sEditIsMonochrome.Format(_T("No"));
	}

	UpdateData(FALSE);
	// TODO:  ここに初期化を追加してください

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CFormatSelectionoDlg::OnBnClickedOk()
{
	if(((CButton*)GetDlgItem(IDC_FORMAT_SELECT_BMP24))->GetCheck()==TRUE){m_iFormat=0;m_iBPP=24;}
	if(((CButton*)GetDlgItem(IDC_FORMAT_SELECT_BMP8))->GetCheck()==TRUE){m_iFormat=0;m_iBPP=8;}
	if(((CButton*)GetDlgItem(IDC_FORMAT_SELECT_BMP4))->GetCheck()==TRUE){m_iFormat=0;m_iBPP=4;}
	if(((CButton*)GetDlgItem(IDC_FORMAT_SELECT_BMP2))->GetCheck()==TRUE){m_iFormat=0;m_iBPP=2;}
	if(((CButton*)GetDlgItem(IDC_FORMAT_SELECT_BMP1))->GetCheck()==TRUE){m_iFormat=0;m_iBPP=1;}
	if(((CButton*)GetDlgItem(IDC_FORMAT_SELECT_PNG32))->GetCheck()==TRUE){m_iFormat=1;m_iBPP=32;}
	if(((CButton*)GetDlgItem(IDC_FORMAT_SELECT_PNG24))->GetCheck()==TRUE){m_iFormat=1;m_iBPP=24;}
	if(((CButton*)GetDlgItem(IDC_FORMAT_SELECT_PNG8))->GetCheck()==TRUE){m_iFormat=1;m_iBPP=8;}
	
	CDialogEx::OnOK();
}
