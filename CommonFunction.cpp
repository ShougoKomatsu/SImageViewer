#include "stdafx.h"
#include "CommonFunction.h"
BEGIN_MESSAGE_MAP(CPictureCtrlEx, CStatic)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void CPictureCtrlEx::OnPaint()
{
    CPaintDC dc(this);

    CRect rc;
    GetClientRect(&rc);

    if (m_image.IsNull()==true){return;}

	double dZoom=min(rc.Width()/(m_image.GetWidth()*1.0),rc.Height()/(m_image.GetHeight()*1.0));
	
	CImage imgZoomed;
		ZoomImage(&(m_image),&imgZoomed,
			0,
			0,
			dZoom,
			rc.Width(), rc.Height(),false);

		
		imgZoomed.BitBlt( dc.GetSafeHdc(), 0, 0,imgZoomed.GetWidth(), imgZoomed.GetHeight(), 0, 0  );
}
bool CopyToClipBoardStr(const CString sValue)
{
	BOOL bRet;
	bRet = OpenClipboard(NULL);
	if(bRet == FALSE){return FALSE;}

	bRet = EmptyClipboard();
	if(bRet == FALSE){return FALSE;}
	
	HGLOBAL hGL;
	hGL = GlobalAlloc(GPTR, (sValue.GetLength()+1)*sizeof(TCHAR) );
	if(hGL==NULL){return FALSE;}

	_stprintf_s((TCHAR*)hGL,(sValue.GetLength()+1), _T("%s"), sValue);
	
	HANDLE hResult;
	hResult = SetClipboardData(CF_UNICODETEXT, hGL);
	if(hResult == NULL)
	{
		GlobalFree(hGL);
		return FALSE;
	}

	bRet = CloseClipboard();
	if(bRet == FALSE)
	{
		GlobalFree(hGL);
		return FALSE;
	}
	return TRUE;
}




	bool IsImageFIle(CString sFilePath)
	{
		if(sFilePath.Right(4).CompareNoCase(_T(".bmp"))==0){return true;}
		if(sFilePath.Right(4).CompareNoCase(_T(".png"))==0){return true;}
		if(sFilePath.Right(4).CompareNoCase(_T(".jpg"))==0){return true;}
		if(sFilePath.Right(4).CompareNoCase(_T(".ico"))==0){return true;}
		if(sFilePath.Right(4).CompareNoCase(_T(".exe"))==0){return true;}
		if(sFilePath.Right(4).CompareNoCase(_T(".dll"))==0){return true;}
		return false;
	}
	
	UINT CountImageInOneFile(CString sFilePath)
	{
		if(sFilePath.Right(4).CompareNoCase(_T(".bmp"))==0){return 1;}
		if(sFilePath.Right(4).CompareNoCase(_T(".png"))==0){return 1;}
		if(sFilePath.Right(4).CompareNoCase(_T(".jpg"))==0){return 1;}
		if(sFilePath.Right(4).CompareNoCase(_T(".ico"))==0){return 2*CountIconNum(sFilePath); }
		if(sFilePath.Right(4).CompareNoCase(_T(".exe"))==0){return 2*CountIconNum(sFilePath); }
		if(sFilePath.Right(4).CompareNoCase(_T(".dll"))==0){return 2*CountIconNum(sFilePath); }
		return 0;
	}
	bool RecursivelyGetImageFilePaths(CString sFileOrFolderPath, CStringArray* saFilePath)
	{
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
				bool bRet = RecursivelyGetImageFilePaths(saFilePathTemp.GetAt(i), saFilePath);
				if(bRet != true){return false;}
			}
			return true;
		}

		DWORD dwAttribute = GetFileAttributes(sFileOrFolderPath);
		if (dwAttribute == INVALID_FILE_ATTRIBUTES) {return false;}

		if ((dwAttribute & FILE_ATTRIBUTE_DIRECTORY) == 0) 
		{
			if(IsImageFIle(sFileOrFolderPath)==true)
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

		while (bWorking) 
		{
			bWorking = cf.FindNextFile();
			if (cf.IsDots() == TRUE){continue;}

			CString sFilePath = cf.GetFilePath();

			if (cf.IsDirectory()) 
			{
				bool bRet = RecursivelyGetImageFilePaths(sFilePath, saFilePath);
				if(bRet != true){return false;}
			}
			else 
			{
				if(IsImageFIle(sFilePath) != true){continue;}
				saFilePath->Add(sFilePath);
			}
		}

		cf.Close();
		return true;
	}

	int CountImages(CString sFileOrFolderPath)
	{
		CStringArray saFilePath;
		saFilePath.RemoveAll();
		bool bRet = RecursivelyGetImageFilePaths(sFileOrFolderPath, &saFilePath);
		if(bRet != true){return 0;}

		int iFileNum = (int)saFilePath.GetCount();
		int iImageCount=0;
		for(int i=0; i<iFileNum; i++)
		{
			iImageCount += CountImageInOneFile(saFilePath.GetAt(i));
		}
		return iImageCount;
	}

	bool ReadAndAppendImage(CString sFilePath, PanImage* panImage, int iImageIndex, int* iImageIndexNew)
	{
		if(((sFilePath.Right(4)).CompareNoCase(_T(".ico"))==0)
			||((sFilePath.Right(4)).CompareNoCase(_T(".exe"))==0)
			||((sFilePath.Right(4)).CompareNoCase(_T(".dll"))==0))
		{
		
			UINT uiIconNum = CountIconNum(sFilePath);
			bool bRet = LoadICOFile(sFilePath,panImage,uiIconNum);
			if(bRet != true){return false;}
			*iImageIndexNew = iImageIndex+uiIconNum;
			return true;
		}
		HRESULT hResult = panImage->cImage.Load(sFilePath);
		if(hResult != S_OK){return false;}
		panImage->enumImageType=IMAGE_TYPE_CIMAGE;
		*iImageIndexNew = iImageIndex+1;
		return true;
	}
	
	bool isNearTheBoarder(double d, double dBoarder, double dMargin)
	{
		if(d<dBoarder-dMargin){return false;}
		if(d>dBoarder+dMargin){return false;}
		return true;
	}
	bool isInTheRange(double d, double dMin, double dMax)
	{
		if(d<dMin){return false;}
		if(d>dMax){return false;}
		return true;
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