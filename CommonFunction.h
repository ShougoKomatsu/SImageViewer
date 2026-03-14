#include "stdafx.h"
#define SAFE_DELETE(arg) if(arg != NULL){delete arg; arg=NULL;}
bool CopyToClipBoardStr(const CString sValue);