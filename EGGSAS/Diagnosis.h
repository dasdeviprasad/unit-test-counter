/********************************************************************
Diagnosis.h

Copyright (C) 2003,2004,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CDiagnosis class.

HISTORY:
08-NOV-03  RET  New.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
27-AUG-12  RET
             Implement Diagnosis ID and description.
               Add variables: m_cstrDiagnosisKeyPhrase, m_cDiagList, m_cICDList,
                 m_cstrOrigDiagnosisKeyPhrase, m_cstrOrigICDCode,
                 m_iDiagnosisSelIndex, m_iICDCodeSelIndex
********************************************************************/

#include "afxwin.h"
#if !defined(AFX_DIAGNOSIS_H__1E459FB4_36EF_4EFA_9E37_E293434CA8B7__INCLUDED_)
#define AFX_DIAGNOSIS_H__1E459FB4_36EF_4EFA_9E37_E293434CA8B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum eDiagnosis
  {
  DIAG_NONE = -1,              // No diagonsis.
  DIAG_NORMAL = 0,             // Normal
  DIAG_PROB_NORM,              // Probable Normal
  DIAG_SUB_NORMAL,             // Sub normal
  DIAG_BRADY,                  // Bradygastria
  DIAG_POS_BRADY_CC,           // Possible Bradygastria Clinical Correlation Required
  DIAG_PROB_BRADY,             // Probable Bradygastria
  DIAG_TACHY,                  // Tachygastria
  DIAG_POS_TACHY_CC,           // Possible Tachygastria Clinical Correlation Required
  DIAG_PROB_TACHY,             // Probable Tachygastria
  DIAG_DUOD_HYPER,             // Duodenal Hyperactivity
  DIAG_RESP_EFFECT,            // Respiratory Effect
  DIAG_GAS_OUT_OBS,            // Gastric Outlet Obstruction 
  DIAG_POS_GAS_OUT_OBS,        // Possible Gastric Outlet Obstruction 
  DIAG_PROB_NORM_GAS_OUT_OBS,  // Probable Normal Consider Gastric Outlet Obstruction
  DIAG_MIX_DYS,                // Mixed Dysrythmia
  };

#define NORMAL     0  // below high normal range
#define ABOVE_NORM 1  // above high normal range
#define BELOW_NORM 2  // Below low normal range.

  // Indexes into arrays that indicate value in or out of normal range.
enum eRangeIndex
  {
  RI_BRADY = 0,
  RI_3CPM,
  RI_TACHY,
  RI_DUOD,
  RI_LAST
  };

/////////////////////////////////////////////////////////////////////////////
// CDiagnosis dialog

class CDiagnosis : public CDialog
{
// Construction
public:
	CDiagnosis(CEGGSASDoc *pDoc, unsigned uFrom, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDiagnosis)
	enum { IDD = IDD_DIAGNOSIS };
	CString	m_cstrSuggestedDiagnosis;
	CString	m_cstrDiagnosis;
	int		m_iDiagAction;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiagnosis)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to current document.
  ANALYZED_DATA m_adData; // Contains information and data for analysis.
  unsigned m_uFrom; // Place where caller is from (new study, edit, view).
  BOOL m_bUserDiagnosis; // TRUE indicates that the user changed the diagnosis.
  //BOOLm_bUserICDCode;  // TRUE indicates that the user changed the ICD Code.
  CString m_cstrOrigDiagnosis; // User entered diagnosis when this dialog
                               // box is first started.
  CString m_cstrOrigDiagnosisKeyPhrase;
  CString m_cstrOrigICDCode;
    // Holds indicators of whether frequency band is normal or above normal.
    //   0 = Normal
    //   1 = above normal.
    //   Frequency bands: 
    //     element 0 = bradygastria
    //     element 1 = normal
    //     element 2 = tachygastria
    //     element 2 = duodenal/respiratory
  unsigned short m_uPostWater10[RI_LAST];
  unsigned short m_uPostWater20[RI_LAST];
  unsigned short m_uPostWater30[RI_LAST];
  CString m_cstrDiagnosisKeyPhrase;
  CString m_cstrICDCode;
  int m_iDiagnosisSelIndex; // Index of last select item from diagnosis key phrase combobox.
  int m_iICDCodeSelIndex; // Index of last select item from ICD code combobox.

public: // Methods
  short int iPerform_analysis();
  short int iDiagnose();
  short int iDiagnose_postwater();
  CString cstrGet_diagnosis(short int iDiagnosis);
  void vGet_diagnosis(PATIENT_DATA *pdDiag);
  void vSet_diagnosis(PATIENT_DATA *pdDiag);
  bool bChanged();
  void vSet_text(void);
  void vEnable_diagdesc_icd_fields(BOOL bEnable);
  void vFill_diag_desc_id();
  void vFill_icd_code();

protected:

	// Generated message map functions
	//{{AFX_MSG(CDiagnosis)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnAcceptDiagnosis();
	afx_msg void OnModifyDiagnosis();
	afx_msg void OnRejectDiagnosis();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedDiagnosisHelp();
  afx_msg void OnBnClickedBtnAddToReport();
  afx_msg void OnBnClickedBtnSaveDiagToList();
  afx_msg void OnBnClickedBtnRemoveFromList();
  afx_msg void OnCbnSelchangeCbxDiagList();
  afx_msg void OnBnClickedBtnIncludeIcdCode();
  afx_msg void OnBnClickedBtnSaveIcdToList();
  afx_msg void OnBnClickedBtnRemoveIcdFromList();
  afx_msg void OnCbnSelchangeCbxIcdList();
  CComboBox m_cDiagList;
  CComboBox m_cICDList;
//  afx_msg void OnCbnEditchangeCbxDiagList();
  void vDisplay_diagnosis(void);
  void vDisplay_ICD_code(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIAGNOSIS_H__1E459FB4_36EF_4EFA_9E37_E293434CA8B7__INCLUDED_)
