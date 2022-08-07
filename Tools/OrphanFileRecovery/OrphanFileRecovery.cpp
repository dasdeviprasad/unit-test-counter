/********************************************************************
OrphanFileRecovery.cpp

Copyright (C) 2010 - 2020 3CPM Company, Inc.  All rights reserved.

  Defines the class behaviors for the application.

  Command line parameters:
    /E  -  Causes the program to automatically exit if there are no orphan
           data files.  This is to be used by EGGSAS on start up.

HISTORY:
28-JUN-10  RET  New.
25-JAN-13  RET
             Add looking for command parameter "/E".
               Change method: InitInstance()
               OrphanFileRecoveryDlg.cpp, OrphanFileRecovery.h
             Release 1.1
06-MAR-13  RET
             Remove error message if no orphan files are found and disable the
             add selection to database button.
             Disable the add selection to database button afterf adding a file to
             the database if there are no more files in the list.
               OrphanFileRecoveryDlg.cpp
             Release 1.2
08-JUN-16  RET
             Fix program crash if user selects add to database without selecting
             a study.
               English.h, OrphanFileRecoveryDlg.cpp
             Add hide study date if the study date is encrypted.
               OrphanFileRecoveryDlg.cpp
23-NOV-20  RET Version 1.3
             Add ability to work with a remote database.
               Change files: OrphanFileRecoveryDlg.cpp, OrphanFileRecoveryDlg.h
********************************************************************/

#include "stdafx.h"
#include "OrphanFileRecovery.h"
#include "Dbaccess.h"
#include "OrphanFileRecoveryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// COrphanFileRecoveryApp

BEGIN_MESSAGE_MAP(COrphanFileRecoveryApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// COrphanFileRecoveryApp construction

COrphanFileRecoveryApp::COrphanFileRecoveryApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only COrphanFileRecoveryApp object

COrphanFileRecoveryApp theApp;


// COrphanFileRecoveryApp initialization

BOOL COrphanFileRecoveryApp::InitInstance()
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
  strcat(szIniFile, "OrphanFileRecovery.ini");
  free((void *)m_pszProfileName);
  m_pszProfileName = _strdup(szIniFile);

  // Look for the command line parameter to do an auto exit if no
  // orphan files are found.
  CString cstrCmdLine = m_lpCmdLine;
  if(cstrCmdLine.Find("/E") >= 0)
    m_bAutoExit = true;
  else
    m_bAutoExit = false;

	COrphanFileRecoveryDlg dlg;
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


/********************************************************************
vLog - Record errors and other activity in the log file.

  inputs: Constant string object containing a message and any
            formatting codes.
          Any arguments the message string requires for formatting.
   
  return: None.
********************************************************************/
void COrphanFileRecoveryApp::vLog(LPCTSTR pstrFmt, ...)
  {
  va_list marker;
  CString cstrLog, cstrDate;
  FILE *pfLog;
  SYSTEMTIME SysTime;

  pfLog = fopen("OrphanFileRecoveryLog.txt", "a+");
  if(pfLog != NULL)
    {
    va_start(marker, pstrFmt);  // Initialize variable arguments.
    cstrLog.FormatV(pstrFmt, marker);
    va_end(marker);
      // Insert the current date and time into the message.
    GetLocalTime(&SysTime);
      // Day/Month/Year  Hours:minutes:seconds
    cstrDate.Format("%02d/%02d/%d  %02d:%02d:%02d  ", SysTime.wDay, SysTime.wMonth,
           SysTime.wYear, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
    cstrLog.Insert(0, cstrDate);
      // Write to the log file.
    fputs(cstrLog, pfLog);
    fputs("\n", pfLog);
    fclose(pfLog);
    }
  }
