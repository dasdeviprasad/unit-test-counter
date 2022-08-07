/********************************************************************
StudyExport.h

Copyright (C) 2008 - 2020, 3CPM Company, Inc.  All rights reserved.

  Main header file for this application

HISTORY:
03-NOV-08  RET  New.
Version 1.02
  07-DEC-08  RET  Add defines: PROGRAM_DIR, INI_FILE
27-NOV-20  RET
            Changes to allow using a remote database.
						  Add defines: INI_FILE_EGGSAS, INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_PATH,
							  INI_FILE_CDDR, INI_FILE_RDR
********************************************************************/

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// Return values from functions or other types of status.
//   More specific failures should be less than 0.
//   More specific successes should be greater than 1.
#define FAIL 0
#define SUCCESS 1
#define SUCCESS_NO_DATA 2

#define INV_LONG 0xFFFFFFFF //ULONG_MAX

// Settings file definitions.
#define SETT_SECTION_GENERAL _T("GENERAL")
#define SETT_ITEM_EXPORT_DIR _T("ExportDir")
#define SETT_ITEM_DATA_PATH _T("datapath")

//// INI file definitions for EGGSAS.INI
#define INI_FILE_EGGSAS _T("C:\\3CPM\\Program\\eggsas.ini")
  // General section
#define INI_SEC_GENERAL _T("GENERAL")
#define INI_ITEM_REMOTE_DB_PATH _T("RemoteDBPath") // Path of the remote database.
#define INI_FILE_CDDR _T("C:\\3CPM\\Central Data Depot Reader\\eggsas.ini")
#define INI_FILE_RDR _T("C:\\3CPM\\Reader\\eggsas.ini")


// Name of subdirectory this program gets installed to.
#define PROGRAM_DIR _T("StudyExport")
// Name of INI file.
#define INI_FILE _T("StudyExport.ini")

// CStudyExportApp:
// See StudyExport.cpp for the implementation of this class
//

class CStudyExportApp : public CWinApp
{
public:
	CStudyExportApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation
public: // Methods
  void vLog(LPCTSTR pstrFmt, ...);
  void vShowHelp();


	DECLARE_MESSAGE_MAP()
};

extern CStudyExportApp theApp;