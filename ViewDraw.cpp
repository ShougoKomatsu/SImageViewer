#include "stdafx.h"
#include "CommonFunction.h"

#include "MainFrm.h"

double g_dScale[SCALE_VAR_NUM] = 
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



void ViewDraw::SetScrollPos(int iR_tv, int iC_tv, CWnd* wnd)
{		
	if(wnd==NULL){return;}
	SCROLLINFO si;

	wnd->SetScrollInfo(SB_VERT, &si);
	if(si.nPage>0)
	{
		int iNewPos_scl = (int)(iR_tv*(si.nMax-si.nPage+1.0)/(si.nMax *1.0));
		SetDispOriginR_tv(iR_tv);
		si.nPos = (int)(max(si.nMin,min(si.nMax-si.nPage+1.0,iNewPos_scl) ));;
		wnd->SetScrollInfo(SB_VERT, &si, TRUE);
	}

	wnd->GetScrollInfo(SB_HORZ, &si);
	if(si.nPage>0)
	{
		int iNewPos_scl = (int)(iC_tv*(si.nMax-si.nPage+1.0)/(si.nMax *1.0));
		SetDispOriginC_tv(iC_tv);
		si.nPos = (int)(max(si.nMin,min(si.nMax-si.nPage+1.0, iNewPos_scl) )); 
		wnd->SetScrollInfo(SB_HORZ, &si, TRUE);
	}
}
void ViewDraw::ToggleValueMode()
{
	if(m_bValue == true)
	{
		m_bValue = false;
		return;
	}
	m_bValue = true;
}
void ViewDraw::ToggleRGBSeparate()
{
	if(m_bRGB_Separate == true)
	{
		m_bRGB_Separate = false;
		return;
	}
	m_bRGB_Separate = true;
}
void ViewDraw::ToggleGridMode(const int iGrid)
{
	if(GetGridMode() == iGrid)
	{
		m_iGrid = ID_TOOLBAR_GRID_NONE;
		return;
	}
	m_iGrid = iGrid;
}

