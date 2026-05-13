#include "stdafx.h"
#include "ImageProc.h"
#include "SImgProc_ex.h"
#include "CommonFunction.h"
#include "math.h"

bool SetColorTable(CImage* img, const RGBQUAD* rgbTable, int iLength)
{
	if(img==NULL){return false;}
	if(rgbTable==NULL){return false;}
	if(iLength<0){return false;}
	if(iLength>=256){return false;}
	if(img->IsNull()==true){return false;}

	HDC hDC = img->GetDC();
	SetDIBColorTable(hDC, 0, iLength, rgbTable);
	img->ReleaseDC();
	return true;
}

bool IsColorTableMonochrome(RGBQUAD* rgbTable, int iLength)
{
	for(int i=0; i< iLength; i++)
	{
		if(rgbTable[i].rgbBlue != rgbTable[i].rgbGreen){return false;} 
		if(rgbTable[i].rgbBlue != rgbTable[i].rgbRed){return false;}
	}
	return true;
}
bool IsImageMonochrome(const CImage* imgSrc)
{
	int iBPP = imgSrc->GetBPP();

	switch(iBPP)
	{
	case 1:{return true;}
	case 2:
		{
			int iColors = imgSrc->GetMaxColorTableEntries();
			RGBQUAD* rgbqTable_src= new RGBQUAD[iColors];
			imgSrc->GetColorTable(0, iColors, rgbqTable_src);
			bool bMono = IsColorTableMonochrome(rgbqTable_src, iColors);
			SAFE_DELETE(rgbqTable_src);
			return bMono;
		}
	case 4:
		{
			int iColors = imgSrc->GetMaxColorTableEntries();
			RGBQUAD* rgbqTable_src= new RGBQUAD[iColors];
			imgSrc->GetColorTable(0, iColors, rgbqTable_src);
			bool bMono = IsColorTableMonochrome(rgbqTable_src, iColors);
			SAFE_DELETE(rgbqTable_src);
			return bMono;
		}
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
		{
			BYTE* byData_src = (BYTE*)imgSrc->GetBits();
			int iWidth = imgSrc->GetWidth();
			int iHeight= imgSrc->GetHeight();
			int iPitch = imgSrc->GetPitch();
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c< iWidth; c++)
				{
					if(byData_src[r*iPitch+c*3+0] != byData_src[r*iPitch+c*3+1]){return false;}
					if(byData_src[r*iPitch+c*3+0] != byData_src[r*iPitch+c*3+2]){return false;}
				}
			}
			return true;
		}
	case 32:
		{
			BYTE* byData_src = (BYTE*)imgSrc->GetBits();
			int iWidth = imgSrc->GetWidth();
			int iHeight= imgSrc->GetHeight();
			int iPitch = imgSrc->GetPitch();
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c< iWidth; c++)
				{
					if(byData_src[r*iPitch+c*4+0] != byData_src[r*iPitch+c*4+1]){return false;}
					if(byData_src[r*iPitch+c*4+0] != byData_src[r*iPitch+c*4+2]){return false;}
				}
			}
			return true;
		}
	}
	return false;
}

bool CopyImage(const CImage* imgSrc, CImage* imgDst)
{

	int iWidth = imgSrc->GetWidth();
	int iHeight = imgSrc->GetHeight();
	if (imgDst->IsNull() != true) {imgDst->Destroy();}

	if (imgSrc->IsNull()) {return false;}

	HRESULT hr = imgDst->Create(imgSrc->GetWidth(),imgSrc->GetHeight(),imgSrc->GetBPP());
	if (FAILED(hr)) {return false;}
	BYTE* pbyDataSrc = (BYTE*)imgSrc->GetBits();
	BYTE* pbyDataDst = (BYTE*)imgDst->GetBits();

	int iBPP = imgSrc->GetBPP();

	if((iBPP==2)||(iBPP==4)||(iBPP==8))
	{
		int iColors = imgSrc->GetMaxColorTableEntries();
		RGBQUAD* rgbqTable_src= new RGBQUAD[iColors];
		imgSrc->GetColorTable(0, iColors, rgbqTable_src);
		
SetColorTable(imgDst, rgbqTable_src, 256);

		SAFE_DELETE(rgbqTable_src);
	}

	int iPitch_stc = imgSrc->GetPitch();
	int iPitch_dst = imgDst->GetPitch();
	int iLineLength = abs(iPitch_stc);
	for(int r=0; r< imgSrc->GetHeight(); r++)
	{
		for(int c=0; c< iLineLength; c++)
		{
			pbyDataDst[r*iPitch_dst+c] = pbyDataSrc[r*iPitch_stc+c];
		}

	}


	return true;
}


bool ConvertImageToStr(const CImage* cImageSrc, const CString sSeparater, CString* sImage)
{
	int iWidth_src  = cImageSrc->GetWidth();
	int iHeight_src = cImageSrc->GetHeight();
	int iBPP_src= cImageSrc->GetBPP();
	int iPitch_stc = cImageSrc->GetPitch();
	BYTE* bySrcData = (BYTE*)cImageSrc->GetBits();

	if(iWidth_src <= 0){return FALSE;}
	if(iHeight_src == 0){return FALSE;}
	if(iBPP_src == 0){return FALSE;}

	CString sImageLocal=_T("");
	if(iBPP_src==24)
	{
		for(int r=0; r<iHeight_src; r++)
		{
			CString sLine=_T("");;
			CString sPixel;
			for(int c=0; c<iWidth_src; c++)
			{
				sPixel.Format(_T("%d%s"),bySrcData[r*iPitch_stc+c*3+2],sSeparater);
				sLine+=sPixel;
			}
			sLine+=sSeparater;
			for(int c=0; c<iWidth_src; c++)
			{
				sPixel.Format(_T("%d%s"),bySrcData[r*iPitch_stc+c*3+1],sSeparater);
				sLine+=sPixel;
			}
			sLine+=sSeparater;
			for(int c=0; c<iWidth_src-1; c++)
			{
				sPixel.Format(_T("%d%s"),bySrcData[r*iPitch_stc+c*3+0],sSeparater);
				sLine+=sPixel;
			}
			sPixel.Format(_T("%d\n"),bySrcData[r*iPitch_stc+(iWidth_src-1)*3+0]);
			sLine+=sPixel;
			sImageLocal+=sLine;
		}
		(*sImage)=sImageLocal;
		return true;
	}

	if(iBPP_src==8)
	{
		RGBQUAD* pSrcTable=NULL;
		int iColors = cImageSrc->GetMaxColorTableEntries();
		if (iColors > 0) 
		{
			pSrcTable = new RGBQUAD[iColors];
			cImageSrc->GetColorTable(0, iColors, pSrcTable);
		}

		bool bGrayScale=true;
		for(int i=0; i<iColors; i++)
		{
			if(pSrcTable[i].rgbBlue != pSrcTable[i].rgbRed){bGrayScale=false; break;}
			if(pSrcTable[i].rgbBlue != pSrcTable[i].rgbGreen){bGrayScale=false; break;}
		}

		if(bGrayScale==true)
		{
			for(int r=0; r<iHeight_src; r++)
			{
				CString sLine=_T("");;
				CString sPixel;

				for(int c=0; c<iWidth_src-1; c++)
				{
					sPixel.Format(_T("%d%s"),(BYTE)(((RGBQUAD*)(&(pSrcTable[bySrcData[r*iPitch_stc+c]])))->rgbBlue),sSeparater);
					sLine+=sPixel;
				}
				sPixel.Format(_T("%d\n"),(BYTE)(((RGBQUAD*)(&(pSrcTable[bySrcData[r*iPitch_stc+(iWidth_src-1)]])))->rgbBlue));
				sLine+=sPixel;
				sImageLocal+=sLine;
			}
			(*sImage)=sImageLocal;
			delete [] pSrcTable;
			return true;
		}

		for(int r=0; r<iHeight_src; r++)
		{
			CString sLine=_T("");;
			CString sPixel;
			for(int c=0; c<iWidth_src; c++)
			{
				sPixel.Format(_T("%d%s"),(BYTE)(((RGBQUAD*)(&(pSrcTable[bySrcData[r*iPitch_stc+c]])))->rgbRed),sSeparater);
				sLine+=sPixel;
			}
			sLine+=sSeparater;
			for(int c=0; c<iWidth_src; c++)
			{
				sPixel.Format(_T("%d%s"),(BYTE)(((RGBQUAD*)(&(pSrcTable[bySrcData[r*iPitch_stc+c]])))->rgbGreen),sSeparater);
				sLine+=sPixel;
			}
			sLine+=sSeparater;
			for(int c=0; c<iWidth_src-1; c++)
			{
				sPixel.Format(_T("%d%s"),(BYTE)(((RGBQUAD*)(&(pSrcTable[bySrcData[r*iPitch_stc+c]])))->rgbBlue),sSeparater);
				sLine+=sPixel;
			}
			sPixel.Format(_T("%d\n"),(BYTE)(((RGBQUAD*)(&(pSrcTable[bySrcData[r*iPitch_stc+(iWidth_src-1)]])))->rgbBlue));
			sLine+=sPixel;
			sImageLocal+=sLine;
		}
		(*sImage)=sImageLocal;
		delete [] pSrcTable;
		return true;
	}


	if(iBPP_src==32)
	{
		for(int r=0; r<iHeight_src; r++)
		{
			CString sLine=_T("");;
			CString sPixel;
			for(int c=0; c<iWidth_src; c++)
			{
				sPixel.Format(_T("%d%s"),bySrcData[r*iPitch_stc+c*4+2],sSeparater);
				sLine+=sPixel;
			}
			sLine+=sSeparater;
			for(int c=0; c<iWidth_src; c++)
			{
				sPixel.Format(_T("%d%s"),bySrcData[r*iPitch_stc+c*4+1],sSeparater);
				sLine+=sPixel;
			}
			sLine+=sSeparater;
			for(int c=0; c<iWidth_src; c++)
			{
				sPixel.Format(_T("%d%s"),bySrcData[r*iPitch_stc+c*4+0],sSeparater);
				sLine+=sPixel;
			}
			sLine+=sSeparater;
			for(int c=0; c<iWidth_src-1; c++)
			{
				sPixel.Format(_T("%d%s"),bySrcData[r*iPitch_stc+c*4+3],sSeparater);
				sLine+=sPixel;
			}
			sPixel.Format(_T("%d\n"),bySrcData[r*iPitch_stc+(iWidth_src-1)*4+3]);
			sLine+=sPixel;
			sImageLocal+=sLine;
		}
		(*sImage)=sImageLocal;
		return true;
	}
	return true;
}

