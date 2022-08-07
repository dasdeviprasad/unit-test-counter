/********************************************************************
EventList.h

Copyright (C) 2007,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CEventList class.

HISTORY:
08-AUG-07  RET  New.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#pragma once
#include "afxwin.h"


// CEventList dialog

class CEventList : public CDialog
{
	DECLARE_DYNAMIC(CEventList)

public:
	CEventList(CEGGSASDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CEventList();

  CEGGSASDoc *m_pDoc; // Pointer to document.

  CString cstrFmt_event_line(CEvnt *pEvent);
  void vSet_text(void);

// Dialog Data
	enum { IDD = IDD_EVENT_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnLbnSelchangeListEvents();
  virtual BOOL OnInitDialog();
  CListBox m_cEventList;
  CString m_cstrDescription;
};
