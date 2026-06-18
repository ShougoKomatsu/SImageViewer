#pragma once

#include "ImageProc.h"
// CResampleDlg ダイアログ

class CResampleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CResampleDlg)

public:
	CResampleDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CResampleDlg();

	RESAMPLE m_resample;
	// ダイアログ データ
	enum { IDD = IDD_DLG_RESAMPLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
