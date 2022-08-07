/********************************************************************
InstallEGGSAS.cpp

Copyright (C) 2010,2012, 3CPM Company, Inc.  All rights reserved.

  Defines the class behaviors for the application.

  Operation:
    - Both the program, validate.exe, and the secure data file, ValidateInstall.txt,
      must be present on the installation thumb drive.
    - Start from the installation program.
      . The program looks for a command line argument ("AP2-JK4;") which means
        it was started from the installation program.
      . If the user tries to double click on this program it will display an
        error message and won't run.
    - Ask user for a password, 3cpm, all lower case.
    - When the user clicks on the "Install" button, validate the password using
      the file ValidateInstall.txt. This has the password "3cpm" encoded into it.
    - If the password is valid:
      . Change the password in the secure data file to that of the MAC address.
      . Create a password OK file on the thumb drive indicating that the password is OK.
      . When this program exits, the installation program looks for this file and if
        found, deletes the file and continues with the installation.
    - If the password is invalid or the user cancels, the password OK file is not
      created and the installation program will abort.


HISTORY:
17-SEP-10  RET  New. Version 1.0
18-SEP-12  RET
             Change to allow 3 tries to enter the correct password.
               InstallEGGSASDlg.cpp/InstallEGGSASDlg.h
             Fix problem in validating the password. It used to compare the
               password from the validate text file with what the password
               is supposed to be not what the user entered.
                 InstallEGGSASDlg.cpp/InstallEGGSASDlg.h,
                 SecureInstall.cpp/SecureInstall.h, Encrypt.cpp/Encrypt.h
********************************************************************/

#include "stdafx.h"
#include "InstallEGGSAS.h"
#include "InstallEGGSASDlg.h"
#include ".\installeggsas.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CInstallEGGSASApp

BEGIN_MESSAGE_MAP(CInstallEGGSASApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CInstallEGGSASApp construction

CInstallEGGSASApp::CInstallEGGSASApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CInstallEGGSASApp object

CInstallEGGSASApp theApp;


// CInstallEGGSASApp initialization

BOOL CInstallEGGSASApp::InitInstance()
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
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CInstallEGGSASDlg dlg;
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

