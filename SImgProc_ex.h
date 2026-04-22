#include "stdafx.h"


#pragma once
#ifdef __IMGPROC__
#undef DLL_IE
#define DLL_IE __declspec(dllexport)
#else
#define DLL_IE __declspec(dllimport)
#endif

#define CHANNEL_UNDEFINED (0)
#define CHANNEL_1_8 (1)
#define CHANNEL_3_8RGB (3)
#define CHANNEL_1_24BGR (24)
#define CHANNEL_3_FLOAT (103)


enum RECTANGLE_TYPE
{
	TYPE_UNDEFINED = 0,
	TYPE_RECTANGLE1 = 1
};

struct Rectangle
{
	int iR0;
	int iR1;
	int iC0;
	int iC1;
	int iType;
};


struct RunLength
{
	int iR;
	int iCStart;
	int iCEnd;
	UINT uiLabel;
	BOOL bValid;
	BOOL bIsConnectionOperated;
	void Set(const int iRIn, const int iCStartIn, const int iCEndIn, const UINT uiLabelIn, const BOOL bValidIn);
	void Copy(const RunLength* runLengthIn);
	RunLength();
};

struct DLL_IE Object
{
	UINT m_uiMaxLabel;
	int m_iMaxID;
	int m_iBufNum;
	RunLength* runLength;

	Object();
	BOOL Init();
	BOOL Alloc(const int iBuf);
	BOOL Expand(const int iBuf);
	BOOL Copy(const Object* objSrc);
	BOOL Add(const int iR, const int iCStart, const int iCEnd, const UINT uiLabel);
	BOOL IsNeighbor(const RunLength* runLength1, const RunLength* runLength2, const int iNeighborPolicy);
	BOOL ConnectNeighbor(const RunLength* runLength, const int iID, const int iNeighborPolicy);
	BOOL Connection(const int iNeighborPolicy);
	BOOL SortR();
	BOOL Truncate();
	BOOL ReCheckID();
	BOOL IsInRegion(const int dR, const int dC);
	BOOL IsRInRegion(const int iR);
	BOOL UnionOverwrappedRunlength();
	BOOL Union1();
	BOOL GetRunlengthIDsInR(const int iRIn, int* iIDStart, int* iIDEnd);
};



struct DLL_IE ImgRGB
{
	int iWidth;
	int iHeight;
	int iChannel;
	BYTE* byImgR;
	BYTE* byImgG;
	BYTE* byImgB;
	double* dImg1;
	double* dImg2;
	double* dImg3;
	BYTE* byImg;
	BOOL Set(const int iWidthIn, const int iHeightIn, const int iChannelIn);
	BOOL Init();
	ImgRGB();
	~ImgRGB();
	BOOL Assign(const CString sFilePath);
	BOOL Assign(const BYTE* byData, const int iWidth, const int iHeight);
	BOOL Assign(const ImgRGB* imgRGBIn);

	Object objDomain;
};


struct ImgRGBPyramid
{
	ImgRGB imgRGB;
	int iWidthOriginal;
	int iHeightOriginal;
	BOOL DLL_IE SetPyramid(const ImgRGB* imgRGBIn);
};

struct ImgMap
{

	int iWidth;
	int iHeight;
	UINT* uiMap;
	BOOL Set(int iWidthIn, int iHeightIn)
	{
		Init();
		if(iWidthIn<=0){return FALSE;}
		if(iHeightIn<=0){return FALSE;}

		if(iWidthIn*iHeightIn>(UINT_MAX>>8)){return FALSE;}

		iWidth = iWidthIn;
		iHeight = iHeightIn;
		uiMap = new UINT[iWidth*iHeight];
		memset(uiMap,0,iWidth*iHeight);
		return TRUE;
	}
	BOOL Init()
	{
		if(uiMap!=NULL){delete [] uiMap; uiMap=NULL;}
		iWidth=0;
		iHeight=0;
		return TRUE;
	}
	ImgMap(){uiMap=NULL;Init();}
	~ImgMap(){Init();}
};



BOOL DLL_IE WriteImage(const ImgRGB* imgRGB, CString sFilePath);
BOOL DLL_IE ReadBmpFromData(BOOL bHeader, BYTE* byData, ImgRGB* imgRGB);
BOOL DLL_IE ReadImage(const CString sFilePath, ImgRGB* imgRGB);
BOOL DLL_IE ConvertImage(const ImgRGB* imgIn, ImgRGB* imgOut, const CString sDstColor);
BOOL DLL_IE Decompose3(const ImgRGB* imgIn, ImgRGB* imgR, ImgRGB* imgG, ImgRGB* imgB);
BOOL DLL_IE Compose3(const ImgRGB* imgR, const ImgRGB* imgG, const ImgRGB* imgB, ImgRGB* imgOut);

BOOL DLL_IE Screenshot(ImgRGB* imgRGB);
BOOL DLL_IE CropImage(const ImgRGB* imgRGBin, ImgRGB* imgRGBout, const int iR0, const int iC0, const int iR1, const int iC1);

