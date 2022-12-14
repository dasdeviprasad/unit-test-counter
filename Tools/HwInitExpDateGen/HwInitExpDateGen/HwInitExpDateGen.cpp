/********************************************************************
HwInitExpDateGen.cpp

Copyright (C) 2012, 3CPM Company, Inc.  All rights reserved.

  Defines the class behaviors for the application.

  NOTES:
		This program allows for the creation of a file that contains an expiration date and
    the number of times the program that resets the hardware binding for EGGSAS is allowed to run.
    If this number of times is left at 0 (the default value), it is not used. If this value is
    not used it is set to 0xFFF

    The file consists of 4 lines of GUIDs.  See HwInitExpDateGenDlg.h for the details of the
    file and how items are encoded into it.


HISTORY:
07-JAN-12  RET  New.
********************************************************************/

#include "stdafx.h"
#include "HwInitExpDateGen.h"
#include "HwInitExpDateGenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHwInitExpDateGenApp

BEGIN_MESSAGE_MAP(CHwInitExpDateGenApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CHwInitExpDateGenApp construction

CHwInitExpDateGenApp::CHwInitExpDateGenApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CHwInitExpDateGenApp object

CHwInitExpDateGenApp theApp;


// CHwInitExpDateGenApp initialization

BOOL CHwInitExpDateGenApp::InitInstance()
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
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CHwInitExpDateGenDlg dlg;
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
