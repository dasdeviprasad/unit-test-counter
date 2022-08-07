/********************************************************************
Medications.h

Copyright (C) 2003,2004,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CMedications class.

HISTORY:
03-OCT-03  RET  New.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#if !defined(AFX_MEDICATIONS_H__FD41D610_8C46_406A_A88A_DE9821D2078E__INCLUDED_)
#define AFX_MEDICATIONS_H__FD41D610_8C46_406A_A88A_DE9821D2078E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMedications dialog

class CMedications : public CDialog
{
// Construction
public:
	CMedications(CEGGSASDoc *pDoc, unsigned uFrom, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMedications)
	enum { IDD = IDD_MEDICATIONS };
	CListCtrl	m_cMedicationList;
	CString	m_cstrMedication;
	CString	m_cstrDate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMedications)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  unsigned m_uFrom; // Place where caller is from.
  long m_lStudyID; // Study ID from the database.

  void vSet_text(void);

protected:

	// Generated message map functions
	//{{AFX_MSG(CMedications)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickMedicationList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnMedUpdate();
	afx_msg void OnMedGotoStudy();
	afx_msg void OnDblclkMedicationList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedMedicationsHelp();
//  afx_msg void OnBnClickedCancel();
protected:
//  virtual void OnCancel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEDICATIONS_H__FD41D610_8C46_406A_A88A_DE9821D2078E__INCLUDED_)
