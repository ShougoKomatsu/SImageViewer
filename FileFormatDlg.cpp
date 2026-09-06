// FileFormatDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "FileFormatDlg.h"
#include "afxdialogex.h"


// CFileFormatDlg ダイアログ

IMPLEMENT_DYNAMIC(CFileFormatDlg, CDialogEx)

CFileFormatDlg::CFileFormatDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileFormatDlg::IDD, pParent)
	, m_sEditWidth(_T(""))
	, m_sEditHeight(_T(""))
	, m_sEditWidthInfoOffset(_T(""))
	, m_sEditHeightInfoOffset(_T(""))
	, m_sEditDataInfoOffset(_T(""))
	, m_sEditDataOffsetOffset(_T(""))
{

}

CFileFormatDlg::~CFileFormatDlg()
{
}

void CFileFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_FORMAT_LIST_TYPE, m_ListFileType);
	DDX_Text(pDX, IDC_FILE_FORMAT_EDIT_WIDTH, m_sEditWidth);
	DDX_Text(pDX, IDC_FILE_FORMAT_EDIT_HEIGHT, m_sEditHeight);
	DDX_Text(pDX, IDC_FILE_FORMAT_EDIT_DATA_OFFSET, m_sEditDataOffset);
	DDX_Text(pDX, IDC_FILE_FORMAT_EDIT_WIDTH_INFO_OFFSET, m_sEditWidthInfoOffset);
	DDX_Text(pDX, IDC_FILE_FORMAT_EDIT_HEIGHT_INFO_OFFSET, m_sEditHeightInfoOffset);
	DDX_Text(pDX, IDC_FILE_FORMAT_EDIT_DATA_INFO_OFFSET, m_sEditDataInfoOffset);
	DDX_Text(pDX, IDC_FILE_FORMAT_EDIT_DATA_OFFSET_OFFSET, m_sEditDataOffsetOffset);
}


BEGIN_MESSAGE_MAP(CFileFormatDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CFileFormatDlg::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_FILE_FORMAT_LIST_TYPE, &CFileFormatDlg::OnSelchangeFileFormatListType)
END_MESSAGE_MAP()


// CFileFormatDlg メッセージ ハンドラー


void CFileFormatDlg::OnBnClickedOk()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CDialogEx::OnOK();
}


BOOL CFileFormatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	m_ListFileType.ResetContent();
	for(int i=0; i<m_fileFormatList.uiNum; i++)
	{
		m_ListFileType.AddString(m_fileFormatList.fileFormat[i].sType);
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CFileFormatDlg::OnSelchangeFileFormatListType()
{
	int iSel = m_ListFileType.GetCurSel();
	CString sType;
	m_ListFileType.GetText(iSel, sType);

	for(int i=0; i<m_fileFormatList.uiNum; i++)
	{
		if(sType.CompareNoCase(m_fileFormatList.fileFormat[i].sType)==0)
		{
			m_sEditWidth.Format(_T("%d"), m_fileFormatList.fileFormat[i].iWidth);
			m_sEditHeight.Format(_T("%d"), m_fileFormatList.fileFormat[i].iHeight);
			m_sEditDataOffset.Format(_T("%d"), m_fileFormatList.fileFormat[i].iDataOffset);
			m_sEditWidthInfoOffset.Format(_T("%d"), m_fileFormatList.fileFormat[i].iWidthInfoOffset);
			m_sEditHeightInfoOffset.Format(_T("%d"), m_fileFormatList.fileFormat[i].iHeightInfoOffset);
			m_sEditDataInfoOffset.Format(_T("%d"), m_fileFormatList.fileFormat[i].iDataInfoOffset);
			m_sEditDataOffsetOffset.Format(_T("%d"), m_fileFormatList.fileFormat[i].iDataOffsetOffset);
			UpdateData(FALSE);
			return;
		}
	}
	return;
}
