/********************************************************************
AnalyzePeriod.h

Copyright (C) 2003, 3CPM Company, Inc.  All rights reserved.

  Header file for the CAnalyzePeriod class.

HISTORY:
03-OCT-03  RET  New.
********************************************************************/

#if !defined(AFX_ANALYZEPERIOD_H__C1A1FFF6_5CBD_44E9_9903_7E4D5146FE31__INCLUDED_)
#define AFX_ANALYZEPERIOD_H__C1A1FFF6_5CBD_44E9_9903_7E4D5146FE31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAnalyzePeriod dialog

class CAnalyzePeriod : public CDialog
{
// Construction
public:
	CAnalyzePeriod(unsigned uPeriod, CEGGSASDoc *pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnalyzePeriod)
	enum { IDD = IDD_ANALYZE_PERIOD };
	CListBox	m_cArtifact;
	int		m_iAccept;
	CString	m_cstrAnalyzeTitle;
	CString	m_cstrEndMinute;
	CString	m_cstrStartMinute;
	CString	m_cstrGoodMinutes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnalyzePeriod)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  unsigned m_uPeriod; // Period dialog box is analyzing.
  CEGGSASDoc *m_pDoc; // Pointer to the document.
  float m_fStart; // Program defined start of good minutes.
  float m_fEnd; // Program defined end of good minutes.

public: // Methods
  bool bValid_minute_format(CString cstrMinute);


protected:

	// Generated message map functions
	//{{AFX_MSG(CAnalyzePeriod)
	virtual BOOL OnInitDialog();
	afx_msg void OnAccept();
	afx_msg void OnReject();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANALYZEPERIOD_H__C1A1FFF6_5CBD_44E9_9903_7E4D5146FE31__INCLUDED_)