void ViewDraw::GetSizeIfNoBar(int* iHeightIfNoBar_v, int* iWidthIfNoBar_v, CWnd* wnd)
{
	int iHeight_v = GetClientHeight(wnd);
	int iWidth_v = GetClientWidth(wnd);
	int iBarWidth = ::GetSystemMetrics(SM_CYHSCROLL);
	int iBarHeight = ::GetSystemMetrics(SM_CXVSCROLL);
	*iHeightIfNoBar_v = iHeight_v+(m_bCBar ? iBarHeight : 0);
	*iWidthIfNoBar_v = iWidth_v+(m_bRBar ? iBarWidth : 0);
}

	void ViewDraw::OnLButtonDown(UINT nFlags, CPoint point_v,  const CImage* img,  CWnd* wnd)
	{
		wnd->SetCapture(); 
		m_bDragging = true;
		SetPointStart_v(point_v); 
	}
	void ViewDraw::OnLButtonUp(UINT nFlags, CPoint point_v,  const CImage* img,  CWnd* wnd)
	{
		if (m_bDragging == TRUE) 
		{
			ReleaseCapture(); 
			m_bDragging = false; 

			CRect rect_i=GetRect_i();
			if(GetPointStart_v() == point_v)
			{
				if(m_iMouseMode == CHANGE_B){return;}
				if(m_iMouseMode == CHANGE_L){return;}
				if(m_iMouseMode == CHANGE_R){return;}
				if(m_iMouseMode == CHANGE_U){return;}
				if(m_iMouseMode == CHANGE_LB){return;}
				if(m_iMouseMode == CHANGE_LU){return;}
				if(m_iMouseMode == CHANGE_RB){return;}
				if(m_iMouseMode == CHANGE_RU){return;}

				CRect rect_v;
				rect_v = i_to_v(&rect_i);
				if((point_v.y >= rect_v.top)&&(point_v.y <= rect_v.bottom)&&(point_v.x >= rect_v.left)&&(point_v.x <= rect_v.right) && (m_iMouseMode == CHANGE_ZOOMUP))
				{
					ZoomChange(rect_i.top, rect_i.left, rect_i.bottom,rect_i.right, img, wnd);
					SetRect_v(NULL);
					SetRect_i(NULL);
					m_iMouseMode = CHANGE_NONE;
					CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
					pFrame->m_bRegionSelected = false;
					wnd->Invalidate();
					return;
				}
				SetRect_v(NULL);
				SetRect_i(NULL);
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				pFrame->m_bRegionSelected = false;
				wnd->Invalidate();
				return;
			}
			rect_i = v_to_i(&(GetRect_v()));
			rect_i.left = max(0,rect_i.left);
			rect_i.top = max(0,rect_i.top);
			SetRect_i(&rect_i);
			SetRect_v(NULL);
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->m_bRegionSelected = true;
			wnd->Invalidate();
		}
		return;
	}
	void ViewDraw::OnScroll(int iSB, int nSBCode, int nPos,  const CImage* img,  CWnd* wnd)
	{
		
		if((iSB == SB_VERT) && (m_bRBar == false)){return ;}
		if((iSB == SB_HORZ) && (m_bCBar == false)){return ;}

		int iHeight_v = GetClientHeight(wnd);
		int iWidth_v = GetClientWidth(wnd);

		int iBarWidth = ::GetSystemMetrics(SM_CYHSCROLL);
		int iBarHeight = ::GetSystemMetrics(SM_CXVSCROLL);

		int iWidth_i = max(0,img->GetWidth());
		int iHeight_i = max(0,img->GetHeight());

		int iWidth_tv = (int)(iWidth_i*g_dScale[m_iScaleIndex]);
		int iHeight_tv = (int)(iHeight_i*g_dScale[m_iScaleIndex]);



		int iHeightIfNoBar_v = iHeight_v+(m_bCBar ? iBarHeight : 0);
		int iWidthIfNoBar_v = iWidth_v+(m_bRBar ? iBarWidth : 0);


		SCROLLINFO si;
		wnd->GetScrollInfo(iSB,&si);
		if(si.nPage == 0){return ;}

		int iPageSize = si.nPage;
		int iMin = si.nMin;
		int iMax = si.nMax;
		int iTrackPos = si.nTrackPos ;
		int iOldPos_scl;
		if(iSB == SB_VERT)
		{
			iOldPos_scl = (int)(GetDispOriginR_tv()*(iMax-iPageSize+1.0)/(iMax*1.0));
		}
		else
		{
			iOldPos_scl = (int)(GetDispOriginC_tv()*(iMax-iPageSize+1.0)/(iMax*1.0));
		}


		int iStep;
		if(g_dScale[m_iScaleIndex] == 64)	{iStep = 64;}
		else{iStep = max(int(g_dScale[m_iScaleIndex]),(int)(iPageSize/8.0));}

		int iNewPos_scl;
		switch (nSBCode)
		{
		case SB_LINEUP:		{iNewPos_scl = max(iMin, iOldPos_scl-iStep); break;}
		case SB_LINEDOWN:	{iNewPos_scl = min(iMax-iPageSize+1, iOldPos_scl+iStep); break;}
		case SB_PAGEUP:		{iNewPos_scl = max(iMin, iOldPos_scl-iPageSize); break;}
		case SB_PAGEDOWN:	{iNewPos_scl = min(iMax-iPageSize+1, iOldPos_scl+iPageSize); break;}
		case SB_THUMBTRACK:	{iNewPos_scl = max(iMin,min(iMax-iPageSize+1 , iTrackPos)); break;}
		default:{return;}
		}

		if(iSB == SB_VERT)
		{
			SetDispOriginR_tv(max(0, iMax*(iNewPos_scl*1.0)/(iMax-iPageSize+1.0)));
		}
		else
		{
			SetDispOriginC_tv(max(0, iMax*(iNewPos_scl*1.0)/(iMax-iPageSize+1.0)));
		}
		si.nPos = (iNewPos_scl); 
		wnd->SetScrollInfo(iSB, &si, TRUE);
		wnd->Invalidate();
	}

	
