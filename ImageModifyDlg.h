#pragma once

#include "CommonFunction.h"
#include "ImageProc.h"
// CImageModifyDlg ダイアログ

class CImageModifyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CImageModifyDlg)

public:
	CImage m_image;
	CPictureCtrlEx m_pictureBefore;
	CPictureCtrlEx m_pictureAfter;
	int m_iBrightness;
	int m_iContrast;
	double m_dGamma;
	CSliderCtrl m_sliderBrightness;
	CSliderCtrl m_sliderContrast;
	CSliderCtrl m_sliderGamma;
	int m_iRet;
	CImageModifyDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CImageModifyDlg();
	void UpdateResultImage();

// ダイアログ データ
	enum { IDD = IDD_DLG_IMAGE_MODIFY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CString m_sEditBrightness;
	CString m_sEditContrast;
	CString m_sEditGamma;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedDlgImageModifyButtonOk();
	afx_msg void OnBnClickedDlgImageModifyButtonCancel();
	afx_msg void OnCustomdrawSliderImageModifyBrightness(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomdrawSliderImageModifyContrast(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomdrawSliderImageModifyGamma(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKillfocusEditImageModifyBrightness();
	afx_msg void OnKillfocusEditImageModifyContrast();
	afx_msg void OnKillfocusEditImageModifyGamma();
};
