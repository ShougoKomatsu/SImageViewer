// FileFormatDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "FileFormatDlg.h"
#include "afxdialogex.h"
#include "InputDlg.h"

#define TIMER_INIT_FILE_FORMAT (100)
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
	ON_BN_CLICKED(IDC_FILE_FORMAT_BUTTON_ADD, &CFileFormatDlg::OnBnClickedFileFormatButtonAdd)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_FILE_FORMAT_EDIT_WIDTH, &CFileFormatDlg::OnChangeFileFormatEditWidth)
	ON_EN_CHANGE(IDC_FILE_FORMAT_EDIT_HEIGHT, &CFileFormatDlg::OnChangeFileFormatEditHeight)
	ON_EN_CHANGE(IDC_FILE_FORMAT_EDIT_HEIGHT_INFO_OFFSET, &CFileFormatDlg::OnChangeFileFormatEditHeightInfoOffset)
	ON_EN_CHANGE(IDC_FILE_FORMAT_EDIT_DATA_OFFSET, &CFileFormatDlg::OnChangeFileFormatEditDataOffset)
	ON_EN_CHANGE(IDC_FILE_FORMAT_EDIT_DATA_OFFSET_OFFSET, &CFileFormatDlg::OnChangeFileFormatEditDataOffsetOffset)
	ON_EN_CHANGE(IDC_FILE_FORMAT_EDIT_DATA_INFO_OFFSET, &CFileFormatDlg::OnChangeFileFormatEditDataInfoOffset)
	ON_EN_CHANGE(IDC_FILE_FORMAT_EDIT_WIDTH_INFO_OFFSET, &CFileFormatDlg::OnChangeFileFormatEditWidthInfoOffset)
END_MESSAGE_MAP()


// CFileFormatDlg メッセージ ハンドラー



