
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
#define SCALE_VAR_NUM (36)
double g_dScale[SCALE_VAR_NUM]={0.0100, 0.0125, 0.0160, 0.0200, 0.0250, 0.0315, 0.0400, 0.0500, 0.0630, 0.0800,0.100, 0.125, 0.160, 0.200, 0.250, 0.315, 0.400, 0.500, 0.630, 0.800,1.00, 1.25, 1.60, 2.00, 2.50, 3.15, 4.00, 5.00, 6.30, 8.00, 10.0, 12.5, 16.0, 20.0, 25.0, 31.5};

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
		ON_WM_VSCROLL()
		ON_UPDATE_COMMAND_UI(ID_EDIT_EQU_HIST, &CSImageViewerView::OnUpdateEditEquHist)
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
		m_iScaleIndex=20;
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

		CDC memDC; memDC.CreateCompatibleDC(pDC);

		HBITMAP hBmp = (HBITMAP)m_image[m_iImgIndex];
		HBITMAP hOldBmp = (HBITMAP)memDC.SelectObject(hBmp);

		int iWidth = (int)(m_image[m_iImgIndex].GetWidth()*g_dScale[m_iScaleIndex]);
		int iHeight = (int)(m_image[m_iImgIndex].GetHeight()*g_dScale[m_iScaleIndex]); 
		pDC->StretchBlt(01, 1, iWidth, iHeight, &memDC, 0, 0, m_image[m_iImgIndex].GetWidth(), m_image[m_iImgIndex].GetHeight(), SRCCOPY);

		memDC.SelectObject(hOldBmp);

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


	bool CSImageViewerView::ReadFile(CString sFilePath)
	{
		if(m_image[m_iImgIndex].IsNull()!=true){m_image[m_iImgIndex].Destroy();}

		m_image[m_iImgIndex].Load(m_sFilePath);

		CSize sizeTotal;
		CRect rect;
		GetClientRect(&rect);

		sizeTotal.cx = max(0,m_image[m_iImgIndex].GetWidth());//-rect.Width());
		sizeTotal.cy = max(0,m_image[m_iImgIndex].GetHeight());//-rect.Height());
		m_iScaleIndex =20;
		SetScrollSizes(MM_TEXT, sizeTotal);
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
		CScrollView::OnSize(nType, cx, cy);

		if(m_image[m_iImgIndex].IsNull()==true){return;}
		SetScrollSizes(MM_TEXT, CSize((int)(m_image[m_iImgIndex].GetWidth() * g_dScale[m_iScaleIndex]), (int)(m_image[m_iImgIndex].GetHeight() * g_dScale[m_iScaleIndex]))); 

		Invalidate();
	}


	void CSImageViewerView::OnInitialUpdate()
	{
		CScrollView::OnInitialUpdate();

		
		m_image[m_iImgIndex].Create(100,100,0);

		SetScrollSizes(MM_TEXT, CSize(0,0));
		m_bBingFullScreen = false;
		SetTimer(TIMER_INIT, 100, 0);
	}


	bool CSImageViewerView::ScrollChange( double dH,double dV)
	{
		CPoint pointCurOrigin = GetScrollPosition(); 
		CRect rectClient;
		GetClientRect(&rectClient); 

		CPoint pointNewOrigin( (int)(pointCurOrigin.x  + rectClient.Width()*dH ), (int)(pointCurOrigin.y   + rectClient.Height() *dV) ); 
		ScrollToPosition(pointNewOrigin);
		Invalidate(); 
		return true; 

	}
	bool CSImageViewerView::ZoomChange(int iChange)
	{
		if((m_iScaleIndex>=SCALE_VAR_NUM-1)&&(iChange>1)){return false;}
		if((m_iScaleIndex<=0)&&(iChange<1)){return false;}

		CPoint pointCurOrigin = GetScrollPosition(); 
		CRect rectClient;
		GetClientRect(&rectClient); 
		CPoint pointCurCenter(pointCurOrigin.x + rectClient.Width() / 2, pointCurOrigin.y + rectClient.Height() / 2); 
		double dScalePre=g_dScale[m_iScaleIndex];

		m_iScaleIndex+=iChange;
		if(m_iScaleIndex>=SCALE_VAR_NUM-1){m_iScaleIndex=SCALE_VAR_NUM-1;}
		if(m_iScaleIndex<=0){m_iScaleIndex=0;}

		SetScrollSizes(MM_TEXT, CSize((int)(m_image[m_iImgIndex].GetWidth() * g_dScale[m_iScaleIndex]), (int)(m_image[m_iImgIndex].GetHeight() * g_dScale[m_iScaleIndex]))); 

		CPoint pointNewOrigin( (int)(pointCurCenter.x * (g_dScale[m_iScaleIndex]/dScalePre) - rectClient.Width() / 2), (int)(pointCurCenter.y * (g_dScale[m_iScaleIndex]/dScalePre) - rectClient.Height() / 2) ); 
		ScrollToPosition(pointNewOrigin);
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
		CPoint pointCurOrigin = GetScrollPosition(); 
		CPoint pointInView(point.x + pointCurOrigin.x, point.y + pointCurOrigin.y);

		int iC_img_Local = (int)(pointInView.x / g_dScale[m_iScaleIndex]);
		int iR_img_Local = (int)(pointInView.y / g_dScale[m_iScaleIndex]);


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


	void CSImageViewerView::OnMouseMove(UINT nFlags, CPoint point)
	{

		if(m_image[m_iImgIndex].IsNull()==true){return;}

		int iR_img,iC_img;
		BYTE byR,byG,byB;
		CString sCaption;
		bool bRet = GetColorAtCursor(point,&iR_img,&iC_img, &byR, &byG, &byB);
		if(bRet == true)
		{
			sCaption.Format(_T("%s | (%d, %d) (R, G, B)= (%d, %d, %d) | %f"), m_sFilePath, iC_img, iR_img, byR, byG, byB,g_dScale[m_iScaleIndex]);
		}
		else
		{
			sCaption.Format(m_sFilePath);
		}
		AfxGetMainWnd()->SetWindowText(sCaption);

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


		CScrollView::OnMouseMove(nFlags, point);
	}


	void CSImageViewerView::OnLButtonDown(UINT nFlags, CPoint point)
	{
		SetCapture(); 
		m_bDragging = true;
		m_PointStart = point; 

		CScrollView::OnLButtonDown(nFlags, point);
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

		CScrollView::OnLButtonUp(nFlags, point);
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

		CScrollView::OnTimer(nIDEvent);
	}


	BOOL CSImageViewerView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
	{
		if (nHitTest == HTCLIENT) 
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
			return TRUE;
		}
		return CScrollView::OnSetCursor(pWnd, nHitTest, message);
	}

	CRect CSImageViewerView::v_to_i(const CRect* rect_v)
	{
		CRect rcImage;
		if(rect_v->IsRectEmpty()==TRUE)
		{
		}
		CPoint scrollPos = GetScrollPosition();


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


	void CSImageViewerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。

		CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
	}


	void CSImageViewerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		// TODO: ここにメッセージ ハンドラー コードを追加するか、既定の処理を呼び出します。

		CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
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
			if(pMsg->wParam == VK_LEFT){ScrollChange(-0.1,0); return TRUE; }
			if(pMsg->wParam == VK_RIGHT){ScrollChange(0.1,0); return TRUE; }
			if(pMsg->wParam == VK_UP){ScrollChange(0,-0.1); return TRUE; }
			if(pMsg->wParam == VK_DOWN){ScrollChange(0,0.1); return TRUE; }
			if((pMsg->wParam == 'B') || (pMsg->wParam == VK_NEXT)){ScrollChange(0,0.5);return TRUE; }
			if(pMsg->wParam == VK_PRIOR){ScrollChange(0,-0.5);return TRUE; }
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

		return CScrollView::PreTranslateMessage(pMsg);
	}


	void CSImageViewerView::OnUpdateEditEquHist(CCmdUI *pCmdUI)
	{
		pCmdUI->Enable(TRUE);
	}
