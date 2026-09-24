#include "stdafx.h"
#pragma once
#include "ImageProc.h"
#include "resource.h"

#define RECT_CHANGE_MARGIN_PIX (10)
#define SCALE_VAR_NUM (25)
extern double g_dScale[SCALE_VAR_NUM];

enum MOUSE_MODE
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

	CHANGE_LINE_LR = 1,
	CHANGE_LINE_UD = 2,
	CHANGE_LINE_1ST = 3,
	CHANGE_LINE_2ND = 4,

	CHANGE_LINE_OFFSET = 10,
	CHANGE_LINE1_LR = 11,
	CHANGE_LINE1_UD = 12,
	CHANGE_LINE1_1ST = 13,
	CHANGE_LINE1_2ND = 14,
	CHANGE_LINE2_LR = 21,
	CHANGE_LINE2_UD = 22,
	CHANGE_LINE2_1ST = 23,
	CHANGE_LINE2_2ND = 24,
	CHANGE_LINE3_LR = 31,
	CHANGE_LINE3_UD = 32,
	CHANGE_LINE3_1ST = 33,
	CHANGE_LINE3_2ND = 34,
	CHANGE_LINE4_LR = 41,
	CHANGE_LINE4_UD = 42,
	CHANGE_LINE4_1ST = 43,
	CHANGE_LINE4_2ND = 44,

};
struct Line
{
	double dR0, dR1, dC0, dC1;
	bool bValid;
	Line(){Init();}
	~Line(){Init();}
	void Init(){bValid=false;}
	void Set(const double dR0_in, const double dC0_in, const double dR1_in, const double dC1_in)
	{
		dR0=dR0_in;
		dR1=dR1_in;
		dC0=dC0_in;
		dC1=dC1_in;
		bValid=true;
	}

	const void operator = (const Line line_in)
	{
		this->Set(line_in.dR0, line_in.dC0, line_in.dR1, line_in.dC1);
		return;
	}
};

class ViewDraw
{
private:
	CRect m_Rect_i;
	CRect m_Rect_v;
	
	int m_iScaleIndex;
	double m_dDispOriginR_tv;
	double m_dDispOriginC_tv;
	CPoint m_PointStart_v; 

	MOUSE_MODE m_enumMouseMode;
	bool m_bDragging; 
	bool m_bCBar;
	bool m_bRBar;
	int m_iGrid;
	bool m_bValue;
	bool m_bRGB_Separate;
//	bool m_bSynchroScroll;
	

public:
	void GetScrollSetting(ScrollSetting* scr)
	{
		scr->m_dDispOriginC_tv=this->m_dDispOriginC_tv;
		scr->m_dDispOriginR_tv=this->m_dDispOriginR_tv;
		scr->m_iScaleIndex=this->m_iScaleIndex;
	}
	MOUSE_MODE CheckRect(const CPoint* point_v, const CRect* rect_i);
	MOUSE_MODE CheckLine(const CPoint* point_v, const Line* line_i);

	Line m_HBar1_v;
	Line m_HBar2_v;
	Line m_VBar1_v;
	Line m_VBar2_v;

	Line m_HBar1_i;
	Line m_HBar2_i;
	Line m_VBar1_i;
	Line m_VBar2_i;

	const Line v_to_i(const Line* line_v);
	const Line i_to_v(const Line* line_i);

	void ToggleGridMode(const int iGrid);
	void ToggleRGBSeparate();
	const bool GetRGBSeparateMode(){return m_bRGB_Separate;}

	void ToggleValueMode();
	const bool GetValueMode(){return m_bValue;}
	void SetGridEnableDesable();

	const int GetGridMode(){return m_iGrid;}
	void SetMouseMode(const MOUSE_MODE iMouseMode){m_enumMouseMode=iMouseMode;}
	const bool GetDragging(){return m_bDragging;}
	const int GetMouseMode(){return m_enumMouseMode;}
	const double GetScale(){return g_dScale[m_iScaleIndex];}
	void GetSizeIfNoBar(int* iHeightIfNoBar_v, int* iWidthIfNoBar_v, CWnd* wnd, const bool bTopView);

	void SetScroll(															const PanImage* panImg, CWnd* wnd, const bool bTopView);

	void SetScrollPos(int iR_tv, int iC_tv,									const PanImage* panImg, CWnd* wnd, const bool bTopView);
	const bool ZoomChange(int iR0_i, int iC0_i, int iR1_i, int iC1_i,		const PanImage* panImg, CWnd* wnd, const bool bTopView);
	const bool ZoomChange(int iChange,										const PanImage* panImg, CWnd* wnd, const bool bTopView);
	const bool ZoomChange(int iMousePosR_v, int iMousePosC_v, int iChange,	const PanImage* panImg, CWnd* wnd, const bool bTopView);
	const bool ZoomChangeAbs(const int iScaleIndex,							const PanImage* panImg, CWnd* wnd, const bool bTopView);
	void SetDispOriginR_tv(const double dIn);
	void SetDispOriginC_tv(const double dIn);
	const double GetDispOriginR_tv();
	const double GetDispOriginC_tv();

	void ZoomReset(															const PanImage* panImg, CWnd* wnd, const bool bTopView);

	void OnMouseMove(UINT nFlags, CPoint point_v, const PanImage* panImg,  CWnd* wnd, const bool bTopView);
	void OnLButtonDown(UINT nFlags, CPoint point_v,  const PanImage* panImg,  CWnd* wnd, const bool bTopView);
	void OnLButtonUp(UINT nFlags, CPoint point_v,							const PanImage* panImg, CWnd* wnd, const bool bTopView);
	void OnScroll(int iSB, int nSBCode, int nPos,							const PanImage* panImg, CWnd* wnd, const bool bTopView);

	void SetPointStart_v(const CPoint p_in){m_PointStart_v.SetPoint(p_in.x, p_in.y);}

	const CPoint GetPointStart_v(){return m_PointStart_v;}
	
	const CRect v_to_i(const CRect* rect_v);
	const CRect i_to_v(const CRect* rect_i);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	void OnDraw(CWnd* wnd, CDC* pDC, const PanImage* panImg, const bool bTopView);
	const CRect GetRect_i(){return m_Rect_i;}
	const CRect GetRect_v(){return m_Rect_v;}
	void SetRect_i(const CRect* rect_in){ if(rect_in==NULL){m_Rect_i.SetRectEmpty();}else{m_Rect_i=(*rect_in);}}
	void SetRect_v(const CRect* rect_in){ if(rect_in==NULL){m_Rect_v.SetRectEmpty();}else{m_Rect_v=(*rect_in);}}
	void Init()
	{
	m_dDispOriginR_tv=0;
	m_dDispOriginC_tv=0;
		m_bCBar = false;
		m_bRBar = false;
		m_bDragging = false;
		m_Rect_v.SetRectEmpty();
		m_Rect_i.SetRectEmpty();
		m_iScaleIndex = 8;

		m_iGrid = ID_TOOLBAR_GRID_NONE;
		m_bValue = false;
		m_bRGB_Separate = false;
//		m_bSynchroScroll=false;
	}
	ViewDraw()
	{
		Init();
	}
	int GetClientHeight(CWnd* wnd, const bool bTopView)
	{
		CRect rectClient;
		wnd->GetClientRect(&rectClient);
		return rectClient.Height();
	}

	int GetClientWidth(CWnd* wnd, const bool bTopView)
	{
		CRect rectClient;
		wnd->GetClientRect(&rectClient);
		return rectClient.Width();
	}
};