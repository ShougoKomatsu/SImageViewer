
// MainFrm.h : CMainFrame クラスのインターフェイス
//

#pragma once

class CMainFrame : public CFrameWndEx
{

protected: // シリアル化からのみ作成します。
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
	
	// 属性
public:
	bool m_bGridAble;
	bool m_bFileOpened;
	bool m_bSelected;
	CFont m_cfStatus;
	bool m_bBingFullScreen;
	CRect m_rectPreserved;
	DWORD m_dwStylePreserved;
	DWORD m_dwExStylePreserved ;
	CString m_sStatusSize;
	CString m_sStatusBPP;
	CString m_sStatusZoom;
	CString m_sStatusMousePos;
	CString m_sStatusRGBOriginal;
	CString m_sStatusRGBProcessed;
	CString m_sStatusSelection;
int m_iGrid;
	// 操作
public:
	void ShowNormal();
	void EnterFullScreen();
	void CMainFrame::ExitFullScreen();
	void AdjustViewClientSize(int desiredClientWidth, int desiredClientHeight,int iNowWidth, int iNowHeight);

	void LaunchNewInstance(CString sFilePath);
	// オーバーライド
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	void SetStatusMessage(CString sMes);
	// 実装
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // コントロール バー用メンバー
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;

	// 生成された、メッセージ割り当て関数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnDispStatusSize();
	afx_msg void OnDispStatusBPP();
	afx_msg void OnDispStatusZoom();
	afx_msg void OnDispStatusMousePos();
	afx_msg void OnUpdateMenu(CCmdUI* pCmdUI);
	afx_msg void OnButtonGridNone();
	afx_msg void OnButtonGridDot();
	afx_msg void OnButtonGridLine();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUpdateAfxIdpCommandFailure(CCmdUI *pCmdUI);
	afx_msg void OnZoomdown();
	afx_msg void OnZoomup();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnDestroy();
};

