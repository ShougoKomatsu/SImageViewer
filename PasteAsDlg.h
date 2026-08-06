#pragma once


// CPasteAsDlg ダイアログ

enum PASTE_MODE
{
	PASTE_AS_IMAGE=0,
	PASTE_AS_CSV=1,
	PASTE_AS_TSV=2,
};

class CPasteAsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPasteAsDlg)

public:
	CPasteAsDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CPasteAsDlg();

// ダイアログ データ
	enum { IDD = IDD_DLG_PASTE_AS };
	PASTE_MODE m_enumCopyMode;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedPasteAsButtonTsv();
	afx_msg void OnBnClickedPasteAsButtonCsv();
	afx_msg void OnBnClickedPasteAsButtonImage();
};
