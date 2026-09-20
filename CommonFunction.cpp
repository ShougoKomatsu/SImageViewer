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


BEGIN_MESSAGE_MAP(CPictureCtrlEx, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



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
/*
int ViewDraw::OnLButtonUp(UINT nFlags, CPoint point_v, CWnd* wnd)
{

if (m_bDragging == TRUE) 
{
ReleaseCapture(); 
m_bDragging = false; 

CRect rect_i=GetRect_i();
if(GetPointStart_v() == point_v)
{
if(m_iMouseMode == CHANGE_B){return -1;}
if(m_iMouseMode == CHANGE_L){return -1;}
if(m_iMouseMode == CHANGE_R){return -1;}
if(m_iMouseMode == CHANGE_U){return -1;}
if(m_iMouseMode == CHANGE_LB){return -1;}
if(m_iMouseMode == CHANGE_LU){return -1;}
if(m_iMouseMode == CHANGE_RB){return -1;}
if(m_iMouseMode == CHANGE_RU){return -1;}

CRect rect_v;
rect_v = i_to_v(&rect_i);
if((point_v.y >= rect_v.top)&&(point_v.y <= rect_v.bottom)&&(point_v.x >= rect_v.left)&&(point_v.x <= rect_v.right) && (m_iMouseMode == CHANGE_ZOOMUP))
{
ZoomChange(rect_i.top, rect_i.left, rect_i.bottom,rect_i.right,wnd);
return 0;
}

return 1;
}
rect_i = v_to_i(&(GetRect_v()));
rect_i.left = max(0,rect_i.left);
rect_i.top = max(0,rect_i.top);
SetRect_i(&rect_i);
return 2;
}
return 3;
}
bool ViewDraw::ZoomChange(int iR0_i, int iC0_i, int iR1_i, int iC1_i, CWnd* wnd)
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

double dNewCenterR_i = (iR0_i + iR1_i)/2.0;
double dNewCenterC_i = (iC0_i + iC1_i)/2.0;

double dNewCenterR_tv = dNewCenterR_i*g_dScale[m_iScaleIndex];
double dNewCenterC_tv = dNewCenterC_i*g_dScale[m_iScaleIndex];


int iNewScrollR_tv = max(0,int(dNewCenterR_tv-iHeight_v/2.0));
int iNewScrollC_tv = max(0,int(dNewCenterC_tv-iWidth_v/2.0));

return true; 
}

bool ViewDraw::ZoomChange(int iChange, const CImage* img, CWnd* wnd)
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


SetScrollPos((int)(dNewDispOriginR_tv), (int)(dNewDispOriginC_tv));

if(m_iScaleIndex >= SCALE_VAR_NUM-1){m_iScaleIndex = SCALE_VAR_NUM-1;}
if(m_iScaleIndex <= 0){m_iScaleIndex = 0;}

return true; 
}
*/

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
void CPictureCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{

	CRect rect;
	this->GetWindowRect(&rect);		
	this->GetParent()->ScreenToClient(&rect);
	if(point.x<rect.left){return;}
	if(point.x>rect.right){return;}
	if(point.y<rect.top){return;}
	if(point.y>rect.bottom){return;}

	CStatic::OnMouseMove(nFlags, point);
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

void ViewDraw::OnMouseMove(UINT nFlags, CPoint point_v)
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
	case 1:{if(isInTheRange(point_v.x,rectTemp_v.left,rectTemp_v.right) == true){m_iMouseMode = CHANGE_U;} return;}
	case 2:{if(isInTheRange(point_v.y,rectTemp_v.top,rectTemp_v.bottom) == true){m_iMouseMode = CHANGE_L;} return;}
	case 4:{if(isInTheRange(point_v.y,rectTemp_v.top,rectTemp_v.bottom) == true){m_iMouseMode = CHANGE_R;} return;}
	case 8:{if(isInTheRange(point_v.x,rectTemp_v.left,rectTemp_v.right) == true){m_iMouseMode = CHANGE_B;} return;}
	case 3:{m_iMouseMode = CHANGE_LU; return;}
	case 5:{m_iMouseMode = CHANGE_RU; return;}
	case 10:{m_iMouseMode = CHANGE_LB; return;}
	case 12:{m_iMouseMode = CHANGE_RB; return;}
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
void CPictureCtrlEx::OnPaint()
{
	CPaintDC dc(this);

	CRect rc;
	GetClientRect(&rc);

	if (m_image.IsNull()==true){return;}

	double dZoom=min(rc.Width()/(m_image.GetWidth()*1.0),rc.Height()/(m_image.GetHeight()*1.0));

	CImage imgZoomed;
	ZoomImage(&(m_image),&imgZoomed,0,0,dZoom,rc.Width(), rc.Height(),false);



	if(m_HBar1_i.bValid==true){Line line_v; line_v=i_to_v(&m_HBar1_i); ImposeLine(&imgZoomed, &imgZoomed, line_v.dR0, line_v.dC0, line_v.dR1, line_v.dC1);}
	if(m_HBar2_i.bValid==true){Line line_v; line_v=i_to_v(&m_HBar2_i); ImposeLine(&imgZoomed, &imgZoomed, line_v.dR0, line_v.dC0, line_v.dR1, line_v.dC1);}
	if(m_VBar1_i.bValid==true){Line line_v; line_v=i_to_v(&m_VBar1_i); ImposeLine(&imgZoomed, &imgZoomed, line_v.dR0, line_v.dC0, line_v.dR1, line_v.dC1);}
	if(m_VBar2_i.bValid==true){Line line_v; line_v=i_to_v(&m_VBar2_i); ImposeLine(&imgZoomed, &imgZoomed, line_v.dR0, line_v.dC0, line_v.dR1, line_v.dC1);}

	imgZoomed.BitBlt( dc.GetSafeHdc(), 0, 0,imgZoomed.GetWidth(), imgZoomed.GetHeight(), 0, 0  );
}
bool CopyToClipBoardStr(const CString sValue)
{
	BOOL bRet;
	bRet = OpenClipboard(NULL);
	if(bRet == FALSE){return FALSE;}

	bRet = EmptyClipboard();
	if(bRet == FALSE){return FALSE;}

	HGLOBAL hGL;
	hGL = GlobalAlloc(GPTR, (sValue.GetLength()+1)*sizeof(TCHAR) );
	if(hGL==NULL){return FALSE;}

	_stprintf_s((TCHAR*)hGL,(sValue.GetLength()+1), _T("%s"), sValue);

	HANDLE hResult;
	hResult = SetClipboardData(CF_UNICODETEXT, hGL);
	if(hResult == NULL)
	{
		GlobalFree(hGL);
		return FALSE;
	}

	bRet = CloseClipboard();
	if(bRet == FALSE)
	{
		GlobalFree(hGL);
		return FALSE;
	}
	return TRUE;
}




bool IsImageFIle(CString sFilePath, FileFormatList* fileFormatList)
{
	if(sFilePath.Right(4).CompareNoCase(_T(".bmp"))==0){return true;}
	if(sFilePath.Right(4).CompareNoCase(_T(".png"))==0){return true;}
	if(sFilePath.Right(4).CompareNoCase(_T(".jpg"))==0){return true;}
	if(sFilePath.Right(4).CompareNoCase(_T(".ico"))==0){return true;}
	if(sFilePath.Right(4).CompareNoCase(_T(".exe"))==0){return true;}
	if(sFilePath.Right(4).CompareNoCase(_T(".dll"))==0){return true;}
	for(int i=0; i<fileFormatList->uiNum; i++)
	{
		if(sFilePath.Right(fileFormatList->fileFormat[i].sType.GetLength()).CompareNoCase(fileFormatList->fileFormat[i].sType)==0){return true;}
	}

	return false;
}

UINT CountImageInOneFile(CString sFilePath, FileFormatList* fileFormatList)
{
	if(sFilePath.Right(4).CompareNoCase(_T(".bmp"))==0){return 1;}
	if(sFilePath.Right(4).CompareNoCase(_T(".png"))==0){return 1;}
	if(sFilePath.Right(4).CompareNoCase(_T(".jpg"))==0){return 1;}
	if(sFilePath.Right(4).CompareNoCase(_T(".ico"))==0){return 2*CountIconNum(sFilePath); }
	if(sFilePath.Right(4).CompareNoCase(_T(".exe"))==0){return 2*CountIconNum(sFilePath); }
	if(sFilePath.Right(4).CompareNoCase(_T(".dll"))==0){return 2*CountIconNum(sFilePath); }
	for(int i=0; i<fileFormatList->uiNum; i++)
	{
		if(sFilePath.Right(fileFormatList->fileFormat[i].sType.GetLength()).CompareNoCase(fileFormatList->fileFormat[i].sType)==0){return 1;}
	}
	return 0;
}
bool RecursivelyGetImageFilePaths(CString sFileOrFolderPath, CStringArray* saFilePath, FileFormatList* fileFormatList)
{
	if(sFileOrFolderPath.Find(_T("|"))>=0)
	{
		CStringArray saFilePathTemp;
		saFilePathTemp.RemoveAll();
		int iPlaceStart=0;
		while(1)
		{
			int iPlaceEnd = sFileOrFolderPath.Find(_T("|"),iPlaceStart);
			if(iPlaceEnd<0)
			{
				saFilePathTemp.Add(sFileOrFolderPath.Mid(iPlaceStart,sFileOrFolderPath.GetLength()-iPlaceStart));
				break;
			}
			saFilePathTemp.Add(sFileOrFolderPath.Mid(iPlaceStart,iPlaceEnd-iPlaceStart));
			iPlaceStart=iPlaceEnd+1;
		}
		for(int i=0; i<saFilePathTemp.GetCount(); i++)
		{
			bool bRet = RecursivelyGetImageFilePaths(saFilePathTemp.GetAt(i), saFilePath, fileFormatList);
			if(bRet != true){return false;}
		}
		return true;
	}

	DWORD dwAttribute = GetFileAttributes(sFileOrFolderPath);
	if (dwAttribute == INVALID_FILE_ATTRIBUTES) {return false;}

	if ((dwAttribute & FILE_ATTRIBUTE_DIRECTORY) == 0) 
	{
		if(IsImageFIle(sFileOrFolderPath, fileFormatList)==true)
		{
			saFilePath->Add(sFileOrFolderPath);
		}
		return true;
	}


	CString searchPath = sFileOrFolderPath;
	if (searchPath.Right(1) != _T("\\")) 
	{
		searchPath += _T("\\");
	}
	searchPath += _T("*.*");

	CFileFind cf;
	BOOL bWorking = cf.FindFile(searchPath);

	while (bWorking) 
	{
		bWorking = cf.FindNextFile();
		if (cf.IsDots() == TRUE){continue;}

		CString sFilePath = cf.GetFilePath();

		if (cf.IsDirectory()) 
		{
			bool bRet = RecursivelyGetImageFilePaths(sFilePath, saFilePath, fileFormatList);
			if(bRet != true){return false;}
		}
		else 
		{
			if(IsImageFIle(sFilePath, fileFormatList) != true){continue;}
			saFilePath->Add(sFilePath);
		}
	}

	cf.Close();
	return true;
}

int CountImages(CString sFileOrFolderPath, FileFormatList* fileFormatList)
{
	CStringArray saFilePath;
	saFilePath.RemoveAll();
	bool bRet = RecursivelyGetImageFilePaths(sFileOrFolderPath, &saFilePath, fileFormatList);
	if(bRet != true){return 0;}

	int iFileNum = (int)saFilePath.GetCount();
	int iImageCount=0;
	for(int i=0; i<iFileNum; i++)
	{
		iImageCount += CountImageInOneFile(saFilePath.GetAt(i), fileFormatList);
	}
	return iImageCount;
}

void PanImage::ResetProcessImage()
{
	for(int i=0; i<MAX_IMG_PROCESS; i++)
	{
		if(m_imageProcessed[i].IsNull() != true){m_imageProcessed[i].Destroy();}
	}
	CopyImage_CImage(&cImage, &(m_imageProcessed[0]));
	m_iImgProcessIndex=0;
	m_iReDoAvailableCount=0;
	m_iUnDoAvailableCount=0;
}
bool ReadAndAppendImage(CString sFilePath, FileFormatList* fileFormatList, PanImage* panImage, int iImageIndex, int* iImageIndexNew)
{
	for(int i=0; i<fileFormatList->uiNum; i++)
	{
		if(sFilePath.Right(fileFormatList->fileFormat[i].sType.GetLength()).CompareNoCase(fileFormatList->fileFormat[i].sType)==0)
		{
			return ReadBinaryFile(sFilePath, fileFormatList, panImage);
		}
	}
	if(((sFilePath.Right(4)).CompareNoCase(_T(".ico"))==0)
		||((sFilePath.Right(4)).CompareNoCase(_T(".exe"))==0)
		||((sFilePath.Right(4)).CompareNoCase(_T(".dll"))==0))
	{

		UINT uiIconNum = CountIconNum(sFilePath);
		bool bRet = LoadICOFile(sFilePath,panImage,uiIconNum);
		if(bRet != true){return false;}
		*iImageIndexNew = iImageIndex+uiIconNum;
		return true;
	}
	HRESULT hResult = panImage->SetCImage()->Load(sFilePath);
	if(hResult != S_OK){return false;}
	panImage->SetImageType(IMAGE_TYPE_CIMAGE);
	panImage->SetDataSource(sFilePath);
	panImage->ResetProcessImage();

	*iImageIndexNew = iImageIndex+1;
	return true;
}

bool isNearTheBoarder(double d, double dBoarder, double dMargin)
{
	if(d<dBoarder-dMargin){return false;}
	if(d>dBoarder+dMargin){return false;}
	return true;
}
bool isInTheRange(double d, double dMin, double dMax)
{
	if(d<dMin){return false;}
	if(d>dMax){return false;}
	return true;
}

void QuickSortIndex(const ULONGLONG* iValues, int* iIndex, const int iL, const int iR)
{
	int iL_Local=iL;
	int iR_Local=iR;
	ULONGLONG iPivot = iValues[iIndex[(iL_Local + iR_Local) / 2]];

	while (iL_Local <= iR_Local) 
	{
		while (iValues[iIndex[iL_Local]] < iPivot) {iL_Local++;}
		while (iValues[iIndex[iR_Local]] > iPivot) {iR_Local--;}

		if (iL_Local <= iR_Local) 
		{
			SwapInt(&iIndex[iL_Local], &iIndex[iR_Local]);
			iL_Local++;
			iR_Local--;
		}
	}

	if (iL < iR_Local) {QuickSortIndex(iValues, iIndex, iL, iR_Local);}
	if (iL_Local < iR) {QuickSortIndex(iValues, iIndex, iL_Local, iR);}
}

bool index_i(const ULONGLONG* iValues, const int iLength, int* iIndex)
{
	for (int i = 0; i < iLength; i++) {iIndex[i] = i;}

	QuickSortIndex(iValues, iIndex, 0, iLength - 1);
	return true;
}

bool GetOpenFileList(CString* sFilePaths)
{

	CFileDialog cf(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT , _T(""));
	TCHAR* tchBuf=NULL;
	tchBuf = new TCHAR[100*MAX_PATH];
	for(int i=0; i<100*MAX_PATH; i++)
	{
		tchBuf[i]='\0';
	}
	cf.m_ofn.lpstrFile=tchBuf;

	//		cf.m_ofn.lpstrInitialDir = sMacroFolderPath;		
	if(cf.DoModal()!=IDOK){ SAFE_DELETE(tchBuf); return false;}

	POSITION pos = cf.GetStartPosition();
	int iNum=0;
	while(pos)
	{
		cf.GetNextPathName(pos);
		iNum++;
	}
	if(iNum <= 0){SAFE_DELETE(tchBuf); return false;}

	sFilePaths->Format(_T(""));
	pos=cf.GetStartPosition();
	for(int i=0; i<iNum-1; i++)
	{
		CString sTemp;
		sTemp.Format(_T("%s|"), cf.GetNextPathName(pos));
		sFilePaths->Append(sTemp);
	}
	sFilePaths->Append(cf.GetNextPathName(pos));
	SAFE_DELETE(tchBuf); 
	return true;
}

bool GetImageTypeNum(const CString sIniFilePath, UINT* uiTypeNum)
{
	const UINT uiBufSize=128;
	TCHAR tchData[uiBufSize];

	int i=0;
	while(1)
	{
		CString sKey;
		sKey.Format(_T("Type%d"), i+1);
		GetPrivateProfileString(_T("Types"), sKey, _T(""), tchData, uiBufSize, sIniFilePath);
		if(_tcslen(tchData)<=0){break;}
		i++;
	}
	*uiTypeNum = i;
	return true;
}

bool GetImageType(const CString sIniFilePath, const int iIndexB0, CString* sType)
{
	const UINT uiBufSize=128;
	TCHAR tchData[uiBufSize];

	CString sKey;
	sKey.Format(_T("Type%d"), iIndexB0+1);
	GetPrivateProfileString(_T("Types"), sKey, _T(""), tchData, uiBufSize, sIniFilePath);
	if(_tcslen(tchData)<=0){return false;}


	sType->Format(_T("%s"), tchData);
	return true;
}


bool CopyFromClipBoardStr(CString* sData)
{
	BOOL bRet;

	bRet = OpenClipboard(NULL);
	if(bRet == FALSE){return false;}

	HANDLE hResult;

	hResult = GetClipboardData(CF_UNICODETEXT );
	if(hResult == NULL){return false;}
	LPVOID byDataTemp = GlobalLock(hResult);
	if(byDataTemp==NULL){CloseClipboard();return false;}

	SIZE_T dataSize = GlobalSize(hResult);
	if (dataSize == 0) { GlobalUnlock(hResult);CloseClipboard(); return false;} 

	BYTE* byData;
	byData = new BYTE[dataSize];

	memcpy(byData, byDataTemp, dataSize);

	GlobalUnlock(hResult);

	bRet = CloseClipboard();
	if(bRet == FALSE){SAFE_DELETE(byData); return false;}
	sData->Format(_T("%s"),byData);
	SAFE_DELETE(byData); 

	return true;
}
