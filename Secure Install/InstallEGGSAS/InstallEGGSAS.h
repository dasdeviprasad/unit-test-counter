/********************************************************************
InstallEGGSAS.h

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  main header file for the Validate Install application.


HISTORY:
17-SEP-10  RET  New.
********************************************************************/

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "..\common\SecureInstall.h"

// Defines for program exit codes.
#define EXIT_CODE_OK 0 // OK to install.
#define EXIT_CODE_PASSWORD_INVALID 1 // Don't install, password invalid.
#define EXIT_CODE_USER_CANCELLED 2 // Don't Install, user cancelled
#define EXIT_CODE_FILE_ERROR 3 // Don't Install, Secure data file doesn't exist or file error.

// CInstallEGGSASApp:
// See InstallEGGSAS.cpp for the implementation of this class
//

class CInstallEGGSASApp : public CWinApp
{
public:
	CInstallEGGSASApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
//  virtual int ExitInstance();
};

extern CInstallEGGSASApp theApp;