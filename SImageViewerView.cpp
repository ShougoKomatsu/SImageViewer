
// SImageViewerView.cpp : CSImageViewerView クラスの実装
//

#include "stdafx.h"
// SHARED_HANDLERS は、プレビュー、サムネイル、および検索フィルター ハンドラーを実装している ATL プロジェクトで定義でき、
// そのプロジェクトとのドキュメント コードの共有を可能にします。
#ifndef SHARED_HANDLERS
#include "SImageViewer.h"
#endif

#include "SImageViewerDoc.h"
#include "SImageViewerView.h"
#include "ImageProc.h"
#include "MainFrm.h"
#include "ImageModifyDlg.h"
#include "SetSelectionDlg.h"
#include "CopyAsDlg.h"
#include "CommonFunction.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_INIT (100)
#define SCALE_VAR_NUM (25)
double g_dScale[SCALE_VAR_NUM]=
{
	0.125000,
	0.162105,
	0.210224,
	0.272627,
	0.353553,
	0.458502,
	0.594604,
	0.771105,
	1.000000,
	1.296840,
	1.681793,
	2.181015,
	2.828427,
	3.668016,
	4.756828,
	6.168843,
	8.000000,
	10.374716,
	13.454343,
	17.448124,
	22.627417,
	29.344129,
	38.054628,
	49.350746,
	64.000000,
};
#define RECT_CHANGE_MARGIN_PIX (10)
enum MOUSE_CURSOR
{
	CHANGE_NONE=0,
	CHANGE_ZOOMUP=1,
	CHANGE_L=2,
	CHANGE_R=3,
	CHANGE_U=4,
	CHANGE_B=5,
	CHANGE_LU=6,
	CHANGE_RU=7,
	CHANGE_LB=8,
	CHANGE_RB=9,

};
// CSImageViewerView

IMPLEMENT_DYNCREATE(CSImageViewerView, CView)

	BEGIN_MESSAGE_MAP(CSImageViewerView, CView)
		ON_WM_CONTEXTMENU()
		ON_WM_RBUTTONUP()
		ON_COMMAND(ID_FILE_OPEN, &CSImageViewerView::OnFileOpen)
		ON_COMMAND(ID_FILE_SAVE_AS, &CSImageViewerView::OnFileSave)
		ON_COMMAND(ID_SET_SELECTION, &CSImageViewerView::OnSetSelection)
		ON_COMMAND(ID_COPY_AS, &CSImageViewerView::OnCopyAs)
		ON_WM_SIZE()
		ON_WM_MOUSEMOVE()
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_WM_TIMER()
		ON_WM_SETCURSOR()
		ON_WM_VSCROLL()
		ON_WM_HSCROLL()
		ON_WM_ERASEBKGND()
	END_MESSAGE_MAP()

	// CSImageViewerView コンストラクション/デストラクション

	CSImageViewerView::CSImageViewerView()
	{
		m_bDragging = false;
		m_Rect_v.SetRectEmpty();
		m_Rect_i.SetRectEmpty();
		m_iImgIndex=0;
		m_iUnDoAvailableCount=0;
		m_iReDoAvailableCount=0;
		m_iScaleIndex=8;
		m_bCBar=false;
		m_bRBar=false;
		m_sFilePath=_T("");
		if(g_sParam.GetLength()>0){m_sFilePath.Format(_T("%s"), g_sParam);}
	}

	CSImageViewerView::~CSImageViewerView()
	{
	}

	BOOL CSImageViewerView::PreCreateWindow(CREATESTRUCT& cs)
	{
		// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
		//  修正してください。

		return CView::PreCreateWindow(cs);
	}

	void CSImageViewerView::OnCopyAs()
	{
		CCopyAsDlg copyAsdlg;

		INT_PTR iRet = copyAsdlg.DoModal();
		if(iRet != IDOK){return;}

		CImage imgClipped;
		ClipImage(&m_imageProcessed[m_iImgIndex],&imgClipped, m_Rect_i.top,m_Rect_i.left, m_Rect_i.bottom, m_Rect_i.right); 
		switch(copyAsdlg.m_enumCopyMode)
		{
		case COPY_AS_IMAGE:
			{
				CopyToClipBoardImg(&imgClipped);
				break;
			}
		case COPY_AS_CSV:
			{
				CString sImage;
				bool bRet = ConvertImageToStr(&imgClipped,_T(","), &sImage);
				CopyToClipBoardStr(sImage);
				break;
			}
		case COPY_AS_TSV:
			{
				CString sImage;
				bool bRet = ConvertImageToStr(&imgClipped,_T("	"), &sImage);
				CopyToClipBoardStr(sImage);
				break;
			}
		}
	}


	void CSImageViewerView::OnSetSelection()
	{
		CSetSelectionDlg setdlg;
		if(m_Rect_i.IsRectEmpty() != TRUE)
		{
			setdlg.m_iC0=m_Rect_i.left;
			setdlg.m_iR0=m_Rect_i.top;
			setdlg.m_iC1=m_Rect_i.right;
			setdlg.m_iR1=m_Rect_i.bottom;
		}

		INT_PTR iRet = setdlg.DoModal();
		if(iRet == IDOK)
		{
			m_Rect_i.SetRect(setdlg.m_iC0,setdlg.m_iR0,setdlg.m_iC1,setdlg.m_iR1);
			Invalidate();
		}
	}

	// CSImageViewerView 描画
	double CSImageViewerView::GetDispOriginR_tv()
	{
		return m_dDispOriginR_tv;
	}
	double CSImageViewerView::GetDispOriginC_tv()
	{
		return m_dDispOriginC_tv;
	}
	void CSImageViewerView::OnDraw(CDC* pDC)
	{
		CSImageViewerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc){return;}

		if (m_imageProcessed[m_iImgIndex].IsNull()){return;}

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);

		CImage imgZoomed;
		int iHeight_v=GetClientHeight();
		int iWidth_v=GetClientWidth();

		CBitmap bufferBmp; 
		bufferBmp.CreateCompatibleBitmap(pDC, iWidth_v, iHeight_v);
		CBitmap* pOldBmp = memDC.SelectObject(&bufferBmp);

		double dDispOriginR_tv = GetDispOriginR_tv();
		double dDispOriginC_tv = GetDispOriginC_tv();

		double dR0_i = (dDispOriginR_tv/g_dScale[m_iScaleIndex]);
		double dC0_i = (dDispOriginC_tv/g_dScale[m_iScaleIndex]);

		ZoomImage(&(m_imageProcessed[m_iImgIndex]),&imgZoomed,
			dR0_i,
			dC0_i,
			g_dScale[m_iScaleIndex],
			iWidth_v,iHeight_v);


		imgZoomed.BitBlt( memDC.GetSafeHdc(), 0, 0,imgZoomed.GetWidth(), imgZoomed.GetHeight(), 0, 0  );

		pDC->BitBlt(0, 0, iWidth_v, iHeight_v, &memDC, 0, 0,SRCCOPY);

		memDC.SelectObject(pOldBmp);


		if (m_Rect_i.IsRectEmpty()==FALSE)
		{
			CRect rect_v = i_to_v(&m_Rect_i);

			CPen pen(PS_SOLID,1, RGB(127,127,127));
			CPen* pOldPen=pDC->SelectObject(&pen);
			CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

			pDC->Rectangle(rect_v);

			pDC->SelectObject(pOldPen);
			pDC->SelectObject(pOldBrush);
			ReleaseDC(pDC); 
		} 

	}

	void CSImageViewerView::OnRButtonUp(UINT /* nFlags */, CPoint point_v)
	{
		ClientToScreen(&point_v);
		OnContextMenu(this, point_v);
	}

	void CSImageViewerView::OnContextMenu(CWnd* /* pWnd */, CPoint point_v)
	{
#ifndef SHARED_HANDLERS
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point_v.x, point_v.y, this, TRUE);
#endif
	}


	// CSImageViewerView 診断

