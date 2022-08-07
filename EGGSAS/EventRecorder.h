/********************************************************************
EventRecorder.h

Copyright (C) 2007,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CEventRecorder class.

HISTORY:
06-AUG-07  RET  New.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/
#pragma once


// CEventRecorder dialog

class CEventRecorder : public CDialog
{
	DECLARE_DYNAMIC(CEventRecorder)

public:
	CEventRecorder(int iSeconds, CWnd* pParent = NULL);   // standard constructor
	virtual ~CEventRecorder();

// Dialog Data
	enum { IDD = IDD_EVENT_RECORDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  CString m_cstrDescription;
  BOOL m_bCough;
  BOOL m_bIngestion;
  BOOL m_bMovement;
  BOOL m_bNausea;
  BOOL m_bPain;
  BOOL m_bTalking;
  BOOL m_bVomiting;
  BOOL m_bOther;
  CString m_cstrTime;

  CString cstrGet_description(void);
  CString cstrGet_indications(void);
  void vSet_data(CString cstrIndications, CString cstrDescription);
  void vSet_text(void);

  afx_msg void OnBnClickedOk();
  virtual BOOL OnInitDialog();
};
