
#include "stdafx.h"
#pragma once
#include "SImgProc_ex.h"
#include "CommonFunction.h"
#include "PanImage.h"
#include "math.h"

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


bool ConvertImage(const ImgRGB* imgRGB, CImage* imgDst, const int iBPPDst, const RGBQUAD* rgbqTable, const int iColors);
bool ConvertImage_AreaCoverage(const CImage* imgSrc,const int iBPP, CImage* imgDst);
bool ConvertImage_ByDeviation(const CImage* imgSrc,const int iBPP,  CImage* imgDst);
bool ConvertImage_LossLess(const CImage* imgSrc, const int iBPP, CImage* imgDst);
bool ConvertImageToStr(const CImage* cimage, const CString sSeparator, CString* sImage);
bool ConvertImage(const ImgRGB* imgRGB, CImage* cimage);	


bool CopyToClipBoardImg(const CImage* img);

bool CopyImage_CImage(const CImage* imgSrc, CImage* imgDst);
bool ClipImage(const CImage* imgOriginal, CImage* imgClipped, const int iR0, const int iC0, const int iR1, const int iC1);

bool ZoomImage(const CImage* imgSrc, CImage* imgDst, const double dR0_Src, const double dC0_Src, const double dScale, const int iWidth_Dst, const int iHeight_Dst, const bool bRGBSeparated, const BYTE byBG_R, const BYTE byBG_G, const BYTE byBG_B, const bool bCentered);

bool ExtractChannel(const CImage* imgSrc, CImage* imgDst, const ENUM_COLOR enumColor);

bool CountColorNum(const CImage* imgSrc, int* iColorNum_out, UINT* uiMap_out);
bool MakeColorTable(const CImage* cImage, RGBQUAD* rgbqTable_out, ULONGLONG* ullFrequency_out, int iLength, int* iUsedColors_out, bool* bGrayScale_out);

bool Resize(const CImage* imgSrc, const int iR0_src, const int iC0_src, const int iR1_src, const int iC1_src, CImage* imgDst, const int iWidth_dst, const int iHeight_dst, const RESAMPLE resample);
bool Resample(const CImage* imgSrc, CImage* imgDst, const RESAMPLE resample);
void Resample(const ImgRGB* imgRGBSrc, const int iR0, const int iC0, const int iR1, const int iC1, const int iResamplePitch, ImgRGB* imgRGBDst);

bool MakeReservedChannelZero(const CImage* imgSrc, CImage* imgDst);
bool SetColorTable(CImage* img, const RGBQUAD* rgbTable, int iLength);



bool ImposeGrid(const CImage* imgValueSrc, const CImage* imgSrc, CImage* imgDst, const int iType, const double dROffset, const double dCOffset, const double dScale, const double dScaleThresh,const int iRMax_i, const int iCMax_i);
bool ImposeRect(const CImage* imgSrc, CImage* imgDst, const CRect* rect);
bool ImposeAlphaChannel(const CImage* imgSrc, CImage* imgDst);


bool GetImageTypeNum(const CString sIniFilePath, UINT* uiTypeNum);
bool GetImageType(const CString sIniFilePath, const int iIndexB0, CString* sType);

//		bool Threshold(const CImage* imgSrc, CImage* imgDst, const BYTE byMin, const BYTE byMax, const BYTE byR, const BYTE byG, const BYTE byB);

enum COLOR_ELEMENT
{
	COLOR_R=0,
	COLOR_G=1,
	COLOR_B=2,
};
bool Threshold(const CImage* image, CImage* imageColorized, const BYTE byMin, const BYTE byMax, const bool bConnection, const int iNeighbor );
bool Demosaic(const CImage* imgSrc,const COLOR_ELEMENT i00,const COLOR_ELEMENT i01,const COLOR_ELEMENT i10,const COLOR_ELEMENT i11, CImage* imgDst);


bool GrayToRainbow(const CImage* imgSrc, CImage* imgDst);

void GenHSImage(CImage* imgDst, const int iWidth, const int iHeight, const double dHue0to360Min, const double dHue0to360Max, const double dSaturation0to1Min, const double dSaturation0to1Max, const double dBrightness0to1);
void GenHVImage(CImage* imgDst, const int iWidth, const int iHeight, const double dHue0to360Min, const double dHue0to360Max,  const double dSaturation0to1, const double dBrightness0to1Min, const double dBrightness0to1Max);
void GenSVImage(CImage* imgDst, const int iWidth, const int iHeight, const double dHue0to360, const double dSaturation0to1Min, const double dSaturation0to1Max, const double dBrightness0to1Min, const double dBrightness0to1Max);
void GenRGImage(CImage* imgDst, const int iWidth, const int iHeight, const BYTE byRMin, const BYTE byRMax, const BYTE byGMin, const BYTE byGMax, const BYTE byB);
void GenGBImage(CImage* imgDst, const int iWidth, const int iHeight, const BYTE byR, const BYTE byGMin, const BYTE byGMax, const BYTE byBMin, const BYTE byBMax);
void GenBRImage(CImage* imgDst, const int iWidth, const int iHeight, const BYTE byRMin, const BYTE byRMax, const BYTE byG, const BYTE byBMin, const BYTE byBMax);


