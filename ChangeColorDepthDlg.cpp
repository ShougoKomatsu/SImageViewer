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
	ON_BN_CLICKED(IDC_CHANGE_COLOR_DEPTH_RADIO_1, &CChangeColorDepthDlg::OnBnClickedChangeColorDepthRadio1)
	ON_BN_CLICKED(IDC_CHANGE_COLOR_DEPTH_RADIO_4, &CChangeColorDepthDlg::OnBnClickedChangeColorDepthRadio4)
	ON_BN_CLICKED(IDC_CHANGE_COLOR_DEPTH_RADIO_8, &CChangeColorDepthDlg::OnBnClickedChangeColorDepthRadio8)
	ON_BN_CLICKED(IDC_CHANGE_COLOR_DEPTH_RADIO_24, &CChangeColorDepthDlg::OnBnClickedChangeColorDepthRadio24)
	ON_BN_CLICKED(IDC_CHANGE_COLOR_DEPTH_RADIO_32, &CChangeColorDepthDlg::OnBnClickedChangeColorDepthRadio32)
END_MESSAGE_MAP()


// CChangeColorDepthDlg メッセージ ハンドラー

void CChangeColorDepthDlg::GetSetting(int* iBPP, int* iMode)
{
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_1))->GetCheck()==TRUE){*iBPP=1;}
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_4))->GetCheck()==TRUE){*iBPP=4;}
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_8))->GetCheck()==TRUE){*iBPP=8;}
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_24))->GetCheck()==TRUE){*iBPP=24;}
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_32))->GetCheck()==TRUE){*iBPP=32;}

	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_LOSSLESS))->GetCheck()==TRUE){*iMode=0;}
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_AREA))->GetCheck()==TRUE){*iMode=1;}
	if(((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_DEVIATION))->GetCheck()==TRUE){*iMode=2;}
	}
void CChangeColorDepthDlg::OnBnClickedOk()
{
	GetSetting(&m_iBPP, &m_iMode);
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


void CChangeColorDepthDlg::SetEnableByBPP(const int iBPP)
{	
	if(m_iColors <= 1<<iBPP)
	{
		((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_LOSSLESS))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_LOSSLESS))->SetCheck(true);
		((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_AREA))->SetCheck(false);
		((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_DEVIATION))->SetCheck(false);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_LOSSLESS))->SetCheck(false);
		((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_LOSSLESS))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_CHANGE_COLOR_DEPTH_RADIO_AREA))->SetCheck(true);
}
}
void CChangeColorDepthDlg::OnBnClickedChangeColorDepthRadio1(){SetEnableByBPP(1);}
void CChangeColorDepthDlg::OnBnClickedChangeColorDepthRadio4(){SetEnableByBPP(4);}
void CChangeColorDepthDlg::OnBnClickedChangeColorDepthRadio8(){SetEnableByBPP(8);}
void CChangeColorDepthDlg::OnBnClickedChangeColorDepthRadio24(){SetEnableByBPP(24);}
void CChangeColorDepthDlg::OnBnClickedChangeColorDepthRadio32(){SetEnableByBPP(32);}
