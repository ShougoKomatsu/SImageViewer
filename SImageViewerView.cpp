
se // SImageViewerView.cpp : CSImageViewerView クラスの実装
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
#include "ExtractChannelDlg.h"
#include "FormatSelectionDlg.h"
#include "ChangeColorDepthDlg.h"
#include "SImgProc_ex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_INIT (100)
#define TIMER_REFRESH (101)
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
		ON_WM_RBUTTONUP()
		ON_COMMAND(ID_FILE_OPEN, &CSImageViewerView::OnFileOpen)
		ON_COMMAND(ID_FILE_SAVE_AS, &CSImageViewerView::OnFileSave)
		ON_COMMAND(ID_EDIT_COPY, &CSImageViewerView::OnEditCopy)
		ON_COMMAND(ID_EDIT_PASTE, &CSImageViewerView::OnEditPaste)
		ON_COMMAND(ID_MENU_SET_SELECTION, &CSImageViewerView::OnSetSelection)
		ON_COMMAND(ID_MENU_COPY_AS, &CSImageViewerView::OnCopyAs)
		ON_COMMAND(ID_MENU_CONVERT_COLOR_SPACE, &CSImageViewerView::OperateConvertColorSpace)
		ON_COMMAND(ID_MENU_CHANGE_COLOR_DEPTH, &CSImageViewerView::OperateChangeColorDepth)
		ON_COMMAND(ID_MENU_COLOR_CORRECTON, &CSImageViewerView::OperateBrightnessContrastGamma)
		ON_COMMAND(ID_MENU_EQU_HIST, &CSImageViewerView::OperateEquHistImage)
		ON_COMMAND(ID_MENU_RESAMPLE, &CSImageViewerView::OperateResample)
		ON_WM_SIZE()
		ON_WM_MOUSEMOVE()
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_WM_TIMER()
		ON_WM_SETCURSOR()
		ON_WM_VSCROLL()
		ON_WM_HSCROLL()
		ON_WM_ERASEBKGND()
		ON_WM_CONTEXTMENU()
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
		if(m_Rect_i.IsRectEmpty()==TRUE){return;}

		CCopyAsDlg copyAsdlg;

		INT_PTR iRet = copyAsdlg.DoModal();
		if(iRet != IDOK){return;}

		CImage imgClipped;

		ClipImage(&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)],&imgClipped, m_Rect_i.top,m_Rect_i.left, m_Rect_i.bottom, m_Rect_i.right); 
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
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->m_bSelected = true;
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

		if (m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].IsNull()){return;}

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

		ZoomImage(&(m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)]),&imgZoomed,
			dR0_i,
			dC0_i,
			g_dScale[m_iScaleIndex],
			iWidth_v,iHeight_v);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		int iGrid=0;
		switch(pFrame->m_iGrid)
		{
		case ID_BUTTON_GRID_NONE:{iGrid=0;break;}
		case ID_BUTTON_GRID_DOT:{iGrid=1;break;}
		case ID_BUTTON_GRID_LINE:{iGrid=2;break;}
		case ID_BUTTON_GRID_CONNECT:{iGrid=3;break;}
		default:{iGrid=0;}
		}
		ImposeGrid(&imgZoomed, &imgZoomed, iGrid, int(dR0_i)-dR0_i, int(dC0_i)-dC0_i,g_dScale[m_iScaleIndex], 10);

		if(m_bDragging==true)
		{
			if (m_Rect_v.IsRectEmpty()==FALSE)
			{
				CRect rect_i= v_to_i(&m_Rect_v);
				CRect rect_v = i_to_v(&rect_i);
				ImposeRect(&imgZoomed, &imgZoomed,&rect_v);
			}
		}
		else
		{
			if (m_Rect_i.IsRectEmpty()==FALSE)
			{
				CRect rect_v = i_to_v(&m_Rect_i);
				ImposeRect(&imgZoomed, &imgZoomed,&rect_v);
			}
		}
		CImage imgAlphaed;
		ImposeAlphaChannel(&imgZoomed,&imgAlphaed);

		imgAlphaed.BitBlt( memDC.GetSafeHdc(), 0, 0,imgAlphaed.GetWidth(), imgAlphaed.GetHeight(), 0, 0  );

		pDC->BitBlt(0, 0, iWidth_v, iHeight_v, &memDC, 0, 0,SRCCOPY);

		memDC.SelectObject(pOldBmp);


	}

	void CSImageViewerView::OnRButtonUp(UINT /* nFlags */, CPoint point_v)
	{
		ClientToScreen(&point_v);
		OnContextMenu(this, point_v);
	}

	//	void CSImageViewerView::OnContextMenu(CWnd* /* pWnd */, CPoint point_v)
	//	{
	//#ifndef SHARED_HANDLERS
	//		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point_v.x, point_v.y, this, TRUE);
	//#endif
	//	}


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

		int iWidth_i =max(0,m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetWidth());
		int iHeight_i =max(0,m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetHeight());

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
		m_iImgIndex=0;
		if(m_bRefresh==true){KillTimer(TIMER_REFRESH);}
		if(m_image.GetBPP()==32)
		{	
			SetTimer(TIMER_REFRESH,50,0);
		}
		CopyImage(&m_image,&(m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)]));
		m_iScaleIndex =8;

		SetGridEnableDesable();

		CRect rectClient;
		GetClientRect(&rectClient);

		int iHeight_v=GetClientHeight();
		int iWidth_v=GetClientWidth();

		int iBarWidth= ::GetSystemMetrics(SM_CYHSCROLL);
		int iBarHeight= ::GetSystemMetrics(SM_CXVSCROLL);

		int iWidth_i =max(0,m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetWidth());
		int iHeight_i =max(0,m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetHeight());

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
		pFrame->m_bSelected = false;
		Invalidate();

		CString sImageSize;
		sImageSize.Format(_T("W %d x H %d"), m_image.GetWidth(),m_image.GetHeight());

		CView::OnInitialUpdate();
		pFrame->m_sStatusSize.Format(_T("%s"),sImageSize);
		pFrame->SendMessage(WM_COMMAND, ID_DISP_STATUS_SIZE);

		pFrame->m_sStatusBPP.Format(_T("%d BPP"),m_image.GetBPP());
		pFrame->SendMessage(WM_COMMAND, ID_DISP_STATUS_BPP);


		if (pFrame != nullptr)	{pFrame->SetStatusMessage(sImageSize);}

	}

	bool CSImageViewerView::ReadImage(CString sFilePath)
	{
		CFileFind cf;
		BOOL bRet = cf.FindFile(sFilePath);
		if(bRet != TRUE){return false;}
		if(m_image.IsNull()!=true){m_image.Destroy();}
		HRESULT hResult = m_image.Load(m_sFilePath);
		if(hResult != S_OK){return false;}


		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->m_bFileOpened = true;
		pFrame->m_bSelected = true;


		ResetImage();
		return true;
	}

	bool CSImageViewerView::SaveImage(CImage* image)
	{

		CFormatSelectionDlg formatDlg;
		formatDlg.m_iBPP = image->GetBPP();
		formatDlg.m_bImageIsMonochrome= IsImageMonochrome(image);
		INT_PTR iRet = formatDlg.DoModal();
		if(iRet != IDOK){return false;}
		CString sFileExt;
		CString sFilter;

		switch(formatDlg.m_iFormat)
		{
		case 0:{sFileExt.Format(_T("bmp"));sFilter.Format(_T("BitMap|*.bmp"));break;}
		case 1:{sFileExt.Format(_T("png"));sFilter.Format(_T("PNG|*.png"));break;}
		}


		CFileDialog cf(FALSE, (LPCTSTR)sFileExt,NULL, 0, (LPCTSTR)sFilter);
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

		CImage imgWrite;
		bRet = MakeReservedChannelZero(image,&imgWrite);
		if(bRet != true){return false;}

		HRESULT hResult = imgWrite.Save(sFilePath);
		if(hResult != S_OK){return false;}

		return true;
	}

	void CSImageViewerView::OnFileSave()
	{
		SaveImage(&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)]);
	}
	void CSImageViewerView::OnFileOpen()
	{
		CFileDialog cf(TRUE);
		//		cf.m_ofn.lpstrInitialDir = sMacroFolderPath;
		if(cf.DoModal()!=IDOK){ return;}
		m_sFilePath.Format(_T("%s"),cf.GetPathName());
		ReadImage(m_sFilePath);

	}
	void CSImageViewerView::OnEditCopy()
	{
		if(m_Rect_i.IsRectEmpty()==TRUE){return;}

		CImage imgClipped;
		ClipImage(&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)],&imgClipped, m_Rect_i.top,m_Rect_i.left, m_Rect_i.bottom, m_Rect_i.right); 
		CopyToClipBoardImg(&imgClipped);
		return;
	}
	void CSImageViewerView::OnEditPaste()
	{
		BOOL bRet = CopyFromClipBoardImg(&m_image);
		if(bRet != TRUE){return;}
		m_sFilePath.Format(_T("Clipboard"));

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->m_bFileOpened = true;
		pFrame->m_bSelected = true;
		ResetImage();

	}

	void CSImageViewerView::FullDomain()
	{
		m_Rect_i.SetRect(0, 0, m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetWidth()-1,m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetHeight()-1);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->m_bSelected = true;
	}

	void CSImageViewerView::OperateEquHistImage()
	{
		bool bAutoFull = false;
		if(m_Rect_i.IsRectEmpty()==TRUE){bAutoFull=true; FullDomain();}

		ImgRGB imgRGB;
		ImgRGB imgMeaned;
		ConvertImage(&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)], &imgRGB);
		EquHistImage(&imgRGB,&imgMeaned,m_Rect_i.top,m_Rect_i.left,m_Rect_i.bottom,m_Rect_i.right);
		if(bAutoFull==true)
		{
			m_Rect_i.SetRectEmpty();
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->m_bSelected = false;
		}

		m_iImgIndex++;
		m_iUnDoAvailableCount++;
		if(m_iUnDoAvailableCount>=MAX_IMG_BUF-1){m_iUnDoAvailableCount=MAX_IMG_BUF-1;}
		ConvertImage(&imgMeaned,&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)]);
		Invalidate();
	}
	void CSImageViewerView::OperateResample()
	{

		bool bAutoFull = false;
		if(m_Rect_i.IsRectEmpty()==TRUE){bAutoFull=true; FullDomain();}

		CResampleDlg dlg;
		dlg.m_iHeightOrg = m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetHeight();
		dlg.m_iWidthOrg = m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetWidth();
		INT_PTR iRet = dlg.DoModal();
		if(iRet != IDOK){return;}

		CImage imgSrc;
		CopyImage(&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)], &imgSrc);
		m_iImgIndex++;
		m_iUnDoAvailableCount++;
		if((dlg.m_resample==RESIZE_NEAREST) || (dlg.m_resample==RESIZE_BILINEAR))
		{
			Resize(&imgSrc, &m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)], dlg.m_iWidth,dlg.m_iHeight,dlg.m_resample);
		}
		else
		{
			Resample(&imgSrc, &m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)], dlg.m_resample);
		}
		Invalidate();
	}
	void CSImageViewerView::OperateChangeColorDepth()
	{

		CChangeColorDepthDlg colorDepthDlg;

		int iUsedColors;
		bool bGrayScale;

		CountColorNum(&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)], &iUsedColors, NULL);

		MakeColorTable(&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)],NULL,NULL, 1<<min(24, m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetBPP()), &iUsedColors, &bGrayScale);

		colorDepthDlg.m_iColors = iUsedColors;
		colorDepthDlg.m_bGrayScale = bGrayScale;
		colorDepthDlg.m_iBPP = m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetBPP();

		INT_PTR iRet = colorDepthDlg.DoModal();
		if(iRet != IDOK){return;}

		CImage imgSrc;
		CopyImage(&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)], &imgSrc);
		m_iImgIndex++;
		m_iUnDoAvailableCount++;
		if(m_iUnDoAvailableCount>=MAX_IMG_BUF-1){m_iUnDoAvailableCount=MAX_IMG_BUF-1;}

		switch(colorDepthDlg.m_iMode)
		{
		case 0:{ConvertImage_LossLess(&imgSrc, colorDepthDlg.m_iBPP, &m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)]);break;}
		case 1:{ConvertImage_AreaCoverage(&imgSrc,colorDepthDlg.m_iBPP, &m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)]);break;}
		case 2:{ConvertImage_ByDeviation(&imgSrc, colorDepthDlg.m_iBPP, &m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)]);break;}
		}
		//		m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].Save(_T("D:\\test.bmp"));
		Invalidate();
	}

	void CSImageViewerView::OperateConvertColorSpace()
	{
		ENUM_COLOR color;
		CExtractChannelDlg extractDlg;

		INT_PTR iRet = extractDlg.DoModal();
		if(iRet != IDOK){return;}

		color= extractDlg.m_enumColor;

		CImage imgSrc;
		CopyImage(&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)], &imgSrc);
		m_iImgIndex++;
		m_iUnDoAvailableCount++;
		if(m_iUnDoAvailableCount>=MAX_IMG_BUF-1){m_iUnDoAvailableCount=MAX_IMG_BUF-1;}

		ExtractChannel(&imgSrc,&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)], color);
		Invalidate();
		return;
	}
	void CSImageViewerView::OperateRotaateImage(enumRotate rotate)
	{
		bool bAutoFull = false;

		ImgRGB imgRGB;
		ImgRGB imgResult;
		ConvertImage(&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)], &imgRGB);

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

		CImage imgClipped;
		ClipImage(&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)],&imgClipped, m_Rect_i.top,m_Rect_i.left, m_Rect_i.bottom, m_Rect_i.right); 
		CopyImage(&imgClipped, &dlgModify.m_image);
		INT_PTR iRet;
		dlgModify.DoModal();
		iRet = dlgModify.m_iRet;
		if(iRet == IDCANCEL)
		{
			if(bAutoFull==true)
			{
				m_Rect_i.SetRectEmpty();
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				pFrame->m_bSelected = false;
			}
			return ;
		}

		int iBrightness=dlgModify.m_iBrightness;
		int iContrast=dlgModify.m_iContrast;
		double dGamma=dlgModify.m_dGamma;

		ImgRGB imgRGB;
		ImgRGB imgResult1;
		ImgRGB imgResult2;
		ConvertImage(&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)], &imgRGB);
		BrightnessContrast(&imgRGB,&imgResult1,m_Rect_i.top,m_Rect_i.left,m_Rect_i.bottom,m_Rect_i.right,(double)iBrightness,(double)iContrast);
		Gamma(&imgResult1,&imgResult2,m_Rect_i.top,m_Rect_i.left,m_Rect_i.bottom,m_Rect_i.right,dGamma);
		if(bAutoFull==true)
		{
			m_Rect_i.SetRectEmpty();
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->m_bSelected = false;
		}

		m_iImgIndex++;
		m_iUnDoAvailableCount++;
		if(m_iUnDoAvailableCount>=MAX_IMG_BUF-1){m_iUnDoAvailableCount=MAX_IMG_BUF-1;}
		ConvertImage(&imgResult2,&m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)]);
		Invalidate();
	}



	void CSImageViewerView::OnSize(UINT nType, int cx, int cy)
	{
		CView::OnSize(nType, cx, cy);

		if(m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].IsNull()==true){return;}

		SetScroll();
		Invalidate();
	}

	void CSImageViewerView::OnInitialUpdate()
	{
		CView::OnInitialUpdate();
		CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, GetParentFrame());
		pFrame->m_sStatusSize.Format(_T("W 0 x H 0"));
		pFrame->SendMessage(WM_COMMAND, ID_DISP_STATUS_SIZE);
		pFrame->m_sStatusBPP.Format(_T("0 BPP"));
		pFrame->SendMessage(WM_COMMAND, ID_DISP_STATUS_BPP);
		m_bRefresh=false;
		/*
		CImage imgTest;
		imgTest.Create(16,16,8);
		RGBQUAD colorTable[256];
		for(int i=0; i<256; i++)
		{
		colorTable[i].rgbBlue=i;
		colorTable[i].rgbGreen=i;
		colorTable[i].rgbRed=i;
		colorTable[i].rgbReserved=255;
		}

		SetColorTable(&imgTest, colorTable, 256);
		int iPitch = imgTest.GetPitch();
		BYTE* byData = (BYTE*)imgTest.GetBits();
		for(int r=0; r<16; r++)
		{
		for(int c=0; c<16; c++)
		{
		byData[r*iPitch+c]=r*16+c;
		}
		}

		imgTest.Save(_T("d:\\16_16_256.bmp"));
		*/
		int iHeight_v=GetClientHeight();
		int iWidth_v=GetClientWidth();
		int iBarWidth= ::GetSystemMetrics(SM_CYHSCROLL);
		int iBarHeight= ::GetSystemMetrics(SM_CXVSCROLL);
		int iHeightIfNoBar_v=iHeight_v+(m_bCBar ? iBarHeight : 0);
		int iWidthIfNoBar_v=iWidth_v+(m_bRBar ? iBarWidth : 0);
		m_imageProcessed[m_iImgIndex].Create(100,100,0);
		pFrame->AdjustViewClientSize(100, 100,iWidthIfNoBar_v, iHeightIfNoBar_v);
		/*
		int ii[100];
		for(int i=0; i<100; i++)
		{
		ii[i]=rand();
		}
		int iIndex[100];
		index_i(ii,100,iIndex);

		int i2[100];
		for(int i=0; i<100; i++)
		{
		i2[i]=ii[iIndex[i]];
		}
		for(int i=0; i<99; i++)
		{
		if(ii[iIndex[i]]>ii[iIndex[i+1]])
		{
		break;
		}
		}*/

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

		int iWidth_i =max(0,m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetWidth());
		int iHeight_i =max(0,m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetHeight());

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

		SetGridEnableDesable();
		SetScroll();
		Invalidate();

		CPoint point_v;
		GetCursorPos(&point_v);
		ScreenToClient(&point_v);
		DispStatus(point_v);
		return true; 
	}
	void CSImageViewerView::SetGridEnableDesable()
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if(g_dScale[m_iScaleIndex]>10){	pFrame->m_bGridAble=true;}
		else{pFrame->m_bGridAble=false;}
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

		int iWidth_i =max(0,m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetWidth());
		int iHeight_i =max(0,m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetHeight());

		double dWidth_tv = iWidth_i*g_dScale[m_iScaleIndex];
		double dHeight_tv= iHeight_i*g_dScale[m_iScaleIndex];
		double dNewDispOriginC_tv;
		double dNewDispOriginR_tv;
		m_iScaleIndex+=iChange;		

		SetGridEnableDesable();
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

		SetGridEnableDesable();

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


	bool CSImageViewerView::GetColorAtCursor(CImage* img, CPoint point_v, int* iR_img, int* iC_img, BYTE* byR, BYTE* byG, BYTE* byB)
	{
		if(img->IsNull()==true){return false;}
		CPoint point_tv(point_v.x + GetDispOriginC_tv(), point_v.y +  GetDispOriginR_tv());

		int iC_img_Local = (int)((point_tv.x) / g_dScale[m_iScaleIndex]);
		int iR_img_Local = (int)((point_tv.y) / g_dScale[m_iScaleIndex]);


		if (iC_img_Local < 0){return false;}
		if (iR_img_Local < 0){return false;}
		if (iC_img_Local >= img->GetWidth()){return false;}
		if (iR_img_Local >= img->GetHeight()){return false;}

		COLORREF col = img->GetPixel(iC_img_Local,iR_img_Local);

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
		BYTE byR=0,byG=0,byB=0;
		BYTE byR_Processed = 0,byG_Processed = 0,byB_Processed = 0;
		CString sCaption;
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

		bool bRet_Original = GetColorAtCursor(&m_image, point_v, &iR_img, &iC_img, &byR, &byG, &byB);
		if(bRet_Original == false)
		{
			pFrame->m_sStatusRGBOriginal.Format(_T("out of range"));
		}
		else
		{
			pFrame->m_sStatusRGBOriginal.Format(_T("(%d, %d, %d)"), byR, byG, byB);
		}

		bool bRet_Processed=false;
		pFrame->m_sStatusRGBProcessed.Format(_T("not processed"));
		if(m_iImgIndex != 0)
		{
			bRet_Processed = GetColorAtCursor(&(m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)]), point_v, &iR_img, &iC_img, &byR_Processed, &byG_Processed, &byB_Processed);
			if(bRet_Processed == false)
			{
				pFrame->m_sStatusRGBProcessed.Format(_T("out of range"));
			}
			else
			{
				pFrame->m_sStatusRGBProcessed.Format(_T("(%d, %d, %d)"), byR_Processed, byG_Processed, byB_Processed);
			}
		}

		if((bRet_Original == false) && (bRet_Processed == false))
		{
			pFrame->m_sStatusMousePos.Format(_T("out of range"));
		}
		else
		{
			pFrame->m_sStatusMousePos.Format(_T("(%d, %d)"),iC_img, iR_img);
		}
		if(m_bDragging == true)
		{
			if(m_Rect_v.IsRectEmpty()==TRUE)
			{
				pFrame->m_sStatusSelection.Format(_T("not selected"));
			}
			else
			{
				CRect rectTemp = v_to_i(&m_Rect_v);
				pFrame->m_sStatusSelection.Format(_T("(%d, %d) - (%d, %d) : %d x %d "), rectTemp.left,rectTemp.top,rectTemp.right,rectTemp.bottom,rectTemp.right-rectTemp.left+1,rectTemp.bottom-rectTemp.top+1);	
			}
		}
		else
		{
			if(m_Rect_i.IsRectEmpty()==TRUE)
			{
				pFrame->m_sStatusSelection.Format(_T("not selected"));
			}
			else
			{
				pFrame->m_sStatusSelection.Format(_T("(%d, %d) - (%d, %d) : %d x %d "), m_Rect_i.left,m_Rect_i.top,m_Rect_i.right,m_Rect_i.bottom,m_Rect_i.right-m_Rect_i.left+1,m_Rect_i.bottom-m_Rect_i.top+1);	
			}
		}

		pFrame->SendMessage(WM_COMMAND, ID_DISP_STATUS_MOUSE_POS);

		pFrame->m_sStatusZoom.Format(_T("%.3f%%"), 100*g_dScale[m_iScaleIndex]);
		pFrame->SendMessage(WM_COMMAND, ID_DISP_STATUS_ZOOM);


		sCaption.Format(sFileName);
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

		if(m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].IsNull()==true){return;}

		DispStatus(point_v);

		if (m_bDragging==true) 
		{ 
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
			CView::OnMouseMove(nFlags, point_v);
			Invalidate();
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

			if(m_PointStart_v==point_v)
			{
				if(m_iMouseMode==CHANGE_B){CView::OnLButtonUp(nFlags, point_v); return;}
				if(m_iMouseMode==CHANGE_L){CView::OnLButtonUp(nFlags, point_v); return;}
				if(m_iMouseMode==CHANGE_R){CView::OnLButtonUp(nFlags, point_v); return;}
				if(m_iMouseMode==CHANGE_U){CView::OnLButtonUp(nFlags, point_v); return;}
				if(m_iMouseMode==CHANGE_LB){CView::OnLButtonUp(nFlags, point_v); return;}
				if(m_iMouseMode==CHANGE_LU){CView::OnLButtonUp(nFlags, point_v); return;}
				if(m_iMouseMode==CHANGE_RB){CView::OnLButtonUp(nFlags, point_v); return;}
				if(m_iMouseMode==CHANGE_RU){CView::OnLButtonUp(nFlags, point_v); return;}

				CRect rect_v;
				rect_v = i_to_v(&m_Rect_i);
				if((point_v.y>=rect_v.top)&&(point_v.y<=rect_v.bottom)&&(point_v.x>=rect_v.left)&&(point_v.x<=rect_v.right) && (m_iMouseMode==CHANGE_ZOOMUP))
				{
					ZoomChange(m_Rect_i.top, m_Rect_i.left, m_Rect_i.bottom,m_Rect_i.right);
					m_Rect_v.SetRectEmpty();
					m_Rect_i.SetRectEmpty();
					m_iMouseMode=CHANGE_NONE;
					CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
					pFrame->m_bSelected = false;
					Invalidate();
					CView::OnLButtonUp(nFlags, point_v);
					return;
				}
				m_Rect_v.SetRectEmpty();
				m_Rect_i.SetRectEmpty();
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				pFrame->m_bSelected = false;
				Invalidate();
				CView::OnLButtonUp(nFlags, point_v);
				return;
			}
			m_Rect_i = v_to_i(&m_Rect_v);
			m_Rect_i.left=max(0,m_Rect_i.left);
			m_Rect_i.top=max(0,m_Rect_i.top);
			m_Rect_v.SetRectEmpty();
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->m_bSelected = true;
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
		
		if(nIDEvent==TIMER_REFRESH)
		{
			Invalidate();
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
				//			if(pMsg->wParam == 'U'){OperateEquHistImage();return TRUE;}
				//		if(pMsg->wParam == 'G'){OperateBrightnessContrastGamma();return TRUE;}
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

		int iWidth_i =max(0,m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetWidth());
		int iHeight_i =max(0,m_imageProcessed[(m_iImgIndex % MAX_IMG_BUF)].GetHeight());

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


	void CSImageViewerView::OnContextMenu(CWnd* pWnd, CPoint point)
	{

		CMenu menu;
		menu.LoadMenu(IDR_POPUP_EDIT);

		CMenu* pPopup = menu.GetSubMenu(0);

		UpdateDialogControls(this, FALSE);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		bool bFileOpened = pFrame ->m_bFileOpened;
		bool bSelected = pFrame ->m_bSelected;

		pPopup->EnableMenuItem(ID_EDIT_COPY, MF_BYCOMMAND | (( bSelected  == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_MENU_COPY_AS, MF_BYCOMMAND | (( bSelected  == true) ? MF_ENABLED : MF_DISABLED));

		pPopup->EnableMenuItem(ID_EDIT_COPY, MF_BYCOMMAND | (( bFileOpened  == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_EDIT_PASTE, MF_BYCOMMAND | (( bFileOpened  == true) ? MF_ENABLED : MF_DISABLED));

		pPopup->EnableMenuItem(ID_MENU_EQU_HIST, MF_BYCOMMAND | (( bFileOpened  == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_MENU_CONVERT_COLOR_SPACE, MF_BYCOMMAND | (( bFileOpened  == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_MENU_CHANGE_COLOR_DEPTH, MF_BYCOMMAND | (( bFileOpened  == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_MENU_COLOR_CORRECTON, MF_BYCOMMAND | (( bFileOpened  == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_MENU_SET_SELECTION, MF_BYCOMMAND | (( bFileOpened  == true) ? MF_ENABLED : MF_DISABLED));

		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}