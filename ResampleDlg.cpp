// ResampleDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "ResampleDlg.h"
#include "afxdialogex.h"


// CResampleDlg ダイアログ

IMPLEMENT_DYNAMIC(CResampleDlg, CDialogEx)

CResampleDlg::CResampleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CResampleDlg::IDD, pParent)
{

}

CResampleDlg::~CResampleDlg()
{
}

void CResampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RESAMPLE_EDIT_WIDTH, m_sEditWidth);
	DDX_Text(pDX, IDC_RESAMPLE_EDIT_HEIGHT, m_sEditHeight);
	DDX_Text(pDX, IDC_RESAMPLE_EDIT_WIDTH_ORG, m_sEditWidthOrg);
	DDX_Text(pDX, IDC_RESAMPLE_EDIT_HEIGHT_ORG, m_sEditHeightOrg);
}


BEGIN_MESSAGE_MAP(CResampleDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CResampleDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CResampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RESAMPLE_RADIO_RESIZE_NEAREST, &CResampleDlg::OnBnClickedResampleRadioResizeNearest)
	ON_BN_CLICKED(IDC_RESAMPLE_RADIO_RESIZE_BILINEAR, &CResampleDlg::OnBnClickedResampleRadioResizeBilinear)
	ON_BN_CLICKED(IDC_RESAMPLE_RADIO_4, &CResampleDlg::OnBnClickedResampleRadio4)
	ON_BN_CLICKED(IDC_RESAMPLE_RADIO_3, &CResampleDlg::OnBnClickedResampleRadio3)
	ON_BN_CLICKED(IDC_RESAMPLE_RADIO_2, &CResampleDlg::OnBnClickedResampleRadio2)
	ON_BN_CLICKED(IDC_RESAMPLE_RADIO_1, &CResampleDlg::OnBnClickedResampleRadio1)
	ON_BN_CLICKED(IDC_RESAMPLE_RADIO_2ND, &CResampleDlg::OnBnClickedResampleRadio2nd)
	ON_BN_CLICKED(IDC_RESAMPLE_RADIO_3RD, &CResampleDlg::OnBnClickedResampleRadio3rd)
	ON_BN_CLICKED(IDC_RESAMPLE_RADIO_4TH, &CResampleDlg::OnBnClickedResampleRadio4th)
END_MESSAGE_MAP()


// CResampleDlg メッセージ ハンドラー


BOOL CResampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_1))->SetCheck(true);
	m_resample = RESAMPLE_NONE;
	ChangeMode(RESAMPLE_NONE);
	m_sEditWidthOrg.Format(_T("%d"), m_iWidthOrg);
	m_sEditHeightOrg.Format(_T("%d"), m_iHeightOrg);
	m_sEditWidth.Format(_T("%d"), m_iWidthOrg);
	m_sEditHeight.Format(_T("%d"), m_iHeightOrg);
	UpdateData(FALSE);
	return TRUE;  
}


void CResampleDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_1))->GetCheck() == TRUE){m_resample = RESAMPLE_NONE;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_2))->GetCheck() == TRUE){m_resample = RESAMPLE_2;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_3))->GetCheck() == TRUE){m_resample = RESAMPLE_3;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_4))->GetCheck() == TRUE){m_resample = RESAMPLE_4;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_2ND))->GetCheck() == TRUE){m_resample = RESAMPLE_2ND;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_3RD))->GetCheck() == TRUE){m_resample = RESAMPLE_3RD;CDialogEx::OnOK();return;}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_4TH))->GetCheck() == TRUE){m_resample = RESAMPLE_4TH;CDialogEx::OnOK();return;}
	
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_RESIZE_NEAREST))->GetCheck() == TRUE)
	{
		m_resample = RESIZE_NEAREST;
		m_iWidth = _ttoi((LPCTSTR)m_sEditWidth);
		m_iHeight = _ttoi((LPCTSTR)m_sEditHeight);
		CDialogEx::OnOK();
		return;
	}
	if( ((CButton*)GetDlgItem(IDC_RESAMPLE_RADIO_RESIZE_BILINEAR))->GetCheck() == TRUE)
	{
		m_resample = RESIZE_BILINEAR;
		m_iWidth = _ttoi((LPCTSTR)m_sEditWidth);
		m_iHeight = _ttoi((LPCTSTR)m_sEditHeight);
		CDialogEx::OnOK();
		return;
	}

	CDialogEx::OnOK();
}