bool ImposeLine(const CImage* imgSrc, CImage* imgDst, const double dR0, const double dC0, const double dR1, const double dC1);
bool ImposeImage(const CImage* imgSrcBase, const CImage* imgSrcImposed, const int iROffset, const int iCOffset, CImage* imgDst);
bool ImposeImage(const CImage* imgSrcBase, const ImgRGB* imgSrcImposed, const int iROffset, const int iCOffset, CImage* imgDst);

bool AnalyzeStrAsImage(const CString sImage, int* iWidth, int* iHeight, CString* sSeparator_out, bool* bColored, bool* bFloat);
bool ConvertStrToByte3(const CString sImage, const CString sSeparator, const int iHeight, const int iWidth, BYTE* byImageR, BYTE* byImageG, BYTE* byImageB);
bool ConvertStrToInt(const CString sImage, const CString sSeparator, const int iHeight, const int iWidth, int* iImage);



extern const BYTE g_byFont_8_16[1992];
extern const BYTE g_byFont_4_8[96];

inline void SetRGBAValue(BYTE* pbyData, const int r, const int c, const int iPitch, const BYTE byR, const BYTE byG, const BYTE byB, const BYTE byA)
{
	pbyData[r*iPitch+4*c+2]=byR;
	pbyData[r*iPitch+4*c+1]=byG;
	pbyData[r*iPitch+4*c+0]=byB;
	pbyData[r*iPitch+4*c+3]=byA;
}

inline void SetRGBValue(BYTE* pbyData, const int r, const int c, const int iPitch, const int iColorPitch, const BYTE byR, const BYTE byG, const BYTE byB)
{
	pbyData[r*iPitch+iColorPitch*c+2]=byR;
	pbyData[r*iPitch+iColorPitch*c+1]=byG;
	pbyData[r*iPitch+iColorPitch*c+0]=byB;
}

inline bool HSVValue(BYTE* pbyData, const int iPitch, const int iColorPitch, const int r, const int c, const UINT uiValue_in, const int uiMax)
{
	int uiValue = uiValue_in;

	if(uiValue<uiMax)
	{
		SetRGBValue(pbyData, r, c, iPitch, iColorPitch, uiMax-1, uiValue-0, 0);
		return true;
	}
	uiValue-=uiMax;
	if(uiValue<uiMax)
	{
		SetRGBValue(pbyData, r, c, iPitch, iColorPitch, uiMax-1-uiValue, uiMax-1, 0);
		return true;
	}
	uiValue-=uiMax;
	if(uiValue<uiMax)
	{
		SetRGBValue(pbyData, r, c, iPitch, iColorPitch, 0, uiMax-1, uiValue);
		return true;
	}

	uiValue-=uiMax;
	if(uiValue<uiMax)
	{
		SetRGBValue(pbyData, r, c, iPitch, iColorPitch, 0, uiMax-1-uiValue, uiMax-1);
		return true;
	}
	uiValue-=uiMax;
	if(uiValue<uiMax)
	{
		SetRGBValue(pbyData, r, c, iPitch, iColorPitch, uiValue, 0, uiMax-1);
		return true;
	}
	uiValue-=uiMax;
	if(uiValue<uiMax)
	{
		SetRGBValue(pbyData, r, c, iPitch, iColorPitch, uiMax-1, 0, uiMax-1-uiValue);
		return true;
	}
	return false;
}

inline bool SetHSVValue(BYTE* pbyData, const int iPitch, const int iColorPitch, const int r, const int c, const UINT uiValue, const int iDigit)
{
	bool bRet;
	UINT uiValue_local=uiValue;
	for (int i=1;i<=256/iDigit; i++)
	{
		bRet = HSVValue(pbyData, iPitch, iColorPitch, r, c, uiValue_local, iDigit*i);
		if(bRet == true){break;}
		uiValue_local-=iDigit*i*6;
	}
	return true;
}


