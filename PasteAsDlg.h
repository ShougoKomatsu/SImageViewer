#pragma once
#include "ImageProc.h"

// CPasteAsDlg ダイアログ

enum PASTE_FROM_MODE
{
	PASTE_FROM_IMAGE=0,
	PASTE_FROM_CSV=1,
	PASTE_FROM_TSV=2,
};

class CPasteAsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPasteAsDlg)

public:
	CPasteAsDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CPasteAsDlg();

// ダイアログ データ
	enum { IDD = IDD_DLG_PASTE_AS };
	PASTE_FROM_MODE m_enumPasteFrom;
	VALUE_IMAGE m_enumPasteAs;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
