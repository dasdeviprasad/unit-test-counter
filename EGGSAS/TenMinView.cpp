/********************************************************************
TenMinView.cpp

Copyright (C) 2003,2004,2005,2007,2008,2009,2012,2013,2016 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CTenMinView class.

  This class provides the main view for the document.
    - Displays patient name
    - Graphs the 10 minutes of raw data for the selected period.

HISTORY:
03-OCT-03  RET  New.
23-FEB-04  RET  Change dropdown list to be a droplist so that user may click
                  anywhere on dropdown list to drop down the list (and can't
                  edit it).
06-FEB-05  RET  Changes to use the research reference number.
                  Modified methods: vSet_patient_info()
10-FEB-05  RET  Changes for middle initial.  Change vSet_patient_info().
01-MAR-05  RET  Add "Product of" to 3CPM logo.  Change the following
                  methods: OnInitialUpdate(), OnDraw()
05-APR-05  RET  Add EZEM VisiSpace logo to center of screen and moved
                3CPM logo to bottom center of screen.
12-MAY-07  RET  Research Version
                  Change OnInitialUpdate() to fill in period combobox
                    according to version.
                  Change vUpdate_period_combobox() to fill in the combobox
                    with the correct number of periods.
                  Add methods: vCalc_min_per_graph(), vInit_research_data(),
                    vMove_research_analyze_fields(), vMove_research_sel_min_group(),
                    bValidate_good_minutes(), vInit_period_good_mins()
                  Add scrollbar to scroll the data graphs.  Add scrollbar
                    event handler OnVScroll().
                  Change the Start and End minute fields to Start and end
                    Good minute fields.
                  Change the Accept changes button to Set Period button.
                  Add checkboxes for setting the period and good minutes.
                    Add a field variable and message handler for each.
25-AUG-07  RET  Version 2.02
                  Add field between patient name/date and drop down period
                    combobox for the study type.
05-FEB-08  RET  Version 2.03
                  Add method vShow_WhatIf() to display/hide the fields
                    for the What If.
30-JUN-08  RET  Version 2.05
                  Remove "Delete period" and "Insert period" buttons.
                  Add "Reset Periods" button and event handler.
                    Change methods: vMove_research_analyze_fields(),
                      vShow_research_fields(), vEnable_research_period_fields().
27-JUL-08  RET  Version 2.05a
                  Change OnInitialUpdate() to display the WhatIf fields if
                    a WhatIf is currently loaded.
14-JAN-09  RET  Version CDDRdr.2.08c
                  Modify OnInitialUpdate() to disable all fields that can
                    change original study.
                  Change vEnable_research_period_fields() and
                    vEnable_research_goodmin_fields() to always disable fields.
27-SEP-11  RET
             Changes to write an ASCII raw data file and not allow any analysis.
               Change methods: vShow_analyze_fields(), vShow_min_sel_fields()
23-FEB-12  RET  Version 
                  Changes for foreign languages.
03-OCT-12  RET
             Change for 60/200 CPM
               Change OnDraw() to display the select frequency range dialog
                 box the first time if build for the 60/200 CPM hardware.
13-MAR-13  RET
             Changes for frequency range selection.
              Remove asking for the frequency range selection on program startup.
                Change methods: OnDraw()
              Add displaying the frequency range to the study type.
                Change method: vShow_study_type()
17-MAR-13  RET
             Changes for frequency range selection.
               Don't hide the study type field for research only because
               it will display the recording frequency.
                 Change methods: OnInitialUpdate(), vShow_study_type()
09-JUN-16  RET
             Add feature to hide study dates.
                 Change methods: vSet_patient_info()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "MainFrm.h"
#include "EGGSASDoc.h"
#include "analyze.h"
#include "TenMinView.h"
#include "EventRecorder.h"
#include "FreqRange.h"
#include "util.h"

#include "Pegrpapi.h"
#include "Pemessag.h"
#include ".\tenminview.h"
//#include ".\tenminview.h"
//#include ".\tenminview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame *g_pFrame;

/////////////////////////////////////////////////////////////////////////////
// CTenMinView dialog

IMPLEMENT_DYNCREATE(CTenMinView, CFormView)

/********************************************************************
Constructor

  Initialize class data.
********************************************************************/
CTenMinView::CTenMinView()
	: CFormView(CTenMinView::IDD)
  , m_bLogoVisible(true)
  , m_cstrPeriodStartMin(_T(""))
  , m_cstrPeriodEndMin(_T(""))
  , m_bSetPeriodLength(FALSE)
  , m_bSetGoodMinutes(FALSE)
  , m_cstrStartGoodMin(_T(""))
  , m_cstrEndGoodMin(_T(""))
  , m_cstrEventDescription(_T(""))
  , m_cstrStudyType(_T(""))
  , m_cstrWhatIfDesc(_T(""))
  {

	//{{AFX_DATA_INIT(CTenMinView)
	m_cstrPatientName = _T("");
	m_cstrStartMin = _T("");
	m_cstrEndMin = _T("");
	m_iAccept = -1;
	m_cstrGoodMin = _T("");
	//}}AFX_DATA_INIT
  
  m_bInitialUpdate = false;
  m_bAnalyzedFieldsMoved = false;
  m_bAnalyzedFieldsVisible = false;
  m_bRsrchAnalyzedFieldsMoved = false;
  m_bRsrchAnalyzedFieldsVisible = false;
  }

/********************************************************************
Destructor

  Delete the windows holding the graphs.
********************************************************************/
CTenMinView::~CTenMinView()
  {

  if(m_hPEData1 != NULL)
    PEdestroy(m_hPEData1);
  if(m_hPEData2 != NULL)
    PEdestroy(m_hPEData2);
  if(m_hPEData3 != NULL)
    PEdestroy(m_hPEData3);

  }

void CTenMinView::DoDataExchange(CDataExchange* pDX)
{
CFormView::DoDataExchange(pDX);
//{{AFX_DATA_MAP(CTenMinView)
DDX_Control(pDX, IDC_ARTIFACT_LIST, m_cArtifactList);
DDX_Control(pDX, IDC_10MIN_PERIOD_LST, m_c10MinPeriodLst);
DDX_Text(pDX, IDC_PATIENT_NAME, m_cstrPatientName);
DDX_Text(pDX, IDC_START_MIN, m_cstrStartMin);
DDX_Text(pDX, IDC_END_MIN, m_cstrEndMin);
DDX_Radio(pDX, IDC_ACCEPT, m_iAccept);
DDX_Text(pDX, IDC_GOOD_MIN, m_cstrGoodMin);
//}}AFX_DATA_MAP
DDX_Text(pDX, IDC_START_PERIOD_MIN, m_cstrPeriodStartMin);
DDX_Text(pDX, IDC_END_PERIOD_MIN, m_cstrPeriodEndMin);
DDX_Control(pDX, IDC_SCROLLBAR_GRAPH, m_cGraphScrollBar);
DDX_Check(pDX, IDC_CHK_SET_PERIOD_LENGTH, m_bSetPeriodLength);
DDX_Check(pDX, IDC_CHK_SET_GOOD_MINUTES, m_bSetGoodMinutes);
DDX_Text(pDX, IDC_START_GOOD_MIN, m_cstrStartGoodMin);
DDX_Text(pDX, IDC_END_GOOD_MIN, m_cstrEndGoodMin);
DDX_Control(pDX, IDC_EVENT_LIST, m_cEventsList);
DDX_Text(pDX, IDC_EVENT_DESCRIPTION, m_cstrEventDescription);
DDX_Text(pDX, IDC_STUDY_TYPE, m_cstrStudyType);
DDX_Text(pDX, IDC_EDIT_WHAT_IF, m_cstrWhatIfDesc);
}


BEGIN_MESSAGE_MAP(CTenMinView, CFormView)
	//{{AFX_MSG_MAP(CTenMinView)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_10MIN_PERIOD_LST, OnSelchange10minPeriodLst)
	ON_NOTIFY(UDN_DELTAPOS, IDC_START_MIN_SPIN, OnDeltaposStartMinSpin)
	ON_BN_CLICKED(IDC_ACCEPT, OnAccept)
	ON_BN_CLICKED(IDC_REJECT, OnReject)
	ON_NOTIFY(UDN_DELTAPOS, IDC_END_MIN_SPIN, OnDeltaposEndMinSpin)
	ON_BN_CLICKED(IDC_UNDO_CHANGES, OnUndoChanges)
	ON_BN_CLICKED(IDC_ACCEPT_CHANGES, OnAcceptChanges)
	//}}AFX_MSG_MAP
  ON_NOTIFY(UDN_DELTAPOS, IDC_START_PERIOD_MIN_SPIN, OnDeltaposStartPeriodMinSpin)
  ON_NOTIFY(UDN_DELTAPOS, IDC_END_PERIOD_MIN_SPIN, OnDeltaposEndPeriodMinSpin)
  ON_WM_VSCROLL()
  ON_EN_CHANGE(IDC_START_MIN, OnEnChangeStartMin)
  ON_EN_CHANGE(IDC_END_MIN, OnEnChangeEndMin)
  ON_EN_CHANGE(IDC_START_PERIOD_MIN, OnEnChangeStartPeriodMin)
  ON_EN_CHANGE(IDC_END_PERIOD_MIN, OnEnChangeEndPeriodMin)
  ON_BN_CLICKED(IDC_CHK_SET_PERIOD_LENGTH, OnBnClickedChkSetPeriodLength)
  ON_BN_CLICKED(IDC_CHK_SET_GOOD_MINUTES, OnBnClickedChkSetGoodMinutes)
//  ON_BN_CLICKED(IDC_DELETE_THIS_PERIOD, OnBnClickedDeleteThisPeriod)
//  ON_BN_CLICKED(IDC_INSERT_NEW_PERIOD, OnBnClickedInsertNewPeriod)
  ON_NOTIFY(UDN_DELTAPOS, IDC_START_GOOD_MIN_SPIN, OnDeltaposStartGoodMinSpin)
  ON_NOTIFY(UDN_DELTAPOS, IDC_END_GOOD_MIN_SPIN, OnDeltaposEndGoodMinSpin)
  ON_EN_CHANGE(IDC_START_GOOD_MIN, OnEnChangeStartGoodMin)
  ON_EN_CHANGE(IDC_END_GOOD_MIN, OnEnChangeEndGoodMin)
  ON_LBN_SELCHANGE(IDC_EVENT_LIST, OnLbnSelchangeEventList)
  ON_LBN_DBLCLK(IDC_EVENT_LIST, OnLbnDblclkEventList)
  ON_BN_CLICKED(IDC_BTN_EVENTS_EDIT, OnBnClickedBtnEventsEdit)
  ON_BN_CLICKED(IDC_BTN_EVENTS_DELETE, OnBnClickedBtnEventsDelete)
  ON_BN_CLICKED(IDC_RESET_PERIODS, OnBnClickedResetPeriods)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTenMinView message handlers

/********************************************************************
OnInitialUpdate

  Perform initialization of the form view.
********************************************************************/
void CTenMinView::OnInitialUpdate() 
  {
  CFont cfBoldFont;
  CString cstrText; //, cstrTemp;
  CDC *pDC;

  m_bGraphScrolled = false;

  pDC = GetDC();
  pDC->SetBkColor(RGB(255,255,255));
  ReleaseDC(pDC);

	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
  m_pDoc =(CEGGSASDoc *)GetDocument();

  if(theApp.m_ptPgmType == PT_STD_RSCH)
    {
    GetDlgItem(IDC_STUDY_TYPE)->ShowWindow(SW_SHOW);
    }
  else
    {
#ifndef _200_CPM_HARDWARE
    GetDlgItem(IDC_STUDY_TYPE)->ShowWindow(SW_HIDE);
#endif
    }

    // Fill in the 10 minute period combobox.
    // The data item contains the information that tells what 10 minute
    // period the item is.
//  cstrText.LoadString(IDS_BASELINE);
//  iIndex = m_c10MinPeriodLst.AddString(cstrText);
//  m_c10MinPeriodLst.SetItemData(iIndex, (DWORD)PERIOD_BL);
  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard study
    vBuild_standard_period_cbx();
//    cstrText.LoadString(IDS_FIRST_10_MIN_PD);
//    iIndex = m_c10MinPeriodLst.AddString(cstrText);
//    m_c10MinPeriodLst.SetItemData(iIndex, (DWORD)PERIOD_10);
//    cstrText.LoadString(IDS_SECOND_10_MIN_PD);
//    iIndex = m_c10MinPeriodLst.AddString(cstrText);
//    m_c10MinPeriodLst.SetItemData(iIndex, (DWORD)PERIOD_20);
//    cstrText.LoadString(IDS_THIRD_10_MIN_PD);
//    iIndex = m_c10MinPeriodLst.AddString(cstrText);
//    m_c10MinPeriodLst.SetItemData(iIndex, (DWORD)PERIOD_30);
    }
  else
    { // Research version
    m_iNumMinOnGraph = m_pDoc->m_iMinPerGraph;
    m_iFirstMinOnGraph = 1;
    m_cGraphScrollBar.EnableWindow(FALSE);
    vInit_period_good_mins();
//    m_bSetPeriodLength = FALSE;
//    m_bSetGoodMinutes = FALSE;
//    vAdd_menu_items_for_period();
    if(m_pDoc->m_bUsingWhatIf == true)
      { // Displaying a WhatIf.
      vShow_WhatIf(true, m_pDoc->m_wifOrigData.cstrDescription);
      }

  // For Central Data Depot Reader fields are always disabled.
#if EGGSAS_READER == READER_DEPOT
  if(m_pDoc->m_bUsingWhatIf == false)
    { // Changes allowed for WhatIF
    vEnable_research_period_fields(false);
    vEnable_research_goodmin_fields(false);
    GetDlgItem(IDC_RESET_PERIODS)->EnableWindow(FALSE);
    }
  GetDlgItem(IDC_BTN_EVENTS_EDIT)->EnableWindow(FALSE);
  GetDlgItem(IDC_BTN_EVENTS_DELETE)->EnableWindow(FALSE);
#endif
    }

  m_c10MinPeriodLst.SetCurSel(0);
    // Disable the combobox as there is no data yet.
  m_c10MinPeriodLst.EnableWindow(FALSE);

  m_hPEData1 = NULL;
  m_hPEData2 = NULL;
  m_hPEData3 = NULL;

/**************************************
    // Get the font to use in "Product of" field.
  vSet_font(this, NULL, &cfBoldFont, FW_BOLD);
    // Set the font in the "Product of" field.
  GetDlgItem(IDC_PRODUCT_OF)->SetFont(&cfBoldFont);
*********************************************/

  m_bInitialUpdate = true;

  UpdateData(FALSE);
  }

HBRUSH CTenMinView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

/********************************************************************
OnDraw

  Message handler for drawing the view.
    - Center the logo.
********************************************************************/
void CTenMinView::OnDraw(CDC* /*pDC*/)
  {
  CWnd *pcwndC;
  RECT rWin, rFieldC;
  int iWidth, iHeight, iYC;
/*********************
#ifdef EZEM_LOGO
  CWnd *pcwnd, *pcwndB;
  RECT rFieldB, rFieldProd;
  int iY, iYB;
#endif
******************************/

  // TODO: Add your specialized code here and/or call the base class
  GetClientRect(&rWin); // Dimensions of dialog box client area.
/*************************************************************
#ifdef EZEM_LOGO
    // Display both the EZEM logo and the 3CPM logo.
  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard version
      // Move the EZEM VISIPACE logo to the center of the screen.
    pcwndC = GetDlgItem(IDC_VISISPACE_LOGO);
      // Move 3CPM logo to the bottom center of the screen.
    pcwndB = GetDlgItem(IDC_LOGO_3CPM);
    }
  else
    { // Research version
      // Move 3CPM logo to the center of the screen.
    pcwndC = GetDlgItem(IDC_LOGO_3CPM);
      // Move the EZEM VISIPACE logo to the bottom center of the screen.
    pcwndB = GetDlgItem(IDC_VISISPACE_LOGO);
    }

  pcwndC->GetWindowRect(&rFieldC); // Dimension of the logo window.
  iWidth = rFieldC.right - rFieldC.left; // Width of logo.
  iHeight = rFieldC.bottom - rFieldC.top; // Height of logo.
    // Save the Y-coordinate.
  iYC = ((rWin.bottom - rWin.top) / 2) - (iHeight / 2);
    // First input is X coordinate.
    // Second input is Y coordinate.
  pcwndC->MoveWindow(((rWin.right - rWin.left) / 2) - (iWidth / 2),
                     iYC, iWidth, iHeight);
    // Move 3CPM logo to the bottom center of the screen.
  GetClientRect(&rWin); // Dimensions of dialog box client area.
  pcwndB->GetWindowRect(&rFieldB); // Dimension of the logo window.
  iWidth = rFieldB.right - rFieldB.left; // Width of logo.
  iHeight = rFieldB.bottom - rFieldB.top; // Height of logo.
    // Save the Y-coordinate as this is where the "Product of" field goes.
  iYB = rWin.bottom - rWin.top - iHeight;
    // First input is X coordinate.
    // Second input is Y coordinate.
  pcwndB->MoveWindow(((rWin.right - rWin.left) / 2) - (iWidth / 2),
                     iYB, iWidth, iHeight);
    // Move "Product of" to the center of the screen over the 3CPM logo.
  pcwnd = GetDlgItem(IDC_PRODUCT_OF);
  pcwnd->GetWindowRect(&rFieldProd); // Dimension of the logo window.
  iWidth = rFieldProd.right - rFieldProd.left; // Width of logo.
  iHeight = rFieldProd.bottom - rFieldProd.top; // Height of logo.
  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard version
    iY = iYB - iHeight;
    }
  else
    { // Research Version
    iY = iYC - iHeight;
    }
    // First input is X coordinate.
    // Second input is Y coordinate.
  pcwnd->MoveWindow(((rWin.right - rWin.left) / 2) - (iWidth / 2),
                    iY, iWidth, iHeight);
#else
********************************************************/
  // Display only the 3CPM logo.
    // Move the 3CPM logo to the center of the screen.
  pcwndC = GetDlgItem(IDC_LOGO_3CPM);
  pcwndC->GetWindowRect(&rFieldC); // Dimension of the logo window.
  iWidth = rFieldC.right - rFieldC.left; // Width of logo.
  iHeight = rFieldC.bottom - rFieldC.top; // Height of logo.
    // Save the Y-coordinate.
  iYC = ((rWin.bottom - rWin.top) / 2) - (iHeight / 2);
    // First input is X coordinate.
    // Second input is Y coordinate.
  pcwndC->MoveWindow(((rWin.right - rWin.left) / 2) - (iWidth / 2),
                     iYC, iWidth, iHeight);
/********************************************
    // Hide the EZEM VISIPACE logo and the Product Of.
  GetDlgItem(IDC_VISISPACE_LOGO)->ShowWindow(FALSE);
  GetDlgItem(IDC_PRODUCT_OF)->ShowWindow(FALSE);

#endif
*********************************************/
//#ifdef _200_CPM_HARDWARE
//  if(theApp.m_ptPgmType == PT_RESEARCH || theApp.m_ptPgmType == PT_STD_RSCH)
//    {
//    if(theApp.m_bFirstTimeInTenMinView  == true)
//      {
//      int iFreqRange;
//      theApp.m_bFirstTimeInTenMinView = false;
//      CFreqRange *pdlg = new CFreqRange();
//      pdlg->vSet_data(g_pConfig->iGet_item(CFG_FREQ_RANGE));
//      if(pdlg->DoModal() == IDOK)
//        {
//        iFreqRange = pdlg->iGet_freq_range();
//        g_pConfig->vWrite_item(CFG_FREQ_RANGE, iFreqRange);
//        theApp.vSet_freq_range(iFreqRange);
//        }
//      delete pdlg;
//      }
//    }
//#endif
  
  }

/********************************************************************
OnSelchange10minPeriodLst

  Message handler for selecting the period to view from the combobox.
********************************************************************/
void CTenMinView::OnSelchange10minPeriodLst() 
  {
  int iSel;
	
  UpdateData(TRUE);
    // Get the selected item.
  iSel = m_c10MinPeriodLst.GetCurSel();
  m_pDoc->m_uPreviousPeriod = m_pDoc->m_uCurrentPeriod;
  m_pDoc->m_uCurrentPeriod = (unsigned short)m_c10MinPeriodLst.GetItemData(iSel);
  if(m_pDoc->bGet_data_graph(false, m_uMenuType) == false)
    vUpdate_period_combobox(m_pDoc->m_uCurrentPeriod);
  }

