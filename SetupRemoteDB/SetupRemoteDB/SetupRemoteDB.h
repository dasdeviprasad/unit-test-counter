/********************************************************************
SetupRemoteDB.h

Copyright (C) 2020 3CPM Company, Inc.  All rights reserved.

 main header file for the SetupRemoteDB application

HISTORY:
31-OCT-20  RET  New.  Version 1.0.0.1
********************************************************************/

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "DBAccess.h"


// Return values from functions or other types of status.
//   More specific failures should be less than 0.
//   More specific successes should be greater than 1.
#define FAIL 0
#define SUCCESS 1
#define SUCCESS_NO_DATA 2

#define INV_LONG 0xFFFFFFFF //ULONG_MAX

#define LOG_FILE "RemoteDBSetuplog.txt" // Name of the log file.

// CSetupRemoteDBApp:
// See SetupRemoteDB.cpp for the implementation of this class
//

class CSetupRemoteDBApp : public CWinApp
{
public:
	CSetupRemoteDBApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation
public: // Data
	  FILE *m_pfLog; // Pointer to file ID for the log file.

public: // Methods
  void vLog(LPCTSTR pstrFmt, ...);
	void vSet_ini_file();
  bool bGet_version_info(CString &cstrProgName, wchar_t *pszInfoType, CString &cstrInfo);

	DECLARE_MESSAGE_MAP()
};

extern CSetupRemoteDBApp theApp;
