#include "stdafx.h"
#include "ImageProc.h"
#include "SImgProc_ex.h"
#include "CommonFunction.h"
#include "math.h"


inline void SetRGBAValue(BYTE* pbyData, const int r, const int c, const int iPitch, const BYTE byR, const BYTE byG, const BYTE byB, const BYTE byA)
{
	pbyData[r*iPitch+4*c+2]=byR;
	pbyData[r*iPitch+4*c+1]=byG;
	pbyData[r*iPitch+4*c+0]=byB;
	pbyData[r*iPitch+4*c+3]=byA;
}
inline void SetRGBValue(BYTE* pbyData, const int r, const int c, const int iPitch, const BYTE byR, const BYTE byG, const BYTE byB)
{
	pbyData[r*iPitch+3*c+2]=byR;
	pbyData[r*iPitch+3*c+1]=byG;
	pbyData[r*iPitch+3*c+0]=byB;
}


bool SetColorTable(CImage* imgDst, const RGBQUAD* rgbTable, const int iLength)
{
	if(imgDst==NULL){return false;}
	if(rgbTable==NULL){return false;}
	if(iLength<0){return false;}
	if(iLength>256){return false;}
	if(imgDst->IsNull()==true){return false;}

	HDC hDC = imgDst->GetDC();
	SetDIBColorTable(hDC, 0, iLength, rgbTable);
	imgDst->ReleaseDC();
	return true;
}

bool IsColorTableMonochrome(const RGBQUAD* rgbTable, const int iLength)
{
	for(int i=0; i< iLength; i++)
	{
		if(rgbTable[i].rgbBlue != rgbTable[i].rgbGreen){return false;} 
		if(rgbTable[i].rgbBlue != rgbTable[i].rgbRed){return false;}
	}
	return true;
}

bool IsThereAlphaValue(const RGBQUAD* rgbTable, const int iLength)
{
	for(int i=0; i< iLength; i++)
	{
		if((rgbTable[i].rgbReserved != 255) && (rgbTable[i].rgbReserved != 0))
		{
			return true;
		}
	}
	return false;
}

bool IsImageMonochrome(const CImage* imgSrc)
{
	int iBPP = imgSrc->GetBPP();

	switch(iBPP)
	{
	case 1:
	case 4:
	case 8:
		{
			int iColors = imgSrc->GetMaxColorTableEntries();
			RGBQUAD* rgbqTable_src= new RGBQUAD[iColors];
			imgSrc->GetColorTable(0, iColors, rgbqTable_src);
			bool bMono = IsColorTableMonochrome(rgbqTable_src, iColors);
			SAFE_DELETE(rgbqTable_src);
			return bMono;
		}
	case 24:
	case 32:
		{
			int iColorPitch = (iBPP==24 ? 3 : 4);
			BYTE* pbyDataSrc = (BYTE*)imgSrc->GetBits();
			int iWidth = imgSrc->GetWidth();
			int iHeight= imgSrc->GetHeight();
			int iPitch = imgSrc->GetPitch();
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c< iWidth; c++)
				{
					if(pbyDataSrc[r*iPitch+c*iColorPitch+0] != pbyDataSrc[r*iPitch+c*iColorPitch+1]){return false;}
					if(pbyDataSrc[r*iPitch+c*iColorPitch+0] != pbyDataSrc[r*iPitch+c*iColorPitch+2]){return false;}
				}
			}
			return true;
		}
	}
	return false;
}

bool IsAlphaChanneled(const CImage* imgSrc)
{
	int iBPP = imgSrc->GetBPP();

	switch(iBPP)
	{
	case 1:
	case 4:
	case 8:
		{
			int iColors = imgSrc->GetMaxColorTableEntries();
			RGBQUAD* rgbqTable_src= new RGBQUAD[iColors];
			imgSrc->GetColorTable(0, iColors, rgbqTable_src);
			bool Alpha = IsThereAlphaValue(rgbqTable_src, iColors);
			SAFE_DELETE(rgbqTable_src);
			return Alpha;
		}
	case 24:
		{
			return false;
		}
	case 32:
		{
			BYTE* pbyDataSrc = (BYTE*)imgSrc->GetBits();
			int iWidth = imgSrc->GetWidth();
			int iHeight= imgSrc->GetHeight();
			int iPitch = imgSrc->GetPitch();
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c< iWidth; c++)
				{
					if(pbyDataSrc[r*iPitch+c*4+3] != 255){return true;}
				}
			}
			return false;
		}
	}
	return false;
}

CImage* PanImage::ProgressImageProcess()
{
	m_iImgProcessIndex++;
	m_iUnDoAvailableCount++;
	if(m_iUnDoAvailableCount>=MAX_IMG_PROCESS-1){m_iUnDoAvailableCount=MAX_IMG_PROCESS-1;}
	return &(m_imageProcessed[(m_iImgProcessIndex % MAX_IMG_PROCESS)]);
}
bool PanImage::UnDo()
{
	if(m_iUnDoAvailableCount<=0){return false;}
	m_iImgProcessIndex--;
	m_iUnDoAvailableCount--;
	m_iReDoAvailableCount++;
	return true;
}

bool PanImage::ReDo()
{
	if(m_iReDoAvailableCount<=0){return false;}
	m_iImgProcessIndex++;
	m_iReDoAvailableCount--;
	m_iUnDoAvailableCount++;
	return true;

}
CImage* PanImage::GetCurrentProcess()
{
	return &(m_imageProcessed[(m_iImgProcessIndex % MAX_IMG_PROCESS)]);
}


bool CopyImage(const PanImage* imgSrc, PanImage* imgDst)
{
	bool bRet = imgDst->Set(imgSrc->enumImageType, imgSrc->iImage, imgSrc->dImage, imgSrc->iWidth, imgSrc->iHeight, &imgSrc->cImage, imgSrc->enumValueImage, imgSrc->sDataSource);
	if(bRet != true){return false;}
	if(imgSrc->enumImageType != IMAGE_TYPE_CIMAGE)
	{
		bRet = CopyImage(&(imgSrc->cImage), &(imgDst->cImage));
		if(bRet != true){return false;}
	}
	for(int i=0; i<MAX_IMG_PROCESS; i++)
	{
		if(imgSrc->m_imageProcessed[i].IsNull() != true)
		{
			CopyImage(&(imgSrc->m_imageProcessed[i]), &(imgDst->m_imageProcessed[i]));
		}

		imgDst->m_iImgProcessIndex = imgSrc->m_iImgProcessIndex;
		imgDst->m_iUnDoAvailableCount = imgSrc->m_iUnDoAvailableCount;
		imgDst->m_iReDoAvailableCount = imgSrc->m_iReDoAvailableCount;

	}
	return true;
}
bool CopyImage(const CImage* imgSrc, CImage* imgDst)
{
	if (imgSrc->IsNull() == true) {return false;}

	int iWidth = imgSrc->GetWidth();
	int iHeight = imgSrc->GetHeight();


	if (imgDst->IsNull() != true) {imgDst->Destroy();}
	HRESULT hr = imgDst->Create(imgSrc->GetWidth(),imgSrc->GetHeight(),imgSrc->GetBPP());
	if (FAILED(hr)) {return false;}
	BYTE* pbyDataSrc = (BYTE*)imgSrc->GetBits();
	BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();

	int iBPP = imgSrc->GetBPP();

	if((iBPP==1)||(iBPP==2)||(iBPP==4)||(iBPP==8))
	{
		int iColors = imgSrc->GetMaxColorTableEntries();
		RGBQUAD* rgbqTable_src= new RGBQUAD[iColors];
		imgSrc->GetColorTable(0, iColors, rgbqTable_src);

		SetColorTable(imgDst, rgbqTable_src, iColors);

		SAFE_DELETE(rgbqTable_src);
	}

	int iPitch_src = imgSrc->GetPitch();
	int iPitch_dst = imgDst->GetPitch();
	int iLineLength = abs(iPitch_src);
	for(int r=0; r< imgSrc->GetHeight(); r++)
	{
		for(int c=0; c< iLineLength; c++)
		{
			pbyDataDst[r*iPitch_dst+c] = pbyDataSrc[r*iPitch_src+c];
		}
	}
	return true;
}

inline bool HSVValue(BYTE* pbyData, const int iPitch, const int r, const int c, const UINT uiValue_in, const int uiMax)
{
	int uiValue = uiValue_in;

	if(uiValue<uiMax)
	{
		SetRGBValue(pbyData, r, c, iPitch, uiMax-1, uiValue-0, 0);
		return true;
	}
	uiValue-=uiMax;
	if(uiValue<uiMax)
	{
		SetRGBValue(pbyData, r, c, iPitch, uiMax-1-uiValue, uiMax-1, 0);
		return true;
	}
	uiValue-=uiMax;
	if(uiValue<uiMax)
	{
		SetRGBValue(pbyData, r, c, iPitch, 0, uiMax-1, uiValue);
		return true;
	}

	uiValue-=uiMax;
	if(uiValue<uiMax)
	{
		SetRGBValue(pbyData, r, c, iPitch, 0, uiMax-1-uiValue, uiMax-1);
		return true;
	}
	uiValue-=uiMax;
	if(uiValue<uiMax)
	{
		SetRGBValue(pbyData, r, c, iPitch, uiValue, 0, uiMax-1);
		return true;
	}
	uiValue-=uiMax;
	if(uiValue<uiMax)
	{
		SetRGBValue(pbyData, r, c, iPitch, uiMax-1, 0, uiMax-1-uiValue);
		return true;
	}
	return false;
}
inline bool SetHSVValue(BYTE* pbyData, const int iPitch, const int r, const int c, const UINT uiValue, const int iDigit)
{
	bool bRet;
	UINT uiValue_local=uiValue;
	for (int i=1;i<=256/iDigit; i++)
	{
		bRet = HSVValue(pbyData, iPitch, r, c, uiValue, iDigit*i);
		if(bRet == true){break;}
		uiValue_local-=iDigit*i*6;
	}
	return true;
}

bool AnalyzeStrAsImage(const CString sImage, int* iWidth, int* iHeight, CString* sSeparator_out, bool* bColored, bool* bFloat)
{
	int iPlace =sImage.Find(_T("."),0);
	if(iPlace>=0){*bFloat=true;}
	CString sSeparator;
	iPlace = sImage.Find(_T("\t"));
	if(iPlace>=0){sSeparator.Format(_T("\t"));}
	else
	{
		iPlace = sImage.Find(_T(","));
		if(iPlace>=0){sSeparator.Format(_T(","));}
		else
		{
			return false;
		}
	}

	int iStart=0;
	int iLineCount=0;
	while(1)
	{
		int iFound =sImage.Find(_T("\r\n"),iStart);
		if(iFound<0){break;}
		iLineCount++;
		iStart=iFound+1;
	}

	bool bChecking= true;
	int iWidthPreserved=0;
	CStringArray* saLines;
	saLines = new CStringArray[iLineCount];
	iStart=0;
	for(int i=0; i<iLineCount; i++)
	{
		saLines[i].RemoveAll();
		int iPos=0;
		int iWidthR=0;
		int iWidthG=0;
		int iWidthB=0;

		while(1)
		{
			int iFound = sImage.Find(sSeparator,iStart);
			if(bChecking==true)
			{
				if(iFound == iStart)
				{
					if(iWidthR==0){iWidthR=iPos;}
					else
					{
						if(iWidthG==0){iWidthG=iPos-iWidthR;}
						else
						{
							if(iWidthB==0){iWidthB=iPos-iWidthG;}
						}
					}
				}
			}
			int iFoundLineEnd =sImage.Find(_T("\r\n"),iStart);
			if(iFoundLineEnd<iFound)
			{
				saLines[i].Add(sImage.Mid(iStart,iFoundLineEnd-iStart));
				iStart=iFoundLineEnd+1;
				break;
			}
			if(iFound<0){saLines[i].Add(sImage.Mid(iStart,sImage.GetLength()-iStart));break;}
			saLines[i].Add(sImage.Mid(iStart,iFound-iStart));
			iStart=iFound+1;
			iPos++;
		}
		if(bChecking==true)
		{
			if(iWidthR != 0)
			{
				if(iWidthR != iWidthG){iWidthPreserved=-1; bChecking = false;}
				if(iWidthR != iWidthB){iWidthPreserved=-1; bChecking = false;}
				if(iWidthR != iWidthPreserved){iWidthPreserved=-1; bChecking = false;}
				iWidthPreserved=iWidthR;
			}
			if(iWidthR != iWidthPreserved){iWidthPreserved=-1; bChecking = false;}
		}
	}

	sSeparator_out->Format(_T("%s"),sSeparator);
	*iHeight=iLineCount;
	if(iWidthPreserved>0)
	{
		*iWidth=iWidthPreserved;
		*bColored=true;
	}
	else
	{
		int iMaxLineLength=0;
		for(int i=0; i<iLineCount; i++)
		{
			iMaxLineLength = (int)max(iMaxLineLength,saLines[i].GetCount());
		}
		*iWidth=iMaxLineLength;
		*bColored=false;
	}

	SAFE_DELETE(saLines);
	return true;
}
bool ConvertStrToInt(const CString sImage, const CString sSeparator, const int iHeight, const int iWidth, int* iImage)
{
	CStringArray* saLines;
	saLines = new CStringArray[iHeight];
	int iStart=0;
	for(int i=0; i<iHeight; i++)
	{
		saLines[i].RemoveAll();
		while(1)
		{
			int iFound = sImage.Find(sSeparator,iStart);
			int iFoundLineEnd =sImage.Find(_T("\r\n"),iStart);
			if(iFoundLineEnd<iFound)
			{
				saLines[i].Add(sImage.Mid(iStart,iFoundLineEnd-iStart));
				iStart=iFoundLineEnd+1;
				break;
			}
			if(iFound<0){saLines[i].Add(sImage.Mid(iStart,sImage.GetLength()-iStart));break;}
			saLines[i].Add(sImage.Mid(iStart,iFound-iStart));
			iStart=iFound+1;
		}
	}

	for(int r=0; r<iHeight; r++)
	{
		for(int c=0; c<iWidth; c++)
		{
			iImage[r*iWidth+c]=0;
		}
	}

	for(int r=0; r<iHeight; r++)
	{
		for(int c=0; c<iWidth; c++)
		{
			iImage[r*iWidth+c]=_ttoi(saLines[r].GetAt(c));
		}
	}
	SAFE_DELETE(saLines);
	return true;
}

bool ConvertStrToByte3(const CString sImage, const CString sSeparator, const int iHeight, const int iWidth, BYTE* byImageR, BYTE* byImageG, BYTE* byImageB)
{
	CStringArray* saLines;
	saLines = new CStringArray[iHeight];
	int iStart=0;
	for(int i=0; i<iHeight; i++)
	{
		saLines[i].RemoveAll();
		while(1)
		{
			int iFound = sImage.Find(sSeparator,iStart);
			if(iFound==iStart){iStart=iFound+1;continue;}
			int iFoundLineEnd =sImage.Find(_T("\r\n"),iStart);
			if(iFoundLineEnd<iFound)
			{
				saLines[i].Add(sImage.Mid(iStart,iFoundLineEnd-iStart));
				iStart=iFoundLineEnd+1;
				break;
			}
			if(iFound<0){saLines[i].Add(sImage.Mid(iStart,sImage.GetLength()-iStart));break;}
			saLines[i].Add(sImage.Mid(iStart,iFound-iStart));
			iStart=iFound+1;
		}
	}

	for(int r=0; r<iHeight; r++)
	{
		for(int c=0; c<iWidth; c++)
		{
			if(c*3>=saLines[r].GetCount())
			{
				byImageR[r*iWidth+c]=0;
				byImageG[r*iWidth+c]=0;
				byImageB[r*iWidth+c]=0;
				continue;
			}
			byImageR[r*iWidth+c]=max(0, min(255, _ttoi(saLines[r].GetAt(c))));
			byImageG[r*iWidth+c]=max(0, min(255, _ttoi(saLines[r].GetAt(c+iWidth))));
			byImageB[r*iWidth+c]=max(0, min(255, _ttoi(saLines[r].GetAt(c+iWidth*2))));
		}
	}
	SAFE_DELETE(saLines);
	return true;
}



bool ConvertStrToPanImage(const CString sImage,  const VALUE_IMAGE enumImageMode, const CString sDataSource, PanImage* imgDst)
{
	CString sSeparator;
	int iHeight;
	int iWidth;
	bool bColored;
	bool bFloat;
	bool bRet = AnalyzeStrAsImage(sImage, &iWidth, &iHeight, &sSeparator, &bColored, &bFloat);
	if(bRet != true){return false;}
	if(bColored==true)
	{
		BYTE* byImageR;
		BYTE* byImageG;
		BYTE* byImageB;
		byImageR = new BYTE[iWidth*iHeight];
		byImageG = new BYTE[iWidth*iHeight];
		byImageB = new BYTE[iWidth*iHeight];
		bRet = ConvertStrToByte3(sImage, sSeparator, iHeight, iWidth, byImageR, byImageG, byImageB);
		CImage imgTemp;
		imgTemp.Create(iWidth, iHeight, 24);
		BYTE* pbyData=(BYTE*)imgTemp.GetBits();
		int iPitch = imgTemp.GetPitch();
		for(int r=0; r<iHeight; r++)
		{
			for(int c=0; c<iWidth; c++)
			{
				pbyData[r*iPitch+3*c+2]=byImageR[r*iWidth+c];
				pbyData[r*iPitch+3*c+1]=byImageG[r*iWidth+c];
				pbyData[r*iPitch+3*c+0]=byImageB[r*iWidth+c];
			}
		}
		SAFE_DELETE(byImageR);
		SAFE_DELETE(byImageG);
		SAFE_DELETE(byImageB);
		imgDst->Set(IMAGE_TYPE_CIMAGE, NULL, NULL, iWidth, iHeight, &imgTemp, VALUE_IMAGE_CLIP_0_TO_255, sDataSource);
		return true;
	}

	if(bFloat == true)
	{
	}
	else
	{
		int* iImage;
		iImage = new int[iWidth*iHeight];
		bRet = ConvertStrToInt(sImage, sSeparator, iHeight, iWidth, iImage);
		if(bRet != true){SAFE_DELETE(iImage); return false;}

		imgDst->Set(IMAGE_TYPE_IIMAGE,iImage,NULL, iWidth,iHeight,NULL,enumImageMode, sDataSource);
		SAFE_DELETE(iImage);
		return true;
	}
	return false;
}


