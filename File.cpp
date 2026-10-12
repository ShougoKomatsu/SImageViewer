#include "stdafx.h"
#include "File.h"

	bool GetImgFormat(const CString sExt, FileFormatList* fileFormatList, FileFormat* fileFormat)
	{
		for(UINT i=0; i<fileFormatList->uiNum; i++)
		{
			if(fileFormatList->fileFormat[i].sType.CompareNoCase(sExt)==0)
			{
				fileFormat->Copy(&(fileFormatList->fileFormat[i]));
				return true;
			}
		}

		return false;
	}

bool ReadBinaryFile(const CString sFilePath, FileFormatList* fileFormatList, PanImage* panImgDst)
{
	CString sExt;
	int iPlace = sFilePath.ReverseFind('.');
	if(iPlace<0){return false;}
	sExt.Format(_T("%s"), sFilePath.Mid(iPlace+1));

	FileFormat fileFormat;
	bool bRet = GetImgFormat(sExt, fileFormatList, &fileFormat);
	if(bRet != true){return false;}

	CFile f;
	f.Open(sFilePath, CFile::modeRead);
	ULONGLONG ullFileSize = f.SeekToEnd();
	if(ullFileSize>=UINT_MAX){f.Close(); return false;}
	f.SeekToBegin();

	BYTE* byData;
	byData = new BYTE[ullFileSize];
	memset(byData,0x00,ullFileSize);
	UINT uiRead = f.Read(byData, (UINT)ullFileSize);
	f.Close();

	UINT uiWidth;
	UINT uiHeight;

	if(fileFormat.iWidth>0){uiWidth=fileFormat.iWidth;}
	else{uiWidth =  *(UINT*)(&(byData[fileFormat.iWidthInfoOffset]));}
	
	if(fileFormat.iHeight>0){uiHeight = fileFormat.iHeight;}
	else{uiHeight = *(UINT*)(&(byData[fileFormat.iHeightInfoOffset]));}

	UINT uiDataOffset;
	
	if(fileFormat.iDataOffset>=0){uiDataOffset = fileFormat.iDataOffset;}
	else{uiDataOffset = fileFormat.iDataOffsetOffset + (*(UINT*)(&(byData[fileFormat.iDataOffset])));}

	CImage imgTemp;
	imgTemp.Create(uiWidth, uiHeight, 8);
	RGBQUAD colorTable[256];
	for(int i=0; i<256; i++)
	{
		colorTable[i].rgbBlue=i;
		colorTable[i].rgbGreen=i;
		colorTable[i].rgbRed=i;
		colorTable[i].rgbReserved=0;
	}
	SetColorTable(&imgTemp, colorTable, 256);

	int iPitch = imgTemp.GetPitch();
	BYTE* pbyData = (BYTE*)imgTemp.GetBits();

	for(UINT r=0; r< uiHeight; r++)
	{
		for(UINT c=0; c<uiWidth; c++)
		{
			if(uiDataOffset + r*uiWidth+c>=ullFileSize){break;}
			pbyData[r*iPitch+c]=byData[uiDataOffset + r*uiWidth+c];
		}
	}

	SAFE_DELETE(byData);

	panImgDst->Set(IMAGE_TYPE_CIMAGE, NULL, NULL, uiWidth, uiHeight, &imgTemp, VALUE_IMAGE_UNDEFINED, sFilePath);
	return true;
}
	bool GetFileFormat(const CString sIniFilePath, const CString sType, FileFormat* fileFormat)
	{
		const UINT uiBufSize=128;
		TCHAR tchData[uiBufSize];
		fileFormat->sType.Format(_T("%s"), sType);
		GetPrivateProfileString(sType, _T("iWidth"), _T(""), tchData, uiBufSize, sIniFilePath);
		if(_tcslen(tchData)>=0){fileFormat->iWidth = _ttoi(tchData);}
		else{fileFormat->iWidth = 0;}

		GetPrivateProfileString(sType, _T("iHeight"), _T(""), tchData, uiBufSize, sIniFilePath);
		if(_tcslen(tchData)>=0){fileFormat->iHeight = _ttoi(tchData);}
		else{fileFormat->iHeight = 0;}

		GetPrivateProfileString(sType, _T("iDataOffset"), _T(""), tchData, uiBufSize, sIniFilePath);
		if(_tcslen(tchData)>=0){fileFormat->iDataOffset = _ttoi(tchData);}
		else{fileFormat->iDataOffset = -1;}

		GetPrivateProfileString(sType, _T("iWidthInfoOffset"), _T(""), tchData, uiBufSize, sIniFilePath);
		if(_tcslen(tchData)>=0){fileFormat->iWidthInfoOffset = _ttoi(tchData);}
		else{fileFormat->iWidthInfoOffset = 0;}

		GetPrivateProfileString(sType, _T("iHeightInfoOffset"), _T(""), tchData, uiBufSize, sIniFilePath);
		if(_tcslen(tchData)>=0){fileFormat->iHeightInfoOffset = _ttoi(tchData);}
		else{fileFormat->iHeightInfoOffset = 0;}

		GetPrivateProfileString(sType, _T("iDataInfoOffset"), _T(""), tchData, uiBufSize, sIniFilePath);
		if(_tcslen(tchData)>=0){fileFormat->iDataInfoOffset = _ttoi(tchData);}
		else{fileFormat->iDataInfoOffset = 0;}

		GetPrivateProfileString(sType, _T("iDataOffsetOffset"), _T(""), tchData, uiBufSize, sIniFilePath);
		if(_tcslen(tchData)>=0){fileFormat->iDataOffsetOffset = _ttoi(tchData);}
		else{fileFormat->iDataOffsetOffset =0;}

		return true;
	}
	
	bool WriteFileFormat(const CString sIniFilePath, FileFormatList* fileFormatList)
	{
		CString sWrite;
		for(UINT i=0; i<fileFormatList->uiNum; i++)
		{
			CString sKey;
			sKey.Format(_T("Type%d"), i+1);
			WritePrivateProfileString(_T("Types"), sKey, fileFormatList->fileFormat[i].sType, sIniFilePath);

			sWrite.Format(_T("%d"), fileFormatList->fileFormat[i].iWidth);
			WritePrivateProfileString(fileFormatList->fileFormat[i].sType, _T("iWidth"), sWrite, sIniFilePath);
			
			sWrite.Format(_T("%d"), fileFormatList->fileFormat[i].iHeight);
			WritePrivateProfileString(fileFormatList->fileFormat[i].sType, _T("iHeight"), sWrite, sIniFilePath);
			
			sWrite.Format(_T("%d"), fileFormatList->fileFormat[i].iDataOffset);
			WritePrivateProfileString(fileFormatList->fileFormat[i].sType, _T("iDataOffset"), sWrite,sIniFilePath);
			
			sWrite.Format(_T("%d"), fileFormatList->fileFormat[i].iWidthInfoOffset);
			WritePrivateProfileString(fileFormatList->fileFormat[i].sType, _T("iWidthInfoOffset"), sWrite, sIniFilePath);
			
			sWrite.Format(_T("%d"), fileFormatList->fileFormat[i].iHeightInfoOffset);
			WritePrivateProfileString(fileFormatList->fileFormat[i].sType, _T("iHeightInfoOffset"), sWrite, sIniFilePath);
			
			sWrite.Format(_T("%d"), fileFormatList->fileFormat[i].iDataInfoOffset);
			WritePrivateProfileString(fileFormatList->fileFormat[i].sType, _T("iDataInfoOffset"), sWrite, sIniFilePath);
			
			sWrite.Format(_T("%d"), fileFormatList->fileFormat[i].iDataOffsetOffset);
			WritePrivateProfileString(fileFormatList->fileFormat[i].sType, _T("iDataOffsetOffset"), sWrite, sIniFilePath);
		}

		return true;
	}
	TYPE_IMAGE_FILE IsImageFIle(CString sFilePath, FileFormatList* fileFormatList)
{
	if(sFilePath.Right(4).CompareNoCase(_T(".bmp"))==0){return IMAGE_FILE_TYPE_SINGLE;}
	if(sFilePath.Right(4).CompareNoCase(_T(".png"))==0){return IMAGE_FILE_TYPE_SINGLE;}
	if(sFilePath.Right(4).CompareNoCase(_T(".jpg"))==0){return IMAGE_FILE_TYPE_SINGLE;}
	if(sFilePath.Right(4).CompareNoCase(_T(".ico"))==0){return IMAGE_FILE_TYPE_MULTI;}
	if(sFilePath.Right(4).CompareNoCase(_T(".exe"))==0){return IMAGE_FILE_TYPE_MULTI;}
	if(sFilePath.Right(4).CompareNoCase(_T(".dll"))==0){return IMAGE_FILE_TYPE_MULTI;}
	for(UINT i=0; i<fileFormatList->uiNum; i++)
	{
		if(sFilePath.Right(fileFormatList->fileFormat[i].sType.GetLength()).CompareNoCase(fileFormatList->fileFormat[i].sType)==0){return IMAGE_FILE_TYPE_SINGLE;}
	}

	return IMAGE_FILE_TYPE_UNDEFINED;
}

