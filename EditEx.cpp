#include "stdafx.h"
#include "EditEx.h"

void CEditEx::SelectAll()
{
	SetSel(0, -1);
}

void CEditEx::SetCursorAtSelectedFirst()
{
	int iStart;
	int iEnd;
	GetSel(iStart, iEnd);
	if(iStart==iEnd){iStart--;}
	if(iStart<0){iStart=0;}
	SetSel(iStart, iStart);
}

void CEditEx::SetCursorAtSelectedLast()
{
	int iStart;
	int iEnd;
	GetSel(iStart, iEnd);
	if(iStart==iEnd){iEnd++;}
	if(iEnd<0){iEnd=0;}
	SetSel(iEnd, iEnd);
}


BOOL CEditEx::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{	
		if(GetKeyState(VK_CONTROL)<0)
		{	
			if(pMsg->wParam == 'A'){SelectAll();return TRUE;}
		}
		
		if((GetKeyState(VK_CONTROL)>=0) && (GetKeyState(VK_SHIFT)>=0) && (GetKeyState(VK_MENU)>=0))
		{	
			if(pMsg->wParam == VK_LEFT){SetCursorAtSelectedFirst();return TRUE;}
			if(pMsg->wParam == VK_RIGHT){SetCursorAtSelectedLast();return TRUE;}
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}