bool ConvertImageToStr(const CImage* imgSrc, const CString sSeparator, CString* sImage)
{
	int iWidth_src  = imgSrc->GetWidth();
	int iHeight_src = imgSrc->GetHeight();

	if(iWidth_src <= 0){return FALSE;}
	if(iHeight_src == 0){return FALSE;}

	ImgRGB imgRGB;
	ConvertImage(imgSrc,&imgRGB);
	bool bMono = IsImageMonochrome(imgSrc);
	bool bAlpha = IsAlphaChanneled(imgSrc);
	CString sImageLocal=_T("");
	if(bMono==false)
	{
		for(int r=0; r<iHeight_src; r++)
		{
			CString sLine=_T("");;
			CString sPixel;
			for(int c=0; c<iWidth_src; c++)
			{
				sPixel.Format(_T("%d%s"),imgRGB.byImgR[r*iWidth_src+c],sSeparator);
				sLine+=sPixel;
			}
			sLine+=sSeparator;
			for(int c=0; c<iWidth_src; c++)
			{
				sPixel.Format(_T("%d%s"),imgRGB.byImgG[r*iWidth_src+c],sSeparator);
				sLine+=sPixel;
			}
			sLine+=sSeparator;
			for(int c=0; c<iWidth_src-1; c++)
			{
				sPixel.Format(_T("%d%s"),imgRGB.byImgB[r*iWidth_src+c],sSeparator);
				sLine+=sPixel;
			}
			if(bAlpha!=true)
			{
				sPixel.Format(_T("%d\n"),imgRGB.byImgB[r*iWidth_src+(iWidth_src-1)]);
				sLine+=sPixel;
			}
			else
			{
				sPixel.Format(_T("%d%s"),imgRGB.byImgB[r*iWidth_src+(iWidth_src-1)], sSeparator);
				sLine+=sPixel;
				sLine+=sSeparator;
				for(int c=0; c<iWidth_src-1; c++)
				{
					sPixel.Format(_T("%d%s"),imgRGB.byImgA[r*iWidth_src+c],sSeparator);
					sLine+=sPixel;
				}
				sPixel.Format(_T("%d\n"),imgRGB.byImgA[r*iWidth_src+(iWidth_src-1)]);
				sLine+=sPixel;
			}
			sImageLocal+=sLine;
		}
		(*sImage)=sImageLocal;
		return true;
	}

	for(int r=0; r<iHeight_src; r++)
	{
		CString sLine=_T("");;
		CString sPixel;

		for(int c=0; c<iWidth_src-1; c++)
		{
			sPixel.Format(_T("%d%s"),imgRGB.byImgR[r*iWidth_src+c],sSeparator);
			sLine+=sPixel;
		}
		if(bAlpha!=true)
		{
			sPixel.Format(_T("%d\n"),imgRGB.byImgR[r*iWidth_src+(iWidth_src-1)]);
			sLine+=sPixel;
		}
		else
		{
			sPixel.Format(_T("%d%s"),imgRGB.byImgR[r*iWidth_src+(iWidth_src-1)],sSeparator);
			sLine+=sPixel;
			sLine+=sSeparator;
			for(int c=0; c<iWidth_src-1; c++)
			{
				sPixel.Format(_T("%d%s"),imgRGB.byImgA[r*iWidth_src+c],sSeparator);
				sLine+=sPixel;
			}
			sPixel.Format(_T("%d\n"),imgRGB.byImgA[r*iWidth_src+(iWidth_src-1)]);
			sLine+=sPixel;
		}

		sImageLocal+=sLine;
	}
	(*sImage)=sImageLocal;
	return true;
}

bool ClipImage(const CImage* imgOriginal, CImage* imgClipped, const int iR0, const int iC0, const int iR1, const int iC1)
{
	if (imgOriginal->IsNull() == true){return false;}

	int iImgWidth  = imgOriginal->GetWidth();
	int iImgHeight = imgOriginal->GetHeight();

	int iR0Local=max(0, min(iR0,iR1));
	int iC0Local=max(0, min(iC0,iC1));
	int iR1Local=min(iImgHeight-1, max(iR0,iR1));
	int iC1Local=min(iImgWidth-1, max(iC0,iC1));

	int iClipWidth  = iC1Local-iC0Local+1;
	int iClipHeight = iR1Local-iR0Local+1;

	if (iClipWidth<= 0){return false;}
	if (iClipHeight<= 0){return false;}

	int iBPP_src = imgOriginal->GetBPP();
	if (iBPP_src == 0) {return false;}

	if (imgClipped->IsNull() != true){imgClipped->Destroy();}
	HRESULT hr = imgClipped->Create(iClipWidth, iClipHeight, iBPP_src);
	if (FAILED(hr)) {return false;}

	if (iBPP_src <= 8) 
	{
		int iColors = imgOriginal->GetMaxColorTableEntries();
		if (iColors > 0) 
		{
			RGBQUAD* srcTable = new RGBQUAD[iColors];
			imgOriginal->GetColorTable(0, iColors, srcTable);

			SetColorTable(imgClipped, srcTable, iColors);
			SAFE_DELETE(srcTable);
		}
	}

	int iPitch_src = imgOriginal->GetPitch();
	int iPitch_dst = imgClipped->GetPitch();

	BYTE* pbyDataSrc = (BYTE*)imgOriginal->GetBits();
	BYTE* pbyDataDst = (BYTE*)imgClipped->GetBits();

	bool bSrcBottomUp = (iPitch_src > 0);
	bool bDstBottomUp = (iPitch_dst > 0);

	int iSrcByPP = iBPP_src / 8;

	for (int r = 0; r < iClipHeight; r++) 
	{
		BYTE* pSrcLine = nullptr;
		BYTE* pDstLine = nullptr;

		if (bSrcBottomUp == true) 
		{
			pSrcLine = &(pbyDataSrc[(iImgHeight - 1 - (iR0+r)) * iPitch_src + iC0 * iSrcByPP]);
		}
		else 
		{
			pSrcLine = &(pbyDataSrc[(iR0+r) * iPitch_src + iC0 * iSrcByPP]);
		}

		if (bDstBottomUp == true)
		{
			pDstLine = &(pbyDataDst[(iClipHeight - 1 - r) * iPitch_dst]);
		}
		else
		{
			pDstLine = &(pbyDataDst[r * iPitch_dst]);
		}

		memcpy(pDstLine, pSrcLine, iClipWidth * iSrcByPP);
	}
	return true;
}

bool CopyToClipBoardImg(const CImage* imgSrc)
{
	if (imgSrc->IsNull() == true){return false;}

	int iWidth_src = imgSrc->GetWidth();
	int iHeight_src = (imgSrc->GetHeight() < 0 ? -1*imgSrc->GetHeight() : imgSrc->GetHeight());
	int iBPP_src = imgSrc->GetBPP();

	int iHeaderSize = sizeof(BITMAPINFOHEADER);

	int iColors = 0;
	if (iBPP_src <= 8) 
	{
		iColors = imgSrc->GetMaxColorTableEntries();
	}

	int iPaletteSize = iColors * sizeof(RGBQUAD);
	int iBytesPerLine = ((iWidth_src * iBPP_src + 31) / 32) * 4;
	int iTotalSize = iHeaderSize + iPaletteSize + ( iBytesPerLine * iHeight_src);

	HGLOBAL hGL;
	BYTE* pbyDib ;

	hGL = GlobalAlloc(GPTR, iTotalSize);
	if (hGL == NULL){return false;}

	pbyDib = (BYTE*)GlobalLock(hGL);
	if (pbyDib == NULL)
	{
		GlobalFree(hGL);
		return false;
	}

	BITMAPINFOHEADER* bih = (BITMAPINFOHEADER*)pbyDib;
	bih->biSize          = sizeof(BITMAPINFOHEADER);
	bih->biWidth         = iWidth_src;
	bih->biHeight        = -1*iHeight_src;
	bih->biPlanes        = 1;
	bih->biBitCount      = (WORD)iBPP_src;
	bih->biCompression   = BI_RGB;
	bih->biSizeImage     = iBytesPerLine * iHeight_src;
	bih->biXPelsPerMeter = 0;
	bih->biYPelsPerMeter = 0;
	bih->biClrUsed       = ((iBPP_src <= 8) ? iColors : 0);
	bih->biClrImportant  = 0;

	BYTE* pPalette = pbyDib + iHeaderSize;
	BYTE* pBits    = pPalette + iPaletteSize;

	if ((iBPP_src <= 8) && (iColors > 0) )
	{
		RGBQUAD* rgbqTable = new RGBQUAD[iColors];
		imgSrc->GetColorTable(0, iColors, rgbqTable);

		memcpy(pPalette, rgbqTable, iPaletteSize);
		delete[] rgbqTable;
	}


	BYTE* pbyDataSrc = (BYTE*)imgSrc->GetBits();
	int iPitch_src   = imgSrc->GetPitch();
	bool bBottomUp  = (iPitch_src > 0);

	for (int r = 0; r < iHeight_src; r++) 
	{

		BYTE* pSrcLine = nullptr;
		BYTE* pDstLine = &(pBits[r * iBytesPerLine]);

		if (bBottomUp == true)
		{
			pSrcLine = &(pbyDataSrc[(iHeight_src - 1 -r) * iPitch_src]);
		} 
		else 
		{
			pSrcLine = &(pbyDataSrc[r * iPitch_src]);
		}

		memcpy(pDstLine, pSrcLine, iBytesPerLine);
	}


	BOOL bRet;
	bRet = GlobalUnlock(hGL);
	if(bRet == FALSE){GlobalFree(hGL);return false;}

	bRet = OpenClipboard(NULL);
	if(bRet == FALSE){GlobalFree(hGL);return false;}

	bRet = EmptyClipboard();
	if(bRet == FALSE){CloseClipboard();GlobalFree(hGL);return false;}

	HANDLE hResult;
	hResult = SetClipboardData(CF_DIB, hGL);
	if (hResult == NULL){CloseClipboard();GlobalFree(hGL);return false;}

	bRet = CloseClipboard();
	if(bRet != TRUE){return false;}

	return true;
}

bool ReadCImageFromData(const BYTE* byData, const SIZE_T dataSize, CImage* imgDst)
{

	BITMAPINFOHEADER* bih = (BITMAPINFOHEADER*)byData;

	int iWidth  = bih->biWidth;
	int iHeight = bih->biHeight;
	int iBPP_src= bih->biBitCount;

	if(iWidth <= 0){SAFE_DELETE(byData); return false;}
	if(iHeight == 0){SAFE_DELETE(byData); return false;}
	if(iBPP_src == 0){SAFE_DELETE(byData); return false;}

	int iColors = 0;
	if (iBPP_src <= 8) 
	{
		iColors = bih->biClrUsed;
		if (iColors == 0) 
		{
			iColors = 1 << min(24, iBPP_src);
		}
	}

	int iPaletteSize = iColors * sizeof(RGBQUAD);

	const BYTE* pPalette = &(byData[sizeof(BITMAPINFOHEADER)]);
	const BYTE* pbyDataSrc;
	int iBytesPerLine = ((iWidth * iBPP_src + 31) / 32) * 4;
	if(iBPP_src == 32)
	{
		if(dataSize == sizeof(BITMAPINFOHEADER) + iPaletteSize + abs(iHeight)*iBytesPerLine)
		{
			pbyDataSrc = &(byData[sizeof(BITMAPINFOHEADER)+iPaletteSize ]);
		}
		else
		{
			pbyDataSrc = &(byData[dataSize - iWidth*abs(iHeight)*iBPP_src/8 ]);
		}
	}
	else if(iBPP_src == 24)
	{
		if(dataSize == sizeof(BITMAPINFOHEADER) + iPaletteSize + abs(iHeight)*iBytesPerLine)
		{
			pbyDataSrc = &(byData[sizeof(BITMAPINFOHEADER)+iPaletteSize ]);
		}
		else
		{
			pbyDataSrc = &(byData[sizeof(BITMAPINFOHEADER)+iPaletteSize ]);
		}
	}
	else
	{
		pbyDataSrc = &(byData[sizeof(BITMAPINFOHEADER)+iPaletteSize ]);;
	}

	if(imgDst->IsNull()!=true){imgDst->Destroy();};
	HRESULT hr = imgDst->Create(iWidth, abs(iHeight), iBPP_src);
	if (FAILED(hr)) {SAFE_DELETE(byData); return false;}

	if ((iBPP_src <= 8) && (iColors > 0))
	{
		SetColorTable(imgDst,(RGBQUAD*)pPalette,iColors);
	}

	int iPitch_dst = imgDst->GetPitch();
	BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();

	bool bDstBottomUp = (iPitch_dst > 0);
	bool bSrcBottomUp = (iHeight > 0); 

	int iAbsHeight = abs(iHeight);

	for (int r = 0; r < iAbsHeight; r++)
	{
		const BYTE* pSrcLine;
		if (bSrcBottomUp) 
		{
			pSrcLine = &(pbyDataSrc[(iAbsHeight - 1 - r) * iBytesPerLine]);
		}
		else
		{
			pSrcLine = &(pbyDataSrc[r * iBytesPerLine]);
		}

		BYTE* pDstLine = nullptr;
		if (bDstBottomUp) 
		{
			pDstLine = &(pbyDataDst[(iAbsHeight - 1 - r) * iPitch_dst]);
		}
		else
		{
			pDstLine = &(pbyDataDst[r * iPitch_dst]);
		}

		memcpy(pDstLine, pSrcLine, iBytesPerLine);
	}
	return true;
}

bool CopyFromClipBoardImg(PanImage* imgDst)
{
	BOOL bRet;

	bRet = OpenClipboard(NULL);
	if(bRet == FALSE){return false;}

	HANDLE hResult;

	hResult = GetClipboardData(CF_UNICODETEXT );
	if(hResult != NULL)
	{
		LPVOID byDataTemp = GlobalLock(hResult);
		if(byDataTemp==NULL){CloseClipboard();return false;}

		SIZE_T dataSize = GlobalSize(hResult);
		if (dataSize == 0) { GlobalUnlock(hResult);CloseClipboard(); return false;} 

		BYTE* byData;
		byData = new BYTE[dataSize];

		memcpy(byData, byDataTemp, dataSize);

		GlobalUnlock(hResult);

		bRet = CloseClipboard();
		if(bRet == FALSE){SAFE_DELETE(byData); return false;}
		CString sData;
		sData.Format(_T("%s"),byData);
		SAFE_DELETE(byData); 
		
		return ConvertStrToPanImage(sData,VALUE_IMAGE_CLIP_0_TO_255, _T("Clipboard"), imgDst);
	}

	hResult = GetClipboardData(CF_DIB);
	if(hResult == NULL){CloseClipboard();return false;}

	LPVOID byDataTemp = GlobalLock(hResult);
	if(byDataTemp==NULL){CloseClipboard();return false;}

	SIZE_T dataSize = GlobalSize(hResult);
	if (dataSize == 0) { GlobalUnlock(hResult);CloseClipboard(); return false;} 

	BYTE* byData;
	byData = new BYTE[dataSize];

	memcpy(byData, byDataTemp, dataSize);

	GlobalUnlock(hResult);

	bRet = CloseClipboard();
	if(bRet == FALSE){SAFE_DELETE(byData); return false;}
	
	bRet = ReadCImageFromData(byData, dataSize, &imgDst->cImage);
	if(bRet == FALSE){SAFE_DELETE(byData); return false;}

	SAFE_DELETE(byData); 
	imgDst->enumImageType=IMAGE_TYPE_CIMAGE;
	imgDst->sDataSource.Format(_T("Clipboard"));
	return true;
}
int GetColorTableIndex(const RGBQUAD* rgbqTable, const int iLength, const BYTE byR, const BYTE byG, const BYTE byB, const BYTE byA)
{
	for(int i=0; i<iLength; i++)
	{
		if((rgbqTable[i].rgbRed==byR)&&(rgbqTable[i].rgbGreen==byG)&&(rgbqTable[i].rgbBlue==byB)&&(rgbqTable[i].rgbReserved==byA))
		{
			return i;
		}
	}
	return -1;
}


