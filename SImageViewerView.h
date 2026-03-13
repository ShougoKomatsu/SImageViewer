
// SImageViewerView.h : CSImageViewerView クラスのインターフェイス
//

#pragma once
#define MAX_IMG_BUF (32)

#include "SImageViewerDoc.h"

class CSImageViewerView : public CView
{
protected: // シリアル化からのみ作成します。
	CSImageViewerView();
	DECLARE_DYNCREATE(CSImageViewerView)

// 属性
public:
	void FullDomain();
	CString m_sFilePath;
	CImage m_imageZoomed;
	CImage m_image;
	CImage m_imageProcessed[32];
	int m_iImgIndex;
	int m_iUnDoAvailableCount;
	int m_iReDoAvailableCount;

	int m_iScaleIndex;
	int m_iCurSor;
	double m_dDispOriginR_tv;
	double m_dDispOriginC_tv;

	bool m_bBingFullScreen;
	RECT m_rectPreserved;
	DWORD m_dwStylePreserved;
	DWORD m_dwExStylePreserved;	

	bool m_bDragging; 
	CPoint m_PointStart; 
	CRect m_Rect_v;
	CRect m_Rect_i;

	int GetClientHeight();
	int GetClientWidth();

	void SetScroll();
	bool SaveImage(CImage* image);
	CSImageViewerDoc* GetDocument() const;
	CRect v_to_i(const CRect* rect_v);
	CRect i_to_v(const CRect* rect_i);
	void ResetImage();
	bool ReadImage(CString sFilePath);
	bool ZoomChange(int iChange);
	bool ZoomChange(int iMousePosR_v, int iMousePosC_v,int iChange);
	bool ZoomChange(int iR0_i, int iC0_i, int iR1_i, int iC1_i);
	void SetScrollPos(int iR, int iC);
	double GetDispOriginR_tv();
	double GetDispOriginC_tv();

	void EnterFullScreen();
	void ExitFullScreen();

	void OperateBrightnessContrastGamma();
	void OperateEquHistImage();

	bool GetColorAtCursor(CPoint point, int* iR_img, int* iC_img, BYTE* byR, BYTE* byG, BYTE* bYB);
	void OnScroll(int iSB, int nSBCode);
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
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
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