bool RecursivelyGetImageFilePaths(const CString sFileOrFolderPath, const int iDepth, CStringArray* saFilePath, FileFormatList* fileFormatList)
{
	if(iDepth==0){return false;}
	if(sFileOrFolderPath.Find(_T("|"))>=0)
	{
		CStringArray saFilePathTemp;
		saFilePathTemp.RemoveAll();
		int iPlaceStart=0;
		while(1)
		{
			int iPlaceEnd = sFileOrFolderPath.Find(_T("|"),iPlaceStart);
			if(iPlaceEnd<0)
			{
				saFilePathTemp.Add(sFileOrFolderPath.Mid(iPlaceStart,sFileOrFolderPath.GetLength()-iPlaceStart));
				break;
			}
			saFilePathTemp.Add(sFileOrFolderPath.Mid(iPlaceStart,iPlaceEnd-iPlaceStart));
			iPlaceStart=iPlaceEnd+1;
		}
		for(int i=0; i<saFilePathTemp.GetCount(); i++)
		{
			bool bRet = RecursivelyGetImageFilePaths(saFilePathTemp.GetAt(i), iDepth-1, saFilePath, fileFormatList);
			if(bRet != true){return false;}
		}
		return true;
	}

	DWORD dwAttribute = GetFileAttributes(sFileOrFolderPath);
	if (dwAttribute == INVALID_FILE_ATTRIBUTES) {return false;}

	if ((dwAttribute & FILE_ATTRIBUTE_DIRECTORY) == 0) 
	{
		if(IsImageFIle(sFileOrFolderPath, fileFormatList)==IMAGE_FILE_TYPE_SINGLE)
		{
			saFilePath->Add(sFileOrFolderPath);
		}
		if(IsImageFIle(sFileOrFolderPath, fileFormatList)==IMAGE_FILE_TYPE_MULTI)
		{
			saFilePath->Add(sFileOrFolderPath);
		}
		return true;
	}


	CString searchPath = sFileOrFolderPath;
	if (searchPath.Right(1) != _T("\\")) 
	{
		searchPath += _T("\\");
	}
	searchPath += _T("*.*");

	CFileFind cf;
	BOOL bWorking = cf.FindFile(searchPath);

	while (bWorking == TRUE) 
	{
		bWorking = cf.FindNextFile();
		if (cf.IsDots() == TRUE){continue;}

		CString sFilePath = cf.GetFilePath();

		if (cf.IsDirectory() == TRUE) 
		{
			bool bRet = RecursivelyGetImageFilePaths(sFilePath, iDepth-1, saFilePath, fileFormatList);
			if(bRet != true){continue;}
		}
		else 
		{
			if(IsImageFIle(sFilePath, fileFormatList) == IMAGE_FILE_TYPE_UNDEFINED){continue;}
			saFilePath->Add(sFilePath);
		}
	}

	cf.Close();
	return true;
}