bool ConvertImage(const CImage* imgSrc, ImgRGB* imgRGB)
{
	int iWidth_src = imgSrc->GetWidth();
	int iHeight_src = imgSrc->GetHeight();
	imgRGB->Set(iWidth_src,iHeight_src,CHANNEL_4_8RGBA);

	int iBPP = imgSrc->GetBPP();
	BYTE* pbyDataSrc = (BYTE*)imgSrc->GetBits();
	int iPitch_src=imgSrc->GetPitch();

	if((iBPP==24) || (iBPP==32))
	{
		int iColorPitch = (iBPP==24 ? 3 : 4);
		for(int r=0; r<iHeight_src; r++)
		{
			for(int c=0; c<iWidth_src; c++)
			{
				imgRGB->byImgR[r*imgRGB->iWidth+c]=pbyDataSrc[r*iPitch_src+c*iColorPitch+2];
				imgRGB->byImgG[r*imgRGB->iWidth+c]=pbyDataSrc[r*iPitch_src+c*iColorPitch+1];
				imgRGB->byImgB[r*imgRGB->iWidth+c]=pbyDataSrc[r*iPitch_src+c*iColorPitch+0];
				imgRGB->byImgA[r*imgRGB->iWidth+c]=(iBPP==24 ? 255 : pbyDataSrc[r*iPitch_src+c*iColorPitch+3]);
			}
		}
		return true;
	}


	bool bAlpha = IsAlphaChanneled(imgSrc);

	RGBQUAD* srcTable=NULL;
	int iColors = imgSrc->GetMaxColorTableEntries();
	if (iColors > 0) 
	{
		srcTable = new RGBQUAD[iColors];
		imgSrc->GetColorTable(0, iColors, srcTable);
	}
	const BYTE byMasks[9]={0,1,3,0,15,0,0,0,255};
	int iWidth = imgRGB->iWidth;
	int iHeight = imgRGB->iHeight;

	for(int r=0; r<iHeight; r++)
	{
		for(int c=0; c<iWidth; c++)
		{
			int iPosition= r*iPitch_src+c/(8/iBPP);
			int iDigit = (8/iBPP)-(c%(8/iBPP))-1;

			BYTE byIndex = (pbyDataSrc[iPosition] & (byMasks[iBPP]<< (iDigit*iBPP))) >> (iDigit*iBPP);

			imgRGB->byImgR[r*imgRGB->iWidth+c]=srcTable[byIndex].rgbRed;
			imgRGB->byImgG[r*imgRGB->iWidth+c]=srcTable[byIndex].rgbGreen;
			imgRGB->byImgB[r*imgRGB->iWidth+c]=srcTable[byIndex].rgbBlue;
			imgRGB->byImgA[r*imgRGB->iWidth+c]=(bAlpha==true) ? srcTable[byIndex].rgbReserved : 255;

		}
	}
	SAFE_DELETE(srcTable);
	return true;
}
bool Resize(const CImage* imgSrc, CImage* imgDst, const int iWidth_dst, const int iHeight_dst, const RESAMPLE rsample)
{
	if(imgSrc->IsNull() == true){return false;}
	ImgRGB imgRGBSrc;
	ImgRGB imgRGBDst;
	int iBPP=imgSrc->GetBPP();
	ConvertImage(imgSrc, &imgRGBSrc);
	imgRGBDst.Set(iWidth_dst, iHeight_dst, CHANNEL_3_8RGB);
	int iWidth_src = imgSrc->GetWidth();
	int iHeight_src = imgSrc->GetHeight();

	switch(rsample)
	{
	case RESIZE_NEAREST:
		{
			for(int r=0; r<imgRGBDst.iHeight; r++)
			{
				for(int c=0; c<imgRGBDst.iWidth; c++)
				{
					double dR_src = r * (iHeight_src-1)/(double(iHeight_dst-1));
					double dC_src = c * (iWidth_src-1)/(double(iWidth_dst-1));
					int iPosDsc = r*imgRGBDst.iWidth+c;
					int iPosSrc = (int(dR_src+0.5))*imgRGBSrc.iWidth+(int(dC_src+0.5));
					imgRGBDst.byImgR[iPosDsc ]=imgRGBSrc.byImgR[iPosSrc];
					imgRGBDst.byImgG[iPosDsc ]=imgRGBSrc.byImgG[iPosSrc];
					imgRGBDst.byImgB[iPosDsc ]=imgRGBSrc.byImgB[iPosSrc];

				}
			}
			break;
		}

	case RESIZE_BILINEAR:
		{
			for(int r=0; r<imgRGBDst.iHeight; r++)
			{
				for(int c=0; c<imgRGBDst.iWidth; c++)
				{
					double dR_src = r * (iHeight_src-1)/(double(iHeight_dst-1));
					double dC_src = c * (iWidth_src-1)/(double(iWidth_dst-1));
					double dR_frac = dR_src-int(dR_src);
					double dC_frac = dC_src-int(dC_src);


					int iPosDsc = r*imgRGBDst.iWidth+c;
					int iPosSrc1 = (int(dR_src))*imgRGBSrc.iWidth+(int(dC_src));
					int iPosSrc2 = (int(dR_src))*imgRGBSrc.iWidth+(int(dC_src)+1);
					int iPosSrc3 = (int(dR_src)+1)*imgRGBSrc.iWidth+(int(dC_src));
					int iPosSrc4 = (int(dR_src)+1)*imgRGBSrc.iWidth+(int(dC_src)+1);

					double dValue1 = imgRGBSrc.byImgR[iPosSrc1] * (1-dC_frac) + imgRGBSrc.byImgR[iPosSrc2] * (dC_frac);
					double dValue2 = imgRGBSrc.byImgR[iPosSrc3] * (1-dC_frac) + imgRGBSrc.byImgR[iPosSrc4] * (dC_frac);
					double dValue = dValue1*(1-dR_frac)+dValue2*(dR_frac);
					imgRGBDst.byImgR[iPosDsc ]=(BYTE)dValue;

					dValue1 = imgRGBSrc.byImgG[iPosSrc1] * (1-dC_frac) + imgRGBSrc.byImgG[iPosSrc2] * (dC_frac);
					dValue2 = imgRGBSrc.byImgG[iPosSrc3] * (1-dC_frac) + imgRGBSrc.byImgG[iPosSrc4] * (dC_frac);
					dValue = dValue1*(1-dR_frac)+dValue2*(dR_frac);
					imgRGBDst.byImgG[iPosDsc ]=(BYTE)dValue;

					dValue1 = imgRGBSrc.byImgB[iPosSrc1] * (1-dC_frac) + imgRGBSrc.byImgB[iPosSrc2] * (dC_frac);
					dValue2 = imgRGBSrc.byImgB[iPosSrc3] * (1-dC_frac) + imgRGBSrc.byImgB[iPosSrc4] * (dC_frac);
					dValue = dValue1*(1-dR_frac)+dValue2*(dR_frac);
					imgRGBDst.byImgB[iPosDsc ]=(BYTE)dValue;
				}
			}
			break;
		}
	}
	ConvertImage(&imgRGBDst,imgDst);
	return true;
}
bool Resample(const CImage* imgSrc, CImage* imgDst, const RESAMPLE resample)
{
	if(imgSrc->IsNull() == true){return false;}
	ImgRGB imgRGBSrc;
	ImgRGB imgRGBDst;
	int iBPP=imgSrc->GetBPP();
	switch(resample)
	{
	case RESAMPLE_NONE:{return CopyImage(imgSrc, imgDst);}
	case RESAMPLE_2:
	case RESAMPLE_3:
	case RESAMPLE_4:
		{
			int iStep;
			switch (resample)
			{
			case RESAMPLE_2:{iStep=2;break;}
			case RESAMPLE_3:{iStep=3;break;}
			case RESAMPLE_4:{iStep=4;break;}
			default:{return false;}
			}
			ConvertImage(imgSrc, &imgRGBSrc);
			if((iBPP==24) || (iBPP==32))
			{
				if(iBPP==24){imgRGBDst.Set((imgRGBSrc.iWidth + (iStep-1))/iStep, (imgRGBSrc.iHeight+(iStep-1))/iStep,CHANNEL_3_8RGB);}
				if(iBPP==32){imgRGBDst.Set((imgRGBSrc.iWidth + (iStep-1))/iStep, (imgRGBSrc.iHeight+(iStep-1))/iStep,CHANNEL_4_8RGBA);}
				for(int r=0; r<imgRGBDst.iHeight; r++)
				{
					for(int c=0; c<imgRGBDst.iWidth; c++)
					{
						int iPosDsc = r*imgRGBDst.iWidth+c;
						int iPosSrc = (r*iStep)*imgRGBSrc.iWidth+(c*iStep);
						imgRGBDst.byImgR[iPosDsc ]=imgRGBSrc.byImgR[iPosSrc];
						imgRGBDst.byImgG[iPosDsc ]=imgRGBSrc.byImgG[iPosSrc];
						imgRGBDst.byImgB[iPosDsc ]=imgRGBSrc.byImgB[iPosSrc];
						if(iBPP==32){imgRGBDst.byImgA[iPosDsc ]=imgRGBSrc.byImgA[iPosSrc];}
					}
				}
				ConvertImage(&imgRGBDst,imgDst);
				return true;
			}

			imgRGBDst.Set((imgRGBSrc.iWidth + (iStep-1))/iStep, (imgRGBSrc.iHeight+(iStep-1))/iStep,CHANNEL_3_8RGB);
			int iColors = imgSrc->GetMaxColorTableEntries();
			if (iColors > 0) 
			{
				RGBQUAD* srcTable = new RGBQUAD[iColors];
				imgSrc->GetColorTable(0, iColors, srcTable);

				ConvertImage(&imgRGBDst,imgDst, iBPP, srcTable, iColors);
				SAFE_DELETE(srcTable);
				return true;
			}
			ConvertImage(&imgRGBDst,imgDst);
			return true;
		}
	case RESAMPLE_2ND:
	case RESAMPLE_3RD:
	case RESAMPLE_4TH:
		{
			int iStep;
			switch (resample)
			{
			case RESAMPLE_2ND:{iStep=2;break;}
			case RESAMPLE_3RD:{iStep=3;break;}
			case RESAMPLE_4TH:{iStep=4;break;}
			}
			ConvertImage(imgSrc, &imgRGBSrc);

			if((iBPP==24) || (iBPP==32))
			{
				if(iBPP==24){imgRGBDst.Set(imgRGBSrc.iWidth*iStep, imgRGBSrc.iHeight*iStep,CHANNEL_3_8RGB);}
				if(iBPP==32){imgRGBDst.Set(imgRGBSrc.iWidth*iStep, imgRGBSrc.iHeight*iStep,CHANNEL_4_8RGBA);}
				for(int r=0; r<imgRGBDst.iHeight; r++)
				{
					for(int c=0; c<imgRGBDst.iWidth; c++)
					{
						int iPosDsc = r*imgRGBDst.iWidth+c;
						int iPosSrc = (r/iStep)*imgRGBSrc.iWidth+(c/iStep);
						imgRGBDst.byImgR[iPosDsc ]=imgRGBSrc.byImgR[iPosSrc];
						imgRGBDst.byImgG[iPosDsc ]=imgRGBSrc.byImgG[iPosSrc];
						imgRGBDst.byImgB[iPosDsc ]=imgRGBSrc.byImgB[iPosSrc];
						if(iBPP==32){imgRGBDst.byImgA[iPosDsc ]=imgRGBSrc.byImgA[iPosSrc];}
					}
				}
				ConvertImage(&imgRGBDst,imgDst);
				return true;
			}

			imgRGBDst.Set(imgRGBSrc.iWidth*iStep, imgRGBSrc.iHeight*iStep,CHANNEL_3_8RGB);
			int iColors = imgSrc->GetMaxColorTableEntries();
			if (iColors > 0) 
			{
				RGBQUAD* srcTable = new RGBQUAD[iColors];
				imgSrc->GetColorTable(0, iColors, srcTable);

				ConvertImage(&imgRGBDst,imgDst, iBPP, srcTable, iColors);
				SAFE_DELETE(srcTable);
				return true;
			}
			ConvertImage(&imgRGBDst,imgDst);
			return true;
		}
	default:{return false;}
	}
	return false;
}

bool ConvertImage(const ImgRGB* imgRGB, CImage* imgDst)
{
	int iWidth_src = imgRGB->iWidth;;
	int iHeight_src= imgRGB->iHeight;;

	switch(imgRGB->iChannel)
	{
	case CHANNEL_3_8RGB:
	case CHANNEL_4_8RGBA:
		{
			if(imgDst->IsNull() != true){imgDst->Destroy();}
			if(imgRGB->iChannel==CHANNEL_3_8RGB){imgDst->Create(iWidth_src, iHeight_src, 24);}
			if(imgRGB->iChannel==CHANNEL_4_8RGBA){imgDst->Create(iWidth_src, iHeight_src, 32);}

			int iColorPitch = ((imgRGB->iChannel==CHANNEL_3_8RGB) ? 3 : 4);
			BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
			int iPitch_dst=imgDst->GetPitch();
			for(int r=0; r<iHeight_src; r++)
			{
				for(int c=0; c<iWidth_src; c++)
				{
					pbyDataDst[r*iPitch_dst+c*iColorPitch+2]=imgRGB->byImgR[r*iWidth_src+c];
					pbyDataDst[r*iPitch_dst+c*iColorPitch+1]=imgRGB->byImgG[r*iWidth_src+c];
					pbyDataDst[r*iPitch_dst+c*iColorPitch+0]=imgRGB->byImgB[r*iWidth_src+c];
					if(imgRGB->iChannel==CHANNEL_4_8RGBA){pbyDataDst[r*iPitch_dst+c*iColorPitch+3]=imgRGB->byImgA[r*iWidth_src+c];}
				}
			}
			return true;
		}
	case CHANNEL_1_8:
		{
			if(imgDst->IsNull() != true){imgDst->Destroy();}
			imgDst->Create(iWidth_src, iHeight_src, 8);

			RGBQUAD colorTable[256];
			for(int i=0; i<256; i++)
			{
				colorTable[i].rgbBlue=i;
				colorTable[i].rgbGreen=i;
				colorTable[i].rgbRed=i;
				colorTable[i].rgbReserved=0;
			}
			SetColorTable(imgDst, colorTable, 256);

			BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
			int iPitch_dst=imgDst->GetPitch();
			for(int r=0; r<iHeight_src; r++)
			{
				for(int c=0; c<iWidth_src; c++)
				{
					pbyDataDst[r*iPitch_dst+c]=imgRGB->byImg[r*iWidth_src+c];
				}
			}
			return true;
		}
	default : {return false;}
	}
	return true;
}

int GetNearestColor(const RGBQUAD* rgbqTable, const int iLength, const RGBQUAD rgbqTarget)
{
	int iNearestIndex=-1;
	UINT uiDelta=256*256*256+1;

	BYTE byR_Target=rgbqTarget.rgbRed;
	BYTE byG_Target=rgbqTarget.rgbGreen;
	BYTE byB_Target=rgbqTarget.rgbBlue;
	for(int i=0; i<iLength; i++)
	{
		BYTE byR_Table=rgbqTable[i].rgbRed;
		BYTE byG_Table=rgbqTable[i].rgbGreen;
		BYTE byB_Table=rgbqTable[i].rgbBlue;

		UINT uiTemp = (byR_Table-byR_Target)*(byR_Table-byR_Target)
			+(byG_Table-byG_Target)*(byG_Table-byG_Target)
			+(byB_Table-byB_Target)*(byB_Table-byB_Target);
		if(uiTemp <uiDelta)
		{
			uiDelta = uiTemp;
			iNearestIndex = i;
		}
	}
	return iNearestIndex;
}


bool GetColorConversionTable(const RGBQUAD* rgbqTable, const int* iPopularOrder, const int iLength, RGBQUAD* rgbqTable_Sorted, const int iMax, int* iConversionTable)
{
	if(iLength <= iMax)
	{
		for(int i=0; i<iLength; i++)
		{
			iConversionTable[i]=iPopularOrder[i];
			rgbqTable_Sorted[i]=rgbqTable[iPopularOrder[i]];
		}
		return true;
	}

	for(int i=0; i<iMax; i++)
	{
		rgbqTable_Sorted[i]=rgbqTable[iPopularOrder[iLength-1-i]];
	}

	for(int i=0; i<iLength; i++)
	{
		int iFound=0;
		for(int j=0; j<iLength; j++)
		{
			if(iPopularOrder[iLength-1-j]==i){iFound=j;break;}
		}
		if(iFound<iMax){iConversionTable[i]=iFound; continue;}
		int iNearest = GetNearestColor(rgbqTable_Sorted, iMax, rgbqTable[i]);
		iConversionTable[i]=iNearest;
	}

	return true;
}

inline void CopyRGBQ(const RGBQUAD* rgbq_src, RGBQUAD* rgbq_dst)
{
	rgbq_dst->rgbRed = rgbq_src->rgbRed;
	rgbq_dst->rgbGreen = rgbq_src->rgbGreen;
	rgbq_dst->rgbBlue = rgbq_src->rgbBlue;
	rgbq_dst->rgbReserved = rgbq_src->rgbReserved;
}

bool ConvertImage_LossLess(const CImage* imgSrc, const int iBPPDst, CImage* imgDst)
{
	if(imgSrc->GetBPP() == iBPPDst){return CopyImage(imgSrc,imgDst);}

	int iUsedColors;
	bool bGrayScale;
	MakeColorTable(imgSrc,NULL,NULL, 1<<min(24, imgSrc->GetBPP()), &iUsedColors, &bGrayScale);

	if(iUsedColors > (1<<min(24, iBPPDst))){return false;}

	ImgRGB imgRGB;
	ConvertImage(imgSrc,&imgRGB);

	if(imgDst->IsNull() != true){imgDst->Destroy();}
	imgDst->Create(imgRGB.iWidth, imgRGB.iHeight, iBPPDst);
	BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
	int iPitch_dst=imgDst->GetPitch();


	if((iBPPDst==24) || (iBPPDst==32))
	{
		int iColorPitch = ((iBPPDst==24) ? 3 : 4);

		for(int r=0; r< imgRGB.iHeight; r++)
		{
			for(int c=0; c<imgRGB.iWidth; c++)
			{
				pbyDataDst[r*iPitch_dst+c*3+2]=imgRGB.byImgR[r*imgRGB.iWidth+c];
				pbyDataDst[r*iPitch_dst+c*3+1]=imgRGB.byImgG[r*imgRGB.iWidth+c];
				pbyDataDst[r*iPitch_dst+c*3+0]=imgRGB.byImgB[r*imgRGB.iWidth+c];
				if(iBPPDst==32){pbyDataDst[r*iPitch_dst+c*iColorPitch+3]=255;}
			}
		}
		return true;
	}

	RGBQUAD* rgbqTable;
	RGBQUAD* rgbqTable_unsorted;
	ULONGLONG* ullFrequency;
	rgbqTable_unsorted = new RGBQUAD[1ull<<min(24, imgSrc->GetBPP())];
	rgbqTable = new RGBQUAD[1ull<<min(24, imgSrc->GetBPP())];
	ullFrequency = new ULONGLONG[min(24, 1<<imgSrc->GetBPP())];

	MakeColorTable(imgSrc, rgbqTable_unsorted, NULL, 1<<min(24, imgSrc->GetBPP()), &iUsedColors, &bGrayScale);
	const BYTE byMasks[9]={0,1,3,0,15,0,0,0,255};

	if(bGrayScale==true)
	{
		int* iIndex;
		iIndex=new int[iUsedColors];
		ULONGLONG* ullValues;
		ullValues=new ULONGLONG[iUsedColors];
		for(int i=0; i<iUsedColors; i++)
		{
			ullValues[i]=rgbqTable_unsorted[i].rgbRed;
		}
		index_i(ullValues,iUsedColors,iIndex);

		for(int i=0; i<iUsedColors; i++)
		{
			CopyRGBQ(&(rgbqTable_unsorted[iIndex[i]]), &(rgbqTable[i]));
		}
		SAFE_DELETE(iIndex);
		SAFE_DELETE(ullValues);
	}	
	else
	{
		int* iIndex;
		iIndex=new int[iUsedColors];
		index_i(ullFrequency,iUsedColors,iIndex);
		int* iConversionTable;
		iConversionTable=new int[iUsedColors];
		GetColorConversionTable(rgbqTable_unsorted, iIndex, iUsedColors, rgbqTable, iUsedColors, iConversionTable);
		SAFE_DELETE(iConversionTable);
		SAFE_DELETE(iIndex);
	}

	SetColorTable(imgDst, rgbqTable, 1<<min(24, iBPPDst));

	for(int i=iUsedColors; i<(1<<min(24, iBPPDst)); i++)
	{
		rgbqTable[i].rgbRed=255;
		rgbqTable[i].rgbGreen=255;
		rgbqTable[i].rgbBlue=255;
		rgbqTable[i].rgbReserved=0;
	}


	int iWidthUnioned = (imgRGB.iWidth + (8/iBPPDst-1))/(8/iBPPDst);
	for(int r=0; r< imgRGB.iHeight; r++)
	{
		for(int c=0; c<iWidthUnioned ; c++)
		{
			pbyDataDst[r*iPitch_dst+c]=0;
		}
	}

	for(int r=0; r< imgRGB.iHeight; r++)
	{
		for(int c=0; c<imgRGB.iWidth ; c++)
		{

			int iColorIndex = GetColorTableIndex(rgbqTable, 1<<min(24, iBPPDst), imgRGB.byImgR[r*imgRGB.iWidth+c], imgRGB.byImgG[r*imgRGB.iWidth+c], imgRGB.byImgB[r*imgRGB.iWidth+c], 0);

			int iPosition= r*iPitch_dst+c/(8/iBPPDst);
			int iDigit = (8/iBPPDst)-(c%(8/iBPPDst))-1;
			pbyDataDst[iPosition] += iColorIndex<<(iDigit*iBPPDst);
		}
	}
	SAFE_DELETE(ullFrequency);
	SAFE_DELETE(rgbqTable);
	SAFE_DELETE(rgbqTable_unsorted);
	return true;
}


