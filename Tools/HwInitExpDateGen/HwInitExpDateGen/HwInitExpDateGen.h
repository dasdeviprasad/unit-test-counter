/********************************************************************
HwInitExpDateGen.h

Copyright (C) 2012, 3CPM Company, Inc.  All rights reserved.

  Main header file for the HwInitExpDateGen application.

HISTORY:
07-JAN-12  RET  New.
********************************************************************/

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CHwInitExpDateGenApp:
// See HwInitExpDateGen.cpp for the implementation of this class
//

class CHwInitExpDateGenApp : public CWinApp
{
public:
	CHwInitExpDateGenApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CHwInitExpDateGenApp theApp;