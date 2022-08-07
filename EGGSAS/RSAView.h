/********************************************************************
RSAView.h

Copyright (C) 2003,2004,2007, 3CPM Company, Inc.  All rights reserved.

  Header file for the CRSAView class.

HISTORY:
08-NOV-03  RET  New.
10-MAY-07  RET  Research Version
                  Add class variables: m_uMaxPeriods
********************************************************************/

#if !defined(AFX_RSAVIEW_H__8D384A55_0EE2_44D9_B109_9DB4049AF6C9__INCLUDED_)
#define AFX_RSAVIEW_H__8D384A55_0EE2_44D9_B109_9DB4049AF6C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRSAView view

class CRSAView : public CView
{
protected:
	CRSAView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRSAView)

// Attributes
public:
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  float *m_pfData; // Pointer to the data for graphing.
  short int m_iMaxGraphs; // Maximum number of graphs.
  short int m_iMaxFreqPts; // Maximum number of frequency points(X Axis points).
  COLORREF m_crPreWaterColor; // Color for the baseline graphs.
  unsigned short m_uMaxPeriods; // Maximum number of periods (timebands).

// Operations
public:
  void vAnalyze_data_for_RSA();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRSAView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CRSAView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CRSAView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RSAVIEW_H__8D384A55_0EE2_44D9_B109_9DB4049AF6C9__INCLUDED_)