UINT CountImageInOneFile(CString sFilePath, FileFormatList* fileFormatList)
{
	if(sFilePath.Right(4).CompareNoCase(_T(".bmp"))==0){return 1;}
	if(sFilePath.Right(4).CompareNoCase(_T(".png"))==0){return 1;}
	if(sFilePath.Right(4).CompareNoCase(_T(".jpg"))==0){return 1;}
	if(sFilePath.Right(4).CompareNoCase(_T(".ico"))==0){return 2*CountIconNum(sFilePath); }
	if(sFilePath.Right(4).CompareNoCase(_T(".exe"))==0){return 2*CountIconNum(sFilePath); }
	if(sFilePath.Right(4).CompareNoCase(_T(".dll"))==0){return 2*CountIconNum(sFilePath); }
	for(UINT i=0; i<fileFormatList->uiNum; i++)
	{
		if(sFilePath.Right(fileFormatList->fileFormat[i].sType.GetLength()).CompareNoCase(fileFormatList->fileFormat[i].sType)==0){return 1;}
	}
	return 0;
}

int CountImages(const CString sFileOrFolderPath, const int iDepth, FileFormatList* fileFormatList)
{
	if(iDepth==0){return false;}
	CStringArray saFilePath;
	saFilePath.RemoveAll();
	bool bRet = RecursivelyGetImageFilePaths(sFileOrFolderPath, iDepth, &saFilePath, fileFormatList);
	if(bRet != true){return 0;}

	int iFileNum = (int)saFilePath.GetCount();
	int iImageCount=0;
	for(int i=0; i<iFileNum; i++)
	{
		iImageCount += CountImageInOneFile(saFilePath.GetAt(i), fileFormatList);
	}
	return iImageCount;
}

