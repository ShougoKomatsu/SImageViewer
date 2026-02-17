
// SImageViewer.h : SImageViewer アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル


// CSImageViewerApp:
// このクラスの実装については、SImageViewer.cpp を参照してください。
//

class CSImageViewerApp : public CWinAppEx
{
public:
	CSImageViewerApp();


// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSImageViewerApp theApp;
extern CString g_sParam;