/********************************************************************
OnDeltaposStartMinSpin

  Standard Version
    Message handler for changing the Start minute spin button.
      Update the associated edit field by +-0.5.
  Research Version
    Message handler for changing the Start Good Minute spin button.
      Update the associated edit field by +-0.5.
********************************************************************/
void CTenMinView::OnDeltaposStartMinSpin(NMHDR* pNMHDR, LRESULT* pResult) 
  {

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
  UpdateData(TRUE);
  if(m_fStart != (float)atof(m_cstrStartMin))
    {
    m_fStart = (float)atof(m_cstrStartMin);
    m_fSpinEndLoLim = m_fStart;
    }
  if(m_fEnd != (float)atof(m_cstrEndMin))
    {
    m_fEnd = (float)atof(m_cstrEndMin);
    m_fSpinStartHiLim = m_fEnd;
     }
  if(pNMUpDown->iDelta < 0) //== -1)
    { // Down arrow
    if(m_fStart > m_fSpinStartLoLim)
      {
      m_fStart -= MINUTE_DELTA; //0.5F;
      m_bSetGoodMinutes = FALSE;
      }
    }
  else
    { // Up arrow
    if((m_fEnd - (m_fStart + MINUTE_DELTA /*0.5F*/)) >= 4)
      { // After increment, there are still 4 minutes left.
      if(m_fStart < m_fSpinStartHiLim)
        {
        m_fStart += MINUTE_DELTA; //0.5F;
        m_bSetGoodMinutes = FALSE;
        }
      }
    }
  m_cstrStartMin.Format("%.1f", m_fStart);
  m_fStart -= m_fSpinStart;
  m_fEnd -= m_fStart;
/****	
  m_fStart = (float)atof(m_cstrStartMin);
  m_fEnd = (float)atof(m_cstrEndMin);
  if(pNMUpDown->iDelta < 0) //== -1)
    { // Down arrow
    if(m_fSpinStart < m_fStart)
      m_fStart -= 0.5F;
    }
  else
    { // Up arrow
    if((m_fEnd - (m_fStart + 0.5F)) >= 4)
      { // After increment, there are still 4 minutes left.
      if(m_fSpinEnd > m_fStart)
        m_fStart += 0.5F;
      }
    }
  m_cstrStartMin.Format("%.1f", m_fStart);
  m_fStart -= m_fSpinStart;
  m_fEnd -= m_fStart;
***********/
  UpdateData(FALSE);

	*pResult = 0;
  }

/********************************************************************
OnDeltaposEndMinSpin

  Standard Version
    Message handler for changing the End minute spin button.
      Update the associated edit field by +-0.5.
  Research Version
    Message handler for changing the End Good Minute spin button.
      Update the associated edit field by +-0.5.
********************************************************************/
void CTenMinView::OnDeltaposEndMinSpin(NMHDR* pNMHDR, LRESULT* pResult) 
  {

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
  UpdateData(TRUE);
	
  if(m_fStart != (float)atof(m_cstrStartMin))
    {
    m_fStart = (float)atof(m_cstrStartMin);
    m_fSpinEndLoLim = m_fStart;
    }
  if(m_fEnd != (float)atof(m_cstrEndMin))
    {
    m_fEnd = (float)atof(m_cstrEndMin);
    m_fSpinStartHiLim = m_fEnd;
     }
  if(pNMUpDown->iDelta < 0) //== -1)
    { // Down arrow
    if(((m_fEnd - MINUTE_DELTA /*0.5F*/) - m_fStart) >= 4)
      { // After decrement, there are still 4 minutes left.
      if(m_fEnd > m_fSpinEndLoLim)
        {
        m_fEnd -= MINUTE_DELTA; //0.5F;
        m_bSetGoodMinutes = FALSE;
        }
      }
    }
  else
    { // Up arrow
    if(m_fEnd < m_fSpinEndHiLim)
      {
      m_fEnd += MINUTE_DELTA; //0.5F;
      m_bSetGoodMinutes = FALSE;
      }
    }
  m_cstrEndMin.Format("%.1f", m_fEnd);
  m_fStart -= m_fSpinStart;
  m_fEnd -= m_fStart;
/**
  m_fStart = (float)atof(m_cstrStartMin);
  m_fEnd = (float)atof(m_cstrEndMin);
  if(pNMUpDown->iDelta < 0) //== -1)
    { // Down arrow
    if(((m_fEnd - 0.5F) - m_fStart) >= 4)
      { // After decrement, there are still 4 minutes left.
      if(m_fSpinStart < m_fEnd)
        m_fEnd -= 0.5F;
      }
    }
  else
    { // Up arrow
    if(m_fSpinEnd > m_fEnd)
      m_fEnd += 0.5F;
    }
  m_cstrEndMin.Format("%.1f", m_fEnd);
  m_fStart -= m_fSpinStart;
  m_fEnd -= m_fStart;
****/
  UpdateData(FALSE);
	
	*pResult = 0;
  }

/********************************************************************
OnDeltaposStartPeriodMinSpin

  Research Version
    Message handler for changing the Period Start Minute spin button.
      Update the associated edit field by +-0.5.
********************************************************************/
void CTenMinView::OnDeltaposStartPeriodMinSpin(NMHDR *pNMHDR, LRESULT *pResult)
  {
  float fStart, fEnd;

  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  fStart = (float)atof(m_cstrPeriodStartMin);
  fEnd = (float)atof(m_cstrPeriodEndMin);

  if(pNMUpDown->iDelta < 0) //== -1)
    { // Down arrow
    if(fStart > m_fSpinPeriodStartLoLim)
      {
      fStart -= MINUTE_DELTA; //0.5F;
      m_bSetPeriodLength = FALSE;
      m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
      }
    }
  else
    { // Up arrow
    if((fEnd - (fStart + MINUTE_DELTA)) >= 4)
      { // After increment, there are still 4 minutes left.
      if(fStart < m_fSpinPeriodStartHiLim)
        {
        fStart += MINUTE_DELTA; //0.5F;
        m_bSetPeriodLength = FALSE;
        m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
        }
      }
    }
  m_cstrPeriodStartMin.Format("%.1f", fStart);
  UpdateData(FALSE);

	*pResult = 0;
  }

/********************************************************************
OnDeltaposEndPeriodMinSpin

  Research Version
    Message handler for changing the Period End Minute spin button.
      Update the associated edit field by +-0.5.
********************************************************************/
void CTenMinView::OnDeltaposEndPeriodMinSpin(NMHDR *pNMHDR, LRESULT *pResult)
  {
  float fStart, fEnd;

  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
	
  fStart = (float)atof(m_cstrPeriodStartMin);
  fEnd = (float)atof(m_cstrPeriodEndMin);
  if(pNMUpDown->iDelta < 0) //== -1)
    { // Down arrow
    if(((fEnd - MINUTE_DELTA) - m_fSpinPeriodStartLoLim) >= 4)
      { // After decrement, there are still 4 minutes left.
      if(fEnd > m_fSpinPeriodEndLoLim)
        {
        fEnd -= MINUTE_DELTA;
        m_bSetPeriodLength = FALSE;
        m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
        }
      }
    }
  else
    { // Up arrow
    if(fEnd < m_fSpinPeriodEndHiLim)
      {
      fEnd += MINUTE_DELTA; //0.5F;
      m_bSetPeriodLength = FALSE;
      m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
      }
    }
  m_cstrPeriodEndMin.Format("%.1f", fEnd);
  UpdateData(FALSE);
	
  *pResult = 0;
  }

/********************************************************************
OnDeltaposStartGoodMinSpin

  Research Version
    Message handler for changing the Start Good Minute spin button.
      Update the associated edit field by +-0.5.
********************************************************************/
void CTenMinView::OnDeltaposStartGoodMinSpin(NMHDR *pNMHDR, LRESULT *pResult)
  {

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
  UpdateData(TRUE);
  if(m_fStart != (float)atof(m_cstrStartGoodMin))
    {
    m_fStart = (float)atof(m_cstrStartGoodMin);
    m_fSpinEndLoLim = m_fStart;
    }
  if(m_fEnd != (float)atof(m_cstrEndGoodMin))
    {
    m_fEnd = (float)atof(m_cstrEndGoodMin);
    m_fSpinStartHiLim = m_fEnd;
     }
  if(pNMUpDown->iDelta < 0) //== -1)
    { // Down arrow
    if(m_fStart > m_fSpinStartLoLim)
      {
      m_fStart -= MINUTE_DELTA; //0.5F;
      m_bSetGoodMinutes = FALSE;
      m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
      }
    }
  else
    { // Up arrow
    if((m_fEnd - (m_fStart + MINUTE_DELTA /*0.5F*/)) >= 4)
      { // After increment, there are still 4 minutes left.
      if(m_fStart < m_fSpinStartHiLim)
        {
        m_fStart += MINUTE_DELTA; //0.5F;
        m_bSetGoodMinutes = FALSE;
        m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
        }
      }
    }
  m_cstrStartGoodMin.Format("%.1f", m_fStart);
  m_fStart -= m_fSpinStart;
  m_fEnd -= m_fStart;
  UpdateData(FALSE);

	*pResult = 0;
  }

/********************************************************************
OnDeltaposEndGoodMinSpin

  Research Version
    Message handler for changing the End Good Minute spin button.
      Update the associated edit field by +-0.5.
********************************************************************/
void CTenMinView::OnDeltaposEndGoodMinSpin(NMHDR *pNMHDR, LRESULT *pResult)
  {

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
  UpdateData(TRUE);
	
  if(m_fStart != (float)atof(m_cstrStartGoodMin))
    {
    m_fStart = (float)atof(m_cstrStartGoodMin);
    m_fSpinEndLoLim = m_fStart;
    }
  if(m_fEnd != (float)atof(m_cstrEndGoodMin))
    {
    m_fEnd = (float)atof(m_cstrEndGoodMin);
    m_fSpinStartHiLim = m_fEnd;
     }
  if(pNMUpDown->iDelta < 0) //== -1)
    { // Down arrow
    if(((m_fEnd - MINUTE_DELTA /*0.5F*/) - m_fStart) >= 4)
      { // After decrement, there are still 4 minutes left.
      if(m_fEnd > m_fSpinEndLoLim)
        {
        m_fEnd -= MINUTE_DELTA; //0.5F;
        m_bSetGoodMinutes = FALSE;
        m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
        }
      }
    }
  else
    { // Up arrow
    if(m_fEnd < m_fSpinEndHiLim)
      {
      m_fEnd += MINUTE_DELTA; //0.5F;
      m_bSetGoodMinutes = FALSE;
      m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
      }
    }
  m_cstrEndGoodMin.Format("%.1f", m_fEnd);
  m_fStart -= m_fSpinStart;
  m_fEnd -= m_fStart;
  UpdateData(FALSE);
	
	*pResult = 0;
  }

/********************************************************************
OnBnClickedResetPeriods

  Research Version
    Message handler for changing the "Reset Periods" button.
      Reset all the periods and good minutes so that there is just
        a baseline period and 1 study period.
********************************************************************/
void CTenMinView::OnBnClickedResetPeriods()
  {
  int iIndex;
  CString cstrTitle, cstrMsg;

  //cstrTitle.LoadString(IDS_ERROR1);
  cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
  //cstrMsg.LoadString(IDS_RESET_PERIODS_QUESTION);
  cstrMsg = g_pLang->cstrLoad_string(ITEM_G_RESET_PERIODS_QUESTION)
    + "\n" + g_pLang->cstrLoad_string(ITEM_G_RESET_PERIODS_QUESTION1);
  if(MessageBox(cstrMsg, cstrTitle, MB_YESNO) == IDYES)
    {
    for(iIndex = 0; iIndex < MAX_TOTAL_PERIODS; ++iIndex)
      {
      m_pDoc->m_pdPatient.fPeriodLen[iIndex] = (float)INV_PERIOD_LEN;
      m_pDoc->m_pdPatient.fPeriodStart[iIndex] = (float)INV_PERIOD_START;
      m_pDoc->m_pdPatient.fGoodMinStart[iIndex] = INV_MINUTE;
      m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] = INV_MINUTE;
      }
    m_pDoc->vReset_periods_analyzed();
      // Baseline
    m_pDoc->m_pdPatient.fPeriodStart[PERIOD_BL] = (float)0.0;
    m_pDoc->m_pdPatient.fPeriodLen[PERIOD_BL] = fRound_down_to_half_min(
        (float)m_pDoc->m_pdPatient.uDataPtsInBaseline / (float)DATA_POINTS_PER_MIN_1);
      // Study
    m_pDoc->m_pdPatient.uNumPeriods = 1;
    m_pDoc->m_pdPatient.fPeriodStart[1] = (float)0.0;
    m_pDoc->m_pdPatient.fPeriodLen[1] = 
               fRound_down_to_half_min(
               (float)m_pDoc->m_pdPatient.uPostStimDataPoints
               / (float)(DATA_POINTS_PER_MIN_1 * 2));
      // Reset the period list combobox.
    m_uPeriod = PERIOD_BL;
    m_pDoc->m_uCurrentPeriod = m_uPeriod;
    m_pDoc->m_uPreviousPeriod = m_pDoc->m_uCurrentPeriod;
    vUpdate_period_combobox(m_uPeriod);
      // Reset the period lists in the menus.
    m_pDoc->vInit_stim_period_menus();
    m_pDoc->vSetup_menus();
    vAdd_menu_items_for_period();
      // Uncheck the period length and good minutes checkboxes.
    m_bSetPeriodLength = false;
    m_bSetGoodMinutes = false;
    m_pDoc->vGotoStart(); // go to the baseline period.
    }
  }

/********************************************************************
OnBnClickedDeleteThisPeriod

  Research Version
    Message handler for changing the "Delete this period" button.
      Delete the period.
      Remove it from all lists.
********************************************************************/
//void CTenMinView::OnBnClickedDeleteThisPeriod()
//  {
//  CString cstrTitle, cstrMsg;
//  unsigned short uIndex;
//
//    // Ask if user is sure first.
//  cstrTitle.LoadString(IDS_WARNING);
//  cstrMsg.LoadString(IDS_ASK_DELETE_PERIOD);
//  if(MessageBox(cstrMsg, cstrTitle, MB_YESNO) == IDYES)
//    { // User wants to delete the period.
//    m_pDoc->vRemove_period_menu_item(m_uPeriod);
//    m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
//    if(m_uPeriod == (MAX_TOTAL_PERIODS - 1)
//    || m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod + 1] == INV_PERIOD_START)
//      { // User is removing the last period.
//      m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod] = INV_PERIOD_START;
//      m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod] = INV_PERIOD_LEN;
//      m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod] = INV_MINUTE;
//      m_pDoc->m_pdPatient.fGoodMinEnd[m_uPeriod] = INV_MINUTE;
//      --m_uPeriod;
//      --m_pDoc->m_uCurrentPeriod;
//      --m_pDoc->m_pdPatient.uNumPeriods;
//      }
//    else
//      {
//      // Remove the period and remove the good minutes.
//      uIndex = m_uPeriod;
//      while(m_pDoc->m_pdPatient.fPeriodStart[uIndex + 1] != INV_PERIOD_START)
//        {
//        m_pDoc->m_pdPatient.fPeriodStart[uIndex]
//                 = m_pDoc->m_pdPatient.fPeriodStart[uIndex + 1];
//        m_pDoc->m_pdPatient.fPeriodLen[uIndex]
//                 = m_pDoc->m_pdPatient.fPeriodLen[uIndex + 1];
//        m_pDoc->m_pdPatient.fGoodMinStart[uIndex]
//                 = m_pDoc->m_pdPatient.fGoodMinStart[uIndex + 1];
//        m_pDoc->m_pdPatient.fGoodMinEnd[uIndex]
//                 = m_pDoc->m_pdPatient.fGoodMinEnd[uIndex + 1];
//        m_pDoc->m_pdPatient.bPeriodsAnalyzed[uIndex]
//                 = m_pDoc->m_pdPatient.bPeriodsAnalyzed[uIndex + 1];
//        ++uIndex;
//        }
//      --m_pDoc->m_pdPatient.uNumPeriods;
//        // No more minutes left in study, remove the last period.
//      m_pDoc->m_pdPatient.fPeriodStart[uIndex] = INV_PERIOD_START;
//      m_pDoc->m_pdPatient.fPeriodLen[uIndex] = INV_PERIOD_LEN;
//      m_pDoc->m_pdPatient.fGoodMinStart[uIndex] = INV_MINUTE;
//      m_pDoc->m_pdPatient.fGoodMinEnd[uIndex] = INV_MINUTE;
//      m_pDoc->m_pdPatient.bPeriodsAnalyzed[uIndex] = false;
//      if(m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod] == INV_PERIOD_START)
//        {
//        --m_uPeriod;
//        --m_pDoc->m_uCurrentPeriod;
//        }
//      }
//      // Remove menu items for the removed period.
//    vUpdate_period_combobox(m_uPeriod);
//    m_pDoc->vGotoCurrent();
//    }
//  }

/********************************************************************
OnBnClickedInsertNewPeriod

  Research Version
    Message handler for changing the "Insert new period" button.
      Insert a period.
      Add it to all lists.
********************************************************************/
//void CTenMinView::OnBnClickedInsertNewPeriod()
//  {
//  CString cstrTitle, cstrMsg;
//  bool bOkToInsert = true;
//  unsigned short uIndex;
//  float fMinutes;
//
////  if(m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod] == INV_PERIOD_LEN)
//  if(m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] == false)
//    { // Current period has not been analyzed and it must be.
//    cstrTitle.LoadString(IDS_WARNING);
//    cstrMsg.LoadString(IDS_THIS_PERIOD_MUST_BE_ANALYZED);
//    MessageBox(cstrMsg, cstrTitle, MB_OK);
//    bOkToInsert = false;
//    }
//  else if(m_uPeriod == 1)
//    { // First post-stimulation period.  If not at 4 minutes before the period
//      // start minute, user can't insert a period.
//    if(m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod] < (float)MINUTES_PER_EPOCH)
//      {
//      cstrTitle.LoadString(IDS_WARNING);
//      cstrMsg.LoadString(IDS_INSERT_PERIOD_NO_MINUTES);
//      MessageBox(cstrMsg, cstrTitle, MB_OK);
//      bOkToInsert = false;
//      }
//    }
//  else if(m_uPeriod > 1)
//    {
////    if(m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod - 1] == INV_PERIOD_LEN)
//    if(m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod - 1] == false)
//      { // Previous period has not been analyzed and it must be.
//      cstrTitle.LoadString(IDS_WARNING);
//      cstrMsg.LoadString(IDS_PREV_PERIOD_MUST_BE_ANALYZED);
//      MessageBox(cstrMsg, cstrTitle, MB_OK);
//      bOkToInsert = false;
//      }
//    else
//      { // Prior period has been analyzed.
//        // Make sure there is enough minutes between this period and the prior period.
//      fMinutes = m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod]
//                 - (m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod - 1]
//                 + m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod - 1]);
//      if(fMinutes < (float)MINUTES_PER_EPOCH)
//        {
//        cstrTitle.LoadString(IDS_WARNING);
//        cstrMsg.LoadString(IDS_INSERT_PERIOD_NO_MINUTES);
//        MessageBox(cstrMsg, cstrTitle, MB_OK);
//        bOkToInsert = false;
//        }
//      }
///**********************
//    if(bOkToInsert == true)
//      { // Make sure there is enough minutes between this period and the prior period.
//        // Prior period must also have been analyzed.
//      if(m_uPeriod == m_pDoc->m_pdPatient.uNumPeriods)
//        fMinutes = m_pDoc->m_fStudyLen;
//      else
//        fMinutes = m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod + 1];
//      fMinutes -= (m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod - 1]
//                  + m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod - 1]);
//      if(fMinutes < (float)MINUTES_PER_EPOCH)
//        {
//        cstrTitle.LoadString(IDS_WARNING);
//        cstrMsg.LoadString(IDS_INSERT_PERIOD_NO_MINUTES);
//        MessageBox(cstrMsg, cstrTitle, MB_OK);
//        bOkToInsert = false;
//        }
//      }
//***************************/
//    }
//  else if(m_pDoc->m_pdPatient.uNumPeriods == MAX_TOTAL_PERIODS)
//    { // At maximum number of periods.
//    cstrTitle.LoadString(IDS_WARNING);
//    cstrMsg.LoadString(IDS_MAX_PERIODS_REACHED);
//    MessageBox(cstrMsg, cstrTitle, MB_OK);
//    bOkToInsert = false;
//    }
//  if(bOkToInsert == true)
//    { // Inserting new period.
//    ++m_pDoc->m_pdPatient.uNumPeriods;
//    for(uIndex = m_pDoc->m_pdPatient.uNumPeriods; uIndex > m_uPeriod; --uIndex)
//      {
//      m_pDoc->m_pdPatient.fPeriodStart[uIndex]
//               = m_pDoc->m_pdPatient.fPeriodStart[uIndex - 1];
//      m_pDoc->m_pdPatient.fPeriodLen[uIndex]
//               = m_pDoc->m_pdPatient.fPeriodLen[uIndex - 1];
//      m_pDoc->m_pdPatient.fGoodMinStart[uIndex]
//               = m_pDoc->m_pdPatient.fGoodMinStart[uIndex - 1];
//      m_pDoc->m_pdPatient.fGoodMinEnd[uIndex]
//               = m_pDoc->m_pdPatient.fGoodMinEnd[uIndex - 1];
//      m_pDoc->m_pdPatient.bPeriodsAnalyzed[uIndex]
//               = m_pDoc->m_pdPatient.bPeriodsAnalyzed[uIndex - 1];
//      }
//      // No more minutes left in study, remove the last period.
//    m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod] = INV_PERIOD_START;
//    m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod] = INV_PERIOD_LEN;
//    m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod] = INV_MINUTE;
//    m_pDoc->m_pdPatient.fGoodMinEnd[m_uPeriod] = INV_MINUTE;
//    m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
//    vAdd_menu_items_for_period();
//    vUpdate_period_combobox(m_uPeriod);
//    m_pDoc->vGotoCurrent();
//    }
//  }

