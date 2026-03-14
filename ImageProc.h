#include "stdafx.h"
#include "SImgProc_ex.h"
	BOOL CopyToClipBoardImg(CImage* img);
	bool ClipImage( CImage* src, CImage* dst, int iR0, int iC0, int iR1, int iC1);
	BOOL ConvertImage(ImgRGB* imgRGB, CImage* cimage);	

	BOOL ConvertImage(CImage* cimage, ImgRGB* imgRGB);
bool CreateZoomedImage(CImage* imgOriginal, CImage* imgZoomed, const int iZoomFactor,  const int iCenterR, const int iCenterC);
//bool CopyImage(CImage* imgSrc, CImage* imgDst);
BOOL ZoomImage(CImage* imgSrc, CImage* imgDst, const double iR0_Src, const double iC0_Src, const double dScale, const int iWidth_Dst, const int iHeight_Dst);
BOOL CopyFromClipBoardImg(CImage* img);
bool ConvertImageToStr(CImage* cimage, CString sSeparater, CString* sImage);
bool CopyToClipBoardStr(const CString sValue);