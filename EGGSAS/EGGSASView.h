/********************************************************************
EGGSASView.h

Copyright (C) 2003,2004, 3CPM Company, Inc.  All rights reserved.

  Header file for the CEGGSASView class.

HISTORY:
03-OCT-03  RET  New.
********************************************************************/

#if !defined(AFX_EGGSASVIEW_H__A0CBBE9B_39EA_472D_896C_F1DE7E6EDEBF__INCLUDED_)
#define AFX_EGGSASVIEW_H__A0CBBE9B_39EA_472D_896C_F1DE7E6EDEBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CEGGSASView : public CView
{
protected: // create from serialization only
	CEGGSASView();
	DECLARE_DYNCREATE(CEGGSASView)

// Attributes
public:
	CEGGSASDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEGGSASView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEGGSASView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEGGSASView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in EGGSASView.cpp
inline CEGGSASDoc* CEGGSASView::GetDocument()
   { return (CEGGSASDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EGGSASVIEW_H__A0CBBE9B_39EA_472D_896C_F1DE7E6EDEBF__INCLUDED_)