inline void SetRGBTransparent(BYTE* pbyData, const int r, const int c, const int iPitch, const BYTE byR, const BYTE byG, const BYTE byB)
{
	pbyData[r*iPitch+4*c+2]=(int(byR)*3+pbyData[r*iPitch+4*c+2])/4;
	pbyData[r*iPitch+4*c+1]=(int(byG)*3+pbyData[r*iPitch+4*c+1])/4;
	pbyData[r*iPitch+4*c+0]=(int(byB)*3+pbyData[r*iPitch+4*c+0])/4;
}


	inline void ImposeSingleValue_8_16(BYTE* pbyData, const int iPitch, const int iHeight, const int iWidth, const BYTE byDigitValue, const int ir_Origin, const int ic_Origin, const BYTE byDot)
	{
		for(int ir=0; ir<16; ir++)
		{
			if(ir_Origin+ir>=iHeight){continue;}
			if(ir_Origin+ir<0){continue;}
			for(int icc=0; icc<8; icc++)
			{
				if(ic_Origin+icc>=iWidth){continue;}
				if(ic_Origin+icc<0){continue;}

				if(byDot<128){SetRGBTransparent(pbyData, ir_Origin+ir, ic_Origin+icc, iPitch, 255, 255, 255);}
				else{SetRGBTransparent(pbyData, ir_Origin+ir, ic_Origin+icc, iPitch, 0, 0, 0);}

				if( (g_byFont_8_16[byDigitValue*16 + ir] & (1<<(7-icc))) == 1<<(7-icc))
				{
					SetRGBAValue(pbyData, ir_Origin+ir, ic_Origin+icc, iPitch, byDot, byDot, byDot, 255);
				}
			}
		}
	}

	inline void ImposeSingleValue_4_8(BYTE* pbyData, const int iPitch, const int iHeight, const int iWidth, const BYTE byDigitValue,const int ir_Origin, int ic_Origin, BYTE byDot)
	{

		for(int ir=0; ir<8; ir++)
		{
			if(ir_Origin+ir>=iHeight){continue;}
			if(ir_Origin+ir<0){continue;}
			for(int icc=0; icc<5; icc++)
			{
				if(ic_Origin+icc>=iWidth){continue;}
				if(ic_Origin+icc<0){continue;}

				if(byDot<128){SetRGBTransparent(pbyData, ir_Origin+ir, ic_Origin+icc, iPitch, 255, 255, 255);}
				else{SetRGBTransparent(pbyData, ir_Origin+ir, ic_Origin+icc, iPitch, 0, 0, 0);}}
		}

		for(int ir=0; ir<8; ir++)
		{
			if(ir_Origin+ir>=iHeight){continue;}
			if(ir_Origin+ir<0){continue;}
			for(int icc=0; icc<4; icc++)
			{
				if(ic_Origin+icc>=iWidth){continue;}
				if(ic_Origin+icc<0){continue;}
				
				if( (g_byFont_4_8[byDigitValue*8 + ir] & (1<<(3-icc))) == 1<<(3-icc))
				{
					SetRGBAValue(pbyData, ir_Origin+ir, ic_Origin+icc, iPitch, byDot, byDot, byDot, 255);
				}
			}
		}
	}


	inline void ImposeValue_8_16(BYTE* pbyData, const int iPitch, const int iHeight, const int iWidth, const int iValue_in, const int ir_Origin, const int ic_Origin_0, const BYTE byDot)
	{
		UINT uiValue=iValue_in;
		int iDigit = CountDigit(iValue_in);
		if(iDigit<0)
		{
			ImposeSingleValue_8_16(pbyData, iPitch, iHeight, iWidth, 10, ir_Origin, ic_Origin_0 - 8*iDigit*(-1), byDot);
			iDigit*=-1;
			uiValue = iValue_in*(-1);
		}

		for(int i=iDigit-1; i>=0; i--)
		{
			BYTE byDigitValue = uiValue/(int(pow(10,(double)i)));
			ImposeSingleValue_8_16(pbyData, iPitch, iHeight, iWidth, byDigitValue,ir_Origin, ic_Origin_0 - 8*i, byDot);
			uiValue-=byDigitValue*(int(pow(10,(double)i)));
		}

	}

	inline void ImposeValue_4_8(BYTE* pbyData, const int iPitch, const int iHeight, const int iWidth, const int iValue_in, const int ir_Origin, const int ic_Origin_0, const BYTE byDot)
	{
		UINT uiValue=iValue_in;
		int iDigit = CountDigit(iValue_in);
		if(iDigit<0)
		{
			ImposeSingleValue_4_8(pbyData, iPitch, iHeight, iWidth, 10, ir_Origin, ic_Origin_0 - 4*iDigit*(-1), byDot);
			iDigit*=-1;
			uiValue = iValue_in*(-1);
		}

		for(int i=iDigit-1; i>=0; i--)
		{
			BYTE byDigitValue = uiValue/(int(pow(10,(double)i)));
			ImposeSingleValue_4_8(pbyData, iPitch, iHeight, iWidth, byDigitValue,ir_Origin, ic_Origin_0 - 4*i, byDot);
			uiValue-=byDigitValue*(int(pow(10,(double)i)));
		}
	}