/********************************************************************
OnAccept

  Message handler for changing the Accept button.
    Validate the selected minutes:
      - There must be at least 4 minutes.
    If this is the baseline period, set the artifact limits for later
      periods.
********************************************************************/
void CTenMinView::OnAccept() 
  {
  CString cstrMsg, cstrTitle;

  UpdateData(TRUE);

  if((m_fEnd - m_fStart) < 4)
    { // Need 4 minutes.
    //cstrMsg.LoadString(IDS_NEED_4_MINUTES);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_NEED_4_MINUTES);
    //cstrTitle.LoadString(IDS_ERROR1);
    cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    MessageBox(cstrMsg, cstrTitle);
    }
  else
    { // Have at least 4 minutes.
      // Save the selected minutes.
    m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod] = m_fStart;
    m_pDoc->m_pdPatient.fGoodMinEnd[m_uPeriod] = m_fEnd;
      // Indicate that the period has been analyzed.
    m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = true;
    if(m_uPeriod == PERIOD_BL)
      { // This is the baseline period.  It requires additional processing.
        // If there were previously periods analyzed, they may have been reset
        // if user had previously selected the reject button, so we should
        // put them back.
      vSet_periods_analyzed_from_orig();
        // Find the max baseline signal during the good minutes just selected.
      m_pDoc->m_pdPatient.fMeanBaseline = 
                     m_pDoc->m_pAnalyze->fFind_mean_signal(m_pDoc->m_fData, m_fStart, m_fEnd);
      if(m_pDoc->m_pArtifact != NULL)
        m_pDoc->m_pArtifact->vSet_artifact_limit(m_pDoc->m_pdPatient.fMeanBaseline);
      }

      // Hide the additional dialog box fields.
    vShow_min_sel_fields(SW_HIDE);
    UpdateData(FALSE);
    }
  }

/********************************************************************
OnReject

  Message handler for changing the Reject button.
    Show the minute selection fields.
    If this is the baseline period
      - reset all the periods analyzed
      - the artifact limits.
********************************************************************/
void CTenMinView::OnReject() 
  {
  short int iIndex;
	
  if(m_pDoc->m_bHaveFile == TRUE)
    {
    m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod] = INV_MINUTE;
    m_pDoc->m_pdPatient.fGoodMinEnd[m_uPeriod] = INV_MINUTE;
      // Indicate that the period has not been analyzed.
    m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
    if(m_uPeriod == PERIOD_BL)
      { // This is the baseline period.  It requires additional processing.
        // Reset the baseline mean
      m_pDoc->m_pdPatient.fMeanBaseline = INV_FLOAT;
        // Reset all the periods analyzed.
      m_pDoc->vReset_periods_analyzed();
        // Reset the selected minutes for all periods.
      for(iIndex = 0; iIndex < MAX_MANUAL_MODE_PERIODS; ++iIndex)
        {
        m_pDoc->m_pdPatient.fGoodMinStart[iIndex] = INV_MINUTE;
        m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] = INV_MINUTE;
        if(m_pDoc->m_pArtifact != NULL && iIndex != 0)
          m_pDoc->m_pArtifact->vReset_artifact_list(iIndex);
        }
        // Reset the artifact limit and the artifact list.
      if(m_pDoc->m_pArtifact != NULL)
        m_pDoc->m_pArtifact->vSet_artifact_limit(m_pDoc->m_pdPatient.fMeanBaseline);
      }
    vShow_min_sel_fields(SW_SHOW);
    }
  }

/********************************************************************
OnUndoChanges

  Message handler for changing the Undo Changes button.
    Put back the original minute selection.
    Mark the period as analyzed if it was previously analyzed.
    If this is the baseline period:
      - Restore the indicatiors of all the periods that were previously
        analyzed.
      - Restore the artifact limits.
********************************************************************/
void CTenMinView::OnUndoChanges() 
  {
  short int iIndex;
  float fPeriodStart;

  if(m_fOrigStart == m_fStart && m_fOrigEnd == m_fEnd)
    { // The selected minutes have not changed, so undo what we did
      // when the minutes were rejected.
      // Indicate that the period has been analyzed if it was previously analyzed.
    if(m_bOrigPeriodsAnalyzed[m_uPeriod] != true)
      m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = true;
    if(m_uPeriod == PERIOD_BL)
      { // This is the baseline period.  It requires additional processing.
        // If there were previously periods analyzed, they may been reset
        // when user had previously selected the reject button, so we should
        // put them back if the selected minutes are unchanged.
      vSet_periods_analyzed_from_orig();
        // Find the max baseline signal during the good minutes just selected.
      m_pDoc->m_pdPatient.fMeanBaseline = m_fOrigMeanBaseline;
      if(m_pDoc->m_pArtifact != NULL)
        m_pDoc->m_pArtifact->vSet_artifact_limit(m_pDoc->m_pdPatient.fMeanBaseline);
        // Restore the selected minutes.
      for(iIndex = 0; iIndex < MAX_MANUAL_MODE_PERIODS; ++iIndex)
        {
        m_pDoc->m_pdPatient.fGoodMinStart[iIndex] = m_fOrigGoodMinStart[iIndex];
        m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] = m_fOrigGoodMinEnd[iIndex];
        if(m_pDoc->m_pArtifact != NULL)
          m_pDoc->m_pArtifact->vUndo_reset_artifact_list(iIndex);
        }
      }
    }

    // Put back the original minutes in the patient info structure.
  m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod] = m_fOrigStart;
  m_pDoc->m_pdPatient.fGoodMinEnd[m_uPeriod] = m_fOrigEnd;
    // Redisplay the original minutes in the minute selection boxes.
  m_fStart = m_fOrigStart;
  m_fEnd = m_fOrigEnd;
  fPeriodStart = m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod];
  if(m_fStart != INV_MINUTE)
    m_cstrStartMin.Format("%.1f", m_fStart + fPeriodStart);
  else
    m_cstrStartMin.Format("%.1f", fPeriodStart);
  if(m_fEnd != INV_MINUTE)
    m_cstrEndMin.Format("%.1f", m_fEnd + fPeriodStart);
  else
    m_cstrEndMin.Format("%.1f", fPeriodStart + m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod]);
  UpdateData(FALSE);
  }

/********************************************************************
OnAcceptChanges

  Standard Version
    Message handler for changing the Accept Changes button.
      User defined own good minutes.
      Validate them.
        - The selected minutes must start on the minute or half minute.
        - Minutes are no longer than 4 characters (before the decimal point).
        - The minutes fall within the period.
        - There are at least 4 minutes that have been selected.
        - If there is artifact within the selected minutes, warn the user.
      If this is the baseline period, set the artifact limits.
********************************************************************/
void CTenMinView::OnAcceptChanges() 
  {
  CString cstrMsg, cstrTitle;
  float fStartMinute, fEndMinute;
  bool bOKToSave;

  UpdateData(TRUE);

  if((bOKToSave = bValidate_good_minutes(&fStartMinute, &fEndMinute,
  m_cstrStartMin, m_cstrEndMin)) == true)
    {
//      // Convert the minutes to floating point values.
//    fStartMinute = (float)atof(m_cstrStartMin);
//    fEndMinute = (float)atof(m_cstrEndMin);
      // Check if user define minutes include an artifact and if so, display
      // a warning message.
    fStartMinute -= m_fPeriodStartMin; //(m_uPeriod * MIN_PER_PERIOD);
    fEndMinute -= m_fPeriodStartMin; //(m_uPeriod * MIN_PER_PERIOD);
    if(m_pDoc->m_pdPatient.bManualMode == FALSE)
      {
      if(m_pDoc->m_pArtifact->bArtifact_in_minutes(m_uPeriod, fStartMinute, fEndMinute)
      == true)
        {
        //cstrTitle.LoadString(IDS_WARNING);
        cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
        //cstrMsg.LoadString(IDS_MIN_INCL_ART);
        cstrMsg = g_pLang->cstrLoad_string(ITEM_G_MIN_INCL_ART)
          + "\n" + g_pLang->cstrLoad_string(ITEM_G_MIN_INCL_ART1);
        // Note: two blank lines
        if(MessageBox(cstrMsg, cstrTitle, MB_YESNO) == IDNO)
          bOKToSave = false;
        }
      }
    if(bOKToSave == true)
      {
      m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod] = fStartMinute;
      m_pDoc->m_pdPatient.fGoodMinEnd[m_uPeriod] = fEndMinute;
        // Indicate that the period has been analyzed.
      m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = true;
      m_fStart = fStartMinute;
      m_fEnd = fEndMinute;
      if(m_uPeriod == PERIOD_BL)
        { // This is the baseline period.  It requires additional processing.
          // Find the max baseline signal during the good minutes just selected.
        m_pDoc->m_pdPatient.fMeanBaseline = 
                       m_pDoc->m_pAnalyze->fFind_mean_signal(m_pDoc->m_fData, m_fStart, m_fEnd);
        if(m_pDoc->m_pArtifact != NULL)
          m_pDoc->m_pArtifact->vSet_artifact_limit(m_pDoc->m_pdPatient.fMeanBaseline);
        }
        // Now hide the "reject" fields, put the minutes in the top field and select "Accept".
      m_cstrGoodMin.Format("%.1f - %.1f", m_fStart + m_fPeriodStartMin, 
                           m_fEnd + m_fPeriodStartMin);
      if(m_pDoc->m_pdPatient.bManualMode == FALSE)
        vShow_min_sel_fields(SW_HIDE);
      m_iAccept = 0; // Good minutes selected.
      UpdateData(FALSE);
      }
    }
/******************************
    // Make sure that the minutes entered are valid.
    // Verify that minutes must start on the minute or half minute.
  if(bValid_minute_format(m_cstrStartMin) == false)
    { // Start minute doesn't start on the minute or half minute.
    cstrMsg.LoadString(IDS_START_MINUTE_FORMAT_ERR);
    bOKToSave = false;
    }
  else if(bValid_minute_format(m_cstrEndMin) == false)
    { // End minute doesn't start on the minute or half minute.
    cstrMsg.LoadString(IDS_END_MINUTE_FORMAT_ERR);
    bOKToSave = false;
    }
  if(bOKToSave == true)
    { // Minutes are formatted correctly.
      // Convert the minutes to floating point values.
    fStartMinute = (float)atof(m_cstrStartMin);
    fEndMinute = (float)atof(m_cstrEndMin);
//    if(theApp.m_ptStudyType == PT_STANDARD)
//      { // Standard version
        // Check to make sure that the minutes actually fall within the period.
      if(fStartMinute < m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod]
      || fStartMinute > m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod] + m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod])
        { // Start minutes is out of range.
        cstrMsg.LoadString(IDS_START_MINUTE_OUT_OF_RANGE);
        bOKToSave = false;
        }
      else if(fEndMinute < m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod]
      || fEndMinute > m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod] + m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod])
        { // Start minutes is out of range.
        cstrMsg.LoadString(IDS_END_MINUTE_OUT_OF_RANGE);
        bOKToSave = false;
        }
      else if((fEndMinute - fStartMinute) < 4)
        { // Need 4 minutes.
        cstrMsg.LoadString(IDS_NEED_4_MINUTES);
        bOKToSave = false;
        }
    
      if(bOKToSave == false)
        { // Error, display the error message.
        cstrTitle.LoadString(IDS_ERROR1);
        MessageBox(cstrMsg, cstrTitle);
        }
      else
        {
        if(theApp.m_ptStudyType == PT_STANDARD)
          { // Standard version
            // Check if user define minutes include an artifact and if so, display
            // a warning message.
          fStartMinute -= m_fPeriodStartMin; //(m_uPeriod * MIN_PER_PERIOD);
          fEndMinute -= m_fPeriodStartMin; //(m_uPeriod * MIN_PER_PERIOD);
          if(m_pDoc->m_pdPatient.bManualMode == FALSE)
            {
            if(m_pDoc->m_pArtifact->bArtifact_in_minutes(m_uPeriod, fStartMinute, fEndMinute)
            == true)
              {
              cstrTitle.LoadString(IDS_WARNING);
              cstrMsg.LoadString(IDS_MIN_INCL_ART);
              if(MessageBox(cstrMsg, cstrTitle, MB_YESNO) == IDNO)
                bOKToSave = false;
              }
            }
          }
        if(bOKToSave == true)
          {
          m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod] = fStartMinute;
          m_pDoc->m_pdPatient.fGoodMinEnd[m_uPeriod] = fEndMinute;
            // Indicate that the period has been analyzed.
          m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = true;
          m_fStart = fStartMinute;
          m_fEnd = fEndMinute;
          if(theApp.m_ptStudyType == PT_STANDARD)
            { // Standard version
            if(m_uPeriod == PERIOD_BL)
              { // This is the baseline period.  It requires additional processing.
                // Find the max baseline signal during the good minutes just selected.
              m_pDoc->m_pdPatient.fMeanBaseline = 
                             m_pDoc->m_pAnalyze->fFind_mean_signal(m_pDoc->m_fData, m_fStart, m_fEnd);
              if(m_pDoc->m_pArtifact != NULL)
                m_pDoc->m_pArtifact->vSet_artifact_limit(m_pDoc->m_pdPatient.fMeanBaseline);
              }
              // Now hide the "reject" fields, put the minutes in the top field and select "Accept".
            m_cstrGoodMin.Format("%.1f - %.1f", m_fStart + m_fPeriodStartMin, 
                                 m_fEnd + m_fPeriodStartMin);
            if(m_pDoc->m_pdPatient.bManualMode == FALSE)
              vShow_min_sel_fields(SW_HIDE);
            }
          m_iAccept = 0; // Good minutes selected.
          UpdateData(FALSE);
          }
        }
//      }
//    else
//      { // Research Version
//      }
    }
********************************/
  }

/********************************************************************
OnVScroll

  Event handler for clicking on the graph scrollbar.
    Display the next set of graphs depending on what the user clicked.
********************************************************************/
void CTenMinView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
  {
  bool bGraph = false;
//  int iLastMinOnGrph, iMinDisplayed;
//  UINT uCurPos;

  // TODO: Add your message handler code here and/or call default
  if(pScrollBar != NULL && pScrollBar->m_hWnd == m_cGraphScrollBar.m_hWnd)
    {
		SCROLLINFO si;
		si.fMask=SIF_ALL;
		m_cGraphScrollBar.GetScrollInfo(&si,SIF_ALL);
		switch(nSBCode)
		{
		case SB_LINEUP: // Scroll up one graph.
      if(si.nPos > si.nMin)
        {
        --si.nPos;
        m_iFirstMinOnGraph = (m_iNumMinOnGraph * si.nPos) + 1;
        bGraph = true;
        }
/**********************************
			si.nPos--;
      if(m_iFirstMinOnGraph > 1)
        {
        m_iFirstMinOnGraph -= m_iNumMinOnGraph;
        bGraph = true;
        }
********************************/
			break;
		case SB_LINEDOWN: // Scroll down one graph.
      if(si.nPos < (si.nMax - 2))
        {
        si.nPos++;
        m_iFirstMinOnGraph = (m_iNumMinOnGraph * si.nPos) + 1;
        bGraph = true;
        }
/**********************************
			si.nPos++;
        // Determine if there is any more data left to graph.
      if((m_iFirstMinOnGraph + (m_iNumMinOnGraph * 3)) <= m_fPeriodLen)
//      <= m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod])
        { // Still have more data to display. 
        m_iFirstMinOnGraph += m_iNumMinOnGraph;
        bGraph = true;
        }
*******************************/
			break;
		case SB_PAGEUP:  // Scroll up 3 graphs
      if(si.nPos > si.nMin)
        {
        si.nPos -= si.nPage;
        if(si.nPos < si.nMin)
          si.nPos = si.nMin;
        m_iFirstMinOnGraph = (m_iNumMinOnGraph * si.nPos) + 1;
        bGraph = true;
        }
/***********************************
			si.nPos -= si.nPage;
      if(m_iFirstMinOnGraph > 1)
        {
        m_iFirstMinOnGraph -= (3 * m_iNumMinOnGraph);
        if(m_iFirstMinOnGraph < 1)
          m_iFirstMinOnGraph = 1;
        bGraph = true;
        }
******************************/
			break;
		case SB_PAGEDOWN:  // Scroll down 3 graphs.
      if(si.nPos <= (si.nMax - (int)si.nPage))
        {
        si.nPos += si.nPage;
        if(si.nPos > (si.nMax - 2))
          si.nPos = si.nMax - 2;
        m_iFirstMinOnGraph = (m_iNumMinOnGraph * si.nPos) + 1;
        bGraph = true;
        }
/*************************************
			si.nPos += si.nPage;
        // Determine if there is any more data left to graph.
      iLastMinOnGrph = uFround(m_fPeriodLen) + 1;
      iMinDisplayed = m_iNumMinOnGraph * 3;
      if((m_iFirstMinOnGraph + iMinDisplayed) <= iLastMinOnGrph)
//      <= m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod])
        { // Still have more data to display. 
          // Determine if we can do a full page or just a partial page.
          // First move 1 graph down.
        m_iFirstMinOnGraph += m_iNumMinOnGraph;
        if((m_iFirstMinOnGraph + iMinDisplayed) <= iLastMinOnGrph)
//        <= m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod])
          { // Still more data, move 2 graphs down.
          m_iFirstMinOnGraph += m_iNumMinOnGraph;
          if((m_iFirstMinOnGraph + iMinDisplayed) <= iLastMinOnGrph)
//          <= m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod])
            { // Have more data, move the full 3 graphs down.
            m_iFirstMinOnGraph += m_iNumMinOnGraph;
            }
          }
        bGraph = true;
        }
***********************************/
			break;
    case SB_THUMBPOSITION:
        // Find out where we are.
			si.nPos = si.nTrackPos;
      m_iFirstMinOnGraph = (m_iNumMinOnGraph * si.nPos) + 1;
      bGraph = true;
      break;
    case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
      m_iFirstMinOnGraph = (m_iNumMinOnGraph * si.nPos) + 1;
      bGraph = true;
      break;
		}
		if(si.nPos > (int)(si.nMax - si.nMin - si.nPage + 1))
      si.nPos = si.nMax - si.nMin - si.nPage + 1;
		if(si.nPos < si.nMin)
      si.nPos = si.nMin;
		si.fMask = SIF_POS;
		m_cGraphScrollBar.SetScrollInfo(&si);

    if(bGraph == true)
      {
      m_bGraphScrolled = true;
      vGraph_period(m_uPeriod, m_pDoc->m_fData, &m_pDoc->m_pdPatient, m_uMenuTypeForGraph);
      m_bGraphScrolled = false;
      Invalidate();
      }
    }
  CFormView::OnVScroll(nSBCode, nPos, pScrollBar);
  }