double ViewDraw::GetDispOriginR_tv()
{
	return m_dDispOriginR_tv;
}
double ViewDraw::GetDispOriginC_tv()
{
	return m_dDispOriginC_tv;
}

CRect ViewDraw::v_to_i(const CRect* rect_v)
{
	CRect rect_i;
	rect_i.SetRectEmpty();
	if(rect_v->IsRectNull() == TRUE)
	{
		return rect_i;
	}

	int iCOrigin_tv = (int)(GetDispOriginC_tv());
	int iROrigin_tv = (int)(GetDispOriginR_tv());

	rect_i.SetRect(
		(int)(((rect_v->left+ iCOrigin_tv) / g_dScale[m_iScaleIndex]) +0.5)
		,(int)(((rect_v->top+ iROrigin_tv) / g_dScale[m_iScaleIndex]) +0.5)
		,(int)(((rect_v->right+ iCOrigin_tv) / g_dScale[m_iScaleIndex]) -0.5)
		,(int)(((rect_v->bottom+ iROrigin_tv) / g_dScale[m_iScaleIndex]) -0.5));
	if(rect_i.right<rect_i.left){rect_i.right = rect_i.left;}
	if(rect_i.bottom<rect_i.top){rect_i.bottom = rect_i.top;}
	return rect_i;
}

CRect ViewDraw::i_to_v(const CRect* rect_i)
{
	CRect rect_v;
	if(rect_i->IsRectNull() == TRUE)
	{
		rect_v.SetRectEmpty();
		return rect_v;
	}

	int iCOrigin_tv = (int)GetDispOriginC_tv();
	int iROrigin_tv = (int)GetDispOriginR_tv();

	rect_v.SetRect(
		(int)((rect_i->left ) * g_dScale[m_iScaleIndex])-iCOrigin_tv
		,(int)((rect_i->top ) * g_dScale[m_iScaleIndex])-iROrigin_tv
		,(int)((rect_i->right +1 ) * g_dScale[m_iScaleIndex])-iCOrigin_tv
		,(int)((rect_i->bottom +1) * g_dScale[m_iScaleIndex])-iROrigin_tv);

	return rect_v;
}

	bool ViewDraw::ZoomChange(int iMousePosR_v, int iMousePosC_v, int iChange,  const CImage* img,  CWnd* wnd)
	{
		
		if((m_iScaleIndex >= SCALE_VAR_NUM-1)&&(iChange>0)){return false;}
		if((m_iScaleIndex <= 0)&&(iChange<0)){return false;}

		double dMousePosR_tv = iMousePosR_v + GetDispOriginR_tv();
		double dMousePosC_tv = iMousePosC_v + GetDispOriginC_tv();

		double dMousePosR_i = dMousePosR_tv/g_dScale[m_iScaleIndex];
		double dMousePosC_i = dMousePosC_tv/g_dScale[m_iScaleIndex];


		int iHeight_v = GetClientHeight(wnd);
		int iWidth_v = GetClientWidth(wnd);
		double dOldDispOriginR_tv = GetDispOriginR_tv();
		double dOldDispOriginC_tv = GetDispOriginC_tv();

		int iWidth_i = max(0,img->GetWidth());
		int iHeight_i = max(0,img->GetHeight());

		double dWidth_tv = iWidth_i*g_dScale[m_iScaleIndex];
		double dHeight_tv = iHeight_i*g_dScale[m_iScaleIndex];
		double dNewDispOriginC_tv;
		double dNewDispOriginR_tv;
		m_iScaleIndex+= iChange;		

		SetGridEnableDesable();
		SetScroll(img, wnd);
		if(dWidth_tv>iWidth_v)
		{
			double dNewMousePosC_tv = dMousePosC_i*g_dScale[m_iScaleIndex];
			dNewDispOriginC_tv = max(0, (dNewMousePosC_tv-iMousePosC_v));
		}
		else
		{
			dNewDispOriginC_tv = 0;
		}

		if(dHeight_tv>iHeight_v)
		{
			double dNewMousePosR_tv = dMousePosR_i*g_dScale[m_iScaleIndex];
			dNewDispOriginR_tv = max(0, dNewMousePosR_tv-iMousePosR_v);
		}
		else
		{
			dNewDispOriginR_tv = 0;
		}

		SetScrollPos((int)(dNewDispOriginR_tv), (int)(dNewDispOriginC_tv), wnd);

		wnd->Invalidate();

		CPoint point_v;
		GetCursorPos(&point_v);
		wnd->ScreenToClient(&point_v);
		((CSImageViewerView*)wnd)->DispStatus(point_v);
		return true;
	}

	bool ViewDraw::ZoomChange(int iR0_i, int iC0_i, int iR1_i, int iC1_i, const CImage* img,  CWnd* wnd)
	{
		int iHeight_v = GetClientHeight(wnd);
		int iWidth_v = GetClientWidth(wnd);

		int iNewScaleIndex = m_iScaleIndex;
		for(int i = SCALE_VAR_NUM-1; i >= 0; i--)
		{
			if((iHeight_v>(iR1_i-iR0_i+1)*g_dScale[i]) && (iWidth_v>(iC1_i-iC0_i+1)*g_dScale[i]))
			{
				iNewScaleIndex = i;
				break;
			}
		}


		m_iScaleIndex = iNewScaleIndex;
		if(m_iScaleIndex >= SCALE_VAR_NUM-1){m_iScaleIndex = SCALE_VAR_NUM-1;}
		if(m_iScaleIndex <= 0){m_iScaleIndex = 0;}

		SetGridEnableDesable();

		SetScroll(img, wnd);

		double dNewCenterR_i = (iR0_i + iR1_i)/2.0;
		double dNewCenterC_i = (iC0_i + iC1_i)/2.0;

		double dNewCenterR_tv = dNewCenterR_i*g_dScale[m_iScaleIndex];
		double dNewCenterC_tv = dNewCenterC_i*g_dScale[m_iScaleIndex];


		int iNewScrollR_tv = max(0,int(dNewCenterR_tv-iHeight_v/2.0));
		int iNewScrollC_tv = max(0,int(dNewCenterC_tv-iWidth_v/2.0));

		SetScrollPos(iNewScrollR_tv, iNewScrollC_tv, wnd);


		wnd->Invalidate();

		CPoint point_v;
		GetCursorPos(&point_v);
		wnd->ScreenToClient(&point_v);
		((CSImageViewerView*)wnd)->DispStatus(point_v);
		return true; 
	}

