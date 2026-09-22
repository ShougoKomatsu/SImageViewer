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
	ON_BN_CLICKED(IDC_COLORIZE_RADIO_DEMOSAIC, &CColorizeDlg::OnBnClickedColorizeRadioDemosaic)
	ON_BN_CLICKED(IDC_COLORIZE_RADIO_THRESHOLD, &CColorizeDlg::OnBnClickedColorizeRadioThreshold)
	ON_CBN_SELCHANGE(IDC_COLORIZE_COMBO_00, &CColorizeDlg::OnSelchangeColorizeCombo)
	ON_CBN_SELCHANGE(IDC_COLORIZE_COMBO_01, &CColorizeDlg::OnSelchangeColorizeCombo)
	ON_CBN_SELCHANGE(IDC_COLORIZE_COMBO_10, &CColorizeDlg::OnSelchangeColorizeCombo)
	ON_CBN_SELCHANGE(IDC_COLORIZE_COMBO_11, &CColorizeDlg::OnSelchangeColorizeCombo)
	ON_BN_CLICKED(IDC_COLORIZE_RADIO_RAINBOW, &CColorizeDlg::OnBnClickedColorizeRadioRainbow)
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
void CColorizeDlg::OperateRainbow()
{

	GrayToRainbow(&m_image, &m_imageColorized);
	m_pictureAfter.m_image.Set(IMAGE_TYPE_CIMAGE, NULL, NULL, 0, 0, &m_imageColorized, VALUE_IMAGE_CLIP_0_TO_255, _T("temp"));
	m_pictureAfter.Refresh();
}
void CColorizeDlg::OperateThreshold()
{
	UpdateData(TRUE);
	BYTE byMin = max(0, min(255, _ttoi(m_sEditThreshMin)));
	BYTE byMax = max(0, min(255, _ttoi(m_sEditThreshMax)));

	int iNeighbor =((((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_CONNECTION4)))->GetCheck() == TRUE) ? 4 : 8);

	ImgRGB imgRGB;
	_ConvertImage(&m_image,&imgRGB);
	ImgRGB imgGray;
	ImgRGB imgDummy; 
	Decompose3(&imgRGB, &imgGray, &imgDummy, &imgDummy, &imgDummy); 
	Object obj;
	Threshold( &imgGray, byMin, byMax, &obj);
	ImgRGB imgResult;

	if( ((CButton*)(GetDlgItem(IDC_COLORIZE_CHECK_CONNECTION)))->GetCheck() == FALSE)
	{
		PaintRegion(&imgRGB, &obj, &imgResult); 
	}
	else
	{
		Object objConnected;
		Connection(&obj, &objConnected, iNeighbor);
		PaintRegion(&imgRGB, &objConnected, &imgResult); 
	}
	ConvertImage(&imgResult, &m_imageColorized);
	m_pictureAfter.m_image.Set(IMAGE_TYPE_CIMAGE, NULL, NULL, 0, 0, &m_imageColorized, VALUE_IMAGE_CLIP_0_TO_255, _T("temp"));

	m_pictureAfter.Refresh();
}

