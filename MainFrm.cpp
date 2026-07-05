
// MainFrm.cpp : CMainFrame クラスの実装
//

#include "stdafx.h"
#include "SImageViewer.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

	const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	//	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	//	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_DISP_STATUS_SIZE, &CMainFrame::OnDispStatusSize)
	ON_COMMAND(ID_DISP_STATUS_BPP, &CMainFrame::OnDispStatusBPP)
	ON_COMMAND(ID_DISP_STATUS_ZOOM, &CMainFrame::OnDispStatusZoom)
	ON_COMMAND(ID_DISP_STATUS_MOUSE_POS, &CMainFrame::OnDispStatusMousePos)
	ON_UPDATE_COMMAND_UI(AFX_IDP_COMMAND_FAILURE, &CMainFrame::OnUpdateAfxIdpCommandFailure)
	ON_UPDATE_COMMAND_UI(ID_MENU_EQU_HIST, &CMainFrame::OnUpdateMenu)
	ON_UPDATE_COMMAND_UI(ID_MENU_COPY_AS, &CMainFrame::OnUpdateMenu)
	ON_UPDATE_COMMAND_UI(ID_MENU_CONVERT_COLOR_SPACE, &CMainFrame::OnUpdateMenu)
	ON_UPDATE_COMMAND_UI(ID_MENU_CHANGE_COLOR_DEPTH, &CMainFrame::OnUpdateMenu)
	ON_UPDATE_COMMAND_UI(ID_MENU_COLOR_CORRECTON, &CMainFrame::OnUpdateMenu)
	ON_UPDATE_COMMAND_UI(ID_MENU_SET_SELECTION, &CMainFrame::OnUpdateMenu)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &CMainFrame::OnUpdateMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CMainFrame::OnUpdateMenu)
	ON_UPDATE_COMMAND_UI(IDM_ZOOMUP, &CMainFrame::OnUpdateMenu)
	ON_UPDATE_COMMAND_UI(IDM_ZOOMDOWN, &CMainFrame::OnUpdateMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CMainFrame::OnUpdateMenu)
	ON_UPDATE_COMMAND_UI(ID_MENU_RESAMPLE, &CMainFrame::OnUpdateMenu)	
	
    ON_COMMAND(ID_BUTTON_GRID_DOT, &CMainFrame::OnButtonGridDot)
    ON_COMMAND(ID_BUTTON_GRID_LINE, &CMainFrame::OnButtonGridLine)
    ON_COMMAND(ID_BUTTON_GRID_CONNECT, &CMainFrame::OnButtonGridConnect)
    ON_UPDATE_COMMAND_UI(ID_BUTTON_GRID_DOT, &CMainFrame::OnUpdateMenu)
    ON_UPDATE_COMMAND_UI(ID_BUTTON_GRID_LINE, &CMainFrame::OnUpdateMenu)
    ON_UPDATE_COMMAND_UI(ID_BUTTON_GRID_CONNECT, &CMainFrame::OnUpdateMenu)
	ON_COMMAND(IDM_ZOOMDOWN, &CMainFrame::OnZoomdown)
	ON_COMMAND(IDM_ZOOMUP, &CMainFrame::OnZoomup)
    ON_WM_INITMENUPOPUP()
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_STATUS_SIZE,
	ID_STATUS_BPP,
	ID_STATUS_ZOOM,
	ID_STATUS_MOUSE_POS,
	ID_STATUS_RGB_ORIGINAL,
	ID_STATUS_RGB_PROCESSED,
	ID_STATUS_SELECTION,
	ID_SEPARATOR,
};

// CMainFrame コンストラクション/デストラクション
void CMainFrame::OnDispStatusSize()
{
	m_wndStatusBar.SetPaneText(0, m_sStatusSize);
}
void CMainFrame::OnDispStatusBPP()
{
	m_wndStatusBar.SetPaneText(1, m_sStatusBPP);
}
void CMainFrame::OnDispStatusZoom()
{
	m_wndStatusBar.SetPaneText(2, m_sStatusZoom);
}
void CMainFrame::OnDispStatusMousePos()
{
	m_wndStatusBar.SetPaneText(3, m_sStatusMousePos);
	m_wndStatusBar.SetPaneText(4, m_sStatusRGBOriginal);
	m_wndStatusBar.SetPaneText(5, m_sStatusRGBProcessed);
	m_wndStatusBar.SetPaneText(6, m_sStatusSelection);
}
CMainFrame::CMainFrame()
{
	// TODO: メンバー初期化コードをここに追加してください。
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_WIN_2000);
}