bool CreateZoomedImage(CImage* imgOriginal, CImage* imgZoomed, const int iZoomFactor, const int iCenterR, const int iCenterC)
{
	if (imgOriginal->IsNull() == true){return false;}

	int iImgWidth  = imgOriginal->GetWidth();
	int iImgHeight = imgOriginal->GetHeight();

	int iBPP = imgOriginal->GetBPP();

	if(iBPP==24)
	{
		ImgRGB imgRGBOld;
		ImgRGB imgRGB;
		ConvertImage(imgOriginal,&imgRGBOld);
		imgRGB.Set(iImgWidth, iImgHeight, CHANNEL_3_8RGB);
		for(int r=0; r<iImgHeight; r++)
		{
			for(int c=0; c<iImgWidth; c++)
			{
				imgRGB.byImgR[r*iImgWidth+c]=0;
				imgRGB.byImgG[r*iImgWidth+c]=0;
				imgRGB.byImgB[r*iImgWidth+c]=0;
			}
		}
		int iStartOffset_oldR=-iImgHeight/(2*iZoomFactor);
		int iStartOffset_oldC=-iImgWidth/(2*iZoomFactor);
		for(int iOffset_oldr=0; iOffset_oldr<iImgHeight/iZoomFactor; iOffset_oldr++)
		{
			int r_old=iCenterR+iStartOffset_oldR+iOffset_oldr;
			if(r_old<0){continue;}
			if(r_old>=iImgHeight-1){continue;}
			for(int iOffset_oldc=0; iOffset_oldc<iImgWidth/iZoomFactor; iOffset_oldc++)
			{
				int c_old=iCenterC+iStartOffset_oldC+iOffset_oldc;
				if(c_old<0){continue;}
				if(c_old>=iImgHeight-1){continue;}

				int iNewR=iOffset_oldr*iZoomFactor;
				int iNewC=iOffset_oldc*iZoomFactor;
				if(iNewR<0){continue;}
				if(iNewR>=iImgHeight-1){continue;}
				if(iNewC<0){continue;}
				if(iNewC>=iImgWidth-1){continue;}
				BYTE byR=imgRGBOld.byImgR[r_old*iImgWidth+c_old];
				BYTE byG=imgRGBOld.byImgG[r_old*iImgWidth+c_old];
				BYTE byB=imgRGBOld.byImgB[r_old*iImgWidth+c_old];

				for(int iOffset_newr=0; iOffset_newr<=iZoomFactor-1; iOffset_newr++)
				{
					for(int iOffset_newc=0; iOffset_newc<=iZoomFactor-1; iOffset_newc++)
					{
						imgRGB.byImgR[(iOffset_newr+iNewR)*iImgWidth+(iOffset_newc+iNewC)]=byR;
						imgRGB.byImgG[(iOffset_newr+iNewR)*iImgWidth+(iOffset_newc+iNewC)]=byG;
						imgRGB.byImgB[(iOffset_newr+iNewR)*iImgWidth+(iOffset_newc+iNewC)]=byB;
					}
				}
			}
		}
		ConvertImage(&imgRGB,imgZoomed);
	}

	return true;
}

bool ClipImage( CImage* imgOriginal, CImage* imgClipped, int iR0, int iC0, int iR1, int iC1)
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

	HRESULT hr = imgClipped->Create(iClipWidth, iClipHeight, iBPP_src);
	if (FAILED(hr)) {return false;}

	if (iBPP_src <= 8) 
	{
		int iColors = imgOriginal->GetMaxColorTableEntries();
		if (iColors > 0) 
		{
			RGBQUAD* pSrcTable = new RGBQUAD[iColors];
			imgOriginal->GetColorTable(0, iColors, pSrcTable);
			imgClipped->SetColorTable(0, iColors, pSrcTable);
			delete[] pSrcTable;
		}
	}

	int iPitch_stc = imgOriginal->GetPitch();
	int iPitch_dst = imgClipped->GetPitch();

	BYTE* bySrcData = (BYTE*)imgOriginal->GetBits();
	BYTE* byDstData = (BYTE*)imgClipped->GetBits();

	bool bSrcBottomUp = (iPitch_stc > 0);
	bool bDstBottomUp = (iPitch_dst > 0);

	int iSrcByPP = iBPP_src / 8;

	for (int r = 0; r < iClipHeight; r++) 
	{
		BYTE* pSrcLine = nullptr;
		BYTE* pDstLine = nullptr;

		if (bSrcBottomUp == true) 
		{
			pSrcLine = &(bySrcData[(iImgHeight - 1 - (iR0+r)) * iPitch_stc + iC0 * iSrcByPP]);
		}
		else 
		{
			pSrcLine = &(bySrcData[(iR0+r) * iPitch_stc + iC0 * iSrcByPP]);
		}

		if (bDstBottomUp == true)
		{
			pDstLine = &(byDstData[(iClipHeight - 1 - r) * iPitch_dst]);
		}
		else
		{
			pDstLine = &(byDstData[r * iPitch_dst]);
		}

		memcpy(pDstLine, pSrcLine, iClipWidth * iSrcByPP);
	}
	return false;
}

