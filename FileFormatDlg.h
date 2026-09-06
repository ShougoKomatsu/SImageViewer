#pragma once
#include "afxwin.h"
#include "ImageProc.h"

// CFileFormatDlg ダイアログ

class CFileFormatDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileFormatDlg)

public:
	CFileFormatDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CFileFormatDlg();

	FileFormatList m_fileFormatList;

// ダイアログ データ
	enum { IDD = IDD_DLG_FILE_FORMAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ListFileType;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFileFormatListType();
	CString m_sEditWidth;
	CString m_sEditHeight;
	CString m_sEditDataOffset;
	CString m_sEditWidthInfoOffset;
	CString m_sEditHeightInfoOffset;
	CString m_sEditDataInfoOffset;
	CString m_sEditDataOffsetOffset;
};