void CColorizeDlg::OperateDemosaic()
{
	CString sText;
	COLOR_ELEMENT i00, i01, i10, i11;
	int iSel;
	iSel = ((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_00)))->GetCurSel();
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_00)))->GetLBText(iSel,sText);
	i00 = (sText.Compare(_T("R"))==0) ? COLOR_R : ((sText.Compare(_T("G"))==0) ? COLOR_G: COLOR_B);

	iSel = ((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_01)))->GetCurSel();
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_01)))->GetLBText(iSel,sText);
	i01 = (sText.Compare(_T("R"))==0) ? COLOR_R : ((sText.Compare(_T("G"))==0) ? COLOR_G: COLOR_B);

	iSel = ((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_10)))->GetCurSel();
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_10)))->GetLBText(iSel,sText);
	i10 = (sText.Compare(_T("R"))==0) ? COLOR_R : ((sText.Compare(_T("G"))==0) ? COLOR_G: COLOR_B);

	iSel = ((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_11)))->GetCurSel();
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_11)))->GetLBText(iSel,sText);
	i11 = (sText.Compare(_T("R"))==0) ? COLOR_R : ((sText.Compare(_T("G"))==0) ? COLOR_G: COLOR_B);

	Demosaic(&m_image, i00, i01, i10, i11, &m_imageColorized);
	m_pictureAfter.m_image.Set(IMAGE_TYPE_CIMAGE, NULL, NULL, 0, 0, &m_imageColorized, VALUE_IMAGE_CLIP_0_TO_255, _T("temp"));
	m_pictureAfter.Refresh();
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
	
	EnableDemosaic(false);
	EnableThreshold(true);

	((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_THRESHOLD)))->SetCheck(TRUE);
	((CButton*)(GetDlgItem(IDC_COLORIZE_CHECK_CONNECTION)))->SetCheck(FALSE);
	((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_CONNECTION4)))->SetCheck(TRUE);
	((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_CONNECTION8)))->SetCheck(FALSE);
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_00)))->AddString(_T("R"));
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_01)))->AddString(_T("R"));
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_10)))->AddString(_T("R"));
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_11)))->AddString(_T("R"));

	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_00)))->AddString(_T("G"));
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_01)))->AddString(_T("G"));
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_10)))->AddString(_T("G"));
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_11)))->AddString(_T("G"));

	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_00)))->AddString(_T("B"));
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_01)))->AddString(_T("B"));
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_10)))->AddString(_T("B"));
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_11)))->AddString(_T("B"));
	
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_00)))->SetCurSel(2);
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_01)))->SetCurSel(1);
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_10)))->SetCurSel(1);
	((CComboBox*)(GetDlgItem(IDC_COLORIZE_COMBO_11)))->SetCurSel(0);

	m_sEditThreshMin.Format(_T("0"));
	m_sEditThreshMax.Format(_T("0"));
	m_pictureBefore.iID=0;
	m_pictureAfter.iID=1;


	
	CopyImage_CImage(&m_image,&m_imageColorized);

	m_pictureBefore.m_image.Set(IMAGE_TYPE_CIMAGE, NULL, NULL, 0, 0, &m_image, VALUE_IMAGE_CLIP_0_TO_255, _T("temp"));
    m_pictureBefore.Refresh();
	
	m_pictureAfter.m_image.Set(IMAGE_TYPE_CIMAGE, NULL, NULL, 0, 0, &m_image, VALUE_IMAGE_CLIP_0_TO_255, _T("temp"));
    m_pictureAfter.Refresh();

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


void CColorizeDlg::EnableThreshold(bool bTF)
{	
	GetDlgItem(IDC_COLORIZE_EDIT_THRESH_MIN)->EnableWindow(bTF);
	GetDlgItem(IDC_COLORIZE_EDIT_THRESH_MAX)->EnableWindow(bTF);
	GetDlgItem(IDC_COLORIZE_CHECK_CONNECTION)->EnableWindow(bTF);

	GetDlgItem(IDC_COLORIZE_RADIO_CONNECTION4)->EnableWindow(FALSE);
	GetDlgItem(IDC_COLORIZE_RADIO_CONNECTION8)->EnableWindow(FALSE);
	if( ((CButton*)(GetDlgItem(IDC_COLORIZE_CHECK_CONNECTION)))->GetCheck() == TRUE)
	{
		GetDlgItem(IDC_COLORIZE_RADIO_CONNECTION4)->EnableWindow(bTF);
		GetDlgItem(IDC_COLORIZE_RADIO_CONNECTION8)->EnableWindow(bTF);
	}
	((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_THRESHOLD)))->SetCheck(bTF); 
}
void CColorizeDlg::EnableDemosaic(bool bTF)
{
	GetDlgItem(IDC_COLORIZE_COMBO_00)->EnableWindow(bTF);
	GetDlgItem(IDC_COLORIZE_COMBO_01)->EnableWindow(bTF);
	GetDlgItem(IDC_COLORIZE_COMBO_10)->EnableWindow(bTF);
	GetDlgItem(IDC_COLORIZE_COMBO_11)->EnableWindow(bTF);
	((CButton*)(GetDlgItem(IDC_COLORIZE_RADIO_DEMOSAIC)))->SetCheck(bTF); 
}


void CColorizeDlg::OnBnClickedColorizeRadioDemosaic()
{
	EnableDemosaic(true);
	EnableThreshold(false);
	OperateDemosaic();
}


void CColorizeDlg::OnBnClickedColorizeRadioThreshold()
{
	EnableDemosaic(false);
	EnableThreshold(true);
	OperateThreshold();
}

void CColorizeDlg::OnBnClickedColorizeRadioRainbow()
{
	EnableDemosaic(false);
	EnableThreshold(false);
	OperateRainbow();
}


void CColorizeDlg::OnSelchangeColorizeCombo()
{
	OperateDemosaic();
}

