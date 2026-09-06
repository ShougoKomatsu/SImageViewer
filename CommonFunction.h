#include "stdafx.h"
#pragma once
#include "ImageProc.h"
class CPictureCtrlEx : public CStatic
{
public:
    CImage m_image;

protected:
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};



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