/********************************************************************
OnBnClickedChkSetPeriodLength

  Event handler for Checking/unchecking the set period length checkbox.
    Validate the period start minute and length.
    Add/remove a period appropriately.
    If good minutes have changed and not be set, do that.
    If period and good minutes are valid, mark the period as analyzed.
    If checkbox is unchecked, mark period as not analyzed.
********************************************************************/
void CTenMinView::OnBnClickedChkSetPeriodLength()
  {
  CString cstrMsg, cstrTitle;
  float fEndMinute, fStartMinute; //, fGoodStartMin, fGoodEndMin;
  bool bOKToSave = true;

  UpdateData(TRUE);
  if(m_bSetPeriodLength == TRUE)
    {
      // Make sure that the minutes entered are valid.
      // Verify that minutes must start on the minute or half minute.
    if(bValid_minute_format(m_cstrPeriodStartMin) == false)
      { // Start minute doesn't start on the minute or half minute.
      //cstrMsg.LoadString(IDS_START_MINUTE_FORMAT_ERR);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_START_MINUTE_FORMAT_ERR);
      bOKToSave = false;
      }
    else if(bValid_minute_format(m_cstrPeriodEndMin) == false)
      { // End minute doesn't start on the minute or half minute.
      //cstrMsg.LoadString(IDS_END_MINUTE_FORMAT_ERR);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_END_MINUTE_FORMAT_ERR);
      bOKToSave = false;
      }
    if(bOKToSave == true)
      { // Minutes are formatted correctly.
        // Convert the minutes to floating point values.
      fStartMinute = (float)atof(m_cstrPeriodStartMin);
      fEndMinute = (float)atof(m_cstrPeriodEndMin);
      if((fEndMinute - fStartMinute) < 4)
        { // Need 4 minutes.
        //cstrMsg.LoadString(IDS_NEED_4_MINUTES);
        cstrMsg = g_pLang->cstrLoad_string(ITEM_G_NEED_4_MINUTES);
        bOKToSave = false;
        }
      else if(m_pDoc->m_uCurrentPeriod == m_pDoc->m_pdPatient.uNumPeriods)
        { // On the last period in the study.
          // Make sure the minutes are not greater than the last minute in the study
        if(fEndMinute > m_pDoc->m_fStudyLen)
          { // End minutes is out of range.
          //cstrMsg.LoadString(IDS_END_MINUTE_OUT_OF_RANGE);
          cstrMsg = g_pLang->cstrLoad_string(ITEM_G_END_MINUTE_OUT_OF_RANGE);
          bOKToSave = false;
          }
////      else if(fStartMinute > (float)m_pDoc->m_uStudyLen)
////        { // Start minutes is out of range.
////        cstrMsg.LoadString(IDS_START_MINUTE_OUT_OF_RANGE);
////        bOKToSave = false;
////        }
        else if(m_pDoc->m_pdPatient.uNumPeriods > 1)
          { // There is more than 1 period in study.
            // Make sure the start minute doesn't interfere with the previous period.
          if(fStartMinute < 
          (m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod - 1]
          + m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod - 1]))
            {
            //cstrMsg.LoadString(IDS_START_MIN_IN_PREV_PERIOD);
            cstrMsg = g_pLang->cstrLoad_string(ITEM_G_END_MINUTE_OUT_OF_RANGE);
            bOKToSave = false;
            }
          }
        }
      else
        { // On a period in the middle of the study.
          // Make sure the start and end minutes and don't interfere with neighboring periods.
        if(m_uPeriod > 1 && fStartMinute <
        (m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod - 1]
        + m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod - 1]))
          {
          //cstrMsg.LoadString(IDS_START_MIN_IN_PREV_PERIOD);
          cstrMsg = g_pLang->cstrLoad_string(ITEM_G_START_MIN_IN_PREV_PERIOD);
          bOKToSave = false;
          }
        else if(m_uPeriod < m_pDoc->m_pdPatient.uNumPeriods
        && fEndMinute > m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod + 1])
          {
          //cstrMsg.LoadString(IDS_END_MIN_IN_NEXT_PERIOD);
          cstrMsg = g_pLang->cstrLoad_string(ITEM_G_END_MIN_IN_NEXT_PERIOD);
          bOKToSave = false;
          }
        }
      }
    if(bOKToSave == false)
      { // Error, display the error message.
      m_bSetPeriodLength = FALSE;
      //cstrTitle.LoadString(IDS_ERROR1);
      cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
      MessageBox(cstrMsg, cstrTitle);
      }
    else
      { // Period Minutes are OK.
        // User changed the period start minute and/or period length.
        // Indicate that the period has been analyzed.
      m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
      if(m_bSetGoodMinutes == TRUE)
        { // User has previously set the good minutes.  Uncheck them
          // to make the user re-evaluate them.
        m_bSetGoodMinutes = FALSE;
        m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
        }
      m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod] = fStartMinute;
      m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod] = fEndMinute - fStartMinute;
      if(m_uPeriod == m_pDoc->m_pdPatient.uNumPeriods)
        {
          // This is a new period or the last period, add appropriate menu items.
          // Reset both good minutes and period change flags.
          // Add a new period.
        if(fEndMinute < (m_pDoc->m_fStudyLen - (float)4))
          bAdd_new_period();
        }
        // Redisplay the graph and scroll bar.
      m_iFirstMinOnGraph = 1;
        // Read in the period again, since it may have changed.
      if(m_pDoc->iRead_period(m_uPeriod, m_pDoc->m_fData) == SUCCESS)
        {
        vGraph_period(m_uPeriod, m_pDoc->m_fData, &m_pDoc->m_pdPatient, m_uMenuTypeForGraph);
        Invalidate();
        }
      }
    UpdateData(FALSE);
    }
  else
    {
    m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
    }
  }

/********************************************************************
OnBnClickedChkSetGoodMinutes

  Event handler for Checking/unchecking the set good minutes checkbox.
    Validate the good start and end minutes.
********************************************************************/
void CTenMinView::OnBnClickedChkSetGoodMinutes()
  {
  CString cstrMsg, cstrTitle;
  float fStartMinute, fEndMinute;
  bool bOKToSave;

  UpdateData(TRUE);
  if(m_bSetGoodMinutes == TRUE)
    {
    if((bOKToSave = bValidate_good_minutes(&fStartMinute, &fEndMinute,
    m_cstrStartGoodMin, m_cstrEndGoodMin)) == true)
      {
      m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod] = fStartMinute;
      m_pDoc->m_pdPatient.fGoodMinEnd[m_uPeriod] = fEndMinute;
        // Indicate that the period has been analyzed.
      if(m_bSetPeriodLength == TRUE)
        m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = true;
      m_fStart = fStartMinute;
      m_fEnd = fEndMinute;
      }
    else
      { // Error in validating good minutes.
      m_bSetGoodMinutes = FALSE;
      UpdateData(FALSE);
      }
    }
  else
    {
    m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
    }
/******************************
    // Make sure that the minutes entered are valid.
    // Verify that minutes must start on the minute or half minute.
  if(bValid_minute_format(m_cstrStartMin) == false)
    { // Start minute doesn't start on the minute or half minute.
    cstrMsg.LoadString(IDS_START_MINUTE_FORMAT_ERR);
    bOKToSave = false;
    }
  else if(bValid_minute_format(m_cstrEndMin) == false)
    { // End minute doesn't start on the minute or half minute.
    cstrMsg.LoadString(IDS_END_MINUTE_FORMAT_ERR);
    bOKToSave = false;
    }
  if(bOKToSave == true)
    { // Minutes are formatted correctly.
      // Convert the minutes to floating point values.
    fStartMinute = (float)atof(m_cstrStartMin);
    fEndMinute = (float)atof(m_cstrEndMin);
//    if(theApp.m_ptStudyType == PT_STANDARD)
//      { // Standard version
        // Check to make sure that the minutes actually fall within the period.
      if(fStartMinute < m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod]
      || fStartMinute > m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod] + m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod])
        { // Start minutes is out of range.
        cstrMsg.LoadString(IDS_START_MINUTE_OUT_OF_RANGE);
        bOKToSave = false;
        }
      else if(fEndMinute < m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod]
      || fEndMinute > m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod] + m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod])
        { // Start minutes is out of range.
        cstrMsg.LoadString(IDS_END_MINUTE_OUT_OF_RANGE);
        bOKToSave = false;
        }
      else if((fEndMinute - fStartMinute) < 4)
        { // Need 4 minutes.
        cstrMsg.LoadString(IDS_NEED_4_MINUTES);
        bOKToSave = false;
        }
    
      if(bOKToSave == false)
        { // Error, display the error message.
        cstrTitle.LoadString(IDS_ERROR1);
        MessageBox(cstrMsg, cstrTitle);
        }
      else
        {
        if(theApp.m_ptStudyType == PT_STANDARD)
          { // Standard version
            // Check if user define minutes include an artifact and if so, display
            // a warning message.
          fStartMinute -= m_fPeriodStartMin; //(m_uPeriod * MIN_PER_PERIOD);
          fEndMinute -= m_fPeriodStartMin; //(m_uPeriod * MIN_PER_PERIOD);
          if(m_pDoc->m_pdPatient.bManualMode == FALSE)
            {
            if(m_pDoc->m_pArtifact->bArtifact_in_minutes(m_uPeriod, fStartMinute, fEndMinute)
            == true)
              {
              cstrTitle.LoadString(IDS_WARNING);
              cstrMsg.LoadString(IDS_MIN_INCL_ART);
              if(MessageBox(cstrMsg, cstrTitle, MB_YESNO) == IDNO)
                bOKToSave = false;
              }
            }
          }
        if(bOKToSave == true)
          {
          m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod] = fStartMinute;
          m_pDoc->m_pdPatient.fGoodMinEnd[m_uPeriod] = fEndMinute;
            // Indicate that the period has been analyzed.
          m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = true;
          m_fStart = fStartMinute;
          m_fEnd = fEndMinute;
          if(theApp.m_ptStudyType == PT_STANDARD)
            { // Standard version
            if(m_uPeriod == PERIOD_BL)
              { // This is the baseline period.  It requires additional processing.
                // Find the max baseline signal during the good minutes just selected.
              m_pDoc->m_pdPatient.fMeanBaseline = 
                             m_pDoc->m_pAnalyze->fFind_mean_signal(m_pDoc->m_fData, m_fStart, m_fEnd);
              if(m_pDoc->m_pArtifact != NULL)
                m_pDoc->m_pArtifact->vSet_artifact_limit(m_pDoc->m_pdPatient.fMeanBaseline);
              }
              // Now hide the "reject" fields, put the minutes in the top field and select "Accept".
            m_cstrGoodMin.Format("%.1f - %.1f", m_fStart + m_fPeriodStartMin, 
                                 m_fEnd + m_fPeriodStartMin);
            if(m_pDoc->m_pdPatient.bManualMode == FALSE)
              vShow_min_sel_fields(SW_HIDE);
            }
          m_iAccept = 0; // Good minutes selected.
          UpdateData(FALSE);
          }
        }
//      }
//    else
//      { // Research Version
//      }
    }
********************************/
    }

/********************************************************************
OnEnChangeStartMin

  Event handler for changing the start good minutes edit field.
    Mark the good minutes as being changed.
    Mark the period as not analyzed.
********************************************************************/
void CTenMinView::OnEnChangeStartMin()
  {

  UpdateData(TRUE);
  m_bSetGoodMinutes = FALSE;
  m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
  UpdateData(FALSE);
  }

/********************************************************************
OnEnChangeEndMin

  Event handler for changing the end good minutes edit field.
    Mark the good minutes as being changed.
    Mark the period as not analyzed.
********************************************************************/
void CTenMinView::OnEnChangeEndMin()
  {

  UpdateData(TRUE);
  m_bSetGoodMinutes = FALSE;
  m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
  UpdateData(FALSE);
  }

/********************************************************************
OnEnChangeStartGoodMin

  Event handler for changing the Research study start good minutes edit field.
    Mark the good minutes as being changed.
    Mark the period as not analyzed.
********************************************************************/
void CTenMinView::OnEnChangeStartGoodMin()
  {

  UpdateData(TRUE);
  m_bSetGoodMinutes = FALSE;
  m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
  UpdateData(FALSE);
  }

/********************************************************************
OnEnChangeEndGoodMin

  Event handler for changing the Research study end good minutes edit field.
    Mark the good minutes as being changed.
    Mark the period as not analyzed.
********************************************************************/
void CTenMinView::OnEnChangeEndGoodMin()
  {

  UpdateData(TRUE);
  m_bSetGoodMinutes = FALSE;
  m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
  UpdateData(FALSE);
  }

/********************************************************************
OnEnChangeStartPeriodMin

  Event handler for changing the start period minutes edit field.
    Mark the period as being changed.
    Mark the period as not analyzed.
********************************************************************/
void CTenMinView::OnEnChangeStartPeriodMin()
  {

  UpdateData(TRUE);
  m_bSetPeriodLength = FALSE;
  m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
  UpdateData(FALSE);
  }

/********************************************************************
OnEnChangeEndPeriodMin

  Event handler for changing the end period minutes edit field.
    Mark the period as being changed.
    Mark the period as not analyzed.
********************************************************************/
void CTenMinView::OnEnChangeEndPeriodMin()
  {

  UpdateData(TRUE);
  m_bSetPeriodLength = FALSE;
  m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_uPeriod] = false;
  UpdateData(FALSE);
  }

/********************************************************************
OnLbnSelchangeEventList

  Event handler for changing the selection in the event listbox.
  For Research Version.
    Show the event description in the description edit box.
********************************************************************/
void CTenMinView::OnLbnSelchangeEventList()
  {
  CEvnt *pEvent;
  int iEventIndex;

  UpdateData(TRUE);
  iEventIndex = (int)m_cEventsList.GetItemData(m_cEventsList.GetCurSel());
  if(m_uPeriod == 0)
    { // Baseline.
    pEvent = m_pDoc->m_pdPatient.Events.m_BLEvents[iEventIndex];
    }
  else
    { // Post-stimulation period
    pEvent = m_pDoc->m_pdPatient.Events.m_PSEvents[iEventIndex];
    }
  m_cstrEventDescription = pEvent->m_cstrDescription;
  UpdateData(FALSE);
  }

/********************************************************************
OnLbnDblclkEventList

  Event handler for double clicking on the selection in the event listbox.
  For Research Version.
    Display the EventRecorder dialog box and allow user to changed the event.
********************************************************************/
void CTenMinView::OnLbnDblclkEventList()
  {
  
  vEdit_event_from_lstbox();
  }

/********************************************************************
OnBnClickedBtnEventsEdit

  Event handler for clicking on the Edit event.
  For Research Version.
    Display the EventRecorder dialog box and allow user to changed the event.
********************************************************************/
void CTenMinView::OnBnClickedBtnEventsEdit()
  {

  vEdit_event_from_lstbox();
  }

/********************************************************************
OnBnClickedBtnEventsDelete

  Event handler for clicking on the Delete event.
  For Research Version.
    Delete the selected event if the user confirms it.
********************************************************************/
void CTenMinView::OnBnClickedBtnEventsDelete()
  {
  CEvnt *pEvent;
  CString cstrTitle, cstrMsg, cstrLBLine;
  int iSel, iEventIndex;

  UpdateData(TRUE);
  iSel = m_cEventsList.GetCurSel();
  iEventIndex = (int)m_cEventsList.GetItemData(iSel);
    // Ask are you sure?
  //cstrTitle.LoadString(IDS_WARNING);
  cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
  //cstrMsg.LoadString(IDS_ASK_DELETE_EVENT);
  cstrMsg = g_pLang->cstrLoad_string(ITEM_G_ASK_DELETE_EVENT);
    // Add line from list box to the message.
  cstrMsg += "\n";
  m_cEventsList.GetText(iSel, cstrLBLine);
  cstrMsg += cstrLBLine;
  if(MessageBox(cstrMsg, cstrTitle, MB_YESNO) == IDYES)
    {
    if(m_uPeriod == 0)
      { // Baseline.
      pEvent = m_pDoc->m_pdPatient.Events.m_BLEvents[iEventIndex];
      m_pDoc->m_pdPatient.Events.m_BLEvents.RemoveAt(iEventIndex);
      delete pEvent;
      }
    else
      { // Post-stimulation period
      pEvent = m_pDoc->m_pdPatient.Events.m_PSEvents[iEventIndex];
      m_pDoc->m_pdPatient.Events.m_PSEvents.RemoveAt(iEventIndex);
      delete pEvent;
      }
      // Redraw the graphs.  This will also update the event data.
    vGraph_period(m_uPeriod, m_pDoc->m_fData, &m_pDoc->m_pdPatient, m_uMenuTypeForGraph);
    Invalidate();
    }
  }

/********************************************************************
OnCommand

  Event handler for processing the WM_COMMAND message
  For Research Version.
    Process the left mouse click on one of the graph window.
       Displays the EventRecorder dialog box.
********************************************************************/
BOOL CTenMinView::OnCommand(WPARAM wParam, LPARAM lParam)
  {

  switch (HIWORD(wParam))
    {
    case PEWN_CLICKED:  // Mouse left button click in the graph window.
      HOTSPOTDATA HSData;
      HWND hwndFrom;
      float fFirstMinOnGrph;
#if EGGSAS_READER != READER_DEPOT
      CEventRecorder *pdlg;
      CEvnt *pEvent, *pExistEvent;
      CEvntArray *pEventArray;
      int iIndex, iDataPointIndex, iSeconds;
      bool bAdd;
#endif

        // HSData.w1 = subset index, HSData.w2 = point index
        // Figure out which graph window the click was from.
      hwndFrom = (HWND)lParam;
      if(hwndFrom == m_hPEData1)
        {
        PEvget(m_hPEData1, PEP_structHOTSPOTDATA, &HSData);
        fFirstMinOnGrph = m_fFirstMinOnGraph[0];
        }
      else if(hwndFrom == m_hPEData2)
        {
        PEvget(m_hPEData2, PEP_structHOTSPOTDATA, &HSData);
        fFirstMinOnGrph = m_fFirstMinOnGraph[1];
        }
      else if(hwndFrom == m_hPEData3)
        {
        PEvget(m_hPEData3, PEP_structHOTSPOTDATA, &HSData);
        fFirstMinOnGrph = m_fFirstMinOnGraph[2];
        }
      if (HSData.nHotSpotType == PEHS_DATAPOINT)
        {
#if EGGSAS_READER != READER_DEPOT
        bAdd = true;
        iDataPointIndex = (int)(fFirstMinOnGrph * (float)DATA_POINTS_PER_MIN_1)
                          + HSData.w2;
        iSeconds = (int)(fFirstMinOnGrph * (float)60.0) + (HSData.w2 * 60) / 256;
        pdlg = new CEventRecorder(iSeconds);
          // Look to see if this event already exists.  If so, give the dialog box
          // all the information for the event.
        if(m_uPeriod == 0)
          pEventArray = &m_pDoc->m_pdPatient.Events.m_BLEvents;
        else
          pEventArray = &m_pDoc->m_pdPatient.Events.m_PSEvents;
        for(iIndex = 0; iIndex < pEventArray->GetCount(); ++iIndex)
          {
          pExistEvent = pEventArray->ElementAt(iIndex);
          if(pExistEvent->m_iDataPointIndex == iDataPointIndex
            || pExistEvent->m_iSeconds == iSeconds)
            {
            pdlg->vSet_data(pExistEvent->m_cstrIndications,
                            pExistEvent->m_cstrDescription);
            bAdd = false;
            break;
            }
          }
        if(pdlg->DoModal() == IDOK)
          {
          if(bAdd == true)
            { // Adding a new event.
            pEvent = new CEvnt();
            pEvent->m_cstrDescription = pdlg->cstrGet_description();
            pEvent->m_cstrIndications = pdlg->cstrGet_indications();
            pEvent->m_iDataPointIndex = iDataPointIndex;
            pEvent->m_iSeconds = iSeconds;
            if(m_uPeriod == 0)
              {
              pEvent->m_iPeriodType = EVENT_BASELINE;
              m_pDoc->m_pdPatient.Events.m_BLEvents.Add(pEvent);
              }
            else
              {
              pEvent->m_iPeriodType = EVENT_POST_STIM;
              m_pDoc->m_pdPatient.Events.m_PSEvents.Add(pEvent);
              }
            }
          else
            { // Editing an existing event.
            pExistEvent->m_cstrDescription = pdlg->cstrGet_description();
            pExistEvent->m_cstrIndications = pdlg->cstrGet_indications();
              // Rebuild the listbox to reflect any changes.
            vFill_events_fields(&m_pDoc->m_pdPatient);
            }
          }
        delete pdlg;
        vGraph_period(m_uPeriod, m_pDoc->m_fData, &m_pDoc->m_pdPatient, m_uMenuTypeForGraph);
        Invalidate();
#endif
        } 
      return TRUE;
    }

  return CFormView::OnCommand(wParam, lParam);
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
vInit - Initialize all data having to do with the current patient, but
        only if the initial update has already been done.

  inputs: None.

  return: None.
********************************************************************/
void CTenMinView::vInit()
  {

  if(m_bInitialUpdate == true)
    {
    if(m_hPEData1 != NULL)
      {
      PEdestroy(m_hPEData1);
      m_hPEData1 = NULL;
      }
    if(m_hPEData2 != NULL)
      {
      PEdestroy(m_hPEData2);
      m_hPEData2 = NULL;
      }
    if(m_hPEData3 != NULL)
      {
      PEdestroy(m_hPEData3);
      m_hPEData3 = NULL;
      }
    m_cstrPatientName.Empty();
    vEnable_fields(FALSE);
    m_c10MinPeriodLst.SetCurSel(0);
      // Hide the analyze fields.
    if(theApp.m_ptStudyType == PT_STANDARD)
      { // Standard version
      vShow_analyze_fields(SW_HIDE);
      m_bAnalyzedFieldsVisible = false;
      }

    UpdateData(FALSE);
    }
  }

/********************************************************************
vSet_patient_info - Set the patient name and study date in the form view.

                    Used by the parent to tell this object what the
                    patient name is.

  inputs: Pointer to a PATIENT_DATA containing the needed information.

  return: None.
********************************************************************/
void CTenMinView::vSet_patient_info(PATIENT_DATA *pdInfo)
  {

#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
  if(pdInfo->bUseResrchRefNum == TRUE)
    {
#if EGGSAS_READER == READER
      m_cstrPatientName = pdInfo->cstrResearchRefNum;
#else
    if(pdInfo->cstrDate.GetLength() <= 10)
      m_cstrPatientName.Format("%s    %s", pdInfo->cstrResearchRefNum, pdInfo->cstrDate);
    else
      m_cstrPatientName = pdInfo->cstrResearchRefNum;
#endif
    }
  else
    vFormat_patient_name(pdInfo, m_cstrPatientName, false);
#else
  if(pdInfo->bUseResrchRefNum == TRUE)
    m_cstrPatientName.Format("%s    %s", pdInfo->cstrResearchRefNum, pdInfo->cstrDate);
  else
    vFormat_patient_name(pdInfo, m_cstrPatientName, false);
#endif

  vShow_study_type();
  UpdateData(FALSE);
  }

/********************************************************************
vGraph_period - Graph the entire 10 minute period as follows:
                  Top graph - First 4 minutes
                  Middle graph - Second 4 minutes
                  Bottom graph - Last 2 minutes.
                  If there is an extra 10 minutes for the baseline,
                  then the baseline is graphed as follows.
                    Top graph - First 8 minutes
                    Middle graph - Second 8 minutes
                    Bottom graph - Last 4 minutes.

  inputs: The 10 minute period to graph.
          Pointer to an array containing the data to graph.
          Pointer to a PATIENT_DATA structure containing additional
            data for graphing.
          The menu used to get here (VIEW, EDIT(edit and analyze))

  return: None.
********************************************************************/
void CTenMinView::vGraph_period(unsigned short uPeriod, float *pfData,
PATIENT_DATA *pdInfo, unsigned uMenuType)
  {
  RECT rGraph, rScrollBar, rStsBarRect; // rRect, rWin, 
  WINDOWPLACEMENT wp10MinView, wpTemp;
  GRAPH_INFO giData;
  CString cstrText;
  int iWidth, iHeight, iGraphHeight3, iStsBarHeight;
  short int iIndex;
  unsigned short uTotalPages, uTotalGraphs;
  float fNumMin[3], fTemp;

  m_uMenuTypeForGraph = uMenuType;
  m_uPeriod = uPeriod;
    // Get the minute that this period starts on.
  m_fPeriodStartMin = m_pDoc->fGet_period_start_min(m_uPeriod);
  m_uMenuType = uMenuType;
  m_fOrigStart = pdInfo->fGoodMinStart[m_uPeriod];
  m_fOrigEnd = pdInfo->fGoodMinEnd[m_uPeriod];
    // Save the selected minutes for a possible undo later, then reset the selected minutes.
  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard study
    for(iIndex = 0; iIndex < MAX_MANUAL_MODE_PERIODS; ++iIndex)
      {
      m_fOrigGoodMinStart[iIndex] = pdInfo->fGoodMinStart[iIndex];
      m_fOrigGoodMinEnd[iIndex] = pdInfo->fGoodMinEnd[iIndex];
      }
    }
  else
    { // Research study
    for(iIndex = 0; iIndex < MAX_TOTAL_PERIODS; ++iIndex)
      {
      m_fOrigGoodMinStart[iIndex] = pdInfo->fGoodMinStart[iIndex];
      m_fOrigGoodMinEnd[iIndex] = pdInfo->fGoodMinEnd[iIndex];
      }
    }
  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard study
    if(m_uMenuType == SM_VIEW || g_bHiddenPatients == TRUE)
      { // Viewing, disable the accept and reject buttons so user
        // can't change anything.
      GetDlgItem(IDC_ACCEPT)->EnableWindow(FALSE);
      GetDlgItem(IDC_REJECT)->EnableWindow(FALSE);
      }
    else
      { // Editing or analyzing.
      GetDlgItem(IDC_ACCEPT)->EnableWindow(TRUE);
      GetDlgItem(IDC_REJECT)->EnableWindow(TRUE);
      }
    if(m_bAnalyzedFieldsMoved == false)
      {
        // Move the analyze fields to the right as far as possible.
      vMove_analyze_fields();
      }
      // Have to hide fields here because moving them shows them and not all
      // of them should be shown.
    vShow_analyze_fields(SW_HIDE);
    m_bAnalyzedFieldsVisible = false;
    if(m_bAnalyzedFieldsVisible == false)
      {
      vShow_research_fields(false);
      vShow_analyze_fields(SW_SHOW);
      m_bAnalyzedFieldsVisible = true;
      }
    else
      vShow_min_sel_fields(SW_HIDE); // Hide the minute selection fields.
      // Set the limits on the spin controls based on the period and the selected minutes.
    m_fSpinStart = pdInfo->fPeriodStart[m_uPeriod];
    if(pdInfo->fGoodMinStart[m_uPeriod] != INV_MINUTE)
      m_fSpinStart += m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod];
    m_fSpinEnd = pdInfo->fPeriodStart[m_uPeriod];
    if(pdInfo->fGoodMinEnd[m_uPeriod] == INV_MINUTE)
      m_fSpinEnd += pdInfo->fPeriodLen[m_uPeriod];
    else
      m_fSpinEnd += pdInfo->fGoodMinEnd[m_uPeriod];
  
    m_fSpinStartLoLim = pdInfo->fPeriodStart[m_uPeriod];
    m_fSpinStartHiLim = m_fSpinEnd;
    m_fSpinEndLoLim = m_fSpinStart;
    m_fSpinEndHiLim = pdInfo->fPeriodStart[m_uPeriod] + pdInfo->fPeriodLen[m_uPeriod];
    ((CSpinButtonCtrl *)GetDlgItem(IDC_START_MIN_SPIN))->SetRange(0, (short)pdInfo->fPeriodLen[m_uPeriod] * 2);
    ((CSpinButtonCtrl *)GetDlgItem(IDC_END_MIN_SPIN))->SetRange(0, (short)pdInfo->fPeriodLen[m_uPeriod] * 2);
