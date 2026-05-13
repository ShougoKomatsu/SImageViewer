
#include "stdafx.h"
#pragma once
#include "SImgProc_ex.h"
	BOOL CopyToClipBoardImg(CImage* img);
	bool ClipImage( CImage* src, CImage* dst, int iR0, int iC0, int iR1, int iC1);
	BOOL ConvertImage(const ImgRGB* imgRGB, CImage* cimage);	

	BOOL ConvertImage(const CImage* cimage, ImgRGB* imgRGB);
bool CreateZoomedImage(CImage* imgOriginal, CImage* imgZoomed, const int iZoomFactor,  const int iCenterR, const int iCenterC);
//bool CopyImage(CImage* imgSrc, CImage* imgDst);
BOOL ZoomImage(CImage* imgSrc, CImage* imgDst, const double dR0_Src, const double dC0_Src, const double dScale, const int iWidth_Dst, const int iHeight_Dst);
BOOL CopyFromClipBoardImg(CImage* img);
bool ConvertImageToStr(const CImage* cimage, const CString sSeparater, CString* sImage);

bool ExtractChannel(CImage* imgSrc, CImage* imgDst, ENUM_COLOR enumColor);
bool CopyImage(const CImage* imgSrc, CImage* imgDst);

bool IsImageMonochrome(const CImage* imgSrc);
bool ConvertImageBPP1(const CImage* imgSrc, CImage* imgDst, bool bLosslessOnly);
bool ConvertImageBPP8(const CImage* imgSrc, CImage* imgDst, bool bLosslessOnly);
bool ConvertImageBPP24(const CImage* imgSrc, CImage* imgDst);
bool ConvertImageBPP32(const CImage* imgSrc, CImage* imgDst);
bool index_i(const ULONGLONG* arr, const int iLength, int* indx);
bool ConvertImageBPP8_AreaCoverage(const CImage* imgSrc, CImage* imgDst);
bool ConvertImageBPP8_ByDeviation(const CImage* imgSrc, CImage* imgDst);