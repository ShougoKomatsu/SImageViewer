
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
		ON_WM_HSCROLL()
//		ON_WM_VSCROLL()
		ON_UPDATE_COMMAND_UI(ID_EDIT_EQU_HIST, &CSImageViewerView::OnUpdateEditEquHist)
		ON_WM_VSCROLL()
		ON_WM_HSCROLL()
		ON_WM_VSCROLL()
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

	void CSImageViewerView::OnDraw(CDC* pDC)
	{
		CSImageViewerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc){return;}

		if (m_image[m_iImgIndex].IsNull()){return;}

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		
	//	int iBarWidth= ::GetSystemMetrics(SM_CYHSCROLL);
	//	int iBarHeight= ::GetSystemMetrics(SM_CXVSCROLL);

		CImage imgZoomed;
		CRect rect;
		GetClientRect(&rect);
		
		int iWidth = rect.Width()-1;//-iBarWidth;
		int iHeight = rect.Height()-1;//-iBarHeight; 

		CBitmap bufferBmp; 
		bufferBmp.CreateCompatibleBitmap(pDC, iWidth, iHeight);
		CBitmap* pOldBmp = memDC.SelectObject(&bufferBmp);

		
		if(m_iDispOriginR==68478)
		{
			m_iDispOriginC=m_iDispOriginC;
		}

		ZoomImage(&(m_image[m_iImgIndex]),&imgZoomed,
			m_iDispOriginR/g_dScale[m_iScaleIndex],
			m_iDispOriginC/g_dScale[m_iScaleIndex],
			g_dScale[m_iScaleIndex],
			iWidth,iHeight);


		imgZoomed.BitBlt( memDC.GetSafeHdc(), 1, 1,imgZoomed.GetWidth(), imgZoomed.GetHeight(), 0, 0  );

		pDC->BitBlt(0, 0, iWidth, iHeight, &memDC, 0, 0,SRCCOPY);

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
		
		CRect rect;
		GetClientRect(&rect);
	
		int iBarWidth= ::GetSystemMetrics(SM_CYHSCROLL);
		int iBarHeight= ::GetSystemMetrics(SM_CXVSCROLL);

		int iImageWidth =max(0,m_image[m_iImgIndex].GetWidth());//-rect.Width());
		int iImageHeight =max(0,m_image[m_iImgIndex].GetHeight());//-rect.Height());

		int iZoomedWidth = iImageWidth*g_dScale[m_iScaleIndex];
		int iZoomedHeight=iImageHeight*g_dScale[m_iScaleIndex];
		
		if(iZoomedWidth<=rect.Width()-1){iBarWidth=0;}
		if(iZoomedHeight<=rect.Height()-1){iBarHeight=0;}

		SCROLLINFO si = { 0 };
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;

		GetScrollInfo(SB_VERT, &si);
		si.nMin=0;
		si.nMax=iZoomedHeight;
		si.nPage = rect.Height()-1-iBarHeight;
		SetScrollInfo(SB_VERT, &si, TRUE);
		
		GetScrollInfo(SB_HORZ, &si);
		si.nMin=0;
		si.nMax=iZoomedWidth;
		si.nPage = rect.Width()-1-iBarWidth;
		SetScrollInfo(SB_HORZ, &si, TRUE);

	}

	bool CSImageViewerView::ReadFile(CString sFilePath)
	{
		if(m_image[m_iImgIndex].IsNull()!=true){m_image[m_iImgIndex].Destroy();}

		m_image[m_iImgIndex].Load(m_sFilePath);
		
				m_iScaleIndex =8;
		SetScroll();

		m_iImgIndex=0;
		m_iUnDoAvailableCount=0;
		m_iReDoAvailableCount=0;
		m_Rect_i.SetRect(0, 0, m_image[m_iImgIndex].GetWidth()-1,m_image[m_iImgIndex].GetHeight()-1);
		Invalidate();
		return true;
	}

	void CSImageViewerView::OnFileOpen()
	{
		CFileDialog cf(TRUE);
		//		cf.m_ofn.lpstrInitialDir = sMacroFolderPath;
		if(cf.DoModal()!=IDOK){ return;}
		m_sFilePath.Format(_T("%s"),cf.GetPathName());
		ReadFile(m_sFilePath);
		m_iDispOriginC=0;
		m_iDispOriginR=0;

	}
	void CSImageViewerView::OnEquHistImage()
	{
		

		ImgRGB imgRGB;
		ImgRGB imgMeaned;
		ConvertImage(&m_image[m_iImgIndex], &imgRGB);
		EquHistImage(&imgRGB,&imgMeaned,m_Rect_i.top,m_Rect_i.left,m_Rect_i.bottom,m_Rect_i.right);
		m_iImgIndex++;
		m_iUnDoAvailableCount++;
		if(m_iUnDoAvailableCount>=MAX_IMG_BUF-1){m_iUnDoAvailableCount=MAX_IMG_BUF-1;}
		ConvertImage(&imgMeaned,&m_image[(m_iImgIndex % MAX_IMG_BUF)]);
		Invalidate();
	}



	void CSImageViewerView::OnSize(UINT nType, int cx, int cy)
	{
		CView::OnSize(nType, cx, cy);

		if(m_image[m_iImgIndex].IsNull()==true){return;}
		
		//SetScrollSizes(MM_TEXT, CSize((int)(m_image[m_iImgIndex].GetWidth() * g_dScale[m_iScaleIndex]), (int)(m_image[m_iImgIndex].GetHeight() * g_dScale[m_iScaleIndex]))); 

		Invalidate();
	}


	void CSImageViewerView::OnInitialUpdate()
	{
		CView::OnInitialUpdate();

		CRect rectClient;
		GetClientRect(&rectClient); 
		
		m_image[m_iImgIndex].Create(100,100,0);

//		SetScrollSizes(MM_TEXT, CSize(10,rectClient.Height()+1));
		m_bBingFullScreen = false;
		SetTimer(TIMER_INIT, 100, 0);
	}

	bool CSImageViewerView::ZoomChange(int iChange)
	{
		if((m_iScaleIndex>=SCALE_VAR_NUM-1)&&(iChange>0)){return false;}
		if((m_iScaleIndex<=0)&&(iChange<0)){return false;}

		CRect rectClient;
		GetClientRect(&rectClient); 

		double dScalePre=g_dScale[m_iScaleIndex];

		m_iScaleIndex+=iChange;
		if(m_iScaleIndex>=SCALE_VAR_NUM-1){m_iScaleIndex=SCALE_VAR_NUM-1;}
		if(m_iScaleIndex<=0){m_iScaleIndex=0;}
		SetScroll();
		Invalidate();

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

		if(m_image[m_iImgIndex].IsNull()==true){return false;}

		
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
		if (iC_img_Local >= m_image[m_iImgIndex].GetWidth()){return false;}
		if (iR_img_Local >= m_image[m_iImgIndex].GetHeight()){return false;}

		COLORREF col = m_image[m_iImgIndex].GetPixel(iC_img_Local,iR_img_Local);

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

		if(m_image[m_iImgIndex].IsNull()==true){return;}

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
		} 


		CView::OnMouseMove(nFlags, point);
	}


	void CSImageViewerView::OnLButtonDown(UINT nFlags, CPoint point)
	{
		SetCapture(); 
		m_bDragging = true;
		m_PointStart = point; 

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
		if (nHitTest == HTCLIENT) 
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
			return TRUE;
		}
		return CView::OnSetCursor(pWnd, nHitTest, message);
	}

	CRect CSImageViewerView::v_to_i(const CRect* rect_v)
	{
		CRect rcImage;
		if(rect_v->IsRectEmpty()==TRUE)
		{
		}
		int ix=GetScrollPos(SB_HORZ);
		int iy=GetScrollPos(SB_VERT);
		CPoint scrollPos(ix,iy);

		rcImage.left   = (int)(((rect_v->left   + scrollPos.x) / g_dScale[m_iScaleIndex])+0.5);
		rcImage.top    = (int)(((rect_v->top    + scrollPos.y) / g_dScale[m_iScaleIndex])+0.5);
		rcImage.right  = (int)(((rect_v->right  + scrollPos.x) / g_dScale[m_iScaleIndex])+0.5);
		rcImage.bottom = (int)(((rect_v->bottom + scrollPos.y) / g_dScale[m_iScaleIndex])+0.5);

		return rcImage;
	}

	CRect CSImageViewerView::i_to_v(const CRect* rect_i)
	{
		CRect rcView;
		if(rect_i->IsRectEmpty()==TRUE)
		{
		}
		rcView.left   = (int)((rect_i->left   ) * g_dScale[m_iScaleIndex]);
		rcView.top    = (int)((rect_i->top    ) * g_dScale[m_iScaleIndex]);
		rcView.right  = (int)((rect_i->right  ) * g_dScale[m_iScaleIndex]);
		rcView.bottom = (int)((rect_i->bottom ) * g_dScale[m_iScaleIndex]);

		return rcView;
	}



	BOOL CSImageViewerView::PreTranslateMessage(MSG* pMsg)
	{
			if (pMsg->message == WM_KEYDOWN)
		{
			if (pMsg->wParam == 'C') 
			{ 
				if(GetKeyState(VK_CONTROL)<0)
				{
					CImage imgClipped;
					ClipImage(&m_image[m_iImgIndex],&imgClipped, m_Rect_i.top,m_Rect_i.left, m_Rect_i.bottom, m_Rect_i.right); 
					CopyToClipBoardImg(&imgClipped);
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
			if(pMsg->wParam == VK_LEFT){OnScroll(SB_HORZ,SB_LINEUP,&m_iDispOriginC);return TRUE; }
			if(pMsg->wParam == VK_RIGHT){OnScroll(SB_HORZ,SB_LINEDOWN,&m_iDispOriginC); return TRUE; }
			if(pMsg->wParam == VK_UP){OnScroll(SB_VERT,SB_LINEUP,&m_iDispOriginC);return TRUE; }
			if(pMsg->wParam == VK_DOWN){OnScroll(SB_VERT,SB_LINEDOWN,&m_iDispOriginC); return TRUE; }
			if(pMsg->wParam == VK_PRIOR){OnScroll(SB_VERT,SB_PAGEUP,&m_iDispOriginC);return TRUE; }
			if(pMsg->wParam == VK_NEXT){OnScroll(SB_VERT,SB_PAGEDOWN,&m_iDispOriginC);return TRUE; }
			if((pMsg->wParam == 'A') || (pMsg->wParam == VK_ADD))
			{
				if(GetKeyState(VK_CONTROL)<0)
				{
//					m_Rect_i=CRect(0, 0, m_image[m_iImgIndex].GetWidth()-1, m_image[m_iImgIndex].GetHeight()-1);

					ImgRGB imgRGB;
					ImgRGB imgMeaned;
				ConvertImage(&m_image[m_iImgIndex], &imgRGB);
				MeanImage(&imgRGB,&imgMeaned,m_Rect_i.top,m_Rect_i.left,m_Rect_i.bottom,m_Rect_i.right,3,3);
				ConvertImage(&imgMeaned,&m_image[m_iImgIndex]);
//WriteImage(&imgRGB, _T("d:\\test.bmp"));
			Invalidate();
					return TRUE; 
				}
				ZoomChange(2);
				return TRUE; 
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


	void CSImageViewerView::OnScroll(int iSB, int nSBCode, double* iPos)
	{
		SCROLLINFO si;
		GetScrollInfo(iSB,&si);
		int iPageSize=si.nPage;
		int iMin=si.nMin;
		int iMax=si.nMax;

		int iNewPos=0;
		switch (nSBCode)
		{
		case SB_LINEUP:
			{
				iNewPos=max(iMin, (*iPos)-iPageSize/8.0);
				break;
			}
		case SB_LINEDOWN:
			{
				iNewPos=min(iMax-iPageSize, (*iPos)+iPageSize/8.0);
				break;
			}
		case SB_PAGEUP:
			{
				iNewPos=max(iMin, (*iPos)-iPageSize);
				break;
			}
		case SB_PAGEDOWN:
			{
				iNewPos=min(iMax-iPageSize, (*iPos)+iPageSize);
				break;
			}
		case SB_THUMBTRACK:
			{
				iNewPos=max(iMin,min(iMax-iPageSize , si.nTrackPos));
				break;
			}
		default:
			{
				return;
			}
		}
				*iPos= iNewPos;
		si.nPos = iNewPos; 
		SetScrollInfo(iSB, &si, TRUE);
	}


	void CSImageViewerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		OnScroll(SB_HORZ, nSBCode, &m_iDispOriginC);

		Invalidate();
	}


	void CSImageViewerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		OnScroll(SB_VERT, nSBCode, &m_iDispOriginR);

		Invalidate();
	}


	BOOL CSImageViewerView::OnEraseBkgnd(CDC* pDC)
	{
		// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。
		return TRUE;
		return CView::OnEraseBkgnd(pDC);
	}
