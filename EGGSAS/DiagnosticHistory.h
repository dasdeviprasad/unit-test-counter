/********************************************************************
DiagnosticHistory.h

Copyright (C) 2003,2004,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CDiagnosticHistory class.

HISTORY:
03-OCT-03  RET  New.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#if !defined(AFX_DIAGNOSTICHISTORY_H__3C952930_B34F_402C_BFBA_D144E1C71B60__INCLUDED_)
#define AFX_DIAGNOSTICHISTORY_H__3C952930_B34F_402C_BFBA_D144E1C71B60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDiagnosticHistory dialog

class CDiagnosticHistory : public CDialog
{
// Construction
public:
	CDiagnosticHistory(CEGGSASDoc *pDoc, unsigned uFrom, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDiagnosticHistory)
	enum { IDD = IDD_DIAGNOSTIC_HISTORY };
	CListCtrl	m_cDiagnosisList;
	CString	m_cstrDiagnosis;
	CString	m_cstrDate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiagnosticHistory)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to current document.
  unsigned m_uFrom; // Place where caller is from.
  long m_lStudyID; // Study ID.

  void vSet_text(void);

protected:

	// Generated message map functions
	//{{AFX_MSG(CDiagnosticHistory)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnClickDiagnosisList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkDiagnosisList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGotoStudy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedDiaghistHelp();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIAGNOSTICHISTORY_H__3C952930_B34F_402C_BFBA_D144E1C71B60__INCLUDED_)
