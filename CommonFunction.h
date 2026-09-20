#include "stdafx.h"
#pragma once
#include "ImageProc.h"
#include "resource.h"

#define RECT_CHANGE_MARGIN_PIX (10)
#define SCALE_VAR_NUM (25)
extern double g_dScale[SCALE_VAR_NUM];

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

	const Line operator = (const Line line_in)
	{
		Line line;
		line.Set(line_in.dR0, line_in.dC0, line_in.dR1, line_in.dC1);
		return line;
	}
};


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

public:
		void SetScrollPos(int iR_tv, int iC_tv, CWnd* wnd);
//	bool ZoomChange(int iR0_i, int iC0_i, int iR1_i, int iC1_i, CWnd* wnd);
//	bool ZoomChange(int iChange,  const CImage* img,  CWnd* wnd);
//	int OnLButtonUp(UINT nFlags, CPoint point_v, CWnd* wnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	int m_iGrid;
	bool m_bValue;
	bool m_bRGB_Separate;

	void SetPointStart_v(const CPoint p_in){m_PointStart_v.SetPoint(p_in.x, p_in.y);}
	const CPoint GetPointStart_v(){return m_PointStart_v;}

	int m_iMouseMode;
	int m_iScaleIndex;
	bool m_bDragging; 
	CRect v_to_i(const CRect* rect_v);
	CRect i_to_v(const CRect* rect_i);
	void OnMouseMove(UINT nFlags, CPoint point_v);
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

class CPictureCtrlEx : public CStatic
{
public:
	ViewDraw view;
	CImage m_image;

	Line m_HBar1_v;
	Line m_HBar2_v;
	Line m_VBar1_v;
	Line m_VBar2_v;

	Line m_HBar1_i;
	Line m_HBar2_i;
	Line m_VBar1_i;
	Line m_VBar2_i;


	double GetDispOriginR_tv()
	{
		return view.GetDispOriginR_tv();
	}
	double GetDispOriginC_tv()
	{
		return view.GetDispOriginC_tv();
	}


	const Line v_to_i(const Line* line_v)
	{
		Line line;
		if(line_v->bValid == false)
		{
			line.Init();
			return line;
		}

		int iCOrigin_tv = (int)(GetDispOriginC_tv());
		int iROrigin_tv = (int)(GetDispOriginR_tv());

		line.Set(
			((line_v->dR0+ iROrigin_tv) / g_dScale[view.m_iScaleIndex]) +0.5
			,((line_v->dC0+ iCOrigin_tv) / g_dScale[view.m_iScaleIndex]) +0.5
			,((line_v->dR0+ iROrigin_tv) / g_dScale[view.m_iScaleIndex]) -0.5
			,((line_v->dC1+ iCOrigin_tv) / g_dScale[view.m_iScaleIndex]) -0.5);

		double dTemp;
		dTemp=line.dR0;
		line.dR0=min(line.dR0, line.dR1);
		line.dR1=max(dTemp, line.dR1);

		dTemp=line.dC0;
		line.dC0=min(line.dC0, line.dC1);
		line.dC1=max(dTemp, line.dC1);
		return line;
	}

	const Line i_to_v(const Line* line_i)
	{
		Line line;
		if(line_i->bValid == false)
		{
			line.Init();
			return line;
		}

		int iCOrigin_tv = (int)GetDispOriginC_tv();
		int iROrigin_tv = (int)GetDispOriginR_tv();

		line.Set(
			((line_i->dR0) * g_dScale[view.m_iScaleIndex])-iROrigin_tv
			,((line_i->dC0) * g_dScale[view.m_iScaleIndex])-iCOrigin_tv
			,((line_i->dR1+1) * g_dScale[view.m_iScaleIndex])-iROrigin_tv
			,((line_i->dC1+1 ) * g_dScale[view.m_iScaleIndex])-iCOrigin_tv);

		return line;
	}


	CPictureCtrlEx()
	{
	}

protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

bool CopyFromClipBoardStr(CString* sData);


bool isNearTheBoarder(double d, double dBoarder, double dMargin);
bool isInTheRange(double d, double dMin, double dMax);
bool CopyToClipBoardStr(const CString sValue);
int CountImages(CString sFileOrFolderPath, FileFormatList* fileFormatList);
bool RecursivelyGetImageFilePaths(CString sFileOrFolderPath, CStringArray* saFilePath, FileFormatList* fileFormatList);
bool ReadAndAppendImage(CString sFilePath, FileFormatList* fileFormatList, PanImage* panImage, int iImageIndex, int* iImageIndexNew);
bool GetOpenFileList(CString* sFilePaths);


void QuickSortIndex(const ULONGLONG* iValues, int* iIndex, const int iL, const int iR);
bool index_i(const ULONGLONG* iValues, const int iLength, int* iIndex);

inline void SwapInt(int *a, int *b)
{
	int iTemp = *a;
	*a = *b;
	*b = iTemp;
}

