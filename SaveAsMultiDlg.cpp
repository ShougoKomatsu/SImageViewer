// SaveAsMultiDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SImageViewer.h"
#include "SaveAsMultiDlg.h"
#include "afxdialogex.h"


// CSaveAsMultiDlg ダイアログ

IMPLEMENT_DYNAMIC(CSaveAsMultiDlg, CDialogEx)

CSaveAsMultiDlg::CSaveAsMultiDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSaveAsMultiDlg::IDD, pParent)
	, m_sComboExt(_T(""))
	, m_sEditDir(_T(""))
	, m_sComboPrefix1(_T(""))
	, m_sComboPrefix2(_T(""))
	, m_sComboBase(_T(""))
	, m_sComboSuffix1(_T(""))
	, m_sComboSuffix2(_T(""))
{

}

CSaveAsMultiDlg::~CSaveAsMultiDlg()
{
}

void CSaveAsMultiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_SAVE_AS_MULTI_COMBO_EXY, m_sComboExt);
	DDX_Control(pDX, IDC_SAVE_AS_MULTI_COMBO_EXY, m_comboExt);
	DDX_Control(pDX, IDC_SAVE_AS_MULTI_COMBO_SUFFIX1, m_comboSuffix1);
	DDX_Control(pDX, IDC_SAVE_AS_MULTI_COMBO_SUFFIX2, m_comboSuffix2);
	DDX_Control(pDX, IDC_SAVE_AS_MULTI_COMBO_FILE_NAME, m_comboBase);
	DDX_Control(pDX, IDC_SAVE_AS_MULTI_COMBO_PREFIX1, m_comboPrefix1);
	DDX_Control(pDX, IDC_SAVE_AS_MULTI_COMBO_PREFIX2, m_comboPrefix2);
	DDX_Text(pDX, IDC_SAVE_AS_MULTI_EDIT_DIR, m_sEditDir);
	DDX_CBString(pDX, IDC_SAVE_AS_MULTI_COMBO_PREFIX1, m_sComboPrefix1);
	DDX_CBString(pDX, IDC_SAVE_AS_MULTI_COMBO_PREFIX2, m_sComboPrefix2);
	DDX_CBString(pDX, IDC_SAVE_AS_MULTI_COMBO_FILE_NAME, m_sComboBase);
	DDX_CBString(pDX, IDC_SAVE_AS_MULTI_COMBO_SUFFIX1, m_sComboSuffix1);
	DDX_CBString(pDX, IDC_SAVE_AS_MULTI_COMBO_SUFFIX2, m_sComboSuffix2);
}


BEGIN_MESSAGE_MAP(CSaveAsMultiDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSaveAsMultiDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSaveAsMultiDlg メッセージ ハンドラー


BOOL CSaveAsMultiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_sEditDir=m_sDir;

	m_comboExt.ResetContent();
	m_comboExt.AddString(_T("元ファイルと同じ"));
	m_comboExt.AddString(_T("png"));
	m_comboExt.AddString(_T("bmp"));
	m_comboExt.SetCurSel(0);


	
	m_comboBase.ResetContent();
	m_comboBase.AddString(_T("元ファイルと同じ"));
	m_comboBase.SetCurSel(0);
	m_sComboBase=_T("元ファイルと同じ");
	
	
	m_comboPrefix1.ResetContent();
	m_comboPrefix1.AddString(_T(""));
	m_comboPrefix1.AddString(_T("0埋め0始まり"));
	m_comboPrefix1.AddString(_T("0埋め1始まり"));
	m_comboPrefix1.AddString(_T("埋め無し0始まり"));
	m_comboPrefix1.AddString(_T("埋め無し1始まり"));
	m_comboPrefix1.SetCurSel(0);
	
	m_comboPrefix2.ResetContent();
	m_comboPrefix2.AddString(_T(""));
	m_comboPrefix2.AddString(_T("0埋め0始まり"));
	m_comboPrefix2.AddString(_T("0埋め1始まり"));
	m_comboPrefix2.AddString(_T("埋め無し0始まり"));
	m_comboPrefix2.AddString(_T("埋め無し1始まり"));
	m_comboPrefix2.SetCurSel(0);

	
	m_comboSuffix1.ResetContent();
	m_comboSuffix1.AddString(_T(""));
	m_comboSuffix1.AddString(_T("0埋め0始まり"));
	m_comboSuffix1.AddString(_T("0埋め1始まり"));
	m_comboSuffix1.AddString(_T("埋め無し0始まり"));
	m_comboSuffix1.AddString(_T("埋め無し1始まり"));
	m_comboSuffix1.SetCurSel(0);
	
	m_comboSuffix2.ResetContent();
	m_comboSuffix2.AddString(_T(""));
	m_comboSuffix2.AddString(_T("0埋め0始まり"));
	m_comboSuffix2.AddString(_T("0埋め1始まり"));
	m_comboSuffix2.AddString(_T("埋め無し0始まり"));
	m_comboSuffix2.AddString(_T("埋め無し1始まり"));
	m_comboSuffix2.SetCurSel(0);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

bool IsValidChars(const CString sStr)
{
	if(sStr.Find(_T("/"))>=0){return false;}
	if(sStr.Find(_T("?"))>=0){return false;}
	if(sStr.Find(_T("<"))>=0){return false;}
	if(sStr.Find(_T("?"))>=0){return false;}
	if(sStr.Find(_T("*"))>=0){return false;}
	if(sStr.Find(_T("|"))>=0){return false;}
	if(sStr.Find(_T("\""))>=0){return false;}
	return true;
}

void CSaveAsMultiDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	
	if(IsValidChars(m_sEditDir)==false){AfxMessageBox(_T("無効な文字列が含まれています"));return;}
	if(IsValidChars(m_sComboPrefix1)==false){AfxMessageBox(_T("無効な文字列が含まれています"));return;}
	if(IsValidChars(m_sComboPrefix2)==false){AfxMessageBox(_T("無効な文字列が含まれています"));return;}
	if(IsValidChars(m_sComboBase)==false){AfxMessageBox(_T("無効な文字列が含まれています"));return;}
	if(IsValidChars(m_sComboSuffix1)==false){AfxMessageBox(_T("無効な文字列が含まれています"));return;}
	if(IsValidChars(m_sComboSuffix2)==false){AfxMessageBox(_T("無効な文字列が含まれています"));return;}
	if(IsValidChars(m_sComboExt)==false){AfxMessageBox(_T("無効な文字列が含まれています"));return;}


	m_sDir = m_sEditDir;
	m_sPrefix1 = m_sComboPrefix1;
	m_sPrefix2 = m_sComboPrefix2;
	m_sBase = m_sComboBase;
	m_sSuffix1 = m_sComboSuffix1;
	m_sSuffix2 = m_sComboSuffix2;
	m_sExt = m_sComboExt;


	CDialogEx::OnOK();
}
