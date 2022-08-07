/********************************************************************
dbupdate.cpp

Copyright (C) 2005,2008,2012,2019, 3CPM Company, Inc.  All rights reserved.

  Defines the class behaviors for the application.

  Version number:
    a.b.c.d
     a.b is program version.
     c.d is database version.

Command line parameters.
 - The path and name of the database to update.
      If this is missing, the standard database (specified by the DSN) is updated.

HISTORY:
28-FEB-05  RET  New.
06-AUG-07  RET  Program version 1.2, Database version 2.01
                  Add Events table.
29-AUG-08  RET  Version 2.03 of the database.
                  Move Research Reference fields from PatientInfo table
                    to Patient table.
02-SEP-12  RET  Version 2.04 of the database.
             Add tables: DiagnosisDesc, ICDCode, Recommendations
             Modify StudyData table:
               Add fields: DiagKeyPhrase, ICDCode, RecommendationKeyPhrase
13-SEP-12  RET  Version 2.04 of the database.
                  Add diagnosis key phrases and descriptions.
                  Add ICD codes.
                  Add columns to the StudyData table for new indications.
27-FEB-13  RET  Version 2.05 of the database.
                  Add column to the Patient table to hide patients.
                  Add columns to the PatientInfo table for male and female designations.
                    Dbaccess.cpp/Dbaccess.h, dbupdateDlg.cpp
16-MAR-13  RET  Version 2.06 of the database.
                  Add column to the StudyData table to record the frequency.
                  Add columns to the WhatIf table to record the frequency.
                    Dbaccess.cpp/Dbaccess.h, dbupdateDlg.cpp
22-JAN-19  RET  DB Ver 2.06, product ver 1.6
                  Add ability to specify a different database on the  command line.
                    Add global variable: g_cstrDBFile
                    Change method: InitInstance() to save a file name specified on the command line.
                    Change files: Dbaccess.cpp, Dbaccess.h, dbupdateDlg.cpp
********************************************************************/

#include "stdafx.h"
#include "dbupdate.h"
#include "sysdefs.h"
#include "Dbaccess.h"
#include "dbupdateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CdbupdateApp

BEGIN_MESSAGE_MAP(CdbupdateApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CdbupdateApp construction

CdbupdateApp::CdbupdateApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CdbupdateApp object

CdbupdateApp theApp;

// Path and name of database to be updated from the command line.
//   If not specifed on the command line, this is empty.
CString g_cstrDBFile;


// CdbupdateApp initialization

BOOL CdbupdateApp::InitInstance()
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

  g_cstrDBFile.Empty();
	// Parse command line for standard shell commands, DDE, file open
  // and the path and name of the database to update.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

  //AfxGetApp()->m_pActiveWnd->MessageBox(cmdInfo.m_strFileName);

  // If a database is specified on the commannd line, save it.
  if(cmdInfo.m_strFileName.IsEmpty() == false)
    g_cstrDBFile = cmdInfo.m_strFileName;


	CdbupdateDlg dlg;
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
void CdbupdateApp::vLog(LPCTSTR pstrFmt, ...)
  {
  va_list marker;
  CString cstrLog, cstrDate;
  FILE *pfLog;
  SYSTEMTIME SysTime;

  pfLog = fopen("dbupdatelog.txt", "a+");
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

/********************************************************************
cstrGet_sys_err_msg - Get the system error message for the error that
                      just occurred.

  inputs: None.

  output: None.

  return: CString object containing the system error message.
********************************************************************/
CString CdbupdateApp::cstrGet_sys_err_msg()
  {
  CString cstrSysErrMsg;
  char szMsg[256];

  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0L, szMsg, 256, NULL);
  cstrSysErrMsg = szMsg;
  return(cstrSysErrMsg);
  }
