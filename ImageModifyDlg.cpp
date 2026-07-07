// ImageModifyDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "ImageModifyDlg.h"
#include "afxdialogex.h"


// CImageModifyDlg ダイアログ

IMPLEMENT_DYNAMIC(CImageModifyDlg, CDialogEx)

CImageModifyDlg::CImageModifyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImageModifyDlg::IDD, pParent)
	, m_sEditBrightness(_T(""))
{
}

CImageModifyDlg::~CImageModifyDlg()
{
}

void CImageModifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_MODIFY_SLIDER_BRIGHTNESS, m_sliderBrightness);
	DDX_Control(pDX, IDC_IMAGE_MODIFY_SLIDER_CONTRAST, m_sliderContrast);
	DDX_Control(pDX, IDC_IMAGE_MODIFY_SLIDER_GAMMA, m_sliderGamma);
	DDX_Text(pDX, IDC_IMAGE_MODIFY_EDIT_BRIGHTNESS, m_sEditBrightness);
	DDX_Text(pDX, IDC_IMAGE_MODIFY_EDIT_CONTRAST, m_sEditContrast);
	DDX_Text(pDX, IDC_IMAGE_MODIFY_EDIT_GAMMA, m_sEditGamma);
    DDX_Control(pDX, IDC_IMAGE_MODIFY_STATIC_BEFORE, m_pictureBefore);
    DDX_Control(pDX, IDC_IMAGE_MODIFY_STATIC_AFTER, m_pictureAfter);
}
BEGIN_MESSAGE_MAP(CImageModifyDlg, CDialogEx)

	ON_BN_CLICKED(IDC_IMAGE_MODIFY_BUTTON_OK, &CImageModifyDlg::OnBnClickedDlgImageModifyButtonOk)
	ON_BN_CLICKED(IDC_IMAGE_MODIFY_BUTTON_CANCEL, &CImageModifyDlg::OnBnClickedDlgImageModifyButtonCancel)

	ON_NOTIFY(NM_CUSTOMDRAW, IDC_IMAGE_MODIFY_SLIDER_BRIGHTNESS, &CImageModifyDlg::OnCustomdrawSliderImageModifyBrightness)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_IMAGE_MODIFY_SLIDER_CONTRAST, &CImageModifyDlg::OnCustomdrawSliderImageModifyContrast)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_IMAGE_MODIFY_SLIDER_GAMMA, &CImageModifyDlg::OnCustomdrawSliderImageModifyGamma)
	ON_EN_KILLFOCUS(IDC_IMAGE_MODIFY_EDIT_BRIGHTNESS, &CImageModifyDlg::OnKillfocusEditImageModifyBrightness)
	ON_EN_KILLFOCUS(IDC_IMAGE_MODIFY_EDIT_CONTRAST, &CImageModifyDlg::OnKillfocusEditImageModifyContrast)
	ON_EN_KILLFOCUS(IDC_IMAGE_MODIFY_EDIT_GAMMA, &CImageModifyDlg::OnKillfocusEditImageModifyGamma)
END_MESSAGE_MAP()


// CImageModifyDlg メッセージ ハンドラー


void CImageModifyDlg::OnBnClickedDlgImageModifyButtonOk()
{

	m_iRet=IDOK;
	CDialogEx::OnOK();
}

void CImageModifyDlg::OnBnClickedDlgImageModifyButtonCancel()
{
	m_iRet=IDCANCEL;
	CDialogEx::OnOK();
}


BOOL CImageModifyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_sliderBrightness.SetRangeMin(0);	
	m_sliderBrightness.SetRangeMax(510);
	m_sliderBrightness.SetPos(255);
	m_sEditBrightness=_T("0");
	
	m_sliderContrast.SetRangeMin(0);	
	m_sliderContrast.SetRangeMax(90);
	m_sliderContrast.SetPos(45);
	m_sEditContrast=_T("45");
	
	m_sliderGamma.SetRangeMin(0);	
	m_sliderGamma.SetRangeMax(99);
	m_sliderGamma.SetPos(50);
	m_sEditGamma=_T("1.0");

	
    CopyImage(&m_image,&m_pictureBefore.m_image);
    m_pictureBefore.Invalidate(FALSE);
	
    CopyImage(&m_image,&m_pictureAfter.m_image);
    m_pictureAfter.Invalidate(FALSE);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}



