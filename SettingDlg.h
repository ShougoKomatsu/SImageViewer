#pragma once


// CSettingDlg ダイアログ

class CSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CSettingDlg();
	bool m_bExitByEsc;
	bool m_bCenteredWhenFullScreen;
	bool m_bPPFWwithoutCtrlWhenFullScreen;
// ダイアログ データ
	enum { IDD = IDD_DLG_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
