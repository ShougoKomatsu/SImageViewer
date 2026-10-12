#include "stdafx.h"
#pragma once
#include "CommonFunction.h"
#include "SImgProc_ex.h"
#include "ImageProc.h"

#define MAX_IMG_PROCESS (32)

static BYTE s_byVisibleR[256]={15, 0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127, 129, 131, 133, 135, 137, 139, 141, 143, 145, 147, 149, 151, 153, 155, 157, 159, 161, 163, 165, 167, 169, 171, 173, 175, 177, 179, 181, 183, 185, 187, 189, 191, 193, 195, 197, 199, 201, 203, 205, 207, 209, 211, 213, 215, 217, 219, 221, 223, 225, 227, 229, 231, 233, 235, 237, 239, 241, 243, 245, 247, 249, 251, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 249, 241, 233, 229, 220, 211, 201, 191, 179, 173, 160, 145, 128, 107, 79, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 24, 42, 54, 62, 69, 74, 79, 81, 84, 87, 90, 92, 94, 95, 97, 98, 99, 100, 101, 102, 103, 103, 104, 104, 105, 103, 100, 98, 95, 92, 90, 87, 84, 82, 79, 77, 74, 71, 69, 66, 63, 61, 58, 55, 53, 50, 47, 45, 42, 40, 37, 34, 32, 29, 26};
static BYTE s_byVisibleG[256]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 49, 66, 79, 85, 96, 106, 116, 125, 134, 138, 146, 154, 162, 169, 176, 183, 186, 193, 199, 205, 211, 216, 219, 225, 230, 235, 240, 245, 247, 251, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 246, 234, 228, 215, 201, 186, 172, 158, 143, 136, 121, 107, 92, 77, 61, 51, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static BYTE s_byVisibleB[256]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 49, 77, 98, 116, 124, 138, 152, 164, 175, 185, 190, 198, 206, 212, 218, 223, 226, 228, 231, 233, 235, 237, 238, 238, 239, 240, 240, 240, 240, 240, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 241, 235, 229, 223, 217, 211, 205, 199, 193, 187, 181, 175, 169, 163, 157, 151, 144, 138, 132, 126, 120, 114, 108, 102, 96, 90, 84, 78, 72, 66, 60};


enum IMAGE_TYPE
{
	IMAGE_TYPE_UNDEFINED=0,
	IMAGE_TYPE_CIMAGE=1,
	IMAGE_TYPE_IIMAGE=2,
	IMAGE_TYPE_DIMAGE=3,
};
enum VALUE_IMAGE
{
	VALUE_IMAGE_UNDEFINED = -1,
	VALUE_IMAGE_RESCALE_0_TO_255 = 0,
	VALUE_IMAGE_CLIP_0_TO_255 = 1,
	VALUE_IMAGE_HUE_CYCLIC = 2,
	VALUE_IMAGE_RAINBOW = 3,
	VALUE_IMAGE_EQUALIZE= 4,
};

struct ScrollSetting
{
	int m_iScaleIndex;
	double m_dDispOriginR_tv;
	double m_dDispOriginC_tv;
	ScrollSetting()
	{
		m_iScaleIndex=8;
		m_dDispOriginR_tv=0;
		m_dDispOriginC_tv=0;
	}
};


bool _IsImageMonochrome(const CImage* imgSrc);

bool _ConvertImage(const CImage* cimage, ImgRGB* imgRGB);


class PanImage
{
public:
	ScrollSetting scr;
	int GetUnDoAvailableCount(){return m_iUnDoAvailableCount;}
	int GetReDoAvailableCount(){return m_iReDoAvailableCount;}

	CImage* ProgressImageProcess();
	const CImage* GetCurrentProcess() const;
	bool UnDo();
	bool ReDo();
	bool CopyImage(const PanImage* panImgSrc);
	void ResetProcessImage();
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
		ResetProcessImage();
	}
	//	bool GetColor(int r, int c, ColorValue* colorValue);
	bool Convert(const VALUE_IMAGE enumMode, CImage* imgDst);
	bool GetValue(const int r, const int c, double* dValue) const;
	bool GetValue(const int r, const int c, int* iValue) const;
	bool GetValue(const int r, const int c, RGBQUAD* rgbValue, BYTE* byAlpha, int* iBPP) const;
	int GetProcessIndex(){return m_iImgProcessIndex;}
	const int* GetIImage() const {return iImage;}
	const double* GetDImage() const {return dImage;}
	const CImage* GetCImage() const {return &cImage;}
	int* SetIImage(){return iImage;}
	double* SetDImage(){return dImage;}
	CImage* SetCImage(){return &cImage;}
	const IMAGE_TYPE GetImageType() const{return enumImageType;}
	void SetImageType(const IMAGE_TYPE enumImageType_in){enumImageType = enumImageType_in;}
	void SetDataSource(const CString sDataSource_in){sDataSource.Format(_T("%s"), sDataSource_in);}
	const CString GetDataSource() const{return sDataSource;}
	bool IsImageMonochrome(){return _IsImageMonochrome(&cImage);}
	bool ConvertImage(ImgRGB* imgRGB){return _ConvertImage(&cImage, imgRGB);}
private:
	CString sDataSource;
	VALUE_IMAGE enumValueImage;
	IMAGE_TYPE enumImageType;
	int* iImage;
	double* dImage;
	CImage cImage;
	int iWidth;
	int iHeight;
	CImage m_imageProcessed[MAX_IMG_PROCESS];
	int m_iImgProcessIndex;
	int m_iUnDoAvailableCount;
	int m_iReDoAvailableCount;
};




bool ConvertStrToPanImage(const CString sImage, const VALUE_IMAGE enumImageMode, const CString sDataSource, PanImage* panImgDst);
bool CopyFromClipBoardImg(PanImage* panImg);
bool ImposeRGBValue(const PanImage* panImgSrc, const CImage* imgZoomed, CImage* imgDst, const int iType,const double dROffset, const double dCOffset, const double dScale, const double dScaleThresh,const int iRs_i, const int iCs_i, const int iRe_i, const int iCe_i);