bool ReadAndAppendImage(CString sFilePath, FileFormatList* fileFormatList, PanImage* panImage, int iImageIndex, int* iImageIndexNew)
{
	for(UINT i=0; i<fileFormatList->uiNum; i++)
	{
		if(sFilePath.Right(fileFormatList->fileFormat[i].sType.GetLength()).CompareNoCase(fileFormatList->fileFormat[i].sType)==0)
		{
			return ReadBinaryFile(sFilePath, fileFormatList, panImage);
		}
	}
	if(((sFilePath.Right(4)).CompareNoCase(_T(".ico"))==0)
		||((sFilePath.Right(4)).CompareNoCase(_T(".exe"))==0)
		||((sFilePath.Right(4)).CompareNoCase(_T(".dll"))==0))
	{

		UINT uiIconNum = CountIconNum(sFilePath);
		bool bRet = LoadICOFile(sFilePath,&(panImage[iImageIndex]),uiIconNum);
		if(bRet != true){return false;}
		*iImageIndexNew = iImageIndex+uiIconNum;
		return true;
	}
	CImage img;
	HRESULT hResult = img.Load(sFilePath);
	if(hResult != S_OK){return false;}

	panImage->Set(IMAGE_TYPE_CIMAGE, NULL, NULL, 0, 0, &img, VALUE_IMAGE_UNDEFINED, sFilePath);

	*iImageIndexNew = iImageIndex+1;
	return true;
}

