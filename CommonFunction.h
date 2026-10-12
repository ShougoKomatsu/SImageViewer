#include "stdafx.h"
#pragma once

#define SAFE_DELETE(arg) if(arg != NULL){delete [] arg; arg=NULL;}


//#include "ImageProc.h"
#include "resource.h"




bool CopyFromClipBoardStr(CString* sData);


bool isNearTheBoarder(double d, double dBoarder, double dMargin);
bool isInTheRange(double d, double dMin, double dMax);
bool CopyToClipBoardStr(const CString sValue);
//bool GetOpenFileList(CString* sFilePaths);

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

bool SplitFilePath(const CString sFilePath, CString* sFileDirWithOutBS, CString* sFileNameWithoutDotExt, CString* sExt);
bool GetDirectory(const CString sFilePath, CString* sFileDir);


bool SortStrings(const CStringArray* saInput, CStringArray* saOutput);

	int CountDigit(const int iValue);
