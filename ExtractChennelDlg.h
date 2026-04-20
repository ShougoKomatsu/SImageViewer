#pragma once
#include "ImageProc.h"

// CExtractChennelDlg ダイアログ

class CExtractChennelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CExtractChennelDlg)

public:
	ENUM_COLOR m_enumColor;
	CExtractChennelDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CExtractChennelDlg();

// ダイアログ データ
	enum { IDD = IDD_DLG_EXTRACT_CHANNEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
