#include "stdafx.h"
#pragma once 

#include "PictureCtrlEx.h"

BEGIN_MESSAGE_MAP(CPictureCtrlEx, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


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
static int iIDLast=0;
void CPictureCtrlEx::OnPaint()
{
//	if(iID==iIDLast){return;}
//	iIDLast=iID;
	CPaintDC dc(this);
	CDC* pDC = this->GetDC();

	CRect rc;
	GetClientRect(&rc);

	if (m_image.GetCurrentProcess()->IsNull()==true){return;}

	double dZoom=min(rc.Width()/(m_image.GetWidth()*1.0),rc.Height()/(m_image.GetHeight()*1.0));

	CImage imgZoomed;


	view.ZoomChange(0, 0, m_image.GetHeight()-1, m_image.GetWidth()-1, false, &m_image, this, false);
	view.GetScrollSetting(&(m_image.scr));
	view.OnDraw(this, pDC, &m_image, false);
}