void CImageModifyDlg::OnCustomdrawSliderImageModifyBrightness(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);


	UpdateData(TRUE);
	m_iBrightness=m_sliderBrightness.GetPos()-255;
	if(m_iBrightness<=-255){m_iBrightness=-255;}
	if(m_iBrightness>=255){m_iBrightness=255;}
	m_sEditBrightness.Format(_T("%d"),m_iBrightness);
	UpdateData(FALSE);
	UpdateResultImage();
	*pResult = 0;
}


void CImageModifyDlg::OnCustomdrawSliderImageModifyContrast(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);


	UpdateData(TRUE);
	m_iContrast=m_sliderContrast.GetPos();
	if(m_iContrast<=0){m_iContrast=9;}
	if(m_iContrast>=90){m_iContrast=90;}
	m_sEditContrast.Format(_T("%d"),m_iContrast);
	UpdateData(FALSE);
	UpdateResultImage();
	*pResult = 0;
}
#include "math.h"
void CImageModifyDlg::OnCustomdrawSliderImageModifyGamma(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);


	UpdateData(TRUE);
	int iGamma=m_sliderGamma.GetPos();
	if(iGamma<=0){iGamma=0;}
	if(iGamma>=99){iGamma=99;}

	if(iGamma==50){m_dGamma=1.0;}
	else if(iGamma<50){m_dGamma=pow(0.860363073,50-iGamma);}
	else{m_dGamma=pow(1.17605006,iGamma-50);}
	if(m_dGamma<=0.00063){m_dGamma=0.00063;}
	if(m_dGamma>=2824){m_dGamma=2824;}

	CString sFormat;
	if(m_dGamma<0.0001){sFormat.Format(_T("%%.6f"));}
	else if(m_dGamma<0.001){sFormat.Format(_T("%%.5f"));}
	else if(m_dGamma<0.01){sFormat.Format(_T("%%.4f"));}
	else if(m_dGamma<0.1){sFormat.Format(_T("%%.3f"));}
	else if(m_dGamma<0){sFormat.Format(_T("%%.2f"));}
	else{sFormat.Format(_T("%%.1f"));}

	m_sEditGamma.Format(sFormat,m_dGamma);

	UpdateData(FALSE);
	UpdateResultImage();
	*pResult = 0;
}


void CImageModifyDlg::OnKillfocusEditImageModifyBrightness()
{
	UpdateData(TRUE);

	m_iBrightness=_ttoi(m_sEditBrightness);

	if(m_iBrightness<=-255){m_iBrightness=-255;}
	if(m_iBrightness>=255){m_iBrightness=255;}
	m_sEditBrightness.Format(_T("%d"),m_iBrightness);

	m_sliderBrightness.SetPos(m_iBrightness+255);

	UpdateData(FALSE);
	UpdateResultImage();
}

void CImageModifyDlg::OnKillfocusEditImageModifyContrast()
{
	UpdateData(TRUE);

	m_iContrast=_ttoi(m_sEditContrast);

	if(m_iContrast<=0){m_iContrast=0;}
	if(m_iContrast>=90){m_iContrast=90;}
	m_sEditContrast.Format(_T("%d"),m_iContrast);

	m_sliderContrast.SetPos(m_iContrast);

	UpdateData(FALSE);
	UpdateResultImage();
}

void CImageModifyDlg::OnKillfocusEditImageModifyGamma()
{
	UpdateData(TRUE);

	m_dGamma=_ttof(m_sEditGamma);

	int iGamma;
	if(m_dGamma==1.0){iGamma=50;}
	else if(m_dGamma<0){iGamma=int(50 - log(m_dGamma)/log(0.860363073));}
	else{iGamma=int(50 + log(m_dGamma)/log(1.17605006));}

	m_sliderGamma.SetPos(iGamma);

	UpdateData(FALSE);
	UpdateResultImage();
}

void CImageModifyDlg::UpdateResultImage()
{
	ImgRGB imgRGB;
	ImgRGB imgResult1;
	ImgRGB imgResult2;
	ConvertImage(&m_image, &imgRGB);
	BrightnessContrast(&imgRGB,&imgResult1,0,0,m_image.GetHeight()-1,m_image.GetWidth()-1,(double)m_iBrightness,(double)m_iContrast);
	Gamma(&imgResult1,&imgResult2,0,0,m_image.GetHeight()-1,m_image.GetWidth()-1,m_dGamma);
	
    ConvertImage(&imgResult2, &m_pictureAfter.m_image);
    m_pictureAfter.Invalidate(FALSE);

}
