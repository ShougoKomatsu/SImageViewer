#pragma once


#include "PictureCtrlEx.h"
#include "ImageProc.h"


// CSetTransparentDlg ダイアログ

class CSetTransparentDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetTransparentDlg)

public:
	CImage m_image;
	CImage m_imageTransparent;

	CImage m_imageColor1;
	CPictureCtrlEx m_pictureBefore;
	CPictureCtrlEx m_pictureAfter;
	CPictureCtrlEx m_pictureColor1;


	CSetTransparentDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CSetTransparentDlg();

// ダイアログ データ
	enum { IDD = IDD_DIALOG_SET_TRANSPARENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
