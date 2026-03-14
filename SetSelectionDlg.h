#pragma once


// CSetSelectionDlg ダイアログ

class CSetSelectionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetSelectionDlg)

public:
	CSetSelectionDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CSetSelectionDlg();
	int m_iR0;
	int m_iR1;
	int m_iC0;
	int m_iC1;

// ダイアログ データ
	enum { IDD = IDD_DIALOG_SET_SELECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CString m_sEditTop;
	CString m_sEditLeft;
	CString m_sEditBottom;
	CString m_sEditRight;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
