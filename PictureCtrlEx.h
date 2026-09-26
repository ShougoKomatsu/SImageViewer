#include "stdafx.h"
#pragma once 

#include "ViewDraw.h"


class CPictureCtrlEx : public CStatic
{
public:
	ViewDraw view;
	PanImage m_image;
	int iID;

	double GetDispOriginR_tv(){return view.GetDispOriginR_tv();}
	double GetDispOriginC_tv(){return view.GetDispOriginC_tv();}

	void Refresh(){OnPaint();Invalidate();}
	CPictureCtrlEx()
	{
	}

protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
