
// SImageViewerView.cpp : CSImageViewerView クラスの実装
//

#include "stdafx.h"
// SHARED_HANDLERS は、プレビュー、サムネイル、および検索フィルター ハンドラーを実装している ATL プロジェクトで定義でき、
// そのプロジェクトとのドキュメント コードの共有を可能にします。
#ifndef SHARED_HANDLERS
#include "SImageViewer.h"
#endif

#include "SImageViewerDoc.h"
#include "SImageViewerView.h"
#include "ImageProc.h"
#include "MainFrm.h"
#include "ImageModifyDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_INIT (100)
#define SCALE_VAR_NUM (25)
double g_dScale[SCALE_VAR_NUM]=
{
	0.125000,
	0.162105,
	0.210224,
	0.272627,
	0.353553,
	0.458502,
	0.594604,
	0.771105,
	1.000000,
	1.296840,
	1.681793,
	2.181015,
	2.828427,
	3.668016,
	4.756828,
	6.168843,
	8.000000,
	10.374716,
	13.454343,
	17.448124,
	22.627417,
	29.344129,
	38.054628,
	49.350746,
	64.000000,
};

// CSImageViewerView

IMPLEMENT_DYNCREATE(CSImageViewerView, CView)

	BEGIN_MESSAGE_MAP(CSImageViewerView, CView)
		ON_WM_CONTEXTMENU()
		ON_WM_RBUTTONUP()
		ON_COMMAND(ID_FILE_OPEN, &CSImageViewerView::OnFileOpen)
		ON_COMMAND(ID_EDIT_EQU_HIST, &CSImageViewerView::OnEquHistImage)
		ON_WM_SIZE()
		ON_WM_MOUSEMOVE()
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_WM_TIMER()
		ON_WM_SETCURSOR()
		ON_UPDATE_COMMAND_UI(ID_EDIT_EQU_HIST, &CSImageViewerView::OnUpdateEditEquHist)
		ON_WM_VSCROLL()
		ON_WM_HSCROLL()
		ON_WM_ERASEBKGND()
	END_MESSAGE_MAP()

	// CSImageViewerView コンストラクション/デストラクション

	CSImageViewerView::CSImageViewerView()
	{
		m_bDragging = false;
		m_Rect_v.SetRectEmpty();
		m_Rect_i.SetRectEmpty();
		m_iImgIndex=0;
		m_iUnDoAvailableCount=0;
		m_iReDoAvailableCount=0;
		m_iScaleIndex=8;
		m_sFilePath=_T("");
		if(g_sParam.GetLength()>0){m_sFilePath.Format(_T("%s"), g_sParam);}
	}

	CSImageViewerView::~CSImageViewerView()
	{
	}

	BOOL CSImageViewerView::PreCreateWindow(CREATESTRUCT& cs)
	{
		// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
		//  修正してください。

		return CView::PreCreateWindow(cs);
	}

	// CSImageViewerView 描画
	double CSImageViewerView::GetDispOriginR_tv()
	{
		return m_dDispOriginR_tv;
	}
	double CSImageViewerView::GetDispOriginC_tv()
	{
		return m_dDispOriginC_tv;
	}
	void CSImageViewerView::OnDraw(CDC* pDC)
	{
		CSImageViewerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc){return;}

		if (m_imageProcessed[m_iImgIndex].IsNull()){return;}

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);

		CImage imgZoomed;
		int iHeight_v=GetClientHeight();
		int iWidth_v=GetClientWidth();

		CBitmap bufferBmp; 
		bufferBmp.CreateCompatibleBitmap(pDC, iWidth_v, iHeight_v);
		CBitmap* pOldBmp = memDC.SelectObject(&bufferBmp);

		int iDispOriginR_tv = GetDispOriginR_tv();
		int iDispOriginC_tv = GetDispOriginC_tv();

		int iR0_i = iDispOriginR_tv/g_dScale[m_iScaleIndex];
		int iC0_i = iDispOriginC_tv/g_dScale[m_iScaleIndex];

		ZoomImage(&(m_imageProcessed[m_iImgIndex]),&imgZoomed,
			iR0_i,
			iC0_i,
			g_dScale[m_iScaleIndex],
			iWidth_v,iHeight_v);


		imgZoomed.BitBlt( memDC.GetSafeHdc(), 1, 1,imgZoomed.GetWidth(), imgZoomed.GetHeight(), 0, 0  );

		pDC->BitBlt(0, 0, iWidth_v, iHeight_v, &memDC, 0, 0,SRCCOPY);

		memDC.SelectObject(pOldBmp);


		if (m_Rect_i.IsRectEmpty()==FALSE)
		{
			CRect rect_v = i_to_v(&m_Rect_i);

			CPen pen(PS_SOLID,1, RGB(127,127,127));
			CPen* pOldPen=pDC->SelectObject(&pen);
			CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

			pDC->Rectangle(rect_v);

			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);
			ReleaseDC(pDC); 
		} 

	}

	void CSImageViewerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
	{
		ClientToScreen(&point);
		OnContextMenu(this, point);
	}

	void CSImageViewerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
	{
#ifndef SHARED_HANDLERS
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
	}


	// CSImageViewerView 診断

