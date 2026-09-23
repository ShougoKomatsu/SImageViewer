
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
		ON_COMMAND(ID_EDIT_COPY, &CSImageViewerView::OnEditCopy)
		ON_COMMAND(ID_EDIT_PASTE, &CSImageViewerView::OnEditPaste)
		ON_COMMAND(ID_MENU_EDIT_SET_SELECTION, &CSImageViewerView::OnSetSelection)
		ON_COMMAND(ID_MENU_EDIT_COPY_AS, &CSImageViewerView::OnCopyAs)
		ON_COMMAND(ID_MENU_EDIT_PASTE_AS, &CSImageViewerView::OnPasteAs)
		ON_COMMAND(ID_MENU_EDIT_CONVERT_COLOR_SPACE, &CSImageViewerView::OperateConvertColorSpace)
		ON_COMMAND(ID_MENU_EDIT_CHANGE_COLOR_DEPTH, &CSImageViewerView::OperateChangeColorDepth)
		ON_COMMAND(ID_MENU_EDIT_COLOR_CORRECTON, &CSImageViewerView::OperateBrightnessContrastGamma)
		ON_COMMAND(ID_MENU_EDIT_COLORIZE, &CSImageViewerView::OperateColorize)
		ON_COMMAND(ID_MENU_EDIT_TRANSPARENT, &CSImageViewerView::OperateTransparent)
		ON_COMMAND(ID_MENU_EDIT_INVERT, &CSImageViewerView::OperateInvert)
		ON_COMMAND(ID_MENU_EDIT_EQU_HIST, &CSImageViewerView::OperateEquHistImage)
		ON_COMMAND(ID_MENU_EDIT_RESAMPLE, &CSImageViewerView::OperateResample)
		ON_COMMAND(ID_MENU_TOOL_FILEFORMAT, &CSImageViewerView::SetToolFormat)
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

		m_bScrollPin=false;
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
	if(m_bScrollPin == true)
	{
		m_bScrollPin = false;
		return;
	}
	m_bScrollPin = true;
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
			pFrame->m_bRegionSelected = true;
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

		view.OnDraw(this, pDC,  &(m_image[m_iImageIndex]), true);

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
		view.SetScroll(&(m_image[m_iImageIndex]), m_bScrollPin, this, true);
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
			view.ZoomReset(&(m_image[m_iImageIndex]), m_bScrollPin, this, true);
		}
		view.SetMouseMode(CHANGE_NONE);
		view.SetRect_i(NULL);
		pFrame->m_bRegionSelected = false;
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
	}
	bool CSImageViewerView::AddImage(CString sFilePath)
	{
		CStringArray saFilePath;
		bool bRet = RecursivelyGetImageFilePaths(sFilePath, &saFilePath, &m_fileFomatList);
		if(bRet != true){return false;}
		int iImageNum = CountImages(sFilePath, &m_fileFomatList);

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
		pFrame->m_bFileOpened = true;
		pFrame->m_bRegionSelected = true;

		ResetImage(true, false);
		SetCaption();
		return true;

	}

	bool CSImageViewerView::ReadImage(CString sFilePath)
	{
		for(int i = 0; i<m_iImageMax; i++)
		{
			m_image[i].Init();
		}
		SAFE_DELETE(m_image);
		m_sFilePath.Format(_T("%s"), sFilePath);
		CStringArray saFilePath;
		bool bRet = RecursivelyGetImageFilePaths(sFilePath, &saFilePath, &m_fileFomatList);
		if((bRet != true) && saFilePath.GetCount()<0){return false;}
		int iImageNum = CountImages(sFilePath, &m_fileFomatList);

		m_image = new PanImage[iImageNum];

		int iImageIndex = 0;
		for(int i = 0; i<saFilePath.GetCount(); i++)
		{
			bRet = ReadAndAppendImage(saFilePath.GetAt(i), &m_fileFomatList, &m_image[iImageIndex], iImageIndex, &iImageIndex);
			if(bRet != true){return false;}
		}

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		m_iImageIndex = 0;
		m_iImageMax = iImageNum;
		pFrame->m_bFileOpened = true;
		pFrame->m_bRegionSelected = true;
		if(m_iImageMax>=2){m_bScrollPin=true; Invalidate();}

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

		AddImage(sFilePaths);
	}

	void CSImageViewerView::OnFileOpen()
	{
		CString sFilePaths;
		bool bRet = GetOpenFileList(&sFilePaths);

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
		pFrame->m_bRegionSelected = true;



		ResetImage(true, true);
		SetCaption();
	}
	void CSImageViewerView::SetCaption()
	{
		CString sCaption;
		if(m_iImageMax==1)
		{
			sCaption.Format(_T("%s - SImageViewer"), m_image[m_iImageIndex].GetDataSource());
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
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		SAFE_DELETE(m_image);
		m_iImageIndex = 0;
		m_iImageMax = 1;
		m_image = new PanImage[m_iImageMax];
		BOOL bRet = CopyFromClipBoardImg(&(m_image[m_iImageIndex]));

		if(bRet != TRUE){return;}
		m_sFilePath.Format(_T("Clipboard"));

		pFrame->m_bFileOpened = true;
		pFrame->m_bRegionSelected = true;
		ResetImage(true, true);
		SetCaption();
	}

	void CSImageViewerView::FullDomain(CRect* rect_i)
	{
		if(m_iImageMax <= 0){return;}
		rect_i->SetRect(0, 0, m_image[m_iImageIndex].GetCurrentProcess()->GetWidth()-1,m_image[m_iImageIndex].GetCurrentProcess()->GetHeight()-1);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->m_bRegionSelected = true;
	}
	void CSImageViewerView::OperateTransparent()
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
	void CSImageViewerView::OperateCopyHistGramToClipboard()
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
	}

	
	void CSImageViewerView::OperateInvert()
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
			pFrame->m_bRegionSelected = false;
		}

		ConvertImage(&imgRGB,m_image[m_iImageIndex].ProgressImageProcess());

		Invalidate();
	}

	void CSImageViewerView::OperateEquHistImage()
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
			pFrame->m_bRegionSelected = false;
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

	void CSImageViewerView::OperateResample()
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
				pFrame->m_bRegionSelected = false;
			}
			return;
		}

		CImage imgSrc;
		CopyImage_CImage(m_image[m_iImageIndex].GetCurrentProcess(), &imgSrc);
		if((dlg.m_resample == RESIZE_NEAREST) || (dlg.m_resample == RESIZE_BILINEAR))
		{
			Resize(&imgSrc, m_image[m_iImageIndex].ProgressImageProcess(), dlg.m_iWidth,dlg.m_iHeight,dlg.m_resample);
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
			pFrame->m_bRegionSelected = false;
		}
		Invalidate();
	}
	void CSImageViewerView::OperateChangeColorDepth()
	{
		if(m_iImageMax <= 0){return;}
		CChangeColorDepthDlg colorDepthDlg;

		int iUsedColors;
		bool bGrayScale;

		CountColorNum(m_image[m_iImageIndex].GetCurrentProcess(), &iUsedColors, NULL);

		MakeColorTable(m_image[m_iImageIndex].GetCurrentProcess(), NULL,NULL, 1<<min(24, m_image[m_iImageIndex].GetCurrentProcess()->GetBPP()), &iUsedColors, &bGrayScale);

		colorDepthDlg.m_iColors = iUsedColors;
		colorDepthDlg.m_bGrayScale = bGrayScale;
		colorDepthDlg.m_iBPP = m_image[m_iImageIndex].GetCurrentProcess()->GetBPP();

		INT_PTR iRet = colorDepthDlg.DoModal();
		if(iRet != IDOK){return;}

		CImage imgSrc;
		CopyImage_CImage(m_image[m_iImageIndex].GetCurrentProcess(), &imgSrc);

		switch(colorDepthDlg.m_iMode)
		{
		case 0:{ConvertImage_LossLess(&imgSrc, colorDepthDlg.m_iBPP, m_image[m_iImageIndex].ProgressImageProcess());break;}
		case 1:{ConvertImage_AreaCoverage(&imgSrc,colorDepthDlg.m_iBPP, m_image[m_iImageIndex].ProgressImageProcess());break;}
		case 2:{ConvertImage_ByDeviation(&imgSrc, colorDepthDlg.m_iBPP, m_image[m_iImageIndex].ProgressImageProcess());break;}
		}
		//		m_image[m_iImageIndex].m_imageProcessed[(m_image[m_iImageIndex].m_iImgProcessIndex % MAX_IMG_PROCESS)].Save(_T("D:\\test.bmp"));
		Invalidate();
	}

	void CSImageViewerView::OperateConvertColorSpace()
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

	void CSImageViewerView::OperateColorize()
	{
		if(m_iImageMax <= 0){return;}
		if(_IsImageMonochrome(m_image[m_iImageIndex].GetCurrentProcess())==false){AfxMessageBox(_T("This image is not monochrome.")); return;}
		bool bAutoFull = false;
		CRect rect_i=view.GetRect_i();
		if(rect_i.IsRectNull() == TRUE){bAutoFull = true;FullDomain(&rect_i);}

		CColorizeDlg dlgModify;

		CImage imgClipped;
		ClipImage(m_image[m_iImageIndex].GetCurrentProcess(), &imgClipped, rect_i.top,rect_i.left, rect_i.bottom, rect_i.right); 
		CopyImage_CImage(&imgClipped, &dlgModify.m_image);
		INT_PTR iRet = dlgModify.DoModal();
		if(iRet != IDOK)
		{
			if(bAutoFull == true)
			{
				view.SetRect_v(NULL);
				view.SetRect_i(NULL);
				CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
				pFrame->m_bRegionSelected = false;
			}
			return;
		}
		
		ImgRGB imgRGB;
		_ConvertImage(&(dlgModify.m_imageColorized), &imgRGB);
		
		ConvertImage(&imgRGB, m_image[m_iImageIndex].ProgressImageProcess());
		if(bAutoFull == true)
		{
			view.SetRect_v(NULL);
			view.SetRect_i(NULL);
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->m_bRegionSelected = false;
		}
		Invalidate();
	
	}
	void CSImageViewerView::OperateBrightnessContrastGamma()
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
				pFrame->m_bRegionSelected = false;
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
			pFrame->m_bRegionSelected = false;
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
		view.GetSizeIfNoBar(&iHeightIfNoBar_v, &iWidthIfNoBar_v, this, true);
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

		m_bBeingFullScreen = false;

		TCHAR tszExePath[MAX_PATH];
		::GetModuleFileName(NULL, tszExePath, MAX_PATH);
		CString sExePath;
		sExePath.Format(_T("%s"), tszExePath);
		int iPlace = sExePath.ReverseFind('\\');
		m_sIniFilePath.Format(_T("%s\\setting.ini"),sExePath.Left(iPlace)); 
		UINT uiTypeNum;
		bool bRet = GetImageTypeNum(m_sIniFilePath, &uiTypeNum);
		m_fileFomatList.Set(uiTypeNum);
		for(int i=0; i<uiTypeNum; i++)
		{
			CString sType;
			bRet = GetImageType(m_sIniFilePath, i, &sType);
			bRet = GetFileFormat(m_sIniFilePath, sType, &(m_fileFomatList.fileFormat[i]));
		}

		pFrame->m_pView = this;
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

	bool CSImageViewerView::OnImagePPFW(const int iStep)
	{
		if(iStep==INT_MAX){m_iImageIndex=m_iImageMax-1;}
		else if(iStep==INT_MIN){m_iImageIndex=0;}
		else
		{
			m_iImageIndex+=iStep;
			if(iStep<0){m_iImageIndex = max(m_iImageIndex,0);}
			if(iStep>0){m_iImageIndex = min(m_iImageIndex,m_iImageMax-1);}
		}
		ResetImage(false, false);
		if(m_bScrollPin==false)
		{
			double dR=(m_image[m_iImageIndex].m_dDispOriginR_tv);
				double dC=(m_image[m_iImageIndex].m_dDispOriginC_tv);
			view.ZoomChangeAbs(m_image[m_iImageIndex].m_iScaleIndex, &(m_image[m_iImageIndex]), false, this, true);

			view.SetDispOriginR_tv(dR, false, &(m_image[m_iImageIndex].m_dDispOriginR_tv));
			view.SetDispOriginC_tv(dC, false, &(m_image[m_iImageIndex].m_dDispOriginC_tv));
			
		view.OnScroll(SB_VERT, -1, 0, &(m_image[m_iImageIndex]), m_bScrollPin, this, true);
		view.OnScroll(SB_HORZ, -1, 0, &(m_image[m_iImageIndex]), m_bScrollPin, this, true);
		}
		SetCaption();
		Invalidate();
		return true;
	}


	bool CSImageViewerView::ZoomChange(int iR0_i, int iC0_i, int iR1_i, int iC1_i)
	{
		if(m_iImageMax <= 0){return false;}
		view.ZoomChange(iR0_i, iC0_i, iR1_i, iC1_i, &(m_image[m_iImageIndex]), m_bScrollPin, this, true);
		return true; 
	}

	bool CSImageViewerView::ZoomChange(int iChange)
	{
		if(m_iImageMax <= 0){return false;}
		view.ZoomChange(iChange, &(m_image[m_iImageIndex]), m_bScrollPin, this, true);		
		return true; 
	}
	
	bool CSImageViewerView::ZoomChangeAbs(int iChangeAbs)
	{
		if(m_iImageMax <= 0){return false;}
		view.ZoomChangeAbs(iChangeAbs, &(m_image[m_iImageIndex]), m_bScrollPin, this, true);		
		return true; 
	}

	bool CSImageViewerView::ZoomChange(int iMousePosR_v, int iMousePosC_v, int iChange)
	{
		if(m_iImageMax <= 0){return false;}
		view.ZoomChange(iMousePosR_v, iMousePosC_v, iChange, &(m_image[m_iImageIndex]), m_bScrollPin, this, true);
		return true; 
	}



	void CSImageViewerView::EnterFullScreen()
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame == NULL) {return;}
		pFrame->EnterFullScreen();
		m_bBeingFullScreen = true;

	}

	void CSImageViewerView::ExitFullScreen()
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame == NULL) {return;}
		pFrame->ExitFullScreen();
		m_bBeingFullScreen = false;
	}



	bool CSImageViewerView::GetColorAtCursor(PanImage* img, CPoint point_v, int* iR_img, int* iC_img, ColorValue* colorValue)
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

		switch(img->GetImageType())
		{
		case IMAGE_TYPE_IIMAGE:
			{
				int iValue;
				img->GetValue(iR_img_Local,iC_img_Local,&iValue);
				colorValue->iValue = iValue;
				colorValue->valueType = VALUE_TYPE_INT;
				return true;
			}
		case IMAGE_TYPE_DIMAGE:
			{
				double dValue;
				img->GetValue(iR_img_Local,iC_img_Local,&dValue);
				colorValue->dValue = dValue;
				colorValue->valueType = VALUE_TYPE_DOUBLE;
				return true;
			}
		case IMAGE_TYPE_CIMAGE:
			{
				return GetColorAtCursor(img->SetCImage(), point_v, iR_img, iC_img, colorValue);
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

		view.OnMouseMove(nFlags, point_v, &(m_image[m_iImageIndex]), this, true);
		CView::OnMouseMove(nFlags, point_v);
	}


	void CSImageViewerView::OnLButtonDown(UINT nFlags, CPoint point_v)
	{
		view.OnLButtonDown(nFlags, point_v, &(m_image[m_iImageIndex]), this, true);
		CView::OnLButtonDown(nFlags, point_v);
	}


	void CSImageViewerView::OnLButtonUp(UINT nFlags, CPoint point_v)
	{
		view.OnLButtonUp(nFlags, point_v, &(m_image[m_iImageIndex]), m_bScrollPin, this, true);
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
			if(pMsg->wParam == VK_F2)
			{
				OperateFileNameChange();
				return TRUE; 
			}
			if(GetKeyState(VK_CONTROL)<0)
			{	
				if(pMsg->wParam == 'H')
				{
					OperateCopyHistGramToClipboard();
					return TRUE; 
				}
				if(pMsg->wParam == 'A')
				{
					CRect rect_i;
					FullDomain(&rect_i);
					view.SetRect_i(&rect_i);
					return TRUE; 
				}
				if(pMsg->wParam == 'Z')
				{
					bool bRet = m_image[m_iImageIndex].UnDo();
					if(bRet != true){return FALSE;}

					Invalidate();
					return TRUE; 
				}
				if(pMsg->wParam == 'Y')
				{
					bool bRet = m_image[m_iImageIndex].ReDo();
					if(bRet != true){return FALSE;}

					Invalidate();
					return TRUE; 
				}

				if(pMsg->wParam == VK_LEFT){OnImagePPFW(-1);Invalidate();return TRUE;}
				if(pMsg->wParam == VK_RIGHT){OnImagePPFW(+1);Invalidate();return TRUE;}
				if(pMsg->wParam == VK_PRIOR){OnImagePPFW(-10);Invalidate();return TRUE;}
				if(pMsg->wParam == VK_NEXT){OnImagePPFW(+10);Invalidate();return TRUE;}
				if(pMsg->wParam == VK_HOME){OnImagePPFW(INT_MIN);Invalidate();return TRUE;}
				if(pMsg->wParam == VK_END){OnImagePPFW(INT_MAX);Invalidate();return TRUE;}
			}

			if(GetKeyState(VK_SHIFT)<0)
			{
				//			if(pMsg->wParam == 'U'){OperateEquHistImage();return TRUE;}
				//		if(pMsg->wParam == 'G'){OperateBrightnessContrastGamma();return TRUE;}
			}	

			if(pMsg->wParam == VK_F5)
			{
				if(m_sFilePath.Compare(_T("Clipboard")) != 0)
				{
					return ReadImage(m_sFilePath);
				}
				ResetImage(true, true);
			}
			if(pMsg->wParam == 'R'){OperateRotaateImage(ROTATE_CW90);return TRUE;}

			if(pMsg->wParam == VK_RETURN) { if(m_bBeingFullScreen == true){ ExitFullScreen(); return TRUE;} EnterFullScreen(); return TRUE; } 
			if(pMsg->wParam == VK_ESCAPE) { if(m_bBeingFullScreen == true){ ExitFullScreen(); return TRUE;} ::PostQuitMessage( 0 );}

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

	void CSImageViewerView::OperateFileNameChange()
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
		view.OnScroll(iSB, nSBCode, nPos, &(m_image[m_iImageIndex]), m_bScrollPin, this, true);
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
		bool bSelected = pFrame ->m_bRegionSelected;

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