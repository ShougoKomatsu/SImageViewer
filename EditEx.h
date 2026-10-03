#include "stdafx.h"

#pragma once

class CEditEx: public CEdit
{
public:
	void SelectAll();
void SetCursorAtSelectedLast();
void SetCursorAtSelectedFirst();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};