BOOL CFileFormatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	m_ListFileType.ResetContent();
	for(int i=0; i<m_fileFormatList.uiNum; i++)
	{
		m_ListFileType.AddString(m_fileFormatList.fileFormat[i].sType);
	}
	
	SetTimer(TIMER_INIT_FILE_FORMAT, 100, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CFileFormatDlg::DispSetting(const CString sType)
{
	for(int i=0; i<m_fileFormatList.uiNum; i++)
	{
		if(sType.CompareNoCase(m_fileFormatList.fileFormat[i].sType)==0)
		{
			for(int j=0; j<m_ListFileType.GetCount(); j++)
			{
				CString sTypeList;
				m_ListFileType.GetText(j, sTypeList);
				if(sTypeList.CompareNoCase(sType)==0)
				{
					m_ListFileType.SetCurSel(j);
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
		}
	}

}

void CFileFormatDlg::OnSelchangeFileFormatListType()
{
	int iSel = m_ListFileType.GetCurSel();
	CString sType;
	m_ListFileType.GetText(iSel, sType);
	DispSetting(sType);
	return;
}


void CFileFormatDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();


	for(int i=0; i<m_fileFormatList.uiNum; i++)
	{
		CString sKey;
		sKey.Format(_T("Type%d"), i+1);
		CString sType;
		sType.Format(_T("%s"),m_fileFormatList.fileFormat[i].sType);
		WritePrivateProfileString(_T("Types"), sKey, sType, m_sIniFilePath);

		WritePrivateProfileString(sType, _T("iWidth"), m_sEditWidth,m_sIniFilePath);
		WritePrivateProfileString(sType, _T("iHeight"), m_sEditHeight,m_sIniFilePath);
		WritePrivateProfileString(sType, _T("iDataOffset"), m_sEditDataOffset,m_sIniFilePath);
		WritePrivateProfileString(sType, _T("iWidthInfoOffset"), m_sEditWidthInfoOffset,m_sIniFilePath);
		WritePrivateProfileString(sType, _T("iHeightInfoOffset"), m_sEditHeightInfoOffset,m_sIniFilePath);
		WritePrivateProfileString(sType, _T("iDataInfoOffset"), m_sEditDataInfoOffset,m_sIniFilePath);
		WritePrivateProfileString(sType, _T("iDataOffsetOffset"), m_sEditDataOffsetOffset,m_sIniFilePath);

	}

}

void CFileFormatDlg::OnBnClickedFileFormatButtonAdd()
{
	CInputDlg dlg;
	INT_PTR iRet = dlg.DoModal();
	if(iRet != IDOK){return;}

	FileFormatList fileFormatListTemp;
	fileFormatListTemp.Copy(&m_fileFormatList);
	m_fileFormatList.Set(fileFormatListTemp.uiNum+1);
	for(int i=0; i<fileFormatListTemp.uiNum; i++)
	{
		m_fileFormatList.fileFormat[i].Copy(&(fileFormatListTemp.fileFormat[i]));
	}
	m_fileFormatList.fileFormat[m_fileFormatList.uiNum-1].sType.Format(_T("%s"),dlg.m_sEditInput);
	m_fileFormatList.fileFormat[m_fileFormatList.uiNum-1].iWidth=0;
	m_fileFormatList.fileFormat[m_fileFormatList.uiNum-1].iHeight=0;
	m_fileFormatList.fileFormat[m_fileFormatList.uiNum-1].iDataOffset=0;
	m_fileFormatList.fileFormat[m_fileFormatList.uiNum-1].iWidthInfoOffset=0;
	m_fileFormatList.fileFormat[m_fileFormatList.uiNum-1].iHeightInfoOffset=0;
	m_fileFormatList.fileFormat[m_fileFormatList.uiNum-1].iDataInfoOffset=0;
	m_fileFormatList.fileFormat[m_fileFormatList.uiNum-1].iDataOffsetOffset=0;

	m_ListFileType.AddString(dlg.m_sEditInput);
	m_ListFileType.SetSel(m_fileFormatList.uiNum-1);
	UpdateData(FALSE);
	DispSetting(dlg.m_sEditInput);
}


void CFileFormatDlg::OnTimer(UINT_PTR nIDEvent)
{

	if(nIDEvent==TIMER_INIT_FILE_FORMAT)
	{
		if(m_ListFileType.GetCount()>0)
		{
			m_ListFileType.SetCurSel(0);
			OnSelchangeFileFormatListType();
		}
		UpdateData(FALSE);
		KillTimer(TIMER_INIT_FILE_FORMAT);
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CFileFormatDlg::OnChangeFileFormatEditWidth()
{
	UpdateData(TRUE);
	int iSel = m_ListFileType.GetCurSel();
	CString sType;
	m_ListFileType.GetText(iSel, sType);

	for(int i=0; i<m_fileFormatList.uiNum; i++)
	{
		if(sType.CompareNoCase(m_fileFormatList.fileFormat[i].sType)==0)
		{
			m_fileFormatList.fileFormat[i].iWidth=_ttoi(m_sEditWidth);
			return;
		}
	}
	DispSetting(sType);
}


void CFileFormatDlg::OnChangeFileFormatEditHeight()
{
	UpdateData(TRUE);
	int iSel = m_ListFileType.GetCurSel();
	CString sType;
	m_ListFileType.GetText(iSel, sType);

	for(int i=0; i<m_fileFormatList.uiNum; i++)
	{
		if(sType.CompareNoCase(m_fileFormatList.fileFormat[i].sType)==0)
		{
			m_fileFormatList.fileFormat[i].iHeight=_ttoi(m_sEditHeight);
			return;
		}
	}
	DispSetting(sType);
}


void CFileFormatDlg::OnChangeFileFormatEditWidthInfoOffset()
{
	UpdateData(TRUE);
	int iSel = m_ListFileType.GetCurSel();
	CString sType;
	m_ListFileType.GetText(iSel, sType);

	for(int i=0; i<m_fileFormatList.uiNum; i++)
	{
		if(sType.CompareNoCase(m_fileFormatList.fileFormat[i].sType)==0)
		{
			m_fileFormatList.fileFormat[i].iWidthInfoOffset=_ttoi(m_sEditWidthInfoOffset);
			return;
		}
	}
	DispSetting(sType);
}

void CFileFormatDlg::OnChangeFileFormatEditHeightInfoOffset()
{
	UpdateData(TRUE);
	int iSel = m_ListFileType.GetCurSel();
	CString sType;
	m_ListFileType.GetText(iSel, sType);

	for(int i=0; i<m_fileFormatList.uiNum; i++)
	{
		if(sType.CompareNoCase(m_fileFormatList.fileFormat[i].sType)==0)
		{
			m_fileFormatList.fileFormat[i].iHeightInfoOffset=_ttoi(m_sEditHeightInfoOffset);
			return;
		}
	}
	DispSetting(sType);
}


void CFileFormatDlg::OnChangeFileFormatEditDataOffset()
{
	UpdateData(TRUE);
	int iSel = m_ListFileType.GetCurSel();
	CString sType;
	m_ListFileType.GetText(iSel, sType);

	for(int i=0; i<m_fileFormatList.uiNum; i++)
	{
		if(sType.CompareNoCase(m_fileFormatList.fileFormat[i].sType)==0)
		{
			m_fileFormatList.fileFormat[i].iDataOffset=_ttoi(m_sEditDataOffset);
			return;
		}
	}
	DispSetting(sType);
}


void CFileFormatDlg::OnChangeFileFormatEditDataOffsetOffset()
{
	UpdateData(TRUE);
	int iSel = m_ListFileType.GetCurSel();
	CString sType;
	m_ListFileType.GetText(iSel, sType);

	for(int i=0; i<m_fileFormatList.uiNum; i++)
	{
		if(sType.CompareNoCase(m_fileFormatList.fileFormat[i].sType)==0)
		{
			m_fileFormatList.fileFormat[i].iDataOffsetOffset=_ttoi(m_sEditDataOffsetOffset);
			return;
		}
	}
	DispSetting(sType);
}


void CFileFormatDlg::OnChangeFileFormatEditDataInfoOffset()
{
	UpdateData(TRUE);
	int iSel = m_ListFileType.GetCurSel();
	CString sType;
	m_ListFileType.GetText(iSel, sType);

	for(int i=0; i<m_fileFormatList.uiNum; i++)
	{
		if(sType.CompareNoCase(m_fileFormatList.fileFormat[i].sType)==0)
		{
			m_fileFormatList.fileFormat[i].iDataInfoOffset=_ttoi(m_sEditDataInfoOffset);
			return;
		}
	}
	DispSetting(sType);
}

