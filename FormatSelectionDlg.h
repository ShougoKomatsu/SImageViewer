#pragma once


// CFormatSelectionDlg ダイアログ

class CFormatSelectionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFormatSelectionDlg)


public:
	bool m_bImageIsMonochrome;
	CFormatSelectionDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CFormatSelectionDlg();
	int m_iFormat;
	int m_iBPP;
// ダイアログ データ
	enum { IDD = IDD_DIALOG_SELECT_FORMAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_sEditIsMonochrome;
	afx_msg void OnBnClickedOk();
	CString m_sBPP;
};
