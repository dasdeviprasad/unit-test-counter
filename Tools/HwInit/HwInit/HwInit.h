/********************************************************************
HwInit.h

Copyright (C) 2012, 3CPM Company, Inc.  All rights reserved.

  Main header file for the HwInit application.

HISTORY:
07-JAN-12  RET  New.
********************************************************************/

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#define EGGSASINIPATH "c:\\3cpm\\program\\eggsas.ini"

// CHwInitApp:
// See HwInit.cpp for the implementation of this class
//

class CHwInitApp : public CWinApp
{
public:
	CHwInitApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CHwInitApp theApp;