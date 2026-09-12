// ColorizeDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "ColorizeDlg.h"
#include "afxdialogex.h"


// CColorizeDlg ダイアログ

IMPLEMENT_DYNAMIC(CColorizeDlg, CDialogEx)

CColorizeDlg::CColorizeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CColorizeDlg::IDD, pParent)
	, m_sEditThreshMin(_T(""))
	, m_sEditThreshMax(_T(""))
{

}

CColorizeDlg::~CColorizeDlg()
{
}

void CColorizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COLORIZE_STATIC_BEFORE, m_pictureBefore);
	DDX_Control(pDX, IDC_COLORIZE_STATIC_AFTER, m_pictureAfter);
	DDX_Text(pDX, IDC_COLORIZE_EDIT_THRESH_MIN, m_sEditThreshMin);
	DDX_Text(pDX, IDC_COLORIZE_EDIT_THRESH_MAX, m_sEditThreshMax);
}


BEGIN_MESSAGE_MAP(CColorizeDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CColorizeDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_COLORIZE_EDIT_THRESH_MIN, &CColorizeDlg::OnChangeColorizeEditValue1)
	ON_EN_CHANGE(IDC_COLORIZE_EDIT_THRESH_MAX, &CColorizeDlg::OnChangeColorizeEditValue2)
	ON_BN_CLICKED(IDC_COLORIZE_CHECK_CONNECTION, &CColorizeDlg::OnBnClickedColorizeCheckConnection)
	ON_BN_CLICKED(IDC_COLORIZE_RADIO_CONNECTION4, &CColorizeDlg::OnBnClickedColorizeRadioConnection4)
	ON_BN_CLICKED(IDC_COLORIZE_RADIO_CONNECTION8, &CColorizeDlg::OnBnClickedColorizeRadioConnection8)
END_MESSAGE_MAP()


// CColorizeDlg メッセージ ハンドラー


void CColorizeDlg::OnBnClickedOk()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnOK();
}



void CColorizeDlg::UpdateResultImage(CImage* imgResult)
{	
}
void CColorizeDlg::OperateThreshold()
{
	UpdateData(TRUE);
	BYTE byMin = max(0, min(255, _ttoi(m_sEditThreshMin)));
	BYTE byMax = max(0, min(255, _ttoi(m_sEditThreshMax)));

	if( ((CButton*)(GetDlgItem(IDC_COLORIZE_CHECK_CONNECTION)))->GetCheck() == FALSE)
	{
		Threshold(&m_image,&m_imageColorized, byMin, byMax, 255, 0, 0);
	}
	else
	{
		int iNeighbor =((((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_CONNECTION4)))->GetCheck() == TRUE) ? 4 : 8);

		ImgRGB imgRGB;
		_ConvertImage(&m_image,&imgRGB);
		ImgRGB imgGray;
		ImgRGB imgDummy; 
		Decompose3(&imgRGB, &imgGray, &imgDummy, &imgDummy, &imgDummy); 
		Object obj;
		Threshold( &imgGray, byMin, byMax, &obj);
		Object objConnected;
		Connection(&obj, &objConnected, iNeighbor);
		ImgRGB imgResult;
		PaintRegion(&imgRGB, &objConnected, &imgResult); 
		ConvertImage(&imgResult, &m_imageColorized);
	}
	CopyImage_CImage(&m_imageColorized, &m_pictureAfter.m_image);
	m_pictureAfter.Invalidate(FALSE);
}

void CColorizeDlg::OnChangeColorizeEditValue1()
{
	if( ((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_THRESHOLD)))->GetCheck() == TRUE){OperateThreshold(); return;}
}


void CColorizeDlg::OnChangeColorizeEditValue2()
{
	if( ((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_THRESHOLD)))->GetCheck() == TRUE){OperateThreshold(); return;}
}


BOOL CColorizeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_THRESHOLD)))->SetCheck(TRUE);
	((CButton*)(GetDlgItem(IDC_COLORIZE_CHECK_CONNECTION)))->SetCheck(FALSE);
	((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_CONNECTION4)))->SetCheck(TRUE);
	((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_CONNECTION8)))->SetCheck(FALSE);
	GetDlgItem(IDC_COLORIZE_RADIO_CONNECTION4)->EnableWindow(FALSE);
	GetDlgItem(IDC_COLORIZE_RADIO_CONNECTION8)->EnableWindow(FALSE);
	m_sEditThreshMin.Format(_T("0"));
	m_sEditThreshMax.Format(_T("0"));
	
	CopyImage_CImage(&m_image,&m_imageColorized);

    CopyImage_CImage(&m_image,&m_pictureBefore.m_image);
    m_pictureBefore.Invalidate(FALSE);
	
    CopyImage_CImage(&m_image,&m_pictureAfter.m_image);
    m_pictureAfter.Invalidate(FALSE);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CColorizeDlg::OnBnClickedColorizeCheckConnection()
{
	BOOL bChecked = ((CButton*)(GetDlgItem(IDC_COLORIZE_CHECK_CONNECTION)))->GetCheck();
	GetDlgItem(IDC_COLORIZE_RADIO_CONNECTION4)->EnableWindow(bChecked);
	GetDlgItem(IDC_COLORIZE_RADIO_CONNECTION8)->EnableWindow(bChecked);
	if( ((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_THRESHOLD)))->GetCheck() == TRUE){OperateThreshold(); return;}
}


void CColorizeDlg::OnBnClickedColorizeRadioConnection4()
{
	if( ((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_THRESHOLD)))->GetCheck() == TRUE){OperateThreshold(); return;}
}


void CColorizeDlg::OnBnClickedColorizeRadioConnection8()
{
	if( ((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_THRESHOLD)))->GetCheck() == TRUE){OperateThreshold(); return;}
}