CMainFrame::~CMainFrame()
{
}
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_bFileOpened = false;
	m_bSelected = false;
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1){return -1;}

	if (m_cfStatus.GetSafeHandle() != nullptr)
	{
		m_cfStatus.DeleteObject();
	}
	LOGFONT lf = {};
    lf.lfHeight = -18;                     // 高さ（負値で論理単位）
	lf.lfWeight = FW_NORMAL;                 // 太字
    lf.lfCharSet = DEFAULT_CHARSET;
    _tcscpy_s(lf.lfFaceName, _T("MS Gothic"));  // お好みのフォント名

    m_cfStatus.CreateFontIndirect(&lf);



	BOOL bNameValid;
	// 固定値に基づいてビジュアル マネージャーと visual スタイルを設定します
	OnApplicationLook(theApp.m_nAppLook);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("メニュー バーを作成できませんでした\n");
		return -1;      // 作成できませんでした。
	}
	m_wndMenuBar.SetPaneStyle(CBRS_FLOATING);

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// アクティブになったときメニュー バーにフォーカスを移動しない
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("ツール バーの作成に失敗しました。\n");
		return -1;      // 作成できませんでした。
	}
m_iGrid = ID_BUTTON_GRID_NONE;

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);


	// ユーザー定義のツール バーの操作を許可します:
	//	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("ステータス バーの作成に失敗しました。\n");
		return -1;      // 作成できない場合
	}
        m_wndStatusBar.SetFont(&m_cfStatus);


		m_bGridAble=false;

	m_wndStatusBar.SetIndicators(indicators, _countof(indicators));

	m_wndStatusBar.SetPaneInfo(0, ID_STATUS_SIZE, SBPS_POPOUT, 140);
	m_wndStatusBar.SetPaneInfo(1, ID_STATUS_BPP, SBPS_POPOUT, 70);
	m_wndStatusBar.SetPaneInfo(2, ID_STATUS_ZOOM, SBPS_POPOUT, 90);

	m_wndStatusBar.SetPaneInfo(3, ID_STATUS_MOUSE_POS, SBPS_POPOUT, 105);
	m_wndStatusBar.SetPaneInfo(4, ID_STATUS_RGB_ORIGINAL, SBPS_POPOUT, 140);
	m_wndStatusBar.SetPaneInfo(5, ID_STATUS_RGB_PROCESSED, SBPS_POPOUT, 140);
	m_wndStatusBar.SetPaneInfo(6, ID_STATUS_SELECTION, SBPS_POPOUT, 350);
	m_wndStatusBar.SetPaneInfo(7, ID_SEPARATOR, SBPS_STRETCH|SBPS_NOBORDERS, 0);

	for(int i=0; i<=6; i++)
	{
		m_wndStatusBar.SetPaneTextColor(i, RGB(0,0,0));
		m_wndStatusBar.SetPaneBackgroundColor(i, RGB(255, 255, 255));
	}
	

	// TODO: ツール バーおよびメニュー バーをドッキング可能にしない場合は、この 5 つの行を削除します
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);
	DragAcceptFiles(TRUE);
	// Visual Studio 2005 スタイルのドッキング ウィンドウ動作を有効にします
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 スタイルのドッキング ウィンドウの自動非表示動作を有効にします
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// ツール バーとドッキング ウィンドウ メニューの配置変更を有効にします
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// ツール バーのクイック (Alt キーを押しながらドラッグ) カスタマイズを有効にします
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// ユーザー定義のツール バー イメージを読み込みます
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// メニューのパーソナル化 (最近使用されたコマンド) を有効にします
	// TODO: ユーザー固有の基本コマンドを定義し、各メニューをクリックしたときに基本コマンドが 1 つ以上表示されるようにします。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	
	lstBasicCommands.AddTail(ID_FILE_SAVE_AS);
	lstBasicCommands.AddTail(ID_EDIT_COPY);

	lstBasicCommands.AddTail(ID_MENU_EQU_HIST);
	lstBasicCommands.AddTail(ID_MENU_SET_SELECTION);
	lstBasicCommands.AddTail(ID_MENU_COPY_AS);
	lstBasicCommands.AddTail(ID_MENU_CONVERT_COLOR_SPACE);
	lstBasicCommands.AddTail(ID_MENU_CHANGE_COLOR_DEPTH);
	lstBasicCommands.AddTail(ID_MENU_COLOR_CORRECTON);
	lstBasicCommands.AddTail(ID_MENU_RESAMPLE);
	/*
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
	*/
	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));

	return 0;
}
void CMainFrame::ShowNormal()
{
	ShowWindow(SW_SHOWNORMAL);
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	return TRUE;
}
void CMainFrame::SetStatusMessage(CString sMes)
{
	m_wndStatusBar.SetPaneText(0, sMes);
}
// CMainFrame 診断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame メッセージ ハンドラー

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* メニューをスキャンします*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}


