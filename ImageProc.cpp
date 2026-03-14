#include "stdafx.h"
#include "ImageProc.h"
#include "SImgProc_ex.h"
#include "CommonFunction.h"
bool ConvertImageToStr(CImage* cImageSrc, CString sSeparater, CString* sImage)
{
	int iSrcWidth  = cImageSrc->GetWidth();
	int iSrcHeight = cImageSrc->GetHeight();
	int iSrcBPP= cImageSrc->GetBPP();
	int iSrcPitch = cImageSrc->GetPitch();
    BYTE* bySrcData = (BYTE*)cImageSrc->GetBits();

	if(iSrcWidth <= 0){return FALSE;}
	if(iSrcHeight == 0){return FALSE;}
	if(iSrcBPP == 0){return FALSE;}

	CString sImageLocal=_T("");
	if(iSrcBPP==24)
	{
		for(int r=0; r<iSrcHeight; r++)
		{
			CString sLine=_T("");;
			CString sPixel;
			for(int c=0; c<iSrcWidth; c++)
			{
				sPixel.Format(_T("%d%s"),bySrcData[r*iSrcPitch+c*3+2],sSeparater);
				sLine+=sPixel;
			}
			sLine+=sSeparater;
			for(int c=0; c<iSrcWidth; c++)
			{
				sPixel.Format(_T("%d%s"),bySrcData[r*iSrcPitch+c*3+1],sSeparater);
				sLine+=sPixel;
			}
			sLine+=sSeparater;
			for(int c=0; c<iSrcWidth-1; c++)
			{
				sPixel.Format(_T("%d%s"),bySrcData[r*iSrcPitch+c*3+0],sSeparater);
				sLine+=sPixel;
			}
			sPixel.Format(_T("%d\n"),bySrcData[r*iSrcPitch+(iSrcWidth-1)*3+0]);
			sLine+=sPixel;
			sImageLocal+=sLine;
		}
		(*sImage)=sImageLocal;
		return true;
	}

	if(iSrcBPP==8)
	{
		RGBQUAD* pSrcTable=NULL;
		int iColorCount = cImageSrc->GetMaxColorTableEntries();
		if (iColorCount > 0) 
		{
			pSrcTable = new RGBQUAD[iColorCount];
			cImageSrc->GetColorTable(0, iColorCount, pSrcTable);
		}

		bool bGrayScale=true;
		for(int i=0; i<iColorCount; i++)
		{
			if(pSrcTable[i].rgbBlue != pSrcTable[i].rgbRed){bGrayScale=false; break;}
			if(pSrcTable[i].rgbBlue != pSrcTable[i].rgbGreen){bGrayScale=false; break;}
		}

		if(bGrayScale==true)
		{
			for(int r=0; r<iSrcHeight; r++)
			{
				CString sLine=_T("");;
				CString sPixel;

				for(int c=0; c<iSrcWidth-1; c++)
				{
					sPixel.Format(_T("%d%s"),(BYTE)(((RGBQUAD*)(&(pSrcTable[bySrcData[r*iSrcPitch+c]])))->rgbBlue),sSeparater);
					sLine+=sPixel;
				}
				sPixel.Format(_T("%d\n"),(BYTE)(((RGBQUAD*)(&(pSrcTable[bySrcData[r*iSrcPitch+(iSrcWidth-1)]])))->rgbBlue));
				sLine+=sPixel;
				sImageLocal+=sLine;
			}
			(*sImage)=sImageLocal;
			delete [] pSrcTable;
			return true;
		}

		for(int r=0; r<iSrcHeight; r++)
		{
			CString sLine=_T("");;
			CString sPixel;
			for(int c=0; c<iSrcWidth; c++)
			{
				sPixel.Format(_T("%d%s"),(BYTE)(((RGBQUAD*)(&(pSrcTable[bySrcData[r*iSrcPitch+c]])))->rgbRed),sSeparater);
				sLine+=sPixel;
			}
			sLine+=sSeparater;
			for(int c=0; c<iSrcWidth; c++)
			{
				sPixel.Format(_T("%d%s"),(BYTE)(((RGBQUAD*)(&(pSrcTable[bySrcData[r*iSrcPitch+c]])))->rgbGreen),sSeparater);
				sLine+=sPixel;
			}
			sLine+=sSeparater;
			for(int c=0; c<iSrcWidth-1; c++)
			{
				sPixel.Format(_T("%d%s"),(BYTE)(((RGBQUAD*)(&(pSrcTable[bySrcData[r*iSrcPitch+c]])))->rgbBlue),sSeparater);
				sLine+=sPixel;
			}
			sPixel.Format(_T("%d\n"),(BYTE)(((RGBQUAD*)(&(pSrcTable[bySrcData[r*iSrcPitch+(iSrcWidth-1)]])))->rgbBlue));
			sLine+=sPixel;
			sImageLocal+=sLine;
		}
		(*sImage)=sImageLocal;
		delete [] pSrcTable;
		return true;
	}


	if(iSrcBPP==32)
	{
		for(int r=0; r<iSrcHeight; r++)
		{
			CString sLine=_T("");;
			CString sPixel;
			for(int c=0; c<iSrcWidth; c++)
			{
				sPixel.Format(_T("%d%s"),bySrcData[r*iSrcPitch+c*4+2],sSeparater);
				sLine+=sPixel;
			}
			sLine+=sSeparater;
			for(int c=0; c<iSrcWidth; c++)
			{
				sPixel.Format(_T("%d%s"),bySrcData[r*iSrcPitch+c*4+1],sSeparater);
				sLine+=sPixel;
			}
			sLine+=sSeparater;
			for(int c=0; c<iSrcWidth; c++)
			{
				sPixel.Format(_T("%d%s"),bySrcData[r*iSrcPitch+c*4+0],sSeparater);
				sLine+=sPixel;
			}
			sLine+=sSeparater;
			for(int c=0; c<iSrcWidth-1; c++)
			{
				sPixel.Format(_T("%d%s"),bySrcData[r*iSrcPitch+c*4+3],sSeparater);
				sLine+=sPixel;
			}
			sPixel.Format(_T("%d\n"),bySrcData[r*iSrcPitch+(iSrcWidth-1)*4+3]);
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

    int iSrcBPP = imgOriginal->GetBPP();
    if (iSrcBPP == 0) {return false;}

	HRESULT hr = imgClipped->Create(iClipWidth, iClipHeight, iSrcBPP);
    if (FAILED(hr)) {return false;}

    if (iSrcBPP <= 8) 
	{
        int nColors = imgOriginal->GetMaxColorTableEntries();
        if (nColors > 0) 
		{
            RGBQUAD* pSrcTable = new RGBQUAD[nColors];
            imgOriginal->GetColorTable(0, nColors, pSrcTable);
            imgClipped->SetColorTable(0, nColors, pSrcTable);
            delete[] pSrcTable;
        }
    }

    int iSrcPitch = imgOriginal->GetPitch();
    int iDstPitch = imgClipped->GetPitch();

    BYTE* bySrcData = (BYTE*)imgOriginal->GetBits();
    BYTE* byDstData = (BYTE*)imgClipped->GetBits();

    bool bSrcBottomUp = (iSrcPitch > 0);
    bool bDstBottomUp = (iDstPitch > 0);

	int iSrcByPP = iSrcBPP / 8;

	for (int r = 0; r < iClipHeight; r++) 
	{
		BYTE* pSrcLine = nullptr;
		BYTE* pDstLine = nullptr;

		if (bSrcBottomUp == true) 
		{
			pSrcLine = &(bySrcData[(iImgHeight - 1 - (iR0+r)) * iSrcPitch + iC0 * iSrcByPP]);
		}
		else 
		{
			pSrcLine = &(bySrcData[(iR0+r) * iSrcPitch + iC0 * iSrcByPP]);
		}

		if (bDstBottomUp == true)
		{
			pDstLine = &(byDstData[(iClipHeight - 1 - r) * iDstPitch]);
		}
		else
		{
			pDstLine = &(byDstData[r * iDstPitch]);
		}

		memcpy(pDstLine, pSrcLine, iClipWidth * iSrcByPP);
	}
	return false;
}

BOOL CopyToClipBoardImg(CImage* cImageSrc)
{
	if (cImageSrc->IsNull() == true){return false;}

	int iSrcWidth = cImageSrc->GetWidth();
	int iSrcHeight = cImageSrc->GetHeight();
	int iSrcBPP = cImageSrc->GetBPP();

    int iHeaderSize = sizeof(BITMAPINFOHEADER);

    int iColorCount = 0;
    if (iSrcBPP <= 8) 
	{
        iColorCount = cImageSrc->GetMaxColorTableEntries();
    }
    int iPaletteSize = iColorCount * sizeof(RGBQUAD);

	int iBytesPerLine = ((iSrcWidth * iSrcBPP + 31) / 32) * 4;
	

    int iTotalSize = iHeaderSize + iPaletteSize + ( iBytesPerLine * iSrcHeight);
	
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
    bih->biWidth         = iSrcWidth;
    bih->biHeight        = iSrcHeight;
    bih->biPlanes        = 1;
    bih->biBitCount      = (WORD)iSrcBPP;
    bih->biCompression   = BI_RGB;
    bih->biSizeImage     = iBytesPerLine * iSrcHeight;
    bih->biXPelsPerMeter = 0;
    bih->biYPelsPerMeter = 0;
    bih->biClrUsed       = ((iSrcBPP <= 8) ? iColorCount : 0);
    bih->biClrImportant  = 0;

    BYTE* pPalette = pbyDib + iHeaderSize;
    BYTE* pBits    = pPalette + iPaletteSize;

    if ((iSrcBPP <= 8) && (iColorCount > 0) )
	{
        RGBQUAD* rgbqTable = new RGBQUAD[iColorCount];
        cImageSrc->GetColorTable(0, iColorCount, rgbqTable);

        memcpy(pPalette, rgbqTable, iPaletteSize);
        delete[] rgbqTable;
    }


    BYTE* bySrcData = (BYTE*)cImageSrc->GetBits();
    int iSrcPitch   = cImageSrc->GetPitch();
    bool bBottomUp  = (iSrcPitch > 0);

    for (int r = 0; r < iSrcHeight; r++) 
	{

        BYTE* pSrcLine = nullptr;
        BYTE* pDstLine = &(pBits[r * iBytesPerLine]);

        if (bBottomUp = true)
		{
            pSrcLine = &(bySrcData[(iSrcHeight - 1 -r) * iSrcPitch]);
        } 
		else 
		{
            pSrcLine = &(bySrcData[r * iSrcPitch]);
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
    int iSrcBPP= bih->biBitCount;

	if(iWidth <= 0){SAFE_DELETE(byData); return FALSE;}
	if(iHeight == 0){SAFE_DELETE(byData); return FALSE;}
	if(iSrcBPP == 0){SAFE_DELETE(byData); return FALSE;}

    int iColorCount = 0;
    if (iSrcBPP <= 8) 
	{
        iColorCount = bih->biClrUsed;
        if (iColorCount == 0) 
		{
            iColorCount = 1 << iSrcBPP ;
        }
    }

    int iPaletteSize = iColorCount * sizeof(RGBQUAD);

    BYTE* pPalette = &(byData[sizeof(BITMAPINFOHEADER)]);
    BYTE* bySrcData    = &(byData[sizeof(BITMAPINFOHEADER)+iPaletteSize ]);;

	if(cImageDst->IsNull() != true){cImageDst->Destroy();}

    HRESULT hr = cImageDst->Create(iWidth, abs(iHeight), iSrcBPP);
    if (FAILED(hr)) {SAFE_DELETE(byData); return FALSE;}

    if ((iSrcBPP <= 8) && (iColorCount > 0))
	{
        cImageDst->SetColorTable(0, iColorCount, (RGBQUAD*)pPalette);
    }

    int iDstPitch = cImageDst->GetPitch();
    BYTE* byDstData = (BYTE*)cImageDst->GetBits();

    bool bDstBottomUp = (iDstPitch > 0);
    bool bSrcBottomUp = (iHeight > 0); 

	int iBytesPerLine = ((iWidth * iSrcBPP + 31) / 32) * 4;
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
            pDstLine = &(byDstData[(iAbsHeight - 1 - r) * iDstPitch]);
        }
		else
		{
            pDstLine = &(byDstData[r * iDstPitch]);
        }

        memcpy(pDstLine, pSrcLine, iBytesPerLine);
    }
	SAFE_DELETE(byData); 
	return TRUE;
}

BOOL ConvertImage(CImage* cimage, ImgRGB* imgRGB)
{
	int iSrcWidth = cimage->GetWidth();
	int iSrcHeight = cimage->GetHeight();
	imgRGB->Set(iSrcWidth,iSrcHeight,CHANNEL_3_8RGB);
	
	int iBPP = cimage->GetBPP();
	BYTE* src = (BYTE*)cimage->GetBits();
	int iSrcPitch=cimage->GetPitch();
	int iIncR;
	if(iBPP==24)
	{
		for(int r=0; r<iSrcHeight; r++)
		{
			for(int c=0; c<iSrcWidth; c++)
			{
				imgRGB->byImgR[r*imgRGB->iWidth+c]=src[r*iSrcPitch+c*3+2];
				imgRGB->byImgG[r*imgRGB->iWidth+c]=src[r*iSrcPitch+c*3+1];
				imgRGB->byImgB[r*imgRGB->iWidth+c]=src[r*iSrcPitch+c*3+0];
			}
		}
		return TRUE;
	}

	if(iBPP==8)
	{
		RGBQUAD* pSrcTable=NULL;
        int nColors = cimage->GetMaxColorTableEntries();
        if (nColors > 0) 
		{
            pSrcTable = new RGBQUAD[nColors];
            cimage->GetColorTable(0, nColors, pSrcTable);
        }
		for(int r=0; r<iSrcHeight; r++)
		{
			for(int c=0; c<iSrcWidth; c++)
			{
				imgRGB->byImgR[r*imgRGB->iWidth+c]=(BYTE)(((RGBQUAD*)(&(pSrcTable[src[r*iSrcPitch+c]])))->rgbRed);
				imgRGB->byImgG[r*imgRGB->iWidth+c]=(BYTE)(((RGBQUAD*)(&(pSrcTable[src[r*iSrcPitch+c]])))->rgbGreen);
				imgRGB->byImgB[r*imgRGB->iWidth+c]=(BYTE)(((RGBQUAD*)(&(pSrcTable[src[r*iSrcPitch+c]])))->rgbBlue);
			}
		}
		if(pSrcTable != NULL){delete [] pSrcTable;}
		return TRUE;
	}
	
	if(iBPP==32)
	{
		for(int r=0; r<iSrcHeight; r++)
		{
			for(int c=0; c<iSrcWidth; c++)
			{
				imgRGB->byImgR[r*imgRGB->iWidth+c]=src[r*iSrcPitch+c*4+2];
				imgRGB->byImgG[r*imgRGB->iWidth+c]=src[r*iSrcPitch+c*4+1];
				imgRGB->byImgB[r*imgRGB->iWidth+c]=src[r*iSrcPitch+c*4+0];
			}
		}
		return TRUE;
	}

	return TRUE;
}



BOOL ConvertImage(ImgRGB* imgRGB, CImage* cImageDst)
{
	if(cImageDst->IsNull() != true){cImageDst->Destroy();}
	int iSrcWidth = imgRGB->iWidth;;
	int iSrcHeight= imgRGB->iHeight;;

	cImageDst->Create(iSrcWidth, iSrcHeight, 24);


	BYTE* byDstData = (BYTE*)cImageDst->GetBits();
	int iDstPitch=cImageDst->GetPitch();
	int iIncR;
	for(int r=0; r<iSrcHeight; r++)
	{
		for(int c=0; c<iSrcWidth; c++)
		{
			byDstData[r*iDstPitch+c*3+2]=imgRGB->byImgR[r*iSrcWidth+c];
			byDstData[r*iDstPitch+c*3+1]=imgRGB->byImgG[r*iSrcWidth+c];
			byDstData[r*iDstPitch+c*3+0]=imgRGB->byImgB[r*iSrcWidth+c];
		}
	}
	return TRUE;
}


BOOL ZoomImage(CImage* imgSrc, CImage* imgDst, const double iR0_Src, const double iC0_Src, const double dScale, const int iWidth_Dst, const int iHeight_Dst)
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
		int ir_Src=r/dScale+iR0_Src;
	
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
			int ic_Src=c/dScale+iC0_Src;
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
		int ir_Src=r/dScale+iR0_Src;
	
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
			int ic_Src=c/dScale+iC0_Src;
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
	if(iBPP==8)
	{
		RGBQUAD* pSrcTable=NULL;
		int nColors = imgSrc->GetMaxColorTableEntries();
		if (nColors > 0) 
		{
			pSrcTable = new RGBQUAD[nColors];
			imgSrc->GetColorTable(0, nColors, pSrcTable);
		}
		for(int r=0; r<iHeight_Dst; r++)
		{
			int ir_Src=r/dScale+iR0_Src;

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
				int ic_Src=c/dScale+iC0_Src;
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
	return TRUE;
}
