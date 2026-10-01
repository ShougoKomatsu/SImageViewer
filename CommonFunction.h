#include "stdafx.h"
#pragma once
#include "ImageProc.h"
#include "resource.h"


bool CopyFromClipBoardStr(CString* sData);


bool isNearTheBoarder(double d, double dBoarder, double dMargin);
bool isInTheRange(double d, double dMin, double dMax);
bool CopyToClipBoardStr(const CString sValue);
int CountImages(const CString sFileOrFolderPath, const int iDepth, FileFormatList* fileFormatList);
bool RecursivelyGetImageFilePaths(const CString sFileOrFolderPath, const int iDepth, CStringArray* saFilePath, FileFormatList* fileFormatList);
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

bool GetDirectory(const CString sFilePath, CString* sFileDir);

bool ReadAndAppendSingllImage(CString sFilePath, FileFormatList* fileFormatList, PanImage* panImage, int iImageIndex);