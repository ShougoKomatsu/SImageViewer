#pragma once
#include "afxcmn.h"


// CProgressDlg ダイアログ

class CProgressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	int m_iStatus;
	void SetPos(int i0to100);
	void SetText(const CString sText);

	CProgressDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CProgressDlg();

// ダイアログ データ
	enum { IDD = IDD_DLG_PROGRESS };
	
        void OnOK();
        void OnCancel();
        void PostNcDestroy();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CProgressCtrl m_progress;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CString m_sStatic;
};