#ifdef _DEBUG
	void CSImageViewerView::AssertValid() const
	{
		CView::AssertValid();
	}

	void CSImageViewerView::Dump(CDumpContext& dc) const
	{
		CView::Dump(dc);
	}

	CSImageViewerDoc* CSImageViewerView::GetDocument() const // デバッグ以外のバージョンはインラインです。
	{
		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSImageViewerDoc)));
		return (CSImageViewerDoc*)m_pDocument;
	}
#endif //_DEBUG

	void CSImageViewerView::SetScroll()
	{

		int iHeight_v=GetClientHeight();
		int iWidth_v=GetClientWidth();

		int iBarWidth= ::GetSystemMetrics(SM_CYHSCROLL);
		int iBarHeight= ::GetSystemMetrics(SM_CXVSCROLL);

		int iWidth_i =max(0,m_imageProcessed[m_iImgIndex].GetWidth());//-rect.Width());
		int iHeight_i =max(0,m_imageProcessed[m_iImgIndex].GetHeight());//-rect.Height());

		int iWidth_tv = iWidth_i*g_dScale[m_iScaleIndex];
		int iHeight_tv= iHeight_i*g_dScale[m_iScaleIndex];

		if(iWidth_tv<=iWidth_v){iBarWidth=0;}
		if(iHeight_tv<=iHeight_v){iBarHeight=0;}



		SCROLLINFO si = { 0 };
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;

		GetScrollInfo(SB_VERT, &si);
		si.nMin=0;
		si.nMax=iHeight_tv;
		si.nPage = iHeight_v-1-iBarHeight;
		SetScrollInfo(SB_VERT, &si, TRUE);

		GetScrollInfo(SB_HORZ, &si);
		si.nMin=0;
		si.nMax=iWidth_tv;
		si.nPage = iWidth_v-1-iBarWidth;
		SetScrollInfo(SB_HORZ, &si, TRUE);

	}
	void CSImageViewerView::ResetImage()
	{
		m_imageProcessed[m_iImgIndex]=m_image;
		m_iScaleIndex =8;
		SetScroll();

		m_iImgIndex=0;
		m_iUnDoAvailableCount=0;
		m_iReDoAvailableCount=0;
		m_dDispOriginC_tv=0;
		m_dDispOriginR_tv=0;

		SCROLLINFO si;
		si.nPos = 0; 
		SetScrollInfo(SB_HORZ, &si, TRUE);
		SetScrollInfo(SB_VERT, &si, TRUE);

		m_iCurSor=0;
		m_Rect_i.SetRectEmpty();
		Invalidate();

		CString sImageSize;
		sImageSize.Format(_T("%d x %d"), m_image.GetWidth(),m_image.GetHeight());

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame != nullptr)	{pFrame->SetStatusMessage(sImageSize);}

	}

	bool CSImageViewerView::ReadFile(CString sFilePath)
	{
		if(m_imageProcessed[m_iImgIndex].IsNull()!=true){m_imageProcessed[m_iImgIndex].Destroy();}

		CFileFind cf;
		BOOL bRet = cf.FindFile(sFilePath);
		if(bRet != TRUE){return false;}
		if(m_image.IsNull()!=true){m_image.Destroy();}

		HRESULT hResult = m_image.Load(m_sFilePath);
		if(hResult != S_OK){return false;}

		ResetImage();
		return true;
	}

	void CSImageViewerView::OnFileOpen()
	{
		CFileDialog cf(TRUE);
		//		cf.m_ofn.lpstrInitialDir = sMacroFolderPath;
		if(cf.DoModal()!=IDOK){ return;}
		m_sFilePath.Format(_T("%s"),cf.GetPathName());
		ReadFile(m_sFilePath);

	}
	void CSImageViewerView::OnEquHistImage()
	{
		if(m_Rect_i.IsRectEmpty()==TRUE){return;}

		ImgRGB imgRGB;
		ImgRGB imgMeaned;
		ConvertImage(&m_imageProcessed[m_iImgIndex], &imgRGB);
		EquHistImage(&imgRGB,&imgMeaned,m_Rect_i.top,m_Rect_i.left,m_Rect_i.bottom,m_Rect_i.right);
		m_iImgIndex++;
		m_iUnDoAvailableCount++;
		if(m_iUnDoAvailableCount>=MAX_IMG_BUF-1){m_iUnDoAvailableCount=MAX_IMG_BUF-1;}
		ConvertImage(&imgMeaned,&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)]);
		Invalidate();
	}

	void CSImageViewerView::ModifyBrightnessContrastGamma()
	{
		if(m_Rect_i.IsRectEmpty()==TRUE){return;}

		CImageModifyDlg dlgModify;
		INT_PTR iRet;
		dlgModify.DoModal();
		iRet = dlgModify.m_iRet;
		if(iRet == IDCANCEL){return ;}

		int iBrightness=dlgModify.m_iBrightness;
		int iContrast=dlgModify.m_iContrast;
		double dGamma=dlgModify.m_dGamma;

		ImgRGB imgRGB;
		ImgRGB imgResult1;
		ImgRGB imgResult2;
		ConvertImage(&m_imageProcessed[m_iImgIndex], &imgRGB);
		BrightnessContrast(&imgRGB,&imgResult1,m_Rect_i.top,m_Rect_i.left,m_Rect_i.bottom,m_Rect_i.right,(double)iBrightness,(double)iContrast);
		Gamma(&imgResult1,&imgResult2,m_Rect_i.top,m_Rect_i.left,m_Rect_i.bottom,m_Rect_i.right,dGamma);


		m_iImgIndex++;
		m_iUnDoAvailableCount++;
		if(m_iUnDoAvailableCount>=MAX_IMG_BUF-1){m_iUnDoAvailableCount=MAX_IMG_BUF-1;}
		ConvertImage(&imgResult2,&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)]);
		Invalidate();
	}



	void CSImageViewerView::OnSize(UINT nType, int cx, int cy)
	{
		CView::OnSize(nType, cx, cy);

		if(m_imageProcessed[m_iImgIndex].IsNull()==true){return;}

		SetScroll();
		Invalidate();
	}


	void CSImageViewerView::OnInitialUpdate()
	{
		CView::OnInitialUpdate();


		m_imageProcessed[m_iImgIndex].Create(100,100,0);

		m_bBingFullScreen = false;
		SetTimer(TIMER_INIT, 100, 0);
	}

	int CSImageViewerView::GetClientHeight()
	{
		CRect rectClient;
		GetClientRect(&rectClient);
		return rectClient.Height();
	}
	
	int CSImageViewerView::GetClientWidth()
	{
		CRect rectClient;
		GetClientRect(&rectClient);
		return rectClient.Width();
	}

	bool CSImageViewerView::ZoomChange(int iChange)
	{
		if((m_iScaleIndex>=SCALE_VAR_NUM-1)&&(iChange>0)){return false;}
		if((m_iScaleIndex<=0)&&(iChange<0)){return false;}

		int iHeight_v=GetClientHeight();
		int iWidth_v=GetClientWidth();
		double dOldDispOriginR_tv = GetDispOriginR_tv();
		double dOldDispOriginC_tv = GetDispOriginC_tv();

		int iOldCenterR_i = (dOldDispOriginR_tv +iHeight_v/2.0)/g_dScale[m_iScaleIndex];
		int iOldCenterC_i = (dOldDispOriginC_tv +iWidth_v/2.0)/g_dScale[m_iScaleIndex];

		double dScalePre=g_dScale[m_iScaleIndex];
		m_iScaleIndex+=iChange;

		double dNewDispOriginR_tv = iOldCenterR_i*g_dScale[m_iScaleIndex] - iHeight_v/2.0;
		double dNewDispOriginC_tv = iOldCenterC_i*g_dScale[m_iScaleIndex] - iWidth_v/2.0;

		SetScrollPos(dNewDispOriginR_tv, dNewDispOriginC_tv);

		if(m_iScaleIndex>=SCALE_VAR_NUM-1){m_iScaleIndex=SCALE_VAR_NUM-1;}
		if(m_iScaleIndex<=0){m_iScaleIndex=0;}
		SetScroll();
		Invalidate();

		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		DispStatus(point);
		return true; 
	}

	bool CSImageViewerView::ZoomChange(int iMousePosR_v, int iMousePosC_v, int iChange)
	{
		if((m_iScaleIndex>=SCALE_VAR_NUM-1)&&(iChange>0)){return false;}
		if((m_iScaleIndex<=0)&&(iChange<0)){return false;}

		int iMousePosR_tv=iMousePosR_v + GetDispOriginR_tv();
		int iMousePosC_tv=iMousePosC_v + GetDispOriginC_tv();
		
		int iMousePosR_i=iMousePosR_tv/g_dScale[m_iScaleIndex];
		int iMousePosC_i=iMousePosC_tv/g_dScale[m_iScaleIndex];

		m_iScaleIndex+=iChange;		
		SetScroll();

		double dNewMousePosR_tv = iMousePosR_i*g_dScale[m_iScaleIndex];
		double dNewMousePosC_tv = iMousePosC_i*g_dScale[m_iScaleIndex];

		int iNewScrollR_tv = int(dNewMousePosR_tv-iMousePosR_v);
		int iNewScrollC_tv = int(dNewMousePosC_tv-iMousePosC_v);

		SetScrollPos(iNewScrollR_tv, iNewScrollC_tv);

		Invalidate();

		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		DispStatus(point);
		return true; 
	}


	void CSImageViewerView::SetScrollPos(int iR, int iC)
	{		
		SCROLLINFO si;

		GetScrollInfo(SB_VERT, &si);
		m_dDispOriginR_tv = min(si.nMax,max(si.nMin,iR) );
		si.nPos = m_dDispOriginR_tv; 
		SetScrollInfo(SB_VERT, &si, TRUE);

		GetScrollInfo(SB_HORZ, &si);
		m_dDispOriginC_tv = min(si.nMax,max(si.nMin,iC) );
		si.nPos = m_dDispOriginC_tv; 
		SetScrollInfo(SB_HORZ, &si, TRUE);

	}
	bool CSImageViewerView::ZoomChange(int iR0_i, int iC0_i, int iR1_i, int iC1_i)
	{
		int iHeight_v=GetClientHeight();
		int iWidth_v=GetClientWidth();

		int iNewScaleIndex = m_iScaleIndex;
		for(int i=SCALE_VAR_NUM-1; i>=0; i--)
		{
			if((iHeight_v>(iR1_i-iR0_i+1)*g_dScale[i]) && (iWidth_v>(iC1_i-iC0_i+1)*g_dScale[i]))
			{
				iNewScaleIndex=i;
				break;
			}
		}


		m_iScaleIndex = iNewScaleIndex;
		if(m_iScaleIndex>=SCALE_VAR_NUM-1){m_iScaleIndex=SCALE_VAR_NUM-1;}
		if(m_iScaleIndex<=0){m_iScaleIndex=0;}
		
		SetScroll();

		double dNewCenterR_i = (iR0_i + iR1_i)/2.0;
		double dNewCenterC_i = (iC0_i + iC1_i)/2.0;

		double dNewCenterR_tv = dNewCenterR_i*g_dScale[m_iScaleIndex];
		double dNewCenterC_tv = dNewCenterC_i*g_dScale[m_iScaleIndex];


		int iNewScrollR_tv = int(dNewCenterR_tv-iHeight_v/2.0);
		int iNewScrollC_tv = int(dNewCenterC_tv-iWidth_v/2.0);

		SetScrollPos(iNewScrollR_tv, iNewScrollC_tv);


		Invalidate();

		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		DispStatus(point);
		return true; 
	}

	void CSImageViewerView::EnterFullScreen()
	{
		if (m_bBingFullScreen == true) {return;}

		CFrameWnd* pFrame = GetParentFrame();
		if (pFrame == NULL) {return;}

		m_bBingFullScreen = true;

		pFrame->GetWindowRect(&m_rectPreserved);
		m_dwStylePreserved = pFrame->GetStyle();
		m_dwExStylePreserved = pFrame->GetExStyle();

		pFrame->ModifyStyle(WS_OVERLAPPEDWINDOW, WS_POPUP);
		pFrame->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

		HMONITOR hMon = MonitorFromWindow(pFrame->m_hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hMon, &mi);

		pFrame->SetWindowPos(
			NULL,
			mi.rcMonitor.left,
			mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_FRAMECHANGED | SWP_SHOWWINDOW
			);
	}

	void CSImageViewerView::ExitFullScreen()
	{
		if (m_bBingFullScreen != true){ return;}

		CFrameWnd* pFrame = GetParentFrame();
		if (pFrame==NULL) {return;}

		m_bBingFullScreen = FALSE;

		pFrame->ModifyStyle(WS_POPUP, m_dwStylePreserved);
		pFrame->ModifyStyleEx(0, m_dwExStylePreserved);

		pFrame->SetWindowPos(
			NULL,
			m_rectPreserved.left,
			m_rectPreserved.top,
			m_rectPreserved.right - m_rectPreserved.left,
			m_rectPreserved.bottom - m_rectPreserved.top,
			SWP_FRAMECHANGED | SWP_SHOWWINDOW
			);
	}


	bool CSImageViewerView::GetColorAtCursor(CPoint point, int* iR_img, int* iC_img, BYTE* byR, BYTE* byG, BYTE* byB)
	{

		if(m_image.IsNull()==true){return false;}


		SCROLLINFO si;
		GetScrollInfo(SB_HORZ,&si);
		int iScrC=si.nPos;
		GetScrollInfo(SB_VERT,&si);
		int iScrR=si.nPos;


		CPoint pointInView(point.x + iScrC, point.y + iScrR);

		int iC_img_Local = (int)((pointInView.x-1) / g_dScale[m_iScaleIndex]);
		int iR_img_Local = (int)((pointInView.y-1) / g_dScale[m_iScaleIndex]);


		if (iC_img_Local < 0){return false;}
		if (iR_img_Local < 0){return false;}
		if (iC_img_Local >= m_image.GetWidth()){return false;}
		if (iR_img_Local >= m_image.GetHeight()){return false;}

		COLORREF col = m_image.GetPixel(iC_img_Local,iR_img_Local);

		*iR_img = iR_img_Local;
		*iC_img = iC_img_Local;
		*byR = GetRValue(col);
		*byG = GetGValue(col);
		*byB = GetBValue(col);

		return true;
	}

	void CSImageViewerView::DispStatus(CPoint point)
	{

		int iR_img,iC_img;
		BYTE byR,byG,byB;
		CString sCaption;
		bool bRet = GetColorAtCursor(point,&iR_img,&iC_img, &byR, &byG, &byB);
		if(bRet == true)
		{
			sCaption.Format(_T("%s | (%d, %d) (R, G, B)= (%d, %d, %d) | %.3f%%"), m_sFilePath, iC_img, iR_img, byR, byG, byB,100*g_dScale[m_iScaleIndex]);
		}
		else
		{
			sCaption.Format(m_sFilePath);
		}
		AfxGetMainWnd()->SetWindowText(sCaption);
	}

	void CSImageViewerView::OnMouseMove(UINT nFlags, CPoint point)
	{

		if(m_imageProcessed[m_iImgIndex].IsNull()==true){return;}

		DispStatus(point);

		if (m_bDragging==true) 
		{ 
			CDC* pDC = GetDC(); 
			if (m_Rect_v.IsRectEmpty() == FALSE)
			{
				pDC->DrawFocusRect(&m_Rect_v);
			}

			m_Rect_v = CRect(m_PointStart, point);
			m_Rect_v.NormalizeRect();
			pDC->DrawFocusRect(&m_Rect_v); 
			ReleaseDC(pDC);
			CView::OnMouseMove(nFlags, point);
			return;
		} 


		CRect rect_v;
		rect_v = i_to_v(&m_Rect_i);
		if((point.y>=rect_v.top)&&(point.y<=rect_v.bottom)&&(point.x>=rect_v.left)&&(point.x<=rect_v.right))
		{
			m_iCurSor=1;
		}
		else
		{
			m_iCurSor=0;
		}

		CView::OnMouseMove(nFlags, point);
	}


	void CSImageViewerView::OnLButtonDown(UINT nFlags, CPoint point)
	{
		SetCapture(); 
		m_bDragging = true;
		m_PointStart.SetPoint(point.x,point.y); 

		CView::OnLButtonDown(nFlags, point);
	}


	void CSImageViewerView::OnLButtonUp(UINT nFlags, CPoint point)
	{
		if (m_bDragging==TRUE) 
		{
			ReleaseCapture(); 
			m_bDragging = false; 
			if (m_Rect_v.IsRectEmpty()==FALSE)
			{
				CDC* pDC = GetDC();
				pDC->DrawFocusRect(&m_Rect_v); 
				ReleaseDC(pDC); 
			} 
			if(m_PointStart==point)
			{
				CRect rect_v;
				rect_v = i_to_v(&m_Rect_i);
				if((point.y>=rect_v.top)&&(point.y<=rect_v.bottom)&&(point.x>=rect_v.left)&&(point.x<=rect_v.right))
				{
					ZoomChange(m_Rect_i.top, m_Rect_i.left, m_Rect_i.bottom,m_Rect_i.right);
					return;
				}
				m_Rect_v.SetRectEmpty();
			}
			m_Rect_i = v_to_i(&m_Rect_v);
			m_Rect_v.SetRectEmpty();
			Invalidate();
		}

		CView::OnLButtonUp(nFlags, point);
	}


	void CSImageViewerView::OnTimer(UINT_PTR nIDEvent)
	{
		if(nIDEvent==TIMER_INIT)
		{
			KillTimer(TIMER_INIT);
			if(m_sFilePath.GetLength()>0)
			{
				ReadFile(m_sFilePath);
			}
			//	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
			return;
		}

		CView::OnTimer(nIDEvent);
	}


	BOOL CSImageViewerView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
	{
		switch(m_iCurSor)
		{
		case 0:
			{
				if (nHitTest == HTCLIENT) 
				{
					SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
					return TRUE;
				}
				break;
			}
		case 1:
			{
				if (nHitTest == HTCLIENT) 
				{
					SetCursor(AfxGetApp()->LoadCursorW(IDC_CURSOR_ZOOMIN));
					return TRUE;
				}
				break;
			}
		}

		return CView::OnSetCursor(pWnd, nHitTest, message);
	}

	CRect CSImageViewerView::v_to_i(const CRect* rect_v)
	{
		CRect rcImage;
		if(rect_v->IsRectEmpty()==TRUE)
		{
		}

		int iCOrigin_tv = GetScrollPos(SB_HORZ);
		int iROrigin_tv = GetScrollPos(SB_VERT);

		rcImage.left   = (int)(((rect_v->left+ iCOrigin_tv) / g_dScale[m_iScaleIndex])   +0.5);
		rcImage.top    = (int)(((rect_v->top+ iROrigin_tv) / g_dScale[m_iScaleIndex])    +0.5);

		rcImage.right  = (int)(((rect_v->right+ iCOrigin_tv) / g_dScale[m_iScaleIndex])  -0.5);
		rcImage.bottom = (int)(((rect_v->bottom+ iROrigin_tv) / g_dScale[m_iScaleIndex]) -0.5);

		return rcImage;
	}

	CRect CSImageViewerView::i_to_v(const CRect* rect_i)
	{
		CRect rcView;
		if(rect_i->IsRectEmpty()==TRUE)
		{
		}
		int iCOrigin_tv = GetScrollPos(SB_HORZ);
		int iROrigin_tv = GetScrollPos(SB_VERT);
		rcView.left   = (int)((rect_i->left   ) * g_dScale[m_iScaleIndex])-iCOrigin_tv;
		rcView.top    = (int)((rect_i->top    ) * g_dScale[m_iScaleIndex])-iROrigin_tv;
		rcView.right  = (int)((rect_i->right +1 ) * g_dScale[m_iScaleIndex])-iCOrigin_tv;
		rcView.bottom = (int)((rect_i->bottom +1) * g_dScale[m_iScaleIndex])-iROrigin_tv;

		return rcView;
	}



	BOOL CSImageViewerView::PreTranslateMessage(MSG* pMsg)
	{
		if(pMsg->message==WM_MOUSEWHEEL)
		{
			int iDelta;
			iDelta = GET_WHEEL_DELTA_WPARAM(pMsg->wParam);
			if(GetKeyState(VK_CONTROL)<0)
			{
				CPoint point;
				::GetCursorPos(&point);
				this->ScreenToClient(&point);



				if(iDelta>0)
				{
					ZoomChange(point.y, point.x,1);
				}
				else
				{
					ZoomChange(point.y, point.x,-1);
				}
				return TRUE;
			}
			if(iDelta>0)
			{
				OnScroll(SB_VERT, SB_LINEUP);
			}
			else
			{
				OnScroll(SB_VERT, SB_LINEDOWN);
			}

			return TRUE;


		}

		if (pMsg->message == WM_KEYDOWN)
		{
			if (pMsg->wParam == 'C') 
			{ 
				if(GetKeyState(VK_CONTROL)<0)
				{
					if(m_Rect_i.IsRectEmpty()==TRUE){return FALSE;}

					CImage imgClipped;
					ClipImage(&m_imageProcessed[m_iImgIndex],&imgClipped, m_Rect_i.top,m_Rect_i.left, m_Rect_i.bottom, m_Rect_i.right); 
					CopyToClipBoardImg(&imgClipped);
				}
				return TRUE;
			} 
			if (pMsg->wParam == 'V') 
			{ 
				if(GetKeyState(VK_CONTROL)<0)
				{

					BOOL bRet = CopyFromClipBoardImg(&m_image);
					if(bRet != TRUE){return FALSE;}
					m_sFilePath.Format(_T("Clipboard"));
					ResetImage();
				}
				return TRUE;
			} 
			if(pMsg->wParam=='U')
			{
				if(GetKeyState(VK_SHIFT)<0)
				{
					OnEquHistImage();
					return TRUE;
				}
			}

			if (pMsg->wParam == VK_RETURN) { EnterFullScreen(); return TRUE; } 
			if (pMsg->wParam == VK_ESCAPE) { if(m_bBingFullScreen==true){ ExitFullScreen(); return TRUE;} }

			if(pMsg->wParam == VK_ADD)
			{
				ZoomChange(1);
				return TRUE; 
			}
			if(pMsg->wParam == VK_SUBTRACT)
			{
				ZoomChange(-1);
				return TRUE; 
			}
			if(pMsg->wParam == 'Z')
			{
				if(GetKeyState(VK_CONTROL)<0)
				{
					if(m_iUnDoAvailableCount<=0){return FALSE;}
					m_iImgIndex--;
					m_iUnDoAvailableCount--;
					m_iReDoAvailableCount++;
					Invalidate();
				}
			}
			if(pMsg->wParam == 'Y')
			{
				if(GetKeyState(VK_CONTROL)<0)
				{
					if(m_iReDoAvailableCount<=0){return FALSE;}
					m_iImgIndex++;
					m_iReDoAvailableCount--;
					m_iUnDoAvailableCount++;
					Invalidate();
				}
			}
			if(pMsg->wParam == VK_LEFT){OnScroll(SB_HORZ,SB_LINEUP);return TRUE; }
			if(pMsg->wParam == VK_RIGHT){OnScroll(SB_HORZ,SB_LINEDOWN); return TRUE; }
			if(pMsg->wParam == VK_UP){OnScroll(SB_VERT,SB_LINEUP);return TRUE; }
			if(pMsg->wParam == VK_DOWN){OnScroll(SB_VERT,SB_LINEDOWN); return TRUE; }
			if(pMsg->wParam == VK_PRIOR){OnScroll(SB_VERT,SB_PAGEUP);return TRUE; }
			if(pMsg->wParam == VK_NEXT){OnScroll(SB_VERT,SB_PAGEDOWN);return TRUE; }
			if(pMsg->wParam == VK_ADD){ZoomChange(1);}
			if(pMsg->wParam == VK_SUBTRACT){ZoomChange(-1);}


			if(pMsg->wParam == 'A')
			{
				if(GetKeyState(VK_CONTROL)<0)
				{
					m_Rect_i.SetRect(0, 0, m_image.GetWidth()-1,m_image.GetHeight()-1);
				}
				return TRUE; 
			}
			if(pMsg->wParam == 'G')
			{
				if(GetKeyState(VK_SHIFT)<0)
				{
					ModifyBrightnessContrastGamma();
					return TRUE;
				}
			}
		}

		return CView::PreTranslateMessage(pMsg);
	}


	void CSImageViewerView::OnUpdateEditEquHist(CCmdUI *pCmdUI)
	{
		pCmdUI->Enable(TRUE);
	}


	//	void CSImageViewerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	//	{
	//		// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
	//
	//		CView::OnVScroll(nSBCode, nPos, pScrollBar);
	//	}


	void CSImageViewerView::OnScroll(int iSB, int nSBCode)
	{
		SCROLLINFO si;
		GetScrollInfo(iSB,&si);
		int iOldPos;
		double* dNewPos;
		if(iSB==SB_VERT)
		{
			iOldPos=GetDispOriginR_tv();
			dNewPos=&m_dDispOriginR_tv;
		}
		else
		{
			iOldPos=GetDispOriginC_tv();
			dNewPos=&m_dDispOriginC_tv;
		}

		int iPageSize=si.nPage;
		int iMin=si.nMin;
		int iMax=si.nMax;

		switch (nSBCode)
		{
		case SB_LINEUP:
			{
				*dNewPos=max(iMin, iOldPos-iPageSize/8.0);
				break;
			}
		case SB_LINEDOWN:
			{
				*dNewPos=min(iMax-iPageSize, iOldPos+iPageSize/8.0);
				break;
			}
		case SB_PAGEUP:
			{
				*dNewPos=max(iMin, iOldPos-iPageSize);
				break;
			}
		case SB_PAGEDOWN:
			{
				*dNewPos=min(iMax-iPageSize, iOldPos+iPageSize);
				break;
			}
		case SB_THUMBTRACK:
			{
				*dNewPos=max(iMin,min(iMax-iPageSize , si.nTrackPos));
				break;
			}
		default:
			{
				return;
			}
		}
		si.nPos = *dNewPos; 
		SetScrollInfo(iSB, &si, TRUE);
		Invalidate();
	}


	void CSImageViewerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		OnScroll(SB_HORZ, nSBCode);

	}


	void CSImageViewerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		OnScroll(SB_VERT, nSBCode);
	}


	BOOL CSImageViewerView::OnEraseBkgnd(CDC* pDC)
	{
		// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
		return TRUE;
		return CView::OnEraseBkgnd(pDC);
	}
