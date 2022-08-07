/********************************************************************
HwInit.cpp

Copyright (C) 2012,2021, 3CPM Company, Inc.  All rights reserved.

  Defines the class behaviors for the application.

  NOTES:
		This program reads the file created by HwInitExpDateGen program containsing
    the expiration date and the number of times the program that resets the hardware
    binding for EGGSAS is allowed to run.

    If the expiration date hasn't been reached, and if used, the number of times the program
    is allowed to run is > 0, the EGGSAS.INI file is modifed to allow the re-binding of the
    software-hardware.

HISTORY:
07-JAN-12  RET  New.
18-JAN-21  RET
             Change to ignore the date file and the date.  This now will
             unconditionally set the device as not initialized in the
             EGGSAS.INI file.
********************************************************************/

#include "stdafx.h"
#include "HwInit.h"
#include "HwInitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHwInitApp

BEGIN_MESSAGE_MAP(CHwInitApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CHwInitApp construction

CHwInitApp::CHwInitApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CHwInitApp object

CHwInitApp theApp;


// CHwInitApp initialization

BOOL CHwInitApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

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
  //char szIniFile[100];
  //GetCurrentDirectory(100, szIniFile); // Path program is in.
  //strcat(szIniFile, "\\");
  //char *pszPath = EGGSASINIPATH;
  //int iLen = strlen(pszPath);
  //strcpy_s(szIniFile, 100, EGGSASINIPATH);
  //strcat(szIniFile, "EGGSAS.ini");
  free((void *)m_pszProfileName);
  m_pszProfileName = (LPCTSTR)_wcsdup(_T(EGGSASINIPATH)); //szIniFile);

	CHwInitDlg dlg;
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
