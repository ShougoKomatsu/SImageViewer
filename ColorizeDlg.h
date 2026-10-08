#pragma once

#include "PictureCtrlEx.h"
#include "ImageProc.h"

// CColorizeDlg ダイアログ

enum Colorize
{
	Colorize_threshold = 1,
	Colorize_Demosaic = 2,
	Colorize_rainbow = 3,
};

class CColorizeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CColorizeDlg)

public:
	Colorize m_colorize;

	COLOR_ELEMENT m_i00, m_i01, m_i10, m_i11;
	BYTE m_byMin ;
	BYTE m_byMax ;
	int m_iNeighbor;
	bool m_bConnection;

	CImage m_image;
	CImage m_imageColorized;
	CPictureCtrlEx m_pictureBefore;
	CPictureCtrlEx m_pictureAfter;
	CColorizeDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	void OperateRainbow();
	void OperateThreshold();
	void OperateDemosaic();
	virtual ~CColorizeDlg();
	void UpdateResultImage(CImage* imgResult);
	void EnableThreshold(bool bTF);
	void EnableDemosaic(bool bTF);

// ダイアログ データ
	enum { IDD = IDD_DLG_COLORIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnChangeColorizeEditValue1();
	afx_msg void OnChangeColorizeEditValue2();
	virtual BOOL OnInitDialog();
	CString m_sEditThreshMin;
	CString m_sEditThreshMax;
	afx_msg void OnBnClickedColorizeCheckConnection();
	afx_msg void OnBnClickedColorizeRadioConnection4();
	afx_msg void OnBnClickedColorizeRadioConnection8();
	afx_msg void OnBnClickedColorizeRadioDemosaic();
	afx_msg void OnBnClickedColorizeRadioThreshold();
	afx_msg void OnSelchangeColorizeCombo();
	afx_msg void OnBnClickedColorizeRadioRainbow();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
