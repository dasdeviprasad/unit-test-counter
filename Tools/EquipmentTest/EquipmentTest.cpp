/********************************************************************
EquipmentTest.cpp

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Defines the class behaviors for the application.


HISTORY:
17-AUG-10  RET  New. Version 1.0
03-SEP-10  RET  Version 1.1
                  Add verification that the Respiration belt is operating OK.
                  Remove reference to EGGSAS INI file and put the need items
                    from that INI file into this programs INI file.
********************************************************************/

#include "stdafx.h"
#include "EquipmentTest.h"
#include "EquipmentTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEquipmentTestApp

BEGIN_MESSAGE_MAP(CEquipmentTestApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CEquipmentTestApp construction

CEquipmentTestApp::CEquipmentTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CEquipmentTestApp object

CEquipmentTestApp theApp;


// CEquipmentTestApp initialization

BOOL CEquipmentTestApp::InitInstance()
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

  //CString cstrPath;
    // Use an INI file rather than the Registry. Set up the INI file name
    // and path.  It uses the current directory.
    // The INI file must be in the same directory as the executable program.
  char szIniFile[100];
  GetCurrentDirectory(100, szIniFile); // Path program is in.
  strcat(szIniFile, "\\");
  strcat(szIniFile, "EquipmentTest.ini");
  free((void *)m_pszProfileName);
  m_pszProfileName = _strdup(szIniFile);
  //// Get the data file path from the INI file.
  //// Then create the program path from this and use the EGGSAS INI
  //// file for the equipment driver.
  //cstrPath = GetProfileString(INI_SEC_GENERAL, INI_ITEM_DATAPATH, "");
  //cstrPath += "\\Program";
  //strcpy(szIniFile, cstrPath);
  //strcat(szIniFile, "\\");
  //strcat(szIniFile, "eggsas.ini");
  //free((void *)m_pszProfileName);
  //m_pszProfileName = _strdup(szIniFile);


	CEquipmentTestDlg dlg;
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
void CEquipmentTestApp::vLog(LPCTSTR pstrFmt, ...)
  {
  //va_list marker;
  //CString cstrLog, cstrDate;
  //FILE *pfLog;
  //SYSTEMTIME SysTime;

  //pfLog = fopen("eggsaslog.txt", "a+");
  //if(pfLog != NULL)
  //  {
  //  va_start(marker, pstrFmt);  // Initialize variable arguments.
  //  cstrLog.FormatV(pstrFmt, marker);
  //  va_end(marker);
  //    // Insert the current date and time into the message.
  //  GetLocalTime(&SysTime);
  //    // Day/Month/Year  Hours:minutes:seconds
  //  cstrDate.Format("%02d/%02d/%d  %02d:%02d:%02d  ", SysTime.wDay, SysTime.wMonth,
  //         SysTime.wYear, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
  //  cstrLog.Insert(0, cstrDate);
  //    // Write to the log file.
  //  fputs(cstrLog, pfLog);
  //  fputs("\n", pfLog);
  //  fclose(pfLog);
  //  }
  }