void CResampleDlg::OnBnClickedCancel()
{
	m_resample = RESAMPLE_NONE;
	CDialogEx::OnCancel();
}


void CResampleDlg::ChangeMode(RESAMPLE resample)
{
	switch(resample)
	{
	case RESAMPLE_NONE:
	case RESAMPLE_2:
	case RESAMPLE_3:
	case RESAMPLE_4:
	case RESAMPLE_2ND:
	case RESAMPLE_3RD:
	case RESAMPLE_4TH:
		{
			((CEdit*)(GetDlgItem(IDC_RESAMPLE_EDIT_WIDTH)))->SetReadOnly(TRUE);
			((CEdit*)(GetDlgItem(IDC_RESAMPLE_EDIT_HEIGHT)))->SetReadOnly(TRUE);
			switch(resample)
			{
			case RESAMPLE_NONE:{m_sEditHeight.Format(_T("%d"), m_iHeightOrg);m_sEditWidth.Format(_T("%d"), m_iWidthOrg);break;}
			case RESAMPLE_2:{m_sEditHeight.Format(_T("%d"), (m_iHeightOrg+1)/2);m_sEditWidth.Format(_T("%d"), (m_iWidthOrg+1)/2);break;}
			case RESAMPLE_3:{m_sEditHeight.Format(_T("%d"), (m_iHeightOrg+2)/3);m_sEditWidth.Format(_T("%d"), (m_iWidthOrg+2)/3);break;}
			case RESAMPLE_4:{m_sEditHeight.Format(_T("%d"), (m_iHeightOrg+3)/4);m_sEditWidth.Format(_T("%d"), (m_iWidthOrg+3)/4);break;}
			case RESAMPLE_2ND:{m_sEditHeight.Format(_T("%d"), m_iHeightOrg*2);m_sEditWidth.Format(_T("%d"), m_iWidthOrg*2);break;}
			case RESAMPLE_3RD:{m_sEditHeight.Format(_T("%d"), m_iHeightOrg*3);m_sEditWidth.Format(_T("%d"), m_iWidthOrg*3);break;}
			case RESAMPLE_4TH:{m_sEditHeight.Format(_T("%d"), m_iHeightOrg*4);m_sEditWidth.Format(_T("%d"), m_iWidthOrg*4);break;}
			}
			break;
		}
	case RESIZE_NEAREST:
	case RESIZE_BILINEAR:
		{
			((CEdit*)(GetDlgItem(IDC_RESAMPLE_EDIT_WIDTH)))->SetReadOnly(FALSE);
			((CEdit*)(GetDlgItem(IDC_RESAMPLE_EDIT_HEIGHT)))->SetReadOnly(FALSE);
			break;
		}
	default :{break;}
	}
	UpdateData(FALSE);
}
void CResampleDlg::OnBnClickedResampleRadioResizeNearest()
{
	ChangeMode(RESIZE_NEAREST);
}


void CResampleDlg::OnBnClickedResampleRadioResizeBilinear()
{
	ChangeMode(RESIZE_BILINEAR);
}


void CResampleDlg::OnBnClickedResampleRadio4()
{
	ChangeMode(RESAMPLE_4);
}


void CResampleDlg::OnBnClickedResampleRadio3()
{
	ChangeMode(RESAMPLE_3);
}


void CResampleDlg::OnBnClickedResampleRadio2()
{
	ChangeMode(RESAMPLE_2);
}


void CResampleDlg::OnBnClickedResampleRadio1()
{
	ChangeMode(RESAMPLE_NONE);
}


void CResampleDlg::OnBnClickedResampleRadio2nd()
{
	ChangeMode(RESAMPLE_2ND);
}


void CResampleDlg::OnBnClickedResampleRadio3rd()
{
	ChangeMode(RESAMPLE_3RD);
}


void CResampleDlg::OnBnClickedResampleRadio4th()
{
	ChangeMode(RESAMPLE_4TH);
}
