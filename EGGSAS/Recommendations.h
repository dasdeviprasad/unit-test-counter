/********************************************************************
Recommendations.h

Copyright (C) 2003,2004,2012 3CPM Company, Inc.  All rights reserved.

  Header file for the CRecommendations class.

HISTORY:
08-NOV-03  RET  New.
02-SEP-12  RET
             Implement new database table Recommendations
               Add variables: m_cstrOrigRecommKeyPhrase, m_cstrRecommKeyPhrase,
                 m_cRecommList, m_iRecommSelIndex
********************************************************************/

#include "afxwin.h"
#if !defined(AFX_RECOMMENDATIONS_H__9C795515_4C33_4014_92C8_30D42DC2B3D2__INCLUDED_)
#define AFX_RECOMMENDATIONS_H__9C795515_4C33_4014_92C8_30D42DC2B3D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRecommendations dialog

class CRecommendations : public CDialog
{
// Construction
public:
	CRecommendations(CEGGSASDoc *pDoc, unsigned uFrom, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRecommendations)
	enum { IDD = IDD_RECOMMENDATIONS };
	CString	m_cstrRecommendations;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecommendations)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  unsigned m_uFrom; // Place where caller is from (new study, edit, view).
	CString	m_cstrOrigRecommendations; // Original value read from the database.
  CString m_cstrOrigRecommKeyPhrase;
  CString m_cstrRecommKeyPhrase;
  int m_iRecommSelIndex; // Index of last select item from recommendation key phrase combobox.

public: // Support methods
  void vGet_recommendations(CString &cstrKeyPhrase, CString &cstrRecommendations);
  void vSet_recommendations(CString &cstrKeyPhrase, CString &cstrRecommendations);
  bool bChanged();
  void vFill_key_phrase();
  void vSet_text();

protected:

	// Generated message map functions
	//{{AFX_MSG(CRecommendations)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedRecommendationsHelp();
  afx_msg void OnBnClickedBtnAddToReport();
//  afx_msg void OnBnClickedBtnSaveDiagToList();
  afx_msg void OnBnClickedBtnRemoveFromList();
  afx_msg void OnCbnSelchangeCbxRecommKeyPhrase();
  afx_msg void OnBnClickedBtnSaveRecommToList();
  CComboBox m_cRecommList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECOMMENDATIONS_H__9C795515_4C33_4014_92C8_30D42DC2B3D2__INCLUDED_)
