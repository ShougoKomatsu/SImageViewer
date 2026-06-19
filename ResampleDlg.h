#pragma once

#include "ImageProc.h"
// CResampleDlg ダイアログ

class CResampleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CResampleDlg)

public:
	CResampleDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CResampleDlg();
	
void ChangeMode(RESAMPLE resample);
	RESAMPLE m_resample;
	int m_iWidth;
	int m_iHeight;
	int m_iWidthOrg;
	int m_iHeightOrg;
	// ダイアログ データ
	enum { IDD = IDD_DLG_RESAMPLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CString m_sEditWidth;
	CString m_sEditHeight;
	CString m_sEditWidthOrg;
	CString m_sEditHeightOrg;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedResampleRadioResizeNearest();
	afx_msg void OnBnClickedResampleRadioResizeBilinear();
	afx_msg void OnBnClickedResampleRadio4();
	afx_msg void OnBnClickedResampleRadio3();
	afx_msg void OnBnClickedResampleRadio2();
	afx_msg void OnBnClickedResampleRadio1();
	afx_msg void OnBnClickedResampleRadio2nd();
	afx_msg void OnBnClickedResampleRadio3rd();
	afx_msg void OnBnClickedResampleRadio4th();
};
