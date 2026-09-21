#include "stdafx.h"
#pragma once
#include "ImageProc.h"
#include "resource.h"
#include "ViewDraw.h"


class CPictureCtrlEx : public CStatic
{
public:
	ViewDraw view;
	CImage m_image;


	const double GetDispOriginR_tv(){return view.GetDispOriginR_tv();}
	const double GetDispOriginC_tv(){return view.GetDispOriginC_tv();}

	CPictureCtrlEx()
	{
	}

protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

bool CopyFromClipBoardStr(CString* sData);


bool isNearTheBoarder(double d, double dBoarder, double dMargin);
bool isInTheRange(double d, double dMin, double dMax);
bool CopyToClipBoardStr(const CString sValue);
int CountImages(CString sFileOrFolderPath, FileFormatList* fileFormatList);
bool RecursivelyGetImageFilePaths(CString sFileOrFolderPath, CStringArray* saFilePath, FileFormatList* fileFormatList);
bool ReadAndAppendImage(CString sFilePath, FileFormatList* fileFormatList, PanImage* panImage, int iImageIndex, int* iImageIndexNew);
bool GetOpenFileList(CString* sFilePaths);


void QuickSortIndex(const ULONGLONG* iValues, int* iIndex, const int iL, const int iR);
bool index_i(const ULONGLONG* iValues, const int iLength, int* iIndex);

inline void SwapInt(int *a, int *b)
{
	int iTemp = *a;
	*a = *b;
	*b = iTemp;
}

