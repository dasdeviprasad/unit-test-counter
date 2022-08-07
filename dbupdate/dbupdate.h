/********************************************************************
dbupdate.h

Copyright (C) 2005,2007,2008, 3CPM Company, Inc.  All rights reserved.

  Main header file for the CdbupdateApp application

HISTORY:
28-FEB-05  RET  New.
07-FEB-08  RET  Version 2.02 of the database.
                  Change version number.
29-AUG-08  RET  Version 2.03 of the database.
                  Change version number.
29-AUG-08  RET  Version 2.03 of the database.
                  Change version number.
02-SEP-12  RET  Version 2.04 of the database.
                  Change version number.
27-FEB-13  RET  Version 2.05 of the database.
                  Change version number.
16-MAR-13  RET  Version 2.06 of the database.
                  Change version number.
********************************************************************/

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


#define DB_VERSION_MAJOR 2
#define DB_VERSION_MINOR 6 //3

//// EGGSAS INI file definitions
  // General section
#define INI_SEC_GENERAL "GENERAL"
#define INI_ITEM_DATAPATH "datapath"


// CdbupdateApp:
// See dbupdate.cpp for the implementation of this class
//

class CdbupdateApp : public CWinApp
{
public:
	CdbupdateApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation
  void vLog(LPCTSTR pstrFmt, ...);
  CString cstrGet_sys_err_msg();


	DECLARE_MESSAGE_MAP()
};

extern CdbupdateApp theApp;