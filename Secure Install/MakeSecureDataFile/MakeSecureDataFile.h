/********************************************************************
MakeSecureDataFile.h

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  main header file for the Make Secure Data File application.


HISTORY:
17-SEP-10  RET  New.
********************************************************************/

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

//// INI file definitions for this program's INI file.
  // General section
#define INI_SEC_GENERAL "GENERAL"
#define INI_ITEM_PASSWORD "password"


// CMakeSecureDataFileApp:
// See MakeSecureDataFile.cpp for the implementation of this class
//

class CMakeSecureDataFileApp : public CWinApp
{
public:
	CMakeSecureDataFileApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMakeSecureDataFileApp theApp;