BOOL CopyToClipBoardImg(CImage* cImageSrc)
{
	if (cImageSrc->IsNull() == true){return false;}

	int iWidth_src = cImageSrc->GetWidth();
	int iHeight_src = cImageSrc->GetHeight();
	int iBPP_src = cImageSrc->GetBPP();

	int iHeaderSize = sizeof(BITMAPINFOHEADER);

	int iColors = 0;
	if (iBPP_src <= 8) 
	{
		iColors = cImageSrc->GetMaxColorTableEntries();
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
	bih->biHeight        = iHeight_src;
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
		cImageSrc->GetColorTable(0, iColors, rgbqTable);

		memcpy(pPalette, rgbqTable, iPaletteSize);
		delete[] rgbqTable;
	}


	BYTE* bySrcData = (BYTE*)cImageSrc->GetBits();
	int iPitch_stc   = cImageSrc->GetPitch();
	bool bBottomUp  = (iPitch_stc > 0);

	for (int r = 0; r < iHeight_src; r++) 
	{

		BYTE* pSrcLine = nullptr;
		BYTE* pDstLine = &(pBits[r * iBytesPerLine]);

		if (bBottomUp = true)
		{
			pSrcLine = &(bySrcData[(iHeight_src - 1 -r) * iPitch_stc]);
		} 
		else 
		{
			pSrcLine = &(bySrcData[r * iPitch_stc]);
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

BOOL CopyFromClipBoardImg(CImage* cImageDst)
{
	BOOL bRet;

	bRet = OpenClipboard(NULL);
	if(bRet == FALSE){return FALSE;}

	HANDLE hResult;
	hResult = GetClipboardData(CF_DIB);
	if(hResult == NULL){return FALSE;}

	LPVOID byDataTemp = GlobalLock(hResult);
	if(byDataTemp==NULL){return FALSE;}

	SIZE_T dataSize = GlobalSize(hResult);
	if (dataSize == 0) { GlobalUnlock(hResult);CloseClipboard();}

	BYTE* byData;
	byData = new BYTE[dataSize];

	memcpy(byData, byDataTemp, dataSize);

	GlobalUnlock(hResult);

	bRet = CloseClipboard();
	if(bRet == FALSE){SAFE_DELETE(byData); return FALSE;}


	BITMAPINFOHEADER* bih = (BITMAPINFOHEADER*)byData;

	int iWidth  = bih->biWidth;
	int iHeight = bih->biHeight;
	int iBPP_src= bih->biBitCount;

	if(iWidth <= 0){SAFE_DELETE(byData); return FALSE;}
	if(iHeight == 0){SAFE_DELETE(byData); return FALSE;}
	if(iBPP_src == 0){SAFE_DELETE(byData); return FALSE;}

	int iColors = 0;
	if (iBPP_src <= 8) 
	{
		iColors = bih->biClrUsed;
		if (iColors == 0) 
		{
			iColors = 1 << iBPP_src ;
		}
	}

	int iPaletteSize = iColors * sizeof(RGBQUAD);

	BYTE* pPalette = &(byData[sizeof(BITMAPINFOHEADER)]);
	BYTE* bySrcData;
	if((iBPP_src == 24) || (iBPP_src == 32))
	{
		if(dataSize == sizeof(BITMAPINFOHEADER) + iPaletteSize + iWidth*abs(iHeight)*iBPP_src/8)
		{
			bySrcData = &(byData[sizeof(BITMAPINFOHEADER)+iPaletteSize ]);
		}
		else
		{
			bySrcData = &(byData[dataSize - iWidth*abs(iHeight)*iBPP_src/8 ]);
		}
	}
	else
	{
		bySrcData = &(byData[sizeof(BITMAPINFOHEADER)+iPaletteSize ]);;
	}

	if(cImageDst->IsNull() != true){cImageDst->Destroy();}

	HRESULT hr = cImageDst->Create(iWidth, abs(iHeight), iBPP_src);
	if (FAILED(hr)) {SAFE_DELETE(byData); return FALSE;}

	if ((iBPP_src <= 8) && (iColors > 0))
	{
		cImageDst->SetColorTable(0, iColors, (RGBQUAD*)pPalette);
	}

	int iPitch_dst = cImageDst->GetPitch();
	BYTE* byDstData = (BYTE*)cImageDst->GetBits();

	bool bDstBottomUp = (iPitch_dst > 0);
	bool bSrcBottomUp = (iHeight > 0); 

	int iBytesPerLine = ((iWidth * iBPP_src + 31) / 32) * 4;
	int iAbsHeight = abs(iHeight);

	for (int r = 0; r < iAbsHeight; r++)
	{

		BYTE* pSrcLine = nullptr;
		BYTE* pDstLine = nullptr;

		if (bSrcBottomUp) 
		{
			pSrcLine = &(bySrcData[(iAbsHeight - 1 - r) * iBytesPerLine]);
		}
		else
		{
			pSrcLine = &(bySrcData[r * iBytesPerLine]);
		}

		if (bDstBottomUp) 
		{
			pDstLine = &(byDstData[(iAbsHeight - 1 - r) * iPitch_dst]);
		}
		else
		{
			pDstLine = &(byDstData[r * iPitch_dst]);
		}

		memcpy(pDstLine, pSrcLine, iBytesPerLine);
	}
	SAFE_DELETE(byData); 
	return TRUE;
}

BOOL ConvertImage(const CImage* cimage, ImgRGB* imgRGB)
{
	int iWidth_src = cimage->GetWidth();
	int iHeight_src = cimage->GetHeight();
	imgRGB->Set(iWidth_src,iHeight_src,CHANNEL_3_8RGB);

	int iBPP = cimage->GetBPP();
	BYTE* src = (BYTE*)cimage->GetBits();
	int iPitch_stc=cimage->GetPitch();
	int iIncR;
	if(iBPP==24)
	{
		for(int r=0; r<iHeight_src; r++)
		{
			for(int c=0; c<iWidth_src; c++)
			{
				imgRGB->byImgR[r*imgRGB->iWidth+c]=src[r*iPitch_stc+c*3+2];
				imgRGB->byImgG[r*imgRGB->iWidth+c]=src[r*iPitch_stc+c*3+1];
				imgRGB->byImgB[r*imgRGB->iWidth+c]=src[r*iPitch_stc+c*3+0];
			}
		}
		return TRUE;
	}

	if((iBPP==2)||(iBPP==4)||(iBPP==8))
	{
		RGBQUAD* pSrcTable=NULL;
		int iColors = cimage->GetMaxColorTableEntries();
		if (iColors > 0) 
		{
			pSrcTable = new RGBQUAD[iColors];
			cimage->GetColorTable(0, iColors, pSrcTable);
		}
		for(int r=0; r<iHeight_src; r++)
		{
			for(int c=0; c<iWidth_src; c++)
			{
				imgRGB->byImgR[r*imgRGB->iWidth+c]=(BYTE)(((RGBQUAD*)(&(pSrcTable[src[r*iPitch_stc+c]])))->rgbRed);
				imgRGB->byImgG[r*imgRGB->iWidth+c]=(BYTE)(((RGBQUAD*)(&(pSrcTable[src[r*iPitch_stc+c]])))->rgbGreen);
				imgRGB->byImgB[r*imgRGB->iWidth+c]=(BYTE)(((RGBQUAD*)(&(pSrcTable[src[r*iPitch_stc+c]])))->rgbBlue);
			}
		}
		if(pSrcTable != NULL){delete [] pSrcTable;}
		return TRUE;
	}

	if(iBPP==1) 
	{
		for(int r=0; r<iHeight_src; r++)
		{
			for(int c=0; c<iWidth_src; c++)
			{
				int iPosition= r*iPitch_stc+c/8;
				int iDigit = 8-(c%8)-1;
				BYTE byValue = (( (src[iPosition] & (1<< iDigit)) == (1<< iDigit)) ? 255 : 0);

				imgRGB->byImgR[r*imgRGB->iWidth+c]=byValue;
				imgRGB->byImgG[r*imgRGB->iWidth+c]=byValue;
				imgRGB->byImgB[r*imgRGB->iWidth+c]=byValue;

			}
		}
		return TRUE;
	}
	if(iBPP==32)
	{
		for(int r=0; r<iHeight_src; r++)
		{
			for(int c=0; c<iWidth_src; c++)
			{
				imgRGB->byImgR[r*imgRGB->iWidth+c]=src[r*iPitch_stc+c*4+2];
				imgRGB->byImgG[r*imgRGB->iWidth+c]=src[r*iPitch_stc+c*4+1];
				imgRGB->byImgB[r*imgRGB->iWidth+c]=src[r*iPitch_stc+c*4+0];
			}
		}
		return TRUE;
	}

	return TRUE;
}



BOOL ConvertImage(const ImgRGB* imgRGB, CImage* cImageDst)
{
	if(cImageDst->IsNull() != true){cImageDst->Destroy();}
	int iWidth_src = imgRGB->iWidth;;
	int iHeight_src= imgRGB->iHeight;;

	if(imgRGB->iChannel==CHANNEL_3_8RGB)
	{
		cImageDst->Create(iWidth_src, iHeight_src, 24);


		BYTE* byDstData = (BYTE*)cImageDst->GetBits();
		int iPitch_dst=cImageDst->GetPitch();
		int iIncR;
		for(int r=0; r<iHeight_src; r++)
		{
			for(int c=0; c<iWidth_src; c++)
			{
				byDstData[r*iPitch_dst+c*3+2]=imgRGB->byImgR[r*iWidth_src+c];
				byDstData[r*iPitch_dst+c*3+1]=imgRGB->byImgG[r*iWidth_src+c];
				byDstData[r*iPitch_dst+c*3+0]=imgRGB->byImgB[r*iWidth_src+c];
			}
		}
		return TRUE;
	}

	if(imgRGB->iChannel==CHANNEL_1_8)
	{
		cImageDst->Create(iWidth_src, iHeight_src, 8);

		RGBQUAD colorTable[256];
		for(int i=0; i<256; i++)
		{
			colorTable[i].rgbBlue=i;
			colorTable[i].rgbGreen=i;
			colorTable[i].rgbRed=i;
			colorTable[i].rgbReserved=255;
		}

		
SetColorTable(cImageDst, colorTable, 256);


		BYTE* byDstData = (BYTE*)cImageDst->GetBits();
		int iPitch_dst=cImageDst->GetPitch();
		int iIncR;
		for(int r=0; r<iHeight_src; r++)
		{
			for(int c=0; c<iWidth_src; c++)
			{
				byDstData[r*iPitch_dst+c]=imgRGB->byImg[r*iWidth_src+c];
			}
		}
		return TRUE;
	}

	return TRUE;
}


bool ExtractChannel_Gray(CImage* imgSrc, CImage* imgDst, ENUM_COLOR color)
{
	if(imgDst->IsNull() != true){imgDst->Destroy();}
	int iWidth = imgSrc->GetWidth();
	int iHeight = imgSrc->GetHeight();
	if(imgSrc->GetBPP()==8)
	{
		CopyImage(imgSrc,imgDst);
		return true;
	}

	imgDst->Create(iWidth, iHeight, 8);
	RGBQUAD colorTable[256];
	for(int i=0; i<256; i++)
	{
		colorTable[i].rgbBlue=i;
		colorTable[i].rgbGreen=i;
		colorTable[i].rgbRed=i;
		colorTable[i].rgbReserved=255;
	}

SetColorTable(imgDst, colorTable, 256);

	ImgRGB imgRGB;
	ConvertImage(imgSrc, &imgRGB);
	int iPitch_dst=imgDst->GetPitch();
	BYTE* byDstData = (BYTE*)imgDst->GetBits();
	switch(color)
	{
	case COLOR_RED_GRAY: 
		{
			int iIncR;
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					byDstData[r*iPitch_dst+c]=imgRGB.byImgR[r*iWidth+c];
				}
			}
			return true;
		}

	case COLOR_GREEN_GRAY: 
		{
			int iIncR;
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					byDstData[r*iPitch_dst+c]=imgRGB.byImgG[r*iWidth+c];
				}
			}
			return true;
		}
	case COLOR_BLUE_GRAY: 
		{
			int iIncR;
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					byDstData[r*iPitch_dst+c]=imgRGB.byImgB[r*iWidth+c];
				}
			}
			return true;
		}
	}

	return true;
}
bool ExtractChannel_Color(CImage* imgSrc, CImage* imgDst, ENUM_COLOR color)
{
	if(imgDst->IsNull() != true){imgDst->Destroy();}
	int iWidth = imgSrc->GetWidth();
	int iHeight = imgSrc->GetHeight();

	ImgRGB imgRGB;
	ConvertImage(imgSrc, &imgRGB);

	imgDst->Create(iWidth, iHeight, 24);
	int iPitch_dst=imgDst->GetPitch();
	BYTE* byDstData = (BYTE*)imgDst->GetBits();
	switch(color)
	{
	case COLOR_RED: 
		{
			int iIncR;
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					byDstData[r*iPitch_dst+c*3+2]=imgRGB.byImgR[r*iWidth+c];
					byDstData[r*iPitch_dst+c*3+1]=0;
					byDstData[r*iPitch_dst+c*3+0]=0;
				}
			}
			return true;
		}

	case COLOR_GREEN: 
		{
			int iIncR;
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					byDstData[r*iPitch_dst+c*3+2]=0;
					byDstData[r*iPitch_dst+c*3+1]=imgRGB.byImgG[r*iWidth+c];
					byDstData[r*iPitch_dst+c*3+0]=0;
				}
			}
			return true;
		}
	case COLOR_BLUE: 
		{
			int iIncR;
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					byDstData[r*iPitch_dst+c*3+2]=0;
					byDstData[r*iPitch_dst+c*3+1]=0;
					byDstData[r*iPitch_dst+c*3+0]=imgRGB.byImgB[r*iWidth+c];
				}
			}
			return true;
		}
	}

	return true;
}