void ViewDraw::OnMouseMove(UINT nFlags, CPoint point_v, const CImage* img,  CWnd* wnd)
{


	if (m_bDragging == true) 
	{ 
		switch(m_iMouseMode)
		{
		case CHANGE_U: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(rectTemp_v.left,point_v.y), CPoint(rectTemp_v.right,rectTemp_v.bottom)); break;}
		case CHANGE_B: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(rectTemp_v.left,rectTemp_v.top), CPoint(rectTemp_v.right,point_v.y)); break;}
		case CHANGE_L: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(point_v.x,rectTemp_v.top), CPoint(rectTemp_v.right,rectTemp_v.bottom)); break;}
		case CHANGE_R: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(rectTemp_v.left, rectTemp_v.top), CPoint(point_v.x,rectTemp_v.bottom)); break;}
		case CHANGE_LU: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(point_v.x, point_v.y), CPoint(rectTemp_v.right,rectTemp_v.bottom)); break;}
		case CHANGE_RU: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(rectTemp_v.left, point_v.y), CPoint(point_v.x,rectTemp_v.bottom)); break;}
		case CHANGE_LB: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(point_v.x, rectTemp_v.top), CPoint(rectTemp_v.right,point_v.y)); break;}
		case CHANGE_RB: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(rectTemp_v.left, rectTemp_v.top), CPoint(point_v.x,point_v.y)); break;}
		default :
			{
				m_Rect_v = CRect(m_PointStart_v, point_v);
			}
		}
		m_Rect_v.NormalizeRect();
		wnd->Invalidate();
		return;
	} 


	CRect rectTemp_v;
	rectTemp_v = i_to_v(&m_Rect_i);

	int iBoarder = 0;
	if(isNearTheBoarder(point_v.y, rectTemp_v.top,RECT_CHANGE_MARGIN_PIX) == true){iBoarder += 1;}
	if(isNearTheBoarder(point_v.x, rectTemp_v.left,RECT_CHANGE_MARGIN_PIX) == true){iBoarder += 2;}
	if(isNearTheBoarder(point_v.x, rectTemp_v.right,RECT_CHANGE_MARGIN_PIX) == true){iBoarder += 4;}
	if(isNearTheBoarder(point_v.y, rectTemp_v.bottom,RECT_CHANGE_MARGIN_PIX) == true){iBoarder += 8;}

	switch(iBoarder)
	{
	case 1:{if(isInTheRange(point_v.x,rectTemp_v.left,rectTemp_v.right) == true){m_iMouseMode = CHANGE_U;} wnd->Invalidate(); return;}
	case 2:{if(isInTheRange(point_v.y,rectTemp_v.top,rectTemp_v.bottom) == true){m_iMouseMode = CHANGE_L;} wnd->Invalidate(); return;}
	case 4:{if(isInTheRange(point_v.y,rectTemp_v.top,rectTemp_v.bottom) == true){m_iMouseMode = CHANGE_R;} wnd->Invalidate(); return;}
	case 8:{if(isInTheRange(point_v.x,rectTemp_v.left,rectTemp_v.right) == true){m_iMouseMode = CHANGE_B;} wnd->Invalidate(); return;}
	case 3:{m_iMouseMode = CHANGE_LU; wnd->Invalidate(); return;}
	case 5:{m_iMouseMode = CHANGE_RU; wnd->Invalidate(); return;}
	case 10:{m_iMouseMode = CHANGE_LB; wnd->Invalidate(); return;}
	case 12:{m_iMouseMode = CHANGE_RB; wnd->Invalidate(); return;}
	default:{break;}
	}
	if((point_v.y >= rectTemp_v.top)&&(point_v.y <= rectTemp_v.bottom)&&(point_v.x >= rectTemp_v.left)&&(point_v.x <= rectTemp_v.right))
	{
		m_iMouseMode = CHANGE_ZOOMUP;
	}
	else
	{
		m_iMouseMode = CHANGE_NONE;
	}
	wnd->Invalidate(); 
}

