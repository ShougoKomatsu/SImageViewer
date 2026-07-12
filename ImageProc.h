
#include "stdafx.h"
#pragma once
#include "SImgProc_ex.h"

enum RESAMPLE
{
	RESIZE_NEAREST=-100,
	RESIZE_BILINEAR=-99,
	RESAMPLE_4TH=-4,
	RESAMPLE_3RD=-3,
	RESAMPLE_2ND=-2,
	RESAMPLE_NONE=1,
	RESAMPLE_2=2,
	RESAMPLE_3=3,
	RESAMPLE_4=4,
};

bool CopyToClipBoardImg(const CImage* img);
bool CopyFromClipBoardImg(CImage* img);

bool ClipImage(const CImage* imgOriginal, CImage* imgClipped, const int iR0, const int iC0, const int iR1, const int iC1);
bool ConvertImage(const ImgRGB* imgRGB, CImage* cimage);	

bool ConvertImage(const CImage* cimage, ImgRGB* imgRGB);
bool ZoomImage(CImage* imgSrc, CImage* imgDst, const double dR0_Src, const double dC0_Src, const double dScale, const int iWidth_Dst, const int iHeight_Dst);
bool ConvertImageToStr(const CImage* cimage, const CString sSeparator, CString* sImage);

bool ExtractChannel(CImage* imgSrc, CImage* imgDst, ENUM_COLOR enumColor);
bool CopyImage(const CImage* imgSrc, CImage* imgDst);

bool IsImageMonochrome(const CImage* imgSrc);
bool index_i(const ULONGLONG* arr, const int iLength, int* indx);
bool ConvertImage_AreaCoverage(const CImage* imgSrc,const int iBPP, CImage* imgDst);
bool ConvertImage_ByDeviation(const CImage* imgSrc,const int iBPP,  CImage* imgDst);
bool ConvertImage_LossLess(const CImage* imgSrc, const int iBPP, CImage* imgDst);
bool CountColorNum(const CImage* imgSrc, int* iColorNum_out, UINT* uiMap_out);
bool MakeColorTable(const CImage* cImage, RGBQUAD* rgbqTable_out, ULONGLONG* ullFrequency_out, int iLength, int* iUsedColors_out, bool* bGrayScale_out);

bool SetColorTable(CImage* img, const RGBQUAD* rgbTable, int iLength);
bool Resample(const CImage* imgSrc, CImage* imgDst, const RESAMPLE resample);
bool ConvertImage(const ImgRGB* imgRGB, CImage* imgDst, const int iBPPDst, const RGBQUAD* rgbqTable, const int iColors);
bool Resize(const CImage* imgSrc, CImage* imgDst, const int iWidth_dst, const int iHeight_dst, const RESAMPLE resample);
bool ImposeGrid(CImage* imgSrc, CImage* imgDst, const int iType, const double dROffset, const double dCOffset, const double dScale, const double dScaleThresh,const int iRMax_i, const int iCMax_i);

bool ImposeRect(CImage* imgSrc, CImage* imgDst, CRect* rect);
bool ImposeAlphaChannel(CImage* imgSrc, CImage* imgDst);
bool MakeReservedChannelZero(CImage* imgSrc, CImage* imgDst);

UINT CountIconNum(const CString sFilePath);
bool LoadICOFile(const CString sFilePath, CImage* img, UINT uiNum);
bool LoadICON2(const CString sFilePath, CImage* imgs, UINT uiNum);