BOOL ZoomImage(CImage* imgSrc, CImage* imgDst, const double dR0_Src, const double dC0_Src, const double dScale, const int iWidth_Dst, const int iHeight_Dst)
{

	int iWidthSrc = imgSrc->GetWidth();
	int iHeightSrc = imgSrc->GetHeight();

	if(imgDst->IsNull() != true){imgDst->Destroy();}
	imgDst->Create(iWidth_Dst, iHeight_Dst,24);

	BYTE* src = (BYTE*)imgSrc->GetBits();
	int iPitch_src=imgSrc->GetPitch();
	BYTE* dst = (BYTE*)imgDst->GetBits();
	int iPitch_dst=imgDst->GetPitch();

	int iBPP = imgSrc->GetBPP();
	if(iBPP==24)
	{
		for(int r=0; r<iHeight_Dst; r++)
		{
			int ir_Src=r/dScale+dR0_Src;

			if((ir_Src<0)||(ir_Src>=iHeightSrc))
			{
				for(int c=0; c<iWidth_Dst; c++)
				{
					dst[r*iPitch_dst+c*3+2]=127;
					dst[r*iPitch_dst+c*3+1]=127;
					dst[r*iPitch_dst+c*3+0]=127;
				}
				continue;
			}

			for(int c=0; c<iWidth_Dst; c++)
			{
				int ic_Src=c/dScale+dC0_Src;
				if((ic_Src<0)||(ic_Src>=iWidthSrc))
				{
					dst[r*iPitch_dst+c*3+2]=127;
					dst[r*iPitch_dst+c*3+1]=127;
					dst[r*iPitch_dst+c*3+0]=127;
					continue;
				}
				dst[r*iPitch_dst+c*3+2]=src[ir_Src*iPitch_src+ic_Src*3+2];
				dst[r*iPitch_dst+c*3+1]=src[ir_Src*iPitch_src+ic_Src*3+1];
				dst[r*iPitch_dst+c*3+0]=src[ir_Src*iPitch_src+ic_Src*3+0];
			}
		}
		return TRUE;
	}
	if(iBPP==32)
	{
		for(int r=0; r<iHeight_Dst; r++)
		{
			int ir_Src=r/dScale+dR0_Src;

			if((ir_Src<0)||(ir_Src>=iHeightSrc))
			{
				for(int c=0; c<iWidth_Dst; c++)
				{
					dst[r*iPitch_dst+c*3+2]=127;
					dst[r*iPitch_dst+c*3+1]=127;
					dst[r*iPitch_dst+c*3+0]=127;
				}
				continue;
			}

			for(int c=0; c<iWidth_Dst; c++)
			{
				int ic_Src=c/dScale+dC0_Src;
				if((ic_Src<0)||(ic_Src>=iWidthSrc))
				{
					dst[r*iPitch_dst+c*3+2]=127;
					dst[r*iPitch_dst+c*3+1]=127;
					dst[r*iPitch_dst+c*3+0]=127;
					continue;
				}
				dst[r*iPitch_dst+c*3+2]=src[ir_Src*iPitch_src+ic_Src*4+2];
				dst[r*iPitch_dst+c*3+1]=src[ir_Src*iPitch_src+ic_Src*4+1];
				dst[r*iPitch_dst+c*3+0]=src[ir_Src*iPitch_src+ic_Src*4+0];
			}
		}
		return TRUE;
	}
	if((iBPP==2) || (iBPP==4) || (iBPP==8))
	{
		RGBQUAD* pSrcTable=NULL;
		int iColors = imgSrc->GetMaxColorTableEntries();
		if (iColors > 0) 
		{
			pSrcTable = new RGBQUAD[iColors];
			imgSrc->GetColorTable(0, iColors, pSrcTable);
		}
		for(int r=0; r<iHeight_Dst; r++)
		{
			int ir_Src=r/dScale+dR0_Src;

			if((ir_Src<0)||(ir_Src>=iHeightSrc))
			{
				for(int c=0; c<iWidth_Dst; c++)
				{
					dst[r*iPitch_dst+c*3+2]=127;
					dst[r*iPitch_dst+c*3+1]=127;
					dst[r*iPitch_dst+c*3+0]=127;
				}
				continue;
			}

			for(int c=0; c<iWidth_Dst; c++)
			{
				int ic_Src=c/dScale+dC0_Src;
				if((ic_Src<0)||(ic_Src>=iWidthSrc))
				{
					dst[r*iPitch_dst+c*3+2]=127;
					dst[r*iPitch_dst+c*3+1]=127;
					dst[r*iPitch_dst+c*3+0]=127;
					continue;
				}
				dst[r*iPitch_dst+c*3+2]=(BYTE)(((RGBQUAD*)(&(pSrcTable[src[ir_Src*iPitch_src+ic_Src]])))->rgbRed);
				dst[r*iPitch_dst+c*3+1]=(BYTE)(((RGBQUAD*)(&(pSrcTable[src[ir_Src*iPitch_src+ic_Src]])))->rgbGreen);
				dst[r*iPitch_dst+c*3+0]=(BYTE)(((RGBQUAD*)(&(pSrcTable[src[ir_Src*iPitch_src+ic_Src]])))->rgbBlue);

			}
		}
		if(pSrcTable != NULL){delete [] pSrcTable;}
		return TRUE;
	}
	if(iBPP==1) 
	{
		for(int r=0; r<iHeight_Dst; r++)
		{
			int ir_Src=r/dScale+dR0_Src;

			if((ir_Src<0)||(ir_Src>=iHeightSrc))
			{
				for(int c=0; c<iWidth_Dst; c++)
				{
					dst[r*iPitch_dst+c*3+2]=127;
					dst[r*iPitch_dst+c*3+1]=127;
					dst[r*iPitch_dst+c*3+0]=127;
				}
				continue;
			}

			for(int c=0; c<iWidth_Dst; c++)
			{
				int ic_Src=c/dScale+dC0_Src;
				if((ic_Src<0)||(ic_Src>=iWidthSrc))
				{
					dst[r*iPitch_dst+c*3+2]=127;
					dst[r*iPitch_dst+c*3+1]=127;
					dst[r*iPitch_dst+c*3+0]=127;
					continue;
				}
				int iPosition= ir_Src*iPitch_src+ic_Src/8;
				int iDigit = 8-(ic_Src%8)-1;
				BYTE byValue = (( (src[iPosition] & (1<< iDigit)) == (1<< iDigit)) ? 255 : 0);

				dst[r*iPitch_dst+c*3+2]=byValue;
				dst[r*iPitch_dst+c*3+1]=byValue;
				dst[r*iPitch_dst+c*3+0]=byValue;

			}
		}
		return TRUE;
	}
	return TRUE;
}