bool ExtractChannel_Gray(const CImage* imgSrc, CImage* imgDst, const ENUM_COLOR color)
{
	int iWidth = imgSrc->GetWidth();
	int iHeight = imgSrc->GetHeight();
	if(imgSrc->GetBPP()==8)
	{
		CopyImage(imgSrc,imgDst);
		return true;
	}

	if(imgDst->IsNull() != true){imgDst->Destroy();}
	imgDst->Create(iWidth, iHeight, 8);
	RGBQUAD colorTable[256];
	for(int i=0; i<256; i++)
	{
		colorTable[i].rgbBlue=i;
		colorTable[i].rgbGreen=i;
		colorTable[i].rgbRed=i;
		colorTable[i].rgbReserved=0;
	}

	SetColorTable(imgDst, colorTable, 256);

	ImgRGB imgRGB;
	ConvertImage(imgSrc, &imgRGB);
	int iPitch_dst=imgDst->GetPitch();
	BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();

	BYTE* pbySrc;
	switch(color)
	{
	case COLOR_RED_GRAY: {pbySrc=imgRGB.byImgR; break;}
	case COLOR_GREEN_GRAY: {pbySrc=imgRGB.byImgG;break;}
	case COLOR_BLUE_GRAY: {pbySrc=imgRGB.byImgB;break;}
	default : {return false;}
	}

	for(int r=0; r<iHeight; r++)
	{
		for(int c=0; c<iWidth; c++)
		{
			pbyDataDst[r*iPitch_dst+c]=pbySrc[r*iWidth+c];
		}
	}
	return true;
}


bool ExtractChannel_Color(const CImage* imgSrc, CImage* imgDst, const ENUM_COLOR color)
{
	int iWidth = imgSrc->GetWidth();
	int iHeight = imgSrc->GetHeight();

	ImgRGB imgRGB;
	ConvertImage(imgSrc, &imgRGB);

	if(imgDst->IsNull() != true){imgDst->Destroy();}
	imgDst->Create(iWidth, iHeight, 24);
	int iPitch_dst=imgDst->GetPitch();
	BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
	switch(color)
	{
	case COLOR_RED: 
		{
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					pbyDataDst[r*iPitch_dst+c*3+2]=imgRGB.byImgR[r*iWidth+c];
					pbyDataDst[r*iPitch_dst+c*3+1]=0;
					pbyDataDst[r*iPitch_dst+c*3+0]=0;
				}
			}
			return true;
		}

	case COLOR_GREEN: 
		{
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					pbyDataDst[r*iPitch_dst+c*3+2]=0;
					pbyDataDst[r*iPitch_dst+c*3+1]=imgRGB.byImgG[r*iWidth+c];
					pbyDataDst[r*iPitch_dst+c*3+0]=0;
				}
			}
			return true;
		}
	case COLOR_BLUE: 
		{
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					pbyDataDst[r*iPitch_dst+c*3+2]=0;
					pbyDataDst[r*iPitch_dst+c*3+1]=0;
					pbyDataDst[r*iPitch_dst+c*3+0]=imgRGB.byImgB[r*iWidth+c];
				}
			}
			return true;
		}
	}

	return true;
}

bool ExtractChannel(const CImage* imgSrc, CImage* imgDst, const ENUM_COLOR color)
{
	if((color==COLOR_RED)||(color==COLOR_GREEN)||(color==COLOR_BLUE))
	{
		return ExtractChannel_Color(imgSrc, imgDst , color);
	}
	if((color==COLOR_RED_GRAY)||(color==COLOR_GREEN_GRAY)||(color==COLOR_BLUE_GRAY))
	{
		return ExtractChannel_Gray(imgSrc, imgDst , color);
	}

	ImgRGB imgSrcRGB;
	ImgRGB imgDstRGB;
	ConvertImage(imgSrc,&imgSrcRGB);
	ConvertColorSpace(&imgSrcRGB,&imgDstRGB,color);
	return ConvertImage(&imgDstRGB,imgDst);
}

bool ImposeRect(const CImage* imgSrc, CImage* imgDst, const CRect* rect)
{
	if(imgSrc != imgDst)
	{
		CopyImage(imgSrc,imgDst);
	}

	int iBPP = imgDst->GetBPP();
	if(iBPP != 32){return false;}
	int iPitch =imgDst->GetPitch();
	int iWidth = imgDst->GetWidth();
	int iHeight = imgDst->GetHeight();
	BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();

	BYTE byDot=0;
	for(int r=rect->top; r<=rect->bottom; r++)
	{
		if(r<0){continue;}
		if(r>=iHeight){continue;}
		byDot = (r&0x02)*255;

		int c=rect->left;
		if((c>0) && (c<iWidth))
		{
			SetRGBAValue(pbyDataDst, r, c, iPitch, byDot, byDot, byDot, 255);
		}

		if((c+1>0) && (c*1<iWidth))
		{
			SetRGBAValue(pbyDataDst, r, c+1, iPitch, byDot, byDot, byDot, 255);
		}

		c=rect->right;		
		if((c>0) && (c<iWidth))
		{
			SetRGBAValue(pbyDataDst, r, c, iPitch, byDot, byDot, byDot, 255);
		}
		if((c+1>0) && (c+1<iWidth))
		{
			SetRGBAValue(pbyDataDst, r, c+1, iPitch, byDot, byDot, byDot, 255);
		}
	}

	for(int c=rect->left; c<=rect->right; c++)
	{
		if(c<0){continue;}
		if(c>=iWidth){continue;}
		byDot = (c&0x02)*255;

		int r=rect->top;

		if((r>=0) && (r<iHeight))
		{
			SetRGBAValue(pbyDataDst, r, c, iPitch, byDot, byDot, byDot, 255);
		}

		if((r+1>=0) && (r+1<iHeight))
		{
			SetRGBAValue(pbyDataDst, r+1, c, iPitch, byDot, byDot, byDot, 255);
		}

		r=rect->bottom;

		if((r>=0) && (r<iHeight))
		{
			SetRGBAValue(pbyDataDst, r, c, iPitch, byDot, byDot, byDot, 255);
		}

		if((r+1>=0) && (r+1<iHeight))
		{
			SetRGBAValue(pbyDataDst, r+1, c, iPitch, byDot, byDot, byDot, 255);
		}
	}

	return true;
}

bool ImposeAlphaChannel(const CImage* imgSrc, CImage* imgDst)
{
	SYSTEMTIME st;
	GetSystemTime(&st);

	if(imgSrc->GetBPP() != 32)
	{
		return false;
	}

	if(imgDst->IsNull() != true){imgDst->Destroy();}

	int iWidth_Dst = imgSrc->GetWidth();
	int iHeight_Dst = imgSrc->GetHeight();

	imgDst->Create(iWidth_Dst, iHeight_Dst,24);


	BYTE* pByDataSrc = (BYTE*)imgSrc->GetBits();
	int iPitch_src=imgSrc->GetPitch();
	BYTE* pByDataDst = (BYTE*)imgDst->GetBits();
	int iPitch_dst=imgDst->GetPitch();

	int iDisplace=((st.wMinute*60000+ st.wSecond*1000 + st.wMilliseconds)/100)%16;

	for(int r=0; r<iHeight_Dst; r++)
	{
		for(int c=0; c<iWidth_Dst; c++)
		{
			BYTE byBack=(((((r+iDisplace)/16)+((c+iDisplace)/16))%2==1)?  255: 192);
			BYTE byAlpha = pByDataSrc[r*iPitch_src+c*4+3];
			if(byAlpha != 255)
			{
				byAlpha=byAlpha;
			}
			pByDataDst[r*iPitch_dst+c*3+2]=(BYTE)((pByDataSrc[r*iPitch_src+c*4+2]*(byAlpha) + byBack*(255.0-byAlpha))/255) ;
			pByDataDst[r*iPitch_dst+c*3+1]=(BYTE)((pByDataSrc[r*iPitch_src+c*4+1]*(byAlpha) + byBack*(255.0-byAlpha))/255) ;
			pByDataDst[r*iPitch_dst+c*3+0]=(BYTE)((pByDataSrc[r*iPitch_src+c*4+0]*(byAlpha) + byBack*(255.0-byAlpha))/255) ;

		}
	}
	return false;
}
/*
bool ImposeValue(CImage* imgSrc, CImage* imgDst,  const int iType,const double dROffset, const double dCOffset, const double dScale, const double dScaleThresh,const int iRMax_i, const int iCMax_i)
{
if(imgSrc != imgDst)
{
CopyImage(imgSrc,imgDst);
}
//if(dScale<dScaleThresh){return true;}
//if(iType==0){return true;}

BYTE* pbyData = (BYTE*)imgDst->GetBits();
int iBPP = imgDst->GetBPP();
if(iBPP != 32){return false;}
int iPitch =imgDst->GetPitch();
int iWidth = imgDst->GetWidth();
int iHeight = imgDst->GetHeight();
int iRMax = int(iHeight/dScale+2);
int iCMax = int(iWidth/dScale+2);

CImage imgTemp;
imgTemp.Create(1024,1024,24);
int iHeightTemp = imgTemp.GetHeight();
int iWidthTemp = imgTemp.GetWidth();
HDC  hDC = imgTemp.GetDC();
CDC dc;
dc.Attach(hDC);
CFont font;
font.CreatePointFont(60, _T("FixedSys"));
CFont* pOldFont = dc.SelectObject(&font);
dc.SetBkMode(TRANSPARENT);          // 透過背景

dc.SetTextColor(RGB(255,255,255));

dc.TextOut(0, 0,_T("||0|1|2|3|4|5|6|7|8|9|"));
imgTemp.Save(L"g:\\fixedsys.png");

BYTE* pbyTemp;
pbyTemp = (BYTE*)imgTemp.GetBits();
int iPitchTemp = imgTemp.GetPitch();

for(int ir=0; ir<min(iRMax_i+1-dROffset, iRMax); ir++)
{
int r0=int((ir+0.5+dROffset)*dScale);
if(r0<0){continue;}
if(r0>=iHeight){continue;}
for(int ic=0; ic<min(iCMax_i+1-dCOffset,iCMax); ic++)
{
int r=r0;
int c=int((ic+0.5+dCOffset)*dScale);

if(c<0){continue;}
if(c>=iWidth){continue;}
int iValueR = pbyData[r*iPitch+4*c+2];
int iValueG = pbyData[r*iPitch+4*c+1];
int iValueB = pbyData[r*iPitch+4*c+0];

int iValue = pbyData[r*iPitch+4*c+0]+pbyData[r*iPitch+4*c+1]+pbyData[r*iPitch+4*c+2];


BYTE byDot=iValue<576 ? 255:0;
BYTE byBG = 255-byDot;

for(int irr=0; irr<iHeightTemp; irr++)
{
for(int icc=0; icc<iWidthTemp; icc++)
{
pbyTemp[irr*iPitchTemp+ 3*icc + 0]=byBG;
pbyTemp[irr*iPitchTemp+ 3*icc + 1]=byBG;
pbyTemp[irr*iPitchTemp+ 3*icc + 2]=byBG;
}
}


dc.SetTextColor(RGB(byDot, byDot, byDot));
CString sTextR;
CString sTextG;
CString sTextB;
sTextR.Format(_T("%d"), iValueR);
sTextG.Format(_T("%d"), iValueR);
sTextB.Format(_T("%d"), iValueR);
dc.TextOut(0, 0, sTextR);
dc.TextOut(0, 20, sTextG);
dc.TextOut(0, 40, sTextB);
int iRs=iHeightTemp;
int iCs=iWidthTemp;
int iRe=0;
int iCe=0;
for(int irr=0; irr<iHeightTemp; irr++)
{
for(int icc=0; icc<iWidthTemp; icc++)
{
if(pbyTemp[irr*iPitchTemp+ 3*icc + 0]==byDot)
{
if(iRs>irr){iRs=irr;}
if(iRe<irr){iRe=irr;}
if(iCs>icc){iCs=icc;}
if(iCe<icc){iCe=icc;}
}
}
}

for(int irr=iRs; irr<iRe; irr++)
{
if(r+irr>=iHeight){continue;}
if(r+irr<0){continue;}
for(int icc=iCs; icc<iCe; icc++)
{
if(c+icc>=iWidth){continue;}
if(c+icc<0){continue;}
if(pbyTemp[irr*iPitchTemp+ 3*icc + 0]==byDot)
{
pbyData[(r+irr)*iPitch+4*(c+icc)+0] = byDot;
pbyData[(r+irr)*iPitch+4*(c+icc)+1] = byDot;
pbyData[(r+irr)*iPitch+4*(c+icc)+2] = byDot;
pbyData[(r+irr)*iPitch+4*(c+icc)+3] = 255;
}
}
}
}
}









// 元のフォントに戻す
if (pOldFont != nullptr)
{
dc.SelectObject(pOldFont);
}

// フォントリソース解放
font.DeleteObject();

// CDC の切り離し
dc.Detach();
imgTemp.ReleaseDC();

return true;
}*/



const BYTE g_byFont_8_16[1992]={
	0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x24, 0x18, 0x00,
	0x00, 0x00, 0x08, 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00,
	0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x02, 0x04, 0x04, 0x08, 0x10, 0x10, 0x20, 0x40, 0x7e, 0x00,
	0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x02, 0x04, 0x18, 0x04, 0x02, 0x42, 0x42, 0x24, 0x18, 0x00,
	0x00, 0x00, 0x04, 0x04, 0x0c, 0x0c, 0x14, 0x14, 0x24, 0x24, 0x44, 0x7e, 0x04, 0x04, 0x04, 0x00,
	0x00, 0x00, 0x7e, 0x40, 0x40, 0x40, 0x58, 0x64, 0x42, 0x02, 0x02, 0x02, 0x42, 0x24, 0x18, 0x00,
	0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x40, 0x58, 0x64, 0x42, 0x42, 0x42, 0x42, 0x24, 0x18, 0x00,
	0x00, 0x00, 0x7e, 0x02, 0x02, 0x04, 0x04, 0x04, 0x08, 0x08, 0x08, 0x10, 0x10, 0x10, 0x10, 0x00,
	0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x42, 0x24, 0x18, 0x24, 0x42, 0x42, 0x42, 0x24, 0x18, 0x00,
	0x00, 0x00, 0x18, 0x24, 0x42, 0x42, 0x42, 0x42, 0x26, 0x1a, 0x02, 0x42, 0x42, 0x24, 0x18, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x22, 0x41, 0x7f, 0x40, 0x40, 0x21, 0x1e, 0x00};


