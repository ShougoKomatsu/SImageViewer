#pragma once
#include "EditEx.h"

// CFilterSettingDlg ダイアログ

class CFilterSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFilterSettingDlg)

public:
	CFilterSettingDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CFilterSettingDlg();

// ダイアログ データ
	enum { IDD = IDD_DLG_FILTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	CString m_sEditW;
	CString m_sEditH;
	CEditEx m_editW;
	CEditEx m_editH;
	afx_msg void OnBnClickedOk();
};
