/********************************************************************
FacilityInfo.h

Copyright (C) 2003,2004,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CFacilityInfo class.

HISTORY:
08-NOV-03  RET  New.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
22-AUG-12  RET
             Add class variable: m_iIncludeRptItems[]
             Add class variables cooresponding to the checkboxes:
               m_bChkName, m_bChkAddr1, m_bChkAddr2, m_bChkCity,
               m_bChkState, m_bChkZipCode, m_bChkPhoneNum, m_bChkLogo
********************************************************************/

#if !defined(AFX_FACILITYINFO_H__1D9A7B27_691C_4D62_A67F_CE246DF45885__INCLUDED_)
#define AFX_FACILITYINFO_H__1D9A7B27_691C_4D62_A67F_CE246DF45885__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFacilityInfo dialog

class CFacilityInfo : public CDialog
{
// Construction
public:
	CFacilityInfo(CEGGSASDoc *pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFacilityInfo)
	enum { IDD = IDD_FACILITY_INFO };
	CString	m_cstrFacAddr1;
	CString	m_cstrFacAddr2;
	CString	m_cstrFacCity;
	CString	m_cstrFacName;
	CString	m_cstrFacPhone;
	CString	m_cstrFacState;
	CString	m_cstrFacZipcode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFacilityInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  long m_lFacilityID; // Facility ID for the database.

  CString m_cstrOrigFacAddr1; // Original value read from the database.
  CString m_cstrOrigFacAddr2; // Original value read from the database.
  CString m_cstrOrigFacCity; // Original value read from the database.
  CString m_cstrOrigFacName; // Original value read from the database.
  CString m_cstrOrigFacPhone; // Original value read from the database.
  CString m_cstrOrigFacState; // Original value read from the database.
  CString m_cstrOrigFacZipcode; // Original value read from the database.
  CString m_cstrOrigLogo; // Original value read from the database.

  // Array to hold a temporary copy of the configuration array that
  // maintains indications of whether fields should be included in
  // the report.
  short int m_iIncludeRptItems[INC_RPT_LAST];

public: // Support methods
  bool bChanged();
  void vSet_text(void);

protected:

	// Generated message map functions
	//{{AFX_MSG(CFacilityInfo)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedFacilityinfoHelp();
  BOOL m_bChkName;
  BOOL m_bChkAddr1;
  BOOL m_bChkAddr2;
  BOOL m_bChkCity;
  BOOL m_bChkState;
  BOOL m_bChkZipCode;
  BOOL m_bChkPhoneNum;
  BOOL m_bChkLogo;
  afx_msg void OnBnClickedChkFacName();
  afx_msg void OnBnClickedChkFacAddr1();
  afx_msg void OnBnClickedChkFacAddr2();
  afx_msg void OnBnClickedChkFacCity();
  afx_msg void OnBnClickedChkFacState();
  afx_msg void OnBnClickedChkFacZipcode();
  afx_msg void OnBnClickedChkFacPhonenum();
  afx_msg void OnBnClickedChkFacLogo();
  CString m_cstrLogo;
  afx_msg void OnBnClickedBtnBrowseLogo();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FACILITYINFO_H__1D9A7B27_691C_4D62_A67F_CE246DF45885__INCLUDED_)
