/********************************************************************
SelectPatient.h

Copyright (C) 2003,2004,2005,2008,2013,2160 3CPM Company, Inc.  All rights reserved.

  Header file for the CSelectPatient class.

HISTORY:
03-OCT-03  RET  New.
10-FEB-05  RET  For middle initial, add class variable: m_cstrMiddleInitial
30-JUN-08 RET  Version 2.05
                 Add define: WHATIF_PREFIX
                 Add class variable: m_lWhatIfID
27-FEB-13  RET
             Changes for hiding patients.
               Add class variable: m_bHiddenPatients
08-JUN-16  RET
             Add feature to hide study dates.
               Add method prototype: vFree_data()
               Add variable: m_cstraStudyIDs
********************************************************************/

#if !defined(AFX_SELECTPATIENT_H__790228F8_80AF_4F4F_8A18_364308CAA3E9__INCLUDED_)
#define AFX_SELECTPATIENT_H__790228F8_80AF_4F4F_8A18_364308CAA3E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WHATIF_PREFIX "  - "

//typedef struct
//  {
//  long lPatientID;
//  long lStudyID;
//  } DATE_SRCH_INFO;

/////////////////////////////////////////////////////////////////////////////
// CSelectPatient dialog

class CSelectPatient : public CDialog
{
// Construction
public:
	CSelectPatient(CEGGSASDoc *pDoc, short int iStudyType, BOOL bHiddenPatients, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectPatient)
	enum { IDD = IDD_SELECT_PATIENT };
	CListCtrl	m_cPatientDBList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectPatient)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  CString m_cstrLastName; // Last name of patient.
  CString m_cstrFirstName; // First name of patient.
  CString m_cstrMiddleInitial; // Middle initial of patient.
  CString m_cstrDate; // Date of study.
  long m_lPatientID; // Patient ID from the database.
  short int m_iStudyType; // Study type.
  long m_lWhatIfID; // WhatIF ID.  INV_LONG if not a WhatIF
  BOOL m_bHiddenPatients; // TRUE to list hidden patient, FALSE to list active patients

public: // Methods
  void vGet_patient_info(PATIENT_DATA *pdInfo, long *plWhatIfID);
  void vGet_selection_data(int iSel);
  void vFill_patient_listbox();
  CString cstrFormat_name(FIND_PATIENT_INFO *pfpiData);
//// REMOTE DB
  void vDisplay_db_file();

protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectPatient)
	virtual BOOL OnInitDialog();
	afx_msg void OnAlternateDb();
	virtual void OnOK();
	afx_msg void OnDblclkPatientDbList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickPatientDbList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedSelectpatientHelp();
  CString m_cstrDBFile;
  afx_msg void OnBnClickedBtnSelectReaderDb();
  afx_msg void OnBnClickedBtnSelectLocalDb();
  void vSet_text(void);
  //long *m_plStudyIDs;
  void vFree_data(void);
  CStringArray m_cstraStudyIDs;
  afx_msg void OnBnClickedBtnSearch();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTPATIENT_H__790228F8_80AF_4F4F_8A18_364308CAA3E9__INCLUDED_)