BOOL DLL_IE IsInRegion(const ImgRGB* imgTarget, const ImgRGB* imgModel, const int iR0, const int iC0, const int iR1, const int iC1, int* iFoundR, int* iFoundC);
BOOL DLL_IE IsInRegionMask(const ImgRGB* imgTarget, const ImgRGB* imgModel, const ImgRGB* imgMask, const int iR0, const int iC0, const int iR1, const int iC1, int* iFoundR, int* iFoundC);
BOOL DLL_IE FindModel(const ImgRGB* imgTarget, const ImgRGB* imgModel, const int iR0, const int iC0, const int iR1, const int iC1, int* iFoundR, int* iFoundC, const double dThreshPercent=0.0);
BOOL DLL_IE FindModelPyramid(const ImgRGB* imgTarget, const ImgRGB* imgModel, const int iR0, const int iC0, const int iR1, const int iC1, const double dThreshPercent, int* iFoundR, int* iFoundC);

BOOL DLL_IE GetValue(const ImgRGB* imgRGBin, const int iR, const int iC, int* iValueR, int* iValueG, int* iValueB);
BOOL DLL_IE GetValueInRegion(const ImgRGB* imgRGBin, const int iR0, const int iC0, const int iR1, const int iC1, double* dValueR, double* dValueG, double* dValueB);


BOOL DLL_IE SortRegion(const Object* objIn, const CString sMode, const CString sAscDsc, Object* objOut);
BOOL DLL_IE Threshold(const ImgRGB* imgIn, const BYTE byThreshMin, const BYTE byThreshMax, Object* ObjOut);
BOOL DLL_IE AreaCenter(const Object* obj, double* dArea, double* dR, double* dC);
BOOL DLL_IE AreaCenter(const Object* obj, double* dArea, double* dR, double* dC, const UINT uiLength);
BOOL DLL_IE SelectObj(const Object* objIn, const int iID, Object* objOut);
BOOL DLL_IE PaintRegion(const ImgRGB* imgIn, const Object* objIn, ImgRGB* imgOut, const BYTE byR, const BYTE byG, const BYTE byB);
BOOL DLL_IE PaintRegion(const ImgRGB* imgIn, Object* objIn, ImgRGB* imgOut);
BOOL DLL_IE SelectShape(const Object* objIn, Object* objOut, const CString sFeature, const double dMin, const double dMax);
BOOL DLL_IE ConcatObj(const Object* objIn1, const Object* objIn2, Object* objOut);

BOOL DLL_IE Connection(Object* objIn, Object* objOut, int iNeighborPolicy);
BOOL DLL_IE Union1(Object* objIn, Object* objOut);

BOOL DLL_IE ReduceDomain(ImgRGB* imgRGBIn, Object* objIn, ImgRGB* imgRGBOut);

BOOL DLL_IE GenRectangle1(Object* objOut, int iR0, int iC0, int iR1, int iC1);
BOOL DLL_IE SubImage(ImgRGB* img1, ImgRGB* img2, ImgRGB* imgResult, double dMult, double dAdd);

BOOL DLL_IE MeanImage(const ImgRGB* imgIn, ImgRGB* imgResult, const int iR0, const int iC0, const int iR1, const int iC1, const int iFilterWidth, const int iFilterHeight);
BOOL DLL_IE MaxImage(const ImgRGB* imgIn, ImgRGB* imgResult, const int iR0, const int C0, const int R1, const int C1, const int iFilterWidth, const int iFilterHeight);
BOOL DLL_IE MinImage(const ImgRGB* imgIn, ImgRGB* imgResult, const int iR0, const int iC0, const int iR1, const int iC1, const int iFilterWidth, const int iFilterHeight);
BOOL DLL_IE ScaleImageMax(const ImgRGB* imgIn, ImgRGB* imgResult, const int r0, const int c0, const int r1, const int c1);
BOOL DLL_IE EquHistImage(const ImgRGB* imgIn, ImgRGB* imgResult, const int r0, const int c0, const int r1, const int c1);
BOOL DLL_IE BrightnessContrast(const ImgRGB* imgIn, ImgRGB* imgResult, const int r0, const int c0, const int r1, const int c1, const double dBrightness, const double dContrastAngleDegree);
BOOL DLL_IE Gamma(const ImgRGB* imgIn, ImgRGB* imgResult, const int r0, const int c0, const int r1, const int c1, const double dGamma);

enum enumRotate
{
	ROTATE_NONE = 0,
	ROTATE_CW90 = 1,
	ROTATE_CW180 = 2,
	ROTATE_CW270 = 3,
	FLIP_UD = 4,
	FLIP_LR = 5
};
BOOL DLL_IE RotateImage(const ImgRGB* imgIn, ImgRGB* imgResult, const enumRotate);

struct DLL_IE Camera
{
	Camera(){}
	~Camera(){}
	int OpenCamera(CString sPipeName);
	int CloseCamera();
	int GrabImage(ImgRGB* imgOut);
	int SetParameter(CString sProp, CString sParam);
};
enum ENUM_COLOR
{
	COLOR_RED,
	COLOR_GREEN,
	COLOR_BLUE,
	COLOR_HUE,
	COLOR_HUE180,
	COLOR_SATUATION,
	COLOR_BRIGHTNESS,
	COLOR_RED_GRAY,
	COLOR_GREEN_GRAY,
	COLOR_BLUE_GRAY,
};
BOOL DLL_IE ConvertColorSpace(const ImgRGB* imgIn, ImgRGB* imgResult, const ENUM_COLOR color);