#pragma once


// CChangeColorDepthDlg ダイアログ

class CChangeColorDepthDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChangeColorDepthDlg)
	int m_iMode;
public:
	CChangeColorDepthDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CChangeColorDepthDlg();

// ダイアログ データ
	enum { IDD = IDD_DLG_CHANGE_COLOR_DEPTH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
