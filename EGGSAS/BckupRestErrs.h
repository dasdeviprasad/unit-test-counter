/********************************************************************
BckupRestErrs.h

Copyright (C) 2009,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CBckupRestErrs class.

HISTORY:
22-MAR-09  RET  New. Version 2.08k
16-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#pragma once
#include "afxwin.h"


// CBckupRestErrs dialog

class CBckupRestErrs : public CDialog
{
	DECLARE_DYNAMIC(CBckupRestErrs)

public:
	CBckupRestErrs(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBckupRestErrs();

// Dialog Data
	enum { IDD = IDD_BCKUP_REST_ERRS };

  CBckupRest *m_pBckupRest; // Pointer to the calling object (backup/restore).

  void vAdd_to_msg_list(CString cstrLine);
  void vSet_text(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  CListBox m_cBckupRestList;
  virtual BOOL OnInitDialog();
};