//    ((CSpinButtonCtrl *)GetDlgItem(IDC_START_MIN_SPIN))->SetRange(0, MIN_PER_PERIOD * 2);
//    ((CSpinButtonCtrl *)GetDlgItem(IDC_END_MIN_SPIN))->SetRange(0, MIN_PER_PERIOD * 2);
    vInit_analyze_fields();
    m_cstrStartMin.Format("%.1f", m_fSpinStart);
  	m_cstrEndMin.Format("%.1f", m_fSpinEnd);
    }
  else
    { // Research Version
      // Accept Changes button becomes Set Good Minutes button.
      // Select New Minutes becomex Select Good Minutes along with the start
      //  and end minutes.
      // Get the rectangle of the scroll bar.
    m_cGraphScrollBar.GetClientRect(&rScrollBar);
    if(m_bRsrchAnalyzedFieldsMoved == false)
      {
        // Move them to proper position.
      vMove_research_analyze_fields();
      m_bRsrchAnalyzedFieldsMoved = true;
      }
    if(m_bRsrchAnalyzedFieldsVisible == false)
      {
      vShow_standard_fields(false);
      vShow_research_fields(true);
      }
      // Enable/disable fields depending on period.
    if(uPeriod == 0)
      { // Baseline, Don't select period length.
        // Can't delete/insert periods in baseline period.
      vEnable_research_period_fields(false);
      if(m_uMenuType == SM_VIEW || g_bAsciiDataFile == true || g_bHiddenPatients == TRUE)
        {
        vEnable_research_buttons(false);
        vEnable_research_goodmin_fields(false);
        }
        // Baseline period length is always checked.
      m_bSetPeriodLength = TRUE;
      }
    else
      {
      if(m_uMenuType == SM_VIEW || g_bAsciiDataFile == true || g_bHiddenPatients == TRUE)
        {
        vEnable_research_period_fields(false);
        vEnable_research_goodmin_fields(false);
        vEnable_research_buttons(false);
        }
      else
        {
        vEnable_research_period_fields(true);
        vEnable_research_goodmin_fields(true);
        vEnable_research_buttons(true);
        }
/********************************
      if(pdInfo->uNumPeriods == 1 || m_uPeriod == pdInfo->uNumPeriods)
        { // Can't delete period if only one post-stimulation period
          // or this is the last period.
        GetDlgItem(IDC_DELETE_THIS_PERIOD)->EnableWindow(FALSE);
        }
      else
        {
        if(m_uMenuType == SM_EDIT)
          GetDlgItem(IDC_DELETE_THIS_PERIOD)->EnableWindow(TRUE);
        }
*******************************/
      }

      // Set the limits on the Period spin controls only if the graph hasn't been
      // scrolled.
    if(m_bGraphScrolled == false)
      {
        // Period Start minute.
      if(pdInfo->fPeriodStart[m_uPeriod] == INV_PERIOD_START)
        { // Period hasn't been defined yet.
        if(m_uPeriod <= 1)
          m_fSpinPeriodStart = (float)0; // Baseline or first post-stimulation period.
        else
          m_fSpinPeriodStart = pdInfo->fPeriodStart[m_uPeriod - 1]
                               + pdInfo->fPeriodLen[m_uPeriod - 1];
        m_fSpinPeriodStartLoLim = m_fSpinPeriodStart;
        }
      else
        { // Period has been defined.
        m_fSpinPeriodStart = pdInfo->fPeriodStart[m_uPeriod];
        if(m_uPeriod <= 1)
          m_fSpinPeriodStartLoLim = (float)0; // Baseline or first post-stimulation period.
        else
          m_fSpinPeriodStartLoLim = pdInfo->fPeriodStart[m_uPeriod - 1]
                                    + pdInfo->fPeriodLen[m_uPeriod - 1];
        }
      m_fSpinPeriodEndLoLim = m_fSpinPeriodStartLoLim;
        // Period End minute.
      if(pdInfo->fPeriodLen[m_uPeriod] == INV_PERIOD_LEN)
        {
        if(m_uPeriod == 0)
          { // Baseline
          m_fSpinPeriodEnd = (float)pdInfo->uDataPtsInBaseline
                             / (float)DATA_POINTS_PER_MIN_1; // Baseline
          m_fSpinPeriodEnd = fRound_down_to_half_min(m_fSpinPeriodEnd);
          }
        else // Post-stimulation periods.
          {
          if(m_uPeriod == pdInfo->uNumPeriods) // Last period.
            m_fSpinPeriodEnd = fRound_down_to_half_min(m_pDoc->m_fStudyLen);
          else
            m_fSpinPeriodEnd = pdInfo->fPeriodStart[m_uPeriod + 1];
          }
        m_fSpinPeriodEndHiLim = m_fSpinPeriodEnd;
        m_fPeriodLen = m_fSpinPeriodEndHiLim - m_fSpinPeriodStartLoLim;
        }
      else
        { // Period has been defined.
        m_fSpinPeriodEnd = pdInfo->fPeriodStart[m_uPeriod]
                           + pdInfo->fPeriodLen[m_uPeriod];
        m_fPeriodLen = pdInfo->fPeriodLen[m_uPeriod];
        if(m_uPeriod == 0)
          {
          m_fSpinPeriodEndHiLim = (float)pdInfo->uDataPtsInBaseline
                                  / (float)DATA_POINTS_PER_MIN_1; // Baseline
          m_fSpinPeriodEndHiLim = fRound_down_to_half_min(m_fSpinPeriodEndHiLim);
          }
        else if(m_uPeriod < (MAX_TOTAL_PERIODS - 2))
          { // Able to look at next period.
          if(pdInfo->fPeriodStart[m_uPeriod + 1] == INV_PERIOD_START)
            m_fSpinPeriodEndHiLim = fRound_down_to_half_min(m_pDoc->m_fStudyLen);
          else
            m_fSpinPeriodEndHiLim = pdInfo->fPeriodStart[m_uPeriod + 1];
          }
        else
          { // At last period.
          m_fSpinPeriodEndHiLim = fRound_down_to_half_min(m_pDoc->m_fStudyLen);
          }
        }
      m_fSpinPeriodStartHiLim = m_fSpinPeriodEndHiLim;
      fTemp = m_fPeriodLen * (float)2;
//      if(modf(m_fPeriodLen, &fTemp) != 0)
//        ++m_fPeriodLen;
      ((CSpinButtonCtrl *)GetDlgItem(IDC_START_PERIOD_MIN_SPIN))->SetRange(0,
                                        (short)fTemp);
      ((CSpinButtonCtrl *)GetDlgItem(IDC_END_PERIOD_MIN_SPIN))->SetRange(0, 
                                        (short)fTemp);
      m_cstrPeriodStartMin.Format("%.1f", m_fSpinPeriodStart);
      m_cstrPeriodEndMin.Format("%.1f", m_fSpinPeriodEnd);
    
        // Set the limits on the Good Minutes spin controls based on the
        // period and the selected minutes.
      if(pdInfo->fPeriodLen[m_uPeriod] == INV_PERIOD_LEN)
        { // Period is undefined, set the good minutes limits based on surrounding
          // periods which is what the period start and length are set to.
        m_fSpinStartLoLim = m_fSpinPeriodStartLoLim;
        m_fSpinStartHiLim = m_fSpinPeriodStartHiLim;
        m_fSpinEndLoLim = m_fSpinPeriodEndLoLim;
        m_fSpinEndHiLim = m_fSpinPeriodEndHiLim;
        }
      else
        { // Period is defined, the good minutes limits are the limits
          // of the period.
        m_fSpinStartLoLim = pdInfo->fPeriodStart[m_uPeriod];
        m_fSpinEndLoLim = m_fSpinStartLoLim;
        m_fSpinStartHiLim = m_fSpinStartLoLim + pdInfo->fPeriodLen[m_uPeriod];
        m_fSpinEndHiLim = m_fSpinStartHiLim;
        }
      if(pdInfo->fGoodMinStart[m_uPeriod] != INV_MINUTE)
        m_fSpinStart = pdInfo->fGoodMinStart[m_uPeriod]; // Good minutes defined.
      else
        m_fSpinStart =  m_fSpinPeriodStart; // Good minutes not defined.
      if(pdInfo->fGoodMinEnd[m_uPeriod] != INV_MINUTE)
        m_fSpinEnd = pdInfo->fGoodMinEnd[m_uPeriod]; // Good minutes defined.
      else
        m_fSpinEnd =  m_fSpinPeriodEnd; // Good minutes not defined.
      fTemp = (m_fSpinStartHiLim - m_fSpinStartLoLim) * (float)2;
//      if(modf(fPeriodLen, &fTemp) != 0)
//        ++fPeriodLen;
      ((CSpinButtonCtrl *)GetDlgItem(IDC_START_GOOD_MIN_SPIN))->SetRange(0, (short)fTemp);
      ((CSpinButtonCtrl *)GetDlgItem(IDC_END_GOOD_MIN_SPIN))->SetRange(0, (short)fTemp);
      m_cstrStartGoodMin.Format("%.1f", m_fSpinStart);
  		m_cstrEndGoodMin.Format("%.1f", m_fSpinEnd);
      }

      // Fill in the fields for the event list.
    vFill_events_fields(pdInfo);
    } // End of research period.
    // Save the periods analyzed.
  memcpy(m_bOrigPeriodsAnalyzed, m_pDoc->m_pdPatient.bPeriodsAnalyzed,
         sizeof(m_pDoc->m_pdPatient.bPeriodsAnalyzed));
    // Save the original baseline.
  m_fOrigMeanBaseline = m_pDoc->m_pdPatient.fMeanBaseline;

  UpdateData(FALSE);

//  GetWindowRect(&rWin);
  GetWindowPlacement(&wp10MinView);

//  theApp.vLog("10 Min View rect(t,b,l,r): %d, %d, %d, %d",
//    rWin.top, rWin.bottom, rWin.left, rWin.right);
  g_pFrame->vGet_status_bar_rect(&rStsBarRect);
//  theApp.vLog("Status bar rect(t,b,l,r): %d, %d, %d, %d",
//    rStsBarRect.top, rStsBarRect.bottom, rStsBarRect.left, rStsBarRect.right);
    // Get the coodinates of the patient name field in terms of this
    // window and adjust the graphic rectangles to not cover up
    // the field.
    // There are 3 graphs, calculate the basic rectangle, center
    // it horizontally and the duplicate it 2 more times, each
    // time moving it to just under the one above it.
  GetDlgItem(IDC_PATIENT_NAME)->GetWindowPlacement(&wpTemp);
  iStsBarHeight = rStsBarRect.bottom - rStsBarRect.top;
  rGraph.top = wpTemp.rcNormalPosition.bottom + 5;
  iGraphHeight3 = (wp10MinView.rcNormalPosition.bottom - rGraph.top) - iStsBarHeight - 9;
  rGraph.bottom = (iGraphHeight3 / 3) + rGraph.top;
/*****************************
  GetDlgItem(IDC_PATIENT_NAME)->GetClientRect(&rRect);
  GetDlgItem(IDC_PATIENT_NAME)->MapWindowPoints(this, &rRect);
  rGraph.top = rRect.bottom + 5;
  rGraph.bottom = rWin.bottom / 3;
  iWidth = (rWin.right / 4) * 3;
  rGraph.left = (rWin.right - iWidth) / 2;
*************************************/
  iWidth = (wp10MinView.rcNormalPosition.right / 4) * 3;
  rGraph.left = (wp10MinView.rcNormalPosition.right - iWidth) / 2;
  rGraph.right =  iWidth; 
  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Get the new left, right and top coordinates for the scroll bar.
    iWidth = rScrollBar.right - rScrollBar.left;
    rScrollBar.top = rGraph.top;
    rScrollBar.left = rGraph.right + 1;
    rScrollBar.right = rScrollBar.left + iWidth;
    }
  if(m_hPEData1 == NULL)
    m_hPEData1 = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rGraph, m_hWnd, IDD_10MIN_VIEW);
  else
    PEreset(m_hPEData1);
  if(m_hPEData1 != NULL)
    {
//  theApp.vLog("First graph rect(t,b,l,r): %d, %d, %d, %d",
//    rGraph.top, rGraph.bottom, rGraph.left, rGraph.right);
    giData.uPeriod = uPeriod;
    giData.fMaxEGG = pdInfo->fMaxEGG;
    giData.fMaxResp = pdInfo->fMaxResp;
    giData.fMinEGG = pdInfo->fMinEGG;
    giData.fMinResp = pdInfo->fMinResp;
      // Determine how many minutes go on the graphs and which minute each graph starts on.
    if(theApp.m_ptStudyType == PT_STANDARD)
      { // Standard version
      if(pdInfo->fPeriodLen[uPeriod] == (float)(MIN_PER_PERIOD * 2))
        { // Extra 10 minutes in period.
        fNumMin[0] = (float)8;
        fNumMin[1] = (float)8;
        fNumMin[2] = (float)4;
        giData.fMinOnGraph = 8.0F;
        }
      else
        { // Normal 10 minute period.
        fNumMin[0] = (float)4;
        fNumMin[1] = (float)4;
        fNumMin[2] = (float)2;
        giData.fMinOnGraph = 4.0F;
        }
      giData.fStartMin = pdInfo->fPeriodStart[uPeriod];
      giData.fGraphStartMin = giData.fStartMin;
      giData.pfData = pfData;
      }
    else
      { // Research version
        // Calculate the total number of graphs needed and set the scrollbar.
//      if(m_iFirstMinOnGraph == 1)
        {
//        fPeriodLen = m_fSpinPeriodEndHiLim - m_fSpinPeriodStartLoLim;
        uTotalGraphs = (unsigned short)m_fPeriodLen / m_iNumMinOnGraph;
        if(fmod(m_fPeriodLen, (float)m_iNumMinOnGraph) > (float)0)
          ++uTotalGraphs;
        if(uTotalGraphs > 3)
          { // More graphs than can be displayed on one screen.
            // Calculate the first minute on the last group of graphs.
          m_cGraphScrollBar.ShowWindow(SW_SHOW);
          uTotalPages = uTotalGraphs / 3;
          if((uTotalGraphs % 3) != 0)
            ++uTotalPages;
            // First minute on last page = 
            //  one less than total pages * number of minutes on a graph
            //    * 3 graphs per page.
//          m_iFirstMinOnLastPage = (uTotalPages - 1) * m_iNumMinOnGraph * 3;
          if(m_iFirstMinOnGraph == 1)
            {
            // Set up scroll bar.
					SCROLLINFO si = {0};
					si.cbSize = sizeof(si);
					si.fMask = SIF_ALL;
					si.nMax = uTotalGraphs - 1;
					si.nMin = 0;
					si.nPage = 3; //uTotalPages; // + 1;
					si.nPos = 0;
          m_cGraphScrollBar.ShowWindow(SW_SHOW);
					m_cGraphScrollBar.SetScrollInfo(&si);
            }
          }
        else
          m_cGraphScrollBar.ShowWindow(SW_HIDE);
        }
        // Calculate the number of minutes on each of the 3 graphs.
      vCalc_min_per_graph(m_fPeriodLen, &giData, fNumMin);
      giData.fMinOnGraph = (float)m_iNumMinOnGraph;
//      giData.fStartMin = (float)(m_iFirstMinOnGraph - 1);
      giData.fStartMin = m_fSpinPeriodStart + (float)(m_iFirstMinOnGraph - 1);
      m_fFirstMinOnGraph[0] = giData.fStartMin;
      giData.fGraphStartMin = giData.fStartMin;
        // Calculate where the first point to graph is in the array of
        // data points for the period.
//      if(uPeriod == PERIOD_BL)
//        giData.pfData = pfData;
//      else
        giData.pfData = pfData + ((m_iFirstMinOnGraph - 1) * DATA_POINTS_PER_MIN_1 * 2);
      }
    giData.iNumDataPoints = (int)(fNumMin[0] * (float)DATA_POINTS_PER_MIN_1);
    giData.bAddXAxisLabel = false;
    giData.uAttrib = MINUTES_INTEGERS;
    m_pDoc->vDraw_raw_data_graph(m_hPEData1, &giData); //pfData, 0, uNumPts / 2);
      // Draw the second graph.
    iHeight = rGraph.bottom - rGraph.top;
    rGraph.top = rGraph.bottom;
    rGraph.bottom += iHeight;
    if(m_hPEData2 == NULL)
      m_hPEData2 = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rGraph, m_hWnd, IDD_10MIN_VIEW);
    else
      PEreset(m_hPEData2);
    if(m_hPEData2 != NULL) // && uDataPtsLeft > 0)
      {
//  theApp.vLog("Second graph rect(t,b,l,r): %d, %d, %d, %d",
//    rGraph.top, rGraph.bottom, rGraph.left, rGraph.right);
      giData.iNumDataPoints = (int)(fNumMin[1] * (float)DATA_POINTS_PER_MIN_1);
      if(theApp.m_ptStudyType == PT_STANDARD)
        giData.fStartMin += fNumMin[0];
      else
        {
        if(fNumMin[0] < m_iNumMinOnGraph)
          giData.fStartMin += (float)m_iNumMinOnGraph;
        else
          giData.fStartMin += fNumMin[0];
        }
      m_fFirstMinOnGraph[1] = giData.fStartMin;
      giData.fGraphStartMin = giData.fStartMin;
      m_pDoc->vDraw_raw_data_graph(m_hPEData2, &giData); //pfData, 4, uNumPts / 2);
        // Draw the third graph. 2 minutes.
      iHeight = rGraph.bottom - rGraph.top;
      rGraph.top = rGraph.bottom;
      rGraph.bottom += iHeight;
      if(m_hPEData3 == NULL)
        m_hPEData3 = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rGraph, m_hWnd, IDD_10MIN_VIEW);
      else
        PEreset(m_hPEData3);
      if(m_hPEData3 != NULL) // && uDataPtsLeft > 0)
        {
//  theApp.vLog("Third graph rect(t,b,l,r): %d, %d, %d, %d",
//    rGraph.top, rGraph.bottom, rGraph.left, rGraph.right);
        giData.iNumDataPoints = (int)(fNumMin[2] * (float)DATA_POINTS_PER_MIN_1);
//        giData.fStartMin += fNumMin[1];
        if(theApp.m_ptStudyType == PT_STANDARD)
          giData.fStartMin += fNumMin[1];
        else
          {
          if(fNumMin[1] < m_iNumMinOnGraph)
            giData.fStartMin += (float)m_iNumMinOnGraph;
          else
            giData.fStartMin += fNumMin[1];
          }
        m_fFirstMinOnGraph[2] = giData.fStartMin;
        giData.fGraphStartMin = giData.fStartMin;
        giData.bAddXAxisLabel = true;
        m_pDoc->vDraw_raw_data_graph(m_hPEData3, &giData); //pfData, 8, uNumPts / 4);
        }
      }
      // Now that we have graphed the data, make sure the combobox
      // selection shows the current period.
    vUpdate_period_combobox(uPeriod);
    }

  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Research study.
      // Move and resize the scroll bar.  Put it immediately to the left of
      // the graphs and make it extend from top to bottom of the 3 graphs.
    if(m_hPEData1 != 0 && m_hPEData3 != 0)
      {
//      CWnd *pcwnd;
      WINDOWPLACEMENT wp;
  
//      pcwnd = GetDlgItem(IDC_SCROLLBAR_GRAPH);
//      if(m_cGraphScrollBar.IsWindowEnabled() == FALSE)
        {
        rScrollBar.bottom = rGraph.bottom;
        m_cGraphScrollBar.GetWindowPlacement(&wp); // Dimensions and position of field.
        wp.rcNormalPosition = rScrollBar;
        m_cGraphScrollBar.SetWindowPlacement(&wp);
        if(uTotalGraphs <= 3)
          { // 3 or less graphs.  No need for scroll bar.
          m_cGraphScrollBar.ShowWindow(SW_HIDE);
          }
        else
          m_cGraphScrollBar.ShowWindow(SW_SHOW);
        m_cGraphScrollBar.EnableWindow(TRUE);
        }
      }
    }
  }