bool ReadAndAppendSingllImage(CString sFilePath, FileFormatList* fileFormatList, PanImage* panImage, int iTargetImageIndex)
{
	for(UINT i=0; i<fileFormatList->uiNum; i++)
	{
		if(sFilePath.Right(fileFormatList->fileFormat[i].sType.GetLength()).CompareNoCase(fileFormatList->fileFormat[i].sType)==0)
		{
			return ReadBinaryFile(sFilePath, fileFormatList, panImage);
		}
	}
	if(((sFilePath.Right(4)).CompareNoCase(_T(".ico"))==0)
		||((sFilePath.Right(4)).CompareNoCase(_T(".exe"))==0)
		||((sFilePath.Right(4)).CompareNoCase(_T(".dll"))==0))
	{

		UINT uiIconNum = CountIconNum(sFilePath);
		bool bRet = LoadICOFileSingle(sFilePath,panImage,1);
		if(bRet != true){return false;}
		return true;
	}
	CImage img;
	HRESULT hResult = img.Load(sFilePath);
	if(hResult != S_OK){return false;}

	panImage->Set(IMAGE_TYPE_CIMAGE, NULL, NULL, 0, 0, &img, VALUE_IMAGE_UNDEFINED, sFilePath);

	return true;
}


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

			bool bAlphasAre0 = true;
			if(iBPP==32)
			{
				for (int iR = 0; iR < iHeight; iR++)
			{
				for (int iC = 0; iC < iWidth; iC++)
				{
					if(byColor[iR * iDataPitch_Src + iC * iColorPitch_Src + 3] != 0){bAlphasAre0=false; break;}
				}
				}
			}
			for (int iR = 0; iR < iHeight; iR++)
			{
				for (int iC = 0; iC < iWidth; iC++)
				{
					pbyDataDst[iR * iPitch_Dst +iC * 4 + 0] = byColor[iR * iDataPitch_Src + iC * iColorPitch_Src + 0];
					pbyDataDst[iR * iPitch_Dst +iC * 4 + 1] = byColor[iR * iDataPitch_Src + iC * iColorPitch_Src + 1];
					pbyDataDst[iR * iPitch_Dst +iC * 4 + 2] = byColor[iR * iDataPitch_Src + iC * iColorPitch_Src + 2];
					pbyDataDst[iR * iPitch_Dst +iC * 4 + 3] = 255;

					if((iBPP==32) && (bAlphasAre0==false))
					{
						pbyDataDst[iR * iPitch_Dst +iC * 4 + 3] = byColor[iR * iDataPitch_Src + iC * iColorPitch_Src + 3];
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
	
	bool LoadICOFileSingle(const CString sFilePath, PanImage* imgs, const int iTargetImageIndex)
	{
		HICON* hLargeIcons=NULL;
		HICON* hSmallIcons=NULL;
		hLargeIcons = new HICON[iTargetImageIndex];
		hSmallIcons = new HICON[iTargetImageIndex];

		UINT uiExtracted = ExtractIconEx(sFilePath, 0, hLargeIcons, hSmallIcons, iTargetImageIndex);
		if (uiExtracted == 0){SAFE_DELETE(hLargeIcons); SAFE_DELETE(hSmallIcons); return false;}

		HDC hScreenDC = ::GetDC(NULL);
		if (hScreenDC == NULL)
		{
			for (UINT ui=0; ui<iTargetImageIndex; ui++)
			{
				if (hLargeIcons[ui] != NULL){::DestroyIcon(hLargeIcons[ui]);}
				if (hSmallIcons[ui] != NULL){::DestroyIcon(hSmallIcons[ui]);}
			}
			SAFE_DELETE(hLargeIcons);
			SAFE_DELETE(hSmallIcons);
			return false;
		}
		if((iTargetImageIndex%2)==0)
		{
			CImage imgTemp;
				bool bRet = ConvertIconToImg(hScreenDC, hLargeIcons[iTargetImageIndex/2], &imgTemp);
				CString sDataSource;
				sDataSource.Format(_T("%s: Large%d"), sFilePath, iTargetImageIndex);
				imgs->Set(IMAGE_TYPE_CIMAGE,NULL,NULL,0,0,&imgTemp,VALUE_IMAGE_RESCALE_0_TO_255, sDataSource);

				::DestroyIcon(hLargeIcons[iTargetImageIndex]);
		}
		else
		{
			CImage imgTemp;
				bool bRet = ConvertIconToImg(hScreenDC, hSmallIcons[iTargetImageIndex/2], &imgTemp);
				CString sDataSource;
				sDataSource.Format(_T("%s: Small%d"), sFilePath, iTargetImageIndex);
				imgs->Set(IMAGE_TYPE_CIMAGE,NULL,NULL,0,0,&imgTemp,VALUE_IMAGE_RESCALE_0_TO_255, sDataSource);
				::DestroyIcon(hSmallIcons[iTargetImageIndex]);
		}

		SAFE_DELETE(hLargeIcons);
		SAFE_DELETE(hSmallIcons);
		::ReleaseDC(NULL, hScreenDC);
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
		if ((uiExtracted != uiNum) && (uiExtracted != 2*uiNum)){SAFE_DELETE(hLargeIcons); SAFE_DELETE(hSmallIcons); return false;}


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

		for (UINT ui = 0; ui < uiNum; ui++)
		{
			CImage imgTemp;
			bool bRet = ConvertIconToImg(hScreenDC, hLargeIcons[ui], &imgTemp);
			CString sDataSource;
			sDataSource.Format(_T("%s: Large%d"), sFilePath, ui);
			imgs[ui].Set(IMAGE_TYPE_CIMAGE,NULL,NULL,0,0,&imgTemp,VALUE_IMAGE_RESCALE_0_TO_255, sDataSource);
			::DestroyIcon(hLargeIcons[ui]);
			imgs[ui].ResetProcessImage();
		}
		for (UINT ui = 0; ui < uiNum; ui++)
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

	bool LoadICON2(const CString sFilePath, PanImage* panImgs, const UINT uiNum)
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
		bool bRet = ConvertIconToImg(hScreenDC, hIcon1, panImgs[0].SetCImage());
		return true;
	}
