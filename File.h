#include "stdafx.h"
#pragma once
#include "CommonFunction.h"
#include "ImageProc.h"

struct FileFormat
{
	CString sType;
	int iWidth;
	int iHeight;
	int iDataOffset;

	int iWidthInfoOffset;
	int iHeightInfoOffset;
	int iDataInfoOffset;
	int iDataOffsetOffset;
	void Copy(FileFormat* fileFormat_in)
	{
		sType.Format(_T("%s"), fileFormat_in->sType);
		iWidth=fileFormat_in->iWidth;
		iHeight=fileFormat_in->iHeight;
		iDataOffset=fileFormat_in->iDataOffset;

		iWidthInfoOffset=fileFormat_in->iWidthInfoOffset;
		iHeightInfoOffset=fileFormat_in->iHeightInfoOffset;
		iDataInfoOffset=fileFormat_in->iDataInfoOffset;
		iDataOffsetOffset=fileFormat_in->iDataOffsetOffset;
	}
};
struct FileFormatList
{
	FileFormat* fileFormat;
	UINT uiNum;
	FileFormatList()
	{
		fileFormat=NULL;
	}
	void Init()
	{
		uiNum=0;
		SAFE_DELETE(fileFormat);
	}
	~FileFormatList()
	{
		Init();
	}
	void Set(UINT uiNum_in)
	{
		Init();
		fileFormat = new FileFormat[uiNum_in];
		uiNum=uiNum_in;
	}
	void Copy(FileFormatList* fileFormatList_in)
	{
		Init();
		this->Set(fileFormatList_in->uiNum);
		for(UINT i=0; i<uiNum; i++)
		{
			fileFormat[i].Copy(&(fileFormatList_in->fileFormat[i]));
		}
	}

};

bool GetFileFormat(const CString sIniFilePath, const CString sType, FileFormat* fileFormat);
bool ReadBinaryFile(const CString sFilePath, FileFormatList* fileFormatList, PanImage* panImgDst);
bool WriteFileFormat(const CString sIniFilePath, FileFormatList* fileFormatList);
enum TYPE_IMAGE_FILE
{
	IMAGE_FILE_TYPE_UNDEFINED = 0,
	IMAGE_FILE_TYPE_SINGLE = 1,
	IMAGE_FILE_TYPE_MULTI = 2,
};

TYPE_IMAGE_FILE GetTypeOfImageFIle(CString sFilePath, FileFormatList* fileFormatList);

int CountImages(const CString sFileOrFolderPath, const int iDepth, FileFormatList* fileFormatList);
bool RecursivelyGetImageFilePaths(const CString sFileOrFolderPath, const int iDepth, CStringArray* saFilePath, FileFormatList* fileFormatList);
bool ReadAndAppendImage(CString sFilePath, FileFormatList* fileFormatList, PanImage* panImage, int iImageIndex, int* iImageIndexNew);
bool ReadAndAppendSingllImage(CString sFilePath, FileFormatList* fileFormatList, PanImage* panImage, int iImageIndex);


UINT CountIconNum(const CString sFilePath);
bool LoadICOFile(const CString sFilePath, PanImage* panImg, UINT uiNum);
bool LoadICON2(const CString sFilePath, PanImage* panImgs, UINT uiNum);
bool LoadICOFileSingle(const CString sFilePath, PanImage* imgs, const int iTargetImageIndex);