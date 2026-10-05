#include "stdafx.h"
#pragma once
#include "ImageProc.h"
#include "resource.h"
#include "math.h"
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
	bool m_bCentered;
	CRect m_Rect_i;
	CRect m_Rect_v;

	//	int m_iScaleIndex;
	double m_dScale;
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
	BYTE m_byBG_R;
	BYTE m_byBG_G;
	BYTE m_byBG_B;

public:
	void SetCentered(const bool bTF){m_bCentered=bTF;}
	void SetBGColor(const BYTE byR, const BYTE byG, const BYTE byB)
	{
		m_byBG_R = byR;
		m_byBG_G = byG;
		m_byBG_B = byB;
	}
	bool m_bGridAble;
	bool m_bRegionSelected;
	void GetScrollSetting(ScrollSetting* scr);
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
	const CRect v_to_i(const CRect* rect_v);
	const CRect i_to_v(const CRect* rect_i);
	
	void SetGridEnableDesable();
	void ToggleGridMode(const int iGrid);
	const int GetGridMode(){return m_iGrid;}

	void ToggleRGBSeparate();
	const bool GetRGBSeparateMode(){return m_bRGB_Separate;}

	void ToggleValueMode();
	const bool GetValueMode(){return m_bValue;}

	void SetMouseMode(const MOUSE_MODE iMouseMode){m_enumMouseMode=iMouseMode;}
	const bool GetDragging(){return m_bDragging;}
	const int GetMouseMode(){return m_enumMouseMode;}

	void ViewDraw::SetDispOriginR_tv(const double dIn){m_dDispOriginR_tv=dIn;}
	void ViewDraw::SetDispOriginC_tv(const double dIn){m_dDispOriginC_tv=dIn;}
	const double GetDispOriginR_tv(){return m_dDispOriginR_tv;}
	const double GetDispOriginC_tv(){return m_dDispOriginC_tv;}
	
	const int GetClientHeight(CWnd* wnd);
	const int GetClientWidth(CWnd* wnd);

	void GetSizeIfNoBar(int* iHeightIfNoBar_v, int* iWidthIfNoBar_v, CWnd* wnd);
	
	const int GetScaleIndex();
	const double GetScale(){return m_dScale;}
	void SetScroll(																										const PanImage* panImg, CWnd* wnd);
	void SetScrollPos(const int iR_tv, const int iC_tv,																	const PanImage* panImg, CWnd* wnd);
	const bool ZoomChangeAbs(const double dScale_in,																	const PanImage* panImg, CWnd* wnd, CPoint* ppoint_v);
	const bool ZoomChange(const int iMousePosR_v, const int iMousePosC_v, const double dScale_in,						const PanImage* panImg, CWnd* wnd, CPoint* ppoint_v);
	const bool ZoomChange(const int iR0_i, const int iC0_i, const int iR1_i, const int iC1_i, const bool bStepped,		const PanImage* panImg, CWnd* wnd, CPoint* ppoint_v);
	const bool ZoomChange(const int iChange,																			const PanImage* panImg, CWnd* wnd, CPoint* ppoint_v);
	const bool ZoomChange(const int iMousePosR_v, const int iMousePosC_v, const int iChange,							const PanImage* panImg, CWnd* wnd, CPoint* ppoint_v);
	const bool ZoomChangeAbs(const int iScaleIndex,																		const PanImage* panImg, CWnd* wnd, CPoint* ppoint_v);
//	void ZoomReset(																										const PanImage* panImg, CWnd* wnd);
	void OnMouseMove(UINT nFlags, CPoint point_v,																		const PanImage* panImg, CWnd* wnd);
	void OnLButtonDown(UINT nFlags, CPoint point_v,																		const PanImage* panImg, CWnd* wnd);
	void OnLButtonUp(UINT nFlags, CPoint point_v, const bool bZoomStepped, 												const PanImage* panImg, CWnd* wnd, CPoint* ppoint_v);
	void OnScroll(int iSB, int nSBCode, int nPos,																		const PanImage* panImg, CWnd* wnd);

	void SetPointStart_v(const CPoint p_in){m_PointStart_v.SetPoint(p_in.x, p_in.y);}

	const CPoint GetPointStart_v(){return m_PointStart_v;}

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	void OnDraw(CWnd* wnd, CDC* pDC, const PanImage* panImg);
	const CRect GetRect_i(){return m_Rect_i;}
	const CRect GetRect_v(){return m_Rect_v;}
	void SetRect_i(const CRect* rect_in){ if(rect_in==NULL){m_Rect_i.SetRectEmpty();}else{m_Rect_i=(*rect_in);}}
	void SetRect_v(const CRect* rect_in){ if(rect_in==NULL){m_Rect_v.SetRectEmpty();}else{m_Rect_v=(*rect_in);}}
	void Init()
	{
	m_byBG_R=127;
	m_byBG_G=127;
	m_byBG_B=127;
		m_bCentered=false;

		m_dDispOriginR_tv=0;
		m_dDispOriginC_tv=0;
		m_bCBar = false;
		m_bRBar = false;
		m_bDragging = false;
		m_Rect_v.SetRectEmpty();
		m_Rect_i.SetRectEmpty();
		m_dScale = 1;
		m_bGridAble=false;
		m_bRegionSelected=false;

		m_iGrid = ID_TOOLBAR_GRID_NONE;
		m_bValue = false;
		m_bRGB_Separate = false;
		//		m_bSynchroScroll=false;
	}
	ViewDraw()
	{
		Init();
	}
};