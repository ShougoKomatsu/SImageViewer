#include "stdafx.h"
#include "ImageProc.h"
#include "SImgProc_ex.h"


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
bool ClipImage(const CImage* imgOriginal, CImage* imgClipped, int iR0, int iC0, int iR1, int iC1)
{
	if (imgOriginal->IsNull() == true){return false;}

	int iImgWidth  = imgOriginal->GetWidth();
	int iImgHeight = imgOriginal->GetHeight();

	int iR0Local=max(0, min(iR0,iR1));
	int iC0Local=max(0, min(iC0,iC1));
	int iR1Local=min(iImgHeight-1, max(iR0,iR1));
	int iC1Local=min(iImgWidth-1, max(iC0,iC1));

	int iClipWidth  = iC1-iC0+1;
	int iClipHeight = iR1-iR0+1;

	if (iClipWidth<= 0){return false;}
	if (iClipHeight<= 0){return false;}

	BOOL bRet = imgClipped->Create(iClipWidth, iClipHeight, imgOriginal->GetBPP());
	if (bRet == FALSE){return false;}

	HDC hSrcDC = imgOriginal->GetDC();
	HDC hDstDC = imgClipped->GetDC();

	if ((hSrcDC == nullptr) || (hDstDC == nullptr))
	{
		if (hSrcDC != nullptr) {imgOriginal->ReleaseDC();}
		if (hDstDC != nullptr) {imgClipped->ReleaseDC();}
		return false;
	}

	bRet = ::BitBlt(hDstDC, 0, 0, iClipWidth, iClipHeight, hSrcDC, iC0, iR0, SRCCOPY);
	if(bRet !=TRUE)
	{
		if (hSrcDC != nullptr) {imgOriginal->ReleaseDC();}
		if (hDstDC != nullptr) {imgClipped->ReleaseDC();}
		return FALSE;
	}

	imgOriginal->ReleaseDC();
	imgClipped->ReleaseDC();
	return true;
}


BOOL CopyToClipBoardImg(CImage* img)
{
	if (img->IsNull() == true){return false;}

	int iWidth = img->GetWidth();
	int iHeight = img->GetHeight();
	int iBitsPerPixel = img->GetBPP();

	int iLineBytes = ((iWidth * iBitsPerPixel + 31) / 32) * 4;

	HGLOBAL hGL = GlobalAlloc(GPTR, sizeof(BITMAPINFOHEADER) + iLineBytes * iHeight);
	if (hGL == NULL){return false;}

	BYTE* pbyDib = (BYTE*)GlobalLock(hGL);
	if (pbyDib == NULL)
	{
		GlobalFree(hGL);
		return false;
	}

	BITMAPINFOHEADER* bmih = (BITMAPINFOHEADER*)pbyDib;
	bmih->biSize        = sizeof(BITMAPINFOHEADER);
	bmih->biWidth       = iWidth;
	bmih->biHeight      = iHeight;
	bmih->biPlanes      = 1;
	bmih->biBitCount    = (WORD)iBitsPerPixel;
	bmih->biCompression = BI_RGB;
	bmih->biSizeImage   = iLineBytes * iHeight;

	for (int r = 0; r < iHeight; r++)
	{
		BYTE* srcLine = (BYTE*)img->GetPixelAddress(0, iHeight-r-1);
		memcpy(&(pbyDib[sizeof(BITMAPINFOHEADER) + iLineBytes * r]), srcLine, iLineBytes);
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

BOOL CopyFromClipBoardImg(CImage* img)
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
	if(bRet == FALSE){return FALSE;}


	ImgRGB imgRGB;
	bRet = ReadBmpFromData(FALSE, (BYTE*)byData, &imgRGB);
	if(bRet == FALSE)
	{
		if(byData != NULL){delete [] byData;}
		return FALSE;
	}
	if(byData != NULL){delete [] byData;}


	ConvertImage(&imgRGB,img);
	return TRUE;
}

BOOL ConvertImage(CImage* cimage, ImgRGB* imgRGB)
{
	int iWidth = cimage->GetWidth();
	imgRGB->Set(iWidth,cimage->GetHeight(),CHANNEL_3_8RGB);
	
	int iBPP = cimage->GetBPP();
	BYTE* src = (BYTE*)cimage->GetBits();
	int iPitch=cimage->GetPitch();
	int iIncR;
	//	if(iPitch<0){iIncR=-1;iPitch=iPitch*-1;}else{iIncR=+1;}
	if(iBPP==24)
	{
		for(int r=0; r<cimage->GetHeight(); r++)
		{
			for(int c=0; c<cimage->GetWidth(); c++)
			{
				imgRGB->byImgR[r*imgRGB->iWidth+c]=src[r*iPitch+c*3+2];
				imgRGB->byImgG[r*imgRGB->iWidth+c]=src[r*iPitch+c*3+1];
				imgRGB->byImgB[r*imgRGB->iWidth+c]=src[r*iPitch+c*3+0];
			}
		}
		return TRUE;
	}

	if(iBPP==8)
	{
		for(int r=0; r<cimage->GetHeight(); r++)
		{
			for(int c=0; c<cimage->GetWidth(); c++)
			{
				imgRGB->byImgR[r*imgRGB->iWidth+c]=src[r*iPitch+c];
				imgRGB->byImgG[r*imgRGB->iWidth+c]=src[r*iPitch+c];
				imgRGB->byImgB[r*imgRGB->iWidth+c]=src[r*iPitch+c];
			}
		}
		return TRUE;
	}
	
	if(iBPP==32)
	{
		for(int r=0; r<cimage->GetHeight(); r++)
		{
			for(int c=0; c<cimage->GetWidth(); c++)
			{
				imgRGB->byImgR[r*imgRGB->iWidth+c]=src[r*iPitch+c*4+2];
				imgRGB->byImgG[r*imgRGB->iWidth+c]=src[r*iPitch+c*4+1];
				imgRGB->byImgB[r*imgRGB->iWidth+c]=src[r*iPitch+c*4+0];
			}
		}
		return TRUE;
	}

	return TRUE;
}



BOOL ConvertImage(ImgRGB* imgRGB, CImage* cimage)
{
	if(cimage->IsNull() != true){cimage->Destroy();}
	cimage->Create(imgRGB->iWidth, imgRGB->iHeight, 24);

	int iWidth = imgRGB->iWidth;;

	BYTE* src = (BYTE*)cimage->GetBits();
	int iPitch=cimage->GetPitch();
	int iIncR;
//	if(iPitch<0){iIncR=-1;iPitch=iPitch*-1;}else{iIncR=+1;}
	for(int r=0; r<cimage->GetHeight(); r++)
	{
		for(int c=0; c<cimage->GetWidth(); c++)
		{
			src[r*iPitch+c*3+2]=imgRGB->byImgR[r*imgRGB->iWidth+c];
			src[r*iPitch+c*3+1]=imgRGB->byImgG[r*imgRGB->iWidth+c];
			src[r*iPitch+c*3+0]=imgRGB->byImgB[r*imgRGB->iWidth+c];
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
			dst[r*iPitch_dst+c*3+2]=src[ir_Src*iPitch_src+ic_Src];
			dst[r*iPitch_dst+c*3+1]=src[ir_Src*iPitch_src+ic_Src];
			dst[r*iPitch_dst+c*3+0]=src[ir_Src*iPitch_src+ic_Src];
		}
	}
	return TRUE;
	}
	return TRUE;
}