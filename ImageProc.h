#include "stdafx.h"
#include "SImgProc_ex.h"
	BOOL CopyToClipBoardImg(CImage* img);
	bool ClipImage(const CImage* src, CImage* dst, int iR0, int iC0, int iR1, int iC1);
	BOOL ConvertImage(ImgRGB* imgRGB, CImage* cimage);	

	BOOL ConvertImage(CImage* cimage, ImgRGB* imgRGB);