#include "stdafx.h"
#pragma once
#include "ImageProc.h"
#include "resource.h"

#define RECT_CHANGE_MARGIN_PIX (10)
#define SCALE_VAR_NUM (25)
extern double g_dScale[SCALE_VAR_NUM];

enum MOUSE_CURSOR
{
	CHANGE_NONE = 0,
	CHANGE_ZOOMUP = 1,
	CHANGE_L = 2,
	CHANGE_R = 3,
	CHANGE_U = 4,
	CHANGE_B = 5,
	CHANGE_LU = 6,
	CHANGE_RU = 7,
	CHANGE_LB = 8,
	CHANGE_RB = 9,

};

class ViewDraw
{
private:
	CRect m_Rect_i;
	CRect m_Rect_v;

	double m_dDispOriginR_tv;
	double m_dDispOriginC_tv;
	CPoint m_PointStart_v; 
	int m_iScaleIndex;

	int m_iMouseMode;
	bool m_bDragging; 
	bool m_bCBar;
	bool m_bRBar;
	int m_iGrid;
	bool m_bValue;
	bool m_bRGB_Separate;
public:

void ToggleGridMode(const int iGrid);
void ToggleRGBSeparate();
	const bool GetRGBSeparateMode(){return m_bRGB_Separate;}

	void ToggleValueMode();
	const bool GetValueMode(){return m_bValue;}

	const int GetGridMode(){return m_iGrid;}
	void GetSizeIfNoBar(int* iHeightIfNoBar_v, int* iWidthIfNoBar_v, CWnd* wnd);
	void SetMouseMode(const int iMouseMode){m_iMouseMode=iMouseMode;}
	const bool GetDragging(){return m_bDragging;}
	const int GetMouseMode(){return m_iMouseMode;}
	const double GetScale(){return g_dScale[m_iScaleIndex];}
	void SetScroll(const CImage* img, CWnd* wnd);

	void SetGridEnableDesable();
	void SetScrollPos(int iR_tv, int iC_tv, CWnd* wnd);
	bool ZoomChange(int iR0_i, int iC0_i, int iR1_i, int iC1_i, const CImage* img,  CWnd* wnd);
	bool ZoomChange(int iChange,  const CImage* img,  CWnd* wnd);
	bool ZoomChange(int iMousePosR_v, int iMousePosC_v, int iChange,  const CImage* img,  CWnd* wnd);

	void OnLButtonDown(UINT nFlags, CPoint point_v,  const CImage* img,  CWnd* wnd);
	void OnLButtonUp(UINT nFlags, CPoint point_v,  const CImage* img,  CWnd* wnd);
	void OnScroll(int iSB, int nSBCode, int nPos,  const CImage* img,  CWnd* wnd);

	//	int OnLButtonUp(UINT nFlags, CPoint point_v, CWnd* wnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	void SetPointStart_v(const CPoint p_in){m_PointStart_v.SetPoint(p_in.x, p_in.y);}
	const CPoint GetPointStart_v(){return m_PointStart_v;}

	CRect v_to_i(const CRect* rect_v);
	CRect i_to_v(const CRect* rect_i);
	void ZoomReset( const CImage* img,  CWnd* wnd);
	void OnMouseMove(UINT nFlags, CPoint point_v, const CImage* img,  CWnd* wnd);
	double GetDispOriginR_tv();
	double GetDispOriginC_tv();
	void SetDispOriginR_tv(const double dIn){m_dDispOriginR_tv = dIn;}
	void SetDispOriginC_tv(const double dIn){m_dDispOriginC_tv = dIn;};
	void OnDraw(CWnd* wnd, CDC* pDC, const CImage* img, PanImage* panImg);
	const CRect GetRect_i(){return m_Rect_i;}
	const CRect GetRect_v(){return m_Rect_v;}
	void SetRect_i(const CRect* rect_in){ if(rect_in==NULL){m_Rect_i.SetRectEmpty();}else{m_Rect_i=(*rect_in);}}
	void SetRect_v(const CRect* rect_in){ if(rect_in==NULL){m_Rect_v.SetRectEmpty();}else{m_Rect_v=(*rect_in);}}
	void Init()
	{
		m_bCBar = false;
		m_bRBar = false;
		m_bDragging = false;
		m_Rect_v.SetRectEmpty();
		m_Rect_i.SetRectEmpty();
		m_iScaleIndex = 8;

		m_iGrid = ID_TOOLBAR_GRID_NONE;
		m_bValue = false;
		m_bRGB_Separate = false;
	}
	ViewDraw()
	{
		Init();
	}
	int GetClientHeight(CWnd* wnd)
	{
		CRect rectClient;
		wnd->GetClientRect(&rectClient);
		return rectClient.Height();
	}

	int GetClientWidth(CWnd* wnd)
	{
		CRect rectClient;
		wnd->GetClientRect(&rectClient);
		return rectClient.Width();
	}
};