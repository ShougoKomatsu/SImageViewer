#include "stdafx.h"
#pragma once 

#include "ViewDraw.h"


class CPictureCtrlEx : public CStatic
{
public:
	ViewDraw view;
	PanImage m_image;
	int iID;
	
	void SetBGColor(const BYTE byR, const BYTE byG, const BYTE byB){view.SetBGColor(byR, byG, byB);}
	double GetDispOriginR_tv(){return view.GetDispOriginR_tv();}
	double GetDispOriginC_tv(){return view.GetDispOriginC_tv();}

	void Refresh();
	CPictureCtrlEx()
	{
	}

protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
