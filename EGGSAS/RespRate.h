/********************************************************************
RespRate.h

Copyright (C) 2003,2004,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CRespRate class.

HISTORY:
11-DEC-03  RET  New.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#if !defined(AFX_RESPRATE_H__B92216A7_F330_4148_B950_021BFA90BD51__INCLUDED_)
#define AFX_RESPRATE_H__B92216A7_F330_4148_B950_021BFA90BD51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRespRate dialog

class CRespRate : public CDialog
{
// Construction
public:
	CRespRate(int iWinTop, unsigned short uPeriod, CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRespRate)
	enum { IDD = IDD_RESPIRATION };
	UINT	m_uRespRate;
	CString	m_cstrPeriod;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRespRate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CWnd *m_pcwndParent; // Pointer to the window that started this dialog box.
  int m_iWinTop; // top of this dialog box.
  unsigned short m_uPeriod; // The 10 minute period for respiration.

public: // Support Methods
  short unsigned uGet_resp_rate();
  void vDisplay_period();
  void vSet_period(unsigned short uPeriod);
  void vSet_text(void);

protected:

	// Generated message map functions
	//{{AFX_MSG(CRespRate)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESPRATE_H__B92216A7_F330_4148_B950_021BFA90BD51__INCLUDED_)
