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

bool CopyToClipBoardStr(const CString sValue);