/********************************************************************
ConfigReport.h

Copyright (C) 2003,2004,2005,2007,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CConfigReport class.

HISTORY:
14-NOV-03  RET  New.
05-FEB-05  RET  Add class variables for check new boxes:
                  m_bCoreReport, m_bDiagRecom, m_bRSA, m_bSummGraphs,
                  m_bRawSigSelMin
                Add class variables:
                  m_bOrigCoreReport, m_bOrigDiagRecom, m_bOrigRSA,
                  m_bOrigSummGraphs, m_bOrigRawSigSelMin
10-MAY-07  RET  Research Version
                Add class variables:
                  m_bOrigPatientInfo, m_bPatientInfo
16-FEB-12  RET  Version 
                  Changes for foreign languages.
22-AUG-12  RET
             Modify the report configuration so that the RSA and the Summary
             Graphs can either be on the first page of the report or on a
             subsequent page.
               Change: m_bOrigRSA to m_bOrigRSAFirstPage
                       m_bOrigSummGraphs to m_bOrigSummGraphsFirstPage
               Add class variables: m_bOrigRSAOtherPage, m_bOrigSummGrphsOtherPage,
                  m_bChkRSAFirstPage, m_bChkRSAOtherPage, m_bChkSummGraphsFirstPage,
                  m_bChkSummGrphsOtherPage
14-DEC-12  RET
             Change to filter out frequencies below 10 cpm in the RSA.
               Add checkbox for printing/viewing both RSAs.
                 Add variable: m_bBothRSA, m_bOrigBothRSA
********************************************************************/

#if !defined(AFX_CONFIGREPORT_H__D1E4D3AA_E4B3_4772_91DD_80EFEE071FA9__INCLUDED_)
#define AFX_CONFIGREPORT_H__D1E4D3AA_E4B3_4772_91DD_80EFEE071FA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CConfigReport dialog

class CConfigReport : public CDialog
{
// Construction
public:
	CConfigReport(CEGGSASDoc *pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigReport)
	enum { IDD = IDD_CUSTOM_REPORT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigReport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the active document.
	BOOL m_bOrigDataSheet; // Original value read from the INI file.
	BOOL m_bOrigEntireRawSignal; // Original value read from the INI file.
  BOOL m_bOrigCoreReport; // Original value read from the INI file.
  BOOL m_bOrigDiagRecom; // Original value read from the INI file.
  BOOL m_bOrigRSAFirstPage; // Original value read from the INI file.
  BOOL m_bOrigSummGraphsFirstPage; // Original value read from the INI file.
  BOOL m_bOrigRawSigSelMin; // Original value read from the INI file.
    // For research version only.
  BOOL m_bOrigPatientInfo; // Original value read from the INI file.
  BOOL m_bOrigEventLog; // Original value read from the INI file.
  BOOL m_bOrigRSAOtherPage; // Original value read from the INI file.
  BOOL m_bOrigSummGrphsOtherPage; // Original value read from the INI file.
  BOOL m_bOrigBothRSA; // Original value read from the INI file.


public: // Support methods
  bool bReport_changed();
  void vSet_text(void);

protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigReport)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedConfigrptHelp();
  afx_msg void OnBnClickedCoreReport();
  void vEnable_rpt_sections(BOOL bCoreReport);
  BOOL m_bCoreReport;
  BOOL m_bDiagRecom;
  BOOL m_bChkRSAFirstPage;
  BOOL m_bDataSheet;
  BOOL m_bChkSummGraphsFirstPage;
  BOOL m_bRawSigSelMin;
  BOOL m_bEntireRawSignal;
  BOOL m_bPatientInfo;
  BOOL m_bEventLog;
  BOOL m_bChkRSAOtherPage;
  BOOL m_bChkSummGrphsOtherPage;
  afx_msg void OnBnClickedRptRsaFirstPage();
  afx_msg void OnBnClickedRptRsaOtherPage();
  afx_msg void OnBnClickedRptSummGrphsFirstPage();
  afx_msg void OnBnClickedRptSummGrphsOtherPage();
  BOOL m_bBothRSA;
  afx_msg void OnBnClickedRptRsaIncludeBoth();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGREPORT_H__D1E4D3AA_E4B3_4772_91DD_80EFEE071FA9__INCLUDED_)
