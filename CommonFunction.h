#include "stdafx.h"
#pragma once
#include "ImageProc.h"
#include "resource.h"
#include "ViewDraw.h"
struct Line
{
	double dR0, dR1, dC0, dC1;
	bool bValid;
	Line(){Init();}
	~Line(){Init();}
	void Init(){bValid=false;}
	void Set(const double dR0_in, const double dC0_in, const double dR1_in, const double dC1_in)
	{
		dR0=dR0_in;
		dR1=dR1_in;
		dC0=dC0_in;
		dC1=dC1_in;
		bValid=true;
	}

	const Line operator = (const Line line_in)
	{
		Line line;
		line.Set(line_in.dR0, line_in.dC0, line_in.dR1, line_in.dC1);
		return line;
	}
};


class CPictureCtrlEx : public CStatic
{
public:
	ViewDraw view;
	CImage m_image;

	Line m_HBar1_v;
	Line m_HBar2_v;
	Line m_VBar1_v;
	Line m_VBar2_v;

	Line m_HBar1_i;
	Line m_HBar2_i;
	Line m_VBar1_i;
	Line m_VBar2_i;


	double GetDispOriginR_tv()
	{
		return view.GetDispOriginR_tv();
	}
	double GetDispOriginC_tv()
	{
		return view.GetDispOriginC_tv();
	}


	const Line v_to_i(const Line* line_v)
	{
		Line line;
		if(line_v->bValid == false)
		{
			line.Init();
			return line;
		}

		int iCOrigin_tv = (int)(GetDispOriginC_tv());
		int iROrigin_tv = (int)(GetDispOriginR_tv());

		line.Set(
			((line_v->dR0+ iROrigin_tv) / view.GetScale()) +0.5
			,((line_v->dC0+ iCOrigin_tv) / view.GetScale()) +0.5
			,((line_v->dR0+ iROrigin_tv) / view.GetScale()) -0.5
			,((line_v->dC1+ iCOrigin_tv) / view.GetScale()) -0.5);

		double dTemp;
		dTemp=line.dR0;
		line.dR0=min(line.dR0, line.dR1);
		line.dR1=max(dTemp, line.dR1);

		dTemp=line.dC0;
		line.dC0=min(line.dC0, line.dC1);
		line.dC1=max(dTemp, line.dC1);
		return line;
	}

	const Line i_to_v(const Line* line_i)
	{
		Line line;
		if(line_i->bValid == false)
		{
			line.Init();
			return line;
		}

		int iCOrigin_tv = (int)GetDispOriginC_tv();
		int iROrigin_tv = (int)GetDispOriginR_tv();

		line.Set(
			((line_i->dR0) * view.GetScale())-iROrigin_tv
			,((line_i->dC0) * view.GetScale())-iCOrigin_tv
			,((line_i->dR1+1) * view.GetScale())-iROrigin_tv
			,((line_i->dC1+1 ) * view.GetScale())-iCOrigin_tv);

		return line;
	}


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

