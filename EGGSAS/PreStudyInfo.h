/********************************************************************
PreStudyInfo.h

Copyright (C) 2003,2004,2005,2008,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CPreStudyInfo class.

HISTORY:
03-OCT-03  RET  New.
10-FEB-05  RET  Add class variable m_bOrigDelayedGastricEmptying.
07-N0V-08  RET  Version 2.08a
                  Added new time picker control.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
13-SEP-12  RET
             Changes to indications:
               Remove variables: m_bEructus, m_bGerd, m_bPPFullness,
                 m_bOrigEructus, m_bOrigGerd, m_bOrigPPFullness
               Add variables: m_bOrigGastroparesis, m_bOrigDyspepsiaBloatSatiety,
                 m_bOrigDyspepsia, m_bOrigAnorexia, m_bOrigRefractoryGERD,
                 m_bOrigWeightLoss, m_bOrigPostDilationEval, m_bOrigEvalEffectMed,
                 m_bGastroparesis, m_bDyspepsiaBloatSatiety, m_bDyspepsia,
                 m_bAnorexia, m_bRefractoryGERD, m_bWeightLoss, m_bPostDilationEval,
                 m_bEvalEffectMed
18-SEP-12  RET
             Add Include in Report Checkboxes for the attending physician,
               the refering physician and the technician.
                 Add class variables: m_bChkAttendingPhysician, m_bChkReferingPhysician,
                   m_bChkTechnician, m_bOrigChkAttendingPhysician,
                   m_bOrigChkReferingPhysician, m_bOrigChkTechnician
********************************************************************/

#include "afxdtctl.h"
#if !defined(AFX_PRESTUDYINFO_H__92494D0E_9DF9_4094_A040_80EAC1CF2F68__INCLUDED_)
#define AFX_PRESTUDYINFO_H__92494D0E_9DF9_4094_A040_80EAC1CF2F68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPreStudyInfo dialog

class CPreStudyInfo : public CDialog
{
// Construction
public:
	CPreStudyInfo(CEGGSASDoc *pDoc, unsigned uFrom, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPreStudyInfo)
	enum { IDD = IDD_PRE_STUDY_INFO };
	CString	m_cstrAttendPhys;
	BOOL	m_bBloating;
	CString	m_cstrIFPComments;
	BOOL	m_bEarlySateity;
	BOOL	m_bNausea;
	BOOL	m_bVomiting;
	CString	m_cstrReferPhys;
	CString	m_cstrTechnician;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreStudyInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  unsigned m_uFrom; // Place where caller is from.

  CString m_cstrOrigMealTime; // Value from when dialog box was started.
  CString m_cstrOrigAttendPhys; // Value from when dialog box was started.
  BOOL m_bOrigBloating; // Value from when dialog box was started.
  CString m_cstrOrigIFPComments; // Value from when dialog box was started.
  BOOL m_bOrigEarlySateity; // Value from when dialog box was started.
  BOOL m_bOrigNausea; // Value from when dialog box was started.
  BOOL m_bOrigVomiting; // Value from when dialog box was started.
  BOOL m_bOrigDelayedGastricEmptying; // Value from when dialog box was started.
  BOOL m_bOrigGastroparesis;
  BOOL m_bOrigDyspepsiaBloatSatiety;
  BOOL m_bOrigDyspepsia;
  BOOL m_bOrigAnorexia;
  BOOL m_bOrigRefractoryGERD;
  BOOL m_bOrigWeightLoss;
  BOOL m_bOrigPostDilationEval;
  BOOL m_bOrigEvalEffectMed;
  CString m_cstrOrigReferPhys; // Value from when dialog box was started.
  CString m_cstrOrigTechnician; // Value from when dialog box was started.
  BOOL m_bOrigChkAttendingPhysician;
  BOOL m_bOrigChkReferingPhysician;
  BOOL m_bOrigChkTechnician;

public: // Methods
  bool bChanged();
  void vSet_text(void);

protected:

	// Generated message map functions
	//{{AFX_MSG(CPreStudyInfo)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedPrestudyHelp();
  BOOL m_bDelayedGastricEmptying;
  CDateTimeCtrl m_cPreTestMealTime;
  BOOL m_bGastroparesis;
  BOOL m_bDyspepsiaBloatSatiety;
  BOOL m_bDyspepsia;
  BOOL m_bAnorexia;
  BOOL m_bRefractoryGERD;
  BOOL m_bWeightLoss;
  BOOL m_bPostDilationEval;
  BOOL m_bEvalEffectMed;
  afx_msg void OnBnClickedChkAttendPhys();
  afx_msg void OnBnClickedChkReferPhys();
  afx_msg void OnBnClickedChkTechnician();
  BOOL m_bChkAttendingPhysician;
  BOOL m_bChkReferingPhysician;
  BOOL m_bChkTechnician;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRESTUDYINFO_H__92494D0E_9DF9_4094_A040_80EAC1CF2F68__INCLUDED_)
