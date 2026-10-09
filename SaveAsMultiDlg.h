#pragma once


// CSaveAsMultiDlg ダイアログ

class CSaveAsMultiDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSaveAsMultiDlg)

public:
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
};
