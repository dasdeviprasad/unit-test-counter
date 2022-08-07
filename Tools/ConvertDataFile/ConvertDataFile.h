// ConvertDataFile.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "sysdefs.h"
#include "EGGSASIncludes.h"
#include "resource.h"		// main symbols


// CConvertDataFileApp:
// See ConvertDataFile.cpp for the implementation of this class
//

class CConvertDataFileApp : public CWinApp
{
public:
	CConvertDataFileApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CConvertDataFileApp theApp;