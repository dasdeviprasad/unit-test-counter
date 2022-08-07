/********************************************************************
SetupRemoteDB.cpp

Copyright (C) 2020 3CPM Company, Inc.  All rights reserved.

  Defines the class behaviors for the application.

	This describes how the password is encoded into a double GUID.
          1  1    1 2  2      3    33   4    4    5    5    6         7 7
01234567890  3    8 0  3      0    56   0    5    0    5    0         0 2
12979280-16d8-4e74-97d3-837d540f8021-c295a5e8-c488-4792-ac8c-17846e422161


                    byte 1   byte 2
Password: 
	password length     21       7
	char 1              4        1
	char 2              10       32
	char 3              31       19
	char 4              38       11
	char 5              34       2
	char 6              48       6
	char 7              17       29
	char 8              24       15
	char 9              33       61
	char 10             12       56
	char 11             70       25
	char 12             20       41
	char 13             5        27
	char 14             16       30
	char 15             9        35
				               
HISTORY:
31-OCT-20  RET  New.  Version 1.0.0.1
22-NOV-20  RET  Version 1.0.0.2
            Change the remote database path written to eggsas.ini to include
						the database file name.
						  Change files: SetupRemoteDBDlg.cpp, SetupRemoteDBDlg.h
********************************************************************/

#include "stdafx.h"
#include "SetupRemoteDB.h"
#include "SetupRemoteDBDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int g_iPasswdLenByt1 = 21;
int g_iPasswdLenByt2 = 7;
int g_iPasswdIndexByt1[15] = 
       {4, 10, 31, 38, 34, 48, 17, 24, 33, 12, 70, 20, 5, 16, 9};
int g_iPasswdIndexByt2[15] = 
       {1, 32, 19, 11, 2, 6, 29, 15, 61, 56, 25, 41, 27, 30, 35};
CString g_cstrDefaultPassword = _T("Kw82$pX#6t");

//CDBAccess *m_pDBA; // Pointer to CDBAccess object for the local database.

// CSetupRemoteDBApp

BEGIN_MESSAGE_MAP(CSetupRemoteDBApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSetupRemoteDBApp construction

CSetupRemoteDBApp::CSetupRemoteDBApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSetupRemoteDBApp object

CSetupRemoteDBApp theApp;


// CSetupRemoteDBApp initialization

BOOL CSetupRemoteDBApp::InitInstance()
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

	m_pfLog = NULL;

	CSetupRemoteDBDlg dlg;
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
void CSetupRemoteDBApp::vLog(LPCTSTR pstrFmt, ...)
  {
  va_list marker;
  CString cstrLog, cstrDate;
  SYSTEMTIME SysTime;

	if(m_pfLog == NULL)
    m_pfLog = fopen(LOG_FILE, "w");
	else
    m_pfLog = fopen(LOG_FILE, "a");
  if(m_pfLog != NULL)
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
    fputs(pszA, m_pfLog);
    fputs("\n", m_pfLog);
    fclose(m_pfLog);
  }
}

/********************************************************************
bGet_version_info - Get the product and program information from the resource
                    file.

    inputs: Reference to a CString object containing the program name and
              extension.
            Reference to a CString object specifying what data item to get
              from the resource file.
            Reference to a CString object which gets the resource file
              text string.

    return: TRUE if the information was retrieved from the resource file.
            FALSE if an error occurred.  If there is an error, the last
              input is empty.
********************************************************************/
bool CSetupRemoteDBApp::bGet_version_info(CString &cstrProgName, wchar_t *pszInfoType, CString &cstrInfo)
  {
  CString cstrTranslation, cstrStringFileInfo, cstrFmt;
	unsigned long lTemp, lInfoSize;
  unsigned char *pucVersionInfo;
  void *pvBuffer;
  unsigned int uiLen;
  unsigned long ulLangCodePage;
  bool bRet = TRUE;

  pucVersionInfo = NULL;
  cstrTranslation = "\\VarFileInfo\\Translation";
  if((lInfoSize = GetFileVersionInfoSize(cstrProgName, &lTemp)) != 0L)
    {
    if((pucVersionInfo = (unsigned char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, lInfoSize)) != NULL)
      {
      if( GetFileVersionInfo(cstrProgName, 0L,  lInfoSize, pucVersionInfo) != 0)
        {
        if( VerQueryValue(pucVersionInfo, cstrTranslation.GetBuffer(), &pvBuffer, &uiLen) != 0)
          {
          ulLangCodePage = *((unsigned long *)pvBuffer);
          // high word is code page, low word is language.
          cstrFmt = "\\StringFileInfo\\%04x%04x\\%s";
          cstrStringFileInfo.Format(cstrFmt, LOWORD(ulLangCodePage), 
            HIWORD(ulLangCodePage), pszInfoType);
          if(VerQueryValue(pucVersionInfo, cstrStringFileInfo.GetBuffer(), &pvBuffer, 
            &uiLen) != 0)
            cstrInfo = (wchar_t *)pvBuffer;
          else
            bRet = FALSE;
          }
        else
          bRet = FALSE;
        }
      else
        bRet = FALSE;
      }
    else
      bRet = FALSE;
    }
  else
    bRet = FALSE;
	if(bRet == FALSE)
	{
		wchar_t szMsg[256];
		DWORD dw = GetLastError(); 
		CString cstrErrMsg, cstrErr;
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0L, szMsg, 256, NULL);
		cstrErrMsg.Format(_T("About: GetFileVersionInfoSize() failed: %s"), szMsg);
		cstrErr.LoadString(IDS_ERROR);
		theApp.m_pActiveWnd->MessageBox(cstrErrMsg, cstrErr, MB_OK);
	}
  if(pucVersionInfo != NULL)
    HeapFree(GetProcessHeap(), 0, (LPVOID)pucVersionInfo);
  return(bRet);
  }
