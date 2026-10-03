#pragma once

#include "CommonFunction.h"
#include "EditEx.h"
// CInputDlg ダイアログ

class CInputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInputDlg)

	CEditEx m_editInput;
public:
	CInputDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CInputDlg();

// ダイアログ データ
	enum { IDD = IDD_DLG_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CString m_sEditInput;
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
