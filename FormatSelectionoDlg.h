#pragma once


// CFormatSelectionoDlg ダイアログ

class CFormatSelectionoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFormatSelectionoDlg)


public:
	bool m_bImageIsMonochrome;
	CFormatSelectionoDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CFormatSelectionoDlg();
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
};
