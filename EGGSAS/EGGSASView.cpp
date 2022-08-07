/********************************************************************
EGGSASView.cpp

Copyright (C) 2003,2004, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CEGGSASView class.

  This class provides the view of the document.  IT IS NOT USED.
  The class, CTenMinView actually provides the main view.

HISTORY:
03-OCT-03  RET  New.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"

#include "EGGSASDoc.h"
#include "EGGSASView.h"
#include "TenMinView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEGGSASView

IMPLEMENT_DYNCREATE(CEGGSASView, CView)

BEGIN_MESSAGE_MAP(CEGGSASView, CView)
	//{{AFX_MSG_MAP(CEGGSASView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEGGSASView construction/destruction

CEGGSASView::CEGGSASView()
{
	// TODO: add construction code here

}

CEGGSASView::~CEGGSASView()
{
}

BOOL CEGGSASView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CEGGSASView drawing

void CEGGSASView::OnDraw(CDC* pDC)
{
	CEGGSASDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CEGGSASView printing

BOOL CEGGSASView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CEGGSASView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CEGGSASView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CEGGSASView diagnostics

#ifdef _DEBUG
void CEGGSASView::AssertValid() const
{
	CView::AssertValid();
}

void CEGGSASView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEGGSASDoc* CEGGSASView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEGGSASDoc)));
	return (CEGGSASDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEGGSASView message handlers
