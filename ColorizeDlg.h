#pragma once


// CColorizeDlg ダイアログ

class CColorizeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CColorizeDlg)

public:
	CColorizeDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CColorizeDlg();

// ダイアログ データ
	enum { IDD = IDD_DLG_COLORIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
};
