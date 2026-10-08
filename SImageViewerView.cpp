
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
#include "PasteAsDlg.h"
#include "CommonFunction.h"
#include "ExtractChannelDlg.h"
#include "FormatSelectionDlg.h"
#include "ChangeColorDepthDlg.h"
#include "SImgProc_ex.h"
#include "FileFormatDlg.h"
#include "InputDlg.h"
#include "ColorizeDlg.h"
#include "SetTransparentDlg.h"
#include "FullScreenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_INIT (100)
#define TIMER_REFRESH (101)
// CSImageViewerView

IMPLEMENT_DYNCREATE(CSImageViewerView, CView)

	BEGIN_MESSAGE_MAP(CSImageViewerView, CView)
		ON_WM_RBUTTONUP()
		ON_COMMAND(ID_FILE_OPEN, &CSImageViewerView::OnFileOpen)
		ON_COMMAND(ID_MENU_FILE_ADD, &CSImageViewerView::OnFileAdd)
		ON_COMMAND(ID_FILE_SAVE_AS, &CSImageViewerView::OnFileSave)
		ON_COMMAND(ID_FILE_SAVE, &CSImageViewerView::OnFileSave)
		ON_COMMAND(ID_EDIT_COPY, &CSImageViewerView::OnEditCopy)
		ON_COMMAND(ID_EDIT_PASTE, &CSImageViewerView::OnEditPaste)
		ON_COMMAND(ID_MENU_EDIT_SET_SELECTION, &CSImageViewerView::OnSetSelection)
		ON_COMMAND(ID_MENU_EDIT_SELECT_ALL, &CSImageViewerView::OnSelectAll)
		ON_COMMAND(ID_MENU_EDIT_COPY_AS, &CSImageViewerView::OnCopyAs)
		ON_COMMAND(ID_MENU_EDIT_PASTE_AS, &CSImageViewerView::OnPasteAs)
		ON_COMMAND(ID_MENU_EDIT_CONVERT_COLOR_SPACE, &CSImageViewerView::OnConvertColorSpace)
		ON_COMMAND(ID_MENU_EDIT_CHANGE_COLOR_DEPTH, &CSImageViewerView::OnChangeColorDepth)
		ON_COMMAND(ID_MENU_EDIT_COLOR_CORRECTON, &CSImageViewerView::OnBrightnessContrastGamma)
		ON_COMMAND(ID_MENU_EDIT_COLORIZE, &CSImageViewerView::OnColorize)
		ON_COMMAND(ID_MENU_EDIT_TRANSPARENT, &CSImageViewerView::OnTransparent)
		ON_COMMAND(ID_MENU_EDIT_INVERT, &CSImageViewerView::OnInvert)
		ON_COMMAND(ID_MENU_EDIT_EQU_HIST, &CSImageViewerView::OnEquHistImage)
		ON_COMMAND(ID_MENU_EDIT_RESAMPLE, &CSImageViewerView::OnResample)
		ON_COMMAND(ID_MENU_EDIT_REDO, &CSImageViewerView::OnReDo)
		ON_COMMAND(ID_EDIT_UNDO, &CSImageViewerView::OnUnDo)

		ON_COMMAND(ID_MENU_VIEW_FW, &CSImageViewerView::OnFW)
		ON_COMMAND(ID_MENU_VIEW_PP, &CSImageViewerView::OnPP)
		ON_COMMAND(ID_MENU_VIEW_FW10, &CSImageViewerView::OnFW10)
		ON_COMMAND(ID_MENU_VIEW_PP10, &CSImageViewerView::OnPP10)
		ON_COMMAND(ID_MENU_VIEW_FW_LAST, &CSImageViewerView::OnFWLast)
		ON_COMMAND(ID_MENU_VIEW_PP_FIRST, &CSImageViewerView::OnPPFirst)
		ON_COMMAND(ID_MENU_VIEW_FLEXIBLE_FW, &CSImageViewerView::OnFWFlexible)
		ON_COMMAND(ID_MENU_VIEW_FLEXIBLE_PP, &CSImageViewerView::OnPPFlexible)

		ON_COMMAND(ID_MENU_TOOL_FILEFORMAT, &CSImageViewerView::SetToolFormat)
		ON_COMMAND(ID_MENU_DATA_HISTGRAM, &CSImageViewerView::OnCopyHistGramToClipboard)
		ON_COMMAND(ID_MENU_DATA_CORRELATION, &CSImageViewerView::OnCopyCorrelMapToClipboard)
		ON_COMMAND(ID_MENU_DATA_AVERAGE, &CSImageViewerView::OnCopyAverageToClipboard)
		ON_COMMAND(ID_MENU_DATA_VARIANCE, &CSImageViewerView::OnCopyVarianceToClipboard)
		ON_COMMAND(ID_MENU_DATA_FILELIST, &CSImageViewerView::OnCopyFileListToClipboard)

		ON_COMMAND(ID_MENU_EDIT_RENAME, &CSImageViewerView::OnRename)
		ON_COMMAND(ID_MENU_EDIT_CW90, &CSImageViewerView::OnRotateCW90)
		ON_COMMAND(ID_MENU_EDIT_RESET, &CSImageViewerView::OnReSet)
		ON_COMMAND(ID_MENU_EDIT_FLIP_UD, &CSImageViewerView::OnFlipUD)
		ON_COMMAND(ID_MENU_EDIT_FLIP_LR, &CSImageViewerView::OnFlipLR)

		ON_COMMAND(ID_MENU_TOOL_OPTION, &CSImageViewerView::OnSetToolOption)
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
		view.Init();

		m_bSynchroScroll=false;
		m_image = NULL;
		m_iImageIndex = 0;
		m_iImageMax = 0;
		m_sFilePath = _T("");
		if(g_sParam.GetLength()>0){m_sFilePath.Format(_T("%s"), g_sParam);}
	}

	CSImageViewerView::~CSImageViewerView()
	{

	}

	void CSImageViewerView::ToggleScrollPin()
	{
		if(m_bSynchroScroll == true)
		{
			m_bSynchroScroll = false;
			m_image[m_iImageIndex].scr.m_dDispOriginR_tv=  scr.m_dDispOriginR_tv;
			m_image[m_iImageIndex].scr.m_dDispOriginC_tv=  scr.m_dDispOriginC_tv;
			m_image[m_iImageIndex].scr.m_iScaleIndex=  scr.m_iScaleIndex;
			return;
		}
		else
		{

			scr.m_dDispOriginR_tv=  m_image[m_iImageIndex].scr.m_dDispOriginR_tv;
			scr.m_dDispOriginC_tv=  m_image[m_iImageIndex].scr.m_dDispOriginC_tv;
			scr.m_iScaleIndex=  m_image[m_iImageIndex].scr.m_iScaleIndex;

		}
		m_bSynchroScroll = true;

	}

	BOOL CSImageViewerView::PreCreateWindow(CREATESTRUCT& cs)
	{
		// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
		// 修正してください。

		return CView::PreCreateWindow(cs);
	}

	void CSImageViewerView::OnCopyAs()
	{
		if(m_iImageMax <= 0){return;}
		CRect rect_i=view.GetRect_i();
		if(rect_i.IsRectNull() == TRUE){return;}

		CCopyAsDlg copyAsdlg;

		INT_PTR iRet = copyAsdlg.DoModal();
		if(iRet != IDOK){return;}

		CImage imgClipped;

		ClipImage(m_image[m_iImageIndex].GetCurrentProcess(),&imgClipped, rect_i.top,rect_i.left, rect_i.bottom, rect_i.right); 
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
		CRect rect_i=view.GetRect_i();
		if(rect_i.IsRectNull() != TRUE)
		{
			setdlg.m_iC0 = rect_i.left;
			setdlg.m_iR0 = rect_i.top;
			setdlg.m_iC1 = rect_i.right;
			setdlg.m_iR1 = rect_i.bottom;
		}

		INT_PTR iRet = setdlg.DoModal();
		if(iRet == IDOK)
		{
			rect_i.SetRect(setdlg.m_iC0,setdlg.m_iR0,setdlg.m_iC1,setdlg.m_iR1);
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			view.m_bRegionSelected = true;
			Invalidate();
		}
	}

	// CSImageViewerView 描画

	void CSImageViewerView::OnDraw(CDC* pDC)
	{
		CSImageViewerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc){return;}
		if(m_iImageMax <= 0){return;}

		view.OnDraw(this, pDC,  &(m_image[m_iImageIndex]));

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

	void CSImageViewerView::SetScroll()
	{
		if(m_iImageMax <= 0){return;}
		view.SetScroll(&(m_image[m_iImageIndex]), this);
		if(m_bSynchroScroll==true)
		{
			view.GetScrollSetting(&scr);
		}
		else
		{
			view.GetScrollSetting(&(m_image[m_iImageIndex].scr));
		}
	}
	void CSImageViewerView::ZoomReset()
	{
		view.SetDispOriginC_tv(0);
		view.SetDispOriginR_tv(0);
		CPoint point_v;
		view.ZoomChangeAbs(1.0, &(m_image[m_iImageIndex]), this, &point_v);
		DispStatus(point_v);
		//			view.ZoomReset(&(m_image[m_iImageIndex]), this);
		if(m_bSynchroScroll==true)
		{
			view.GetScrollSetting(&scr);
		}
		else
		{
			view.GetScrollSetting(&(m_image[m_iImageIndex].scr));
		}
	}

	void CSImageViewerView::ResetImage(bool bZoomReset, bool bProcessReset)
	{
		if(m_iImageMax <= 0){return;}

		if(bProcessReset == true)
		{
			m_image[m_iImageIndex].ResetProcessImage();
		}
		if(m_bRefresh == true){KillTimer(TIMER_REFRESH);}

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame == nullptr){return;}
		switch(m_image[m_iImageIndex].GetImageType())
		{
		case IMAGE_TYPE_CIMAGE:
			{
				if(m_image[m_iImageIndex].GetCImage()->GetBPP() == 32){SetTimer(TIMER_REFRESH,50,0);}

				if(m_image[m_iImageIndex].GetCImage()->GetBPP() == 24)
				{
					pFrame->SendMessage(ID_DISP_STATUS_CHANGE_CHANNEL, 24);
				}
				else
				{
					pFrame->SendMessage(ID_DISP_STATUS_CHANGE_CHANNEL, 32);
				}
				break;
			}
		case IMAGE_TYPE_IIMAGE:
		case IMAGE_TYPE_DIMAGE:
			{
				pFrame->SendMessage(ID_DISP_STATUS_CHANGE_CHANNEL, 24);
				break;
			}
		default:{return;}
		}
		SetGridEnableDesable();

		if(bZoomReset == true)
		{
			ZoomReset();
		}
		view.SetMouseMode(CHANGE_NONE);
		view.SetRect_i(NULL);
		view.m_bRegionSelected = false;
		Invalidate();

		CString sImageSize;
		sImageSize.Format(_T("W %d x H %d"), m_image[m_iImageIndex].GetWidth(),m_image[m_iImageIndex].GetHeight());

		CView::OnInitialUpdate();
		pFrame->m_sStatusSize.Format(_T("%s"),sImageSize);
		pFrame->SendMessage(WM_COMMAND, ID_DISP_STATUS_SIZE);

		switch(m_image[m_iImageIndex].GetImageType())
		{
		case IMAGE_TYPE_CIMAGE:{pFrame->m_sStatusBPP.Format(_T("%d BPP"),m_image[m_iImageIndex].GetCImage()->GetBPP());break;}
		case IMAGE_TYPE_IIMAGE:{pFrame->m_sStatusBPP.Format(_T("int"));break;}
		case IMAGE_TYPE_DIMAGE:{pFrame->m_sStatusBPP.Format(_T("double"));break;}
		default:{return;}
		}
		pFrame->SendMessage(WM_COMMAND, ID_DISP_STATUS_BPP);
		if(m_iImageMax==1){		m_saFilePaths.RemoveAll();ListUpSameDirImages(m_sFilePath);}
	}
	bool CSImageViewerView::AddReadImage(CString sFilePath)
	{
		CStringArray saFilePath;
		bool bRet = RecursivelyGetImageFilePaths(sFilePath, -1, &saFilePath, &m_fileFomatList);
		if(bRet != true){return false;}
		int iImageNum = CountImages(sFilePath, -1, &m_fileFomatList);

		int iOldNum = m_iImageMax;
		PanImage* imgTemp = NULL;
		imgTemp = new PanImage[iOldNum];
		for(int i = 0; i<iOldNum; i++)
		{
			imgTemp[i].CopyImage(&m_image[i]);
		}

		for(int i = 0; i<m_iImageMax; i++)
		{
			m_image[i].Init();
		}
		SAFE_DELETE(m_image);
		int iNewImageNum = iOldNum + iImageNum;
		m_image = new PanImage[iOldNum + iImageNum];
		for(int i = 0; i<iOldNum; i++)
		{
			m_image[i].CopyImage(&imgTemp[i]);
		}
		SAFE_DELETE(imgTemp);

		int iImageIndex = iOldNum;
		for(int i = 0; i<iImageNum; i++)
		{
			bRet = ReadAndAppendImage(saFilePath.GetAt(i), &m_fileFomatList, &m_image[iImageIndex], iImageIndex, &iImageIndex);
			if(bRet != true){return false;}
		}

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		m_iImageIndex = iOldNum;
		m_iImageMax = iNewImageNum;

		if(m_iImageMax>=2){pFrame->m_bMultiFile=true; m_bSynchroScroll=true;;}
		else{pFrame->m_bMultiFile=false; m_bSynchroScroll=false;}

		pFrame->m_bFileOpened = true;
		view.m_bRegionSelected = true;

		ResetImage(true, false);
		SetCaption();
		return true;

	}

	void CSImageViewerView::ListUpSameDirImages(const CString sBaseFilePath)
	{
		if(m_iImageMax <= 0){return;}

		CString sFileDir;
		bool bRet = GetDirectory(sBaseFilePath, &sFileDir);
		CStringArray saFilePathsTemp;
		bRet = RecursivelyGetImageFilePaths(sFileDir,1,&saFilePathsTemp, &m_fileFomatList);
		if((bRet != true) && saFilePathsTemp.GetCount()<0){return;}
		SortStrings(&saFilePathsTemp, &m_saFilePaths);
		m_iTempIndex = 0;
		for(int i=0; i<m_saFilePaths.GetCount(); i++)
		{
			if(m_saFilePaths.GetAt(i).CompareNoCase(m_image[m_iImageIndex].GetDataSource())==0){m_iTempIndex=i; return;}
		}
	}

	bool CSImageViewerView::ReadImage(CString sFilePath)
	{
		for(int i = 0; i<m_iImageMax; i++)
		{
			m_image[i].Init();
		}
		SAFE_DELETE(m_image);
		m_sFilePath.Format(_T("%s"), sFilePath);
		CStringArray saFilePathsTemp;
		bool bRet = RecursivelyGetImageFilePaths(sFilePath, -1, &saFilePathsTemp, &m_fileFomatList);
		if((bRet != true) && saFilePathsTemp.GetCount()<0){return false;}
		SortStrings(&saFilePathsTemp, &m_saFilePaths);
		int iImageNum = CountImages(sFilePath, -1, &m_fileFomatList);

		m_image = new PanImage[iImageNum];

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

		int iImageIndex = 0;
		for(int i = 0; i<saFilePathsTemp.GetCount(); i++)
		{
			bRet = ReadAndAppendImage(saFilePathsTemp.GetAt(i), &m_fileFomatList, &m_image[iImageIndex], iImageIndex, &iImageIndex);
			//			pFrame->SetProgressBar(int(i/iImageNum*1.0));
			//			Invalidate();
			if(bRet != true){
				//pFrame->SetProgressBar(0);
				return false;}
		}

		//			pFrame->SetProgressBar(0);

		m_iImageIndex = 0;
		m_iImageMax = iImageNum;
		pFrame->m_bFileOpened = true;
		view.m_bRegionSelected = true;
		if(m_iImageMax>=2){pFrame->m_bMultiFile=true; m_bSynchroScroll=true;;}
		else{pFrame->m_bMultiFile=false; m_bSynchroScroll=false;}

		ResetImage(true, true);
		SetCaption();

		return true;
	}

	bool CSImageViewerView::SaveImage(const CImage* image)
	{

		CFormatSelectionDlg formatDlg;
		formatDlg.m_iBPP = image->GetBPP();
		formatDlg.m_bImageIsMonochrome = _IsImageMonochrome(image);
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
		if(cf.DoModal() != IDOK){ return false;}
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
		bool bbRet = MakeReservedChannelZero(image,&imgWrite);
		if(bbRet != true){return false;}

		HRESULT hResult = imgWrite.Save(sFilePath);
		if(hResult != S_OK){return false;}

		return true;
	}

	void CSImageViewerView::OnFileSave()
	{
		if(m_iImageMax <= 0){return;}
		SaveImage(m_image[m_iImageIndex].GetCurrentProcess());
	}

	void CSImageViewerView::OnFileAdd()
	{
		CString sFilePaths;
		bool bRet = GetOpenFileList(&sFilePaths);
		if(bRet != true){return;}

		AddReadImage(sFilePaths);
	}

	void CSImageViewerView::OnFileOpen()
	{
		CString sFilePaths;
		bool bRet = GetOpenFileList(&sFilePaths);
		if(bRet != true){return;}

		ReadImage(sFilePaths);
	}

	void CSImageViewerView::OnEditCopy()
	{
		if(m_iImageMax <= 0){return;}
		CRect rect_i=view.GetRect_i();
		if(rect_i.IsRectNull() == TRUE){return;}

		CImage imgClipped;
		ClipImage(m_image[m_iImageIndex].GetCurrentProcess(),&imgClipped, rect_i.top,rect_i.left, rect_i.bottom, rect_i.right); 
		CopyToClipBoardImg(&imgClipped);
		return;
	}

	void CSImageViewerView::OnPasteAs()
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

		PanImage imgTemp;
		bool bRet = CopyFromClipBoardImg(&imgTemp);
		if(bRet != TRUE){return;}


		SAFE_DELETE(m_image);
		m_iImageIndex = 0;
		m_iImageMax = 1;
		m_image = new PanImage[m_iImageMax];

		CPasteAsDlg pasteAsdlg;

		switch(imgTemp.GetImageType())
		{
		case IMAGE_TYPE_IIMAGE:{pasteAsdlg.m_sEditImageType.Format(_T("Value"));break;}
		case IMAGE_TYPE_DIMAGE:{pasteAsdlg.m_sEditImageType.Format(_T("Value"));break;}
		case IMAGE_TYPE_CIMAGE:{pasteAsdlg.m_sEditImageType.Format(_T("Image"));break;}
		default:{}
		}

		INT_PTR iRet = pasteAsdlg.DoModal();
		if(iRet != IDOK){return;}

		m_image[m_iImageIndex].Set(imgTemp.GetImageType(), imgTemp.GetIImage(), imgTemp.GetDImage(), imgTemp.GetWidth(), imgTemp.GetHeight(), imgTemp.GetCImage(), pasteAsdlg.m_enumPasteAs, _T("Clipboard"));
		if(bRet != TRUE){return;}
		m_sFilePath.Format(_T("Clipboard"));

		pFrame->m_bFileOpened = true;
		view.m_bRegionSelected = true;



		ResetImage(true, true);
		SetCaption();
	}
	void CSImageViewerView::SetCaption()
	{
		CString sCaption;
		if(m_iImageMax==1)
		{
			sCaption.Format(_T("< %d / %d >%s - SImageViewer"), m_iTempIndex+1, m_saFilePaths.GetCount(), m_image[m_iImageIndex].GetDataSource());
		}
		else if(m_iImageMax>1)
		{
			sCaption.Format(_T("( %d / %d ) %s - SImageViewer"), m_iImageIndex+1, m_iImageMax, m_image[m_iImageIndex].GetDataSource());
		}
		else
		{
			sCaption.Format(_T("UnLoaded - SImageViewer"));
		}
		AfxGetMainWnd()->SetWindowText(sCaption);
	}

	void CSImageViewerView::OnEditPaste()
	{
		PanImage imgTemp;
		bool bRet = CopyFromClipBoardImg(&imgTemp);
		if(bRet != true){return;}

		SAFE_DELETE(m_image);
		m_iImageIndex = 0;
		m_iImageMax = 1;
		m_image = new PanImage[m_iImageMax];
		m_image[m_iImageIndex].CopyImage(&imgTemp);
		m_sFilePath.Format(_T("Clipboard"));

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->m_bFileOpened = true;
		view.m_bRegionSelected = true;
		ResetImage(true, true);
		SetCaption();
	}

	void CSImageViewerView::FullDomain(CRect* rect_i)
	{
		if(m_iImageMax <= 0){return;}
		rect_i->SetRect(0, 0, m_image[m_iImageIndex].GetCurrentProcess()->GetWidth()-1,m_image[m_iImageIndex].GetCurrentProcess()->GetHeight()-1);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		view.m_bRegionSelected = true;
	}
	void CSImageViewerView::OnTransparent()
	{
		if(m_iImageMax <= 0){return;}
		if(_IsImageMonochrome(m_image[m_iImageIndex].GetCurrentProcess())==false){AfxMessageBox(_T("This image is not monochrome.")); return;}
		bool bAutoFull = false;
		CRect rect_i=view.GetRect_i();
		if(rect_i.IsRectNull() == TRUE){bAutoFull = true;FullDomain(&rect_i);}

		CSetTransparentDlg dlg;

		CImage imgClipped;
		ClipImage(m_image[m_iImageIndex].GetCurrentProcess(), &imgClipped, rect_i.top,rect_i.left, rect_i.bottom, rect_i.right); 
		CopyImage_CImage(&imgClipped, &dlg.m_image);
		INT_PTR iRet = dlg.DoModal();
	}

	void Intensity(ImgRGB* imgIn, double* dMean)
	{
		ULONGLONG ullSum = 0;
		int iHeight = imgIn->iHeight;
		int iWidth = imgIn->iWidth;
		if(imgIn->iChannel == CHANNEL_1_8)
		{
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					ullSum+=imgIn->byImg[r*iWidth+c];
				}
			}
		}
		if(imgIn->iChannel == CHANNEL_3_8RGB)
		{
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					ullSum+=imgIn->byImgR[r*iWidth+c];
				}
			}
		}

		*dMean=ullSum/(iWidth*iHeight*1.0);
	}

	void CSImageViewerView::OnCopyFileListToClipboard()
	{
		CString sData;
		for(int i=0; i<m_iImageMax; i++)
		{
			CString sTemp;
			CString sDataPath;
			sDataPath.Format(_T("%s"),  m_image[i].GetDataSource());
			int iPlace = sDataPath.ReverseFind('\\');
			if(iPlace >=0)
			{
				sTemp.Format(_T("%d\t%s\t%s\n"), i+1, sDataPath, sDataPath.Mid(iPlace+1));
			}
			else
			{
				sTemp.Format(_T("%d\t%s\n"), i+1, sDataPath);
			}
			sData+=sTemp;
		}

		sData.Delete(sData.GetLength()-1);
		CopyToClipBoardStr(sData);
		AfxMessageBox(_T("ファイルリストをクリップボードにコピーしました"));
	}

	int CheckAllImagesAreSameFormat(const PanImage* image, const int iImageMax, int* iWidth, int* iHeight, bool* bMono)
	{
		if(iImageMax < 2){return -1;}
		bool bMono_l = _IsImageMonochrome(image[0].GetCurrentProcess());
		int iWidth_l = image[0].GetCurrentProcess()->GetWidth();
		int iHeight_l = image[0].GetCurrentProcess()->GetHeight();

		for(int i=1; i<iImageMax; i++)
		{
			if(bMono_l==true){bMono_l = _IsImageMonochrome(image[i].GetCurrentProcess());}
			int iWidth_target = image[i].GetCurrentProcess()->GetWidth();
			if(iWidth_l != iWidth_target){return -2;}
			int iHeight_target = image[i].GetCurrentProcess()->GetHeight();
			if(iHeight_l != iHeight_target){return -2;}
		}
		*iWidth = iWidth_l;
		*iHeight = iHeight_l;
		*bMono = bMono_l;
		return 0;
	}

	void CSImageViewerView::OnCopyVarianceToClipboard()
	{
		bool bMono;
		int iWidth;
		int iHeight;
		int iRet = CheckAllImagesAreSameFormat(m_image, m_iImageMax, &iWidth, &iHeight, &bMono);
		if(iRet==-1){AfxMessageBox(_T("複数画像が読み込まれていません")); return;}
		if(iRet==-2){AfxMessageBox(_T("画像の大きさがそろっていません")); return;}
		if(iRet<0){return;}

		CInputDlg dlg;
		dlg.m_sEditInput.Format(_T("1"));
		dlg.DoModal();

		int iSize = _ttoi(dlg.m_sEditInput);
		if(iSize<1){ return;}
		int iSizeHalf = (iSize-1)/2;
		const int iImageNum=m_iImageMax;
		ULONGLONG* ullSumR;
		ULONGLONG* ullSumG;
		ULONGLONG* ullSumB;
		ullSumR = new ULONGLONG[iWidth*iHeight];
		ullSumG = new ULONGLONG[iWidth*iHeight];
		ullSumB = new ULONGLONG[iWidth*iHeight];

		for(int r=0; r<iHeight; r++)
		{
			for(int c=0; c<iWidth; c++)
			{
				ullSumR[r*iWidth+c]=0;
				ullSumG[r*iWidth+c]=0;
				ullSumB[r*iWidth+c]=0;
			}
		}

		if(bMono==true)
		{
			for(int i=0; i<iImageNum; i++)
			{
				ImgRGB imgRGB;
				m_image[i].ConvertImage(&imgRGB);
				for(int r=0; r<iHeight; r++)
				{
					for(int c=0; c<iWidth; c++)
					{
						ullSumR[r*iWidth+c]+=imgRGB.byImgR[r*iWidth+c];
					}
				}
			}
		}
		else
		{
			for(int i=0; i<iImageNum; i++)
			{
				ImgRGB imgRGB;
				m_image[i].ConvertImage(&imgRGB);
				for(int r=0; r<iHeight; r++)
				{
					for(int c=0; c<iWidth; c++)
					{
						ullSumR[r*iWidth+c]+=imgRGB.byImgR[r*iWidth+c];
						ullSumG[r*iWidth+c]+=imgRGB.byImgG[r*iWidth+c];
						ullSumB[r*iWidth+c]+=imgRGB.byImgB[r*iWidth+c];
					}
				}
			}
		}


		ULONGLONG* ullTotalR;
		ULONGLONG* ullTotalG;
		ULONGLONG* ullTotalB;
		ULONGLONG* ullArea;

		ullTotalR = new ULONGLONG[iWidth*iHeight];
		ullTotalG = new ULONGLONG[iWidth*iHeight];
		ullTotalB = new ULONGLONG[iWidth*iHeight];
		ullArea = new ULONGLONG[iWidth*iHeight];

		for(int r=0; r<iHeight; r++)
		{
			for(int c=0; c<iWidth; c++)
			{
				ullTotalR[r*iWidth+c] =0;
				ullTotalG[r*iWidth+c] =0;
				ullTotalB[r*iWidth+c] =0;
				ullArea[r*iWidth+c] =0;
			}
		}

		if(bMono==true)
		{
			for(int i=0; i<iImageNum; i++)
			{
				ImgRGB imgRGB;
				m_image[i].ConvertImage(&imgRGB);


				if(iSizeHalf==0)
				{
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							ullTotalR[r*iWidth+c] += (imgRGB.byImgR[r*iWidth+c]*iImageNum -  ullSumR[r*iWidth+c])*(imgRGB.byImgR[r*iWidth+c]*iImageNum -  ullSumR[r*iWidth+c]);

						}
					}
				}
				else
				{
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							for(int dr=((r<iSizeHalf) ? -r : -iSizeHalf); dr<=((r+iSizeHalf>iHeight-1) ? iHeight-1 - r : iSizeHalf); dr++)
							{
								for(int dc=((c<iSizeHalf) ? -c : -iSizeHalf); dc<=((c+iSizeHalf>iWidth-1) ? iWidth-1 - c : iSizeHalf); dc++)
								{
									ullTotalR[r*iWidth+c] += (imgRGB.byImgR[(r+dr)*iWidth+(c+dc)]*iImageNum -  ullSumR[r*iWidth+c])*(imgRGB.byImgR[(r+dr)*iWidth+(c+dc)]*iImageNum -  ullSumR[r*iWidth+c]);
									ullArea[r*iWidth+c]++;
								}
							}
						}
					}
				}
			}
		}
		else
		{
			for(int i=0; i<iImageNum; i++)
			{
				ImgRGB imgRGB;
				m_image[i].ConvertImage(&imgRGB);
				if(iSizeHalf==0)
				{
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							ullTotalR[r*iWidth+c] += (imgRGB.byImgR[r*iWidth+c]*iImageNum -  ullSumR[r*iWidth+c])*(imgRGB.byImgR[r*iWidth+c]*iImageNum -  ullSumR[r*iWidth+c]);
							ullTotalG[r*iWidth+c] += (imgRGB.byImgG[r*iWidth+c]*iImageNum -  ullSumG[r*iWidth+c])*(imgRGB.byImgG[r*iWidth+c]*iImageNum -  ullSumG[r*iWidth+c]);
							ullTotalB[r*iWidth+c] += (imgRGB.byImgB[r*iWidth+c]*iImageNum -  ullSumB[r*iWidth+c])*(imgRGB.byImgB[r*iWidth+c]*iImageNum -  ullSumB[r*iWidth+c]);
						}
					}
				}
				else
				{
					for(int r=0; r<iHeight; r++)
					{
						for(int c=0; c<iWidth; c++)
						{
							for(int dr=((r<iSizeHalf) ? -r : -iSizeHalf); dr<=((r+iSizeHalf>iHeight-1) ? iHeight-1 - r : iSizeHalf); dr++)
							{
								for(int dc=((c<iSizeHalf) ? -c : -iSizeHalf); dc<=((c+iSizeHalf>iWidth-1) ? iWidth-1 - c : iSizeHalf); dc++)
								{
									ullTotalR[r*iWidth+c] += (imgRGB.byImgR[(r+dr)*iWidth+(c+dc)]*iImageNum -  ullSumR[r*iWidth+c])*(imgRGB.byImgR[(r+dr)*iWidth+(c+dc)]*iImageNum -  ullSumR[r*iWidth+c]);
									ullTotalG[r*iWidth+c] += (imgRGB.byImgG[(r+dr)*iWidth+(c+dc)]*iImageNum -  ullSumG[r*iWidth+c])*(imgRGB.byImgG[(r+dr)*iWidth+(c+dc)]*iImageNum -  ullSumG[r*iWidth+c]);
									ullTotalB[r*iWidth+c] += (imgRGB.byImgB[(r+dr)*iWidth+(c+dc)]*iImageNum -  ullSumB[r*iWidth+c])*(imgRGB.byImgB[(r+dr)*iWidth+(c+dc)]*iImageNum -  ullSumB[r*iWidth+c]);
									ullArea[r*iWidth+c]++;
								}
							}
						}
					}
				}
			}
		}
		SAFE_DELETE(ullSumR);
		SAFE_DELETE(ullSumG);
		SAFE_DELETE(ullSumB);

		CString sVariance;
		for(int r=0; r<iHeight; r++)
		{
			if(bMono==true)
			{
				if(iSizeHalf==0)
				{
					double dNum3=iImageNum*iImageNum*iImageNum*1.0;
					for(int c=0; c<iWidth; c++)
					{
						CString sTemp;
						sTemp.Format(_T("%e%s"), ullTotalR[r*iWidth+c]/(dNum3),(c != (iWidth-1)? _T("\t"): _T("\n")));
						sVariance+=sTemp;
					}
				}
				else
				{
					for(int c=0; c<iWidth; c++)
					{
						CString sTemp;
						double dNum3=ullArea[r*iWidth+c]*ullArea[r*iWidth+c]*ullArea[r*iWidth+c]*1.0;
						sTemp.Format(_T("%e%s"), ullTotalR[r*iWidth+c]/(dNum3),(c != (iWidth-1)? _T("\t"): _T("\n")));
						sVariance+=sTemp;
					}
				}
			}
			else
			{
				if(iSizeHalf==0)
				{
					double dNum3=iImageNum*iImageNum*iImageNum*1.0;
					for(int c=0; c<iWidth; c++)
					{
						CString sTemp;
						sTemp.Format(_T("%e\t"), ullTotalR[r*iWidth+c]/(dNum3));
						sVariance+=sTemp;
					}
					sVariance+=_T("\t");
					for(int c=0; c<iWidth; c++)
					{
						CString sTemp;
						sTemp.Format(_T("%e\t"), ullTotalG[r*iWidth+c]/(dNum3));
						sVariance+=sTemp;
					}
					sVariance+=_T("\t");
					for(int c=0; c<iWidth; c++)
					{
						CString sTemp;
						sTemp.Format(_T("%e%s"), ullTotalB[r*iWidth+c]/(dNum3),(c != (iWidth-1)? _T("\t"): _T("\n")));
						sVariance+=sTemp;
					}
				}
				else
				{
					for(int c=0; c<iWidth; c++)
					{
						CString sTemp;
						double dNum3=ullArea[r*iWidth+c]*ullArea[r*iWidth+c]*ullArea[r*iWidth+c]*1.0;
						sTemp.Format(_T("%e\t"), ullTotalR[r*iWidth+c]/(dNum3));
						sVariance+=sTemp;
					}
					sVariance+=_T("\t");
					for(int c=0; c<iWidth; c++)
					{
						CString sTemp;
						double dNum3=ullArea[r*iWidth+c]*ullArea[r*iWidth+c]*ullArea[r*iWidth+c]*1.0;
						sTemp.Format(_T("%e\t"), ullTotalG[r*iWidth+c]/(dNum3));
						sVariance+=sTemp;
					}
					sVariance+=_T("\t");
					for(int c=0; c<iWidth; c++)
					{
						CString sTemp;
						double dNum3=ullArea[r*iWidth+c]*ullArea[r*iWidth+c]*ullArea[r*iWidth+c]*1.0;
						sTemp.Format(_T("%e%s"), ullTotalB[r*iWidth+c]/(dNum3),(c != (iWidth-1)? _T("\t"): _T("\n")));
						sVariance+=sTemp;
					}
				}
			}
		}

		
		SAFE_DELETE(ullArea);

		SAFE_DELETE(ullTotalR);
		SAFE_DELETE(ullTotalG);
		SAFE_DELETE(ullTotalB);

		sVariance.Delete(sVariance.GetLength()-1);
		CopyToClipBoardStr(sVariance);
		AfxMessageBox(_T("分散値をクリップボードにコピーしました"));
	}


	void CSImageViewerView::OnCopyAverageToClipboard()
	{
		bool bMono;
		int iWidth;
		int iHeight;
		int iRet = CheckAllImagesAreSameFormat(m_image, m_iImageMax, &iWidth, &iHeight, &bMono);
		if(iRet==-1){AfxMessageBox(_T("複数画像が読み込まれていません")); return;}
		if(iRet==-2){AfxMessageBox(_T("画像の大きさがそろっていません")); return;}
		if(iRet<0){return;}

		ULONGLONG* ullSumR;
		ULONGLONG* ullSumG;
		ULONGLONG* ullSumB;
		ullSumR = new ULONGLONG[iWidth*iHeight];
		ullSumG = new ULONGLONG[iWidth*iHeight];
		ullSumB = new ULONGLONG[iWidth*iHeight];

		if(bMono==true)
		{
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					ullSumR[r*iWidth+c]=0;
				}
			}

			for(int i=0; i<m_iImageMax; i++)
			{
				ImgRGB imgRGB;
				m_image[i].ConvertImage(&imgRGB);
				for(int r=0; r<iHeight; r++)
				{
					for(int c=0; c<iWidth; c++)
					{
						ullSumR[r*iWidth+c]+=imgRGB.byImgR[r*iWidth+c];
					}
				}
			}
		}
		else
		{
			for(int r=0; r<iHeight; r++)
			{
				for(int c=0; c<iWidth; c++)
				{
					ullSumR[r*iWidth+c]=0;
					ullSumG[r*iWidth+c]=0;
					ullSumB[r*iWidth+c]=0;
				}
			}
			for(int i=0; i<m_iImageMax; i++)
			{
				ImgRGB imgRGB;
				m_image[i].ConvertImage(&imgRGB);
				for(int r=0; r<iHeight; r++)
				{
					for(int c=0; c<iWidth; c++)
					{
						ullSumR[r*iWidth+c]+=imgRGB.byImgR[r*iWidth+c];
						ullSumG[r*iWidth+c]+=imgRGB.byImgG[r*iWidth+c];
						ullSumB[r*iWidth+c]+=imgRGB.byImgB[r*iWidth+c];
					}
				}
			}
		}

		CString sAverage;
		for(int r=0; r<iHeight; r++)
		{
			if(bMono==true)
			{
				for(int c=0; c<iWidth; c++)
				{
					CString sTemp;
					sTemp.Format(_T("%e%s"), ullSumR[r*iWidth+c]/(m_iImageMax*1.0),(c != (iWidth-1)? _T("\t"): _T("\n")));
					sAverage+=sTemp;
				}
			}
			else
			{
				for(int c=0; c<iWidth; c++)
				{
					CString sTemp;
					sTemp.Format(_T("%e\t"), ullSumR[r*iWidth+c]/(m_iImageMax*1.0));
					sAverage+=sTemp;
				}
				sAverage+=_T("\t");
				for(int c=0; c<iWidth; c++)
				{
					CString sTemp;
					sTemp.Format(_T("%e\t"), ullSumG[r*iWidth+c]/(m_iImageMax*1.0));
					sAverage+=sTemp;
				}
				sAverage+=_T("\t");
				for(int c=0; c<iWidth; c++)
				{
					CString sTemp;
					sTemp.Format(_T("%e%s"), ullSumB[r*iWidth+c]/(m_iImageMax*1.0),(c != (iWidth-1)? _T("\t"): _T("\n")));
					sAverage+=sTemp;
				}
			}
		}
		SAFE_DELETE(ullSumR);
		SAFE_DELETE(ullSumG);
		SAFE_DELETE(ullSumB);

		sAverage.Delete(sAverage.GetLength()-1);
		CopyToClipBoardStr(sAverage);
		AfxMessageBox(_T("平均値をクリップボードにコピーしました"));
	}


	void CSImageViewerView::OnCopyCorrelMapToClipboard()
	{
		bool bMono;
		int iWidth;
		int iHeight;
		int iRet = CheckAllImagesAreSameFormat(m_image, m_iImageMax, &iWidth, &iHeight, &bMono);
		if(iRet==-1){AfxMessageBox(_T("複数画像が読み込まれていません")); return;}
		if(iRet==-2){AfxMessageBox(_T("画像の大きさがそろっていません"));return;}

		if(bMono==false){AfxMessageBox(_T("モノクロではない画像が混ざっています"));return;}

		CString sCor;
		for(int i=0; i<m_iImageMax; i++)
		{
			for(int j=0; j<m_iImageMax; j++)
			{
				ImgRGB imgI;
				ImgRGB imgJ;
				ImgRGB imgResult1;
				ImgRGB imgResult2;
				m_image[i].ConvertImage(&imgI);
				m_image[j].ConvertImage(&imgJ);

				SubImage(&imgI, &imgJ, &imgResult1, 1, 0);
				SubImage(&imgJ, &imgI, &imgResult2, 1, 0);

				ImgRGB imgResult3;
				AddImage(&imgResult1, &imgResult2, &imgResult3);

				CString sTemp;
				double dMean;
				Intensity(&imgResult3, &dMean);
				sTemp.Format(_T("%e%s"), dMean,(j!=(m_iImageMax-1)? _T("\t"): _T("\n")));
				sCor+=sTemp;
			}
		}

		sCor.Delete(sCor.GetLength()-1);
		CopyToClipBoardStr(sCor);
		AfxMessageBox(_T("相関マップをクリップボードにコピーしました"));
	}

	void CSImageViewerView::OnCopyHistGramToClipboard()
	{
		if(m_iImageMax <= 0){return;}
		CString sHist;
		for(int i=0; i<m_iImageMax; i++)
		{
			ImgRGB imgRGB;
			m_image[i].ConvertImage(&imgRGB);
			bool bMono = _IsImageMonochrome(m_image[i].GetCImage());

			int iHistR[256];
			int iHistG[256];
			int iHistB[256];
			for(int i=0; i<256; i++)
			{
				iHistR[i]=0;
				iHistG[i]=0;
				iHistB[i]=0;
			}
			GetHistgram(&imgRGB, 0, 0, imgRGB.iHeight-1, imgRGB.iWidth-1,  iHistR,  iHistG,  iHistB);
			for(int iValue=0; iValue<256; iValue++)
			{
				CString sTemp;
				sTemp.Format(_T("%d%s"), iHistR[iValue],(iValue!=255? _T("\t"): _T("\n")));
				sHist+=sTemp;
			}
			if(bMono==true){continue;}

			for(int iValue=0; iValue<256; iValue++)
			{
				CString sTemp;
				sTemp.Format(_T("%d%s"), iHistG[iValue],(iValue!=255? _T("\t"): _T("\n")));
				sHist+=sTemp;
			}
			for(int iValue=0; iValue<256; iValue++)
			{
				CString sTemp;
				sTemp.Format(_T("%d%s"), iHistB[iValue],(iValue!=255? _T("\t"): _T("\n")));
				sHist+=sTemp;
			}
		}

		sHist.Delete(sHist.GetLength()-1);
		CopyToClipBoardStr(sHist);
		AfxMessageBox(_T("ヒストグラムをクリップボードにコピーしました"));
	}


	void CSImageViewerView::OnInvert()
	{
		if(m_iImageMax <= 0){return;}
		bool bAutoFull = false;
		CRect rect_i=view.GetRect_i();
		if(rect_i.IsRectNull() == TRUE){bAutoFull = true; FullDomain(&rect_i);}

		ImgRGB imgRGB;
		_ConvertImage(m_image[m_iImageIndex].GetCurrentProcess(), &imgRGB);
		InvertImage(&imgRGB,&imgRGB,rect_i.top,rect_i.left,rect_i.bottom,rect_i.right); 

		if(bAutoFull == true)
		{
			view.SetRect_v(NULL);
			view.SetRect_i(NULL);
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			view.m_bRegionSelected = false;
		}

		ConvertImage(&imgRGB,m_image[m_iImageIndex].ProgressImageProcess());

		Invalidate();
	}

	void CSImageViewerView::OnEquHistImage()
	{
		if(m_iImageMax <= 0){return;}
		bool bAutoFull = false;
		CRect rect_i=view.GetRect_i();
		if(rect_i.IsRectNull() == TRUE){bAutoFull = true; FullDomain(&rect_i);}

		ImgRGB imgRGB;
		ImgRGB imgMeaned;
		_ConvertImage(m_image[m_iImageIndex].GetCurrentProcess(), &imgRGB);
		EquHistImage(&imgRGB,&imgMeaned,rect_i.top,rect_i.left,rect_i.bottom,rect_i.right);
		if(bAutoFull == true)
		{
			view.SetRect_v(NULL);
			view.SetRect_i(NULL);
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			view.m_bRegionSelected = false;
		}

		ConvertImage(&imgMeaned,m_image[m_iImageIndex].ProgressImageProcess());
		Invalidate();
	}

	void CSImageViewerView::SetToolFormat()
	{
		CFileFormatDlg dlg;
		dlg.m_fileFormatList.Copy(&m_fileFomatList);
		dlg.m_sIniFilePath.Format(_T("%s"), m_sIniFilePath);;

		INT_PTR iRet = dlg.DoModal();
		if(iRet != IDOK){return;}
		m_fileFomatList.Copy(&dlg.m_fileFormatList);
	}

	void CSImageViewerView::OnResample()
	{
		if(m_iImageMax <= 0){return;}
		bool bAutoFull = false;
		CRect rect_i=view.GetRect_i();
		if(rect_i.IsRectNull() == TRUE){bAutoFull = true; FullDomain(&rect_i);}

		CResampleDlg dlg;
		dlg.m_iHeightOrg = m_image[m_iImageIndex].GetCurrentProcess()->GetHeight();
		dlg.m_iWidthOrg = m_image[m_iImageIndex].GetCurrentProcess()->GetWidth();
		INT_PTR iRet = dlg.DoModal();
		if(iRet != IDOK)
		{
			if(bAutoFull == true)
			{
				view.SetRect_v(NULL);
				view.SetRect_i(NULL);
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				view.m_bRegionSelected = false;
			}
			return;
		}

		CImage imgSrc;
		CopyImage_CImage(m_image[m_iImageIndex].GetCurrentProcess(), &imgSrc);
		if((dlg.m_resample == RESIZE_NEAREST) || (dlg.m_resample == RESIZE_BILINEAR))
		{
			Resize(&imgSrc, 0, 0, imgSrc.GetHeight()-1, imgSrc.GetWidth()-1, m_image[m_iImageIndex].ProgressImageProcess(), dlg.m_iWidth,dlg.m_iHeight,dlg.m_resample);
		}
		else
		{
			Resample(&imgSrc, m_image[m_iImageIndex].ProgressImageProcess(), dlg.m_resample);
		}
		if(bAutoFull == true)
		{
			view.SetRect_v(NULL);
			view.SetRect_i(NULL);
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			view.m_bRegionSelected = false;
		}
		Invalidate();
	}
	void CSImageViewerView::OnChangeColorDepth()
	{
		if(m_iImageMax <= 0){return;}
		CChangeColorDepthDlg dlg;

		int iUsedColors;
		bool bGrayScale;

		CountColorNum(m_image[m_iImageIndex].GetCurrentProcess(), &iUsedColors, NULL);

		MakeColorTable(m_image[m_iImageIndex].GetCurrentProcess(), NULL,NULL, 1<<min(24, m_image[m_iImageIndex].GetCurrentProcess()->GetBPP()), &iUsedColors, &bGrayScale);

		dlg.m_iColors = iUsedColors;
		dlg.m_bGrayScale = bGrayScale;
		dlg.m_iBPP = m_image[m_iImageIndex].GetCurrentProcess()->GetBPP();

		INT_PTR iRet = dlg.DoModal();
		if(iRet != IDOK){return;}

		bool bSame = false;
		int iIndexOf0 = m_image[0].GetProcessIndex();
		for(int i=1; i<m_iImageMax; i++)
		{
			if(iIndexOf0 != m_image[i].GetProcessIndex()){bSame = true;}else{bSame = false; break;}
		}

		bool bApplyToAll = false;
		if(bSame == true){bApplyToAll = ((IDYES == AfxMessageBox(_T("全ての画像に適用しますか？"), MB_YESNO)) ? true: false);}


		int iStart = ((bApplyToAll == true) ?		0	: m_iImageIndex);
		int iEnd = ((bApplyToAll == true) ?	m_iImageMax : m_iImageIndex+1);

		for(int i =iStart; i<iEnd; i++)
		{
			CImage imgSrc;
			CopyImage_CImage(m_image[i].GetCurrentProcess(), &imgSrc);

			switch(dlg.m_iMode)
			{
			case 0:{ConvertImage_LossLess(&imgSrc, dlg.m_iBPP, m_image[i].ProgressImageProcess());break;}
			case 1:{ConvertImage_AreaCoverage(&imgSrc,dlg.m_iBPP, m_image[i].ProgressImageProcess());break;}
			case 2:{ConvertImage_ByDeviation(&imgSrc, dlg.m_iBPP, m_image[i].ProgressImageProcess());break;}
			}
		}

		//		m_image[m_iImageIndex].m_imageProcessed[(m_image[m_iImageIndex].m_iImgProcessIndex % MAX_IMG_PROCESS)].Save(_T("D:\\test.bmp"));
		Invalidate();
	}

	void CSImageViewerView::OnConvertColorSpace()
	{
		if(m_iImageMax <= 0){return;}
		ENUM_COLOR color;
		CExtractChannelDlg extractDlg;

		INT_PTR iRet = extractDlg.DoModal();
		if(iRet != IDOK){return;}

		color = extractDlg.m_enumColor;

		CImage imgSrc;
		CopyImage_CImage(m_image[m_iImageIndex].GetCurrentProcess(), &imgSrc);

		ExtractChannel(&imgSrc,m_image[m_iImageIndex].ProgressImageProcess(), color);
		Invalidate();
		return;
	}
	void CSImageViewerView::OperateRotaateImage(enumRotate rotate)
	{
		if(m_iImageMax <= 0){return;}
		bool bAutoFull = false;

		ImgRGB imgRGB;
		_ConvertImage(m_image[m_iImageIndex].GetCurrentProcess(), &imgRGB);
		ImgRGB imgResult;
		RotateImage(&imgRGB, &imgResult, rotate);
		ConvertImage(&imgResult, m_image[m_iImageIndex].ProgressImageProcess());
		SetScroll();
		Invalidate();
	}

	void CSImageViewerView::OnColorize()
	{
		if(m_iImageMax <= 0){return;}
		if(_IsImageMonochrome(m_image[m_iImageIndex].GetCurrentProcess())==false){AfxMessageBox(_T("This image is not monochrome.")); return;}
		bool bAutoFull = false;
		CRect rect_i=view.GetRect_i();
		if(rect_i.IsRectNull() == TRUE){bAutoFull = true;FullDomain(&rect_i);}

		CColorizeDlg dlg;

		CImage imgClipped;
		ClipImage(m_image[m_iImageIndex].GetCurrentProcess(), &imgClipped, rect_i.top,rect_i.left, rect_i.bottom, rect_i.right); 
		CopyImage_CImage(&imgClipped, &dlg.m_image);
		INT_PTR iRet = dlg.DoModal();
		if(iRet != IDOK)
		{
			if(bAutoFull == true)
			{
				view.SetRect_v(NULL);
				view.SetRect_i(NULL);
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				view.m_bRegionSelected = false;
			}
			return;
		}


		CImage imgTemp;

		ClipImage(m_image[m_iImageIndex].GetCurrentProcess(), &imgClipped, rect_i.top,rect_i.left, rect_i.bottom, rect_i.right); 
		switch(dlg.m_colorize)
		{
		case Colorize_threshold:{ Threshold(&imgClipped, &imgTemp, dlg.m_byMin, dlg.m_byMax, dlg.m_bConnection, dlg.m_iNeighbor); break;}
		case Colorize_Demosaic:{ Demosaic(&imgClipped, dlg.m_i00, dlg.m_i01, dlg.m_i10, dlg.m_i11, &imgTemp); break;}
		case Colorize_rainbow:{ GrayToRainbow(&imgClipped, &imgTemp); break;}
		}

		CImage imgResult2;
		bool bRet = ImposeImage(m_image[m_iImageIndex].GetCurrentProcess(), &(dlg.m_imageColorized), rect_i.top, rect_i.left,&imgResult2);
		CopyImage_CImage(&imgResult2, m_image[m_iImageIndex].ProgressImageProcess());



		if(bAutoFull == true)
		{
			view.SetRect_v(NULL);
			view.SetRect_i(NULL);
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			view.m_bRegionSelected = false;
		}
		Invalidate();

	}
	void CSImageViewerView::OnBrightnessContrastGamma()
	{
		if(m_iImageMax <= 0){return;}
		bool bAutoFull = false;
		CRect rect_i=view.GetRect_i();
		if(rect_i.IsRectNull() == TRUE){bAutoFull = true;FullDomain(&rect_i);}

		CImageModifyDlg dlgModify;

		CImage imgClipped;
		ClipImage(m_image[m_iImageIndex].GetCurrentProcess(), &imgClipped, rect_i.top,rect_i.left, rect_i.bottom, rect_i.right); 
		CopyImage_CImage(&imgClipped, &dlgModify.m_image);
		INT_PTR iRet;
		dlgModify.DoModal();
		iRet = dlgModify.m_iRet;
		if(iRet == IDCANCEL)
		{
			if(bAutoFull == true)
			{
				view.SetRect_v(NULL);
				view.SetRect_i(NULL);
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				view.m_bRegionSelected = false;
			}
			return ;
		}

		int iBrightness = dlgModify.m_iBrightness;
		int iContrast = dlgModify.m_iContrast;
		double dGamma = dlgModify.m_dGamma;

		ImgRGB imgRGB;
		ImgRGB imgResult1;
		ImgRGB imgResult2;
		_ConvertImage(m_image[m_iImageIndex].GetCurrentProcess(), &imgRGB);
		BrightnessContrast(&imgRGB,&imgResult1,rect_i.top, rect_i.left, rect_i.bottom, rect_i.right,(double)iBrightness,(double)iContrast);
		Gamma(&imgResult1,&imgResult2,rect_i.top, rect_i.left, rect_i.bottom, rect_i.right,dGamma);
		if(bAutoFull == true)
		{
			view.SetRect_v(NULL);
			view.SetRect_i(NULL);
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			view.m_bRegionSelected = false;
		}

		ConvertImage(&imgResult2, m_image[m_iImageIndex].ProgressImageProcess());
		Invalidate();
	}



	void CSImageViewerView::OnSize(UINT nType, int cx, int cy)
	{
		if(m_iImageMax <= 0){return;}
		CView::OnSize(nType, cx, cy);

		if(m_iImageMax <= 0){return;}
		if(m_image[m_iImageIndex].GetCurrentProcess()->IsNull() == true){return;}

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
		m_bRefresh = false;
		m_iImageMax = 0;
		/*
		CImage imgTest;
		imgTest.Create(16,16,8);
		RGBQUAD colorTable[256];
		for(int i = 0; i<256; i++)
		{
		colorTable[i].rgbBlue = i;
		colorTable[i].rgbGreen = i;
		colorTable[i].rgbRed = i;
		colorTable[i].rgbReserved = 255;
		}

		SetColorTable(&imgTest, colorTable, 256);
		int iPitch = imgTest.GetPitch();
		BYTE* byData = (BYTE*)imgTest.GetBits();
		for(int r = 0; r<16; r++)
		{
		for(int c = 0; c<16; c++)
		{
		byData[r*iPitch+c] = r*16+c;
		}
		}

		imgTest.Save(_T("d:\\16_16_256.bmp"));
		*/
		int iHeightIfNoBar_v;
		int iWidthIfNoBar_v;
		view.GetSizeIfNoBar(&iHeightIfNoBar_v, &iWidthIfNoBar_v, this);
		//		m_image[m_iImageIndex].m_imageProcessed[m_image[m_iImageIndex].m_iImgProcessIndex].Create(100,100,0);
		pFrame->AdjustViewClientSize(100, 100,iWidthIfNoBar_v, iHeightIfNoBar_v);
		/*
		int ii[100];
		for(int i = 0; i<100; i++)
		{
		ii[i] = rand();
		}
		int iIndex[100];
		index_i(ii,100,iIndex);

		int i2[100];
		for(int i = 0; i<100; i++)
		{
		i2[i] = ii[iIndex[i]];
		}
		for(int i = 0; i<99; i++)
		{
		if(ii[iIndex[i]]>ii[iIndex[i+1]])
		{
		break;
		}
		}*/


		TCHAR tszExePath[MAX_PATH];
		::GetModuleFileName(NULL, tszExePath, MAX_PATH);
		CString sExePath;
		sExePath.Format(_T("%s"), tszExePath);
		int iPlace = sExePath.ReverseFind('\\');
		m_sIniFilePath.Format(_T("%s\\setting.ini"),sExePath.Left(iPlace)); 
		UINT uiTypeNum;
		bool bRet = GetImageTypeNum(m_sIniFilePath, &uiTypeNum);
		m_fileFomatList.Set(uiTypeNum);
		for(UINT i=0; i<uiTypeNum; i++)
		{
			CString sType;
			bRet = GetImageType(m_sIniFilePath, i, &sType);
			bRet = GetFileFormat(m_sIniFilePath, sType, &(m_fileFomatList.fileFormat[i]));
		}
		ReadSetting(m_sIniFilePath);
		pFrame->m_pView = this;
		SetTimer(TIMER_INIT, 100, 0);
	}

	void CSImageViewerView::ReadSetting(const CString sIniFilePath)
	{
		const UINT uiBufSize=128;
		TCHAR tchData[uiBufSize];
		GetPrivateProfileString(_T("Setting"), _T("ExitByEsc"), _T("1"), tchData, uiBufSize, sIniFilePath);
		m_bExitByEsc = ((_ttoi(tchData) == 0) ? 0 : 1);
		GetPrivateProfileString(_T("Setting"), _T("CenteredWhenFullScreen"), _T("0"), tchData, uiBufSize, sIniFilePath);
		m_bCenteredWhenFullScreen = ((_ttoi(tchData) == 0) ? 0 : 1);
		GetPrivateProfileString(_T("Setting"), _T("PPFWwithoutCtrlWhenFullScreen"), _T("0"), tchData, uiBufSize, sIniFilePath);
		m_bPPFWwithoutCtrlWhenFullScreen = ((_ttoi(tchData) == 0) ? 0 : 1);
	}
	void CSImageViewerView::SaveSetting(const CString sIniFilePath)
	{
		WritePrivateProfileString(_T("Setting"), _T("ExitByEsc"), ((m_bExitByEsc == false) ? _T("0") : _T("1")), sIniFilePath);
		WritePrivateProfileString(_T("Setting"), _T("CenteredWhenFullScreen"), ((m_bCenteredWhenFullScreen == false) ? _T("0") : _T("1")), sIniFilePath);
		WritePrivateProfileString(_T("Setting"), _T("PPFWwithoutCtrlWhenFullScreen"), ((m_bPPFWwithoutCtrlWhenFullScreen == false) ? _T("0") : _T("1")), sIniFilePath);
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

	bool CSImageViewerView::OperateImagePPFWFlexible(const int iStep)
	{
		if(m_iImageMax>=2){return false;}


		int iMax = m_saFilePaths.GetCount();
		if(iMax<=0){return false;}

		int iBefore = m_iTempIndex;
		if(iStep==INT_MAX){m_iTempIndex=iMax-1;}
		else if(iStep==INT_MIN){m_iTempIndex=0;}
		else
		{
			m_iTempIndex+=iStep;
			if(iStep<0){m_iTempIndex = max(m_iTempIndex,0);}
			if(iStep>0){m_iTempIndex = min(m_iTempIndex,iMax-1);}
		}

		if(m_iTempIndex==iBefore){return false;}

		bool bRet = ReadAndAppendSingllImage(m_saFilePaths.GetAt(m_iTempIndex), &m_fileFomatList, &m_image[0], 0);

		m_iImageIndex = 0;
		m_iImageMax= 1;
		ResetImage(true, true);
		CPoint point_v;
		GetCursorPos(&point_v);
		ScreenToClient(&point_v);
		DispStatus(point_v);
		ZoomReset();
		SetCaption();
		return true;
	}
	bool CSImageViewerView::OperateImagePPFW(const int iStep)
	{
		//	return OperateImagePPFWFlexible(iStep);

		if(iStep==INT_MAX){m_iImageIndex=m_iImageMax-1;}
		else if(iStep==INT_MIN){m_iImageIndex=0;}
		else
		{
			m_iImageIndex+=iStep;
			if(iStep<0){m_iImageIndex = max(m_iImageIndex,0);}
			if(iStep>0){m_iImageIndex = min(m_iImageIndex,m_iImageMax-1);}
		}
		ResetImage(false, false);
		CPoint point_v;
		if(m_bSynchroScroll==true)
		{
			view.ZoomChangeAbs(scr.m_iScaleIndex, &(m_image[m_iImageIndex]), this, &point_v);

			view.SetDispOriginR_tv(scr.m_dDispOriginR_tv);
			view.SetDispOriginC_tv(scr.m_dDispOriginC_tv);

			view.OnScroll(SB_VERT, -1, 0, &(m_image[m_iImageIndex]), this);
			view.OnScroll(SB_HORZ, -1, 0, &(m_image[m_iImageIndex]), this);
		}
		else
		{
			view.ZoomChangeAbs(m_image[m_iImageIndex].scr.m_iScaleIndex, &(m_image[m_iImageIndex]),  this, &point_v);

			view.SetDispOriginR_tv(m_image[m_iImageIndex].scr.m_dDispOriginR_tv);
			view.SetDispOriginC_tv(m_image[m_iImageIndex].scr.m_dDispOriginC_tv);

			view.OnScroll(SB_VERT, -1, 0, &(m_image[m_iImageIndex]), this);
			view.OnScroll(SB_HORZ, -1, 0, &(m_image[m_iImageIndex]), this);
		}

		DispStatus(point_v);
		SetCaption();
		Invalidate();
		return true;
	}


	bool CSImageViewerView::ZoomChange(int iR0_i, int iC0_i, int iR1_i, int iC1_i)
	{
		if(m_iImageMax <= 0){return false;}
		CPoint point_v;
		bool bRet = view.ZoomChange(iR0_i, iC0_i, iR1_i, iC1_i,true, &(m_image[m_iImageIndex]),this, &point_v);
		if(bRet != true){return false;}
		DispStatus(point_v);

		if(m_bSynchroScroll==true)
		{
			view.GetScrollSetting(&scr);
		}
		else
		{
			view.GetScrollSetting(&(m_image[m_iImageIndex].scr));
		}
		return true; 
	}

	bool CSImageViewerView::ZoomChange(int iChange)
	{
		if(m_iImageMax <= 0){return false;}
		CPoint point_v;
		bool bRet = view.ZoomChange(iChange, &(m_image[m_iImageIndex]), this, &point_v);
		if(bRet != true){return false;}
		if(m_bSynchroScroll==true)
		{
			view.GetScrollSetting(&scr);
		}
		else
		{
			view.GetScrollSetting(&(m_image[m_iImageIndex].scr));
		}
		return true; 
	}

	bool CSImageViewerView::ZoomChangeAbs(int iChangeAbs)
	{
		if(m_iImageMax <= 0){return false;}
		CPoint point_v;
		bool bRet = view.ZoomChangeAbs(iChangeAbs, &(m_image[m_iImageIndex]), this, &point_v);	
		if(bRet != true){return false;}
		DispStatus(point_v);
		if(m_bSynchroScroll==true)
		{
			view.GetScrollSetting(&scr);
		}
		else
		{
			view.GetScrollSetting(&(m_image[m_iImageIndex].scr));
		}
		return true; 
	}

	bool CSImageViewerView::ZoomChange(int iMousePosR_v, int iMousePosC_v, int iChange)
	{
		if(m_iImageMax <= 0){return false;}
		CPoint point_v;
		bool bRet = view.ZoomChange(iMousePosR_v, iMousePosC_v, iChange, &(m_image[m_iImageIndex]), this, &point_v);
		if(bRet != true){return false;}
		DispStatus(point_v);
		if(m_bSynchroScroll==true)
		{
			view.GetScrollSetting(&scr);
		}
		else
		{
			view.GetScrollSetting(&(m_image[m_iImageIndex].scr));
		}
		return true; 
	}



	void CSImageViewerView::EnterFullScreen()
	{

		CFullScreenDlg dlg(this);
		dlg.m_bCentered=m_bCenteredWhenFullScreen;
		dlg.m_bPPFWwithoutCtrlWhenFullScreen=m_bPPFWwithoutCtrlWhenFullScreen;
		dlg.DoModal();
	}


	bool CSImageViewerView::GetColorAtCursor(PanImage* panImg, CPoint point_v, int* iR_img, int* iC_img, ColorValue* colorValue)
	{
		colorValue->Init();
		CPoint point_tv((int)(point_v.x + GetDispOriginC_tv()), (int)(point_v.y + GetDispOriginR_tv()));

		int iC_img_Local = (int)((point_tv.x) / view.GetScale());
		int iR_img_Local = (int)((point_tv.y) / view.GetScale());


		if (iC_img_Local < 0){return false;}
		if (iR_img_Local < 0){return false;}
		if (iC_img_Local >= panImg->GetWidth()){return false;}
		if (iR_img_Local >= panImg->GetHeight()){return false;}

		*iR_img = iR_img_Local;
		*iC_img = iC_img_Local;

		switch(panImg->GetImageType())
		{
		case IMAGE_TYPE_IIMAGE:
			{
				int iValue;
				panImg->GetValue(iR_img_Local,iC_img_Local,&iValue);
				colorValue->iValue = iValue;
				colorValue->valueType = VALUE_TYPE_INT;
				return true;
			}
		case IMAGE_TYPE_DIMAGE:
			{
				double dValue;
				panImg->GetValue(iR_img_Local,iC_img_Local,&dValue);
				colorValue->dValue = dValue;
				colorValue->valueType = VALUE_TYPE_DOUBLE;
				return true;
			}
		case IMAGE_TYPE_CIMAGE:
			{
				return GetColorAtCursor(panImg->SetCImage(), point_v, iR_img, iC_img, colorValue);
			}
		default:{return false;}
		}
		return false;
	}
	bool CSImageViewerView::GetColorAtCursor(const CImage* img, CPoint point_v, int* iR_img, int* iC_img, ColorValue* colorValue)
	{
		colorValue->Init();
		CPoint point_tv((int)(point_v.x + GetDispOriginC_tv()), (int)(point_v.y + GetDispOriginR_tv()));

		int iC_img_Local = (int)((point_tv.x) / view.GetScale());
		int iR_img_Local = (int)((point_tv.y) / view.GetScale());


		if (iC_img_Local < 0){return false;}
		if (iR_img_Local < 0){return false;}
		if (iC_img_Local >= img->GetWidth()){return false;}
		if (iR_img_Local >= img->GetHeight()){return false;}

		*iR_img = iR_img_Local;
		*iC_img = iC_img_Local;

		if(img->IsNull() == true){return false;}
		if(img->GetBPP() == 32)
		{
			BYTE* pbyData = (BYTE*)img->GetBits();
			int iPitch = img->GetPitch();

			colorValue->byR = pbyData[iR_img_Local * iPitch +iC_img_Local *4+2];
			colorValue->byG = pbyData[iR_img_Local * iPitch +iC_img_Local *4+1];
			colorValue->byB = pbyData[iR_img_Local * iPitch +iC_img_Local *4+0];
			colorValue->byA = pbyData[iR_img_Local * iPitch +iC_img_Local *4+3];
			colorValue->valueType = VALUE_TYPE_RGBA;
			return true;
		}

		COLORREF col = img->GetPixel(iC_img_Local,iR_img_Local);
		colorValue->byR = GetRValue(col);
		colorValue->byG = GetGValue(col);
		colorValue->byB = GetBValue(col);
		colorValue->valueType = VALUE_TYPE_RGB;
		return true;
	}

	void CSImageViewerView::DispStatus(CPoint point_v)
	{
		if(m_iImageMax <= 0){return;}
		int iR_img,iC_img;
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		bool bRet_Original;

		ColorValue colorValue;
		bRet_Original = GetColorAtCursor(&(m_image[m_iImageIndex]), point_v, &iR_img, &iC_img, &colorValue);
		if(bRet_Original == false)
		{
			pFrame->m_sStatusRGBOriginal.Format(_T("out of range"));
		}
		else
		{
			switch(colorValue.valueType)
			{
			case VALUE_TYPE_INT:{pFrame->m_sStatusRGBOriginal.Format(_T("% 14d"), colorValue.iValue); break;}
			case VALUE_TYPE_DOUBLE:{pFrame->m_sStatusRGBOriginal.Format(_T("%e"), colorValue.dValue); break;}
			case VALUE_TYPE_RGB:{pFrame->m_sStatusRGBOriginal.Format(_T("(%d, %d, %d)"), colorValue.byR, colorValue.byG, colorValue.byB); break;}
			case VALUE_TYPE_RGBA:{pFrame->m_sStatusRGBOriginal.Format(_T("(%d, %d, %d, %d)"), colorValue.byR, colorValue.byG, colorValue.byB, colorValue.byA); break;}
			}
		}

		bool bRet_Processed = false;
		pFrame->m_sStatusRGBProcessed.Format(_T("not processed"));

		if(m_image[m_iImageIndex].GetProcessIndex() != 0)
		{
			bRet_Processed = GetColorAtCursor(m_image[m_iImageIndex].GetCurrentProcess(), point_v, &iR_img, &iC_img, &colorValue);
			if(bRet_Processed == false)
			{
				pFrame->m_sStatusRGBProcessed.Format(_T("out of range"));
			}
			else
			{
				if(colorValue.valueType == VALUE_TYPE_RGB)
				{
					pFrame->m_sStatusRGBProcessed.Format(_T("(%d, %d, %d)"), colorValue.byR, colorValue.byG, colorValue.byB);
				}
				else
				{
					pFrame->m_sStatusRGBProcessed.Format(_T("(%d, %d, %d, %d)"), colorValue.byR, colorValue.byG, colorValue.byB, colorValue.byA);
				}
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

		CRect rect_i=view.GetRect_i();
		if(view.GetDragging() == true)
		{
			if(view.GetRect_v().IsRectNull() == TRUE)
			{
				pFrame->m_sStatusSelection.Format(_T("not selected"));
			}
			else
			{
				CRect rectTemp = v_to_i(&(view.GetRect_v()));
				pFrame->m_sStatusSelection.Format(_T("(%d, %d) - (%d, %d) : %d x %d "), rectTemp.left,rectTemp.top,rectTemp.right,rectTemp.bottom,rectTemp.right-rectTemp.left+1,rectTemp.bottom-rectTemp.top+1);	
			}
		}
		else
		{
			if(rect_i.IsRectNull() == TRUE)
			{
				pFrame->m_sStatusSelection.Format(_T("not selected"));
			}
			else
			{
				pFrame->m_sStatusSelection.Format(_T("(%d, %d) - (%d, %d) : %d x %d "), rect_i.left,rect_i.top,rect_i.right,rect_i.bottom,rect_i.right-rect_i.left+1,rect_i.bottom-rect_i.top+1);	
			}
		}

		pFrame->SendMessage(WM_COMMAND, ID_DISP_STATUS_MOUSE_POS);

		pFrame->m_sStatusZoom.Format(_T("%.3f%%"), 100*view.GetScale());
		pFrame->SendMessage(WM_COMMAND, ID_DISP_STATUS_ZOOM);

		return;
	}
	void CSImageViewerView::OnMouseMove(UINT nFlags, CPoint point_v)
	{
		if(m_iImageMax <= 0){return;}
		if(m_image[m_iImageIndex].GetCurrentProcess()->IsNull() == true){return;}

		DispStatus(point_v);

		view.OnMouseMove(nFlags, point_v, &(m_image[m_iImageIndex]), this);
		CView::OnMouseMove(nFlags, point_v);
	}


	void CSImageViewerView::OnLButtonDown(UINT nFlags, CPoint point_v)
	{
		if(m_iImageMax <= 0){return;}
		if(m_image[m_iImageIndex].GetCurrentProcess()->IsNull() == true){return;}

		view.OnLButtonDown(nFlags, point_v, &(m_image[m_iImageIndex]), this);
		if(m_bSynchroScroll==true)
		{
			view.GetScrollSetting(&scr);
		}
		else
		{
			view.GetScrollSetting(&(m_image[m_iImageIndex].scr));
		}
		CView::OnLButtonDown(nFlags, point_v);
	}


	void CSImageViewerView::OnLButtonUp(UINT nFlags, CPoint point_v)
	{
		if(m_iImageMax <= 0){return;}
		if(m_image[m_iImageIndex].GetCurrentProcess()->IsNull() == true){return;}

		CPoint point_v_out;
		view.OnLButtonUp(nFlags, point_v, true, &(m_image[m_iImageIndex]), this, &point_v_out);
		if(m_bSynchroScroll==true)
		{
			view.GetScrollSetting(&scr);
		}
		else
		{
			view.GetScrollSetting(&(m_image[m_iImageIndex].scr));
		}
		CView::OnLButtonUp(nFlags, point_v);
	}


	void CSImageViewerView::OnTimer(UINT_PTR nIDEvent)
	{
		if(nIDEvent == TIMER_INIT)
		{
			KillTimer(TIMER_INIT);

			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if (pFrame == NULL) {return;}
			pFrame->ShowNormal();
			//		m_sFilePath.Format(_T("C:\\Users\\PC9\\Desktop\\test"));
			if(m_sFilePath.GetLength()>0)
			{
				ReadImage(m_sFilePath);
			}
			//	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
			return;
		}

		if(nIDEvent == TIMER_REFRESH)
		{
			Invalidate();
			return;
		}

		CView::OnTimer(nIDEvent);
	}


	BOOL CSImageViewerView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
	{
		if(view.OnSetCursor(pWnd, nHitTest, message)==TRUE){return TRUE;}
		return CView::OnSetCursor(pWnd, nHitTest, message);
	}

	void CSImageViewerView::OnSelectAll()
	{
		CRect rect_i;
		FullDomain(&rect_i);
		view.SetRect_i(&rect_i);
		Invalidate();
	}

	BOOL CSImageViewerView::PreTranslateMessage(MSG* pMsg)
	{
		if(pMsg->message == WM_MOUSEWHEEL)
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

		if(pMsg->message == WM_MOUSEHWHEEL)
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

				if(pMsg->wParam == VK_UP){OperateImagePPFW(-1);Invalidate();return TRUE;}
				if(pMsg->wParam == VK_DOWN){OperateImagePPFW(+1);Invalidate();return TRUE;}
				if(pMsg->wParam == VK_PRIOR){OperateImagePPFW(-10);Invalidate();return TRUE;}
				if(pMsg->wParam == VK_NEXT){OperateImagePPFW(+10);Invalidate();return TRUE;}
				if(pMsg->wParam == VK_HOME){OperateImagePPFW(INT_MIN);Invalidate();return TRUE;}
				if(pMsg->wParam == VK_END){OperateImagePPFW(INT_MAX);Invalidate();return TRUE;}
				if(pMsg->wParam == VK_LEFT){OperateImagePPFWFlexible(-1);return TRUE;}
				if(pMsg->wParam == VK_RIGHT){OperateImagePPFWFlexible(+1);return TRUE;}
			}

			if(GetKeyState(VK_SHIFT)<0)
			{
			}	


			if(pMsg->wParam == VK_RETURN) {EnterFullScreen(); return TRUE; } 
			if(pMsg->wParam == VK_ESCAPE) {if(m_bExitByEsc==false){return TRUE;}::PostQuitMessage( 0 );}

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

	void CSImageViewerView::OnReSet()
	{
		if(m_sFilePath.Compare(_T("Clipboard")) != 0)
		{
			ReadImage(m_sFilePath);
			return;
		}
		ResetImage(true, true);
	};

	void CSImageViewerView::OnUnDo()
	{
		bool bRet = m_image[m_iImageIndex].UnDo();
		if(bRet != true){return;}
		Invalidate();
	}
	void CSImageViewerView::OnReDo()
	{
		bool bRet = m_image[m_iImageIndex].ReDo();
		if(bRet != true){return;}
		Invalidate();
	}
	void CSImageViewerView::OnRename()
	{
		if(m_iImageMax<=0){return;}

		CInputDlg dlg;

		CString sFileName;
		CString sFilePath;
		sFilePath.Format(_T("%s"),m_image[m_iImageIndex].GetDataSource());
		int iPlace = sFilePath.ReverseFind('\\');
		if(iPlace<=0){return;}

		sFileName.Format(_T("%s"), sFilePath.Mid(iPlace+1));

		dlg.m_sEditInput.Format(_T("%s"), sFileName);
		INT_PTR iRet = dlg.DoModal();
		if(iRet != IDOK){return;}

		CString sNewFilePath;
		sNewFilePath.Format(_T("%s%s"),sFilePath.Left(iPlace+1),dlg.m_sEditInput);
		if(sNewFilePath.CompareNoCase(m_image[m_iImageIndex].GetDataSource())==0){return;}
		BOOL bRet = MoveFile(m_image[m_iImageIndex].GetDataSource(), sNewFilePath);
		if(bRet != TRUE){return;}
		m_image[m_iImageIndex].SetDataSource(sNewFilePath);
		SetCaption();
	}

	void CSImageViewerView::OnScroll(int iSB, int nSBCode, int nPos)
	{
		if(m_iImageMax <= 0){return;}
		view.OnScroll(iSB, nSBCode, nPos, &(m_image[m_iImageIndex]), this);
		if(m_bSynchroScroll==true)
		{
			view.GetScrollSetting(&scr);
		}
		else
		{
			view.GetScrollSetting(&(m_image[m_iImageIndex].scr));
		}

		CPoint point_v;
		GetCursorPos(&point_v);
		ScreenToClient(&point_v);
		DispStatus(point_v);
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
		bool bSelected = view.m_bRegionSelected;

		pPopup->EnableMenuItem(ID_EDIT_COPY, MF_BYCOMMAND | (( bSelected == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_MENU_EDIT_COPY_AS, MF_BYCOMMAND | (( bSelected == true) ? MF_ENABLED : MF_DISABLED));

		pPopup->EnableMenuItem(ID_EDIT_COPY, MF_BYCOMMAND | (( bFileOpened == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_EDIT_PASTE, MF_BYCOMMAND | (( bFileOpened == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_MENU_EDIT_PASTE_AS, MF_BYCOMMAND | (( bFileOpened == true) ? MF_ENABLED : MF_DISABLED));

		pPopup->EnableMenuItem(ID_MENU_EDIT_EQU_HIST, MF_BYCOMMAND | (( bFileOpened == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_MENU_EDIT_CONVERT_COLOR_SPACE, MF_BYCOMMAND | (( bFileOpened == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_MENU_EDIT_CHANGE_COLOR_DEPTH, MF_BYCOMMAND | (( bFileOpened == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_MENU_EDIT_COLOR_CORRECTON, MF_BYCOMMAND | (( bFileOpened == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_MENU_EDIT_COLORIZE, MF_BYCOMMAND | (( bFileOpened == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_MENU_EDIT_TRANSPARENT, MF_BYCOMMAND | (( bFileOpened == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_MENU_EDIT_INVERT, MF_BYCOMMAND | (( bFileOpened == true) ? MF_ENABLED : MF_DISABLED));
		pPopup->EnableMenuItem(ID_MENU_EDIT_SET_SELECTION, MF_BYCOMMAND | (( bFileOpened == true) ? MF_ENABLED : MF_DISABLED));

		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
#include "SettingDlg.h"
	void CSImageViewerView::OnSetToolOption()
	{
		CSettingDlg dlg;
		dlg.m_bExitByEsc=m_bExitByEsc;
		dlg.m_bCenteredWhenFullScreen=m_bCenteredWhenFullScreen;
		dlg.m_bPPFWwithoutCtrlWhenFullScreen=m_bPPFWwithoutCtrlWhenFullScreen;

		INT_PTR iRet = dlg.DoModal();
		if(iRet != IDOK){return;}


		m_bExitByEsc=dlg.m_bExitByEsc;
		m_bCenteredWhenFullScreen=dlg.m_bCenteredWhenFullScreen;
		m_bPPFWwithoutCtrlWhenFullScreen=dlg.m_bPPFWwithoutCtrlWhenFullScreen;
		SaveSetting(m_sIniFilePath);
	}