void ViewDraw::ZoomReset( const CImage* img,  CWnd* wnd)
{
			m_iScaleIndex = 8;
			CRect rectClient;
			wnd->GetClientRect(&rectClient);

			int iHeight_v = GetClientHeight(wnd);
			int iWidth_v = GetClientWidth(wnd);

			int iBarWidth = ::GetSystemMetrics(SM_CYHSCROLL);
			int iBarHeight = ::GetSystemMetrics(SM_CXVSCROLL);

			int iWidth_i = max(0,img->GetWidth());
			int iHeight_i = max(0,img->GetHeight());

			int iWidth_tv = (int)(iWidth_i*g_dScale[m_iScaleIndex]);
			int iHeight_tv = (int)(iHeight_i*g_dScale[m_iScaleIndex]);

			SCROLLINFO si = { 0 };

			wnd->GetScrollInfo(SB_VERT, &si);
			if(si.nPage == 0){m_bRBar = false;}

			wnd->GetScrollInfo(SB_HORZ, &si);
			if(si.nPage == 0){m_bCBar = false;}

			int iHeightIfNoBar_v = iHeight_v+(m_bCBar ? iBarHeight : 0);
			int iWidthIfNoBar_v = iWidth_v+(m_bRBar ? iBarWidth : 0);
			
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->AdjustViewClientSize(img->GetWidth(), img->GetHeight(),iWidthIfNoBar_v, iHeightIfNoBar_v);
			SetScroll(img, wnd);

			SetDispOriginC_tv(0);
			SetDispOriginR_tv(0);

			wnd->GetScrollInfo(SB_HORZ, &si);
			if(si.nPage>0)
			{
				si.nPos = 0; 
				wnd->SetScrollInfo(SB_HORZ, &si, TRUE);
			}

			wnd->GetScrollInfo(SB_VERT, &si);
			if(si.nPage>0)
			{
				si.nPos = 0; 
				wnd->SetScrollInfo(SB_VERT, &si, TRUE);
			}
}

