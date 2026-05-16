#pragma once


// CChangeColorDepthDlg ダイアログ

class CChangeColorDepthDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChangeColorDepthDlg)
	int m_iBPP;
	int m_iMode;
	int m_iColors;
	bool m_bGrayScale;
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
	virtual BOOL OnInitDialog();
	CString m_sEditBPP;
	CString m_sEditColors;
	CString m_sEditGrayScale;
};
