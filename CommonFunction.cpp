#include "stdafx.h"
#include "CommonFunction.h"

#include "MainFrm.h"


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

bool GetDirectory(const CString sFilePath, CString* sFileDir)
{
	int iPlace = sFilePath.ReverseFind('\\');
	if(iPlace<0){return false;}
	sFileDir->Format(_T("%s"),sFilePath.Left(iPlace)); 
	return true;
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

void PanImage::ResetProcessImage()
{
	for(int i=0; i<MAX_IMG_PROCESS; i++)
	{
		if(m_imageProcessed[i].IsNull() != true){m_imageProcessed[i].Destroy();}
	}
	CopyImage_CImage(&cImage, &(m_imageProcessed[0]));
	m_iImgProcessIndex=0;
	m_iReDoAvailableCount=0;
	m_iUnDoAvailableCount=0;
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

bool SortStrings(const CStringArray* saInput, CStringArray* saOutput)
{
	if(saInput==NULL){return false;}
	if(saOutput==NULL){return false;}
	if(saInput->GetCount()<=0){saOutput->RemoveAll(); return false;}

	CStringArray* psaOutput;
	CStringArray saTemp;
	if(saInput==saOutput){psaOutput=&saTemp;}
	else{psaOutput = saOutput;}

	psaOutput->RemoveAll();
	psaOutput->Add(saInput->GetAt(0));
	for(int iSrc=1; iSrc<saInput->GetCount(); iSrc++)
	{
		CString sSrc(saInput->GetAt(iSrc));
		bool bInserted=false;
		for(int iDst=0; iDst<psaOutput->GetCount(); iDst++)
		{
			CString sTest(psaOutput->GetAt(iDst));
			int iRet = CompareString(LOCALE_SYSTEM_DEFAULT, SORT_DIGITSASNUMBERS, sTest, sTest.GetLength(), sSrc,  sSrc.GetLength());
			if(iRet==CSTR_GREATER_THAN)
			{
				bInserted=true;
				psaOutput->InsertAt(iDst, sSrc);
				break;
			}
		}
		if(bInserted==false){psaOutput->Add(sSrc);}
	}

	if(saInput==saOutput){saOutput->Copy(saTemp);}

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

void QuickSortIndex(const int* iValues, int* iIndex, const int iL, const int iR)
{
	int iL_Local=iL;
	int iR_Local=iR;
	int iPivot = iValues[iIndex[(iL_Local + iR_Local) / 2]];

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

bool index_i(const int* iValues, const int iLength, int* iIndex)
{
	for (int i = 0; i < iLength; i++) {iIndex[i] = i;}

	QuickSortIndex(iValues, iIndex, 0, iLength - 1);
	return true;
}
bool index_i(const ULONGLONG* iValues, const int iLength, int* iIndex)
{
	for (int i = 0; i < iLength; i++) {iIndex[i] = i;}

	QuickSortIndex(iValues, iIndex, 0, iLength - 1);
	return true;
}

bool GetOpenFileList(CString* sFilePaths)
{

	CFileDialog cf(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT , _T(""));
	TCHAR* tchBuf=NULL;
	tchBuf = new TCHAR[100*MAX_PATH];
	for(int i=0; i<100*MAX_PATH; i++)
	{
		tchBuf[i]='\0';
	}
	cf.m_ofn.lpstrFile=tchBuf;

	//		cf.m_ofn.lpstrInitialDir = sMacroFolderPath;		
	if(cf.DoModal()!=IDOK){ SAFE_DELETE(tchBuf); return false;}

	POSITION pos = cf.GetStartPosition();
	int iNum=0;
	while(pos)
	{
		cf.GetNextPathName(pos);
		iNum++;
	}
	if(iNum <= 0){SAFE_DELETE(tchBuf); return false;}

	sFilePaths->Format(_T(""));
	pos=cf.GetStartPosition();
	for(int i=0; i<iNum-1; i++)
	{
		CString sTemp;
		sTemp.Format(_T("%s|"), cf.GetNextPathName(pos));
		sFilePaths->Append(sTemp);
	}
	sFilePaths->Append(cf.GetNextPathName(pos));
	SAFE_DELETE(tchBuf); 
	return true;
}

bool GetImageTypeNum(const CString sIniFilePath, UINT* uiTypeNum)
{
	const UINT uiBufSize=128;
	TCHAR tchData[uiBufSize];

	int i=0;
	while(1)
	{
		CString sKey;
		sKey.Format(_T("Type%d"), i+1);
		GetPrivateProfileString(_T("Types"), sKey, _T(""), tchData, uiBufSize, sIniFilePath);
		if(_tcslen(tchData)<=0){break;}
		i++;
	}
	*uiTypeNum = i;
	return true;
}

bool GetImageType(const CString sIniFilePath, const int iIndexB0, CString* sType)
{
	const UINT uiBufSize=128;
	TCHAR tchData[uiBufSize];

	CString sKey;
	sKey.Format(_T("Type%d"), iIndexB0+1);
	GetPrivateProfileString(_T("Types"), sKey, _T(""), tchData, uiBufSize, sIniFilePath);
	if(_tcslen(tchData)<=0){return false;}


	sType->Format(_T("%s"), tchData);
	return true;
}


bool CopyFromClipBoardStr(CString* sData)
{
	BOOL bRet;

	bRet = OpenClipboard(NULL);
	if(bRet == FALSE){return false;}

	HANDLE hResult;

	hResult = GetClipboardData(CF_UNICODETEXT );
	if(hResult == NULL){return false;}
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
	sData->Format(_T("%s"),byData);
	SAFE_DELETE(byData); 

	return true;
}