void ViewDraw::OnDraw(CWnd* wnd, CDC* pDC, const CImage* img, PanImage* panImg)
{

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	CImage imgZoomed;
	int iHeight_v = GetClientHeight(wnd);
	int iWidth_v = GetClientWidth(wnd);

	CBitmap bufferBmp; 
	bufferBmp.CreateCompatibleBitmap(pDC, iWidth_v, iHeight_v);
	CBitmap* pOldBmp = memDC.SelectObject(&bufferBmp);

	double dDispOriginR_tv = GetDispOriginR_tv();
	double dDispOriginC_tv = GetDispOriginC_tv();



	double dR0_i = (dDispOriginR_tv/g_dScale[m_iScaleIndex]);
	double dC0_i = (dDispOriginC_tv/g_dScale[m_iScaleIndex]);
	if (img->IsNull()){return;}

	int iRMax = img->GetHeight()-1;
	int iCMax = img->GetWidth()-1;


	ZoomImage(img,&imgZoomed,dR0_i,dC0_i,g_dScale[m_iScaleIndex],iWidth_v,iHeight_v,m_bRGB_Separate);
	CImage imgValue;
	ZoomImage(img,&imgValue,dR0_i,dC0_i,g_dScale[m_iScaleIndex],iWidth_v,iHeight_v,false);

	int iGrid = 0;
	switch(m_iGrid)
	{
	case ID_TOOLBAR_GRID_NONE:{iGrid = 0;break;}
	case ID_TOOLBAR_GRID_DOT:{iGrid = 1;break;}
	case ID_TOOLBAR_GRID_LINE:{iGrid = 2;break;}
	case ID_TOOLBAR_GRID_CONNECT:{iGrid = 3;break;}
	default:{iGrid = 0;}
	}
	ImposeGrid(&imgValue, &imgZoomed, &imgZoomed, iGrid, int(dR0_i)-dR0_i, int(dC0_i)-dC0_i,g_dScale[m_iScaleIndex], 10, iRMax, iCMax);
	if(m_bValue == true)
	{
		ImposeRGBValue(panImg, &imgZoomed, &imgZoomed, iGrid, int(dR0_i)-dR0_i, int(dC0_i)-dC0_i,g_dScale[m_iScaleIndex], 10, int(dR0_i),int(dC0_i),iRMax, iCMax);
	}
	if(m_bDragging == true)
	{
		if (m_Rect_v.IsRectNull() == FALSE)
		{
			CRect rect_i = v_to_i(&m_Rect_v);
			CRect rect_v = i_to_v(&rect_i);
			ImposeRect(&imgZoomed, &imgZoomed,&rect_v);
		}
	}
	else
	{
		if (m_Rect_i.IsRectNull() == FALSE)
		{
			CRect rect_v = i_to_v(&m_Rect_i);
			ImposeRect(&imgZoomed, &imgZoomed,&rect_v);
		}
	}
	CImage imgAlphaed;
	ImposeAlphaChannel(&imgZoomed,&imgAlphaed);

	imgAlphaed.BitBlt( memDC.GetSafeHdc(), 0, 0,imgAlphaed.GetWidth(), imgAlphaed.GetHeight(), 0, 0 );
	int ii = imgAlphaed.GetWidth();
	pDC->BitBlt(0, 0, iWidth_v, iHeight_v, &memDC, 0, 0,SRCCOPY);

	memDC.SelectObject(pOldBmp);
}


