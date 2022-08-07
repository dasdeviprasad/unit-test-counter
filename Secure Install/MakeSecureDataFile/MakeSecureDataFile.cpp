/********************************************************************
MakeSecureDataFile.cpp

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Defines the class behaviors for the application.


HISTORY:
17-SEP-10  RET  New. Version 1.0
********************************************************************/

#include "stdafx.h"
#include "..\common\SecureInstall.h"
#include "MakeSecureDataFile.h"
#include "MakeSecureDataFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMakeSecureDataFileApp

BEGIN_MESSAGE_MAP(CMakeSecureDataFileApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMakeSecureDataFileApp construction

CMakeSecureDataFileApp::CMakeSecureDataFileApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMakeSecureDataFileApp object

CMakeSecureDataFileApp theApp;


// CMakeSecureDataFileApp initialization

BOOL CMakeSecureDataFileApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));


    // Use an INI file rather than the Registry. Set up the INI file name
    // and path.  It uses the current directory.
    // The INI file must be in the same directory as the executable program.
  char szIniFile[100];
  GetCurrentDirectory(100, szIniFile); // Path program is in.
  strcat(szIniFile, "\\");
  strcat(szIniFile, "MakeSecureDataFile.ini");
  free((void *)m_pszProfileName);
  m_pszProfileName = _strdup(szIniFile);


	CMakeSecureDataFileDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
