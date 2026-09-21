
// SImageViewerView.h : CSImageViewerView クラスのインターフェイス
//

#pragma once

#include "SImageViewerDoc.h"
#include "SImgProc_ex.h"
#include "ImageProc.h"
#include "ResampleDlg.h"
#include "CommonFunction.h"
class CSImageViewerView : public CView
{
protected: // シリアル化からのみ作成します。
	CSImageViewerView();
	DECLARE_DYNCREATE(CSImageViewerView)

// 属性
public:
	ViewDraw view;

		CString m_sIniFilePath;

	FileFormatList m_fileFomatList;
	void FullDomain(CRect* rect_i);
	CString m_sFilePath;

	CImage m_imageZoomed;
	PanImage * m_image;
	int m_iImageIndex;
	int m_iImageMax;
	bool m_bRefresh;


	bool m_bBeingFullScreen;

	const CRect v_to_i(const CRect* rect_v){return view.v_to_i(rect_v);}
	const CRect i_to_v(const CRect* rect_i){return view.i_to_v(rect_i);}
	

	int GetClientHeight();
	int GetClientWidth();

	void SetScroll();
	bool SaveImage(const CImage* image);
	CSImageViewerDoc* GetDocument() const;
	void SetCaption();
	void ResetImage(bool bZoomReset, bool bProcessReset);
	void AdjustViewClientSize(int desiredClientWidth, int desiredClientHeight);
	bool ReadImage(CString sFilePath);
	bool AddImage(CString sFilePath);
	bool ZoomChange(int iChange);
	bool OnImagePPFW(const int iStep);
	bool ZoomChange(int iMousePosR_v, int iMousePosC_v,int iChange);
	bool ZoomChange(int iR0_i, int iC0_i, int iR1_i, int iC1_i);
	double GetDispOriginR_tv(){return view.GetDispOriginR_tv();}
	double GetDispOriginC_tv(){return view.GetDispOriginC_tv();}


	void EnterFullScreen();
	void ExitFullScreen();
	void SetToolFormat();
	void OperateFileNameChange();
	void OperateCopyHistGramToClipboard();
	void OperateResample();
	void OperateColorize();
	void OperateBrightnessContrastGamma();
	void OperateRotaateImage(enumRotate rotate);
	void OperateEquHistImage();
	void OperateConvertColorSpace();
	void OperateChangeColorDepth();
	void OperateInvert();	
	void SetGridEnableDesable(){view.SetGridEnableDesable();}
	void OperateTransparent();
	bool GetColorAtCursor(const CImage* img, CPoint point_v, int* iR_img, int* iC_img, ColorValue* colorValue);
	bool GetColorAtCursor(PanImage* img, CPoint point_v, int* iR_img, int* iC_img, ColorValue* colorValue);
	void OnScroll(int iSB, int nSBCode, int nPos);
	void DispStatus(CPoint point);
// 操作
public:

// オーバーライド
public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画するためにオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 実装
public:
	virtual ~CSImageViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成された、メッセージ割り当て関数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetSelection();
	afx_msg void OnPasteAs();
	afx_msg void OnCopyAs();
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnFileOpen();
	afx_msg void OnFileAdd();
	afx_msg void OnFileSave();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // SImageViewerView.cpp のデバッグ バージョン
inline CSImageViewerDoc* CSImageViewerView::GetDocument() const
   { return reinterpret_cast<CSImageViewerDoc*>(m_pDocument); }
#endif