#ifdef _DEBUG
	void CSImageViewerView::AssertValid() const
	{
		CView::AssertValid();
	}

	void CSImageViewerView::Dump(CDumpContext& dc) const
	{
		CView::Dump(dc);
	}

	CSImageViewerDoc* CSImageViewerView::GetDocument() const // デバッグ以外のバージョンはインラインです。
	{
		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSImageViewerDoc)));
		return (CSImageViewerDoc*)m_pDocument;
	}
#endif //_DEBUG

	void CheckIfScrollBarsAreNeeded(const int iWidth_tv, const int iHeight_tv, const int iWidthIfNoBar_v, const int iHeightIfNoBar_v, const int iBarWidth, const int iBarHeight, bool* bRBar, bool* bCBar)
	{

		if((iWidth_tv<=iWidthIfNoBar_v)&&(iHeight_tv<=iHeightIfNoBar_v))
		{
			*bRBar=false;
			*bCBar=false;
			return;
		}

		if(iWidth_tv>iWidthIfNoBar_v)
		{
			*bCBar=true;

			if(iHeight_tv<=iHeightIfNoBar_v-iBarHeight)
			{
				*bRBar=false;
				return;
			}
			*bRBar=true;
			return;
		}

		*bRBar=true;
		if(iWidth_tv<=iWidthIfNoBar_v-iBarWidth)
		{
			*bCBar=false;
			return;
		}
		*bCBar=true;
		return;

	}
	void CSImageViewerView::SetScroll()
	{

		int iHeight_v=GetClientHeight();
		int iWidth_v=GetClientWidth();

		int iBarWidth= ::GetSystemMetrics(SM_CYHSCROLL);
		int iBarHeight= ::GetSystemMetrics(SM_CXVSCROLL);

		int iWidth_i =max(0,m_imageProcessed[m_iImgIndex].GetWidth());
		int iHeight_i =max(0,m_imageProcessed[m_iImgIndex].GetHeight());

		int iWidth_tv = iWidth_i*g_dScale[m_iScaleIndex];
		int iHeight_tv= iHeight_i*g_dScale[m_iScaleIndex];

		SCROLLINFO si = { 0 };
		int iPageV, iPageH;
		GetScrollInfo(SB_VERT, &si);
		if(si.nPage==0){m_bRBar=false;}
		iPageV = si.nPage;
		
		GetScrollInfo(SB_HORZ, &si);
		if(si.nPage==0){m_bCBar=false;}
		iPageH = si.nPage;

		int iHeightIfNoBar_v=iHeight_v+(m_bCBar ? iBarHeight : 0);
		int iWidthIfNoBar_v=iWidth_v+(m_bRBar ? iBarWidth : 0);

		bool bRBar=false;
		bool bCBar=false;


		CheckIfScrollBarsAreNeeded(iWidth_tv, iHeight_tv, iWidthIfNoBar_v, iHeightIfNoBar_v, iBarWidth, iBarHeight, &bRBar, &bCBar);

		if(bRBar==true)
		{
			GetScrollInfo(SB_VERT, &si);
			int iPageR=(iHeightIfNoBar_v-(bCBar ? iBarHeight:0))/10;
			si.nMin=0;
			si.nMax=max(0,(iHeight_tv-(iHeightIfNoBar_v-(bCBar ? iBarHeight:0)))+1);
			si.nPage =min(si.nMax,iPageR);
			if(si.nPage>0){m_bRBar = true;}else{m_bRBar = false;}
			SetScrollInfo(SB_VERT, &si, TRUE);
		}
		else
		{
			si.nMin=0;
			si.nMax=0;
			si.nPage =0;
			m_bRBar = false;
			SetScrollInfo(SB_VERT, &si, TRUE);
		}

		if(bCBar==true)
		{
			int iPageC=(iWidthIfNoBar_v-(bRBar ? iBarWidth :0))/10;
			GetScrollInfo(SB_HORZ, &si);
			si.nMin=0;
			si.nMax=max(0,(iWidth_tv-(iWidthIfNoBar_v-(bRBar ? iBarWidth :0)))+1);
			si.nPage =min(si.nMax, iPageC);
			if(si.nPage>0){m_bCBar = true;}else{m_bCBar = false;}
			SetScrollInfo(SB_HORZ, &si, TRUE);
		}
		else
		{
			si.nMin=0;
			si.nMax=0;
			si.nPage =0;
			m_bCBar = false;
			SetScrollInfo(SB_HORZ, &si, TRUE);
		}

	}


	void CSImageViewerView::ResetImage()
	{
		m_imageProcessed[m_iImgIndex]=m_image;
		m_iScaleIndex =8;
		/*
		CRect rectClientTemp;
		CRect rectTemp;
		GetWindowRect(&rectTemp);
		GetClientRect(&rectClientTemp);

		int iX0=rectTemp.left;
		int iY0=rectTemp.top;
		int iWidth=rectTemp.Width();
		int iHeight=rectTemp.Height();
		int iCaptionHeight = ::GetSystemMetrics(SM_CYCAPTION);

		//rectTemp.left, rectTemp.top, rectTemp.Width()-rectClientTemp.Width()-m_image.GetWidth(),rectTemp.Height()-rectClientTemp.Height()-m_image.GetHeight()

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		CRect rect;
		pFrame->GetClientRect(&rect);

		pFrame->MoveWindow(iX0, iY0, rect.Width(), rect.Height()+iCaptionHeight);
		*/
		CRect rectClient;
		GetClientRect(&rectClient);
		
		int iHeight_v=GetClientHeight();
		int iWidth_v=GetClientWidth();

		int iBarWidth= ::GetSystemMetrics(SM_CYHSCROLL);
		int iBarHeight= ::GetSystemMetrics(SM_CXVSCROLL);

		int iWidth_i =max(0,m_imageProcessed[m_iImgIndex].GetWidth());
		int iHeight_i =max(0,m_imageProcessed[m_iImgIndex].GetHeight());

		int iWidth_tv = iWidth_i*g_dScale[m_iScaleIndex];
		int iHeight_tv= iHeight_i*g_dScale[m_iScaleIndex];

		SCROLLINFO si = { 0 };
		
		GetScrollInfo(SB_VERT, &si);
		if(si.nPage==0){m_bRBar=false;}
		
		GetScrollInfo(SB_HORZ, &si);
		if(si.nPage==0){m_bCBar=false;}

		int iHeightIfNoBar_v=iHeight_v+(m_bCBar ? iBarHeight : 0);
		int iWidthIfNoBar_v=iWidth_v+(m_bRBar ? iBarWidth : 0);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->AdjustViewClientSize(m_image.GetWidth(), m_image.GetHeight(),iWidthIfNoBar_v, iHeightIfNoBar_v);
		SetScroll();

		m_iImgIndex=0;
		m_iUnDoAvailableCount=0;
		m_iReDoAvailableCount=0;
		m_dDispOriginC_tv=0;
		m_dDispOriginR_tv=0;

		GetScrollInfo(SB_HORZ, &si);
		if(si.nPage>0)
		{
		si.nPos = 0; 
		SetScrollInfo(SB_HORZ, &si, TRUE);
		}

		GetScrollInfo(SB_VERT, &si);
		if(si.nPage>0)
		{
			si.nPos = 0; 
		SetScrollInfo(SB_VERT, &si, TRUE);
		}
		m_iMouseMode=0;
		m_Rect_i.SetRectEmpty();
		Invalidate();

		CString sImageSize;
		sImageSize.Format(_T("%d x %d"), m_image.GetWidth(),m_image.GetHeight());

		//	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame != nullptr)	{pFrame->SetStatusMessage(sImageSize);}

	}

	bool CSImageViewerView::ReadImage(CString sFilePath)
	{
		if(m_imageProcessed[m_iImgIndex].IsNull()!=true){m_imageProcessed[m_iImgIndex].Destroy();}

		CFileFind cf;
		BOOL bRet = cf.FindFile(sFilePath);
		if(bRet != TRUE){return false;}
		if(m_image.IsNull()!=true){m_image.Destroy();}

		HRESULT hResult = m_image.Load(m_sFilePath);
		if(hResult != S_OK){return false;}

		ResetImage();
		return true;
	}

	bool CSImageViewerView::SaveImage(CImage* image)
	{
		CFileDialog cf(FALSE);
		//		cf.m_ofn.lpstrInitialDir = sMacroFolderPath;
		if(cf.DoModal()!=IDOK){ return false;}
		CString sFilePath;
		sFilePath.Format(_T("%s"),cf.GetPathName());

		CFileFind cff;
		BOOL bRet = cff.FindFile(sFilePath);
		if(bRet = FALSE)
		{
			INT_PTR iRet = AfxMessageBox(_T("ファイルは既に存在します。上書きしますか？"),0,MB_YESNO);
			if(iRet != IDYES){return false;}
		}

		HRESULT hResult = image->Save(sFilePath);
		if(hResult != S_OK){return false;}

		return true;
	}

	void CSImageViewerView::OnFileSave()
	{
		SaveImage(&m_image);
	}
	void CSImageViewerView::OnFileOpen()
	{
		CFileDialog cf(TRUE);
		//		cf.m_ofn.lpstrInitialDir = sMacroFolderPath;
		if(cf.DoModal()!=IDOK){ return;}
		m_sFilePath.Format(_T("%s"),cf.GetPathName());
		ReadImage(m_sFilePath);

	}
	void CSImageViewerView::FullDomain()
	{
		m_Rect_i.SetRect(0, 0, m_imageProcessed[m_iImgIndex].GetWidth()-1,m_imageProcessed[m_iImgIndex].GetHeight()-1);
	}
	void CSImageViewerView::OperateEquHistImage()
	{
		bool bAutoFull = false;
		if(m_Rect_i.IsRectEmpty()==TRUE){bAutoFull=true; FullDomain();}

		ImgRGB imgRGB;
		ImgRGB imgMeaned;
		ConvertImage(&m_imageProcessed[m_iImgIndex], &imgRGB);
		EquHistImage(&imgRGB,&imgMeaned,m_Rect_i.top,m_Rect_i.left,m_Rect_i.bottom,m_Rect_i.right);
		if(bAutoFull==true){m_Rect_i.SetRectEmpty();}

		m_iImgIndex++;
		m_iUnDoAvailableCount++;
		if(m_iUnDoAvailableCount>=MAX_IMG_BUF-1){m_iUnDoAvailableCount=MAX_IMG_BUF-1;}
		ConvertImage(&imgMeaned,&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)]);
		Invalidate();
	}
	
	void CSImageViewerView::OperateRotaateImage(enumRotate rotate)
	{
		bool bAutoFull = false;
	
		ImgRGB imgRGB;
		ImgRGB imgResult;
		ConvertImage(&m_imageProcessed[m_iImgIndex], &imgRGB);

		RotateImage(&imgRGB, &imgResult, rotate);
		m_iImgIndex++;
		m_iUnDoAvailableCount++;
		if(m_iUnDoAvailableCount>=MAX_IMG_BUF-1){m_iUnDoAvailableCount=MAX_IMG_BUF-1;}
		ConvertImage(&imgResult,&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)]);
		SetScroll();
		Invalidate();
	}

	void CSImageViewerView::OperateBrightnessContrastGamma()
	{
		bool bAutoFull = false;
		if(m_Rect_i.IsRectEmpty()==TRUE){bAutoFull=true;FullDomain();}

		CImageModifyDlg dlgModify;
		INT_PTR iRet;
		dlgModify.DoModal();
		iRet = dlgModify.m_iRet;
		if(iRet == IDCANCEL){if(bAutoFull==true){m_Rect_i.SetRectEmpty();} return ;}

		int iBrightness=dlgModify.m_iBrightness;
		int iContrast=dlgModify.m_iContrast;
		double dGamma=dlgModify.m_dGamma;

		ImgRGB imgRGB;
		ImgRGB imgResult1;
		ImgRGB imgResult2;
		ConvertImage(&m_imageProcessed[m_iImgIndex], &imgRGB);
		BrightnessContrast(&imgRGB,&imgResult1,m_Rect_i.top,m_Rect_i.left,m_Rect_i.bottom,m_Rect_i.right,(double)iBrightness,(double)iContrast);
		Gamma(&imgResult1,&imgResult2,m_Rect_i.top,m_Rect_i.left,m_Rect_i.bottom,m_Rect_i.right,dGamma);
		if(bAutoFull==true){m_Rect_i.SetRectEmpty();}

		m_iImgIndex++;
		m_iUnDoAvailableCount++;
		if(m_iUnDoAvailableCount>=MAX_IMG_BUF-1){m_iUnDoAvailableCount=MAX_IMG_BUF-1;}
		ConvertImage(&imgResult2,&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)]);
		Invalidate();
	}



	void CSImageViewerView::OnSize(UINT nType, int cx, int cy)
	{
		CView::OnSize(nType, cx, cy);

		if(m_imageProcessed[m_iImgIndex].IsNull()==true){return;}

		SetScroll();
		Invalidate();
	}


	void CSImageViewerView::OnInitialUpdate()
	{
		CView::OnInitialUpdate();
		
		m_imageProcessed[m_iImgIndex].Create(100,100,0);

		m_bBingFullScreen = false;
		SetTimer(TIMER_INIT, 100, 0);
	}

	int CSImageViewerView::GetClientHeight()
	{
		CRect rectClient;
		GetClientRect(&rectClient);
		return rectClient.Height();
	}

	int CSImageViewerView::GetClientWidth()
	{
		CRect rectClient;
		GetClientRect(&rectClient);
		return rectClient.Width();
	}

	bool CSImageViewerView::ZoomChange(int iChange)
	{
		if((m_iScaleIndex>=SCALE_VAR_NUM-1)&&(iChange>0)){return false;}
		if((m_iScaleIndex<=0)&&(iChange<0)){return false;}

		int iHeight_v=GetClientHeight();
		int iWidth_v=GetClientWidth();
		double dOldDispOriginR_tv = GetDispOriginR_tv();
		double dOldDispOriginC_tv = GetDispOriginC_tv();

		int iWidth_i =max(0,m_imageProcessed[m_iImgIndex].GetWidth());
		int iHeight_i =max(0,m_imageProcessed[m_iImgIndex].GetHeight());
		
		int iOldZoom = m_iScaleIndex;
		double dOldWidth_tv = iWidth_i*g_dScale[iOldZoom];
		double dOldHeight_tv= iHeight_i*g_dScale[iOldZoom];
		double dNewDispOriginC_tv;
		double dNewDispOriginR_tv;
		int iNewZoom = m_iScaleIndex+=iChange;
		double dNewWidth_tv = iWidth_i*g_dScale[iNewZoom];
		double dNewHeight_tv= iHeight_i*g_dScale[iNewZoom];


		if(dNewWidth_tv>iWidth_v)
		{
			double dOldCenterC_i = (dOldDispOriginC_tv +iWidth_v/2.0)/g_dScale[iOldZoom];
			double dOldEndC_i = (dOldDispOriginC_tv +iWidth_v)/g_dScale[iOldZoom];
			dNewDispOriginC_tv = max(0, min(dOldCenterC_i*g_dScale[iNewZoom] - iWidth_v/2.0, dOldEndC_i*g_dScale[iNewZoom]-iWidth_v));
		}
		else
		{
			dNewDispOriginC_tv = 0;
		}

		if(dNewHeight_tv>iHeight_v)
		{
			double dOldCenterR_i = (dOldDispOriginR_tv +iHeight_v/2.0)/g_dScale[iOldZoom];
			double dOldEndR_i = (dOldDispOriginR_tv +iHeight_v)/g_dScale[iOldZoom];
			dNewDispOriginR_tv = max(0, min(dOldCenterR_i*g_dScale[iNewZoom] - iHeight_v/2.0, dOldEndR_i*g_dScale[iNewZoom]-iHeight_v));
		}
		else
		{
			dNewDispOriginR_tv = 0;
		}
		m_iScaleIndex=iNewZoom;


		SetScrollPos(dNewDispOriginR_tv, dNewDispOriginC_tv);

		if(m_iScaleIndex>=SCALE_VAR_NUM-1){m_iScaleIndex=SCALE_VAR_NUM-1;}
		if(m_iScaleIndex<=0){m_iScaleIndex=0;}
		SetScroll();
		Invalidate();

		CPoint point_v;
		GetCursorPos(&point_v);
		ScreenToClient(&point_v);
		DispStatus(point_v);
		return true; 
	}

	bool CSImageViewerView::ZoomChange(int iMousePosR_v, int iMousePosC_v, int iChange)
	{
		if((m_iScaleIndex>=SCALE_VAR_NUM-1)&&(iChange>0)){return false;}
		if((m_iScaleIndex<=0)&&(iChange<0)){return false;}

		double dMousePosR_tv=iMousePosR_v + GetDispOriginR_tv();
		double dMousePosC_tv=iMousePosC_v + GetDispOriginC_tv();

		double dMousePosR_i=dMousePosR_tv/g_dScale[m_iScaleIndex];
		double dMousePosC_i=dMousePosC_tv/g_dScale[m_iScaleIndex];


		int iHeight_v=GetClientHeight();
		int iWidth_v=GetClientWidth();
		double dOldDispOriginR_tv = GetDispOriginR_tv();
		double dOldDispOriginC_tv = GetDispOriginC_tv();

		int iWidth_i =max(0,m_imageProcessed[m_iImgIndex].GetWidth());
		int iHeight_i =max(0,m_imageProcessed[m_iImgIndex].GetHeight());

		double dWidth_tv = iWidth_i*g_dScale[m_iScaleIndex];
		double dHeight_tv= iHeight_i*g_dScale[m_iScaleIndex];
		double dNewDispOriginC_tv;
		double dNewDispOriginR_tv;
		m_iScaleIndex+=iChange;		
		SetScroll();
		if(dWidth_tv>iWidth_v)
		{
			double dNewMousePosC_tv = dMousePosC_i*g_dScale[m_iScaleIndex];
			dNewDispOriginC_tv = max(0, (dNewMousePosC_tv-iMousePosC_v));
		}
		else
		{
			dNewDispOriginC_tv =0;
		}

		if(dHeight_tv>iHeight_v)
		{
			double dNewMousePosR_tv = dMousePosR_i*g_dScale[m_iScaleIndex];
			dNewDispOriginR_tv = max(0, dNewMousePosR_tv-iMousePosR_v);
		}
		else
		{
			dNewDispOriginR_tv=0;
		}

		SetScrollPos(dNewDispOriginR_tv, dNewDispOriginC_tv);

		Invalidate();

		CPoint point_v;
		GetCursorPos(&point_v);
		ScreenToClient(&point_v);
		DispStatus(point_v);
		return true; 
	}


	void CSImageViewerView::SetScrollPos(int iR_tv, int iC_tv)
	{		
		SCROLLINFO si;

		GetScrollInfo(SB_VERT, &si);
		if(si.nPage>0)
		{
			int iNewPos_scl=iR_tv*(si.nMax-si.nPage+1.0)/(si.nMax *1.0);
		m_dDispOriginR_tv = iR_tv;
		si.nPos = (int)(max(si.nMin,min(si.nMax-si.nPage+1.0,iNewPos_scl) ));;
		SetScrollInfo(SB_VERT, &si, TRUE);
		}

		GetScrollInfo(SB_HORZ, &si);
		if(si.nPage>0)
		{
			int iNewPos_scl=iC_tv*(si.nMax-si.nPage+1.0)/(si.nMax *1.0);
		m_dDispOriginC_tv = iC_tv;
		si.nPos = (int)(max(si.nMin,min(si.nMax-si.nPage+1.0, iNewPos_scl) )); 
		SetScrollInfo(SB_HORZ, &si, TRUE);
		}
	}
	bool CSImageViewerView::ZoomChange(int iR0_i, int iC0_i, int iR1_i, int iC1_i)
	{
		int iHeight_v=GetClientHeight();
		int iWidth_v=GetClientWidth();

		int iNewScaleIndex = m_iScaleIndex;
		for(int i=SCALE_VAR_NUM-1; i>=0; i--)
		{
			if((iHeight_v>(iR1_i-iR0_i+1)*g_dScale[i]) && (iWidth_v>(iC1_i-iC0_i+1)*g_dScale[i]))
			{
				iNewScaleIndex=i;
				break;
			}
		}


		m_iScaleIndex = iNewScaleIndex;
		if(m_iScaleIndex>=SCALE_VAR_NUM-1){m_iScaleIndex=SCALE_VAR_NUM-1;}
		if(m_iScaleIndex<=0){m_iScaleIndex=0;}

		SetScroll();

		double dNewCenterR_i = (iR0_i + iR1_i)/2.0;
		double dNewCenterC_i = (iC0_i + iC1_i)/2.0;

		double dNewCenterR_tv = dNewCenterR_i*g_dScale[m_iScaleIndex];
		double dNewCenterC_tv = dNewCenterC_i*g_dScale[m_iScaleIndex];


		int iNewScrollR_tv = max(0,int(dNewCenterR_tv-iHeight_v/2.0));
		int iNewScrollC_tv = max(0,int(dNewCenterC_tv-iWidth_v/2.0));

		SetScrollPos(iNewScrollR_tv, iNewScrollC_tv);


		Invalidate();

		CPoint point_v;
		GetCursorPos(&point_v);
		ScreenToClient(&point_v);
		DispStatus(point_v);
		return true; 
	}

	void CSImageViewerView::EnterFullScreen()
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame==NULL) {return;}
		pFrame->EnterFullScreen();
		m_bBingFullScreen=true;

	}

	void CSImageViewerView::ExitFullScreen()
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame==NULL) {return;}
		pFrame->ExitFullScreen();
		m_bBingFullScreen=false;
	}


	bool CSImageViewerView::GetColorAtCursor(CPoint point_v, int* iR_img, int* iC_img, BYTE* byR, BYTE* byG, BYTE* byB)
	{
		if(m_image.IsNull()==true){return false;}
		CPoint point_tv(point_v.x + GetDispOriginC_tv(), point_v.y +  GetDispOriginR_tv());

		int iC_img_Local = (int)((point_tv.x) / g_dScale[m_iScaleIndex]);
		int iR_img_Local = (int)((point_tv.y) / g_dScale[m_iScaleIndex]);


		if (iC_img_Local < 0){return false;}
		if (iR_img_Local < 0){return false;}
		if (iC_img_Local >= m_image.GetWidth()){return false;}
		if (iR_img_Local >= m_image.GetHeight()){return false;}

		COLORREF col = m_image.GetPixel(iC_img_Local,iR_img_Local);

		*iR_img = iR_img_Local;
		*iC_img = iC_img_Local;
		*byR = GetRValue(col);
		*byG = GetGValue(col);
		*byB = GetBValue(col);

		return true;
	}

	void CSImageViewerView::DispStatus(CPoint point_v)
	{
		CString sFileName = m_sFilePath.Mid(m_sFilePath.ReverseFind('\\') + 1);

		int iR_img,iC_img;
		BYTE byR,byG,byB;
		CString sCaption;
		bool bRet = GetColorAtCursor(point_v, &iR_img, &iC_img, &byR, &byG, &byB);

		CString sRect=_T("");
		if(m_Rect_i.IsRectEmpty()==false)
		{
			sRect.Format(_T("| (%d, %d) - (%d, %d) : %d x %d "), m_Rect_i.left,m_Rect_i.top,m_Rect_i.right,m_Rect_i.bottom,m_Rect_i.right-m_Rect_i.left+1,m_Rect_i.bottom-m_Rect_i.top+1	);	
		}


		if(bRet == true)
		{
			sCaption.Format(_T("%s | (%d, %d) (R, G, B)= (%d, %d, %d) %s | %.3f%%"), sFileName, iC_img, iR_img, byR, byG, byB,sRect,100*g_dScale[m_iScaleIndex]);
		}
		else
		{
			sCaption.Format(sFileName);
		}
		AfxGetMainWnd()->SetWindowText(sCaption);
	}
	bool isNearTheBoarder(double d, double dBoarder, double dMargin)
	{
		if(d<dBoarder-dMargin){return false;}
		if(d>dBoarder+dMargin){return false;}
		return true;
	}
	void CSImageViewerView::OnMouseMove(UINT nFlags, CPoint point_v)
	{

		if(m_imageProcessed[m_iImgIndex].IsNull()==true){return;}

		DispStatus(point_v);

		if (m_bDragging==true) 
		{ 
			CDC* pDC = GetDC(); 
			if (m_Rect_v.IsRectEmpty() == FALSE)
			{
				pDC->DrawFocusRect(&m_Rect_v);
			}

			switch(m_iMouseMode)
			{
			case CHANGE_U: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(rectTemp_v.left,point_v.y), CPoint(rectTemp_v.right,rectTemp_v.bottom)); break;}
			case CHANGE_B: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(rectTemp_v.left,rectTemp_v.top), CPoint(rectTemp_v.right,point_v.y)); break;}
			case CHANGE_L: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(point_v.x,rectTemp_v.top), CPoint(rectTemp_v.right,rectTemp_v.bottom)); break;}
			case CHANGE_R: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(rectTemp_v.left, rectTemp_v.top), CPoint(point_v.x,rectTemp_v.bottom)); break;}
			case CHANGE_LU: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(point_v.x, point_v.y), CPoint(rectTemp_v.right,rectTemp_v.bottom)); break;}
			case CHANGE_RU: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(rectTemp_v.left, point_v.y), CPoint(point_v.x,rectTemp_v.bottom)); break;}
			case CHANGE_LB: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(point_v.x, rectTemp_v.top), CPoint(rectTemp_v.right,point_v.y)); break;}
			case CHANGE_RB: {CRect rectTemp_v = i_to_v(&m_Rect_i); m_Rect_v = CRect(CPoint(rectTemp_v.left, rectTemp_v.top), CPoint(point_v.x,point_v.y)); break;}
			default :
				{
					m_Rect_v = CRect(m_PointStart_v, point_v);
				}
			}

			m_Rect_v.NormalizeRect();
			pDC->DrawFocusRect(&m_Rect_v); 
			ReleaseDC(pDC);
			CView::OnMouseMove(nFlags, point_v);
			return;
		} 


		CRect rectTemp_v;
		rectTemp_v = i_to_v(&m_Rect_i);

		int iBoarder=0;
		if(isNearTheBoarder(point_v.y, rectTemp_v.top,RECT_CHANGE_MARGIN_PIX) == true){iBoarder += 1;}
		if(isNearTheBoarder(point_v.x, rectTemp_v.left,RECT_CHANGE_MARGIN_PIX) == true){iBoarder += 2;}
		if(isNearTheBoarder(point_v.x, rectTemp_v.right,RECT_CHANGE_MARGIN_PIX) == true){iBoarder += 4;}
		if(isNearTheBoarder(point_v.y, rectTemp_v.bottom,RECT_CHANGE_MARGIN_PIX) == true){iBoarder += 8;}

		switch(iBoarder)
		{
		case 1:{m_iMouseMode=CHANGE_U; CView::OnMouseMove(nFlags, point_v); return;}
		case 2:{m_iMouseMode=CHANGE_L; CView::OnMouseMove(nFlags, point_v); return;}
		case 4:{m_iMouseMode=CHANGE_R; CView::OnMouseMove(nFlags, point_v); return;}
		case 8:{m_iMouseMode=CHANGE_B; CView::OnMouseMove(nFlags, point_v); return;}
		case 3:{m_iMouseMode=CHANGE_LU; CView::OnMouseMove(nFlags, point_v); return;}
		case 5:{m_iMouseMode=CHANGE_RU; CView::OnMouseMove(nFlags, point_v); return;}
		case 10:{m_iMouseMode=CHANGE_LB; CView::OnMouseMove(nFlags, point_v); return;}
		case 12:{m_iMouseMode=CHANGE_RB; CView::OnMouseMove(nFlags, point_v); return;}
		default:{break;}
		}
		if((point_v.y>=rectTemp_v.top)&&(point_v.y<=rectTemp_v.bottom)&&(point_v.x>=rectTemp_v.left)&&(point_v.x<=rectTemp_v.right))
		{
			m_iMouseMode=CHANGE_ZOOMUP;
		}
		else
		{
			m_iMouseMode=CHANGE_NONE;
		}

		CView::OnMouseMove(nFlags, point_v);
	}


	void CSImageViewerView::OnLButtonDown(UINT nFlags, CPoint point_v)
	{
		SetCapture(); 
		m_bDragging = true;
		m_PointStart_v.SetPoint(point_v.x,point_v.y); 

		CView::OnLButtonDown(nFlags, point_v);
	}


	void CSImageViewerView::OnLButtonUp(UINT nFlags, CPoint point_v)
	{
		if (m_bDragging==TRUE) 
		{
			ReleaseCapture(); 
			m_bDragging = false; 
			if (m_Rect_v.IsRectEmpty()==FALSE)
			{
				CDC* pDC = GetDC();
				pDC->DrawFocusRect(&m_Rect_v); 
				ReleaseDC(pDC); 
			} 
			if(m_PointStart_v==point_v)
			{
				CRect rect_v;
				rect_v = i_to_v(&m_Rect_i);
				if((point_v.y>=rect_v.top)&&(point_v.y<=rect_v.bottom)&&(point_v.x>=rect_v.left)&&(point_v.x<=rect_v.right))
				{
					ZoomChange(m_Rect_i.top, m_Rect_i.left, m_Rect_i.bottom,m_Rect_i.right);
					m_Rect_v.SetRectEmpty();
					m_Rect_i.SetRectEmpty();
					return;
				}
				m_Rect_v.SetRectEmpty();
			}
			m_Rect_i = v_to_i(&m_Rect_v);
			m_Rect_i.left=max(0,m_Rect_i.left);
			m_Rect_i.top=max(0,m_Rect_i.top);
			m_Rect_v.SetRectEmpty();
			Invalidate();
		}

		CView::OnLButtonUp(nFlags, point_v);
	}


	void CSImageViewerView::OnTimer(UINT_PTR nIDEvent)
	{
		if(nIDEvent==TIMER_INIT)
		{
			KillTimer(TIMER_INIT);
			
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame==NULL) {return;}
		pFrame->ShowNormal();
			if(m_sFilePath.GetLength()>0)
			{
				ReadImage(m_sFilePath);
			}
			//	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
			return;
		}

		CView::OnTimer(nIDEvent);
	}


	BOOL CSImageViewerView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
	{
		if (nHitTest == HTCLIENT) 
		{
			switch(m_iMouseMode)
			{
			case CHANGE_NONE:{SetCursor(AfxGetApp()->LoadCursorW(IDC_CURSOR_CROSS));return TRUE;}
			case CHANGE_L:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));return TRUE;}
			case CHANGE_R:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));return TRUE;}
			case CHANGE_U:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));return TRUE;}
			case CHANGE_B:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));return TRUE;}
			case CHANGE_LU:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE));return TRUE;}
			case CHANGE_RB:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE));return TRUE;}
			case CHANGE_RU:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENESW));return TRUE;}
			case CHANGE_LB:{SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENESW));return TRUE;}
			case CHANGE_ZOOMUP:{SetCursor(AfxGetApp()->LoadCursorW(IDC_CURSOR_ZOOMIN));return TRUE;}
			}
		}

		return CView::OnSetCursor(pWnd, nHitTest, message);
	}

	CRect CSImageViewerView::v_to_i(const CRect* rect_v)
	{
		CRect rect_i;
		rect_i.SetRectEmpty();
		if(rect_v->IsRectEmpty()==TRUE)
		{
			return rect_i;
		}

		int iCOrigin_tv = GetDispOriginC_tv();
		int iROrigin_tv = GetDispOriginR_tv();

		rect_i.SetRect(
			(int)(((rect_v->left+ iCOrigin_tv) / g_dScale[m_iScaleIndex])   +0.5)
			,(int)(((rect_v->top+ iROrigin_tv) / g_dScale[m_iScaleIndex])    +0.5)
			,(int)(((rect_v->right+ iCOrigin_tv) / g_dScale[m_iScaleIndex])  -0.5)
			,(int)(((rect_v->bottom+ iROrigin_tv) / g_dScale[m_iScaleIndex]) -0.5));

		return rect_i;
	}

	CRect CSImageViewerView::i_to_v(const CRect* rect_i)
	{
		CRect rect_v;
		if(rect_i->IsRectEmpty()==TRUE)
		{
			rect_v.SetRectEmpty();
			return rect_v;
		}

		int iCOrigin_tv = GetDispOriginC_tv();
		int iROrigin_tv = GetDispOriginR_tv();

		rect_v.SetRect(
			(int)((rect_i->left   ) * g_dScale[m_iScaleIndex])-iCOrigin_tv
			,(int)((rect_i->top    ) * g_dScale[m_iScaleIndex])-iROrigin_tv
			,(int)((rect_i->right +1 ) * g_dScale[m_iScaleIndex])-iCOrigin_tv
			,(int)((rect_i->bottom +1) * g_dScale[m_iScaleIndex])-iROrigin_tv);

		return rect_v;
	}



	BOOL CSImageViewerView::PreTranslateMessage(MSG* pMsg)
	{
		if(pMsg->message==WM_MOUSEWHEEL)
		{
			int iDelta;
			iDelta = GET_WHEEL_DELTA_WPARAM(pMsg->wParam);
			if(GetKeyState(VK_CONTROL)<0)
			{
				CPoint point_v;
				::GetCursorPos(&point_v);
				this->ScreenToClient(&point_v);

				if(iDelta>0){ZoomChange(point_v.y, point_v.x,1);}
				else{ZoomChange(point_v.y, point_v.x,-1);}
				return TRUE;
			}

			if(iDelta>0){OnScroll(SB_VERT, SB_LINEUP,0);}
			else{OnScroll(SB_VERT, SB_LINEDOWN,0);}

			return TRUE;
		}

		if(pMsg->message==WM_MOUSEHWHEEL)
		{
			int iDelta;
			iDelta = GET_WHEEL_DELTA_WPARAM(pMsg->wParam);
			if(GetKeyState(VK_CONTROL)<0)
			{
				CPoint point_v;
				::GetCursorPos(&point_v);
				this->ScreenToClient(&point_v);

				if(iDelta>0){ZoomChange(point_v.y, point_v.x,1);}
				else{ZoomChange(point_v.y, point_v.x,-1);}
				return TRUE;
			}

			if(iDelta>0){OnScroll(SB_HORZ, SB_LINEDOWN,0);}
			else{OnScroll(SB_HORZ, SB_LINEUP	,0);}

			return TRUE;
		}



		if (pMsg->message == WM_KEYDOWN)
		{	
			if(GetKeyState(VK_CONTROL)<0)
			{
				if (pMsg->wParam == 'C') 
				{ 
					if(m_Rect_i.IsRectEmpty()==TRUE){return FALSE;}

					CImage imgClipped;
					ClipImage(&m_imageProcessed[m_iImgIndex],&imgClipped, m_Rect_i.top,m_Rect_i.left, m_Rect_i.bottom, m_Rect_i.right); 
					CopyToClipBoardImg(&imgClipped);
					return TRUE;
				} 
				if (pMsg->wParam == 'V') 
				{ 
					BOOL bRet = CopyFromClipBoardImg(&m_image);
					if(bRet != TRUE){return FALSE;}
					m_sFilePath.Format(_T("Clipboard"));
					ResetImage();
					return TRUE;
				}

				if(pMsg->wParam == 'A')
				{
					FullDomain();
					return TRUE; 
				}
				if(pMsg->wParam == 'Z')
				{
					if(m_iUnDoAvailableCount<=0){return FALSE;}
					m_iImgIndex--;
					m_iUnDoAvailableCount--;
					m_iReDoAvailableCount++;
					Invalidate();
					return TRUE; 
				}
				if(pMsg->wParam == 'Y')
				{
					if(m_iReDoAvailableCount<=0){return FALSE;}
					m_iImgIndex++;
					m_iReDoAvailableCount--;
					m_iUnDoAvailableCount++;
					Invalidate();
					return TRUE; 
				}
			}

			if(GetKeyState(VK_SHIFT)<0)
			{
				if(pMsg->wParam == 'U'){OperateEquHistImage();return TRUE;}
				if(pMsg->wParam == 'G'){OperateBrightnessContrastGamma();return TRUE;}
			}	

			if(pMsg->wParam==VK_F5)
			{
				if(m_sFilePath.Compare(_T("Clipboard"))!=0)
				{
					return ReadImage(m_sFilePath);
				}
				ResetImage();
			}
			if(pMsg->wParam == 'R'){OperateRotaateImage(ROTATE_CW90);return TRUE;}

			if(pMsg->wParam == VK_RETURN) { if(m_bBingFullScreen==true){ ExitFullScreen(); return TRUE;} EnterFullScreen(); return TRUE; } 
			if(pMsg->wParam == VK_ESCAPE) { if(m_bBingFullScreen==true){ ExitFullScreen(); return TRUE;} ::PostQuitMessage( 0 );}

			if(pMsg->wParam == VK_ADD){ZoomChange(1);return TRUE;}
			if(pMsg->wParam == VK_SUBTRACT){ZoomChange(-1);return TRUE;}
			if(pMsg->wParam == VK_LEFT){OnScroll(SB_HORZ,SB_LINEUP,0);return TRUE; }
			if(pMsg->wParam == VK_RIGHT){OnScroll(SB_HORZ,SB_LINEDOWN,0); return TRUE; }
			if(pMsg->wParam == VK_UP){OnScroll(SB_VERT,SB_LINEUP,0);return TRUE; }
			if(pMsg->wParam == VK_DOWN){OnScroll(SB_VERT,SB_LINEDOWN,0); return TRUE; }
			if(pMsg->wParam == VK_PRIOR){OnScroll(SB_VERT,SB_PAGEUP,0);return TRUE; }
			if(pMsg->wParam == VK_NEXT){OnScroll(SB_VERT,SB_PAGEDOWN,0);return TRUE; }
			if(pMsg->wParam == VK_ADD){ZoomChange(1);}
			if(pMsg->wParam == VK_SUBTRACT){ZoomChange(-1);}
		}

		return CView::PreTranslateMessage(pMsg);
	}


	void CSImageViewerView::OnScroll(int iSB, int nSBCode, int nPos)
	{
		if((iSB==SB_VERT) && (m_bRBar== false)){return ;}
		if((iSB==SB_HORZ) && (m_bCBar== false)){return ;}


		int iHeight_v=GetClientHeight();
		int iWidth_v=GetClientWidth();

		int iBarWidth= ::GetSystemMetrics(SM_CYHSCROLL);
		int iBarHeight= ::GetSystemMetrics(SM_CXVSCROLL);

		int iWidth_i =max(0,m_imageProcessed[m_iImgIndex].GetWidth());
		int iHeight_i =max(0,m_imageProcessed[m_iImgIndex].GetHeight());

		int iWidth_tv = iWidth_i*g_dScale[m_iScaleIndex];
		int iHeight_tv= iHeight_i*g_dScale[m_iScaleIndex];



		int iHeightIfNoBar_v=iHeight_v+(m_bCBar ? iBarHeight : 0);
		int iWidthIfNoBar_v=iWidth_v+(m_bRBar ? iBarWidth : 0);


		SCROLLINFO si;
		GetScrollInfo(iSB,&si);
		if(si.nPage==0){return ;}

		int iPageSize=si.nPage;
		int iMin=si.nMin;
		int iMax=si.nMax;
		int iTrackPos =si.nTrackPos ;
		int iOldPos_scl;
		if(iSB==SB_VERT)
		{
			iOldPos_scl=GetDispOriginR_tv()*(iMax-iPageSize+1.0)/(iMax*1.0);
		}
		else
		{
			iOldPos_scl=GetDispOriginC_tv()*(iMax-iPageSize+1.0)/(iMax*1.0);
		}


		int iStep;
		if(g_dScale[m_iScaleIndex]==64)	{iStep=64;}
		else{iStep=max(int(g_dScale[m_iScaleIndex]),iPageSize/8.0);}

		int iNewPos_scl;
		switch (nSBCode)
		{
		case SB_LINEUP:		{iNewPos_scl=max(iMin, iOldPos_scl-iStep); break;}
		case SB_LINEDOWN:	{iNewPos_scl=min(iMax-iPageSize+1, iOldPos_scl+iStep); break;}
		case SB_PAGEUP:		{iNewPos_scl=max(iMin, iOldPos_scl-iPageSize); break;}
		case SB_PAGEDOWN:	{iNewPos_scl=min(iMax-iPageSize+1, iOldPos_scl+iPageSize); break;}
		case SB_THUMBTRACK:	{iNewPos_scl=max(iMin,min(iMax-iPageSize+1 , iTrackPos)); break;}
		default:{return;}
		}

		if(iSB==SB_VERT)
		{
			m_dDispOriginR_tv = max(0, iMax*(iNewPos_scl*1.0)/(iMax-iPageSize+1.0));
		}
		else
		{
			m_dDispOriginC_tv= max(0, iMax*(iNewPos_scl*1.0)/(iMax-iPageSize+1.0));
		}
		si.nPos = (iNewPos_scl); 
		SetScrollInfo(iSB, &si, TRUE);
		Invalidate();
	}


	void CSImageViewerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		OnScroll(SB_HORZ, nSBCode,nPos);
	}


	void CSImageViewerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		OnScroll(SB_VERT, nSBCode,nPos);
	}


	BOOL CSImageViewerView::OnEraseBkgnd(CDC* pDC)
	{
		return TRUE;
		return CView::OnEraseBkgnd(pDC);
	}
