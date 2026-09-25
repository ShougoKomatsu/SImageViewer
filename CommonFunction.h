#include "stdafx.h"
#pragma once
#include "ImageProc.h"
#include "resource.h"
#include "ViewDraw.h"


class CPictureCtrlEx : public CStatic
{
public:
	ViewDraw view;
	PanImage m_image;
	int iID;

		double GetDispOriginR_tv()
	{
		return view.GetDispOriginR_tv();
	}
	double GetDispOriginC_tv()
	{
		return view.GetDispOriginC_tv();
	}



	void Refresh(){OnPaint();Invalidate();}
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

bool index_i(const int* iValues, const int iLength, int* iIndex);
bool index_i(const ULONGLONG* iValues, const int iLength, int* iIndex);
void QuickSortIndex(const int* iValues, int* iIndex, const int iL, const int iR);
void QuickSortIndex(const ULONGLONG* iValues, int* iIndex, const int iL, const int iR);

inline void SwapInt(int *a, int *b)
{
	int iTemp = *a;
	*a = *b;
	*b = iTemp;
}

