#include "stdafx.h"
#pragma once
#include "ImageProc.h"
class CPictureCtrlEx : public CStatic
{
public:
    CImage m_image;

protected:
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

#define SAFE_DELETE(arg) if(arg != NULL){delete [] arg; arg=NULL;}
bool CopyToClipBoardStr(const CString sValue);