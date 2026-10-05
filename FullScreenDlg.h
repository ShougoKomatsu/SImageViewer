#pragma once
#include "PictureCtrlEx.h"
#include "SImageViewerView.h"
// CFullScreenDlg ダイアログ

class CFullScreenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFullScreenDlg)

public:
	int m_iWidth;
	int m_iHeight;
	bool m_bCentered;
	void ImageRefresh();
	CSImageViewerView* m_pView;
	CPictureCtrlEx m_picture;
	CFullScreenDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CFullScreenDlg();

// ダイアログ データ
	enum { IDD = IDD_DLG_FULLSCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