bool ExtractChannel(CImage* imgSrc, CImage* imgDst, ENUM_COLOR color)
{

	if(imgDst->IsNull() != true){imgDst->Destroy();}
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


bool MakeColorTable(ImgRGB* imgRGB, RGBQUAD* rgbqTable, ULONGLONG* ullFrequency, int iLength, int* iUsedColors)
{
	int iWidth = imgRGB->iWidth;
	RGBQUAD* rgbqTable_temp;
	//	if(iLength>256){return false;}
	rgbqTable_temp = new RGBQUAD[iLength];
	if(ullFrequency != NULL)
	{
		for(int i=0; i<iLength; i++)
		{
			ullFrequency[i]=0;
		}
	}

	int iTableLength = 0;
	for(int r=0; r<imgRGB->iHeight; r++)
	{
		for(int c=0; c<iWidth ; c++)
		{
			BYTE byDataR = imgRGB->byImgR[r*iWidth+c];
			BYTE byDataG = imgRGB->byImgG[r*iWidth+c];
			BYTE byDataB = imgRGB->byImgB[r*iWidth+c];
			bool bFounud = false;
			for(int iTableIndex=0; iTableIndex<iTableLength; iTableIndex++)
			{
				if((byDataR==rgbqTable_temp[iTableIndex].rgbRed)
					&& (byDataG==rgbqTable_temp[iTableIndex].rgbGreen)
					&& (byDataB==rgbqTable_temp[iTableIndex].rgbBlue))
				{
					bFounud = true;
					if(ullFrequency  != NULL){ullFrequency[iTableIndex]++;}
					break;
				}
			}

			if(bFounud==false)
			{
				if(iTableLength==iLength)
				{
					SAFE_DELETE(rgbqTable_temp);
					return false;
				}

				rgbqTable_temp[iTableLength].rgbRed=byDataR;
				rgbqTable_temp[iTableLength].rgbGreen=byDataG;
				rgbqTable_temp[iTableLength].rgbBlue=byDataB;
				rgbqTable_temp[iTableLength].rgbReserved=0;
				if(ullFrequency != NULL){ullFrequency[iTableLength]++;}
				iTableLength++;
			}
		}
	}


	bool bGrayScale=true;
	for(int i=0; i<iLength; i++)
	{
		if(rgbqTable_temp[i].rgbBlue != rgbqTable_temp[i].rgbRed){bGrayScale=false; break;}
		if(rgbqTable_temp[i].rgbBlue != rgbqTable_temp[i].rgbGreen){bGrayScale=false; break;}
	}

	if(bGrayScale==true)
	{
		for(int i=0; i<iLength; i++)
		{
			rgbqTable[i].rgbRed=i;
			rgbqTable[i].rgbGreen=i;
			rgbqTable[i].rgbBlue=i;
		}
	}
	else
	{
		for(int i=0; i<iLength; i++)
		{
			rgbqTable[i].rgbRed=rgbqTable_temp[i].rgbRed;
			rgbqTable[i].rgbGreen=rgbqTable_temp[i].rgbGreen;
			rgbqTable[i].rgbBlue=rgbqTable_temp[i].rgbBlue;
		}
	}
	SAFE_DELETE(rgbqTable_temp);
	if(iUsedColors != NULL){*iUsedColors = iTableLength;}
	return true;
}
int GetTableIndex(const BYTE byR, const BYTE byG, const BYTE byB, const RGBQUAD* rgbqTable, const int iLength)
{
	for(int i=0; i<iLength; i++)
	{
		if((rgbqTable[i].rgbRed==byR)&&(rgbqTable[i].rgbGreen==byG)&&(rgbqTable[i].rgbBlue==byB))
		{
			return i;
		}
	}
	return -1;
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
bool ConvertImageBPP1(const CImage* imgSrc, CImage* imgDst, const bool bLosslessOnly)
{
	if (imgDst->IsNull() != true) {imgDst->Destroy();}
	int iBPP_src = imgSrc->GetBPP();
	if(iBPP_src==1){return CopyImage(imgSrc, imgDst);}

	int iWidth = imgSrc->GetWidth();
	int iHeight = imgSrc->GetHeight();
	int iPitch_src = imgSrc->GetPitch();
	BYTE* pbyData_src = (BYTE*)imgSrc->GetBits();

	imgDst->Create(iWidth, iHeight, 1);
	BYTE* pbyData_dst = (BYTE*)imgDst->GetBits();
	int iPitch_dst = imgDst->GetPitch();

	for(int r=0; r<iHeight; r++)
	{
		for(int pc=0; pc< (iWidth+7)/8; pc++)
		{
			pbyData_dst[r*iPitch_dst+pc]=0;
		}
	}
	ImgRGB imgRGB;
	ConvertImage(imgSrc,&imgRGB);


	if(bLosslessOnly == true)
	{
		for(int r=0; r<iHeight; r++)
		{
			for(int c=0; c< iWidth; c++)
			{
				BYTE byDataR = imgRGB.byImgR[r*iWidth+c];
				BYTE byDataG = imgRGB.byImgG[r*iWidth+c];
				BYTE byDataB = imgRGB.byImgB[r*iWidth+c];
				if((byDataR != byDataG) || (byDataR != byDataB)){imgDst->Destroy(); return false;}
				if((byDataR != 0) && (byDataR != 255)){imgDst->Destroy(); return false;}
				pbyData_dst[r*iPitch_dst+(c/8)]+=(byDataR/255)<<(7-(c%8));
			}
		}
		return true;
	}
	for(int r=0; r<iHeight; r++)
	{
		for(int c=0; c< iWidth; c++)
		{
			int iDataSum = imgRGB.byImgR[r*iWidth+c];
			iDataSum += imgRGB.byImgG[r*iWidth+c];
			iDataSum += imgRGB.byImgB[r*iWidth+c];

			pbyData_dst[r*iPitch_dst+(c/8)]+=(iDataSum/384)<<(7-(c%8));
		}
	}
	return true;
}

bool Set8bitsColorTableImage(const CImage* imgSrc,const RGBQUAD* rgbqTable256, CImage* imgDst)
{
	if (imgDst->IsNull() != true) {imgDst->Destroy();}
	int iWidth = imgSrc->GetWidth();
	int iHeight = imgSrc->GetHeight();
	int iPitch_dst = imgDst->GetPitch();

	imgDst->Create(iWidth, iHeight, 8);

SetColorTable(imgDst, rgbqTable256, 256);


	ImgRGB imgRGB;
	ConvertImage(imgSrc,&imgRGB);
	
	BYTE* pbyData_dst = (BYTE*)imgDst->GetBits();
	for(int r=0; r<iHeight; r++)
	{
		for(int c=0; c< iWidth; c++)
		{
			BYTE byDataR = imgRGB.byImgR[r*iWidth+c];
			BYTE byDataG = imgRGB.byImgG[r*iWidth+c];
			BYTE byDataB = imgRGB.byImgB[r*iWidth+c];

			int iIndex = GetTableIndex(byDataR,byDataG,byDataB,rgbqTable256, 256);
			if(iIndex<0)
			{
				return false;
			}
			pbyData_dst[r*iPitch_dst+c]=(BYTE)iIndex;

		}
	}
	return true;
}
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
	}
SetColorTable(imgDst, rgbqMono, 256);


	BYTE* pbyData_dst = (BYTE*)imgDst->GetBits();
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
			pbyData_dst[r*iPitch_dst+c]=rgbqTable_src[iIndex].rgbRed;
		}
	}
	return true;
}
bool ConvertImageBPP8(const CImage* imgSrc, CImage* imgDst, const bool bLosslessOnly)
{
	if (imgDst->IsNull() != true) {imgDst->Destroy();}
	int iBPP_src = imgSrc->GetBPP();
	if(iBPP_src==8){return CopyImage(imgSrc,imgDst);}

	int iWidth = imgSrc->GetWidth();
	int iHeight = imgSrc->GetHeight();
	int iPitch_src = imgSrc->GetPitch();
	BYTE* pbyData_src = (BYTE*)imgSrc->GetBits();

	ImgRGB imgRGB;
	ConvertImage(imgSrc,&imgRGB);


	imgDst->Create(iWidth, iHeight, 8);

	RGBQUAD rgbqTable_dst[256];
	for(int i=0; i<256; i++)
	{
		rgbqTable_dst[i].rgbRed=0;
		rgbqTable_dst[i].rgbGreen=0;
		rgbqTable_dst[i].rgbBlue=0;
		rgbqTable_dst[i].rgbReserved=0;
	}
	bool bLosslessable = false;

	switch(iBPP_src)
	{
	case 1:
		{
			for(int i=0; i<256; i++)
			{
				rgbqTable_dst[i].rgbRed=i;
				rgbqTable_dst[i].rgbGreen=i;
				rgbqTable_dst[i].rgbBlue=i;
			}
			bLosslessable = true;
			break;
		}
	case 2:
	case 4:
	case 8:
		{
			int iColors = imgSrc->GetMaxColorTableEntries();
			RGBQUAD* rgbqTable_src= new RGBQUAD[iColors];
			imgSrc->GetColorTable(0, iColors, rgbqTable_src);

			for(int i=0; i<iColors; i++)
			{
				rgbqTable_dst[i].rgbRed=rgbqTable_src[i].rgbRed;
				rgbqTable_dst[i].rgbGreen=rgbqTable_src[i].rgbGreen;
				rgbqTable_dst[i].rgbBlue=rgbqTable_src[i].rgbBlue;
				rgbqTable_dst[i].rgbReserved=rgbqTable_src[i].rgbReserved;
			}
			bLosslessable = true;
			break;
		}
	case 24:
	case 32:
		{
			bLosslessable = MakeColorTable(&imgRGB,rgbqTable_dst,NULL,256,NULL);
			break;
		}
	}

	if((bLosslessOnly == true) &&(bLosslessable == false))
	{
		return false;
	}

	if(bLosslessable == true)
	{
		//カラーテーブルを使わない問題があるソフトでも不具合を起こさないようにカラーテーブルを0から255の順に並べる
		if( IsImageMonochrome(imgSrc) == true)
		{
			return SetAsc8bitsMonoColorTableImage(imgSrc, imgDst);
		}

		return Set8bitsColorTableImage(imgSrc,rgbqTable_dst, imgDst);
	}
	return false;
}