void CheckIfScrollBarsAreNeeded(const int iWidth_tv, const int iHeight_tv, const int iWidthIfNoBar_v, const int iHeightIfNoBar_v, const int iBarWidth, const int iBarHeight, bool* bRBar, bool* bCBar)
{

	if((iWidth_tv <= iWidthIfNoBar_v)&&(iHeight_tv <= iHeightIfNoBar_v))
	{
		*bRBar = false;
		*bCBar = false;
		return;
	}

	if(iWidth_tv>iWidthIfNoBar_v)
	{
		*bCBar = true;

		if(iHeight_tv <= iHeightIfNoBar_v-iBarHeight)
		{
			*bRBar = false;
			return;
		}
		*bRBar = true;
		return;
	}

	*bRBar = true;
	if(iWidth_tv <= iWidthIfNoBar_v-iBarWidth)
	{
		*bCBar = false;
		return;
	}
	*bCBar = true;
	return;

}

void ViewDraw::SetScroll(const CImage* img, CWnd* wnd)
{
	int iHeight_v = GetClientHeight(wnd);
	int iWidth_v = GetClientWidth(wnd);

	int iBarWidth = ::GetSystemMetrics(SM_CYHSCROLL);
	int iBarHeight = ::GetSystemMetrics(SM_CXVSCROLL);

	int iWidth_i = max(0,img->GetWidth());
	int iHeight_i = max(0,img->GetHeight());

	int iWidth_tv = (int)(iWidth_i*g_dScale[m_iScaleIndex]);
	int iHeight_tv = (int)(iHeight_i*g_dScale[m_iScaleIndex]);

	SCROLLINFO si = { 0 };
	int iPageV, iPageH;
	wnd->GetScrollInfo(SB_VERT, &si);
	if(si.nPage == 0){m_bRBar = false;}
	iPageV = si.nPage;

	wnd->GetScrollInfo(SB_HORZ, &si);
	if(si.nPage == 0){m_bCBar = false;}
	iPageH = si.nPage;

	int iHeightIfNoBar_v = iHeight_v+(m_bCBar ? iBarHeight : 0);
	int iWidthIfNoBar_v = iWidth_v+(m_bRBar ? iBarWidth : 0);

	bool bRBar = false;
	bool bCBar = false;


	CheckIfScrollBarsAreNeeded(iWidth_tv, iHeight_tv, iWidthIfNoBar_v, iHeightIfNoBar_v, iBarWidth, iBarHeight, &bRBar, &bCBar);

	if(bRBar == true)
	{
		wnd->GetScrollInfo(SB_VERT, &si);
		int iPageR = (iHeightIfNoBar_v-(bCBar ? iBarHeight:0))/10;
		si.nMin = 0;
		si.nMax = max(0,(iHeight_tv-(iHeightIfNoBar_v-(bCBar ? iBarHeight:0)))+1);
		si.nPage = min(si.nMax,iPageR);
		if(si.nPage>0){m_bRBar = true;}else{m_bRBar = false;}
		wnd->SetScrollInfo(SB_VERT, &si, TRUE);
	}
	else
	{
		SetDispOriginR_tv(0);
		si.nMin = 0;
		si.nMax = 0;
		si.nPage = 0;
		m_bRBar = false;
		wnd->SetScrollInfo(SB_VERT, &si, TRUE);
	}

	if(bCBar == true)
	{
		int iPageC = (iWidthIfNoBar_v-(bRBar ? iBarWidth :0))/10;
		wnd->GetScrollInfo(SB_HORZ, &si);
		si.nMin = 0;
		si.nMax = max(0,(iWidth_tv-(iWidthIfNoBar_v-(bRBar ? iBarWidth :0)))+1);
		si.nPage = min(si.nMax, iPageC);
		if(si.nPage>0){m_bCBar = true;}else{m_bCBar = false;}
		wnd->SetScrollInfo(SB_HORZ, &si, TRUE);
	}
	else
	{
		SetDispOriginC_tv(0);
		si.nMin = 0;
		si.nMax = 0;
		si.nPage = 0;
		m_bCBar = false;
		wnd->SetScrollInfo(SB_HORZ, &si, TRUE);
	}

}
void ViewDraw::SetGridEnableDesable()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if(g_dScale[m_iScaleIndex]>10){	pFrame->m_bGridAble = true;}
	else{pFrame->m_bGridAble = false;}
}

