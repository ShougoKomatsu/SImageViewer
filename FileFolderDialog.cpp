#include "stdafx.h"
#include "FileFolderDialog.h"

UINT_PTR CALLBACK OpenFileHookProc(
	HWND hdlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			OPENFILENAME* pOFN =(OPENFILENAME*)lParam;
			SetWindowLongPtr(hdlg,GWLP_USERDATA,(LONG_PTR)pOFN->lCustData);
			return 0;
		}
	case WM_NOTIFY:
		{
			OFNOTIFY* pNotify =(OFNOTIFY*)lParam;

			if( (pNotify->hdr.code == CDN_INITDONE) || (pNotify->hdr.code == CDN_FOLDERCHANGE))
			{
				LPCTSTR* ptchDefault =(LPCTSTR*)GetWindowLongPtr(hdlg,GWLP_USERDATA);
				if (ptchDefault == NULL){break;}

				HWND hParent =GetParent(hdlg);
				CommDlg_OpenSave_SetControlText(hParent,edt1,*ptchDefault);
			}
			return 0;
		}
	}
	return 0;
}


	void SplitFilePath(const TCHAR* tchBuf, CStringArray* saFiles)
	{
		saFiles->RemoveAll();
		LPCTSTR p = tchBuf;
		CString sFirst = p;
		p += sFirst.GetLength() + 1;

		if ( (*p) == '\0')
		{
			if(sFirst.Right(16).Compare(_T(".FolderSelection"))==0)
			{
				saFiles->Add(sFirst.Left(sFirst.GetLength()-17));
			}
			else
			{
				saFiles->Add(sFirst);
			}
			return;
		}

		CString sFolder = sFirst;

		if (sFolder.Right(1).Compare(_T("\\")) != 0){sFolder += _T("\\") ;}

		while ( (*p) != '\0')
		{
			CString sFileName = p;
			saFiles->Add(sFolder + sFileName);
			p += sFileName.GetLength() + 1;
		}
	}

	bool GetOpenFileList(CString* sFilePaths, CWnd* wnd)
	{
		TCHAR* tchBuf=NULL;
		tchBuf = new TCHAR[1000*MAX_PATH];
		ZeroMemory(tchBuf, 1000*MAX_PATH*sizeof(TCHAR));
		_stprintf(tchBuf, _T(".FolderSelection"));
		
		const LPCTSTR pszDefaultFile = _T(".FolderSelection");
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner   = wnd->m_hWnd;
		ofn.lpstrFile	= tchBuf;
		ofn.nMaxFile    = 1000;
		ofn.Flags		= OFN_EXPLORER |OFN_ENABLEHOOK |OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT;
		ofn.lpfnHook    = OpenFileHookProc;
		ofn.lCustData   = (LPARAM)&pszDefaultFile;
		ofn.lpstrFilter = _T("All Files (*.*)\0*.*\0\0");

		BOOL bRet = GetOpenFileName(&ofn);
		if(bRet != TRUE){SAFE_DELETE(tchBuf); return false;}

		CStringArray saFiles;

		SplitFilePath(tchBuf, &saFiles);
		SAFE_DELETE(tchBuf); 


		(*sFilePaths)=_T("");
		if(saFiles.GetCount()<=0){return false;}
		for(int i=0; i<saFiles.GetCount(); i++)
		{
			CString sTemp;
			sTemp.Format(_T("%s|"),saFiles.GetAt(i));
			sFilePaths->Append(sTemp);
		}
		sFilePaths->Delete(sFilePaths->GetLength()-1);

		return true;
	}