const BYTE g_byFont_4_8[96]={
		0, 0, 7, 5, 5, 5, 7, 0,
		0, 0, 2, 2, 2, 2, 2, 0,
		0, 0, 7, 1, 7, 4, 7, 0,
		0, 0, 7, 1, 3, 1, 7, 0,
		0, 0, 5, 5, 7, 1, 1, 0,
		0, 0, 7, 4, 7, 1, 7, 0,
		0, 0, 7, 4, 7, 5, 7, 0,
		0, 0, 7, 1, 1, 1, 1, 0,
		0, 0, 7, 5, 7, 5, 7, 0,
		0, 0, 7, 5, 7, 1, 7, 0,
		0, 0, 0, 0, 7, 0, 0, 0,
		0, 0, 0, 2, 5, 7, 4, 3,
	};

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

	int CountDigit(const int iValue)
	{
		if(iValue>=1000000000){return 10;}
		if(iValue>=100000000){return 9;}
		if(iValue>=10000000){return 8;}
		if(iValue>=1000000){return 7;}
		if(iValue>=100000){return 6;}
		if(iValue>=10000){return 5;}
		if(iValue>=1000){return 4;}
		if(iValue>=100){return 3;}
		if(iValue>=10){return 2;}
		if(iValue>=0){return 1;}
		if(iValue>=-9){return -1;}
		if(iValue>=-99){return -2;}
		if(iValue>=-999){return -3;}
		if(iValue>=-9999){return -4;}
		if(iValue>=-99999){return -5;}
		if(iValue>=-999999){return -6;}
		if(iValue>=-9999999){return -7;}
		if(iValue>=-99999999){return -8;}
		if(iValue>=-999999999){return -9;}
		return -10;
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

	bool ImposeRGBValue(const PanImage* imgSrc, const CImage* imgZoomed, CImage* imgDst, const int iType,const double dROffset, const double dCOffset, const double dScale, const double dScaleThresh,const int iRs_i, const int iCs_i, const int iRe_i, const int iCe_i)
	{
		if(imgZoomed != imgDst)
		{
			CopyImage(imgZoomed,imgDst);
		}
		if(dScale<dScaleThresh){return true;}


		BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
		int iBPP = imgDst->GetBPP();
		if(iBPP != 32){return false;}
		int iPitchDst=imgDst->GetPitch();
		int iWidthDst = imgDst->GetWidth();
		int iHeightDst = imgDst->GetHeight();
		int iRMaxDst = int(iHeightDst/dScale+2);
		int iCMaxDst = int(iWidthDst/dScale+2);

		switch(imgSrc->enumImageType)
		{
		case IMAGE_TYPE_IIMAGE:
			{
				for(int ir_i=iRs_i; ir_i<=iRe_i; ir_i++)
				{
					int ir0_v=int(((ir_i-iRs_i)+0.5+dROffset)*dScale);
					if(ir0_v<0){continue;}
					if(ir0_v>=iHeightDst){continue;}
					for(int ic_i=iCs_i; ic_i<=iCe_i; ic_i++)
					{
						int ic0_v=int(((ic_i-iCs_i)+1+dCOffset)*dScale);
						int ic1_v=int(((ic_i-iCs_i)+0.5+dCOffset)*dScale);

						if(ic0_v<0){continue;}
						if(ic0_v>=iWidthDst){continue;}

						COLORREF col = imgDst->GetPixel(ic1_v, ir0_v);

						int iValueR =  GetRValue(col);
						int iValueG =  GetGValue(col);
						int iValueB =  GetBValue(col);
						BYTE byDot=iValueR+iValueG+iValueB<480 ? 255:0;
						int iValue;
						imgSrc->GetValue(ir_i, ic_i,&iValue);
						if(dScale>48)
						{
							ImposeValue_8_16(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValue,ir0_v-8-0, ic0_v-2-16, byDot);
						}
						else
						{
							ImposeValue_4_8(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValue,ir0_v-4-0, ic0_v-2-8, byDot);
						}
					}
				}
				return true;
			}
		case IMAGE_TYPE_CIMAGE:
			{

				for(int ir_i=iRs_i; ir_i<=iRe_i; ir_i++)
				{
					int ir0_v=int(((ir_i-iRs_i)+0.5+dROffset)*dScale);
					if(ir0_v<0){continue;}
					if(ir0_v>=iHeightDst){continue;}
					for(int ic_i=iCs_i; ic_i<=iCe_i; ic_i++)
					{
						int ic0_v=int(((ic_i-iCs_i)+1+dCOffset)*dScale);

						if(ic0_v<0){continue;}
						if(ic0_v>=iWidthDst){continue;}

						COLORREF col = imgSrc->cImage.GetPixel(ic_i, ir_i);
						int iValueR =  GetRValue(col);
						int iValueG =  GetGValue(col);
						int iValueB =  GetBValue(col);

						BYTE byDot=iValueR+iValueG+iValueB<480 ? 255:0;

						if(dScale>48)
						{
							if((iValueR==iValueG) && (iValueR==iValueB))
							{
								ImposeValue_8_16(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueG,ir0_v-8-0, ic0_v-2-16, byDot);
							}
							else
							{
								ImposeValue_8_16(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueR,ir0_v-8-16, ic0_v-2-16, byDot);
								ImposeValue_8_16(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueG,ir0_v-8-0, ic0_v-2-16, byDot);
								ImposeValue_8_16(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueB,ir0_v-8+16, ic0_v-2-16, byDot);
							}
						}
						else
						{
							if((iValueR==iValueG) && (iValueR==iValueB))
							{
								ImposeValue_4_8(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueG,ir0_v-4-0, ic0_v-2-8, byDot);
							}
							else
							{
								ImposeValue_4_8(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueR,ir0_v-4-8, ic0_v-2-8, byDot);
								ImposeValue_4_8(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueG,ir0_v-4-0, ic0_v-2-8, byDot);
								ImposeValue_4_8(pbyDataDst, iPitchDst, iHeightDst, iWidthDst, iValueB,ir0_v-4+8, ic0_v-2-8, byDot);
							}
						}
					}
				}
				return true;
			}
		}
		return false;
	}
	bool ImposeGrid(const CImage* imgValueSrc, const CImage* imgSrc, CImage* imgDst, const int iType, const double dROffset, const double dCOffset, const double dScale, const double dScaleThresh,const int iRMax_i, const int iCMax_i)
	{
		if(imgSrc != imgDst)
		{
			CopyImage(imgSrc,imgDst);
		}
		if(dScale<dScaleThresh){return true;}
		if(iType==0){return true;}

		BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
		int iBPP = imgDst->GetBPP();
		if(iBPP != 32){return false;}
		int iPitch =imgDst->GetPitch();
		int iWidth = imgDst->GetWidth();
		int iHeight = imgDst->GetHeight();
		int iRMax = int(iHeight/dScale+2);
		int iCMax = int(iWidth/dScale+2);


		if(iType==1)
		{
			for(int ir=0; ir<min(iRMax_i+2-dROffset, iRMax); ir++)
			{
				int r0=int((ir+dROffset)*dScale);
				if(r0<0){continue;}
				if(r0>=iHeight){continue;}
				for(int ic=0; ic<min(iCMax_i+2-dCOffset,iCMax); ic++)
				{
					int r=r0;
					int c=int((ic+dCOffset)*dScale);
					if(c<0){continue;}
					if(c>=iWidth){continue;}
					int iValue = pbyDataDst[r*iPitch+4*c+0]+pbyDataDst[r*iPitch+4*c+1]+pbyDataDst[r*iPitch+4*c+2];
					BYTE byDot=iValue<576 ? 255:0;
					SetRGBAValue(pbyDataDst, r, c, iPitch, byDot, byDot, byDot, 255);

					c=int((ic+dCOffset)*dScale + 1);
					if(c>=iWidth){continue;}
					SetRGBAValue(pbyDataDst, r, c, iPitch, byDot, byDot, byDot, 255);

					r=r0 + 1;
					if(r>=iHeight){break;}

					c=int((ic+dCOffset)*dScale);
					if(c<0){continue;}
					if(c>=iWidth){continue;}
					SetRGBAValue(pbyDataDst, r, c, iPitch, byDot, byDot, byDot, 255);

					c=int((ic+dCOffset)*dScale + 1);
					if(c>=iWidth){continue;}
					SetRGBAValue(pbyDataDst, r, c, iPitch, byDot, byDot, byDot, 255);
				}

			}
			return true;
		}

		if(iType==2)
		{
			LONGLONG llCountW=0;
			LONGLONG llCountM=0;
			LONGLONG llCountB=0;
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					long lSum=pbyDataDst[r*iPitch+4*c+2];
					lSum+=pbyDataDst[r*iPitch+4*c+1];
					lSum+=pbyDataDst[r*iPitch+4*c+0];

					if(lSum>480){llCountW++;}
					else if(lSum>288){llCountM++;}
					else{llCountB++;}
				}
			}
			BYTE byDot;
			if(llCountM<llCountW)
			{
				if(llCountM<llCountB){byDot=128;}
				else if(llCountW>llCountB){byDot=0;}
				else{byDot=255;}
			}
			else if(llCountW>llCountB){byDot=0;}
			else{byDot=255;}

			for(int ir=0; ir<iRMax; ir++)
			{
				if(ir+dROffset>=iRMax_i+2){continue;}
				int r=int((ir+dROffset)*dScale);
				if(r<0){continue;}
				if(r>=iHeight){continue;}

				for(int c=0; c<min((iCMax_i+dCOffset+1)*dScale,iWidth); c++)
				{
					SetRGBAValue(pbyDataDst, r, c, iPitch, byDot, byDot, byDot, 255);
				}
				r=int((ir+dROffset)*dScale+1);
				if(r>=iHeight){continue;}
				for(int c=0; c<min((iCMax_i+dCOffset+1)*dScale,iWidth); c++)
				{
					SetRGBAValue(pbyDataDst, r, c, iPitch, byDot, byDot, byDot, 255);
				}
			}

			for(int ic=0; ic<iCMax; ic++)
			{
				if(ic+dCOffset>=iCMax_i+2){continue;}
				int c=int((ic+dCOffset)*dScale);
				if(c<0){continue;}
				if(c>=iWidth){continue;}
				for(int r=0; r<min((iRMax_i+dROffset+1)*dScale,iHeight); r++)
				{
					SetRGBAValue(pbyDataDst, r, c, iPitch, byDot, byDot, byDot, 255);
				}
				c=int((ic+dCOffset)*dScale+1);
				if(c>=iWidth){continue;}
				for(int r=0; r<min((iRMax_i+dROffset+1)*dScale,iHeight); r++)
				{
					SetRGBAValue(pbyDataDst, r, c, iPitch, byDot, byDot, byDot, 255);
				}
			}
			return true;
		}

		if(iType==3)
		{
			CImage imgTemp;
			CopyImage(imgValueSrc,&imgTemp);
			BYTE* pbyDataTemp = (BYTE*)imgTemp.GetBits();

			for(int r=0; r<iHeight-1;r++)
			{
				for(int c=0; c<iWidth-1; c++)
				{
					if(( pbyDataTemp[r*iPitch+4*c+0] != pbyDataTemp[r*iPitch+4*(c+1)+0]) || ( pbyDataTemp[r*iPitch+4*c+1] != pbyDataTemp[r*iPitch+4*(c+1)+1]) || ( pbyDataTemp[r*iPitch+4*c+2] != pbyDataTemp[r*iPitch+4*(c+1)+2]))
					{
						int iValue = pbyDataTemp[r*iPitch+4*c+0]+pbyDataTemp[r*iPitch+4*c+1]+pbyDataTemp[r*iPitch+4*c+2];
						BYTE byDot=iValue<576 ? 255:0;
						SetRGBAValue(pbyDataDst, r, c, iPitch, byDot, byDot, byDot, 255);
					}
					if(( pbyDataTemp[r*iPitch+4*c+0] != pbyDataTemp[(r+1)*iPitch+4*c+0]) || ( pbyDataTemp[r*iPitch+4*c+1] != pbyDataTemp[(r+1)*iPitch+4*c+1]) || ( pbyDataTemp[r*iPitch+4*c+2] != pbyDataTemp[(r+1)*iPitch+4*c+2]))
					{
						int iValue = pbyDataTemp[r*iPitch+4*c+0]+pbyDataTemp[r*iPitch+4*c+1]+pbyDataTemp[r*iPitch+4*c+2];
						BYTE byDot=iValue<576 ? 255:0;
						SetRGBAValue(pbyDataDst, r, c, iPitch, byDot, byDot, byDot, 255);
					}
				}
			}
			return true;

		}
		return false;
	}
	bool MakeReservedChannelZero(const CImage* imgSrc, CImage* imgDst)
	{
		CopyImage(imgSrc, imgDst);

		if(imgSrc->GetBPP()==32){return true;}
		if(imgSrc->GetBPP()==24){return true;}


		RGBQUAD* srcTable=NULL;
		int iColors = imgDst->GetMaxColorTableEntries();
		if (iColors > 0) 
		{
			srcTable = new RGBQUAD[iColors];
			imgDst->GetColorTable(0, iColors, srcTable);
			for(int i=0; i<iColors; i++)
			{
				srcTable[i].rgbReserved=0;
			}
			SetColorTable(imgDst, srcTable, iColors);
			SAFE_DELETE(srcTable);
		}

		return true;
	}

	bool ZoomImage(const CImage* imgSrc, CImage* imgDst, const double dR0_Src, const double dC0_Src, const double dScale, const int iWidth_Dst, const int iHeight_Dst, const bool bRGBSeparated)
	{

		int iWidthSrc = imgSrc->GetWidth();
		int iHeightSrc = imgSrc->GetHeight();

		if(imgDst->IsNull() != true){imgDst->Destroy();}
		imgDst->Create(iWidth_Dst, iHeight_Dst,32);

		BYTE* pbyDataSrc = (BYTE*)imgSrc->GetBits();
		int iPitch_src=imgSrc->GetPitch();
		BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
		int iPitch_dst=imgDst->GetPitch();

		int iBPP = imgSrc->GetBPP();

		if((iBPP==24) || (iBPP==32))
		{
			int iColorPitch = (iBPP==24 ? 3 : 4);
			for(int r=0; r<iHeight_Dst; r++)
			{
				int ir_Src=int(r/dScale+dR0_Src);

				if((ir_Src<0)||(ir_Src>=iHeightSrc))
				{
					for(int c=0; c<iWidth_Dst; c++)
					{
						SetRGBAValue(pbyDataDst, r, c, iPitch_dst, 127, 127, 127, 255);
					}
					continue;
				}

				for(int c=0; c<iWidth_Dst; c++)
				{
					int ic_Src=int(c/dScale+dC0_Src);
					if((ic_Src<0)||(ic_Src>=iWidthSrc))
					{
						SetRGBAValue(pbyDataDst, r, c, iPitch_dst, 127, 127, 127, 255);
						continue;
					}

					if(bRGBSeparated == false)
					{
						pbyDataDst[r*iPitch_dst+c*4+2]=pbyDataSrc[ir_Src*iPitch_src+ic_Src*iColorPitch+2];
						pbyDataDst[r*iPitch_dst+c*4+1]=pbyDataSrc[ir_Src*iPitch_src+ic_Src*iColorPitch+1];
						pbyDataDst[r*iPitch_dst+c*4+0]=pbyDataSrc[ir_Src*iPitch_src+ic_Src*iColorPitch+0];
						if(iBPP==32){pbyDataDst[r*iPitch_dst+c*4+3]=pbyDataSrc[ir_Src*iPitch_src+ic_Src*iColorPitch+3];}
						else{pbyDataDst[r*iPitch_dst+c*4+3]=255;}
						continue;
					}

					double dFlac = c/dScale+dC0_Src - int(c/dScale+dC0_Src);
					if(dFlac<1/3.0)
					{
						pbyDataDst[r*iPitch_dst+c*4+2]=pbyDataSrc[ir_Src*iPitch_src+ic_Src*iColorPitch+2];
						pbyDataDst[r*iPitch_dst+c*4+1]=0;
						pbyDataDst[r*iPitch_dst+c*4+0]=0;
						if(iBPP==32){pbyDataDst[r*iPitch_dst+c*4+3]=pbyDataSrc[ir_Src*iPitch_src+ic_Src*iColorPitch+3];}
					}
					else if(dFlac<2/3.0)
					{
						pbyDataDst[r*iPitch_dst+c*4+2]=0;
						pbyDataDst[r*iPitch_dst+c*4+1]=pbyDataSrc[ir_Src*iPitch_src+ic_Src*iColorPitch+1];
						pbyDataDst[r*iPitch_dst+c*4+0]=0;
						if(iBPP==32){pbyDataDst[r*iPitch_dst+c*4+3]=pbyDataSrc[ir_Src*iPitch_src+ic_Src*iColorPitch+3];}
					}
					else
					{
						pbyDataDst[r*iPitch_dst+c*4+2]=0;
						pbyDataDst[r*iPitch_dst+c*4+1]=0;
						pbyDataDst[r*iPitch_dst+c*4+0]=pbyDataSrc[ir_Src*iPitch_src+ic_Src*iColorPitch+0];
						if(iBPP==32){pbyDataDst[r*iPitch_dst+c*4+3]=pbyDataSrc[ir_Src*iPitch_src+ic_Src*iColorPitch+3];}
					}
					if(iBPP==24){pbyDataDst[r*iPitch_dst+c*4+3]=255;}
				}
			}
			return true;
		}

		RGBQUAD* srcTable=NULL;
		int iColors = imgSrc->GetMaxColorTableEntries();
		if (iColors > 0) 
		{
			srcTable = new RGBQUAD[iColors];
			imgSrc->GetColorTable(0, iColors, srcTable);
		}
		bool bAlpha = IsAlphaChanneled(imgSrc);

		const BYTE byMasks[9]={0,1,3,0,15,0,0,0,255};
		for(int r=0; r<iHeight_Dst; r++)
		{
			int ir_Src=int(r/dScale+dR0_Src);

			if((ir_Src<0)||(ir_Src>=iHeightSrc))
			{
				for(int c=0; c<iWidth_Dst; c++)
				{
					SetRGBAValue(pbyDataDst, r, c, iPitch_dst, 127, 127, 127, 255);
				}
				continue;
			}

			for(int c=0; c<iWidth_Dst; c++)
			{
				int ic_Src=int(c/dScale+dC0_Src);
				if((ic_Src<0)||(ic_Src>=iWidthSrc))
				{
					SetRGBAValue(pbyDataDst, r, c, iPitch_dst, 127, 127, 127, 255);
					continue;
				}


				int iPosition= ir_Src*iPitch_src+ic_Src/(8/iBPP);
				int iDigit = (8/iBPP)-(ic_Src%(8/iBPP))-1;

				BYTE byIndex = (pbyDataSrc[iPosition] & (byMasks[iBPP]<< (iDigit*iBPP))) >> (iDigit*iBPP);

				if(bRGBSeparated == true)
				{
					double dFlac = c/dScale+dC0_Src - int(c/dScale+dC0_Src);
					if(dFlac<1/3.0)
					{
						pbyDataDst[r*iPitch_dst+c*4+2]=srcTable[byIndex].rgbRed;
						pbyDataDst[r*iPitch_dst+c*4+1]=0;
						pbyDataDst[r*iPitch_dst+c*4+0]=0;
						pbyDataDst[r*iPitch_dst+c*4+3]=((bAlpha == true) ? srcTable[byIndex].rgbReserved : 255);
					}
					else if(dFlac<2/3.0)
					{
						pbyDataDst[r*iPitch_dst+c*4+2]=0;
						pbyDataDst[r*iPitch_dst+c*4+1]=srcTable[byIndex].rgbGreen;
						pbyDataDst[r*iPitch_dst+c*4+0]=0;
						pbyDataDst[r*iPitch_dst+c*4+3]=((bAlpha == true) ? srcTable[byIndex].rgbReserved : 255);
					}
					else
					{
						pbyDataDst[r*iPitch_dst+c*4+2]=0;
						pbyDataDst[r*iPitch_dst+c*4+1]=0;
						pbyDataDst[r*iPitch_dst+c*4+0]=srcTable[byIndex].rgbBlue;
						pbyDataDst[r*iPitch_dst+c*4+3]=((bAlpha == true) ? srcTable[byIndex].rgbReserved : 255);
					}
				}
				else
				{
					pbyDataDst[r*iPitch_dst+c*4+2]=srcTable[byIndex].rgbRed;
					pbyDataDst[r*iPitch_dst+c*4+1]=srcTable[byIndex].rgbGreen;
					pbyDataDst[r*iPitch_dst+c*4+0]=srcTable[byIndex].rgbBlue;
					pbyDataDst[r*iPitch_dst+c*4+3]=((bAlpha == true) ? srcTable[byIndex].rgbReserved : 255);
				}
			}
		}
		SAFE_DELETE(srcTable);

		return true;
	}

	bool CountColorNum(const ImgRGB* imgRGB, int* iColorNum_out, UINT* uiMap_out)
	{
		int iWidth = imgRGB->iWidth;
		int iHeight = imgRGB->iHeight;
		BYTE* byMap;
		byMap = new BYTE [256*256*32];
		memset(byMap, 0, 256*256*32);
		for(int r=0; r<iHeight; r++)
		{
			for(int c=0; c<iWidth ; c++)
			{
				BYTE byDigit = imgRGB->byImgB[r*iWidth+c] & (0x07);
				UINT uiPosition = ((imgRGB->byImgR[r*iWidth+c])<<13) + ((imgRGB->byImgG[r*iWidth+c])<<5) + ((imgRGB->byImgB[r*iWidth+c] & (0xF8)) >> 3);
				byMap[uiPosition] |= (1<<byDigit);
			}
		}
		int iColorNum=0;
		if(uiMap_out != NULL)
		{
			for(int i=0; i<256*256*256; i++)
			{
				uiMap_out[i]=0x1000000;
			}
		}
		for(int i=0; i<256*256*32; i++)
		{
			for(int j=0; j<8; j++)
			{
				if((byMap[i] & (1<<j)) == (1<<j))
				{
					if(uiMap_out != NULL){uiMap_out[i*8+j]=iColorNum;}
					iColorNum++;
				}
			}
		}
		SAFE_DELETE(byMap);
		*iColorNum_out = iColorNum;
		return TRUE;
	}

	bool CountColorNum(const CImage* imgSrc, int* iColorNum_out, UINT* uiMap_out)
	{
		ImgRGB imgRGB;
		ConvertImage(imgSrc, &imgRGB);
		return CountColorNum(&imgRGB, iColorNum_out, uiMap_out);
	}
	bool MakeColorTable(const ImgRGB* imgRGB, RGBQUAD* rgbqTable_out, ULONGLONG* ullFrequency_out, const int iLength, int* iUsedColors_out, bool* bGrayScale_out)
	{

		int iWidth = imgRGB->iWidth;
		int iHeight = imgRGB->iHeight;
		RGBQUAD* rgbqTable_temp;
		//	if(iLength>256){return false;}
		rgbqTable_temp = new RGBQUAD[iLength];
		ULONGLONG* ullFrequency;
		ullFrequency = new ULONGLONG[iLength];
		for(int i=0; i<iLength; i++)
		{
			ullFrequency[i]=0;
		}

		int iColorNum;
		UINT* uiMap;
		uiMap = new UINT[256*256*256];

		bool bRet = CountColorNum(imgRGB, &iColorNum, uiMap);
		for(int i=0; i<256*256*256; i++)
		{
			if(uiMap[i]==0x1000000){continue;}
			UINT uiIndex = uiMap[i];
			rgbqTable_temp[uiIndex].rgbRed=(i>>16);
			rgbqTable_temp[uiIndex].rgbGreen=((i>>8) & 0xFF);
			rgbqTable_temp[uiIndex].rgbBlue=(i & 0xFF);
			rgbqTable_temp[uiIndex].rgbReserved=0;

		}
		int iTableLength = iColorNum;
		for(int r=0; r<iHeight; r++)
		{
			for(int c=0; c<iWidth ; c++)
			{
				BYTE byDataR = imgRGB->byImgR[r*iWidth+c];
				BYTE byDataG = imgRGB->byImgG[r*iWidth+c];
				BYTE byDataB = imgRGB->byImgB[r*iWidth+c];
				UINT uiIndex = uiMap[byDataR*256*256+byDataG*256+byDataB];
				ullFrequency[uiIndex]++;
			}
		}
		bool bGrayScale=true;
		for(int i=0; i<iTableLength; i++)
		{
			if(rgbqTable_temp[i].rgbBlue != rgbqTable_temp[i].rgbRed){bGrayScale=false; break;}
			if(rgbqTable_temp[i].rgbBlue != rgbqTable_temp[i].rgbGreen){bGrayScale=false; break;}
		}

		if(rgbqTable_out != NULL)
		{
			for(int i=0; i<iLength; i++)
			{
				rgbqTable_out[i].rgbRed=rgbqTable_temp[i].rgbRed;
				rgbqTable_out[i].rgbGreen=rgbqTable_temp[i].rgbGreen;
				rgbqTable_out[i].rgbBlue=rgbqTable_temp[i].rgbBlue;
				rgbqTable_out[i].rgbReserved=rgbqTable_temp[i].rgbReserved;
			}
		}
		SAFE_DELETE(rgbqTable_temp);

		if(ullFrequency_out != NULL)
		{
			for(int i=0; i< iLength; i++)
			{
				ullFrequency_out[i]=ullFrequency[i];
			}
		}
		SAFE_DELETE(ullFrequency);

		if(iUsedColors_out != NULL){*iUsedColors_out = iTableLength;}
		if(bGrayScale_out != NULL){*bGrayScale_out = bGrayScale;}
		return true;
	}

	bool MakeColorTable(const CImage* imgSrc, RGBQUAD* rgbqTable_out, ULONGLONG* ullFrequency_out, const int iLength, int* iUsedColors_out, bool* bGrayScale_out)
	{
		int iBPP = imgSrc->GetBPP();
		if((iBPP==24) || (iBPP==32))
		{
			ImgRGB imgRGB;
			ConvertImage(imgSrc,&imgRGB);
			return MakeColorTable(&imgRGB, rgbqTable_out, ullFrequency_out, iLength, iUsedColors_out, bGrayScale_out);
		}
		RGBQUAD* rgbqTable;
		int iColors = imgSrc->GetMaxColorTableEntries();
		if (iColors <= 0){return false;}

		rgbqTable = new RGBQUAD[iColors];
		imgSrc->GetColorTable(0, iColors, rgbqTable);

		ULONGLONG* ullFrequency;
		ullFrequency = new ULONGLONG[iColors];
		for(int i=0; i<1<<min(24, iBPP); i++)
		{
			ullFrequency[i]=0;
		}

		const BYTE byMasks[9]={0,1,3,0,15,0,0,0,255};
		int iPitch = imgSrc->GetPitch();
		BYTE* pbyDataSrc = (BYTE*)imgSrc->GetBits();
		int iHeight= imgSrc->GetHeight();
		int iWidth = imgSrc->GetWidth();

		for(int r=0; r<iHeight; r++)
		{
			for(int c=0; c<iWidth; c++)
			{
				int iPosition= r*iPitch + c/(8/iBPP);
				int iDigit = (8/iBPP)-(c%(8/iBPP))-1;

				BYTE byIndex = (pbyDataSrc[iPosition] & (byMasks[iBPP]<< (iDigit*iBPP))) >> (iDigit*iBPP);
				ullFrequency[byIndex]++;

			}
		}
		int iUsedColors=0;
		bool bGrayScale=true;
		for(int i=0; i<iColors; i++)
		{
			if(ullFrequency[i] == 0){continue;}
			iUsedColors++;
			if(bGrayScale==true)
			{
				if(rgbqTable[i].rgbBlue != rgbqTable[i].rgbRed){bGrayScale=false; }
				if(rgbqTable[i].rgbBlue != rgbqTable[i].rgbGreen){bGrayScale=false; }
			}
		}

		if(rgbqTable_out != NULL)
		{
			if(bGrayScale==true)
			{
				for(int i=0; i<iLength; i++)
				{
					rgbqTable_out [i].rgbRed=i;
					rgbqTable_out [i].rgbGreen=i;
					rgbqTable_out [i].rgbBlue=i;
					rgbqTable_out [i].rgbReserved=0;
				}
			}
			else
			{
				for(int i=0; i<iLength; i++)
				{
					rgbqTable_out [i].rgbRed=rgbqTable[i].rgbRed;
					rgbqTable_out [i].rgbGreen=rgbqTable[i].rgbGreen;
					rgbqTable_out [i].rgbBlue=rgbqTable[i].rgbBlue;
					rgbqTable_out [i].rgbReserved=rgbqTable[i].rgbReserved;
				}
			}
		}
		SAFE_DELETE(rgbqTable);

		if(ullFrequency_out != NULL)
		{
			for(int i=0; i< iLength; i++)
			{
				ullFrequency_out[i]=ullFrequency[i];
			}
		}
		SAFE_DELETE(ullFrequency);

		if(iUsedColors_out != NULL){*iUsedColors_out = iUsedColors;}
		if(bGrayScale_out != NULL){*bGrayScale_out = bGrayScale;}

		return true;
	}


	int GetMonoTableIndex(const BYTE byValue, const RGBQUAD* rgbqTable, const int iLength)
	{
		for(int i=0; i<iLength; i++)
		{
			if(rgbqTable[i].rgbRed==byValue)
			{
				return i;
			}
		}
		return -1;
	}
	/*
	bool Set8bitsColorTableImage(const CImage* imgSrc,const RGBQUAD* rgbqTable256, CImage* imgDst)
	{
		int iWidth = imgSrc->GetWidth();
		int iHeight = imgSrc->GetHeight();
		int iPitch_dst = imgDst->GetPitch();

		if (imgDst->IsNull() != true) {imgDst->Destroy();}
		imgDst->Create(iWidth, iHeight, 8);

		SetColorTable(imgDst, rgbqTable256, 256);


		ImgRGB imgRGB;
		ConvertImage(imgSrc,&imgRGB);

		BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
		if(imgSrc->GetBPP()==24)
		{
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c< iWidth; c++)
				{
					BYTE byDataR = imgRGB.byImgR[r*iWidth+c];
					BYTE byDataG = imgRGB.byImgG[r*iWidth+c];
					BYTE byDataB = imgRGB.byImgB[r*iWidth+c];

					int iIndex = GetColorTableIndex(rgbqTable256, 256, byDataR,byDataG,byDataB,0);
					if(iIndex<0)
					{
						return false;
					}
					pbyDataDst[r*iPitch_dst+c]=(BYTE)iIndex;

				}
			}
		}
		if(imgSrc->GetBPP()==32)
		{
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c< iWidth; c++)
				{
					BYTE byDataR = imgRGB.byImgR[r*iWidth+c];
					BYTE byDataG = imgRGB.byImgG[r*iWidth+c];
					BYTE byDataB = imgRGB.byImgB[r*iWidth+c];
					BYTE byDataA = imgRGB.byImgA[r*iWidth+c];

					int iIndex = GetColorTableIndex(rgbqTable256, 256, byDataR,byDataG,byDataB,byDataA);
					if(iIndex<0)
					{
						return false;
					}
					pbyDataDst[r*iPitch_dst+c]=(BYTE)iIndex;

				}
			}
		}
		return true;
	}
	*/
	bool SetAsc8bitsMonoColorTableImage(const CImage* imgSrc, CImage* imgDst)
	{
		if(IsImageMonochrome(imgSrc)==false){return false;}

		RGBQUAD* rgbqTable_src= new RGBQUAD[256];
		imgSrc->GetColorTable(0, 256, rgbqTable_src);

		CopyImage(imgSrc, imgDst);

		bool bAsc8bit=true;
		for(int iColorIndex=0; iColorIndex<256; iColorIndex++)
		{
			if(rgbqTable_src[iColorIndex].rgbRed != iColorIndex){bAsc8bit = false; break;}
		}
		if(bAsc8bit==true){return true;}

		ImgRGB imgRGB;
		ConvertImage(imgSrc,&imgRGB);

		RGBQUAD rgbqMono[256];
		for(int iColorIndex=0; iColorIndex<256; iColorIndex++)
		{
			rgbqMono[iColorIndex].rgbRed=iColorIndex;
			rgbqMono[iColorIndex].rgbGreen=iColorIndex;
			rgbqMono[iColorIndex].rgbBlue=iColorIndex;
			rgbqMono[iColorIndex].rgbReserved=0;
		}
		SetColorTable(imgDst, rgbqMono, 256);


		BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
		int iHeight = imgDst->GetHeight();
		int iWidth = imgDst->GetWidth();
		int iPitch_dst =imgDst->GetPitch();

		for(int r=0; r<iHeight; r++)
		{
			for(int c=0; c< iWidth; c++)
			{
				BYTE byValue = imgRGB.byImgR[r*iWidth+c];

				int iIndex = GetMonoTableIndex(byValue, rgbqTable_src, 256);
				if(iIndex<0)
				{
					return false;
				}
				pbyDataDst[r*iPitch_dst+c]=rgbqTable_src[iIndex].rgbRed;
			}
		}
		return true;
	}

	bool ConvertImage_AreaCoverage(const CImage* imgSrc, const int iBPP, CImage* imgDst)
	{
		if(iBPP>=24)
		{
			return ConvertImage_LossLess(imgSrc, iBPP, imgDst);
		}

		int iBPP_src = imgSrc->GetBPP();
		if(iBPP_src==8){return CopyImage(imgSrc, imgDst);}


		int iWidth = imgSrc->GetWidth();
		int iHeight = imgSrc->GetHeight();

		ImgRGB imgRGB;
		ConvertImage(imgSrc,&imgRGB);
		RGBQUAD* rgbqTable;
		rgbqTable = new RGBQUAD[iWidth*iHeight];
		ULONGLONG* ullFrequency;
		ullFrequency = new ULONGLONG[iWidth*iHeight];
		int iColors;
		MakeColorTable(&imgRGB, rgbqTable, ullFrequency, iWidth*iHeight, &iColors, NULL);

		int* iPopularOrder;
		int* iConversionTable;
		iPopularOrder=new int[iColors];
		iConversionTable=new int[iColors];

		index_i(ullFrequency, iColors, iPopularOrder);

		RGBQUAD* rgbqTablePopularOrder;
		rgbqTablePopularOrder = new RGBQUAD[1ull<<min(24, iBPP)];
		GetColorConversionTable(rgbqTable, iPopularOrder, iColors, rgbqTablePopularOrder, 1ul<<min(24, iBPP), iConversionTable);

		if (imgDst->IsNull() != true) {imgDst->Destroy();}
		imgDst->Create(iWidth, iHeight,iBPP);



		SetColorTable(imgDst, rgbqTablePopularOrder, 1<<min(24, iBPP));

		int iPitch_dst = imgDst->GetPitch();
		BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
		for(int r=0; r<iHeight; r++)
		{
			for(int c=0; c< iWidth/(8/iBPP); c++)
			{
				pbyDataDst[r*iPitch_dst+c]=0;
			}
		}

		for(int r=0; r<iHeight; r++)
		{		
			for(int c=0; c< iWidth; c++)
			{
				int iColorTableIndex=GetColorTableIndex(rgbqTable,iColors, imgRGB.byImgR[r*iWidth+c], imgRGB.byImgG[r*iWidth+c], imgRGB.byImgB[r*iWidth+c], 0);

				int iDigit = (8/iBPP)-(c%(8/iBPP))-1;
				pbyDataDst[r*iPitch_dst+c/(8/iBPP)]+=(iConversionTable[iColorTableIndex]<<(iBPP*iDigit));
			}
		}
		SAFE_DELETE(rgbqTable);
		SAFE_DELETE(ullFrequency);
		SAFE_DELETE(iPopularOrder);
		SAFE_DELETE(iConversionTable);

		return true;
	}



	inline UINT GetRGBDistanceSq(const RGBQUAD rgbqTable, const BYTE byR, const BYTE byG, const BYTE byB)
	{
		return (rgbqTable.rgbRed-byR)*(rgbqTable.rgbRed-byR)
			+(rgbqTable.rgbGreen-byG)*(rgbqTable.rgbGreen-byG)
			+(rgbqTable.rgbBlue-byB)*(rgbqTable.rgbBlue-byB);
	}
	inline UINT GetRGBDistanceSq(const RGBQUAD rgbqTable,const RGBQUAD rgbqTable2)
	{
		return (rgbqTable.rgbRed-rgbqTable2.rgbRed)*(rgbqTable.rgbRed-rgbqTable2.rgbRed)
			+(rgbqTable.rgbGreen-rgbqTable2.rgbGreen)*(rgbqTable.rgbGreen-rgbqTable2.rgbGreen)
			+(rgbqTable.rgbBlue-rgbqTable2.rgbBlue)*(rgbqTable.rgbBlue-rgbqTable2.rgbBlue);
	}

	void GetMinMaxValue(const RGBQUAD* rgbqTable, const int iLength,
		BYTE* pbyR_Min_out,
		BYTE* pbyG_Min_out,
		BYTE* pbyB_Min_out,
		BYTE* pbyR_Max_out,
		BYTE* pbyG_Max_out,
		BYTE* pbyB_Max_out)
	{

		BYTE byR_Max=0;
		BYTE byG_Max=0;
		BYTE byB_Max=0;
		BYTE byR_Min=255;
		BYTE byG_Min=255;
		BYTE byB_Min=255;

		for(int i=0; i<iLength; i++)
		{
			if(byR_Max < rgbqTable[i].rgbRed){byR_Max = rgbqTable[i].rgbRed;}
			if(byG_Max < rgbqTable[i].rgbGreen){byG_Max = rgbqTable[i].rgbGreen;}
			if(byB_Max < rgbqTable[i].rgbBlue){byB_Max = rgbqTable[i].rgbBlue;}
			if(byR_Min > rgbqTable[i].rgbRed){byR_Min = rgbqTable[i].rgbRed;}
			if(byG_Min > rgbqTable[i].rgbGreen){byG_Min = rgbqTable[i].rgbGreen;}
			if(byB_Min > rgbqTable[i].rgbBlue){byB_Min = rgbqTable[i].rgbBlue;}
		}
		*pbyR_Max_out=byR_Max;
		*pbyG_Max_out=byG_Max;
		*pbyB_Max_out=byB_Max;
		*pbyR_Min_out=byR_Min;
		*pbyG_Min_out=byG_Min;
		*pbyB_Min_out=byB_Min;
	}

	void GetAverageValue(const RGBQUAD* rgbqTable, const int iLength,
		BYTE* pbyR_out,
		BYTE* pbyG_out,
		BYTE* pbyB_out)
	{
		ULONGLONG ullRSum=0;
		ULONGLONG ullGSum=0;
		ULONGLONG ullBSum=0;
		for(int i=0; i<iLength; i++)
		{
			ullRSum+=rgbqTable[i].rgbRed;
			ullGSum+=rgbqTable[i].rgbGreen;
			ullBSum+=rgbqTable[i].rgbBlue;
		}
		*pbyR_out=max(0,min(255,(BYTE)(ullRSum/iLength)));
		*pbyG_out=max(0,min(255,(BYTE)(ullGSum/iLength)));
		*pbyB_out=max(0,min(255,(BYTE)(ullBSum/iLength)));
	}
	bool K_mean_RGB_2Class(const RGBQUAD* rgbqTable, const ULONGLONG* ullFrequency, const int iLength, const BYTE byR1_in, const BYTE byG1_in, const BYTE byB1_in, const BYTE byR2_in, const BYTE byG2_in, const BYTE byB2_in, BYTE* pbyR1_out, BYTE* pbyG1_out, BYTE* pbyB1_out, BYTE* pbyR2_out, BYTE* pbyG2_out, BYTE* pbyB2_out)
	{
		if(iLength<=1){return false;}

		BYTE byR1=byR1_in;
		BYTE byG1=byG1_in;
		BYTE byB1=byB1_in;
		BYTE byR2=byR2_in;
		BYTE byG2=byG2_in;
		BYTE byB2=byB2_in;

		int* iLabel;
		iLabel=new int[iLength];
		for(int i=0; i<iLength; i++){iLabel[i]=0;}

		while(1)
		{
			ULONGLONG ullNum1=0;
			ULONGLONG ullNum2=0;
			bool bChanged = false;
			for(int i=0; i<iLength; i++)
			{
				ULONGLONG ullDistance1=GetRGBDistanceSq(rgbqTable[i], byR1, byG1, byB1);
				ULONGLONG ullDistance2=GetRGBDistanceSq(rgbqTable[i], byR2, byG2, byB2);
				if(ullDistance1<ullDistance2)
				{
					ullNum1+=ullFrequency[i];
					if(iLabel[i] != 1){bChanged=true;}
					iLabel[i]=1;
				}
				else
				{
					ullNum2+=ullFrequency[i];
					if(iLabel[i] != 2){bChanged=true;}
					iLabel[i]=2;
				}
			}

			ULONGLONG ullSumR1=0;
			ULONGLONG ullSumG1=0;
			ULONGLONG ullSumB1=0;
			ULONGLONG ullSumR2=0;
			ULONGLONG ullSumG2=0;
			ULONGLONG ullSumB2=0;

			for(int i=0; i<iLength; i++)
			{
				if(iLabel[i]==1)
				{
					ullSumR1+=rgbqTable[i].rgbRed*ullFrequency[i];
					ullSumG1+=rgbqTable[i].rgbGreen*ullFrequency[i];
					ullSumB1+=rgbqTable[i].rgbBlue*ullFrequency[i];
				}
				else
				{
					ullSumR2+=rgbqTable[i].rgbRed*ullFrequency[i];
					ullSumG2+=rgbqTable[i].rgbGreen*ullFrequency[i];
					ullSumB2+=rgbqTable[i].rgbBlue*ullFrequency[i];
				}

			}


			if((ullNum1==0)||(ullNum2==0))
			{
				SAFE_DELETE(iLabel);

				BYTE byR;
				BYTE byG;
				BYTE byB;

				GetAverageValue(rgbqTable, iLength, &byR, &byG, &byB);

				byR1=max(0,(int)byR-1);
				byG1=max(0,(int)byG-1);
				byB1=max(0,(int)byB-1);
				byR2=min(255,(int)byR+1);
				byG2=min(255,(int)byG+1);
				byB2=min(255,(int)byB+1);

				return K_mean_RGB_2Class(rgbqTable, ullFrequency, iLength, byR1, byG1, byB1, byR2, byG2, byB2, pbyR1_out, pbyG1_out, pbyB1_out, pbyR2_out, pbyG2_out, pbyB2_out);
			}

			byR1=int(ullSumR1/(ullNum1*1.0)+0.5);
			byG1=int(ullSumG1/(ullNum1*1.0)+0.5);
			byB1=int(ullSumB1/(ullNum1*1.0)+0.5);

			byR2=int(ullSumR2/(ullNum2*1.0)+0.5);
			byG2=int(ullSumG2/(ullNum2*1.0)+0.5);
			byB2=int(ullSumB2/(ullNum2*1.0)+0.5);
			if(bChanged==false){break;}
		}

		SAFE_DELETE(iLabel);
		*pbyR1_out=byR1;
		*pbyG1_out=byG1;
		*pbyB1_out=byB1;
		*pbyR2_out=byR2;
		*pbyG2_out=byG2;
		*pbyB2_out=byB2;
		return true;
	}

	bool K_mean_RGB(const RGBQUAD* rgbqTable, const ULONGLONG* ullFrequency, const int iLength, RGBQUAD* rgbqTable_classed, const int iClassNum)
	{
		UINT* uiDistanceSqs;
		uiDistanceSqs=new UINT[iClassNum];

		int* iClasses;
		iClasses=new int[iLength];

		ULONGLONG* ullNums;
		ullNums=new ULONGLONG[iClassNum];

		ULONGLONG* ullSumRs;
		ULONGLONG* ullSumGs;
		ULONGLONG* ullSumBs;
		ullSumRs=new ULONGLONG[iClassNum];
		ullSumGs=new ULONGLONG[iClassNum];
		ullSumBs=new ULONGLONG[iClassNum];

		ULONGLONG ullNearestClassFistanceSq=256*256*256+1;

		for(int iClass1=0; iClass1<iClassNum-1; iClass1++)
		{
			for(int iClass2=iClass1+1; iClass2<iClassNum; iClass2++)
			{
				ULONGLONG uiDistanceSq = GetRGBDistanceSq(rgbqTable_classed[iClass1], rgbqTable_classed[iClass2]);
				if(ullNearestClassFistanceSq>uiDistanceSq){ullNearestClassFistanceSq=uiDistanceSq;}
			}
		}

		while(1)
		{
			for(int iClass=0; iClass<iClassNum; iClass++)
			{
				ullNums[iClass]=0;
				ullSumRs[iClass]=0;
				ullSumGs[iClass]=0;
				ullSumBs[iClass]=0;
			}

			bool bChanged = false;

			for(int i=0; i<iLength; i++)
			{
				int iDirectMachClass = -1;
				for(int iClass=0; iClass<iClassNum; iClass++)
				{
					uiDistanceSqs[iClass]=GetRGBDistanceSq(rgbqTable[i], rgbqTable_classed[iClass]);
					if(uiDistanceSqs[iClass]<ullNearestClassFistanceSq/4){iDirectMachClass = iClass; break;}
				}

				if(iDirectMachClass >= 0)
				{
					ullNums[iDirectMachClass]+=ullFrequency[i];
					if(iClasses[i] != iDirectMachClass){bChanged = true;}
					iClasses[i]=iDirectMachClass;
					continue;
				}

				int iMinClass=0;
				UINT uiMinDistanceSq=256*256*256+1;

				for(int iClass=0; iClass<iClassNum; iClass++)
				{
					if(uiDistanceSqs[iClass] < uiMinDistanceSq)
					{
						uiMinDistanceSq=uiDistanceSqs[iClass];
						iMinClass=iClass;
					}
				}
				ullNums[iMinClass]+=ullFrequency[i];
				if(iClasses[i] != iMinClass){bChanged = true;}
				iClasses[i]=iMinClass;
			}


			for(int i=0; i<iLength; i++)
			{
				ullSumRs[iClasses[i]]+=rgbqTable[i].rgbRed*ullFrequency[i];
				ullSumGs[iClasses[i]]+=rgbqTable[i].rgbGreen*ullFrequency[i];
				ullSumBs[iClasses[i]]+=rgbqTable[i].rgbBlue*ullFrequency[i];
			}


			for(int iClass=0; iClass<iClassNum; iClass++)
			{
				rgbqTable_classed[iClass].rgbRed=int(ullSumRs[iClass]/(ullNums[iClass]*1.0)+0.5);
				rgbqTable_classed[iClass].rgbGreen=int(ullSumGs[iClass]/(ullNums[iClass]*1.0)+0.5);
				rgbqTable_classed[iClass].rgbBlue=int(ullSumBs[iClass]/(ullNums[iClass]*1.0)+0.5);
			}

			if(bChanged==false){break;}
		}
		SAFE_DELETE(ullSumRs);
		SAFE_DELETE(ullSumGs);
		SAFE_DELETE(ullSumBs);
		SAFE_DELETE(iClasses);
		SAFE_DELETE(ullNums);
		SAFE_DELETE(uiDistanceSqs);
		return true;
	}
	bool GetLargestDeviationClass(const RGBQUAD* rgbqTable, const ULONGLONG* ullFrequency, int* iClasses, const int iLength, const RGBQUAD* rgbqTable_classed, const int iClassNum, int* iMaxDeviationClass)
	{
		ULONGLONG* ullSumDistSq;
		ULONGLONG* ullCount;
		ullSumDistSq = new ULONGLONG[iClassNum];
		ullCount = new ULONGLONG[iClassNum];
		for(int iClass=0; iClass<iClassNum; iClass++)
		{
			ullSumDistSq[iClass]=0;ullCount[iClass]=0;
		}

		for(int i=0; i<iLength; i++)
		{
			UINT uiMinDistanceSq=255*255*255;
			int iMinDistanceClass=0;
			for(int iClass=0; iClass<iClassNum; iClass++)
			{
				UINT uiDistanceSq = GetRGBDistanceSq(rgbqTable[i], rgbqTable_classed[iClass].rgbRed, rgbqTable_classed[iClass].rgbGreen, rgbqTable_classed[iClass].rgbBlue);
				if(uiDistanceSq < uiMinDistanceSq){uiMinDistanceSq=uiDistanceSq;iMinDistanceClass=iClass;}
			}
			iClasses[i]=iMinDistanceClass;
			ullSumDistSq[iMinDistanceClass]+=uiMinDistanceSq;
			ullCount[iMinDistanceClass]+=ullFrequency[i];
		}

		double dDeviationMax=0;
		for(int iClass=0; iClass<iClassNum; iClass++)
		{
			double dDeviation=ullSumDistSq[iClass]/(1.0*ullCount[iClass]);
			if(dDeviation>dDeviationMax){dDeviationMax=dDeviation;*iMaxDeviationClass=iClass;}
		}
		SAFE_DELETE(ullSumDistSq);
		SAFE_DELETE(ullCount);
		return true;
	}

	bool CalcDeviationForEachClass(const RGBQUAD* rgbqTable, const ULONGLONG* ullFrequency, int* iClasses, const int iLength, const RGBQUAD* rgbqTable_classed, const int iClassNum, double* dDeviations, int* iMaxDeviationClass)
	{
		ULONGLONG* ullSumDistSq;
		ULONGLONG* ullCount;
		ullSumDistSq = new ULONGLONG[iClassNum];
		ullCount = new ULONGLONG[iClassNum];
		for(int iClass=0; iClass<iClassNum; iClass++)
		{
			ullSumDistSq[iClass]=0;
			ullCount[iClass]=0;
		}

		for(int i=0; i<iLength; i++)
		{
			UINT uiMinDistanceSq=255*255*255;
			int iMinDistanceClass=0;
			for(int iClass=0; iClass<iClassNum; iClass++)
			{
				UINT uiDistanceSq = GetRGBDistanceSq(rgbqTable[i], rgbqTable_classed[iClass].rgbRed, rgbqTable_classed[iClass].rgbGreen, rgbqTable_classed[iClass].rgbBlue);
				if(uiDistanceSq < uiMinDistanceSq){uiMinDistanceSq=uiDistanceSq;iMinDistanceClass=iClass;}
			}
			iClasses[i]=iMinDistanceClass;
			ullSumDistSq[iMinDistanceClass]+=uiMinDistanceSq;
			ullCount[iMinDistanceClass]+=ullFrequency[i];
		}

		double dDeviationMax=0;
		for(int iClass=0; iClass<iClassNum; iClass++)
		{
			if(ullCount[iClass]==0){dDeviations[iClass]=-1; continue;}
			dDeviations[iClass]=ullSumDistSq[iClass]/(1.0*ullCount[iClass]);
			if(dDeviations[iClass]>dDeviationMax){dDeviationMax=dDeviations[iClass];*iMaxDeviationClass=iClass;}
		}
		SAFE_DELETE(ullSumDistSq);
		SAFE_DELETE(ullCount);
		return true;
	}

	bool GetCluster_K_mean(const RGBQUAD* rgbqTable, ULONGLONG* ullFrequency, int* iClaaes_out, const int iLength, RGBQUAD* rgbqResult, const int iMaxClassNum)
	{
		RGBQUAD* rgbqTable_classed;
		rgbqTable_classed = new RGBQUAD[iMaxClassNum];

		RGBQUAD* rgbqTable_temp;
		rgbqTable_temp = new RGBQUAD[iLength];

		ULONGLONG* ullFrequency_temp;
		ullFrequency_temp = new ULONGLONG[iLength];

		int* iClasses;
		iClasses=new int[iLength];

		for(int iClass=0; iClass<iLength; iClass++){iClasses[iClass]=0;}

		int iTargetClass=0;

		for(int iClassNum=1; iClassNum<iMaxClassNum; iClassNum++)
		{
			int iLength_temp=0;
			for(int i=0; i<iLength; i++)
			{
				if(iClasses[i] != iTargetClass){continue;}

				rgbqTable_temp[iLength_temp].rgbRed=rgbqTable[i].rgbRed;
				rgbqTable_temp[iLength_temp].rgbGreen=rgbqTable[i].rgbGreen;
				rgbqTable_temp[iLength_temp].rgbBlue=rgbqTable[i].rgbBlue;
				rgbqTable_temp[iLength_temp].rgbReserved=rgbqTable[i].rgbReserved;
				ullFrequency_temp[iLength_temp]=ullFrequency[i];
				iLength_temp++;
			}


			BYTE byR_Max=0;
			BYTE byG_Max=0;
			BYTE byB_Max=0;
			BYTE byR_Min=255;
			BYTE byG_Min=255;
			BYTE byB_Min=255;
			GetMinMaxValue(rgbqTable_temp, iLength_temp,&byR_Min,&byG_Min,&byB_Min,&byR_Max,&byG_Max,&byB_Max);

			BYTE byR1;
			BYTE byG1;
			BYTE byB1;
			BYTE byR2;
			BYTE byG2;
			BYTE byB2;
			K_mean_RGB_2Class(rgbqTable_temp, ullFrequency_temp, iLength_temp, byR_Min, byG_Min, byB_Min, byR_Max, byG_Max, byB_Max, &byR1, &byG1, &byB1, &byR2, &byG2, &byB2);

			rgbqTable_classed[iTargetClass].rgbRed=byR1;
			rgbqTable_classed[iTargetClass].rgbGreen=byG1;
			rgbqTable_classed[iTargetClass].rgbBlue=byB1;
			rgbqTable_classed[iTargetClass].rgbReserved=0;
			rgbqTable_classed[iClassNum].rgbRed=byR2;
			rgbqTable_classed[iClassNum].rgbGreen=byG2;
			rgbqTable_classed[iClassNum].rgbBlue=byB2;
			rgbqTable_classed[iClassNum].rgbReserved=0;

			K_mean_RGB(rgbqTable, ullFrequency, iLength,rgbqTable_classed, iClassNum+1);
			int iMaxDeviationClass=0;

			GetLargestDeviationClass(rgbqTable, ullFrequency, iClasses, iLength, rgbqTable_classed, iClassNum+1, &iMaxDeviationClass);

			iTargetClass=iMaxDeviationClass;
		}

		SAFE_DELETE(rgbqTable_temp);
		SAFE_DELETE(ullFrequency_temp);
		for(int i=0; i<iLength; i++)
		{
			iClaaes_out[i]=iClasses[i];
		}
		SAFE_DELETE(iClasses);
		for(int i=0; i<iMaxClassNum; i++)
		{
			rgbqResult[i].rgbRed=rgbqTable_classed[i].rgbRed;
			rgbqResult[i].rgbGreen=rgbqTable_classed[i].rgbGreen;
			rgbqResult[i].rgbBlue=rgbqTable_classed[i].rgbBlue;
			rgbqResult[i].rgbReserved=rgbqTable_classed[i].rgbReserved;
		}
		SAFE_DELETE(rgbqTable_classed);

		return true;
	}

	bool ConvertImage_ByDeviation(const CImage* imgSrc, const int iBPP, CImage* imgDst)
	{
		if(iBPP>=24)
		{
			return ConvertImage_LossLess(imgSrc, iBPP, imgDst);
		}
		int iBPP_src = imgSrc->GetBPP();
		if(iBPP_src==iBPP){return CopyImage(imgSrc,imgDst);}

		int iWidth = imgSrc->GetWidth();
		int iHeight = imgSrc->GetHeight();
		ImgRGB imgRGB;
		ConvertImage(imgSrc,&imgRGB);

		RGBQUAD* rgbqTable;
		rgbqTable = new RGBQUAD[iWidth*iHeight];
		ULONGLONG* ullFrequency;
		ullFrequency = new ULONGLONG[iWidth*iHeight];
		int iColors;
		MakeColorTable(&imgRGB, rgbqTable, ullFrequency, iWidth*iHeight, &iColors, NULL);

		int* iIndex;
		int* iConversionTable;
		iIndex=new int[iColors];
		iConversionTable=new int[iColors];

		RGBQUAD* rgbqResult;
		rgbqResult=new RGBQUAD[1ull<<min(24, iBPP)];

		GetCluster_K_mean(rgbqTable, ullFrequency, iConversionTable, iColors, rgbqResult, 1<<min(24, iBPP));


		if (imgDst->IsNull() != true) {imgDst->Destroy();}
		imgDst->Create(iWidth, iHeight,iBPP);
		SetColorTable(imgDst, rgbqResult, 1<<min(24, iBPP));
		SAFE_DELETE(rgbqResult);

		int iPitch_dst = imgDst->GetPitch();
		BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
		for(int r=0; r<iHeight; r++)
		{
			for(int c=0; c< iWidth/(8/iBPP); c++)
			{
				pbyDataDst[r*iPitch_dst+c]=0;
			}
		}


		for(int r=0; r<iHeight; r++)
		{
			for(int c=0; c< iWidth; c++)
			{
				int iColorTableIndex=GetColorTableIndex(rgbqTable,iColors, imgRGB.byImgR[r*iWidth+c], imgRGB.byImgG[r*iWidth+c], imgRGB.byImgB[r*iWidth+c], 0);

				int iDigit = (8/iBPP)-(c%(8/iBPP))-1;
				pbyDataDst[r*iPitch_dst+c/(8/iBPP)]+=(iConversionTable[iColorTableIndex]<<(iBPP*iDigit));
			}
		}
		SAFE_DELETE(rgbqTable);
		SAFE_DELETE(ullFrequency);
		SAFE_DELETE(iIndex);
		SAFE_DELETE(iConversionTable);

		return true;
	}


	bool ConvertImage(const ImgRGB* imgRGB, CImage* imgDst, const int iBPPDst, const RGBQUAD* rgbqTable, const int iColors)
	{
		int iHeight = imgRGB->iHeight;
		int iWidth = imgRGB->iWidth;

		if(imgDst->IsNull()!=false){imgDst->Destroy();}
		imgDst->Create(iWidth,iHeight, iBPPDst);
		int iPitch_dst = imgDst->GetPitch();
		BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();

		SetColorTable(imgDst,rgbqTable,iColors);
		for(int r=0; r<iHeight; r++)
		{
			for(int c=0; c< iWidth; c++)
			{
				int iColorTableIndex=GetColorTableIndex(rgbqTable,iColors, imgRGB->byImgR[r*iWidth+c], imgRGB->byImgG[r*iWidth+c], imgRGB->byImgB[r*iWidth+c], 0);

				int iDigit = (8/iBPPDst)-(c%(8/iBPPDst))-1;
				pbyDataDst[r*iPitch_dst+c/(8/iBPPDst)]+=(iColorTableIndex<<(iBPPDst*iDigit));
			}
		}
		return true;
	}

