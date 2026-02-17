#include "stdafx.h"
#include "ImageProc.h"
#include "SImgProc_ex.h"
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

BOOL ConvertImage(CImage* cimage, ImgRGB* imgRGB)
{
	int iWidth = cimage->GetWidth();
	imgRGB->Set(iWidth,cimage->GetHeight(),CHANNEL_3_8RGB);

	BYTE* src = (BYTE*)cimage->GetBits();
	int iPitch=cimage->GetPitch();
	int iIncR;
//	if(iPitch<0){iIncR=-1;iPitch=iPitch*-1;}else{iIncR=+1;}
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