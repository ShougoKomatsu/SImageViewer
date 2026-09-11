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
	, m_sEditValue1(_T(""))
	, m_sEditValue2(_T(""))
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
	DDX_Text(pDX, IDC_COLORIZE_EDIT_VALUE1, m_sEditValue1);
	DDX_Text(pDX, IDC_COLORIZE_EDIT_VALUE2, m_sEditValue2);
}


BEGIN_MESSAGE_MAP(CColorizeDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CColorizeDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_COLORIZE_EDIT_VALUE1, &CColorizeDlg::OnChangeColorizeEditValue1)
	ON_EN_CHANGE(IDC_COLORIZE_EDIT_VALUE2, &CColorizeDlg::OnChangeColorizeEditValue2)
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
	BYTE byMin = max(0, min(255, _ttoi(m_sEditValue1)));
	BYTE byMax = max(0, min(255, _ttoi(m_sEditValue2)));
	Threshold(&m_image,&m_imageColorized, byMin, byMax, 255, 0, 0);
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
	
	CopyImage_CImage(&m_image,&m_imageColorized);

    CopyImage_CImage(&m_image,&m_pictureBefore.m_image);
    m_pictureBefore.Invalidate(FALSE);
	
    CopyImage_CImage(&m_image,&m_pictureAfter.m_image);
    m_pictureAfter.Invalidate(FALSE);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
