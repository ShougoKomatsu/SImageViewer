// SetTransparentDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "SetTransparentDlg.h"
#include "afxdialogex.h"


// CSetTransparentDlg ダイアログ

IMPLEMENT_DYNAMIC(CSetTransparentDlg, CDialogEx)

CSetTransparentDlg::CSetTransparentDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetTransparentDlg::IDD, pParent)
{

}

CSetTransparentDlg::~CSetTransparentDlg()
{
}

void CSetTransparentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SET_TRANSPARENT_STATIC_BEFORE, m_pictureBefore);
	DDX_Control(pDX, IDC_SET_TRANSPARENT_STATIC_AFTER, m_pictureAfter);
	DDX_Control(pDX, IDC_SET_TRANSPARENT_STATIC_COLOR1, m_pictureColor1);
}


BEGIN_MESSAGE_MAP(CSetTransparentDlg, CDialogEx)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CSetTransparentDlg メッセージ ハンドラー


BOOL CSetTransparentDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	CopyImage_CImage(&m_image,&m_imageTransparent);

    CopyImage_CImage(&m_image,&m_pictureBefore.m_image);
    m_pictureBefore.Invalidate(FALSE);
	
    CopyImage_CImage(&m_image,&m_pictureAfter.m_image);
    m_pictureAfter.Invalidate(FALSE);

	GenHSImage(&m_imageColor1, 256, 256, 0, 360, 0, 1, 1);
    CopyImage_CImage(&m_imageColor1,&m_pictureColor1.m_image);
    m_pictureColor1.Invalidate(FALSE);

	m_pictureColor1.view.m_HBar1_i.Set(10, 0, 100, m_imageColor1.GetWidth()-1);
	
//	m_pictureColor1.m_bHBar2=true;
//	m_pictureColor1.m_iHBar2=100;
	
//	m_pictureColor1.m_bVBar1=true;
//	m_pictureColor1.m_iVBar1=10;
	
//	m_pictureColor1.m_bVBar2=true;
//	m_pictureColor1.m_iVBar2=100;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


BOOL CSetTransparentDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CSetTransparentDlg::OnMouseMove(UINT nFlags, CPoint point)
{

	m_pictureColor1.OnMouseMove(nFlags, point);
	CDialogEx::OnMouseMove(nFlags, point);
}
