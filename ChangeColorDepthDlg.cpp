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
	, m_sEditBPP(_T(""))
	, m_sEditColors(_T(""))
	, m_sEditGrayScale(_T(""))
{

}

CChangeColorDepthDlg::~CChangeColorDepthDlg()
{
}

void CChangeColorDepthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CHANGE_COLOR_DEPTH_EDIT_BPP, m_sEditBPP);
	DDX_Text(pDX, IDC_CHANGE_COLOR_DEPTH_EDIT_COLORS, m_sEditColors);
	DDX_Text(pDX, IDC_CHANGE_COLOR_DEPTH_EDIT_GRAYSCALE, m_sEditGrayScale);
}


BEGIN_MESSAGE_MAP(CChangeColorDepthDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CChangeColorDepthDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CChangeColorDepthDlg メッセージ ハンドラー


void CChangeColorDepthDlg::OnBnClickedOk()
{
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_1))->GetCheck()==TRUE){m_iBPP=1;}
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_4))->GetCheck()==TRUE){m_iBPP=4;}
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_8))->GetCheck()==TRUE){m_iBPP=8;}
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_24))->GetCheck()==TRUE){m_iBPP=24;}
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_32))->GetCheck()==TRUE){m_iBPP=32;}

	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_LOSSLESS))->GetCheck()==TRUE){m_iMode=0;}
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_AREA))->GetCheck()==TRUE){m_iMode=1;}
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_DEVIATION))->GetCheck()==TRUE){m_iMode=2;}
	CDialogEx::OnOK();
}


BOOL CChangeColorDepthDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_sEditBPP.Format(_T("%d"),m_iBPP);
	m_sEditColors.Format(_T("%d"),m_iColors);
	m_sEditGrayScale.Format(_T("%s"),(m_bGrayScale == true ? _T("true"): _T("false")));

	((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_1))->SetCheck(false);
	((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_4))->SetCheck(false);
	((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_8))->SetCheck(false);
	((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_24))->SetCheck(false);
	((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_32))->SetCheck(false);

	
	((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_LOSSLESS))->SetCheck(true);
	((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_AREA))->SetCheck(false);
	((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_DEVIATION))->SetCheck(false);
	switch(m_iBPP)
	{
	case 1:{((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_1))->SetCheck(true);break;}
	case 4:{((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_4))->SetCheck(true);break;}
	case 8:{((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_8))->SetCheck(true);break;}
	case 24:{((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_24))->SetCheck(true);break;}
	case 32:{((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_32))->SetCheck(true);break;}
	}
	UpdateData(FALSE);
	// TODO:  ここに初期化を追加してください

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