/********************************************************************
vCalc_min_per_graph - Calculate the number of point that go on each
                      of the 3 graphs.

  inputs: Number of minutes in the period.
          Pointer to a GRAPH_INFO structure to save the number of minutes
            on a graph for the graphing routine to label the graphs.
          Pointer to an array to store the number of minutes on each graph.

  return: None.
********************************************************************/
void CTenMinView::vCalc_min_per_graph(float fPeriodLen, GRAPH_INFO *pgiData,
float *pfNumMin)
  {
  float fNumMinLeft;

  fNumMinLeft = fPeriodLen - (float)(m_iFirstMinOnGraph - 1);
//  if(floor(fPeriodLen) < fPeriodLen)
//    ++fNumMinLeft;
  *pfNumMin = (float)0;
  *(pfNumMin + 1) = (float)0;
  *(pfNumMin + 2) = (float)0;
  if(fNumMinLeft > (float)m_iNumMinOnGraph)
    {
    *pfNumMin = (float)m_iNumMinOnGraph;
    fNumMinLeft -= (float)m_iNumMinOnGraph;
    if(fNumMinLeft > (float)m_iNumMinOnGraph)
      {
      *(pfNumMin + 1) = (float)m_iNumMinOnGraph;
      fNumMinLeft -= (float)m_iNumMinOnGraph;
      if(fNumMinLeft > (float)m_iNumMinOnGraph)
        *(pfNumMin + 2) = (float)m_iNumMinOnGraph;
      else
        *(pfNumMin + 2) = fNumMinLeft;
      }
    else
      *(pfNumMin + 1) = fNumMinLeft;
    }
  else
    *pfNumMin = fNumMinLeft;
//  pgiData->fMinOnGraph = *pfNumMin;
  }

/********************************************************************
vEnable_fields - Enable/disable fields in the form view.
                 Fields enabled/disabled:
                   Combobox used to select the period.

  inputs: TRUE - enable the fields, FALSE - disable the fields.

  return: None.
********************************************************************/
void CTenMinView::vEnable_fields(BOOL bEnable)
  {
  m_c10MinPeriodLst.EnableWindow(bEnable);
  }

/********************************************************************
vUpdate_period_combobox - Select the period in the combobox that matches
                          the specified period.

                          Called by the parent so that the combobox shows
                          the correct period that is on the graphs.

  inputs: 10 minute period to select from the combobox.

  return: None.
********************************************************************/
void CTenMinView::vUpdate_period_combobox(unsigned short uPeriod)
  {
  CString cstrText, cstrTemp, cstrBaseText;
  int iCnt, iIndex;
  unsigned short uIndex; // uPeriod40, 

  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard version
    for(iCnt = 0; iCnt < m_c10MinPeriodLst.GetCount(); ++iCnt)
      {
      if(uPeriod == (unsigned short)m_c10MinPeriodLst.GetItemData(iCnt))
        {
        m_c10MinPeriodLst.SetCurSel(iCnt);
        break;
        }
      }
/**************************************
      // Look to see if this study has a fourth 10 minute period.  If it does
      // and its not yet in the Combobox, add it.
    if(m_pDoc->m_pdPatient.bExtra10Min == TRUE)
      {
      for(iCnt = 0; iCnt < m_c10MinPeriodLst.GetCount(); ++iCnt)
        {
        if((uPeriod40 = (unsigned short)m_c10MinPeriodLst.GetItemData(iCnt)) == PERIOD_40)
          break;
        }
      if(uPeriod40 != PERIOD_40)
        {
        cstrText.LoadString(IDS_FOURTH_10_MIN_PD);
        iIndex = m_c10MinPeriodLst.AddString(cstrText);
        m_c10MinPeriodLst.SetItemData(iIndex, (DWORD)PERIOD_40);
        }
      }
********************************/
    }
  else
    { // Research version
      // Since the number of periods can increase or decrease, empty the
      // combobox and refill it.
    m_c10MinPeriodLst.ResetContent();
      // Always put in the baseline.
    //cstrText.LoadString(IDS_BASELINE);
    cstrText = g_pLang->cstrLoad_string(ITEM_G_BASELINE);
    iIndex = m_c10MinPeriodLst.AddString(cstrText);
    m_c10MinPeriodLst.SetItemData(iIndex, (DWORD)PERIOD_BL);
    if(m_pDoc->m_pdPatient.uNumPeriods >= 1)
      {
      //cstrBaseText.LoadString(IDS_POST_STIM_PERIOD);
      cstrBaseText = g_pLang->cstrLoad_string(ITEM_G_POST_STIM_PERIOD);
      for(uIndex = 0; uIndex < m_pDoc->m_pdPatient.uNumPeriods; ++uIndex)
        {
        cstrText.Format("%s %d", cstrBaseText, uIndex + 1);
        iIndex = m_c10MinPeriodLst.AddString(cstrText);
        m_c10MinPeriodLst.SetItemData(iIndex, (DWORD)(uIndex + 1));
        }
      }
    }
    // Select the current period.
  for(iCnt = 0; iCnt < m_c10MinPeriodLst.GetCount(); ++iCnt)
    {
    if(uPeriod == (unsigned short)m_c10MinPeriodLst.GetItemData(iCnt))
      {
      m_c10MinPeriodLst.SetCurSel(iCnt);
      break;
      }
    }
  }

/********************************************************************
vShow_analyze_fields - Show or hide the fields used to display the
                       artifact and good minutes, and the accept and
                       reject buttons.
                       If they are to be hidden, also hide the fields
                       used to select the good minutes. Otherwise leave
                       these fields alone.

                       NOTE: This is for the standard version only.

  inputs: Show or hide.

  return: None.
********************************************************************/
void CTenMinView::vShow_analyze_fields(int iShow)
  {

  GetDlgItem(IDC_SUGGESTED_MIN_FRAME)->ShowWindow(iShow);
  GetDlgItem(IDC_ARTIFACT_TITLE)->ShowWindow(iShow);
  GetDlgItem(IDC_ARTIFACT_LIST)->ShowWindow(iShow);
  GetDlgItem(IDC_GOOD_MIN_TITLE)->ShowWindow(iShow);
  GetDlgItem(IDC_GOOD_MIN)->ShowWindow(iShow);
  GetDlgItem(IDC_ACCEPT)->ShowWindow(iShow);
  GetDlgItem(IDC_REJECT)->ShowWindow(iShow);
  if(iShow == SW_HIDE)
    vShow_min_sel_fields(iShow);

  if(g_bAsciiDataFile == true)
    {
    GetDlgItem(IDC_SUGGESTED_MIN_FRAME)->EnableWindow(FALSE);
    GetDlgItem(IDC_ARTIFACT_TITLE)->EnableWindow(FALSE);
    //GetDlgItem(IDC_ARTIFACT_LIST)->EnableWindow(FALSE);
    GetDlgItem(IDC_GOOD_MIN_TITLE)->EnableWindow(FALSE);
    GetDlgItem(IDC_GOOD_MIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_ACCEPT)->EnableWindow(FALSE);
    GetDlgItem(IDC_REJECT)->EnableWindow(FALSE);
    }
  }

/********************************************************************
vShow_min_sel_fields - Show or hide the fields used to select the good
                       minutes.  These are the fields used if the user
                       selects the reject button.

  inputs: Show or hide.

  return: None.
********************************************************************/
void CTenMinView::vShow_min_sel_fields(int iShow)
  {

  GetDlgItem(IDC_SELECT_NEW_MINUTES)->ShowWindow(iShow);
  GetDlgItem(IDC_START_MIN_TITLE)->ShowWindow(iShow);
  GetDlgItem(IDC_START_MIN)->ShowWindow(iShow);
  GetDlgItem(IDC_END_MIN_TITLE)->ShowWindow(iShow);
  GetDlgItem(IDC_END_MIN)->ShowWindow(iShow);
  GetDlgItem(IDC_START_MIN_SPIN)->ShowWindow(iShow);
  GetDlgItem(IDC_END_MIN_SPIN)->ShowWindow(iShow);
  GetDlgItem(IDC_UNDO_CHANGES)->ShowWindow(iShow);
  GetDlgItem(IDC_ACCEPT_CHANGES)->ShowWindow(iShow);

    if(g_bAsciiDataFile == true)
    {
    GetDlgItem(IDC_SELECT_NEW_MINUTES)->EnableWindow(FALSE);
    GetDlgItem(IDC_START_MIN_TITLE)->EnableWindow(FALSE);
    GetDlgItem(IDC_START_MIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_END_MIN_TITLE)->EnableWindow(FALSE);
    GetDlgItem(IDC_END_MIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_START_MIN_SPIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_END_MIN_SPIN)->EnableWindow(FALSE);
    GetDlgItem(IDC_UNDO_CHANGES)->EnableWindow(FALSE);
    GetDlgItem(IDC_ACCEPT_CHANGES)->EnableWindow(FALSE);
    }
  }

/********************************************************************
vMove_analyze_fields - Move all the fields used for the analysis (artifact
                       list, good minutes, minute selection fields) to
                       be visible on the screen.  These fields are initially
                       behind one or more of the graphs and have to be
                       moved to be seen.

  inputs: None.

  return: None.
********************************************************************/
void CTenMinView::vMove_analyze_fields()
  {
  RECT rWin;
  int iDistMoved;

  GetClientRect(&rWin); // Dimensions of dialog box client area.
  CalcWindowRect(&rWin, CWnd::adjustBorder); // Adjust client area to exclude scroll bars.

  iDistMoved = 0;
    // This needs to be the first field moved.
  vMove_field(IDC_SUGGESTED_MIN_FRAME, &rWin, &iDistMoved);
  vMove_field(IDC_ARTIFACT_TITLE, &rWin, &iDistMoved);
  vMove_field(IDC_ARTIFACT_LIST, &rWin, &iDistMoved);
  vMove_field(IDC_GOOD_MIN_TITLE, &rWin, &iDistMoved);
  vMove_field(IDC_GOOD_MIN, &rWin, &iDistMoved);
  vMove_field(IDC_ACCEPT, &rWin, &iDistMoved);
  vMove_field(IDC_REJECT, &rWin, &iDistMoved);
  vMove_field(IDC_SELECT_NEW_MINUTES, &rWin, &iDistMoved);
  vMove_field(IDC_START_MIN_TITLE, &rWin, &iDistMoved);
  vMove_field(IDC_START_MIN, &rWin, &iDistMoved);
  vMove_field(IDC_END_MIN_TITLE, &rWin, &iDistMoved);
  vMove_field(IDC_END_MIN, &rWin, &iDistMoved);
  vMove_field(IDC_START_MIN_SPIN, &rWin, &iDistMoved);
  vMove_field(IDC_END_MIN_SPIN, &rWin, &iDistMoved);
  vMove_field(IDC_UNDO_CHANGES, &rWin, &iDistMoved);
  vMove_field(IDC_ACCEPT_CHANGES, &rWin, &iDistMoved);
  m_bAnalyzedFieldsMoved = true;
  }

/********************************************************************
vMove_field - Move a field as follows:
                - The first field of the group needs to be the widest.
                - Move it so that there are 10 pixels between the right
                  edge of the field and the right edge of the form.
                - Save the distance moved by the first field and use it
                  to move all the remaining fields.

  inputs: Id of the field being moved.
          Pointer to a RECT structure containing the rectangle of the form.
          Pointer to the distance the field needs to be moved.  If this distance
            is zero, the distance is calculated and stored.

  return: None.
********************************************************************/
void CTenMinView::vMove_field(unsigned uFieldID, RECT *prWin, int *piDistMoved)
  {
  CWnd *pcwnd;
  WINDOWPLACEMENT wp;

  pcwnd = GetDlgItem(uFieldID);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  if(*piDistMoved == 0)
    { // First field moved, save the distance moved.
    *piDistMoved = prWin->right - wp.rcNormalPosition.right - 10;
    }
  wp.rcNormalPosition.right += *piDistMoved;
  wp.rcNormalPosition.left += *piDistMoved;
  pcwnd->SetWindowPlacement(&wp);
  }

/********************************************************************
vInit_analyze_fields - Initialize the fields used for analysis.
                         - Search for the maximum contiguous minutes
                           that are artifact free in the period.
                         - Add artifact to the artifact list.
                         - If in manual mode, hide the Accept and Reject
                           buttons and display the minute seslection fields.

  inputs: None.

  return: None.
********************************************************************/
void CTenMinView::vInit_analyze_fields()
  {
  CString cstrTemp;
  short int iPos;
  float fPeriodStart, fStart, fEnd;

  m_iAccept = -1;
  m_cArtifactList.ResetContent();
  fPeriodStart = m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod];
  if(m_pDoc->m_pdPatient.bManualMode == FALSE)
    {
    if(m_pDoc->m_pArtifact != NULL)
      { // Fill in the artifact list box.
      iPos = FIRST_ART;
      while(m_pDoc->m_pArtifact->bGet_next_artifact(m_uPeriod, iPos, &fStart, &fEnd) == true)
        {
        cstrTemp.Format("%.1f - %.1f", fStart + fPeriodStart, fEnd + fPeriodStart);
        m_cArtifactList.AddString(cstrTemp);
        iPos = NEXT_ART;
        }
      }
    }
    // Get the good minutes for this period.
  if(m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod] == INV_MINUTE)
    {
    m_fStart = INV_MINUTE;
    m_fEnd = INV_MINUTE;
    if(m_pDoc->m_pdPatient.bManualMode == FALSE)
      m_pDoc->m_pArtifact->bFind_art_free_min(m_uPeriod, &m_fStart, &m_fEnd);
    }
  else
    {
    m_iAccept = 0; // Good minutes already selected.
    m_fStart = m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod];
    m_fEnd = m_pDoc->m_pdPatient.fGoodMinEnd[m_uPeriod];
    }
  if(m_fStart != INV_MINUTE)
    { // Found artifact free minutes in half minute increments.
      // The minutes are 0 to 10.  Convert them to minutes within the period.
    m_cstrGoodMin.Format("%.1f - %.1f", m_fStart + fPeriodStart, 
                         m_fEnd + fPeriodStart);
    }
  else
    m_cstrGoodMin.Format("%.1f - %.1f", fPeriodStart, 
                         fPeriodStart + m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod]);
  if(m_pDoc->m_pdPatient.bManualMode == TRUE)
    { // In manual mode, hide accept/reject buttons, default to full period
      // for good minutes.
	  GetDlgItem(IDC_ACCEPT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_REJECT)->ShowWindow(SW_HIDE);
      // Show the additional dialog box fields.
    vShow_min_sel_fields(SW_SHOW);
    GetDlgItem(IDC_START_MIN)->SetFocus();
    }
  }

/********************************************************************
bValid_minute_format - Make sure the minute format is correct.
                         - Minute must start on the minute or the half minute.
                         - Minutes have been limited to 4 characters.

  inputs: CString object containing a user entered minute.

  return: true if the minute meets the above criteria.
          false if the minute is not valid.
********************************************************************/
bool CTenMinView::bValid_minute_format(CString cstrMinute)
  {
  int iIndex;
  bool bRet = false;

  if(MINUTE_DELTA == 0.5F)
    {
    if((iIndex = cstrMinute.Find('.')) >= 0)
      { // Have a decimal point, only a 5 or 0 is allowed to the right.
      ++iIndex; // Index of character after the decimal point.
      if(cstrMinute.GetLength() > iIndex)
        { // Have characters after the decimal point. There can be 2 at most.
        if(cstrMinute.GetAt(iIndex) == '0' || cstrMinute.GetAt(iIndex) == '5')
          { // So far OK.  There may be one more character so we need to check it.
          ++iIndex; // Index of next character.
          if(cstrMinute.GetLength() > iIndex)
            { // Last character. It must be a 0.
            if(cstrMinute.GetAt(iIndex) == '0')
              bRet = true;
            }
          else
            bRet = true; // No more characters.
          }
        }
      else
        bRet = true; // No characters after decimal point, on the minute.
      }
    else
      bRet = true; // This is on the minute.
    }
  else
    bRet = true; // This is on the minute.
  return(bRet);
  }

/********************************************************************
vShow_logo - Show or hide the logos.

inputs: true to show logos, false to hide logos.

return: None.
********************************************************************/
void CTenMinView::vShow_logo(bool bShow)
  {

  if(bShow == true)
    {
    if(m_bLogoVisible == false)
      { // Make the logo fields visible.
//      GetDlgItem(IDC_VISISPACE_LOGO)->ShowWindow(TRUE);
//      GetDlgItem(IDC_PRODUCT_OF)->ShowWindow(TRUE);
      GetDlgItem(IDC_LOGO_3CPM)->ShowWindow(TRUE);
      m_bLogoVisible = true;
      }
    }
  else
    {
    if(m_bLogoVisible == true)
      { // Hide the logo fields.
//      GetDlgItem(IDC_VISISPACE_LOGO)->ShowWindow(FALSE);
//      GetDlgItem(IDC_PRODUCT_OF)->ShowWindow(FALSE);
      GetDlgItem(IDC_LOGO_3CPM)->ShowWindow(FALSE);
      m_bLogoVisible = false;
      }
    }
  }

/********************************************************************
vSet_periods_analyzed_from_orig - Set the periods analyzed to analyzed
                                  based on what was originally analyzed.

inputs: None.

return: None.
********************************************************************/
void CTenMinView::vSet_periods_analyzed_from_orig()
  {
  int iIndex;

  for(iIndex = 0; iIndex < MAX_TOTAL_PERIODS; ++iIndex)
    {
    if(m_bOrigPeriodsAnalyzed[iIndex] == true)
      m_pDoc->m_pdPatient.bPeriodsAnalyzed[iIndex] = true;
    }
  }

