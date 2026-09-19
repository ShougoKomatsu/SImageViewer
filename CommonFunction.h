#include "stdafx.h"
#pragma once
#include "ImageProc.h"


#define SCALE_VAR_NUM (25)
extern double g_dScale[SCALE_VAR_NUM];

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
    CImage m_image;

	Line m_HBar1_v;
	Line m_HBar2_v;
	Line m_VBar1_v;
	Line m_VBar2_v;
	
	Line m_HBar1_i;
	Line m_HBar2_i;
	Line m_VBar1_i;
	Line m_VBar2_i;

	int m_iScaleIndex;
	int m_iMouseMode;
	double m_dDispOriginR_tv;
	double m_dDispOriginC_tv;

	double GetDispOriginR_tv()
	{
		return m_dDispOriginR_tv;
	}
	double GetDispOriginC_tv()
	{
		return m_dDispOriginC_tv;
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
			((line_v->dR0+ iROrigin_tv) / g_dScale[m_iScaleIndex]) +0.5
			,((line_v->dC0+ iCOrigin_tv) / g_dScale[m_iScaleIndex]) +0.5
			,((line_v->dR0+ iROrigin_tv) / g_dScale[m_iScaleIndex]) -0.5
			,((line_v->dC1+ iCOrigin_tv) / g_dScale[m_iScaleIndex]) -0.5);

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
			((line_i->dR0) * g_dScale[m_iScaleIndex])-iROrigin_tv
			,((line_i->dC0) * g_dScale[m_iScaleIndex])-iCOrigin_tv
			,((line_i->dR1+1) * g_dScale[m_iScaleIndex])-iROrigin_tv
			,((line_i->dC1+1 ) * g_dScale[m_iScaleIndex])-iCOrigin_tv);

		return line;
	}


	CPictureCtrlEx()
	{
		m_dDispOriginR_tv=0;
		m_dDispOriginC_tv=0;
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