void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;
		/*
		case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

		case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

		case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

		case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

		case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

		case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;
		*/
	default:
		/*
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
		CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
		break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
		CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
		break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
		CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
		break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
		CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
		break;
		}
		*/
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基本クラスが実際の動作を行います。

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// すべてのユーザー定義ツール バーのボタンのカスタマイズを有効にします
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}



void CMainFrame::OnUpdateAfxIdpCommandFailure(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE); 
}

#include "SImageViewerView.h"

void CMainFrame::OnZoomdown()
{

	CView* pView = GetActiveView();
	if (pView != nullptr)
	{
		((CSImageViewerView*)pView)->ZoomChange(-1);
	}
}


void CMainFrame::OnZoomup()
{
	CView* pView = GetActiveView();
	if (pView != nullptr)
	{
		((CSImageViewerView*)pView)->ZoomChange(1);
	}
}
void CMainFrame::LaunchNewInstance(CString sFilePath)
{
	TCHAR tszExePath[MAX_PATH];
	::GetModuleFileName(NULL, tszExePath, MAX_PATH);

	CString sCommand;
	sCommand.Format(_T("\"%s\" \"%s\""), tszExePath, sFilePath);

	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	BOOL bRet = CreateProcess(NULL,sCommand.GetBuffer(),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
	if (bRet == TRUE)
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	TCHAR tchFilePath[MAX_PATH];
	DragQueryFile(hDropInfo, 0, tchFilePath, MAX_PATH);

	LaunchNewInstance(tchFilePath);

	DragFinish(hDropInfo);
	CFrameWndEx::OnDropFiles(hDropInfo);
}

void CMainFrame::AdjustViewClientSize(int iNewClientWidth, int iNewClientHeight,int iCurrentClientWidth, int iCurrentClientHeight)
{
	ShowWindow(SW_NORMAL);

	CRect rectWindow;
	GetWindowRect(&rectWindow);

	CRect rectScreen;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectScreen, 0);

	int iNewWindowWidth = max(1120, rectWindow.Width() +  iNewClientWidth  - iCurrentClientWidth);
	int iNewWindowHeight = max(300,rectWindow.Height() + iNewClientHeight - iCurrentClientHeight);

	if ((iNewWindowWidth > rectScreen.Width()) ||(iNewWindowHeight > rectScreen.Height()))
	{
		ShowWindow(SW_MAXIMIZE);
		return;
	}

	int iC0 = rectWindow.left + min(0, rectScreen.right- (rectWindow.left + iNewWindowWidth)  );
	int iR0 = rectWindow.top + min(0, rectScreen.bottom - (rectWindow.top + iNewWindowHeight) );

	MoveWindow(iC0, iR0, iNewWindowWidth, iNewWindowHeight, TRUE);
}


void CMainFrame::EnterFullScreen()
{

	if (m_bBeingFullScreen == true) {return;}

	GetWindowRect(&m_rectPreserved);
	m_dwStylePreserved = GetStyle();
	m_dwExStylePreserved = GetExStyle();


	ModifyStyle(WS_OVERLAPPEDWINDOW, WS_POPUP);
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

	HMONITOR hMon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	GetMonitorInfo(hMon, &mi);


	CBasePane* basePane=DYNAMIC_DOWNCAST(CBasePane, GetMenuBar());	
	if (basePane != NULL)
	{

		basePane->ShowPane(FALSE, FALSE, FALSE);
	}

	CMFCToolBar* pToolBar = DYNAMIC_DOWNCAST(CMFCToolBar, GetControlBar(AFX_IDW_TOOLBAR));
	if (pToolBar != NULL)
	{
		pToolBar->ShowPane(FALSE, FALSE, FALSE);
	}

	CMFCStatusBar* pStatusBar = DYNAMIC_DOWNCAST(CMFCStatusBar, GetControlBar(AFX_IDW_STATUS_BAR));
	if (pStatusBar != NULL)
	{
		pStatusBar->ShowPane(FALSE, FALSE, FALSE);
	}


	ModifyStyle(WS_OVERLAPPEDWINDOW, WS_POPUP);
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);


	SetWindowPos(
		NULL,
		mi.rcMonitor.left,
		mi.rcMonitor.top,
		mi.rcMonitor.right - mi.rcMonitor.left,
		mi.rcMonitor.bottom - mi.rcMonitor.top,
		SWP_FRAMECHANGED | SWP_SHOWWINDOW
		);
}