/********************************************************************
vMove_research_analyze_fields - Move the analyze fields used for the
                                research version to the upper right corner
                                of the view window.

inputs: None.

return: None.
********************************************************************/
void CTenMinView::vMove_research_analyze_fields()
  {
  RECT rWin;
//  int iDistMoved;
  CWnd *pcwnd;
  WINDOWPLACEMENT wp, wp1, wp2, wpEventListGrpBox;
  int iWidth, iLeftPos, iLeftOffset, iY, iX, iHeight, iDistance1, iDistance2;
  int iFrameX;
  
    // Get left side X coord.
  GetClientRect(&rWin); // Dimensions of dialog box client area.
  CalcWindowRect(&rWin, CWnd::adjustBorder); // Adjust client area to exclude scroll bars.
  iX = rWin.right;
    // Get Starting Y coord.
  GetDlgItem(IDC_PATIENT_NAME)->GetWindowPlacement(&wp);
  iY = wp.rcNormalPosition.bottom;

  pcwnd = GetDlgItem(IDC_PERIOD_GOOD_MIN_FRAME);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.

  iHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
  wp.rcNormalPosition.top = iY + 5; //rWin.top + 10;
  wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + iHeight;
  iWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
  wp.rcNormalPosition.right = iX - 10; //rWin.right - 10;
  wp.rcNormalPosition.left = wp.rcNormalPosition.right - iWidth;


//  iDistMoved = rWin.right - wp.rcNormalPosition.right - 10;
//  wp.rcNormalPosition.right += iDistMoved;
//  wp.rcNormalPosition.left += iDistMoved;
  pcwnd->SetWindowPlacement(&wp);
  rWin = wp.rcNormalPosition;
  iY = rWin.top + 10;

    // Calculate total width of fields
  pcwnd = GetDlgItem(IDC_START_GOOD_MIN_SPIN);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iWidth = wp.rcNormalPosition.right;
  pcwnd = GetDlgItem(IDC_SELECT_GOOD_MINUTES);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iX = wp.rcNormalPosition.left;
  iWidth = iWidth - wp.rcNormalPosition.left;
    // Left most position for title
  iLeftPos = (((rWin.right - rWin.left) - iWidth) / 2) + rWin.left;
    // Left offset (relative to title) of field label.
  pcwnd = GetDlgItem(IDC_START_GOOD_MIN_TITLE);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iLeftOffset = wp.rcNormalPosition.left - iX;
    // Distance between field label and field.
  iDistance1 = wp.rcNormalPosition.right;
  pcwnd = GetDlgItem(IDC_START_GOOD_MIN);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iDistance1 = wp.rcNormalPosition.left - iDistance1;
    // Distance between field and spin control.
  iDistance2 = wp.rcNormalPosition.right;
  pcwnd = GetDlgItem(IDC_START_GOOD_MIN_SPIN);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iDistance2 = wp.rcNormalPosition.left - iDistance2;

    // Move Select Period Length.
  pcwnd = GetDlgItem(IDC_SELECT_PERIOD_LEN);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
  iHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
  wp.rcNormalPosition.left = iLeftPos;
  wp.rcNormalPosition.right = iLeftPos + iWidth;
  wp.rcNormalPosition.top = iY;
  wp.rcNormalPosition.bottom = iY + iHeight;
  pcwnd->SetWindowPlacement(&wp);
  iY += (iHeight + 10);
    // Move the Start minute period field.
  vMove_research_sel_min_group(IDC_START_PERIOD_MIN_TITLE, IDC_START_PERIOD_MIN,
                               IDC_START_PERIOD_MIN_SPIN, iLeftPos + iLeftOffset,
                               iDistance1, iDistance2, &iY);
    // Move the End minute period field.
  iY += 10;
  vMove_research_sel_min_group(IDC_END_PERIOD_MIN_TITLE, IDC_END_PERIOD_MIN,
                               IDC_END_PERIOD_MIN_SPIN, iLeftPos + iLeftOffset,
                               iDistance1, iDistance2, &iY);
    // Move the Set Period Length button.
  iY += 10;
  pcwnd = GetDlgItem(IDC_CHK_SET_PERIOD_LENGTH); //IDC_SET_PERIOD_LENGTH);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
  iHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
  wp.rcNormalPosition.top = iY;
  wp.rcNormalPosition.bottom  = iY + iHeight;
    // Center the button.
  wp.rcNormalPosition.left = (((rWin.right - rWin.left) - iWidth) / 2) + rWin.left;
  wp.rcNormalPosition.right = wp.rcNormalPosition.left + iWidth;
  pcwnd->SetWindowPlacement(&wp);
  iY += (iHeight + 20);

    // Move Select Good Minutes.
  pcwnd = GetDlgItem(IDC_SELECT_GOOD_MINUTES);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
  iHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
  wp.rcNormalPosition.left = iLeftPos;
  wp.rcNormalPosition.right = iLeftPos + iWidth;
  wp.rcNormalPosition.top = iY;
  wp.rcNormalPosition.bottom = iY + iHeight;
  pcwnd->SetWindowPlacement(&wp);
  iY += (iHeight + 10);
    // Move the Start good minute field.
  vMove_research_sel_min_group(IDC_START_GOOD_MIN_TITLE, IDC_START_GOOD_MIN,
                               IDC_START_GOOD_MIN_SPIN, iLeftPos + iLeftOffset,
                               iDistance1, iDistance2, &iY);
    // Move the End good minute field.
  iY += 10;
  vMove_research_sel_min_group(IDC_END_GOOD_MIN_TITLE, IDC_END_GOOD_MIN,
                               IDC_END_GOOD_MIN_SPIN, iLeftPos + iLeftOffset,
                               iDistance1, iDistance2, &iY);
    // Move the Set good minute button.
  iY += 10;
  pcwnd = GetDlgItem(IDC_CHK_SET_GOOD_MINUTES);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
  iHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
  wp.rcNormalPosition.top = iY;
  wp.rcNormalPosition.bottom  = iY + iHeight;
    // Center the button.
  wp.rcNormalPosition.left = (((rWin.right - rWin.left) - iWidth) / 2) + rWin.left;
  wp.rcNormalPosition.right = wp.rcNormalPosition.left + iWidth;
  pcwnd->SetWindowPlacement(&wp);

    // Adjust the frame to it encloses all the above fields.
  pcwnd = GetDlgItem(IDC_PERIOD_GOOD_MIN_FRAME);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  wp.rcNormalPosition.bottom = iY + iHeight + 10;
  pcwnd->SetWindowPlacement(&wp);

    // Move the Delete this period button to just below the frame and centered
    // between the frame sides.
  pcwnd = GetDlgItem(IDC_RESET_PERIODS);
  pcwnd->GetWindowPlacement(&wp1); // Dimensions and position of field.
  iWidth = wp1.rcNormalPosition.right - wp1.rcNormalPosition.left;
  iHeight = wp1.rcNormalPosition.bottom - wp1.rcNormalPosition.top;
  wp1.rcNormalPosition.left = 
    ((wp.rcNormalPosition.right - wp.rcNormalPosition.left)
    - (wp1.rcNormalPosition.right - wp1.rcNormalPosition.left)) / 2
    + wp.rcNormalPosition.left;
  wp1.rcNormalPosition.right = wp1.rcNormalPosition.left + iWidth;
  wp1.rcNormalPosition.top = wp.rcNormalPosition.bottom + 10;
  wp1.rcNormalPosition.bottom = wp1.rcNormalPosition.top + iHeight;
  pcwnd->SetWindowPlacement(&wp1);

/****************************************
    // Move the Insert new period button.
  iY = wp1.rcNormalPosition.bottom + 10;
  pcwnd = GetDlgItem(IDC_INSERT_NEW_PERIOD);
  pcwnd->GetWindowPlacement(&wp1); // Dimensions and position of field.
  iWidth = wp1.rcNormalPosition.right - wp1.rcNormalPosition.left;
  iHeight = wp1.rcNormalPosition.bottom - wp1.rcNormalPosition.top;
  wp1.rcNormalPosition.left = 
    ((wp.rcNormalPosition.right - wp.rcNormalPosition.left)
    - (wp1.rcNormalPosition.right - wp1.rcNormalPosition.left)) / 2
    + wp.rcNormalPosition.left;
  wp1.rcNormalPosition.right = wp1.rcNormalPosition.left + iWidth;
  wp1.rcNormalPosition.top = iY;
  wp1.rcNormalPosition.bottom = wp1.rcNormalPosition.top + iHeight;
  pcwnd->SetWindowPlacement(&wp1);
***************************************/

    // Move the group box for the events list.
    // First get the X and Y offset of the listbox within the group box.
  pcwnd = GetDlgItem(IDC_FRAME_PERIOD_EVENT);
  pcwnd->GetWindowPlacement(&wpEventListGrpBox); // Dimensions and position of field.
  pcwnd = GetDlgItem(IDC_EVENT_LIST);
  pcwnd->GetWindowPlacement(&wp2); // Dimensions and position of field.
  iDistance1 = wp2.rcNormalPosition.top - wpEventListGrpBox.rcNormalPosition.top;
  iDistance2 = wp2.rcNormalPosition.left - wpEventListGrpBox.rcNormalPosition.left;

  iY = wp1.rcNormalPosition.bottom + 10;
  pcwnd = GetDlgItem(IDC_FRAME_PERIOD_EVENT);
  pcwnd->GetWindowPlacement(&wp1); // Dimensions and position of field.
  iWidth = wp1.rcNormalPosition.right - wp1.rcNormalPosition.left;
  iHeight = wp1.rcNormalPosition.bottom - wp1.rcNormalPosition.top;
  wp1.rcNormalPosition.left = 
    ((wp.rcNormalPosition.right - wp.rcNormalPosition.left)
    - (wp1.rcNormalPosition.right - wp1.rcNormalPosition.left)) / 2
    + wp.rcNormalPosition.left;
  wp1.rcNormalPosition.right = wp1.rcNormalPosition.left + iWidth;
  wp1.rcNormalPosition.top = iY;
  wp1.rcNormalPosition.bottom = wp1.rcNormalPosition.top + iHeight;
  pcwnd->SetWindowPlacement(&wp1);
  iLeftPos = wp1.rcNormalPosition.left + iDistance2;
  iFrameX = wp1.rcNormalPosition.left;

    // Move the events listbox into the group box.
  iY = wp1.rcNormalPosition.top + iDistance1;
  pcwnd = GetDlgItem(IDC_EVENT_LIST);
  pcwnd->GetWindowPlacement(&wp1); // Dimensions and position of field.
  iWidth = wp1.rcNormalPosition.right - wp1.rcNormalPosition.left;
  iHeight = wp1.rcNormalPosition.bottom - wp1.rcNormalPosition.top;
  wp1.rcNormalPosition.left = iLeftPos;
  wp1.rcNormalPosition.right = wp1.rcNormalPosition.left + iWidth;
  wp1.rcNormalPosition.top = iY;
  wp1.rcNormalPosition.bottom = wp1.rcNormalPosition.top + iHeight;
  pcwnd->SetWindowPlacement(&wp1);
  
    // Move the event description title into the group box.
  iY = wp1.rcNormalPosition.bottom + 10;
  pcwnd = GetDlgItem(IDC_EVENT_DESC_TITLE);
  pcwnd->GetWindowPlacement(&wp1); // Dimensions and position of field.
  iWidth = wp1.rcNormalPosition.right - wp1.rcNormalPosition.left;
  iHeight = wp1.rcNormalPosition.bottom - wp1.rcNormalPosition.top;
  wp1.rcNormalPosition.left = iLeftPos;
  wp1.rcNormalPosition.right = wp1.rcNormalPosition.left + iWidth;
  wp1.rcNormalPosition.top = iY;
  wp1.rcNormalPosition.bottom = wp1.rcNormalPosition.top + iHeight;
  pcwnd->SetWindowPlacement(&wp1);

    // Move the event description into the group box.
  iY = wp1.rcNormalPosition.bottom + 5;
  pcwnd = GetDlgItem(IDC_EVENT_DESCRIPTION);
  pcwnd->GetWindowPlacement(&wp1); // Dimensions and position of field.
  iWidth = wp1.rcNormalPosition.right - wp1.rcNormalPosition.left;
  iHeight = wp1.rcNormalPosition.bottom - wp1.rcNormalPosition.top;
  wp1.rcNormalPosition.left = iLeftPos;
  wp1.rcNormalPosition.right = wp1.rcNormalPosition.left + iWidth;
  wp1.rcNormalPosition.top = iY;
  wp1.rcNormalPosition.bottom = wp1.rcNormalPosition.top + iHeight;
  pcwnd->SetWindowPlacement(&wp1);

    // Move the Edit button into the group box.
  iY = wp1.rcNormalPosition.bottom + 10;
  pcwnd = GetDlgItem(IDC_BTN_EVENTS_EDIT);
  pcwnd->GetWindowPlacement(&wp1); // Dimensions and position of field.
  iWidth = wp1.rcNormalPosition.right - wp1.rcNormalPosition.left;
  iHeight = wp1.rcNormalPosition.bottom - wp1.rcNormalPosition.top;
  wp1.rcNormalPosition.left = iFrameX
    + (wp1.rcNormalPosition.left - wpEventListGrpBox.rcNormalPosition.left);
  wp1.rcNormalPosition.right = wp1.rcNormalPosition.left + iWidth;
  wp1.rcNormalPosition.top = iY;
  wp1.rcNormalPosition.bottom = wp1.rcNormalPosition.top + iHeight;
  pcwnd->SetWindowPlacement(&wp1);

    // Move the Delete button into the group box next to the Edit button.
  iY = wp1.rcNormalPosition.top;
  iX = wp1.rcNormalPosition.right + 20;
  pcwnd = GetDlgItem(IDC_BTN_EVENTS_DELETE);
  pcwnd->GetWindowPlacement(&wp1); // Dimensions and position of field.
  iWidth = wp1.rcNormalPosition.right - wp1.rcNormalPosition.left;
  iHeight = wp1.rcNormalPosition.bottom - wp1.rcNormalPosition.top;
  wp1.rcNormalPosition.left = iX;
  wp1.rcNormalPosition.right = wp1.rcNormalPosition.left + iWidth;
  wp1.rcNormalPosition.top = iY;
  wp1.rcNormalPosition.bottom = wp1.rcNormalPosition.top + iHeight;
  pcwnd->SetWindowPlacement(&wp1);
  }

/********************************************************************
vMove_research_sel_min_group - Move a select minutes field grouping.
                               A grouping consists of a label field,
                               an edit field (for minutes) and a spin
                               control.

inputs: Field ID of the first field.
        Field ID of the second field.
        Field ID of the third field.
        Left X coordinate of the left most field (label field).
        Distance from the right X coordinate of the first field to 
          the next field (edit field).
        Distance from the right X coordinate of the second field to
          the next field (spin control).
        Pointer to the Top Y coordinate for all fields.  This gets updated
          to contain the bottom Y coordinate of the fields.

return: None.
********************************************************************/
void CTenMinView::vMove_research_sel_min_group(unsigned uFieldID1,
unsigned uFieldID2, unsigned uFieldID3, int iX, int iDistance1, int iDistance2, int *piY)
  {
  CWnd *pcwnd;
  WINDOWPLACEMENT wp;
  int iWidth, iHeight;

    // Move Start minute title.
  pcwnd = GetDlgItem(uFieldID1);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
  iHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
  wp.rcNormalPosition.left = iX; //iLeftPos + iLeftOffset;
  wp.rcNormalPosition.right = wp.rcNormalPosition.left + iWidth;
  wp.rcNormalPosition.top = *piY;
  wp.rcNormalPosition.bottom = *piY + iHeight;
  pcwnd->SetWindowPlacement(&wp);
  iX = wp.rcNormalPosition.right;

    // Move Start minute field.
  pcwnd = GetDlgItem(uFieldID2);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
  iHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
  wp.rcNormalPosition.left = iX + iDistance1;
  wp.rcNormalPosition.right = wp.rcNormalPosition.left + iWidth;
  wp.rcNormalPosition.top = *piY;
  wp.rcNormalPosition.bottom = *piY + iHeight;
  pcwnd->SetWindowPlacement(&wp);
  iX = wp.rcNormalPosition.right;

    // Move Start minute spin control.
  pcwnd = GetDlgItem(uFieldID3);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
  iHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
  wp.rcNormalPosition.left = iX + iDistance2;
  wp.rcNormalPosition.right = wp.rcNormalPosition.left + iWidth;
  wp.rcNormalPosition.top = *piY;
  wp.rcNormalPosition.bottom = *piY + iHeight;
  pcwnd->SetWindowPlacement(&wp);

  *piY += iHeight;
  }

/********************************************************************
vInit_research_data - Initialize data used for the research version.

inputs: None.

return: None.
********************************************************************/
void CTenMinView::vInit_research_data()
  {

  m_iNumMinOnGraph = m_pDoc->m_iMinPerGraph;
  m_iFirstMinOnGraph = 1;
  }

/********************************************************************
bValidate_good_minutes - Validate the start and end good minutes.
                         - Make sure the format is correct.
                         - The minutes must be within the period.
                         - There must be at least 4 minutes between the
                           start and end minutes.

  inputs: Pointer to a variable that gets the start minute.
          Pointer to a variable that gets the end minute.
          String containing the start minute
          String containing the end minute

  return: true if the minutes are OK, otherwise false.
********************************************************************/
bool CTenMinView::bValidate_good_minutes(float *pfStartMin, float *pfEndMin,
CString cstrStartMin, CString cstrEndMin)
  {
  bool bRet;
  float fStartMinute, fEndMinute;
  CString cstrMsg, cstrTitle;

    // Make sure that the minutes entered are valid.
    // Verify that minutes must start on the minute or half minute.
  if((bRet = bValid_minute_format(cstrStartMin)) == false)
    { // Start minute doesn't start on the minute or half minute.
    //cstrMsg.LoadString(IDS_START_MINUTE_FORMAT_ERR);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_START_MINUTE_FORMAT_ERR);
    }
  else if((bRet = bValid_minute_format(cstrEndMin)) == false)
    { // End minute doesn't start on the minute or half minute.
    //cstrMsg.LoadString(IDS_END_MINUTE_FORMAT_ERR);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_END_MINUTE_FORMAT_ERR);
    }
  if(bRet == true)
    { // Minutes are formatted correctly.
      // Convert the minutes to floating point values.
    fStartMinute = (float)atof(cstrStartMin);
    fEndMinute = (float)atof(cstrEndMin);
      // Check to make sure that the minutes actually fall within the period.
    if(fStartMinute < m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod]
    || fStartMinute > m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod] + m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod])
      { // Start minutes is out of range.
      //cstrMsg.LoadString(IDS_START_MINUTE_OUT_OF_RANGE);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_START_MINUTE_OUT_OF_RANGE);
      bRet = false;
      }
    else if(fEndMinute < m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod]
    || fEndMinute > m_pDoc->m_pdPatient.fPeriodStart[m_uPeriod] + m_pDoc->m_pdPatient.fPeriodLen[m_uPeriod])
      { // Start minutes is out of range.
      //cstrMsg.LoadString(IDS_END_MINUTE_OUT_OF_RANGE);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_END_MINUTE_OUT_OF_RANGE);
      bRet = false;
      }
    else if((fEndMinute - fStartMinute) < 4)
      { // Need 4 minutes.
      //cstrMsg.LoadString(IDS_NEED_4_MINUTES);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_NEED_4_MINUTES);
      bRet = false;
      }
    }
  if(bRet == false)
    { // Error, display the error message.
    //cstrTitle.LoadString(IDS_ERROR1);
    cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    MessageBox(cstrMsg, cstrTitle);
    *pfStartMin = (float)0.0;
    *pfEndMin = (float)0.0;
    }
  else
    {
    *pfStartMin = fStartMinute;
    *pfEndMin = fEndMinute;
    }
  return(bRet);
  }

/********************************************************************
vAdd_menu_items_for_period - Add a menu items for all new periods.
                             Rebuild the combobox with the new periods.

  inputs: None.

  return: None.
********************************************************************/
void CTenMinView::vAdd_menu_items_for_period()
  {
  unsigned short uID;
//  int iIndex;
  CString cstrBaseText, cstrText;

  //cstrBaseText.LoadString(IDS_POST_STIM_PERIOD);
  cstrBaseText = g_pLang->cstrLoad_string(ITEM_G_POST_STIM_PERIOD);
  uID = (m_pDoc->m_uNumPeriodMenuItems * 2) + MENU_ID_START;
  while(m_pDoc->m_uNumPeriodMenuItems < m_pDoc->m_pdPatient.uNumPeriods
  && m_pDoc->m_pdPatient.uNumPeriods >= 1)
    {
    cstrText.Format("%s %d", cstrBaseText, m_pDoc->m_uNumPeriodMenuItems + 1);
      // Add to the Analyze menu.
    m_pDoc->m_uAnalyzeMenuPostStimPeriodIDs[m_pDoc->m_uNumPeriodMenuItems] = uID;
    g_pFrame->vAdd_menu_item(ID_ANALYZE_BASELINE, uID, 0, cstrText);
    ++uID;
      // Add to the View menu.
    m_pDoc->m_uViewMenuPostStimPeriodIDs[m_pDoc->m_uNumPeriodMenuItems] = uID;
    g_pFrame->vAdd_menu_item(ID_VIEW_BASELINE, uID, 0, cstrText);
    ++uID;

    ++m_pDoc->m_uNumPeriodMenuItems;

    vUpdate_period_combobox(m_uPeriod);
//    iIndex = m_c10MinPeriodLst.AddString(cstrText);
//    m_c10MinPeriodLst.SetItemData(iIndex, (DWORD)m_pDoc->m_uNumPeriodMenuItems);
    }
  }

