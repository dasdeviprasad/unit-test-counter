/********************************************************************
StudyExport.cpp

Copyright (C) 2009 - 2020 3CPM Company, Inc.  All rights reserved.

  Defines the class behaviors for the application.

What program does:
  - Provides user with a method of selecting EGG studies.
  - Copy those studies to another database.
  - This second database is primarily used to send the studies to
    another location on another computer.

Program design:
  - Allow the user to select multiple studies
    > Use of a multiple selection listbox with Patient first and last name,
      address, and research number and date of study.
  - Read selected studies from "3CPM EGGSAS" database and write the
    selected studies to the "3CPM EGGSAS EXP" database (export DB).
  - User selects a target drive and directory for the export DB.
  - Copy the "3CPM EGGSAS EMPTY" database to the target directory as
    the export DB.
  - Copy each selected study from the "3CPM EGGSAS" DB to the export DB.

Command line arguments
  There is one command line argument used as follows:
	  - This command line argument is used by this  program to determine where
		  the help file is.
	  - If the command line argument is empty (NULL), this program is running
		  standalone.  The help file will be in the current directory, the directory
			the program was started from.
		- If there is a command line argument, this program was started by another
		  program.  The programs current directory is where the program that started
			it resides.  The command line argument is the directory where this program
			resides and also the help file.

HISTORY:
03-NOV-08  RET  New.
Version 1.03
  05-FEB-09  RET  Don't put a study in the list if the data file doesn't exist.
                  Ask if user wants to continue with the export if there is an error
Version 1.05
  27-JAN-11  RET  Add help for selecting multiple studies.
                     StudyExportDlg.cpp
09-JUN-16  RET Version 1.06
             Add ability to hide study dates if they are encoded.
               StudyExportDlg.cpp
            Update database access methods for the most current database version, 2.06.
              DBAccess.cpp, DBAccess.h
            Fix bug in getting next WhatIf record.  Using wrong statement handle on subsequent
            calls to get next record.
              DBAccess.cpp
25-NOV-17  RET Version 1.07
             Users don't seem to know how to make multiple list box selections
						 (i.e don't know how to use Ctrl Left Click).
						 Changed the Left Click to behave as a Ctrl Left Click.
						 How it works:
						   - The Control key is forced down through software for the duration
							   of the program.  This allows a left click on the listbox to look
							   like a Control Left Click and there allow multiple selections.
							 - If the control key is ever pressed and released, it will be forced
							   down again immediately upon receiving the Control key UP event.
							 - Holding the control key down causes problems when editing the destination
							   field or using the select destination button and after the program is
								 closed. The problems are caused because the control key is still down.
								 Therefore when entering any field other than the listbox or selecting
								 any button the control key is let up throught software and after
								 the button click procedure or the field editing is complete, the
								 control key is forced down again through software.
30-JAN-18  RET Version 1.08
             Reworked multiple selection without using the control key.
						   StudyExportDlg.cpp
19-MAY-18  RET Version 1.09
             Add database search button to main screen.  This allows the user
						 to select studies to export based on names/research reference numbers
						 and/or dates.  Both allows use of wild cards.
						   Change: StudyExportDlg.cpp, DBAccess.cpp, DBAccess.h
							 Add class: CSearchDB 
							   Add files: SearchDB.cpp, SearchDB.h
8-JUL-20  RET  Version 1.10
            ".hlp" files are no longer supported in Win10, change help files to HTML help (chm extension).
               Change method: InitInstance()
            Change calls to WinHelp() to HtmlHelp().
              Change files: StudyExportDlg.cpp
27-NOV-20  RET Version 1.11
            Changes to allow using a remote database.
						  Change files: StudyExport.h, StudyExportDlg.cpp, StudyExportDlg.h,
							  Settings.cpp, DBAccess.cpp
********************************************************************/

#include "stdafx.h"
#include "StudyExport.h"
#include "StudyExportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStudyExportApp

BEGIN_MESSAGE_MAP(CStudyExportApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CStudyExportApp construction

CStudyExportApp::CStudyExportApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CStudyExportApp object

CStudyExportApp theApp;


// CStudyExportApp initialization

BOOL CStudyExportApp::InitInstance()
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

	// Set up the help to use HTML help (.chm file extension).
	TCHAR szCurDir[1024];
	CString cstrCurDir, cstrHelpFile, cstrHelpFilePath;
  cstrHelpFile = "\\StudyExport.chm";
	if(theApp.m_lpCmdLine[0] == NULL)
	{
		GetCurrentDirectory(1024, (LPTSTR)szCurDir);
		cstrCurDir = szCurDir;
		cstrHelpFilePath = cstrCurDir + cstrHelpFile;
	}
	else
	{
		cstrHelpFilePath = theApp.m_lpCmdLine;
		cstrHelpFilePath += cstrHelpFile;
		//CString cstrDebug;
		//cstrDebug.Format(_T("Path from command line: %s, %s"), theApp.m_lpCmdLine, cstrHelpFilePath);
		//vLog((LPCTSTR)cstrDebug);
	}
	free((void*)m_pszHelpFilePath);
	m_pszHelpFilePath = _tcsdup(cstrHelpFilePath);
	//theApp.m_eHelpType = afxHTMLHelp;
	theApp.EnableHtmlHelp();


	CStudyExportDlg dlg;
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
void CStudyExportApp::vLog(LPCTSTR pstrFmt, ...)
  {
  va_list marker;
  CString cstrLog, cstrDate;
  FILE *pfLog;
  SYSTEMTIME SysTime;

  pfLog = fopen("StudyExportlog.txt", "a+");
  if(pfLog != NULL)
    {
    va_start(marker, pstrFmt);  // Initialize variable arguments.

    cstrLog.FormatV(pstrFmt, marker);
    va_end(marker);
      // Insert the current date and time into the message.
    GetLocalTime(&SysTime);
      // Day/Month/Year  Hours:minutes:seconds
    cstrDate.Format(_T("%02d/%02d/%d  %02d:%02d:%02d  "), SysTime.wDay, SysTime.wMonth,
           SysTime.wYear, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
    cstrLog.Insert(0, cstrDate);

    CW2A pszA( (LPCTSTR)cstrLog );

    // Write to the log file.
//    fputs((const char *)(LPCTSTR)cstrLog, pfLog);
    fputs(pszA, pfLog);
    fputs("\n", pfLog);
    fclose(pfLog);
  }
}

void CStudyExportApp::vShowHelp()
{

    OnHelp();
}
/****************************************
  void CStudyExportApp::vLog1(char *pstrFmt, ...)
  {
  va_list marker;
  CString cstrLog, cstrDate;
  FILE *pfLog;
  SYSTEMTIME SysTime;
  char szDate[25];

  pfLog = fopen("StudyExportlog.txt", "a+");
  if(pfLog != NULL)
    {
    va_start(marker, pstrFmt);  // Initialize variable arguments.


    int iLen, iLenDate;
    char *pszBuf;
    iLen = _vscprintf( (const char *)pstrFmt, marker ) // _vscprintf doesn't count
                                + 1; // terminating '\0'
    iLen += (int)strlen((const char *)pstrFmt);
    GetLocalTime(&SysTime);
    iLenDate = sprintf(szDate, "%02d/%02d/%d  %02d:%02d:%02d  ",
                       SysTime.wDay, SysTime.wMonth,
                       SysTime.wYear, SysTime.wHour, SysTime.wMinute, SysTime.wSecond) + 1;

    pszBuf = (char*)malloc( (iLenDate + iLen) * sizeof(char) );

    strcpy(pszBuf, szDate);
    vsprintf( pszBuf + iLenDate - 1, (const char *)pstrFmt, marker ); // C4996
    fputs( pszBuf, pfLog );

    fputs("\n", pfLog);
    fclose(pfLog);
    free( pszBuf );

    }
  }
****************************************/

