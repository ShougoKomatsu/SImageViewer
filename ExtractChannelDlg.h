#pragma once
#include "ImageProc.h"

// CExtractChannelDlg ダイアログ

class CExtractChannelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CExtractChannelDlg)

public:
	ENUM_COLOR m_enumColor;
	CExtractChannelDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CExtractChannelDlg();

// ダイアログ データ
	enum { IDD = IDD_DLG_EXTRACT_CHANNEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
