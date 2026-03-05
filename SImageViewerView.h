
// SImageViewerView.h : CSImageViewerView クラスのインターフェイス
//

#pragma once
#define MAX_IMG_BUF (32)


class CSImageViewerView : public CView
{
protected: // シリアル化からのみ作成します。
	CSImageViewerView();
	DECLARE_DYNCREATE(CSImageViewerView)

// 属性
public:
	void SetScroll();
	CSImageViewerDoc* GetDocument() const;
	CRect v_to_i(const CRect* rect_v);
	CRect i_to_v(const CRect* rect_i);
		int m_iImgIndex;
	int m_iUnDoAvailableCount;
	int m_iReDoAvailableCount;
	double m_iDispOriginR_tv;
	double m_iDispOriginC_tv;
	CImage m_imageZoomed;
	CImage m_image;
	CImage m_imageProcessed[32];
	CString m_sFilePath;
	int m_iScaleIndex;
	bool ReadFile(CString sFilePath);
	bool ZoomChange(int iChange);
	bool m_bBingFullScreen;
	RECT m_rectPreserved;
	DWORD m_dwStylePreserved;
	DWORD m_dwExStylePreserved;	
	void EnterFullScreen();
	void ExitFullScreen();
	bool m_bDragging; 
	bool GetColorAtCursor(CPoint point, int* iR_img, int* iC_img, BYTE* byR, BYTE* byG, BYTE* bYB);
	CPoint m_PointStart; 
	CRect m_Rect_v;
	CRect m_Rect_i;
	void OnScroll(int iSB, int nSBCode, double* iPos);
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
	afx_msg void OnFileOpen();
	afx_msg void OnEquHistImage();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnUpdateEditEquHist(CCmdUI *pCmdUI);
//	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // SImageViewerView.cpp のデバッグ バージョン
inline CSImageViewerDoc* CSImageViewerView::GetDocument() const
   { return reinterpret_cast<CSImageViewerDoc*>(m_pDocument); }
#endif