#include "float.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

	UINT CountIconNum(const CString sFilePath)
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);


		UINT nIcons = ExtractIconEx(sFilePath, -1, NULL, NULL, 1);
		Gdiplus::GdiplusShutdown(gdiplusToken);
		return nIcons;
	}


	bool ConvertIconToImg(const HDC hScreenDC, const HICON hIcon, CImage* img)
	{
		if (hIcon==NULL)
		{
			return false;
		}

		ICONINFO ii = {};
		BOOL bRet = ::GetIconInfo(hIcon, &ii);
		if (bRet != TRUE)
		{
			return false;
		}

		BITMAP bmColor = {};
		BITMAP bmMask  = {};
		::GetObject(ii.hbmColor, sizeof(BITMAP), &bmColor);
		::GetObject(ii.hbmMask,  sizeof(BITMAP), &bmMask);

		const int iWidth  = bmColor.bmWidth;
		const int iHeight = bmColor.bmHeight;
		const int iBPP    = bmColor.bmBitsPixel;

		if(img->IsNull() != true){img->Destroy();}

		bRet = img->Create(iWidth, iHeight, 32);
		if (bRet != TRUE)
		{
			::DeleteObject(ii.hbmColor);
			::DeleteObject(ii.hbmMask);
			return false;
		}

		BYTE* bmiColorBuf;
		if(iBPP<=8)
		{
			bmiColorBuf = new BYTE[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1ull<<iBPP)];
		}
		else
		{
			bmiColorBuf = new BYTE[sizeof(BITMAPINFOHEADER)];
		}

		BITMAPINFO* pbmiColor = (BITMAPINFO*)bmiColorBuf;
		pbmiColor->bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		pbmiColor->bmiHeader.biWidth       = iWidth;
		pbmiColor->bmiHeader.biHeight      = -iHeight;
		pbmiColor->bmiHeader.biPlanes      = 1;
		pbmiColor->bmiHeader.biBitCount    = (WORD)iBPP;
		pbmiColor->bmiHeader.biCompression = BI_RGB;

		const int iDataPitch_Src = ((iWidth * iBPP + 31) / 32) * 4;
		BYTE* byColor;
		byColor = new BYTE[iDataPitch_Src * iHeight];

		int iRet = ::GetDIBits(hScreenDC, ii.hbmColor, 0, iHeight, byColor, pbmiColor, DIB_RGB_COLORS);
		if (iRet==0)
		{
			img->Destroy();
			::DeleteObject(ii.hbmColor);
			::DeleteObject(ii.hbmMask);
			SAFE_DELETE(byColor);
			SAFE_DELETE(bmiColorBuf);
			return false;
		}

		BITMAPINFO bmiMask = {};


		BYTE bmiMaskBuf[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2] = {};
		BITMAPINFO* pBmiMask = (BITMAPINFO*)bmiMaskBuf ;

		pBmiMask->bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		pBmiMask->bmiHeader.biWidth       = iWidth;
		pBmiMask->bmiHeader.biHeight      = -iHeight;
		pBmiMask->bmiHeader.biPlanes      = 1;
		pBmiMask->bmiHeader.biBitCount    = 1;
		pBmiMask->bmiHeader.biCompression = BI_RGB;
		pBmiMask->bmiHeader.biClrUsed     = 2;
		pBmiMask->bmiHeader.biClrImportant= 2;

		int iPitchMask_Src = ((iWidth + 31) / 32) * 4;

		BYTE* byMask;
		byMask = new BYTE[iPitchMask_Src * iHeight];

		iRet = GetDIBits(hScreenDC, ii.hbmMask, 0, iHeight,byMask, pBmiMask, DIB_RGB_COLORS);
		if (iRet==0)
		{
			img->Destroy();
			::DeleteObject(ii.hbmColor);
			::DeleteObject(ii.hbmMask);
			SAFE_DELETE(byColor);
			SAFE_DELETE(byMask);
			SAFE_DELETE(bmiColorBuf);
			return false;
		}

		BYTE* pbyDataDst = (BYTE*)img->GetBits();
		int iPitch_Dst = img->GetPitch();

		if ((iBPP == 24) || (iBPP == 32))
		{
			int iColorPitch_Src = ((iBPP==24) ? 3 : 4);
			for (int iR = 0; iR < iHeight; iR++)
			{
				for (int iC = 0; iC < iWidth; iC++)
				{
					pbyDataDst[iR * iPitch_Dst +iC * 4 + 0] = byColor[iR * iDataPitch_Src + iC * iColorPitch_Src + 0];
					pbyDataDst[iR * iPitch_Dst +iC * 4 + 1] = byColor[iR * iDataPitch_Src + iC * iColorPitch_Src + 1];
					pbyDataDst[iR * iPitch_Dst +iC * 4 + 2] = byColor[iR * iDataPitch_Src + iC * iColorPitch_Src + 2];
					if((iBPP==32) && (byColor[iR * iDataPitch_Src + iC * iColorPitch_Src + 3]!=0))
					{
						pbyDataDst[iR * iPitch_Dst +iC * 4 + 3] = byColor[iR * iDataPitch_Src + iC * 4 + 3];
					}
					else
					{
						BYTE byMaskByte = byMask[iR * iPitchMask_Src + (iC / 8)];
						int iBitPos    = 7 - (iC % 8);
						pbyDataDst[iR * iPitch_Dst +iC * 4 + 3] = ((((byMaskByte >> iBitPos) & 0x01) == 0x01)) ? 0 : 255;
					}
				}
			}
		}
		else if(iBPP==8)
		{
			/*
			BYTE r,g,b,a;
			for (int iR = 0; iR < iHeight; iR++)
			{
			BYTE* pDest = (BYTE*)img->GetPixelAddress(0, iR);

			const BYTE* pSrcColor = byColor + iPitchColor_Src * iR;

			for (int iC = 0; iC < iWidth; iC++)
			{
			const BYTE* pSrc = pSrcColor + iC;
			b = g = r = *pSrc;

			const BYTE* pMaskLine = byMask+ iPitchMask_Src * iR;
			int bitIndex = iC;
			int byteIndex = bitIndex / 8;
			int bitPos    = 7 - (bitIndex % 8);
			BYTE maskByte = pMaskLine[byteIndex];
			bool isMasked = ((maskByte >> bitPos) & 0x1) != 0;

			a = isMasked ? 0 : 255;

			pDest[iC * 4 + 0] = b;
			pDest[iC * 4 + 1] = g;
			pDest[iC * 4 + 2] = r;
			pDest[iC * 4 + 3] = a;
			}
			}
			*/
		}
		else
		{
			/*
			BYTE r,g,b,a;
			for (int iR = 0; iR < iHeight; iR++)
			{
			BYTE* pDest = (BYTE*)img->GetPixelAddress(0, iR);

			const BYTE* pSrcColor = byColor + iPitchColor_Src * iR;

			for (int iC = 0; iC < iWidth; iC++)
			{
			// その他のフォーマットは簡易的に不透明扱い
			const BYTE* pSrc = pSrcColor + iC * iPitchColor_Src;

			pDest[iC * 4 + 0] =  pSrc[0];
			pDest[iC * 4 + 1] = (iPitchColor_Src > 1) ? pSrc[1] : pSrc[0];
			pDest[iC * 4 + 2] = (iPitchColor_Src > 2) ? pSrc[2] : pSrc[0];
			pDest[iC * 4 + 3] = 255;
			}
			}
			*/
		}



		SAFE_DELETE(bmiColorBuf);
		SAFE_DELETE(byMask);
		SAFE_DELETE(byColor);
		::DeleteObject(ii.hbmColor);
		::DeleteObject(ii.hbmMask);
		return true;
	}

	bool LoadICOFile(const CString sFilePath, PanImage* imgs, const UINT uiNum)
	{
		HICON* hLargeIcons=NULL;
		HICON* hSmallIcons=NULL;
		hLargeIcons = new HICON[uiNum];
		hSmallIcons = new HICON[uiNum];

		UINT uiExtracted = ExtractIconEx(sFilePath, 0, hLargeIcons, hSmallIcons, uiNum);
		if (uiExtracted == 0){SAFE_DELETE(hLargeIcons); SAFE_DELETE(hSmallIcons); return false;}

		HDC hScreenDC = ::GetDC(NULL);
		if (hScreenDC == NULL)
		{
			for (UINT ui=0; ui<uiNum; ui++)
			{
				if (hLargeIcons[ui] != NULL){::DestroyIcon(hLargeIcons[ui]);}
				if (hSmallIcons[ui] != NULL){::DestroyIcon(hSmallIcons[ui]);}
			}
			SAFE_DELETE(hLargeIcons);
			SAFE_DELETE(hSmallIcons);
			return false;
		}

		for (UINT ui = 0; ui < uiExtracted; ui++)
		{
			CImage imgTemp;
			bool bRet = ConvertIconToImg(hScreenDC, hLargeIcons[ui], &imgTemp);
			CString sDataSource;
			sDataSource.Format(_T("%s: Large%d"), sFilePath, ui);
			imgs[ui].Set(IMAGE_TYPE_CIMAGE,NULL,NULL,0,0,&imgTemp,VALUE_IMAGE_RESCALE_0_TO_255, sDataSource);
			::DestroyIcon(hLargeIcons[ui]);
			imgs[ui].ResetProcessImage();
		}

		for (UINT ui = 0; ui < uiExtracted; ui++)
		{
			CImage imgTemp;
			bool bRet = ConvertIconToImg(hScreenDC, hSmallIcons[ui], &imgTemp);
			CString sDataSource;
			sDataSource.Format(_T("%s: Small%d"), sFilePath, ui);
			imgs[uiNum+ui].Set(IMAGE_TYPE_CIMAGE,NULL,NULL,0,0,&imgTemp,VALUE_IMAGE_RESCALE_0_TO_255, sDataSource);
			::DestroyIcon(hSmallIcons[ui]);
			imgs[ui].ResetProcessImage();
		}

		SAFE_DELETE(hLargeIcons);
		SAFE_DELETE(hSmallIcons);
		::ReleaseDC(NULL, hScreenDC);
		return true;

	}

	bool LoadICON2(const CString sFilePath, PanImage* imgs, const UINT uiNum)
	{
		HINSTANCE hExe;
		hExe = LoadLibrary(sFilePath);

		HRSRC hResource;
		hResource = FindResource(hExe, 
			_T("#1"), 
			RT_GROUP_ICON); 

		HGLOBAL hMem;  
		hMem = LoadResource(hExe, hResource); 

		BYTE *lpResource;
		lpResource =(BYTE*) LockResource(hMem); 

		int nID = LookupIconIdFromDirectoryEx((PBYTE) lpResource, TRUE, SM_CXICON, SM_CYICON, LR_DEFAULTCOLOR); 

		hResource = FindResource(hExe, 
			MAKEINTRESOURCE(nID), 
			MAKEINTRESOURCE(RT_ICON)); 

		hMem = LoadResource(hExe, hResource); 

		lpResource = (BYTE*)LockResource(hMem); 

		HICON hIcon1;
		hIcon1 = CreateIconFromResourceEx((PBYTE) lpResource, 
			SizeofResource(hExe, hResource), TRUE, 0x00030000, 
			SM_CXICON, SM_CYICON, LR_DEFAULTCOLOR); 

		HDC hScreenDC = ::GetDC(NULL);
		bool bRet = ConvertIconToImg(hScreenDC, hIcon1, &(imgs[0].cImage));
		return true;
	}

	bool PanImage::Convert(const VALUE_IMAGE enumMode, CImage* imgDst)
	{
		if(enumImageType==IMAGE_TYPE_CIMAGE){return CopyImage(&(this->cImage), imgDst);}

		if(enumMode == VALUE_IMAGE_RESCALE_0_TO_255)
		{
			imgDst->Create(iWidth, iHeight, 8);

			RGBQUAD colorTable[256];
			for(int i=0; i<256; i++)
			{
				colorTable[i].rgbBlue=i;
				colorTable[i].rgbGreen=i;
				colorTable[i].rgbRed=i;
				colorTable[i].rgbReserved=0;
			}
			SetColorTable(imgDst, colorTable, 256);
			BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
			int iPitch = imgDst->GetPitch();
			switch(enumImageType)
			{
			case IMAGE_TYPE_IIMAGE:
				{
					int iMax=INT_MIN;
					int iMin=INT_MAX;
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							iMax=max(iMax,iImage[r*iWidth+c]);
							iMin=min(iMin,iImage[r*iWidth+c]);
						}
					}
					if(iMin==iMax){for(int r=0; r<iHeight; r++){for(int c=0; c<iWidth; c++){pbyDataDst[r*iPitch+c]=255;}return true;}}

					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							pbyDataDst[r*iPitch+c]=min(255,max(0, int(255 * (iImage[r*iWidth+c] - iMin)/double(iMax-iMin)) ));
						}
					}
					return true;
				}
			case IMAGE_TYPE_DIMAGE:
				{
					double dMax=DBL_MIN;
					double dMin=DBL_MAX;
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							dMax=max(dMax,dImage[r*iWidth+c]);
							dMin=min(dMin,dImage[r*iWidth+c]);
						}
					}
					if(dMin==dMax){for(int r=0; r<iHeight; r++){for(int c=0; c<iWidth; c++){pbyDataDst[r*iPitch+c]=255;}return true;}}

					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							pbyDataDst[r*iPitch+c]=min(255,max(0, 255 * int((dImage[r*iWidth+c] - dMin)/double(dMax-dMin)) ));
						}
					}
					return true;
				}
			}
			return false;
		}
		if(enumMode == VALUE_IMAGE_CLIP_0_TO_255)
		{
			imgDst->Create(iWidth, iHeight, 8);

			RGBQUAD colorTable[256];
			for(int i=0; i<256; i++)
			{
				colorTable[i].rgbBlue=i;
				colorTable[i].rgbGreen=i;
				colorTable[i].rgbRed=i;
				colorTable[i].rgbReserved=0;
			}
			SetColorTable(imgDst, colorTable, 256);

			BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
			int iPitch = imgDst->GetPitch();
			switch(enumImageType)
			{
			case IMAGE_TYPE_IIMAGE:
				{
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							pbyDataDst[r*iPitch+c]=min(255,max(0, iImage[r*iWidth+c]));
						}
					}
					return true;
				}
			case IMAGE_TYPE_DIMAGE:
				{
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							pbyDataDst[r*iPitch+c]=(BYTE)min(255,max(0, dImage[r*iWidth+c]));
						}
					}
				}
			}
		}
		if(enumMode == VALUE_IMAGE_RAINBOW)
		{
			imgDst->Create(iWidth, iHeight,24);

			BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();
			int iPitch = imgDst->GetPitch();
			switch(enumImageType)
			{
			case IMAGE_TYPE_IIMAGE:
				{
					int iMax=INT_MIN;
					int iMin=INT_MAX;
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							iMax=max(iMax,iImage[r*iWidth+c]);
							iMin=min(iMin,iImage[r*iWidth+c]);
						}
					}
					if(iMin==iMax){for(int r=0; r<iHeight; r++){for(int c=0; c<iWidth; c++){pbyDataDst[r*iPitch+c]=255;}return true;}}

					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							int uiValue = int(6910 * (iImage[r*iWidth+c] - iMin)/double(iMax-iMin));
							bool bRet = SetHSVValue(pbyDataDst, iPitch, r, c, uiValue, 32);
						}
					}
					return true;
				}
			case IMAGE_TYPE_DIMAGE:
				{
				}
			}
		}
		return false;
	}


	bool PanImage::GetValue(const int r, const int c, double* dValue) const
	{
		if(r<0){return false;}
		if(c<0){return false;}
		if(r>=iHeight){return false;}
		if(c>=iWidth){return false;}
		if(dImage == NULL){return false;}
		if(enumImageType != IMAGE_TYPE_DIMAGE){return false;}
		*dValue = dImage[r*iWidth+c];
		return true;
	}
	bool PanImage::GetValue(const int r, const int c, int* iValue) const
	{
		if(r<0){return false;}
		if(c<0){return false;}
		if(r>=iHeight){return false;}
		if(c>=iWidth){return false;}
		if(iImage == NULL){return false;}
		if(enumImageType != IMAGE_TYPE_IIMAGE){return false;}
		*iValue = iImage[r*iWidth+c];
		return true;
	}
	bool PanImage::GetValue(const int r, const int c, RGBQUAD* rgbValue, BYTE* pbyAlpha, int* iBPP_out) const
	{
		if(r<0){return false;}
		if(c<0){return false;}
		if(r>=iHeight){return false;}
		if(c>=iWidth){return false;}
		if(enumImageType != IMAGE_TYPE_CIMAGE){return false;}


		COLORREF col = cImage.GetPixel(c,r);
		rgbValue->rgbBlue=GetBValue(col);
		rgbValue->rgbGreen=GetGValue(col);
		rgbValue->rgbRed=GetRValue(col);
		rgbValue->rgbReserved = 0;

		int iBPP =cImage.GetBPP();
		if(iBPP == 32)
		{
			BYTE* pByData = (BYTE*)cImage.GetBits();
			int iPitch = cImage.GetPitch();
			*pbyAlpha= pByData[r * iPitch +c *4+3];
		}
		else
		{
			*pbyAlpha=255;
		}

		*iBPP_out=iBPP;
		return true;
	}

	bool PanImage::Set(const IMAGE_TYPE enumImageType, const int* piImage_in, const double* pdImage_in, const int iWidth, const int iHeight, const CImage* pcImage_in, const VALUE_IMAGE enumValueImage_in, const CString sDataSource_in)
	{
		this->Init();
		switch(enumImageType)
		{
		case IMAGE_TYPE_CIMAGE:
			{
				if(pcImage_in == NULL){return false;}
				bool bRet = CopyImage(pcImage_in, &(this->cImage));
				if(bRet != true){return false;}
				this->enumImageType = IMAGE_TYPE_CIMAGE;
				this->iWidth=this->cImage.GetWidth();
				this->iHeight=this->cImage.GetHeight();
				this->enumValueImage = VALUE_IMAGE_UNDEFINED;
				this->sDataSource.Format(_T("%s"), (LPCTSTR)sDataSource_in);
				return true;
			}
		case IMAGE_TYPE_IIMAGE:
			{
				if(piImage_in == NULL){return false;}
				if(iWidth<=0){return false;}
				if(iHeight<=0){return false;}
				this->iImage=new int[iWidth*iHeight];
				this->iWidth=iWidth;
				this->iHeight=iHeight;
				this->enumImageType=IMAGE_TYPE_IIMAGE;
				this->enumValueImage = enumValueImage_in;
				for(int r=0; r<iHeight; r++)
				{
					for(int c=0; c<iWidth; c++)
					{
						this->iImage[r*iWidth+c]=piImage_in[r*iWidth+c];
					}
				}
				this->sDataSource.Format(_T("%s"), (LPCTSTR)sDataSource_in);
				Convert(enumValueImage, &(this->cImage));
				return true;
			}
		case IMAGE_TYPE_DIMAGE:
			{
				if(pdImage_in == NULL){return false;}
				if(iWidth<=0){return false;}
				if(iHeight<=0){return false;}
				this->dImage=new double[iWidth*iHeight];
				this->iWidth=iWidth;
				this->iHeight=iHeight;
				this->enumImageType=IMAGE_TYPE_DIMAGE;
				this->enumValueImage = enumValueImage_in;
				for(int r=0; r<iHeight; r++)
				{
					for(int c=0; c<iWidth; c++)
					{
						this->dImage[r*iWidth+c]=pdImage_in[r*iWidth+c];
					}
				}
				this->sDataSource.Format(_T("%s"), (LPCTSTR)sDataSource_in);
				Convert(enumValueImage, &(this->cImage));
				return true;
			}
		default:{return false;}
		}
		return false;
	}
