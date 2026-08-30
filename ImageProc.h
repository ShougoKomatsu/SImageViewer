
#include "stdafx.h"
#pragma once
#include "SImgProc_ex.h"


#define SAFE_DELETE(arg) if(arg != NULL){delete [] arg; arg=NULL;}

enum VALUE_IMAGE
{
	VALUE_IMAGE_UNDEFINED = -1,
	VALUE_IMAGE_RESCALE_0_TO_255 = 0,
	VALUE_IMAGE_CLIP_0_TO_255 = 1,
	VALUE_IMAGE_RAINBOW = 2,
};

enum IMAGE_TYPE
{
	IMAGE_TYPE_UNDEFINED=0,
	IMAGE_TYPE_CIMAGE=1,
	IMAGE_TYPE_IIMAGE=2,
	IMAGE_TYPE_DIMAGE=3,
};

enum VALUE_TYPE
{
	VALUE_TYPE_UNDEFINED=-1,
	VALUE_TYPE_INT=0,
	VALUE_TYPE_DOUBLE=1,
	VALUE_TYPE_BYTE=2,
	VALUE_TYPE_RGB=3, 
	VALUE_TYPE_RGBA=4,
};

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

struct ColorValue
{
	VALUE_TYPE valueType;
	BYTE byR;
	BYTE byG;
	BYTE byB;
	BYTE byA;
	BYTE byValue;
	int iValue;
	double dValue;

	void Init()
	{
		valueType=VALUE_TYPE_UNDEFINED;
		byR=0;
		byG=0;
		byB=0;
		byA=255;
		byValue=0;
		iValue=0;
		dValue=0;
	}
	ColorValue(){Init();}
};
#define MAX_IMG_BUF (32)
class PanImage
{
public:
	IMAGE_TYPE enumImageType;
	VALUE_IMAGE enumValueImage;
	CString sDataSource;
	CImage cImage;

	CImage m_imageProcessed[MAX_IMG_BUF];
	int m_iImgProcessIndex;
	int m_iUnDoAvailableCount;
	int m_iReDoAvailableCount;

	int iWidth;
	int iHeight;
	int* iImage;
	double* dImage;

	bool Set(const IMAGE_TYPE enumImageType, const int* iImage_in, const double* dImage_in, const int iWidth, const int iHeight, const CImage* cImage_in, const VALUE_IMAGE enumValueImage_in, const CString sDataSource_in);
	int GetWidth(){if(enumImageType==IMAGE_TYPE_CIMAGE){return cImage.GetWidth();} return iWidth;}
	int GetHeight(){if(enumImageType==IMAGE_TYPE_CIMAGE){return cImage.GetHeight();}return iHeight;}
	~PanImage()
	{
		Init();
	}
	PanImage()
	{
		iImage=NULL;
		dImage=NULL;
		Init();
	}
	void Init()
	{
		if(cImage.IsNull()!=true){cImage.Destroy();};
		SAFE_DELETE(iImage);
		SAFE_DELETE(dImage);
		iWidth=0;
		iHeight=0;
		sDataSource.Format(_T(""));
		enumImageType=IMAGE_TYPE_UNDEFINED;
		enumValueImage=VALUE_IMAGE_UNDEFINED;
		m_iImgProcessIndex=0;
		m_iUnDoAvailableCount=0;
		m_iReDoAvailableCount=0;
	}
//	bool GetColor(int r, int c, ColorValue* colorValue);
	bool Convert(const VALUE_IMAGE enumMode, CImage* imgDst);
	bool GetValue(const int r, const int c, double* dValue) const;
	bool GetValue(const int r, const int c, int* iValue) const;
	bool GetValue(const int r, const int c, RGBQUAD* rgbValue, BYTE* byAlpha, int* iBPP) const;
};






bool CopyToClipBoardImg(const CImage* img);
bool CopyFromClipBoardImg(PanImage* img);

bool CopyImage(const PanImage* imgSrc, PanImage* imgDst);
bool CopyImage(const CImage* imgSrc, CImage* imgDst);
bool ClipImage(const CImage* imgOriginal, CImage* imgClipped, const int iR0, const int iC0, const int iR1, const int iC1);

bool ZoomImage(const CImage* imgSrc, CImage* imgDst, const double dR0_Src, const double dC0_Src, const double dScale, const int iWidth_Dst, const int iHeight_Dst, const bool bRGBSeparated);

bool ExtractChannel(const CImage* imgSrc, CImage* imgDst, const ENUM_COLOR enumColor);

bool IsImageMonochrome(const CImage* imgSrc);
bool CountColorNum(const CImage* imgSrc, int* iColorNum_out, UINT* uiMap_out);
bool MakeColorTable(const CImage* cImage, RGBQUAD* rgbqTable_out, ULONGLONG* ullFrequency_out, int iLength, int* iUsedColors_out, bool* bGrayScale_out);

bool Resize(const CImage* imgSrc, CImage* imgDst, const int iWidth_dst, const int iHeight_dst, const RESAMPLE resample);
bool Resample(const CImage* imgSrc, CImage* imgDst, const RESAMPLE resample);
bool MakeReservedChannelZero(const CImage* imgSrc, CImage* imgDst);
bool SetColorTable(CImage* img, const RGBQUAD* rgbTable, int iLength);

bool ConvertImage(const ImgRGB* imgRGB, CImage* imgDst, const int iBPPDst, const RGBQUAD* rgbqTable, const int iColors);
bool ConvertStrToPanImage(const CString sImage, const VALUE_IMAGE enumImageMode, const CString sDataSource, PanImage* imgDst);
bool ConvertImage_AreaCoverage(const CImage* imgSrc,const int iBPP, CImage* imgDst);
bool ConvertImage_ByDeviation(const CImage* imgSrc,const int iBPP,  CImage* imgDst);
bool ConvertImage_LossLess(const CImage* imgSrc, const int iBPP, CImage* imgDst);
bool ConvertImageToStr(const CImage* cimage, const CString sSeparator, CString* sImage);
bool ConvertImage(const ImgRGB* imgRGB, CImage* cimage);	
bool ConvertImage(const CImage* cimage, ImgRGB* imgRGB);


UINT CountIconNum(const CString sFilePath);
bool LoadICOFile(const CString sFilePath, PanImage* img, UINT uiNum);
bool LoadICON2(const CString sFilePath, PanImage* imgs, UINT uiNum);

bool ImposeGrid(const CImage* imgValueSrc, const CImage* imgSrc, CImage* imgDst, const int iType, const double dROffset, const double dCOffset, const double dScale, const double dScaleThresh,const int iRMax_i, const int iCMax_i);
bool ImposeRGBValue(const PanImage* imgSrc, const CImage* imgZoomed, CImage* imgDst, const int iType,const double dROffset, const double dCOffset, const double dScale, const double dScaleThresh,const int iRs_i, const int iCs_i, const int iRe_i, const int iCe_i);
bool ImposeRect(const CImage* imgSrc, CImage* imgDst, const CRect* rect);
bool ImposeAlphaChannel(const CImage* imgSrc, CImage* imgDst);