bool ConvertImageBPP24(const CImage* imgSrc, CImage* imgDst)
{
	if (imgDst->IsNull() != true) {imgDst->Destroy();}
	int iBPP_src = imgSrc->GetBPP();
	if(iBPP_src==24){return CopyImage(imgSrc, imgDst);}

	int iWidth = imgSrc->GetWidth();
	int iHeight = imgSrc->GetHeight();
	int iPitch_src = imgSrc->GetPitch();
	BYTE* pbyData_src = (BYTE*)imgSrc->GetBits();

	imgDst->Create(iWidth, iHeight, 24);
	BYTE* pbyData_dst = (BYTE*)imgDst->GetBits();
	int iPitch_dst = imgDst->GetPitch();

	ImgRGB imgRGB;
	ConvertImage(imgSrc,&imgRGB);

	for(int r=0; r<iHeight; r++)
	{
		for(int c=0; c< iWidth; c++)
		{
			pbyData_dst[r*iPitch_dst+c*3+0]=imgRGB.byImgB[r*iWidth+c];
			pbyData_dst[r*iPitch_dst+c*3+1]=imgRGB.byImgG[r*iWidth+c];
			pbyData_dst[r*iPitch_dst+c*3+2]=imgRGB.byImgR[r*iWidth+c];
		}
	}
	return true;
}

bool ConvertImageBPP32(const CImage* imgSrc, CImage* imgDst)
{
	if (imgDst->IsNull() != true) {imgDst->Destroy();}
	int iBPP_src = imgSrc->GetBPP();
	if(iBPP_src==32){return CopyImage(imgSrc, imgDst);}

	int iWidth = imgSrc->GetWidth();
	int iHeight = imgSrc->GetHeight();
	int iPitch_src = imgSrc->GetPitch();
	BYTE* pbyData_src = (BYTE*)imgSrc->GetBits();

	imgDst->Create(iWidth, iHeight, 32);
	BYTE* pbyData_dst = (BYTE*)imgDst->GetBits();
	int iPitch_dst = imgDst->GetPitch();

	ImgRGB imgRGB;
	ConvertImage(imgSrc,&imgRGB);

	for(int r=0; r<iHeight; r++)
	{
		for(int c=0; c< iWidth; c++)
		{
			pbyData_dst[r*iPitch_dst+c*4+0]=imgRGB.byImgB[r*iWidth+c];
			pbyData_dst[r*iPitch_dst+c*4+1]=imgRGB.byImgG[r*iWidth+c];
			pbyData_dst[r*iPitch_dst+c*4+2]=imgRGB.byImgR[r*iWidth+c];
			pbyData_dst[r*iPitch_dst+c*4+3]=0;
		}
	}
	return true;
}

int GetNearestColor(const RGBQUAD* rgbqTable, const int iLength, const RGBQUAD rgbqTarget)
{
	int iNearestIndex=-1;
	ULONGLONG ullDelta=256*256*256+1;

	BYTE byR_Target=rgbqTarget.rgbRed;
	BYTE byG_Target=rgbqTarget.rgbGreen;
	BYTE byB_Target=rgbqTarget.rgbBlue;
	for(int i=0; i<iLength; i++)
	{
		BYTE byR_Table=rgbqTable[i].rgbRed;
		BYTE byG_Table=rgbqTable[i].rgbGreen;
		BYTE byB_Table=rgbqTable[i].rgbBlue;

		ULONGLONG ullTemp = (byR_Table-byR_Target)*(byR_Table-byR_Target)
			+1.0*(byG_Table-byG_Target)*(byG_Table-byG_Target)
			+1.0*(byB_Table-byB_Target)*(byB_Table-byB_Target);
		if(ullTemp <ullDelta)
		{
			ullDelta = ullTemp;
			iNearestIndex = i;
		}
	}
	return iNearestIndex;
}

bool GetPopularColorConversionTable(const RGBQUAD* rgbqTable, const int* iPopularOrder, const int iLength, RGBQUAD* rgbqTable_Sorted, const int iMax, int* iConversionTable)
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
		if(iFound<256){iConversionTable[i]=iFound; continue;}
		int iNearest = GetNearestColor(rgbqTable_Sorted, iMax, rgbqTable[i]);
		iConversionTable[i]=iNearest;
	}

	return true;
}