/********************************************************************
bAdd_new_period - Add a new period to the end of the period list.
                  Add the new period to the menus and combobox.

  inputs: None.

  return: true if a new period was added, otherwise false.
********************************************************************/
bool CTenMinView::bAdd_new_period()
  {
  bool bRet;
  float fEndMinute;
  short unsigned uPeriodIndex;

   // If there are more than 4 minutes left in the study, add a new period.
  fEndMinute = 
    (m_pDoc->m_pdPatient.fPeriodStart[m_pDoc->m_pdPatient.uNumPeriods]
    + m_pDoc->m_pdPatient.fPeriodLen[m_pDoc->m_pdPatient.uNumPeriods]);
  if((m_pDoc->m_fStudyLen - fEndMinute) > (float)4)
    {
    uPeriodIndex = m_pDoc->m_pdPatient.uNumPeriods + 1;
    ++m_pDoc->m_pdPatient.uNumPeriods;
    m_pDoc->m_pdPatient.fPeriodStart[uPeriodIndex] = 
      m_pDoc->m_pdPatient.fPeriodStart[uPeriodIndex - 1]
      + m_pDoc->m_pdPatient.fPeriodLen[uPeriodIndex - 1]; // + (float)0.5;
    m_pDoc->m_pdPatient.fPeriodLen[uPeriodIndex] = 
      fRound_down_to_half_min(m_pDoc->m_fStudyLen
                              - m_pDoc->m_pdPatient.fPeriodStart[uPeriodIndex]);
      // Set period not analyzed.
    m_pDoc->m_pdPatient.bPeriodsAnalyzed[uPeriodIndex] = false;
/**************************
      // Set the good minutes for the entire period.
    m_pDoc->m_pdPatient.fGoodMinStart[uPeriodIndex] = 
           m_pDoc->m_pdPatient.fPeriodStart[uPeriodIndex];
    m_pDoc->m_pdPatient.fGoodMinEnd[uPeriodIndex] = 
           m_pDoc->m_pdPatient.fPeriodStart[uPeriodIndex]
           + m_pDoc->m_pdPatient.fPeriodLen[uPeriodIndex];
****************************/
      // Add menu items for the new period.
    vAdd_menu_items_for_period();
    bRet = true;
    }
  else
    bRet = false;
  return(bRet);
  }

/********************************************************************
vInit_period_good_mins - Initialize the period and good minutes
                         checkboxes. Check/uncheck based on whether
                         the period has been analyzed.

  inputs: None.

  return: None.
********************************************************************/
void CTenMinView::vInit_period_good_mins()
  {

  if(m_pDoc->m_pdPatient.bPeriodsAnalyzed[m_pDoc->m_uCurrentPeriod] == true)
    { // Period has been analyzed.
    m_bSetPeriodLength = TRUE;
    m_bSetGoodMinutes = TRUE;
    }
  else
    { // Period has not been analyzed.
    m_bSetPeriodLength = FALSE;
    m_bSetGoodMinutes = FALSE;
    }
  if(m_pDoc->m_uCurrentPeriod == 0)
    m_bSetPeriodLength = TRUE; // Baseline is fixed period.
  UpdateData(FALSE);
  }

/********************************************************************
vBuild_standard_period_cbx - Build the select period combobox for a
                             standard study.

  inputs: None.

  return: None.
********************************************************************/
void CTenMinView::vBuild_standard_period_cbx()
  {
  CString cstrText;
  int iIndex;

  m_c10MinPeriodLst.ResetContent();
  //cstrText.LoadString(IDS_BASELINE);
  cstrText = g_pLang->cstrLoad_string(ITEM_G_BASELINE);
  iIndex = m_c10MinPeriodLst.AddString(cstrText);
  m_c10MinPeriodLst.SetItemData(iIndex, (DWORD)PERIOD_BL);
  //cstrText.LoadString(IDS_FIRST_10_MIN_PD);
  cstrText = g_pLang->cstrLoad_string(ITEM_G_FIRST_10_MIN_PD);
  iIndex = m_c10MinPeriodLst.AddString(cstrText);
  m_c10MinPeriodLst.SetItemData(iIndex, (DWORD)PERIOD_10);
  //cstrText.LoadString(IDS_SECOND_10_MIN_PD);
  cstrText = g_pLang->cstrLoad_string(ITEM_G_SECOND_10_MIN_PD);
  iIndex = m_c10MinPeriodLst.AddString(cstrText);
  m_c10MinPeriodLst.SetItemData(iIndex, (DWORD)PERIOD_20);
  //cstrText.LoadString(IDS_THIRD_10_MIN_PD);
  cstrText = g_pLang->cstrLoad_string(ITEM_G_THIRD_10_MIN_PD);
  iIndex = m_c10MinPeriodLst.AddString(cstrText);
  m_c10MinPeriodLst.SetItemData(iIndex, (DWORD)PERIOD_30);
  }

/********************************************************************
vShow_standard_fields - Show/hide all the fields used by a standard study.

  inputs: true to show the fields, false to hide the fields.

  return: None.
********************************************************************/
void CTenMinView::vShow_standard_fields(bool bShow)
  {
  int iShow;

  if(bShow == true)
    iShow = SW_SHOW;
  else
    iShow = SW_HIDE;

  GetDlgItem(IDC_SUGGESTED_MIN_FRAME)->ShowWindow(iShow);
  GetDlgItem(IDC_ARTIFACT_TITLE)->ShowWindow(iShow);
  GetDlgItem(IDC_ARTIFACT_LIST)->ShowWindow(iShow);
  GetDlgItem(IDC_GOOD_MIN_TITLE)->ShowWindow(iShow);
  GetDlgItem(IDC_GOOD_MIN)->ShowWindow(iShow);
  GetDlgItem(IDC_ACCEPT)->ShowWindow(iShow);
  GetDlgItem(IDC_REJECT)->ShowWindow(iShow);
  GetDlgItem(IDC_SELECT_NEW_MINUTES)->ShowWindow(iShow);
  GetDlgItem(IDC_START_MIN_TITLE)->ShowWindow(iShow);
  GetDlgItem(IDC_START_MIN)->ShowWindow(iShow);
  GetDlgItem(IDC_END_MIN_TITLE)->ShowWindow(iShow);
  GetDlgItem(IDC_END_MIN)->ShowWindow(iShow);
  GetDlgItem(IDC_START_MIN_SPIN)->ShowWindow(iShow);
  GetDlgItem(IDC_END_MIN_SPIN)->ShowWindow(iShow);
  GetDlgItem(IDC_UNDO_CHANGES)->ShowWindow(iShow);
  GetDlgItem(IDC_ACCEPT_CHANGES)->ShowWindow(iShow);
  }

/********************************************************************
vShow_research_fields - Show/hide all the fields used by a research study.

  inputs: true to show the fields, false to hide the fields.

  return: None.
********************************************************************/
void CTenMinView::vShow_research_fields(bool bShow)
  {
  int iShow;

  if(bShow == true)
    iShow = SW_SHOW;
  else
    iShow = SW_HIDE;
  m_bRsrchAnalyzedFieldsVisible = bShow;

  GetDlgItem(IDC_PERIOD_GOOD_MIN_FRAME)->ShowWindow(iShow);
  GetDlgItem(IDC_SELECT_GOOD_MINUTES)->ShowWindow(iShow);
  GetDlgItem(IDC_START_GOOD_MIN_TITLE)->ShowWindow(iShow);
  GetDlgItem(IDC_START_GOOD_MIN)->ShowWindow(iShow);
  GetDlgItem(IDC_START_GOOD_MIN_SPIN)->ShowWindow(iShow);
  GetDlgItem(IDC_END_GOOD_MIN_TITLE)->ShowWindow(iShow);
  GetDlgItem(IDC_END_GOOD_MIN)->ShowWindow(iShow);
  GetDlgItem(IDC_END_GOOD_MIN_SPIN)->ShowWindow(iShow);
  GetDlgItem(IDC_SELECT_PERIOD_LEN)->ShowWindow(iShow);
  GetDlgItem(IDC_START_PERIOD_MIN_TITLE)->ShowWindow(iShow);
  GetDlgItem(IDC_START_PERIOD_MIN)->ShowWindow(iShow);
  GetDlgItem(IDC_START_PERIOD_MIN_SPIN)->ShowWindow(iShow);
  GetDlgItem(IDC_END_PERIOD_MIN_TITLE)->ShowWindow(iShow);
  GetDlgItem(IDC_END_PERIOD_MIN)->ShowWindow(iShow);
  GetDlgItem(IDC_END_PERIOD_MIN_SPIN)->ShowWindow(iShow);
  GetDlgItem(IDC_CHK_SET_PERIOD_LENGTH)->ShowWindow(iShow);
  GetDlgItem(IDC_CHK_SET_GOOD_MINUTES)->ShowWindow(iShow);
  GetDlgItem(IDC_RESET_PERIODS)->ShowWindow(iShow);
//  GetDlgItem(IDC_INSERT_NEW_PERIOD)->ShowWindow(iShow);
  GetDlgItem(IDC_FRAME_PERIOD_EVENT)->ShowWindow(iShow);
  GetDlgItem(IDC_EVENT_LIST)->ShowWindow(iShow);
  GetDlgItem(IDC_EVENT_DESC_TITLE)->ShowWindow(iShow);
  GetDlgItem(IDC_EVENT_DESCRIPTION)->ShowWindow(iShow);
  GetDlgItem(IDC_BTN_EVENTS_EDIT)->ShowWindow(iShow);
  GetDlgItem(IDC_BTN_EVENTS_DELETE)->ShowWindow(iShow);
  if(bShow == false)
    GetDlgItem(IDC_SCROLLBAR_GRAPH)->ShowWindow(iShow);
  }

/********************************************************************
vEnable_research_period_fields - Enable/disable the fields allowing for
                                 the selection of the period.

  inputs: true to enable the fields, false to disable the fields.

  return: None.
********************************************************************/
void CTenMinView::vEnable_research_period_fields(bool bEnable)
  {

  // For Central Data Depot Reader fields are always disabled except for WhatIF.
#if EGGSAS_READER == READER_DEPOT
  if(m_pDoc->m_bUsingWhatIf == false)
    bEnable = false;
#endif

  GetDlgItem(IDC_SELECT_PERIOD_LEN)->EnableWindow(bEnable);
  GetDlgItem(IDC_START_PERIOD_MIN_TITLE)->EnableWindow(bEnable);
  GetDlgItem(IDC_START_PERIOD_MIN)->EnableWindow(bEnable);
  GetDlgItem(IDC_START_PERIOD_MIN_SPIN)->EnableWindow(bEnable);
  GetDlgItem(IDC_END_PERIOD_MIN_TITLE)->EnableWindow(bEnable);
  GetDlgItem(IDC_END_PERIOD_MIN)->EnableWindow(bEnable);
  GetDlgItem(IDC_END_PERIOD_MIN_SPIN)->EnableWindow(bEnable);
  GetDlgItem(IDC_CHK_SET_PERIOD_LENGTH)->EnableWindow(bEnable);
//  GetDlgItem(IDC_RESET_PERIODS)->EnableWindow(bEnable);
//  GetDlgItem(IDC_INSERT_NEW_PERIOD)->EnableWindow(bEnable);
  }

/********************************************************************
vEnable_research_goodmin_fields - Enable/disable the fields allowing for
                                  the selection of the good minutes.

  inputs: true to enable the fields, false to disable the fields.

  return: None.
********************************************************************/
void CTenMinView::vEnable_research_goodmin_fields(bool bEnable)
  {

  // For Central Data Depot Reader fields are always disabled except for WhatIF.
#if EGGSAS_READER == READER_DEPOT
  if(m_pDoc->m_bUsingWhatIf == false)
    bEnable = false;
#endif

  GetDlgItem(IDC_SELECT_GOOD_MINUTES)->EnableWindow(bEnable);
  GetDlgItem(IDC_START_GOOD_MIN_TITLE)->EnableWindow(bEnable);
  GetDlgItem(IDC_START_GOOD_MIN)->EnableWindow(bEnable);
  GetDlgItem(IDC_START_GOOD_MIN_SPIN)->EnableWindow(bEnable);
  GetDlgItem(IDC_END_GOOD_MIN_TITLE)->EnableWindow(bEnable);
  GetDlgItem(IDC_END_GOOD_MIN)->EnableWindow(bEnable);
  GetDlgItem(IDC_END_GOOD_MIN_SPIN)->EnableWindow(bEnable);
  GetDlgItem(IDC_CHK_SET_GOOD_MINUTES)->EnableWindow(bEnable);
  }

/********************************************************************
vEnable_research_buttons - Enable/disable the buttons that allow user
                           to change items.

  inputs: true to enable the buttons, false to disable the buttons.

  return: None.
********************************************************************/
void CTenMinView::vEnable_research_buttons(bool bEnable)
  {

  // For The Central Data Depot reader, the Edit and Delete
  // events buttons are always disabled as they apply to a study
  // and not a What If.
#if EGGSAS_READER == READER_DEPOT
  GetDlgItem(IDC_BTN_EVENTS_EDIT)->EnableWindow(FALSE);
  GetDlgItem(IDC_BTN_EVENTS_DELETE)->EnableWindow(FALSE);
#else
  GetDlgItem(IDC_BTN_EVENTS_EDIT)->EnableWindow(bEnable);
  GetDlgItem(IDC_BTN_EVENTS_DELETE)->EnableWindow(bEnable);
#endif
  GetDlgItem(IDC_RESET_PERIODS)->EnableWindow(bEnable);
  }

/********************************************************************
vFill_events_fields - Fill all the event fields with the events for
                      current period.  Select the first line in the
                      Events listbox.

                      The data item for each listbox line is the index
                      of the event into the list of events.

  inputs: Pointer to the PATIENT_DATA structure for the current patient.

  return: None.
********************************************************************/
void CTenMinView::vFill_events_fields(PATIENT_DATA *pdInfo)
  {
  CString cstrText;
  CEvnt *pEvent;
  int iMinutes, iStartSecond, iEndSecond, iIndex, iLBIndex;

  m_cEventsList.ResetContent();
  m_cstrEventDescription.Empty();
  if(m_uPeriod == 0)
    { // Baseline.
    if(pdInfo->Events.m_BLEvents.GetCount() > 0)
      {
      for(iIndex = 0; iIndex < pdInfo->Events.m_BLEvents.GetCount(); ++iIndex)
        {
        pEvent = pdInfo->Events.m_BLEvents[iIndex];
        iMinutes = pEvent->m_iSeconds / 60;
        cstrText.Format("%d:%02d - %s", iMinutes, pEvent->m_iSeconds - (iMinutes * 60),
                        pEvent->m_cstrIndications);
        iLBIndex = m_cEventsList.AddString(cstrText);
        m_cEventsList.SetItemData(iLBIndex, iIndex);
        }
      pEvent = pdInfo->Events.m_BLEvents[0];
      m_cstrEventDescription = pEvent->m_cstrDescription;
      }
    m_cEventsList.SetCurSel(0);
    }
  else
    { // Post-stimulation period.
    if(pdInfo->Events.m_PSEvents.GetCount() > 0)
      {
      iMinutes = -1;
      iStartSecond = (int)(pdInfo->fPeriodStart[m_uPeriod] * (float)60.0);
      iEndSecond = iStartSecond + (int)(pdInfo->fPeriodLen[m_uPeriod] * (float)60.0);
      for(iIndex = 0; iIndex < pdInfo->Events.m_PSEvents.GetCount(); ++iIndex)
        {
        pEvent = pdInfo->Events.m_PSEvents[iIndex];
        if(pEvent->m_iSeconds >= iStartSecond && pEvent->m_iSeconds <= iEndSecond)
          {
          if(iMinutes < 0)
            m_cstrEventDescription = pEvent->m_cstrDescription;
          iMinutes = pEvent->m_iSeconds / 60;
          cstrText.Format("%d:%02d - %s", iMinutes, pEvent->m_iSeconds - (iMinutes * 60),
                          pEvent->m_cstrIndications);
          iLBIndex = m_cEventsList.AddString(cstrText);
          m_cEventsList.SetItemData(iLBIndex, iIndex);
          }
        }
      }
    m_cEventsList.SetCurSel(0);
    }
  }

/********************************************************************
vEdit_event_from_lstbox - Edit an event.
                          Start the EventRecorder dialog box.
                            Allow changes to the indications and the
                            description.  Don't allow changes to the
                            time (data point).

  inputs: None.

  return: None.
********************************************************************/
void CTenMinView::vEdit_event_from_lstbox()
  {
  CEventRecorder *pdlg;
  CEvnt *pEvent;
  int iSel, iEventIndex;

  UpdateData(TRUE);
  iSel = m_cEventsList.GetCurSel();
  iEventIndex = (int)m_cEventsList.GetItemData(iSel);
  if(m_uPeriod == 0)
    { // Baseline.
    pEvent = m_pDoc->m_pdPatient.Events.m_BLEvents[iEventIndex];
    }
  else
    { // Post-stimulation period
    pEvent = m_pDoc->m_pdPatient.Events.m_PSEvents[iEventIndex];
    }
  m_cstrEventDescription = pEvent->m_cstrDescription;

    // Display the dialog box and allow changes.
  pdlg = new CEventRecorder(pEvent->m_iSeconds);
  pdlg->vSet_data(pEvent->m_cstrIndications, pEvent->m_cstrDescription);
  if(pdlg->DoModal() == IDOK)
    {
    pEvent->m_cstrDescription = pdlg->cstrGet_description();
    pEvent->m_cstrIndications = pdlg->cstrGet_indications();
      // Rebuild the listbox to reflect any changes.
    vFill_events_fields(&m_pDoc->m_pdPatient);
    m_cEventsList.SetCurSel(iSel);
    m_cstrEventDescription = pEvent->m_cstrDescription;
    }
  delete pdlg;
  UpdateData(FALSE);
  }

/********************************************************************
vShow_study_type - Display text indicating the study type.

  inputs: None.

  return: None.
********************************************************************/
void CTenMinView::vShow_study_type()
  {
  CString cstrFreq;

#ifdef _200_CPM_HARDWARE
  if(m_pDoc->m_pdPatient.uFrequency != 0)
    cstrFreq.Format("%d %s", m_pDoc->m_pdPatient.uFrequency, g_pLang->cstrLoad_string(ITEM_G_CPM));
  else
    cstrFreq = "";
  GetDlgItem(IDC_STUDY_TYPE)->ShowWindow(SW_SHOW);
  if(theApp.m_ptPgmType == PT_STD_RSCH)
    {
    GetDlgItem(IDC_STUDY_TYPE)->ShowWindow(SW_SHOW);
    if(theApp.m_ptStudyType == PT_STANDARD)
      m_cstrStudyType = g_pLang->cstrLoad_string(ITEM_G_STANDARD); // + ", " + cstrFreq;
    else
      m_cstrStudyType = g_pLang->cstrLoad_string(ITEM_G_RESEARCH); // + ", " + cstrFreq;
    if(cstrFreq != "")
      m_cstrStudyType += (", " + cstrFreq);
    }
  else
    m_cstrStudyType = cstrFreq;
#else
  // Not using 60/200 cpm hardware.
  if(theApp.m_ptPgmType == PT_STD_RSCH)
    {
    GetDlgItem(IDC_STUDY_TYPE)->ShowWindow(SW_SHOW);
    if(theApp.m_ptStudyType == PT_STANDARD)
      m_cstrStudyType.LoadString(IDS_STANDARD);
    else
      m_cstrStudyType.LoadString(IDS_RESEARCH);
    }
  else
    m_cstrStudyType = cstrFreq;
#endif
//    // Set Research or standard study.
//  if(theApp.m_ptPgmType == PT_STD_RSCH)
//    {
//    GetDlgItem(IDC_STUDY_TYPE)->ShowWindow(SW_SHOW);
//    if(theApp.m_ptStudyType == PT_STANDARD)
//      m_cstrStudyType = g_pLang->cstrLoad_string(ITEM_G_STANDARD) + ", " + cstrFreq;
//      //m_cstrStudyType.LoadString(IDS_STANDARD);
//    else
//      m_cstrStudyType = g_pLang->cstrLoad_string(ITEM_G_RESEARCH) + ", " + cstrFreq;
//      //m_cstrStudyType.LoadString(IDS_RESEARCH);
//    }
//  else
//    {
//#ifdef _200_CPM_HARDWARE
//    m_cstrStudyType = cstrFreq;
//#else
//    GetDlgItem(IDC_STUDY_TYPE)->ShowWindow(SW_HIDE);
//#endif
//    }
  UpdateData(FALSE);
  }

/********************************************************************
vShow_WhatIf - Display the date/time and the description of the
               currently loaded What If Scenario.

  inputs: true to show the fields, false to hide the fields.
          The description to display.

  return: None.
********************************************************************/
void CTenMinView::vShow_WhatIf(bool bShow, CString cstrDesc)
  {

  if(bShow == true)
    {
    GetDlgItem(IDC_STATIC_WHAT_IF)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_EDIT_WHAT_IF)->ShowWindow(SW_SHOW);
    m_cstrWhatIfDesc = cstrDesc;
#if EGGSAS_READER == READER_DEPOT
    // All the fields to change the study have been disabled.
    // Enable them now.
//    vEnable_research_period_fields(true);
    vEnable_research_goodmin_fields(true);
    GetDlgItem(IDC_RESET_PERIODS)->EnableWindow(TRUE);
#endif
    }
  else
    {
    GetDlgItem(IDC_STATIC_WHAT_IF)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDIT_WHAT_IF)->ShowWindow(SW_HIDE);
    m_cstrWhatIfDesc = "";
#if EGGSAS_READER == READER_DEPOT
    // All the fields to change the study have been enabled.
    // Disable them now.
    vEnable_research_period_fields(false);
    vEnable_research_goodmin_fields(false);
    GetDlgItem(IDC_RESET_PERIODS)->EnableWindow(FALSE);
#endif
    }
  UpdateData(FALSE);
  }


