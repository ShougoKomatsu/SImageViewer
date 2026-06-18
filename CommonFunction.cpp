#include "stdafx.h"
#include "CommonFunction.h"
BEGIN_MESSAGE_MAP(CPictureCtrlEx, CStatic)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void CPictureCtrlEx::OnPaint()
{
    CPaintDC dc(this);

    CRect rc;
    GetClientRect(&rc);

    if (m_image.IsNull()==true){return;}

	double dZoom=min(rc.Width()/(m_image.GetWidth()*1.0),rc.Height()/(m_image.GetHeight()*1.0));
	
	CImage imgZoomed;
		ZoomImage(&m_image,&imgZoomed,
			0,
			0,
			dZoom,
			rc.Width(), rc.Height());

		
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