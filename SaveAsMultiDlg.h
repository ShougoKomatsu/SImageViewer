#pragma once
#include "afxwin.h"


// CSaveAsMultiDlg ダイアログ

class CSaveAsMultiDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSaveAsMultiDlg)

public:
	CString m_sDir;
	CString m_sPrefix1;
	CString m_sPrefix2;
	CString m_sBase;
	CString m_sSuffix1;
	CString m_sSuffix2;
	CString m_sExt;

	CSaveAsMultiDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CSaveAsMultiDlg();

// ダイアログ データ
	enum { IDD = IDD_DLG_SAVE_AS_MULTI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_sComboExt;
	CComboBox m_comboExt;
	afx_msg void OnBnClickedOk();
	CComboBox m_comboSuffix1;
	CComboBox m_comboSuffix2;
	CComboBox m_comboBase;
	CComboBox m_comboPrefix1;
	CComboBox m_comboPrefix2;

	CString m_sEditDir;
	CString m_sComboPrefix1;
	CString m_sComboPrefix2;
	CString m_sComboBase;
	CString m_sComboSuffix1;
	CString m_sComboSuffix2;
};
