/********************************************************************
OrphanFileRecovery.h

Copyright (C) 2010,2013 3CPM Company, Inc.  All rights reserved.

  main header file for the Orphan File Recovery application.


HISTORY:
28-JUN-10  RET  New.
25-JAN-13  RET
             Add class variable: m_bAutoExit
********************************************************************/

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "float.h"
#include "sysdefs.h"
#include "EGGSASIncludes.h"
#include "Utils.h"

#define	WMUSER_FIND_ORPHAN_FILES WM_USER+1 // Finds orphan files.

//// INI file definitions
  // General section
#define INI_SEC_GENERAL "GENERAL"
#define INI_ITEM_DATAPATH "datapath"

// COrphanFileRecoveryApp:
// See OrphanFileRecovery.cpp for the implementation of this class
//

class COrphanFileRecoveryApp : public CWinApp
{
public:
	COrphanFileRecoveryApp();

public: // Data
  // true - Automatically exit if no orphan data files are found.
  bool m_bAutoExit;

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation
  void vLog(LPCTSTR pstrFmt, ...);

	DECLARE_MESSAGE_MAP()
};

extern COrphanFileRecoveryApp theApp;