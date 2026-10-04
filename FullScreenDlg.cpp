// FullScreenDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "FullScreenDlg.h"
#include "afxdialogex.h"


// CFullScreenDlg ダイアログ

IMPLEMENT_DYNAMIC(CFullScreenDlg, CDialogEx)

CFullScreenDlg::CFullScreenDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFullScreenDlg::IDD, pParent)
{
	m_pView=(CSImageViewerView*)pParent;
}

CFullScreenDlg::~CFullScreenDlg()
{
}

void CFullScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FULLSCREEN_DISP, m_picture);
}


BEGIN_MESSAGE_MAP(CFullScreenDlg, CDialogEx)
END_MESSAGE_MAP()


// CFullScreenDlg メッセージ ハンドラー


BOOL CFullScreenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	HMONITOR hMon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	GetMonitorInfo(hMon, &mi);


	SetWindowPos(
		NULL,
		mi.rcMonitor.left,
		mi.rcMonitor.top,
		mi.rcMonitor.right - mi.rcMonitor.left,
		mi.rcMonitor.bottom - mi.rcMonitor.top,
		SWP_FRAMECHANGED | SWP_SHOWWINDOW
		);

	

	GetDlgItem(IDC_FULLSCREEN_DISP)->MoveWindow(&(mi.rcMonitor));
	ImageRefresh();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
void CFullScreenDlg::ImageRefresh()
{
	if(m_pView==NULL){return;}
	m_picture.m_image.Set(IMAGE_TYPE_CIMAGE, NULL, NULL, 0, 0, m_pView->m_image[m_pView->m_iImageIndex].GetCurrentProcess (), VALUE_IMAGE_CLIP_0_TO_255, _T("temp"));
	m_picture.Refresh();
}

BOOL CFullScreenDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{	
		if(GetKeyState(VK_CONTROL)<0)
		{	

			if(pMsg->wParam == VK_UP){m_pView->OperateImagePPFW(-1);ImageRefresh();return TRUE;}
			if(pMsg->wParam == VK_DOWN){m_pView->OperateImagePPFW(+1);ImageRefresh();return TRUE;}
			if(pMsg->wParam == VK_PRIOR){m_pView->OperateImagePPFW(-10);ImageRefresh();return TRUE;}
			if(pMsg->wParam == VK_NEXT){m_pView->OperateImagePPFW(+10);ImageRefresh();return TRUE;}
			if(pMsg->wParam == VK_HOME){m_pView->OperateImagePPFW(INT_MIN);ImageRefresh();return TRUE;}
			if(pMsg->wParam == VK_END){m_pView->OperateImagePPFW(INT_MAX);ImageRefresh();return TRUE;}
			if(pMsg->wParam == VK_LEFT){m_pView->OperateImagePPFWFlexible(-1);ImageRefresh();return TRUE;}
			if(pMsg->wParam == VK_RIGHT){m_pView->OperateImagePPFWFlexible(+1);ImageRefresh();return TRUE;}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
