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
	, m_sBPP(_T(""))
{

}

CFormatSelectionoDlg::~CFormatSelectionoDlg()
{
}

void CFormatSelectionoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FORMAT_SELECT_MONO, m_sEditIsMonochrome);
	DDX_Text(pDX, IDC_FORMAT_SELECT_BPP, m_sBPP);
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
	m_sBPP.Format(_T("%d"),m_iBPP);

	UpdateData(FALSE);
	// TODO:  ここに初期化を追加してください

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CFormatSelectionoDlg::OnBnClickedOk()
{
	if(((CButton*)GetDlgItem(IDC_FORMAT_SELECT_BMP24))->GetCheck()==TRUE){m_iFormat=0;m_iBPP=24;}
	if(((CButton*)GetDlgItem(IDC_FORMAT_SELECT_PNG24))->GetCheck()==TRUE){m_iFormat=1;m_iBPP=24;}
	CDialogEx::OnOK();
}