int GetColorTableIndex(const RGBQUAD* rgbqTable, const int iLength, const BYTE byR, const BYTE byG, const BYTE byB)
{
	for(int i=0; i<iLength; i++)
	{
		if((rgbqTable[i].rgbRed==byR)&&(rgbqTable[i].rgbGreen==byG)&&(rgbqTable[i].rgbBlue==byB))
		{
			return i;
		}
	}
	return -1;
}

bool ConvertImageBPP8_AreaCoverage(const CImage* imgSrc, CImage* imgDst)
{
	if (imgDst->IsNull() != true) {imgDst->Destroy();}
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
	MakeColorTable(&imgRGB, rgbqTable, ullFrequency, iWidth*iHeight, &iColors);

	int* iPopularOrder;
	int* iConversionTable;
	iPopularOrder=new int[iColors];
	iConversionTable=new int[iColors];

	index_i(ullFrequency, iColors, iPopularOrder);

	RGBQUAD rgbqTablePopularOrder[256];
	GetPopularColorConversionTable(rgbqTable, iPopularOrder, iColors, rgbqTablePopularOrder, 256, iConversionTable);

	imgDst->Create(iWidth, iHeight,8);



	SetColorTable(imgDst, rgbqTablePopularOrder, 256);


	int iPitch_dst = imgDst->GetPitch();
	BYTE* pbyData_dst = (BYTE*)imgDst->GetBits();
	for(int r=0; r<iHeight; r++)
	{
		for(int c=0; c< iWidth; c++)
		{
			int iColorTableIndex=GetColorTableIndex(rgbqTable,iColors, imgRGB.byImgR[r*iWidth+c], imgRGB.byImgG[r*iWidth+c], imgRGB.byImgB[r*iWidth+c]);
			pbyData_dst[r*iPitch_dst+c]=iConversionTable[iColorTableIndex];
		}
	}
	SAFE_DELETE(rgbqTable);
	SAFE_DELETE(ullFrequency);
	SAFE_DELETE(iPopularOrder);
	SAFE_DELETE(iConversionTable);

	return true;
}

inline void SwapInt(int *a, int *b)
{
	int iTemp = *a;
	*a = *b;
	*b = iTemp;
}

void QuickSortIndex(const ULONGLONG* iValues, int* iIndex, const int iL, const int iR)
{
	int iL_Local=iL;
	int iR_Local=iR;
	ULONGLONG iPivot = iValues[iIndex[(iL_Local + iR_Local) / 2]];

	while (iL_Local <= iR_Local) 
	{
		while (iValues[iIndex[iL_Local]] < iPivot) {iL_Local++;}
		while (iValues[iIndex[iR_Local]] > iPivot) {iR_Local--;}

		if (iL_Local <= iR_Local) 
		{
			SwapInt(&iIndex[iL_Local], &iIndex[iR_Local]);
			iL_Local++;
			iR_Local--;
		}
	}

	if (iL < iR_Local) {QuickSortIndex(iValues, iIndex, iL, iR_Local);}
	if (iL_Local < iR) {QuickSortIndex(iValues, iIndex, iL_Local, iR);}
}

bool index_i(const ULONGLONG* iValues, const int iLength, int* iIndex)
{
	for (int i = 0; i < iLength; i++) {iIndex[i] = i;}

	QuickSortIndex(iValues, iIndex, 0, iLength - 1);
	return true;
}


inline ULONGLONG GetRGBDistanceSq(const RGBQUAD rgbqTable, const BYTE byR, const BYTE byG, const BYTE byB)
{
	return (rgbqTable.rgbRed-byR)*(rgbqTable.rgbRed-byR)
		+(rgbqTable.rgbGreen-byG)*(rgbqTable.rgbGreen-byG)
		+(rgbqTable.rgbBlue-byB)*(rgbqTable.rgbBlue-byB);
}
inline ULONGLONG GetRGBDistanceSq(const RGBQUAD rgbqTable,const RGBQUAD rgbqTable2)
{
	return (rgbqTable.rgbRed-rgbqTable2.rgbRed)*(rgbqTable.rgbRed-rgbqTable2.rgbRed)
		+(rgbqTable.rgbGreen-rgbqTable2.rgbGreen)*(rgbqTable.rgbGreen-rgbqTable2.rgbGreen)
		+(rgbqTable.rgbBlue-rgbqTable2.rgbBlue)*(rgbqTable.rgbBlue-rgbqTable2.rgbBlue);
}

