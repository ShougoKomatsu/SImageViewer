#include "stdafx.h"
#include "CommonFunction.h"

#include "MainFrm.h"

BEGIN_MESSAGE_MAP(CPictureCtrlEx, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/*
int ViewDraw::OnLButtonUp(UINT nFlags, CPoint point_v, CWnd* wnd)
{

if (m_bDragging == TRUE) 
{
ReleaseCapture(); 
m_bDragging = false; 

CRect rect_i=GetRect_i();
if(GetPointStart_v() == point_v)
{
if(m_enumMouseMode == CHANGE_B){return -1;}
if(m_enumMouseMode == CHANGE_L){return -1;}
if(m_enumMouseMode == CHANGE_R){return -1;}
if(m_enumMouseMode == CHANGE_U){return -1;}
if(m_enumMouseMode == CHANGE_LB){return -1;}
if(m_enumMouseMode == CHANGE_LU){return -1;}
if(m_enumMouseMode == CHANGE_RB){return -1;}
if(m_enumMouseMode == CHANGE_RU){return -1;}

CRect rect_v;
rect_v = i_to_v(&rect_i);
if((point_v.y >= rect_v.top)&&(point_v.y <= rect_v.bottom)&&(point_v.x >= rect_v.left)&&(point_v.x <= rect_v.right) && (m_enumMouseMode == CHANGE_ZOOMUP))
{
ZoomChange(rect_i.top, rect_i.left, rect_i.bottom,rect_i.right,wnd);
return 0;
}

return 1;
}
rect_i = v_to_i(&(GetRect_v()));
rect_i.left = max(0,rect_i.left);
rect_i.top = max(0,rect_i.top);
SetRect_i(&rect_i);
return 2;
}
return 3;
}
bool ViewDraw::ZoomChange(int iR0_i, int iC0_i, int iR1_i, int iC1_i, CWnd* wnd)
{
int iHeight_v = GetClientHeight(wnd);
int iWidth_v = GetClientWidth(wnd);

int iNewScaleIndex = m_iScaleIndex;
for(int i = SCALE_VAR_NUM-1; i >= 0; i--)
{
if((iHeight_v>(iR1_i-iR0_i+1)*g_dScale[i]) && (iWidth_v>(iC1_i-iC0_i+1)*g_dScale[i]))
{
iNewScaleIndex = i;
break;
}
}


m_iScaleIndex = iNewScaleIndex;
if(m_iScaleIndex >= SCALE_VAR_NUM-1){m_iScaleIndex = SCALE_VAR_NUM-1;}
if(m_iScaleIndex <= 0){m_iScaleIndex = 0;}

double dNewCenterR_i = (iR0_i + iR1_i)/2.0;
double dNewCenterC_i = (iC0_i + iC1_i)/2.0;

double dNewCenterR_tv = dNewCenterR_i*g_dScale[m_iScaleIndex];
double dNewCenterC_tv = dNewCenterC_i*g_dScale[m_iScaleIndex];


int iNewScrollR_tv = max(0,int(dNewCenterR_tv-iHeight_v/2.0));
int iNewScrollC_tv = max(0,int(dNewCenterC_tv-iWidth_v/2.0));

return true; 
}

bool ViewDraw::ZoomChange(int iChange, const CImage* img, CWnd* wnd)
{
if((m_iScaleIndex >= SCALE_VAR_NUM-1)&&(iChange>0)){return false;}
if((m_iScaleIndex <= 0)&&(iChange<0)){return false;}

int iHeight_v = GetClientHeight(wnd);
int iWidth_v = GetClientWidth(wnd);
double dOldDispOriginR_tv = GetDispOriginR_tv();
double dOldDispOriginC_tv = GetDispOriginC_tv();

int iWidth_i = max(0,img->GetWidth());
int iHeight_i = max(0,img->GetHeight());

int iOldZoom = m_iScaleIndex;
double dOldWidth_tv = iWidth_i*g_dScale[iOldZoom];
double dNewDispOriginC_tv;
double dNewDispOriginR_tv;
int iNewZoom = m_iScaleIndex+= iChange;
double dNewWidth_tv = iWidth_i*g_dScale[iNewZoom];
double dNewHeight_tv = iHeight_i*g_dScale[iNewZoom];


if(dNewWidth_tv>iWidth_v)
{
double dOldCenterC_i = (dOldDispOriginC_tv +iWidth_v/2.0)/g_dScale[iOldZoom];
double dOldEndC_i = (dOldDispOriginC_tv +iWidth_v)/g_dScale[iOldZoom];
dNewDispOriginC_tv = max(0, min(dOldCenterC_i*g_dScale[iNewZoom] - iWidth_v/2.0, dOldEndC_i*g_dScale[iNewZoom]-iWidth_v));
}
else
{
dNewDispOriginC_tv = 0;
}

if(dNewHeight_tv>iHeight_v)
{
double dOldCenterR_i = (dOldDispOriginR_tv +iHeight_v/2.0)/g_dScale[iOldZoom];
double dOldEndR_i = (dOldDispOriginR_tv +iHeight_v)/g_dScale[iOldZoom];
dNewDispOriginR_tv = max(0, min(dOldCenterR_i*g_dScale[iNewZoom] - iHeight_v/2.0, dOldEndR_i*g_dScale[iNewZoom]-iHeight_v));
}
else
{
dNewDispOriginR_tv = 0;
}
m_iScaleIndex = iNewZoom;


SetScrollPos((int)(dNewDispOriginR_tv), (int)(dNewDispOriginC_tv));

if(m_iScaleIndex >= SCALE_VAR_NUM-1){m_iScaleIndex = SCALE_VAR_NUM-1;}
if(m_iScaleIndex <= 0){m_iScaleIndex = 0;}

return true; 
}
*/

void CPictureCtrlEx::OnMouseMove(UINT nFlags, CPoint point)
{

	CRect rect;
	this->GetWindowRect(&rect);		
	this->GetParent()->ScreenToClient(&rect);
	if(point.x<rect.left){return;}
	if(point.x>rect.right){return;}
	if(point.y<rect.top){return;}
	if(point.y>rect.bottom){return;}

	CStatic::OnMouseMove(nFlags, point);
}

void CPictureCtrlEx::OnPaint()
{
	CPaintDC dc(this);
	CDC* pDC = this->GetDC();

	CRect rc;
	GetClientRect(&rc);

	if (m_image.IsNull()==true){return;}

	double dZoom=min(rc.Width()/(m_image.GetWidth()*1.0),rc.Height()/(m_image.GetHeight()*1.0));

	CImage imgZoomed;


	view.ZoomChange(0, 0, m_image.GetHeight()-1, m_image.GetWidth()-1, &m_image, this, false);
	PanImage panImage;
	panImage.Set(IMAGE_TYPE_CIMAGE, NULL, NULL, 0, 0, &m_image, VALUE_IMAGE_CLIP_0_TO_255, _T("temp"));
	view.OnDraw(this, pDC, &m_image, &panImage, false);
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




bool IsImageFIle(CString sFilePath, FileFormatList* fileFormatList)
{
	if(sFilePath.Right(4).CompareNoCase(_T(".bmp"))==0){return true;}
	if(sFilePath.Right(4).CompareNoCase(_T(".png"))==0){return true;}
	if(sFilePath.Right(4).CompareNoCase(_T(".jpg"))==0){return true;}
	if(sFilePath.Right(4).CompareNoCase(_T(".ico"))==0){return true;}
	if(sFilePath.Right(4).CompareNoCase(_T(".exe"))==0){return true;}
	if(sFilePath.Right(4).CompareNoCase(_T(".dll"))==0){return true;}
	for(int i=0; i<fileFormatList->uiNum; i++)
	{
		if(sFilePath.Right(fileFormatList->fileFormat[i].sType.GetLength()).CompareNoCase(fileFormatList->fileFormat[i].sType)==0){return true;}
	}

	return false;
}

UINT CountImageInOneFile(CString sFilePath, FileFormatList* fileFormatList)
{
	if(sFilePath.Right(4).CompareNoCase(_T(".bmp"))==0){return 1;}
	if(sFilePath.Right(4).CompareNoCase(_T(".png"))==0){return 1;}
	if(sFilePath.Right(4).CompareNoCase(_T(".jpg"))==0){return 1;}
	if(sFilePath.Right(4).CompareNoCase(_T(".ico"))==0){return 2*CountIconNum(sFilePath); }
	if(sFilePath.Right(4).CompareNoCase(_T(".exe"))==0){return 2*CountIconNum(sFilePath); }
	if(sFilePath.Right(4).CompareNoCase(_T(".dll"))==0){return 2*CountIconNum(sFilePath); }
	for(int i=0; i<fileFormatList->uiNum; i++)
	{
		if(sFilePath.Right(fileFormatList->fileFormat[i].sType.GetLength()).CompareNoCase(fileFormatList->fileFormat[i].sType)==0){return 1;}
	}
	return 0;
}
bool RecursivelyGetImageFilePaths(CString sFileOrFolderPath, CStringArray* saFilePath, FileFormatList* fileFormatList)
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
			bool bRet = RecursivelyGetImageFilePaths(saFilePathTemp.GetAt(i), saFilePath, fileFormatList);
			if(bRet != true){return false;}
		}
		return true;
	}

	DWORD dwAttribute = GetFileAttributes(sFileOrFolderPath);
	if (dwAttribute == INVALID_FILE_ATTRIBUTES) {return false;}

	if ((dwAttribute & FILE_ATTRIBUTE_DIRECTORY) == 0) 
	{
		if(IsImageFIle(sFileOrFolderPath, fileFormatList)==true)
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
			bool bRet = RecursivelyGetImageFilePaths(sFilePath, saFilePath, fileFormatList);
			if(bRet != true){return false;}
		}
		else 
		{
			if(IsImageFIle(sFilePath, fileFormatList) != true){continue;}
			saFilePath->Add(sFilePath);
		}
	}

	cf.Close();
	return true;
}

int CountImages(CString sFileOrFolderPath, FileFormatList* fileFormatList)
{
	CStringArray saFilePath;
	saFilePath.RemoveAll();
	bool bRet = RecursivelyGetImageFilePaths(sFileOrFolderPath, &saFilePath, fileFormatList);
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
	for(int i=0; i<fileFormatList->uiNum; i++)
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
		bool bRet = LoadICOFile(sFilePath,panImage,uiIconNum);
		if(bRet != true){return false;}
		*iImageIndexNew = iImageIndex+uiIconNum;
		return true;
	}
	HRESULT hResult = panImage->SetCImage()->Load(sFilePath);
	if(hResult != S_OK){return false;}
	panImage->SetImageType(IMAGE_TYPE_CIMAGE);
	panImage->SetDataSource(sFilePath);
	panImage->ResetProcessImage();

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
