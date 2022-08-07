/********************************************************************
DataFileViewer.h

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  main header file for the Data File Viewer application.


HISTORY:
25-JUN-10  RET  New.
********************************************************************/

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "sysdefs.h"
#include "EGGSASIncludes.h"
#include "Utils.h"

//// INI file definitions
  // General section
#define INI_SEC_GENERAL "GENERAL"
#define INI_ITEM_DATAPATH "datapath"


// CDataFileViewerApp:
// See DataFileViewer.cpp for the implementation of this class
//

class CDataFileViewerApp : public CWinApp
{
public:
	CDataFileViewerApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation
  void vLog(LPCTSTR pstrFmt, ...);

	DECLARE_MESSAGE_MAP()
};

extern CDataFileViewerApp theApp;