bool K_mean_RGB_2Class(RGBQUAD* rgbqTable, ULONGLONG* ullFrequency, int iLength, BYTE byR1_in, BYTE byG1_in, BYTE byB1_in, BYTE byR2_in, BYTE byG2_in, BYTE byB2_in,BYTE* byR1_out, BYTE* byG1_out, BYTE* byB1_out, BYTE* byR2_out, BYTE* byG2_out, BYTE* byB2_out)
{
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
		int iNum1=0;
		int iNum2=0;
		bool bChanged = false;
		for(int i=0; i<iLength; i++)
		{
			ULONGLONG ullDistance1=GetRGBDistanceSq(rgbqTable[i], byR1, byG1, byB1);
			ULONGLONG ullDistance2=GetRGBDistanceSq(rgbqTable[i], byR2, byG2, byB2);
			if(ullDistance1<ullDistance2)
			{
				iNum1+=ullFrequency[i];
				if(iLabel[i] != 1){bChanged=true;}
				iLabel[i]=1;
			}
			else
			{
				iNum2+=ullFrequency[i];
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
		if((iNum1==0)||(iNum2==0))
		{
			SAFE_DELETE(iLabel);
			ULONGLONG ullRSum=0;
			ULONGLONG ullGSum=0;
			ULONGLONG ullBSum=0;
			for(int i=0; i<iLength; i++)
			{
				ullRSum+=rgbqTable[i].rgbRed;
				ullGSum+=rgbqTable[i].rgbGreen;
				ullBSum+=rgbqTable[i].rgbBlue;
			}
			byR1=max(0,ullRSum/iLength-1);
			byG1=max(0,ullGSum/iLength-1);
			byB1=max(0,ullBSum/iLength-1);
			byR2=min(255,ullRSum/iLength+1);
			byG2=min(255,ullGSum/iLength+1);
			byB2=min(255,ullBSum/iLength+1);
			K_mean_RGB_2Class(rgbqTable, ullFrequency, iLength, byR1, byG1, byB1, byR2, byG2, byB2, byR1_out, byG1_out, byB1_out, byR2_out, byG2_out, byB2_out);
			return true;
		}

		byR1=int(ullSumR1/(iNum1*1.0)+0.5);
		byG1=int(ullSumG1/(iNum1*1.0)+0.5);
		byB1=int(ullSumB1/(iNum1*1.0)+0.5);

		byR2=int(ullSumR2/(iNum2*1.0)+0.5);
		byG2=int(ullSumG2/(iNum2*1.0)+0.5);
		byB2=int(ullSumB2/(iNum2*1.0)+0.5);
		if(bChanged==false){break;}
	}

	SAFE_DELETE(iLabel);
	*byR1_out=byR1;
	*byG1_out=byG1;
	*byB1_out=byB1;
	*byR2_out=byR2;
	*byG2_out=byG2;
	*byB2_out=byB2;
	return true;
}

bool K_mean_RGB(RGBQUAD* rgbqTable, ULONGLONG* ullFrequency, int iLength, RGBQUAD* rgbqTable_classed, int iClassNum)
{
	ULONGLONG* ullDistances;
	ullDistances=new ULONGLONG[iClassNum];

	int* iLabel;
	iLabel=new int[iLength];

	int* iNums;
	iNums=new int[iClassNum];

	ULONGLONG* ullSumRs;
	ULONGLONG* ullSumGs;
	ULONGLONG* ullSumBs;
	ullSumRs=new ULONGLONG[iClassNum];
	ullSumGs=new ULONGLONG[iClassNum];
	ullSumBs=new ULONGLONG[iClassNum];
	while(1)
	{
		for(int iClass=0; iClass<iClassNum; iClass++)
		{
			iNums[iClass]=0;
			ullSumRs[iClass]=0;
			ullSumGs[iClass]=0;
			ullSumBs[iClass]=0;
		}
		bool bChanged = false;
		for(int i=0; i<iLength; i++)
		{
			for(int iClass=0; iClass<iClassNum; iClass++)
			{
				ullDistances[iClass]=GetRGBDistanceSq(rgbqTable[i], rgbqTable_classed[iClass]);
			}
			int iMinDistance=256*256*256;

			int iMinClass;
			for(int iClass=0; iClass<iClassNum; iClass++)
			{
				if(ullDistances[iClass]<iMinDistance){iMinDistance=ullDistances[iClass]; iMinClass=iClass;}

			}



			iNums[iMinClass]+=ullFrequency[i];
			if(iLabel[i] != iMinClass){bChanged=true;}
			iLabel[i]=iMinClass;

		}


		for(int i=0; i<iLength; i++)
		{
			ullSumRs[iLabel[i]]+=rgbqTable[i].rgbRed*ullFrequency[i];
			ullSumGs[iLabel[i]]+=rgbqTable[i].rgbGreen*ullFrequency[i];
			ullSumBs[iLabel[i]]+=rgbqTable[i].rgbBlue*ullFrequency[i];

		}


		for(int iClass=0; iClass<iClassNum; iClass++)
		{
			rgbqTable_classed[iClass].rgbRed=int(ullSumRs[iClass]/(iNums[iClass]*1.0)+0.5);
			rgbqTable_classed[iClass].rgbGreen=int(ullSumGs[iClass]/(iNums[iClass]*1.0)+0.5);
			rgbqTable_classed[iClass].rgbBlue=int(ullSumBs[iClass]/(iNums[iClass]*1.0)+0.5);
		}

		if(bChanged==false){break;}
	}
	SAFE_DELETE(ullSumRs);
	SAFE_DELETE(ullSumGs);
	SAFE_DELETE(ullSumBs);
	SAFE_DELETE(iLabel);
	SAFE_DELETE(iNums);
	SAFE_DELETE(ullDistances);
	return true;
}
bool CalcDeviationForEachClass(RGBQUAD* rgbqTable, ULONGLONG* ullFrequency, int* iClasses, int iLength, RGBQUAD* rgbqTable_classed, int iClassNum, double* dDeviations, int* iMaxDeviationClass)
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
		int iMinDistanceSq=255*255*255;
		int iMinDistanceClass=0;
		for(int iClass=0; iClass<iClassNum; iClass++)
		{
			int iDistanceSq = GetRGBDistanceSq(rgbqTable[i], rgbqTable_classed[iClass].rgbRed, rgbqTable_classed[iClass].rgbGreen, rgbqTable_classed[iClass].rgbBlue);
			if(iDistanceSq<iMinDistanceSq){iMinDistanceSq=iDistanceSq;iMinDistanceClass=iClass;}
		}
		iClasses[i]=iMinDistanceClass;
		ullSumDistSq[iMinDistanceClass]+=iMinDistanceSq;
		ullCount[iMinDistanceClass]+=ullFrequency[i];
	}

	double dDeviationMax=0;
	for(int iClass=0; iClass<iClassNum; iClass++)
	{
		if(ullCount[iClass]==0){dDeviations[iClass]=-1;}
		dDeviations[iClass]=ullSumDistSq[iClass]/(1.0*ullCount[iClass]);
		if(dDeviations[iClass]>dDeviationMax){dDeviationMax=dDeviations[iClass];*iMaxDeviationClass=iClass;}
	}
	SAFE_DELETE(ullSumDistSq);
	SAFE_DELETE(ullCount);
	return true;
}
bool K_mean(RGBQUAD* rgbqTable, ULONGLONG* ullFrequency, int* iClaaes_out, int iLength, RGBQUAD* rgbqResult)
{
	RGBQUAD rgbqTable_classed[256];

	RGBQUAD* rgbqTable_temp;
	rgbqTable_temp = new RGBQUAD[iLength];

	ULONGLONG* ullFrequency_temp;
	ullFrequency_temp = new ULONGLONG[iLength];

	int* iClasses;
	iClasses=new int[iLength];

	for(int iClass=0; iClass<iLength; iClass++){iClasses[iClass]=0;}

	int iTargetClass=0;

	for(int iClassNum=1; iClassNum<255; iClassNum++)
	{
		int iLength_temp=0;
		for(int i=0; i<iLength; i++)
		{
			if(iClasses[i] != iTargetClass){continue;}

			rgbqTable_temp[iLength_temp].rgbRed=rgbqTable[i].rgbRed;
			rgbqTable_temp[iLength_temp].rgbGreen=rgbqTable[i].rgbGreen;
			rgbqTable_temp[iLength_temp].rgbBlue=rgbqTable[i].rgbBlue;
			ullFrequency_temp[iLength_temp]=ullFrequency[i];
			iLength_temp++;
		}


		BYTE byR_Max=0;
		BYTE byG_Max=0;
		BYTE byB_Max=0;
		BYTE byR_Min=255;
		BYTE byG_Min=255;
		BYTE byB_Min=255;

		for(int i=0; i<iLength_temp; i++)
		{
			if(byR_Max < rgbqTable_temp[i].rgbRed){byR_Max = rgbqTable_temp[i].rgbRed;}
			if(byG_Max < rgbqTable_temp[i].rgbGreen){byG_Max = rgbqTable_temp[i].rgbGreen;}
			if(byB_Max < rgbqTable_temp[i].rgbBlue){byB_Max = rgbqTable_temp[i].rgbBlue;}
			if(byR_Min > rgbqTable_temp[i].rgbRed){byR_Min = rgbqTable_temp[i].rgbRed;}
			if(byG_Min > rgbqTable_temp[i].rgbGreen){byG_Min = rgbqTable_temp[i].rgbGreen;}
			if(byB_Min > rgbqTable_temp[i].rgbBlue){byB_Min = rgbqTable_temp[i].rgbBlue;}
		}


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
		rgbqTable_classed[iClassNum].rgbRed=byR2;
		rgbqTable_classed[iClassNum].rgbGreen=byG2;
		rgbqTable_classed[iClassNum].rgbBlue=byB2;

		K_mean_RGB(rgbqTable, ullFrequency, iLength,rgbqTable_classed, iClassNum+1);
		int iMaxDeviationClass=0;
		double dDeviations[256];
		CalcDeviationForEachClass(rgbqTable, ullFrequency, iClasses, iLength, rgbqTable_classed, iClassNum+1, dDeviations, &iMaxDeviationClass);


		iTargetClass=iMaxDeviationClass;
	}

	SAFE_DELETE(rgbqTable_temp);
	SAFE_DELETE(ullFrequency_temp);
	for(int i=0; i<iLength; i++)
	{
		iClaaes_out[i]=iClasses[i];
	}
	SAFE_DELETE(iClasses);
	for(int i=0; i<256; i++)
	{
		rgbqResult[i].rgbRed=rgbqTable_classed[i].rgbRed;
		rgbqResult[i].rgbGreen=rgbqTable_classed[i].rgbGreen;
		rgbqResult[i].rgbBlue=rgbqTable_classed[i].rgbBlue;
	}

	return true;
}

bool ConvertImageBPP8_ByDeviation(const CImage* imgSrc, CImage* imgDst)
{
	if (imgDst->IsNull() != true) {imgDst->Destroy();}
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
	MakeColorTable(&imgRGB, rgbqTable, ullFrequency, iWidth*iHeight, &iColors);

	int* iIndex;
	int* iConversionTable;
	iIndex=new int[iColors];
	iConversionTable=new int[iColors];
	RGBQUAD rgbqResult[256];

	K_mean(rgbqTable, ullFrequency, iConversionTable, iColors, rgbqResult);


	imgDst->Create(iWidth, iHeight,8);


	
SetColorTable(imgDst, rgbqResult, 256);


	int iPitch_dst = imgDst->GetPitch();
	BYTE* pbyData_dst = (BYTE*)imgDst->GetBits();
	for(int r=0; r<iHeight; r++)
	{
		for(int c=0; c< iWidth; c++)
		{
			int iColorTableIndex=GetColorTableIndex(rgbqTable,iColors, imgRGB.byImgR[r*iWidth+c], imgRGB.byImgG[r*iWidth+c], imgRGB.byImgB[r*iWidth+c]);
			pbyData_dst[r*iPitch_dst+c]=iConversionTable[iColorTableIndex];
		}
	}
	SAFE_DELETE(rgbqTable);
	SAFE_DELETE(ullFrequency);
	SAFE_DELETE(iIndex);
	SAFE_DELETE(iConversionTable);

	return true;
}
