/********************************************************************
Demographics.h

Copyright (C) 2003,2004,2005,2012,2013, 3CPM Company, Inc.  All rights reserved.

  Header file for the CDemographics class.

HISTORY:
03-OCT-03  RET  New.
06-FEB-05  RET  Changes to use the research reference number.  If the
                  Add class variables:
                    m_cstrOrigResearchRefNum, cstrSaveFirstName,
                    m_cstrSaveLastName
10-FEB-05  RET  Add middle initial to dialog box.  Add the following
                  class variables:
                    m_cstrSaveMiddleInitial, m_cstrOrigMiddleInitial
16-FEB-12  RET  Version 
                  Changes for foreign languages.
22-AUG-12  RET
             Add class variable: m_iIncludeRptItems[]
             Add class variables cooresponding to the checkboxes:
               m_bChkFirstName, m_bChkMI, m_bChkLastName, m_bChkDateOfBirth,
               m_bChkSSN, m_bChkAddr1, m_bChkAddr2, m_bChkCity, m_bChkState,
               m_bChkZipCode, m_bChkPhoneNum, m_bChkHospNum, m_bChkRsrchRefNum,
               m_cstrOrigAddress1, m_cstrOrigAddress2, m_cstrOrigCity,
               m_cstrOrigState, m_cstrOrigZipCode, m_cstrOrigDateOfBirth,
               m_cstrOrigHospitalNum, m_cstrOrigPhone, m_cstrOrigSocialSecurityNum
25-FEB-13  RET
             Add checkboxes for Male and Female and a checkbox to include in the report.
               Add class variables: m_bMale, m_bOrigMale, m_bFemale, m_bOrigFemale,
               m_bIncMaleFemale, m_bHide, m_bOrigHide
********************************************************************/

#if !defined(AFX_DEMOGRAPHICS_H__E7F30511_4738_4B75_A3D9_3F4D35284BDB__INCLUDED_)
#define AFX_DEMOGRAPHICS_H__E7F30511_4738_4B75_A3D9_3F4D35284BDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Conditions under which the dialog box is closed.
#define DLG_CANCEL 0 // Cancel button selected
#define DLG_OK 1  // OK button selected
#define DLG_CLOSE_SAVE 2  // Save and Close button selected

/////////////////////////////////////////////////////////////////////////////
// CDemographics dialog

class CDemographics : public CDialog
{
// Construction
public:
	CDemographics(unsigned uFrom, CEGGSASDoc *pDoc, CWnd *pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDemographics)
	enum { IDD = IDD_DEMOGRAPHICS };
	CString	m_cstrAddress1;
	CString	m_cstrAddress2;
	CString	m_cstrCity;
	CString	m_cstrState;
	CString	m_cstrZipCode;
	CString	m_cstrDateOfBirth;
	CString	m_cstrHospitalNum;
	CString	m_cstrPhone;
	CString	m_cstrResearchRefNum;
	CString	m_cstrSocialSecurityNum;
	BOOL	m_bResearchRefEnable;
	CString	m_cstrFirstName;
	CString	m_cstrLastName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDemographics)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to current document.
  unsigned m_uFrom; // Place where caller is from.
  short unsigned m_uExitSts; // The conditions under which the dialog box was closed.
  CString m_cstrOrigFirstName;
  CString m_cstrOrigLastName;
  CString m_cstrOrigResearchRefNum;
  CString m_cstrOrigMiddleInitial;
	CString	m_cstrOrigAddress1;
	CString	m_cstrOrigAddress2;
	CString	m_cstrOrigCity;
	CString	m_cstrOrigState;
	CString	m_cstrOrigZipCode;
	CString	m_cstrOrigDateOfBirth;
	CString	m_cstrOrigHospitalNum;
	CString	m_cstrOrigPhone;
	CString	m_cstrOrigSocialSecurityNum;
  CString m_cstrSaveFirstName;
  CString m_cstrSaveLastName;
  CString m_cstrSaveMiddleInitial;
  BOOL m_bOrigMale;
  BOOL m_bOrigFemale;
  BOOL m_bOrigHide;
  // Array to hold a temporary copy of the configuration array that
  // maintains indications of whether fields should be included in
  // the report.
  short int m_iIncludeRptItems[INC_RPT_LAST];
#if EGGSAS_READER == READER_DEPOT
  bool m_bShowHiddenPatientCheckbox;
#endif

public: // Methods
  void vGet_demographics(PATIENT_DATA *pdDemographics);
  void vSet_demographics(PATIENT_DATA *pdDemographics);
  short unsigned uGet_exit_sts();
  bool bChanged(void);
  void vSet_text(void);

protected:

	// Generated message map functions
	//{{AFX_MSG(CDemographics)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnResrchRefNumEna();
	afx_msg void OnSaveAndClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedDemographicsHelp();
  CString m_cstrMiddleInitial;
  afx_msg void OnBnClickedChkHuman();
  afx_msg void OnBnClickedChkDog();
  BOOL m_bHuman;
  BOOL m_bDog;
  BOOL m_bChkFirstName;
  BOOL m_bChkMI;
  BOOL m_bChkLastName;
  BOOL m_bChkDateOfBirth;
  BOOL m_bChkSSN;
  BOOL m_bChkAddr1;
  BOOL m_bChkAddr2;
  BOOL m_bChkCity;
  BOOL m_bChkState;
  BOOL m_bChkZipCode;
  BOOL m_bChkPhoneNum;
  BOOL m_bChkHospNum;
  BOOL m_bChkRsrchRefNum;
  afx_msg void OnBnClickedChkIncrptFirstname();
  afx_msg void OnBnClickedChkIncrptMi();
  afx_msg void OnBnClickedChkIncrptLastname();
  afx_msg void OnBnClickedChkIncrptDateofbirth();
  afx_msg void OnBnClickedChkIncrptSsn();
  afx_msg void OnBnClickedChkIncrptAddr1();
  afx_msg void OnBnClickedChkIncrptAddr2();
  afx_msg void OnBnClickedChkIncrptCity();
  afx_msg void OnBnClickedChkIncrptState();
  afx_msg void OnBnClickedChkIncrptZipcode();
  afx_msg void OnBnClickedChkIncrptPhonenum();
  afx_msg void OnBnClickedChkIncrptHospnum();
  afx_msg void OnBnClickedChkIncrptRsrchrefnum();
  afx_msg void OnBnClickedChkMale();
  afx_msg void OnBnClickedChkFemale();
  BOOL m_bMale;
  BOOL m_bFemale;
  afx_msg void OnBnClickedChkIncrptMaleFemale();
  BOOL m_bIncMaleFemale;
  BOOL m_bHide;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEMOGRAPHICS_H__E7F30511_4738_4B75_A3D9_3F4D35284BDB__INCLUDED_)
