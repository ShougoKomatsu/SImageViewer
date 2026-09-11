#pragma once

#include "CommonFunction.h"
#include "ImageProc.h"

// CColorizeDlg ダイアログ

class CColorizeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CColorizeDlg)

public:
	CImage m_image;
	CImage m_imageColorized;
	CPictureCtrlEx m_pictureBefore;
	CPictureCtrlEx m_pictureAfter;
	CColorizeDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	void OperateThreshold();
	virtual ~CColorizeDlg();
	void UpdateResultImage(CImage* imgResult);

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
	CString m_sEditValue1;
	CString m_sEditValue2;
};
