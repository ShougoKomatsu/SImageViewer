// FormatSelectionoDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "FormatSelectionDlg.h"
#include "afxdialogex.h"


// CFormatSelectionDlg ダイアログ

IMPLEMENT_DYNAMIC(CFormatSelectionDlg, CDialogEx)

CFormatSelectionDlg::CFormatSelectionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFormatSelectionDlg::IDD, pParent)
	, m_sEditIsMonochrome(_T(""))
	, m_sBPP(_T(""))
{

}

CFormatSelectionDlg::~CFormatSelectionDlg()
{
}

void CFormatSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SELECT_FORMAT_MONO, m_sEditIsMonochrome);
	DDX_Text(pDX, IDC_SELECT_FORMAT_BPP, m_sBPP);
}


BEGIN_MESSAGE_MAP(CFormatSelectionDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CFormatSelectionDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CFormatSelectionDlg メッセージ ハンドラー


BOOL CFormatSelectionDlg::OnInitDialog()
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
	m_sBPP.Format(_T("%d"),m_iBPP);

	UpdateData(FALSE);
	// TODO:  ここに初期化を追加してください

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CFormatSelectionDlg::OnBnClickedOk()
{
	if(((CButton*)GetDlgItem(IDC_SELECT_FORMAT_BMP24))->GetCheck()==TRUE){m_iFormat=0;m_iBPP=24;}
	if(((CButton*)GetDlgItem(IDC_SELECT_FORMAT_PNG24))->GetCheck()==TRUE){m_iFormat=1;m_iBPP=24;}
	CDialogEx::OnOK();
}
