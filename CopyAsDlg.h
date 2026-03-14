#pragma once


// CCopyAsDlg ダイアログ

enum COPY_MODE
{
	COPY_AS_IMAGE=0,
	COPY_AS_CSV=1,
	COPY_AS_TSV=2,
};
class CCopyAsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCopyAsDlg)

public:
	CCopyAsDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CCopyAsDlg();
	COPY_MODE m_enumCopyMode;
// ダイアログ データ
	enum { IDD = IDD_DLG_COPY_AS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonCsv();
	afx_msg void OnBnClickedButtonTsv();
	afx_msg void OnBnClickedButtonImage();
};