void CMainFrame::ExitFullScreen()
{
	if (m_bBeingFullScreen != false){ return;}

	m_bBeingFullScreen = FALSE;

	CBasePane* basePane=DYNAMIC_DOWNCAST(CBasePane, GetMenuBar());	
	if (basePane != NULL)
	{

		basePane->ShowPane(TRUE, FALSE, FALSE);
	}

	CMFCToolBar* pToolBar = DYNAMIC_DOWNCAST(CMFCToolBar, GetControlBar(AFX_IDW_TOOLBAR));
	if (pToolBar != NULL)
	{
		pToolBar->ShowPane(TRUE, FALSE, FALSE);
	}

	CMFCStatusBar* pStatusBar = DYNAMIC_DOWNCAST(CMFCStatusBar, GetControlBar(AFX_IDW_STATUS_BAR));
	if (pStatusBar != NULL)
	{
		pStatusBar->ShowPane(TRUE, FALSE, FALSE);
	}


	ModifyStyle(WS_OVERLAPPEDWINDOW, WS_POPUP);
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0);


	ModifyStyle(WS_POPUP, m_dwStylePreserved);
	ModifyStyleEx(0, m_dwExStylePreserved);

	SetWindowPos(
		NULL,
		m_rectPreserved.left,
		m_rectPreserved.top,
		m_rectPreserved.right - m_rectPreserved.left,
		m_rectPreserved.bottom - m_rectPreserved.top,
		SWP_FRAMECHANGED | SWP_SHOWWINDOW
		);
}

void CMainFrame::OnDestroy()
{
	if (m_cfStatus.GetSafeHandle() != nullptr)
	{
		m_cfStatus.DeleteObject();
	}

	CFrameWndEx::OnDestroy();
}
void CMainFrame::OnButtonGridNone()
{
    m_iGrid = ID_BUTTON_GRID_NONE;
	Invalidate();
}

void CMainFrame::OnButtonGridDot()
{
	if(m_iGrid == ID_BUTTON_GRID_DOT)
	{
		m_iGrid = ID_BUTTON_GRID_NONE;
		Invalidate();
		return;
	}
	m_iGrid = ID_BUTTON_GRID_DOT;
	Invalidate();
}

void CMainFrame::OnButtonGridLine()
{
	if(m_iGrid == ID_BUTTON_GRID_LINE)
	{
		m_iGrid = ID_BUTTON_GRID_NONE;
		Invalidate();
		return;
	}
    m_iGrid = ID_BUTTON_GRID_LINE;
	Invalidate();
}

void CMainFrame::OnButtonGridConnect()
{
	if(m_iGrid == ID_BUTTON_GRID_CONNECT)
	{
		m_iGrid = ID_BUTTON_GRID_NONE;
		Invalidate();
		return;
	}
    m_iGrid = ID_BUTTON_GRID_CONNECT;
	Invalidate();
}

void CMainFrame::OnUpdateMenu(CCmdUI* pCmdUI)
{
	
	CView* pView = GetActiveView();
	switch (pCmdUI->m_nID)
	{
	case ID_EDIT_PASTE:
		{
			pCmdUI->Enable(true);
			break;
		}
	case ID_EDIT_COPY:
		{
			pCmdUI->Enable(m_bSelected);
			break;
		}
	case ID_BUTTON_GRID_DOT:{pCmdUI->Enable(m_bFileOpened & m_bGridAble);break;}
	case ID_BUTTON_GRID_LINE:{pCmdUI->Enable(m_bFileOpened & m_bGridAble);break;}
	case ID_BUTTON_GRID_CONNECT:{pCmdUI->Enable(m_bFileOpened & m_bGridAble);break;}

	case IDM_ZOOMDOWN:
	case IDM_ZOOMUP:
	case ID_FILE_SAVE_AS:
	case ID_MENU_EQU_HIST:
	case ID_MENU_COPY_AS:
	case ID_MENU_CONVERT_COLOR_SPACE:
	case ID_MENU_CHANGE_COLOR_DEPTH:
	case ID_MENU_COLOR_CORRECTON:
	case ID_MENU_SET_SELECTION:
	case ID_MENU_RESAMPLE:
		pCmdUI->Enable(m_bFileOpened);
		break;
	}

	switch (pCmdUI->m_nID)
	{
	case ID_BUTTON_GRID_DOT:{pCmdUI->SetCheck(m_iGrid == ID_BUTTON_GRID_DOT);break;}
	case ID_BUTTON_GRID_LINE:{pCmdUI->SetCheck(m_iGrid == ID_BUTTON_GRID_LINE);break;}
	case ID_BUTTON_GRID_CONNECT:{pCmdUI->SetCheck(m_iGrid == ID_BUTTON_GRID_CONNECT);break;}
	default:{}
	}

}
