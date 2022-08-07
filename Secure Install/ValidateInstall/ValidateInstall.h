// ValidateInstall.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CValidateInstallApp:
// See ValidateInstall.cpp for the implementation of this class
//

class CValidateInstallApp : public CWinApp
{
public:
	CValidateInstallApp();

// Overrides
	public:
	virtual BOOL InitInstance();
  virtual int ExitInstance( );

// Implementation
public:
  int m_iExitCode;

	DECLARE_MESSAGE_MAP()
};

extern CValidateInstallApp theApp;