bool ViewDraw::ZoomChange(int iChange,  const CImage* img,  CWnd* wnd)
{
	if((m_iScaleIndex >= SCALE_VAR_NUM-1)&&(iChange>0)){return false;}
	if((m_iScaleIndex <= 0)&&(iChange<0)){return false;}

	int iHeight_v = GetClientHeight(wnd);
	int iWidth_v = GetClientWidth(wnd);
	double dOldDispOriginR_tv = GetDispOriginR_tv();
	double dOldDispOriginC_tv = GetDispOriginC_tv();

	int iWidth_i = max(0,img->GetWidth());
	int iHeight_i = max(0,img->GetHeight());

	int iOldZoom = m_iScaleIndex;
	double dOldWidth_tv = iWidth_i*g_dScale[iOldZoom];
	double dNewDispOriginC_tv;
	double dNewDispOriginR_tv;
	int iNewZoom = m_iScaleIndex+= iChange;
	double dNewWidth_tv = iWidth_i*g_dScale[iNewZoom];
	double dNewHeight_tv = iHeight_i*g_dScale[iNewZoom];


	if(dNewWidth_tv>iWidth_v)
	{
		double dOldCenterC_i = (dOldDispOriginC_tv +iWidth_v/2.0)/g_dScale[iOldZoom];
		double dOldEndC_i = (dOldDispOriginC_tv +iWidth_v)/g_dScale[iOldZoom];
		dNewDispOriginC_tv = max(0, min(dOldCenterC_i*g_dScale[iNewZoom] - iWidth_v/2.0, dOldEndC_i*g_dScale[iNewZoom]-iWidth_v));
	}
	else
	{
		dNewDispOriginC_tv = 0;
	}

	if(dNewHeight_tv>iHeight_v)
	{
		double dOldCenterR_i = (dOldDispOriginR_tv +iHeight_v/2.0)/g_dScale[iOldZoom];
		double dOldEndR_i = (dOldDispOriginR_tv +iHeight_v)/g_dScale[iOldZoom];
		dNewDispOriginR_tv = max(0, min(dOldCenterR_i*g_dScale[iNewZoom] - iHeight_v/2.0, dOldEndR_i*g_dScale[iNewZoom]-iHeight_v));
	}
	else
	{
		dNewDispOriginR_tv = 0;
	}
	m_iScaleIndex = iNewZoom;


	SetScrollPos((int)(dNewDispOriginR_tv), (int)(dNewDispOriginC_tv), wnd);

	if(m_iScaleIndex >= SCALE_VAR_NUM-1){m_iScaleIndex = SCALE_VAR_NUM-1;}
	if(m_iScaleIndex <= 0){m_iScaleIndex = 0;}

	SetGridEnableDesable();
	SetScroll(img, wnd);
	wnd->Invalidate();

	CPoint point_v;
	GetCursorPos(&point_v);
	wnd->ScreenToClient(&point_v);
	((CSImageViewerView*)wnd)->DispStatus(point_v);
	return true; 
}


BOOL ViewDraw::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCLIENT) 
	{
		switch(m_iMouseMode)
		{
		case CHANGE_NONE:{SetCursor(AfxGetApp()->LoadCursorW(IDC_CURSOR_CROSS));return TRUE;}
		case CHANGE_L:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));return TRUE;}
		case CHANGE_R:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));return TRUE;}
		case CHANGE_U:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));return TRUE;}
		case CHANGE_B:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));return TRUE;}
		case CHANGE_LU:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE));return TRUE;}
		case CHANGE_RB:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE));return TRUE;}
		case CHANGE_RU:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENESW));return TRUE;}
		case CHANGE_LB:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENESW));return TRUE;}
		case CHANGE_ZOOMUP:{SetCursor(AfxGetApp()->LoadCursorW(IDC_CURSOR_ZOOMIN));return TRUE;}
		}
	}
	return FALSE;
}