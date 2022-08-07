/********************************************************************
ReportView.cpp

Copyright (C) 2003 - 2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CReportView class.

  This class implements the view that contains the report.


HISTORY:
08-NOV-03  RET  New.
20-FEB-04  RET  Change iAdd_facility() to get the text extent (for updating
                the Y coordinate) when printing the facility name (rather than
                after printing the first address line - this caused a program
                crash).
                Change routines that draw graphs to delete the windows the graphs
                  are drawn in if the starting Y coordinate changes.  This
                  allows the graphs to be drawn in the correct place.
                    Constructor, iCreate_summary_graphs(),
                    iCreate_RSA(), iCreate_sel_raw_data_graphs()
08-MAR-04  RET  Change vSet_graph_summ_characteristics():
                  - Change X axis label to "period" and show "BL,10,20,30"
                    axis labels with no in between hash marks.
                  - Add setting a larger font size to the title and the
                    X axis label.
                Fix bug in iCreate_summary_graphs() that didn't redraw graphs
                  for printing.
05-FEB-05  RET  Modified to allow for displaying/printing the following
                parts of the report individually:
                  RSA, Data Sheet, Summary Graphs, Raw Signal of Selected Minutes,
                  Entire Raw Signal, Diagnosis/Recommendations 
                Modified to print the name/number and date of study at the
                  head of each page.
                Modified the following methods:
                  OnDraw(), iCreate_all_raw_data_graphs(), vReset_view(),
                  iCreate_summary_graphs(), iCreate_RSA()
                Added methods:
                  iAdd_header()
06-FEB-05  RET  Changes to add viewing the summary graphs and the data sheet
                  Modified methods:
                    OnDraw(), iCreate_summary_graphs()
                Changes to use the research reference number.
                  Modified methods:
                    iAdd_header(), iAdd_left_side()
10-FEB-05  RET  Change iAdd_left_side() to add Delayed gastric emptying.
05-MAY-07  RET  Changed vDraw_summ_graph() to limit the number of periods
                  to 3 plus the baseline.
10-MAY-07  RET  Research Version
                  Change OnDraw() to use Patient Info from the INI file
                    to determine number of pages in report and print
                    accordingly.
                  Change OnInitialUpdate(), vAnalyze_data(),
                    iCreate_data_sheet(), iCreate_sel_raw_data_graphs(),
                    vAnalyze_data_for_RSA(), iCreate_all_raw_data_graphs()
                    to use different number of max periods based
                    on research/standard version.
                  Add prototype for cstrGet_timeband_start_end_min()
26-JUN-08  RET  Version 2.05
                  Add amount and units of stimulation medium to the first
                    page of the report.
                      Change iAdd_left_side().
                  Add stimulation medium type, amount, units to end of data sheet.
                    Change iCreate_data_sheet().
29-JUL-08  RET  Version 2.05b
                  Change OnDraw() to check the added end of page flag to determine
                    if need to add an end of page for the raw data and the
                    events.
                  Change iCreate_all_raw_data_graphs() to add end of pages
                    differently and return the proper Y coordinate.
                  Change iEnd_of_page() to set the end of page flag.
17-OCT-08  RET  Version 2.07a
                  Create column title arrays for dogs: s_uColTitlesDog[],
                    s_uDSColTitlesDog1[].
                  Add resource strings: 
                    IDS_0_300, IDS_325_600, IDS_625_1500, IDS_0_15
                  Change iCreate_data_sheet() to display the correct number
                    of column titles and column data for dogs.
                  Add method bSkip_datasheet_column() to determine datasheet
                    columns to skip.
16-FEB-09  RET  Version 2.08j
                  Change cstrGet_timeband_start_end_min() to properly calculate
                    minutes for the data sheet report for a waterload study.
                    This problem was found if an extra 10 minutes of baseline
                    is recorded.
28-SEP-11  RET
             Changes to write an ASCII raw data file and not allow any analysis.
               Disable the report items that relate to analysis.
                 Change method: OnDraw() 
21-FEB-12  RET  Version 
                  Changes for foreign languages.
24-AUG-12  RET
             Change method iAdd_facility() to print only the items that
               were checked in the facility information dialog box.
             Change method iAdd_left_side():
               - Print only the items that were checked in the demographics dialog box.
               - Print waterload and units for a waterload study.
             Add a title to the report after the facility information.
               Add method: iAdd_title()
25-AUG-12  RET
             Add back in Summary Table to always be printed at the bottom
             of the Summary Graphs.
               Change methods: iCreate_summary_graphs()
               Uncomment method: iCreate_summary_table()
27-AUG-12  RET
             Change report configuration so that RSA and Summary Graphs can
               either be printed on the first page or a subsequent page.
                 Change methods: OnDraw()
13-SEP-12  RET
             Changes to indications.
               Change methods: iAdd_left_side()
18-SEP-12  RET
             Change displaying/printing core report to include/exclude the
               selected raw signal, all the raw signal and the data sheet.
                 Change methods: OnDraw()
             Change the left side of the first page of the report to:
               - Put patient name and address together.
               - Instead of title and value on 2 lines, put on one line.
                  Change method iAdd_left_side()
             Change to print/display attending physician, refering physician,
               and technician only if checked to be in the report.
                  Change method iAdd_left_side()
25-SEP-12  RET
             Change report to leave out large blank spaces above the summary gaphs
             and between the summary graphs and the RSA.
               Change method: OnDraw()
             Change to properly print summary table when included on
             other than first page of report.
               Change methods: iCreate_summary_table()
29-SEP-12  RET
             Change for 60/200 CPM
               Change methods: OnDraw(), iCreate_data_sheet()
13-DEC-12  RET
             Change to allow summary graphs to be drawn for research version.
               Change methods: OnDraw()
14-DEC-12  RET
             Change to filter out frequencies below 10 cpm in the RSA.
             Change to allow both the normal RSA and the filtered RSA
             to be printed/viewed.
               Add class variables: m_iRSAGraphStart1, m_pcwndRSA1
               Change methods: OnDraw(), iCreate_RSA()
01-MAR-13  RET
             Add male/female designation to the left side of page 1 of report.
               Change method: iAdd_left_side()
02-JUN-16  RET
             Add feature to hide study dates.
               Change method: iAdd_left_side(), iAdd_header()
16-NOV-20  RET
             Add new version CDDRE
               Change methods: iAdd_left_side(), iAdd_header()
********************************************************************/

#include "stdafx.h"
#include "atlimage.h"
#include "EGGSAS.h"
#include "MainFrm.h"
#include "EGGSASDoc.h"
#include "Dbaccess.h"
#include "analyze.h"
#include "GraphRSA.h"
#include "ReportView.h"
#include "FacilityInfo.h"
#include "EventList.h"
#include "util.h"

#include "Pegrpapi.h"
#include "Pemessag.h"
#include ".\reportview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame *g_pFrame;
extern float g_fControl[CONTROL_MIN_HI_LO][CONTROL_FREQ_BANDS];

// Array to hold the resource IDs for the frequency range column titles.
static unsigned s_uColTitles[] = 
  {IDS_1_25CPM, IDS_25_375CPM, IDS_375_100CPM, IDS_100_150CPM, IDS_1_15CPM, IDS_MINUTES};

// Array to hold the resource IDs for the diagnosis column titles.
static unsigned s_uDSColTitles1[] = 
  {
  IDS_BRADY, IDS_NORMAL, IDS_TACHY, IDS_DUOD, IDS_ENTIRE_RANGE, IDS_PERIOD
  };

// Array to hold the resource IDs for the frequency range column titles for dogs.
static unsigned s_uColTitlesDog[] = 
  {IDS_0_300, IDS_325_600, IDS_625_1500, IDS_0_15, IDS_MINUTES};

// Array to hold the resource IDs for the diagnosis column titles for dogs.
static unsigned s_uDSColTitlesDog1[] = 
  {
  IDS_BRADY, IDS_NORMAL, IDS_TACHY, IDS_ENTIRE_RANGE, IDS_PERIOD
  };

// Array to hold the resource IDs for the graphs.
static unsigned s_uGraphTitles[] =
  {
  IDS_SUMM_BRADY_TITLE,
  IDS_SUMM_NORMAL_TITLE,
  IDS_SUMM_TACHY_TITLE,
  IDS_SUMM_DUOD_TITLE,
  };

/////////////////////////////////////////////////////////////////////////////
// CReportView

IMPLEMENT_DYNCREATE(CReportView, CScrollView)

/********************************************************************
Constructor

  Initialize class data.
********************************************************************/
CReportView::CReportView()
  {
  short int iIndex;

  for(iIndex = 0; iIndex < CONTROL_FREQ_BANDS; ++iIndex)
    m_hPESum[iIndex] = NULL;
  for(iIndex = 0; iIndex < MAX_TOTAL_PERIODS; ++iIndex)
    m_hPESelRawData[iIndex] = NULL;

  m_pDC = NULL;
  m_pcwndRSA = NULL;
  m_pcwndRSA1 = NULL;
  m_rsaData.pfData = NULL;
  m_phPERawData = NULL;
  m_uNumRawDataGraphs = 0;
  m_iRawDataGraphStartY = -1;
  m_iRawDataGraphWidth = 0;
  m_iSummaryGraphStart = -1;
  m_iRSAGraphStart = -1;
  m_iRSAGraphStart1 = -1;
  m_iSelRawDataGraphStartY = -1;

  m_csViewSize.cx = 0;
  m_csViewSize.cy = 0;
  SetScrollSizes(MM_TEXT, m_csViewSize);
  }

/********************************************************************
Destructor

  Clean up class data.
    - Free all allocated memory used for the data analyses.
    - delete all graphs.
********************************************************************/
CReportView::~CReportView()
  {
  unsigned short uIndex;

    // Make sure the document knows we are no longer is report view.
  m_pDoc->m_bReportView = false;

    // Free the allocated memory.
  if(m_rsaData.pfData != NULL)
    {
    HeapFree(GetProcessHeap(), 0, (LPVOID)m_rsaData.pfData);
    m_rsaData.pfData = NULL;
    }

  vFree_data_sheet_arrays();

  for(uIndex = 0; uIndex < CONTROL_FREQ_BANDS; ++uIndex)
    {
    if(m_hPESum[uIndex] != NULL)
      {
      PEdestroy(m_hPESum[uIndex]);
      m_hPESum[uIndex] = NULL;
      }
    }

  for(uIndex = 0; uIndex < MAX_TOTAL_PERIODS; ++uIndex)
    {
    if(m_hPESelRawData[uIndex] != NULL)
      {
      PEdestroy(m_hPESelRawData[uIndex]);
      m_hPESelRawData[uIndex] = NULL;
      }
    }

  if(m_pcwndRSA != NULL)
    {
    delete m_pcwndRSA;
    m_pcwndRSA = NULL;
    }
  if(m_pcwndRSA1 != NULL)
    {
    delete m_pcwndRSA1;
    m_pcwndRSA1 = NULL;
    }

    // Free the raw data graphs.
  if(m_phPERawData != NULL)
    {
    for(uIndex = 0; uIndex < m_uNumRawDataGraphs; ++uIndex)
      {
      if(*(m_phPERawData + uIndex) != NULL)
        {
        PEdestroy(*(m_phPERawData + uIndex));
        *(m_phPERawData + uIndex) = NULL;
        }
      }
    GlobalFree(m_phPERawData);
    m_phPERawData = NULL;
    }
  }


BEGIN_MESSAGE_MAP(CReportView, CScrollView)
	//{{AFX_MSG_MAP(CReportView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/********************************************************************
OnInitialUpdate

  Perform initialization of the view.
********************************************************************/
void CReportView::OnInitialUpdate() 
  {
  RECT rClient;
  unsigned uIndex;

  GetClientRect(&rClient); // Not printing
  m_csViewSize.cx = rClient.right;
  m_csViewSize.cy = rClient.bottom;
  ResyncScrollSizes();
	CScrollView::OnInitialUpdate();

//  ((CEGGSASApp *)AfxGetApp())->vLog("CReportView::OnInitialUpdate enter");
	
	// TODO: Add your specialized code here and/or call the base class
	m_pDoc = (CEGGSASDoc *)GetDocument();
    // Tell document we are in report view.
  m_pDoc->m_bReportView = true;
    // Make sure all pointers to allocated data are reset.
  m_adData.pfRawData = NULL;
  m_adData.pfPwrSum = NULL;
  m_adData.pfDomFreq = NULL;
  m_adData.pfRatioAvgPwr = NULL;
  m_adData.pfPctDomFreq = NULL;

    // Fill is some default data used for the RSA.
  m_rsaData.uType = RSA_TYPE_REPORT;
  m_rsaData.crPreWaterColor = PRE_WATER_COLOR;
  m_rsaData.crPostWaterColor = POST_WATER_COLOR;
  m_rsaData.cstrPatientName.Empty();
  m_rsaData.cstrStudyDate.Empty();

  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard version
    m_uMaxPeriods = m_pDoc->m_uMaxPeriods;
    }
  else
    { // Research version
      // Number of post_stimulation periods plus 1 baseline period.
    m_uMaxPeriods = m_pDoc->m_pdPatient.uNumPeriods + 1;
    }

  m_uNumRawDataGraphs = 0;
  m_phPERawData = NULL;
  for(uIndex = 0; uIndex < CONTROL_FREQ_BANDS; ++uIndex)
    m_hPESum[uIndex] = NULL;
  for(uIndex = 0; uIndex < MAX_TOTAL_PERIODS; ++uIndex)
    m_hPESelRawData[uIndex] = NULL;
  m_pcwndRSA = NULL;
  m_pcwndRSA1 = NULL;

    // Analyze the data for the entire report.
  vAnalyze_data(DATA_SHEET_TYPE);
  vAnalyze_data_for_RSA();
  }

/////////////////////////////////////////////////////////////////////////////
// CReportView drawing

/********************************************************************
OnDraw - Message handler to display the data on the device which may be
         either the screen or a printer.

  inputs: Pointer to the device context.

  return: None.
********************************************************************/
void CReportView::OnDraw(CDC* pDC)
  {
  RECT rClient;
  TEXTMETRIC tm;
  int iY, iX, iXStart, iYRightStart, iYPixelsPerInch, iYPageEnd;
  int iLeftSideRightMargin, iXGrphStart, iSummGrphRSAYSeparation;
  short int iPageNum, iTemp;
  unsigned short uIndex;
  bool bSummGrphDisplayed, bHavePage;

	m_pDoc = (CEGGSASDoc *)GetDocument();
  m_pDC = pDC;
//  ((CEGGSASApp *)AfxGetApp())->vLog("CReportView::OnDraw enter");
  
  // TODO: add draw code here
  GetClientRect(&rClient);
  m_csViewSize.cx = rClient.right;

  if(m_pDoc->m_uReportItemToView == RPT_VIEW_ITEM_NONE)
    {
    m_iIncludePatientInfo = 0;
    m_iIncludeEventLog = 0;
      // Determine what to include in the report.
    m_iIncludeCoreReport = g_pConfig->iGet_item(CFG_COREREPORT);
    if(m_iIncludeCoreReport == 1) // && theApp.m_ptStudyType == PT_STANDARD)
      { // Printing/displaying core report, ignore the INI file settings.
        // This must be a standard version.
       // Number of pages for the core report. The second page is the selected raw minutes.
      m_iNumPages = 2; // Number of pages for the core report.
#ifdef _200_CPM_HARDWARE
      // If 200 CPM is selected, have to add another page because RSA won't go
      // on page 1 of report.
      if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
        ++m_iNumPages;
#endif
      m_iIncludeDataSheet = g_pConfig->iGet_item(CFG_DATASHEET);
      if(m_iIncludeDataSheet != 0)
        {
        ++m_iNumPages;
        if(m_pDC->IsPrinting() != 0)
          { // Printing.
          if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM
            || theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
            {
            ++m_iNumPages;
            if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
              ++m_iNumPages;
            }
          }
        }
      m_iIncludeRawSigSelMin = g_pConfig->iGet_item(CFG_RAWSIGSELMIN);
      if(m_iIncludeRawSigSelMin == 0)
        --m_iNumPages; // Use deselected the raw minutes.
      m_iIncludeEntireRawSignal = g_pConfig->iGet_item(CFG_ENTIRERAWSIGNAL);
      //if(m_iIncludeEntireRawSignal != 0)
      //  ++m_iNumPages;
      m_iIncludeDiagRecom = 1;
      m_iIncludeRSA = 1;
      if(theApp.m_ptStudyType == PT_STANDARD)
        m_iIncludeSummGraphs = 1;
      else
        {
        if(m_pDoc->bCan_graph_summary_graphs() == false)
          m_iIncludeSummGraphs = 0;
        else
          m_iIncludeSummGraphs = 1;
        }
      }
    else
      { // Report has been customized. // or this is a research version.
        // Use the INI file to determine what gets displayed/printed.
      if(g_bAsciiDataFile == false)
        {
        m_iIncludeDataSheet = g_pConfig->iGet_item(CFG_DATASHEET);
        m_iIncludeEntireRawSignal = g_pConfig->iGet_item(CFG_ENTIRERAWSIGNAL);
        m_iIncludeDiagRecom = g_pConfig->iGet_item(CFG_DIAGRECOMM);
        m_iIncludeRSA = g_pConfig->iGet_item(CFG_RSA);
        if(theApp.m_ptStudyType == PT_STANDARD)
          m_iIncludeSummGraphs = g_pConfig->iGet_item(CFG_SUMMGRAPHS);
        else
          {
          if(m_pDoc->bCan_graph_summary_graphs() == false)
            m_iIncludeSummGraphs = 0;
          else
            m_iIncludeSummGraphs = g_pConfig->iGet_item(CFG_SUMMGRAPHS);
          }
        m_iIncludeRawSigSelMin = g_pConfig->iGet_item(CFG_RAWSIGSELMIN);
        }
      else
        {
        m_iIncludeDataSheet = 0;
        m_iIncludeDiagRecom = 0;
        m_iIncludeRSA = 0;
        m_iIncludeSummGraphs = 0;
        m_iIncludeRawSigSelMin = 0;
        m_iIncludeEntireRawSignal = g_pConfig->iGet_item(CFG_ENTIRERAWSIGNAL);
       }
        // Calculate the number of pages needed.
      m_iNumPages = 0;
      
      m_iIncludePatientInfo = g_pConfig->iGet_item(CFG_PATIENTINFO);
      if(m_iIncludePatientInfo != 0 || m_iIncludeDiagRecom != 0
        || (m_iIncludeSummGraphs != 0 && g_pConfig->iGet_item(CFG_SUMM_GRAPHS_FIRST_PAGE) == 1)
        || (m_iIncludeRSA != 0 && g_pConfig->iGet_item(CFG_RSA_FIRST_PAGE) == 1))
        ++m_iNumPages;
      if(theApp.m_ptStudyType == PT_RESEARCH)
        {
        if(g_bAsciiDataFile == false)
          {
          //m_iIncludeEventLog = AfxGetApp()->GetProfileInt(INI_SEC_REPORT, INI_ITEM_EVENT_LOG, 0);
          m_iIncludeEventLog = g_pConfig->iGet_item(CFG_EVENTLOG);
          if(m_iIncludeEventLog != 0)
            ++m_iNumPages;
          }
        else
          m_iIncludeEventLog = 0;
        }
      if(m_iIncludeRSA != 0)
        {
        if(g_pConfig->iGet_item(CFG_RSA_OTHER_PAGE) == 1)
          ++m_iNumPages;
        else if(g_pConfig->iGet_item(CFG_BOTH_RSA) == 1)
          m_iNumPages += 2;
        }
      if(m_iIncludeSummGraphs != 0)
        {
        if(g_pConfig->iGet_item(CFG_SUMM_GRAPHS_OTHER_PAGE) == 1)
          ++m_iNumPages;
        }
      if(m_iIncludeRawSigSelMin != 0)
        {
        if(theApp.m_ptStudyType == PT_STANDARD)
          ++m_iNumPages;
        else
          {
          m_iNumPages += m_uMaxPeriods / NUM_RAW_DATA_GRAPHS_PER_PAGE; // 3 graphs per page.
          if((m_uMaxPeriods % NUM_RAW_DATA_GRAPHS_PER_PAGE) != 0)
            ++m_iNumPages;
          }
        }
      if(m_iIncludeDataSheet != 0)
        {
        ++m_iNumPages;
        if(m_pDC->IsPrinting() != 0)
          {
          if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM
            || theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
            {
            ++m_iNumPages;
            if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
              ++m_iNumPages;
            }
          }
        }
      }
    if(m_iIncludeEntireRawSignal != 0)
      {
      if((iTemp = iAlloc_raw_data_graphs()) > 0)
        m_iNumPages += iTemp;
      }
    else //if(m_iIncludeEntireRawSignal == 0)
      { // Free the raw data graphs.
      if(m_phPERawData != NULL)
        {
        for(uIndex = 0; uIndex < m_uNumRawDataGraphs; ++uIndex)
          {
          if(*(m_phPERawData + uIndex) != NULL)
            {
            PEdestroy(*(m_phPERawData + uIndex));
            *(m_phPERawData + uIndex) = NULL;
            }
          }
        if(m_uNumRawDataGraphs > 0)
          GlobalFree(m_phPERawData);
        m_phPERawData = NULL;
        m_uNumRawDataGraphs = 0;
        }
      }
    }
  else
    { // Viewing items from the View menu or icons.
    m_iIncludeCoreReport = 0;
    m_iIncludeEntireRawSignal = 0;
    m_iIncludeDiagRecom = 0;
    m_iIncludeRSA = 0;
    m_iIncludeRawSigSelMin = 0;
    m_iIncludePatientInfo = 0;
    m_iIncludeEventLog = 0;
    m_iNumPages = 0;
    if(g_bAsciiDataFile == false)
      {
      if(m_pDoc->m_uReportItemToView == RPT_VIEW_ITEM_SUMM_GRPHS)
        { // Viewing only summary graphs.
        m_iIncludeSummGraphs = 1;
        m_iIncludeDataSheet = 0;
        }
      else
        { // Viewing only data sheet.
        m_iIncludeSummGraphs = 0;
        m_iIncludeDataSheet = 1;
        }
      }
    else
      {
        m_iIncludeSummGraphs = 0;
        m_iIncludeDataSheet = 0;
      }
    }

    // Check to see if any of the graphs have to be cleared from the screen.
  if(m_iIncludeSummGraphs == 0)
    {
    for(uIndex = 0; uIndex < CONTROL_FREQ_BANDS; ++uIndex)
      {
      if(m_hPESum[uIndex] != NULL)
        {
        PEdestroy(m_hPESum[uIndex]);
        m_hPESum[uIndex] = NULL;
        }
      }
    }
  if(m_iIncludeRawSigSelMin == 0)
    {
    for(uIndex = 0; uIndex < MAX_TOTAL_PERIODS; ++uIndex)
      {
      if(m_hPESelRawData[uIndex] != NULL)
        {
        PEdestroy(m_hPESelRawData[uIndex]);
        m_hPESelRawData[uIndex] = NULL;
        }
      }
    }
  if(m_iIncludeRSA == 0)
    {
    if(m_pcwndRSA != NULL)
      {
      delete m_pcwndRSA;
      m_pcwndRSA = NULL;
      }
    if(m_pcwndRSA1 != NULL)
      {
      delete m_pcwndRSA1;
      m_pcwndRSA1 = NULL;
      }
    }

  iYPixelsPerInch = m_pDC->GetDeviceCaps(LOGPIXELSY);
  m_iLineExt = m_pDC->GetDeviceCaps(HORZRES);
  m_iLeftWidth = m_iLineExt / 2; // Use 50% of width.
  iLeftSideRightMargin = (int)((float)m_iLeftWidth * .05F);
  m_iLeftSideUseableWidth = m_iLeftWidth - iLeftSideRightMargin;
  if(m_pDC->IsPrinting() != 0)
    { // Printing.
    // make a slightly narrower useable left side.
    m_iLeftSideUseableWidth -= iLeftSideRightMargin;
    // Leave more of a margin on the left side.
      // Use a smaller font.
    m_pDC->GetTextMetrics(&tm);
    iXStart = 2 * tm.tmAveCharWidth;
    iY = iYPixelsPerInch / 2; // Printing. Leave about 1/2 inch at top.
    iSummGrphRSAYSeparation = 50;
    }
  else
    { // Not printing.
    iXStart = 10;
    iY = 10;
    iSummGrphRSAYSeparation = 25;
    }
  iX = iXStart;
    // Report items.
  if(m_pDoc->m_uReportItemToView == RPT_VIEW_ITEM_NONE)
    { // Only print/display facility if working from report menu.
    iPageNum = 1;
      // Add facility/practice info as header.
    iY = iAdd_facility(iX, iY);
    // Add title
    iY = iAdd_title(iX, iY);
    iYRightStart = iY;
    //if(m_pDC->IsPrinting() != 0)
    //  { // Printing.  Leave some extra room after the facility.
    //  iY += iYPixelsPerInch;
    //  }
    }
    // Add left side of report.
  if(m_iIncludeCoreReport != 0) // && theApp.m_ptStudyType == PT_STANDARD)
    { // Core report
    m_iRightWidth = m_iLineExt - m_iLeftWidth;
      // Where the graphs on the right side of page 1 start.
    iXGrphStart = m_iLeftWidth;
    iYPageEnd = iAdd_left_side(iX, iY);
    iY = iYRightStart; // Now add the first page graphs to the right side.
    //if(m_pDC->IsPrinting() == 0)
      { // Not printing
      if(theApp.m_ptStudyType == PT_STANDARD)
        iY = iCreate_summary_graphs(iXGrphStart, iY);
      else
        {
        if(m_pDoc->bCan_graph_summary_graphs() == true)
          iY = iCreate_summary_graphs(iXGrphStart, iY);
        }

#ifdef _200_CPM_HARDWARE
      // If 200 CPM is selected, have to add another page because RSA won't go
      // on page 1 of report.
      if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
        {
        iY = iEnd_of_page(iXStart, iYPageEnd, iPageNum); // End of page processing.
        ++iPageNum; // Next page.
        iXGrphStart = m_iLineExt / 10;
          // width of graph is 80% of screen width.
        m_iRightWidth = m_iLineExt - (iXGrphStart * 2);
          // Display/print the header information.
        iY = iAdd_header(iX, iY);
        if(m_pDC->IsPrinting() == 0)
          iY = iCreate_RSA(iXGrphStart, iY, 1); // + 50); // Not printing
        else
          iY = iCreate_RSA(iXGrphStart, iY + iYPixelsPerInch, 1);
        }
      else
        iY = iCreate_RSA(iXGrphStart, iY + iSummGrphRSAYSeparation, 1);
#else
      iY = iCreate_RSA(iXGrphStart, iY + iSummGrphRSAYSeparation, 1);
#endif
      }
    //else
    //  { // Printing. Leave some extra room between the 2 sets of graphs
    //    // and leave some extra room after the facility.
    //  if(theApp.m_ptStudyType == PT_STANDARD)
    //    iY = iCreate_summary_graphs(iXGrphStart, iYRightStart + iYPixelsPerInch);
    //  iY = iCreate_RSA(iXGrphStart, iY + iYPixelsPerInch);
    //  }
    if(iY > iYPageEnd)
      iYPageEnd = iY;
    //if(theApp.m_ptStudyType == PT_STANDARD)
    //  iYPageEnd = iY;
    iY = iEnd_of_page(iXStart, iYPageEnd, iPageNum); // End of page processing.
    ++iPageNum; // Next page.
    }
  else
    { // Individual items
    iYRightStart = iY;
    if(m_iIncludeDiagRecom != 0 || m_iIncludePatientInfo != 0)
      {
      iYPageEnd = iAdd_left_side(iX, iY);
      bHavePage = true;
      }
    else
      iYPageEnd = 0;
    // Now do summary graphs and RSA.
    // If summary graphs go on first page, show them first.
    // If RSA goes on first page, show it second if there are summary graphs
    // on the first page.  If there are no summary graphs on first page, then
    // show the RSA first.
    bSummGrphDisplayed = false;
    if(m_iIncludeSummGraphs != 0)
      {
       if(g_pConfig->iGet_item(CFG_SUMM_GRAPHS_FIRST_PAGE) == 1
         && m_pDoc->m_uReportItemToView != RPT_VIEW_ITEM_SUMM_GRPHS)
         { // Summary graphs on first page.
         m_iRightWidth = m_iLineExt - m_iLeftWidth;
         iXGrphStart = m_iLeftWidth;
         iY = iYRightStart; // Now add the first page graphs to the right side.
        //if(m_pDC->IsPrinting() == 0)
          iY = iCreate_summary_graphs(iXGrphStart, iY); // Not printing
        //else
        //  iY = iCreate_summary_graphs(iXGrphStart, iYRightStart + iYPixelsPerInch);
        bSummGrphDisplayed = true;
        if(iY > iYPageEnd)
          iYPageEnd = iY;
        bHavePage = true;
        }
      }
    if(m_iIncludeRSA != 0)
      {
      if(g_pConfig->iGet_item(CFG_RSA_FIRST_PAGE) == 1)
        { // RSA on first page.
        m_iRightWidth = m_iLineExt - m_iLeftWidth;
        iXGrphStart = m_iLeftWidth;
        if(bSummGrphDisplayed == false)
          iY = iYRightStart; // Now add the first page graphs to the right side.
        //if(m_pDC->IsPrinting() == 0)
          iY = iCreate_RSA(iXGrphStart, iY + iSummGrphRSAYSeparation, 1); // Not printing
        //else
        //  iY = iCreate_RSA(iXGrphStart, iY + iYPixelsPerInch);
        if(iY > iYPageEnd)
          iYPageEnd = iY;
        bHavePage = true;
        }
      }
    // This is the end of the first page.
    //if(m_iNumPages != 0)
    //if(bHavePage == true)
    if(iYPageEnd > 0 && m_iNumPages != 0)
      {
      iY = iEnd_of_page(iXStart, iYPageEnd, iPageNum); // End of page processing.
      ++iPageNum; // Next page.
      }
   
    if(m_iIncludeSummGraphs != 0)
      { // Summary graphs are being displayed/printed on a page other than 1.
      if(g_pConfig->iGet_item(CFG_SUMM_GRAPHS_OTHER_PAGE) == 1
         || m_pDoc->m_uReportItemToView == RPT_VIEW_ITEM_SUMM_GRPHS)
        { // Summary graphs on other than first page.
          // Center horizontally and start at 10% of screen width.
        iXGrphStart = m_iLineExt / 10;
          // width of both graphs is 80% of screen width.
        m_iRightWidth = m_iLineExt - (iXGrphStart * 2);
            // Display/print the header information.
        iY = iAdd_header(iX, iY);
        if(m_pDC->IsPrinting() == 0)
          iY = iCreate_summary_graphs(iXGrphStart, iY); // Not printing
        else
          iY = iCreate_summary_graphs(iXGrphStart, iYRightStart + iYPixelsPerInch);
        
        if(m_iNumPages != 0)
          {
          iY = iEnd_of_page(iXStart, iY, iPageNum); // End of page processing.
          ++iPageNum; // Next page.
          }
        }
      }
    if(m_iIncludeRSA != 0)
      {
      if(g_pConfig->iGet_item(CFG_RSA_OTHER_PAGE) == 1)
        { // RSA on other than first page.
          // Center horizontally and start at 10% of screen width.
        iXGrphStart = 10; //m_iLineExt / 10;
          // width of graph is 80% of screen width.
        m_iRightWidth = m_iLineExt - (iXGrphStart * 2);
          // Display/print the header information.
        iY = iAdd_header(iX, iY);
        if(m_pDC->IsPrinting() == 0)
          iY = iCreate_RSA(iXGrphStart, iY, 1); // + 50); // Not printing
        else
          iY = iCreate_RSA(iXGrphStart, iY + iYPixelsPerInch, 1);
        iY = iEnd_of_page(iXStart, iY, iPageNum); // End of page processing.
        ++iPageNum; // Next page.
        }
      else if(g_pConfig->iGet_item(CFG_BOTH_RSA) == 1)
        {
        bool bFilter = g_bFilterRSA;
          // Center horizontally and start at 10% of screen width.
        iXGrphStart = 10; //m_iLineExt / 10;
          // width of graph is 80% of screen width.
        m_iRightWidth = m_iLineExt - (iXGrphStart * 2);
        //// First the normal RSA
        g_bFilterRSA = false;
          // Display/print the header information.
        iY = iAdd_header(iX, iY);
        if(m_pDC->IsPrinting() == 0)
          iY = iCreate_RSA(iXGrphStart, iY, 1); // + 50); // Not printing
        else
          iY = iCreate_RSA(iXGrphStart, iY + iYPixelsPerInch, 1);
        iY = iEnd_of_page(iXStart, iY, iPageNum); // End of page processing.
        ++iPageNum; // Next page.
        //// Second the filtered RSA
        g_bFilterRSA = true;
          // Display/print the header information.
        iY = iAdd_header(iX, iY);
        if(m_pDC->IsPrinting() == 0)
          iY = iCreate_RSA(iXGrphStart, iY, 2); // + 50); // Not printing
        else
          iY = iCreate_RSA(iXGrphStart, iY + iYPixelsPerInch, 2);
        iY = iEnd_of_page(iXStart, iY, iPageNum); // End of page processing.
        ++iPageNum; // Next page.
        g_bFilterRSA = bFilter;
        }
      }
    }

  if(m_iIncludeRawSigSelMin != 0)
    { // Display/print the header information.
    iY = iAdd_header(iXStart, iY);
    iY = iCreate_sel_raw_data_graphs(iXStart, iY, &iPageNum);
    if(theApp.m_ptStudyType == PT_STANDARD)
      {
      iY = iEnd_of_page(iXStart, iY, iPageNum); // End of page processing.
      ++iPageNum; // Next page.
      }
    }

    // Custom report items.
  if(m_iIncludeDataSheet != 0)
    {
    int iPage, iTotalPages;
      // Display/print the header information.
    if(theApp.m_iFreqLimit == FREQ_LIMIT_15_CPM
      || m_pDoc->m_uReportItemToView == RPT_VIEW_ITEM_DATA_SHEET
      || m_pDC->IsPrinting() == 0)
      {
      iPage = -1;
      iTotalPages = 1;
      }
    else
      {
      iPage = 1;
      if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
        iTotalPages = 2;
      else if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
        iTotalPages = 3;
      }
    iY = iAdd_header(iXStart, iY);
    iY = iCreate_data_sheet(iXStart, iY, iPage, iTotalPages);
    if(m_iNumPages != 0)
      {
      iY = iEnd_of_page(iXStart, iY, iPageNum); // End of page processing.
      ++iPageNum; // Next page.
      }
#ifdef _200_CPM_HARDWARE
    if(iPage > 0) //m_pDoc->m_uReportItemToView == RPT_VIEW_ITEM_DATA_SHEET)
      {
      if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM
        || theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
        {
        iY = iAdd_header(iXStart, iY);
        iY = iCreate_data_sheet(iXStart, iY, 2, iTotalPages);
        if(m_iNumPages != 0)
          {
          iY = iEnd_of_page(iXStart, iY, iPageNum); // End of page processing.
          ++iPageNum; // Next page.
          }
        if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
          {
          iY = iAdd_header(iXStart, iY);
          iY = iCreate_data_sheet(iXStart, iY, 3, iTotalPages);
          if(m_iNumPages != 0)
            {
            iY = iEnd_of_page(iXStart, iY, iPageNum); // End of page processing.
            ++iPageNum; // Next page.
            }
          }
        }
      }
#endif
    }
  if(m_iIncludeEntireRawSignal != 0)
    {
    m_bAddedEndOfPage = false;
    iY = iCreate_all_raw_data_graphs(iXStart, iY, &iPageNum);
    if(m_iNumPages != 0 && m_bAddedEndOfPage == false)
      {
      iY = iEnd_of_page(iXStart, iY, iPageNum); // End of page processing.
      ++iPageNum; // Next page.
      }
    }
  else
    m_iRawDataGraphStartY = -1; // Not using raw data signals.

  if(m_iIncludeEventLog != 0)
    { // Event log.  Research version only.
    m_bAddedEndOfPage = false;
      // Display/print the header information.
    iY = iAdd_header(iXStart, iY);
    iY = iCreate_event_log(iXStart, iY);
    if(m_iNumPages != 0 && m_bAddedEndOfPage == false)
      {
      iY = iEnd_of_page(iXStart, iY, iPageNum); // End of page processing.
      ++iPageNum; // Next page.
      }
    }

  m_csViewSize.cy = iY;
  ResyncScrollSizes();
  }

/////////////////////////////////////////////////////////////////////////////
// CReportView diagnostics

#ifdef _DEBUG
void CReportView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CReportView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CReportView message handlers


/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
ResyncScrollSizes - Reset to scroll bars.

  inputs: None.

  return: None.
********************************************************************/
void CReportView::ResyncScrollSizes()
  {

  CClientDC dc(NULL);
  OnPrepareDC(&dc);
  dc.LPtoDP(&m_csViewSize);
	SetScrollSizes(MM_TEXT, m_csViewSize);
  }

/********************************************************************
vSet_graph_summ_characteristics - Set the characterists of the summary graphs.


  inputs: Window handle of the summary graph whose characteristics are being set.
          Resource ID of the graph title.

  return: None.
********************************************************************/
void CReportView::vSet_graph_summ_characteristics(HWND hGraph, unsigned int iTitleResourceID)
  {
  CString cstrTitle;
  int iLineTypes[3], iTypes[11];
//  int iType;
  double dNullData, fYLoc[11], dValue;
//  double dAxisCntrl, dX;
  DWORD dwArray[3];
  float fValue;
    // Specify the numbers at the tick marks for the Y-axis.
  char szText[] = "|L0 %\t|L10 %\t|L20 %\t|L30 %\t|L40 %\t|L50 %\t|L60 %\t|L70 %\t|L80 %\t|L90 %\t|L100 %";
  char szText1[] = "|hBL\t|h10\t|h20\t|h30";

  if(hGraph != NULL)
    {
  		// Make sure zeros are plotted
    dNullData = -.01F;
    PEvset(hGraph, PEP_fNULLDATAVALUEX, &dNullData, 1);
    PEvset(hGraph, PEP_fNULLDATAVALUE, &dNullData, 1);

      // Remove plotting method from pop-up menu.
    PEnset(hGraph, PEP_bALLOWPLOTCUSTOMIZATION, FALSE);

      // Give the graph a main title.
    //cstrTitle.LoadString(iTitleResourceID);
    cstrTitle = cstrGet_col_title(iTitleResourceID);
    PEszset(hGraph, PEP_szMAINTITLE, (char *)(const char *)cstrTitle);
    PEszset(hGraph, PEP_szSUBTITLE, ""); // no subtitle
    fValue = 1.15F;
    PEvset(hGraph, PEP_fFONTSIZETITLECNTL, &fValue, 1);
    
//    cstrTitle.LoadString(IDS_PERCENTAGE);
//    PEszset(hGraph, PEP_szYAXISLABEL, (char *)(const char *)cstrTitle); // Y axis label.
//    cstrTitle.LoadString(IDS_PERIOD);
//    PEszset(hGraph, PEP_szXAXISLABEL, (char *)(const char *)cstrTitle);  // X axis label.

    PEnset(hGraph, PEP_nFONTSIZE, PEFS_LARGE); // Set to large font size.

    PEnset(hGraph, PEP_bMARKDATAPOINTS, TRUE); // Mark the data points.
    PEnset(hGraph, PEP_nHOTSPOTSIZE, PEHSS_SMALL); // Set size of marked data points.

    fValue = 1.5F;
    PEvset(hGraph, PEP_fFONTSIZEALCNTL, &fValue, 1);
    

/****************
      // Manually Control X Axis Grid Line Density
    dAxisCntrl = 10.0f; // Multiples of grid tick marks where values are placed.
    PEvset(hGraph, PEP_fMANUALXAXISLINE, &dAxisCntrl, 1);
    dAxisCntrl = 10.0F; // Where grid tick marks are placed.
    PEvset(hGraph, PEP_fMANUALXAXISTICK, &dAxisCntrl, 1);
    PEnset(hGraph, PEP_bMANUALXAXISTICKNLINE, TRUE);
*******************/
      // Label the X axis for the period.
      // Remove everything on the X-axis.
//    PEnset(hGraph, PEP_nSHOWXAXIS, PESA_EMPTY);
    PEnset(hGraph, PEP_nSHOWXAXIS, PESA_LABELONLY); //PESA_AXISLABELS);
    //cstrTitle.LoadString(IDS_PERIOD);
    cstrTitle = g_pLang->cstrLoad_string(ITEM_G_PERIOD);
    PEszset(hGraph, PEP_szXAXISLABEL, (char *)(const char *)cstrTitle);  // X axis label.

/****************************************************** // Not using.
      // Set the minimum and maximum X-axis scale values.
    PEnset(hGraph, PEP_nMANUALSCALECONTROLX, PEMSC_MINMAX);
    dValue = (double)0.0;
    PEvset(hGraph, PEP_fMANUALMINX, &dValue, 1);
    dValue = (double)30.0;
    PEvset(hGraph, PEP_fMANUALMAXX, &dValue, 1);
    PEvsetcell(hGraph, PEP_szaPOINTLABELS, 0, "BL");
    PEvsetcell(hGraph, PEP_szaPOINTLABELS, 1, "10");
    PEvsetcell(hGraph, PEP_szaPOINTLABELS, 2, "20");
    PEvsetcell(hGraph, PEP_szaPOINTLABELS, 3, "30");
       PEnset(hGraph, PEP_bSIMPLEPOINTLEGEND, TRUE);
       PEnset(hGraph, PEP_bSIMPLELINELEGEND, TRUE);
       PEnset(hGraph, PEP_nLEGENDSTYLE, PELS_1_LINE);
       PEnset(hGraph, PEP_nGRIDLINECONTROL, PEGLC_NONE);
//int nArray[4];
//nArray[0] = 0;
//nArray[1] = 10;
//nArray[2] = 20;
//nArray[3] = 30;
//PEvset (hGraph, PEP_naALTFREQUENCIES, nArray, 4);
      // Specify type of tick mark.
    iTypes[0] = PELAT_GRIDTICK;
    iTypes[1] = PELAT_GRIDTICK;
    iTypes[2] = PELAT_GRIDTICK;
    iTypes[3] = PELAT_GRIDTICK;
    PEvset(hGraph, PEP_naVERTLINEANNOTATIONTYPE, iTypes, 4);
************************************************/

      // Set the minimum and maximum X-axis scale values.
    PEnset(hGraph, PEP_nMANUALSCALECONTROLX, PEMSC_MINMAX);
    dValue = (double)0.0;
    PEvset(hGraph, PEP_fMANUALMINX, &dValue, 1);
    dValue = (double)30.0;
    PEvset(hGraph, PEP_fMANUALMAXX, &dValue, 1);
      // Specify type of tick mark.
    iTypes[0] = PELAT_GRIDTICK;
    iTypes[1] = PELAT_GRIDTICK;
    iTypes[2] = PELAT_GRIDTICK;
    iTypes[3] = PELAT_GRIDTICK;
    PEvset(hGraph, PEP_naVERTLINEANNOTATIONTYPE, iTypes, 4);
      // Specify where on the Y-axis the tick marks go.
    fYLoc[0] = 0;
    fYLoc[1] = 10;
    fYLoc[2] = 20;
    fYLoc[3] = 30;
    PEvset(hGraph, PEP_faVERTLINEANNOTATION, fYLoc, 4);
      // Specify the numbers at the tick marks for the X-axis.
    PEvset(hGraph, PEP_szaVERTLINEANNOTATIONTEXT, szText1, 4);
      // Make the bottom margin wide enough to display the annotations.
    PEszset(hGraph, PEP_szBOTTOMMARGIN, "BL");
      // Set flag so the above data is used.
    PEnset(hGraph, PEP_bSHOWANNOTATIONS, TRUE);
    PEnset(hGraph, PEP_bSHOWVERTLINEANNOTATIONS, TRUE);

      // Label the Y axis using percentage.
      // Remove everything on the Y-axis.
    PEnset(hGraph, PEP_nSHOWYAXIS, PESA_EMPTY);
      // Set the minimum and maximum Y-axis scale values.
    PEnset(hGraph, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
    dValue = (double)0.0;
    PEvset(hGraph, PEP_fMANUALMINY, &dValue, 1);
    dValue = (double)100.0;
    PEvset(hGraph, PEP_fMANUALMAXY, &dValue, 1);

      // Specify type of tick mark.
    iTypes[0] = PELAT_GRIDTICK;
    iTypes[1] = PELAT_GRIDTICK;
    iTypes[2] = PELAT_GRIDTICK;
    iTypes[3] = PELAT_GRIDTICK;
    iTypes[4] = PELAT_GRIDTICK;
    iTypes[5] = PELAT_GRIDTICK;
    iTypes[6] = PELAT_GRIDTICK;
    iTypes[7] = PELAT_GRIDTICK;
    iTypes[8] = PELAT_GRIDTICK;
    iTypes[9] = PELAT_GRIDTICK;
    iTypes[10] = PELAT_GRIDTICK;
    PEvset(hGraph, PEP_naHORZLINEANNOTATIONTYPE, iTypes, 11);
      // Specify where on the Y-axis the tick marks go.
    fYLoc[0] = 0;
    fYLoc[1] = 10;
    fYLoc[2] = 20;
    fYLoc[3] = 30;
    fYLoc[4] = 40;
    fYLoc[5] = 50;
    fYLoc[6] = 60;
    fYLoc[7] = 70;
    fYLoc[8] = 80;
    fYLoc[9] = 90;
    fYLoc[10] = 100;
    PEvset(hGraph, PEP_faHORZLINEANNOTATION, fYLoc, 11);
      // Specify the numbers at the tick marks for the X-axis.
    PEvset(hGraph, PEP_szaHORZLINEANNOTATIONTEXT, szText, 11);
      // Set flag so the above data is used.
    PEnset(hGraph, PEP_bSHOWANNOTATIONS, TRUE);
      // Make the left margin wide enough to display the annotations.
    PEszset(hGraph, PEP_szLEFTMARGIN, "100%   ");

      // remove the legends for the subsets.
    int iTemp = -1;
    PEvsetcell(hGraph, PEP_naSUBSETSTOLEGEND, 0, &iTemp);

      // subset colors
    dwArray[0] = CONTROL_RANGE_COLOR; // High control
    dwArray[1] = PATIENT_COLOR; // Patient
    dwArray[2] = CONTROL_RANGE_COLOR; // Low control
    PEvset(hGraph, PEP_dwaSUBSETCOLORS, dwArray, 3);
		
      // subset line types
    iLineTypes[0] = PELT_DASH; // High control
    iLineTypes[1] = PELT_MEDIUMSOLID; //PELT_EXTRATHICKSOLID; //PELT_THICKSOLID; //PELT_MEDIUMSOLID; //PELT_THINSOLID; // Patient
    iLineTypes[2] = PELT_DASH; // Low control
    PEvset(hGraph, PEP_naSUBSETLINETYPES, iLineTypes, 3);

/*****************
      // Display "BL" for the baseline at the point 0.0. 
    PEnset(hGraph, PEP_bSHOWANNOTATIONS, TRUE);
    PEnset(hGraph, PEP_bSHOWVERTLINEANNOTATIONS, TRUE);
    dX = (double)0.0;
    PEvsetcell(hGraph, PEP_faVERTLINEANNOTATION, 0, &dX);
    iType = PELT_THINSOLID;
    PEvsetcell(hGraph, PEP_naVERTLINEANNOTATIONTYPE, 0, &iType);
//    PEvsetcell(hGraph, PEP_szaVERTLINEANNOTATIONTEXT, 0, "|h  \n      BL");
    cstrTitle.LoadString(IDS_BL);
    cstrTitle.Insert(0, "|h        ");
    PEvsetcell(hGraph, PEP_szaVERTLINEANNOTATIONTEXT, 0, (char *)(const char *)cstrTitle);
********************/
      // Increase line annotation text size
    PEnset(hGraph, PEP_nLINEANNOTATIONTEXTSIZE, 125); //150);
//    PEnset(hGraph, PEP_nAXESANNOTATIONTEXTSIZE, 150);
      // Don't draw any grid lines.
    PEnset(hGraph, PEP_nGRIDLINECONTROL, PEGLC_NONE);
    }
  }

/********************************************************************
vAnalyze_data - Analyze the data for the type of analysis.

  inputs: Type of analysis.

  return: None.
********************************************************************/
void CReportView::vAnalyze_data(short int iAnalysisType)
  {
  REQ_ANALYZE_DATA reqAnlData;
  short int iIndex, iRet = SUCCESS;
  unsigned short uSize, iMaxFreqPts, uEpochCnt;
  float fLastMinute, fStartMinute;
  //bool bHalfMin;

    // First find out how many data points will be needed and allocate an array.
    // total CPM * 4 so that we get four points for each cpm.
  iMaxFreqPts = ((15 - 0) * 4) + 1;
  uSize = 0;
  for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
    {
    if(m_pDoc->m_pdPatient.fGoodMinStart[iIndex] != INV_MINUTE)
      { // Now find out how many data points for this period.
        // Add an extra 2 for the blanks lines separating pre and post water periods.
      uSize += ((uGet_num_epochs(m_pDoc->m_pdPatient.fGoodMinStart[iIndex],
        m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]) + 2) * iMaxFreqPts);
      //uSize += (((unsigned short)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
      //          - (unsigned short)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 2 + 2)
      //          * iMaxFreqPts);
      }
    else
      iRet = FAIL;
    }
  if(iRet == SUCCESS)
    {
    if(iAnalysisType == DATA_SHEET_TYPE)
      { // Allocate data arrays needed for the data sheet.
      vFree_data_sheet_arrays();
      if((m_adData.pfPwrSum = (float *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
      (m_uMaxPeriods * sizeof(float) * theApp.m_iMaxFreqBands))) == NULL
      || (m_adData.pfRatioAvgPwr = (float *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
      (m_uMaxPeriods * sizeof(float) * theApp.m_iMaxFreqBands))) == NULL
      || (m_adData.pfDomFreq = (float *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
      uSize * sizeof(float))) == NULL
      || (m_adData.pfPctDomFreq = (float *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
      (m_uMaxPeriods * sizeof(float) * theApp.m_iMaxFreqBands))) == NULL)
        iRet = FAIL;
      }
    else
      {
      m_adData.pfRawData = NULL;
      m_adData.pfPwrSum = NULL;
      m_adData.pfDomFreq = NULL;
      m_adData.pfRatioAvgPwr = NULL;
      m_adData.pfPctDomFreq = NULL;
      }
    }
  if(iRet == SUCCESS)
    {
    for(iIndex = 0; iIndex < MAX_TOTAL_PERIODS * theApp.m_iMaxFreqBands; ++iIndex)
      m_adData.fAvgPwr[iIndex] = 0.0F;

    uEpochCnt = 0;
      // For each period that has good minutes defined.
    for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
      { // Now for each valid period, analyze all the 4 minute epochs.
        // Read in and graph the baseline period.
      if(m_pDoc->iRead_period(iIndex, m_pDoc->m_fData) == SUCCESS)
        {
        reqAnlData.iChannel = 0; // EGG channel.
        reqAnlData.iNumChannelsInFile = 2;
        reqAnlData.iType = iAnalysisType; //DIAGNOSIS_TYPE;
        reqAnlData.padData = &m_adData;
        reqAnlData.bUsePartMinAtDataEnd = false;
        reqAnlData.uTimeBand = iIndex;
        fLastMinute = m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] - 4;
        fStartMinute = m_pDoc->m_pdPatient.fGoodMinStart[iIndex];
        //bHalfMin = bIs_half_minute(fStartMinute);
        while(fStartMinute <= fLastMinute)
          {
          if(theApp.m_ptStudyType == PT_STANDARD)
            reqAnlData.fEpochNum = fStartMinute; // Standard study
          else // Research study
            reqAnlData.fEpochNum = fStartMinute - m_pDoc->m_pdPatient.fPeriodStart[iIndex];
          reqAnlData.uRunningEpochCnt = uEpochCnt;
          if(m_pDoc->m_pAnalyze->bAnalyze_data(&reqAnlData) == true)
            { // Save the data for graphing.
//            memcpy((unsigned char *)pfData, adData.fData, iMaxFreqPts * sizeof(float));
//            pfData += m_iMaxFreqPts;
            }
          else
            {
            iRet = FAIL;
            break;
            }
          // Find out if start minutes are incremented on the half or whole minute.
          //if(bHalfMin == true)
          //  fStartMinute += 0.5F;
          //else
            ++fStartMinute;
          ++uEpochCnt;
          }
        reqAnlData.uRunningEpochCnt = uGet_num_epochs(m_pDoc->m_pdPatient.fGoodMinStart[iIndex],
          m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]);
        //reqAnlData.uRunningEpochCnt = (unsigned short)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
        //                               - (unsigned short)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 3; //2;
        m_pDoc->m_pAnalyze->vCalc_diagnosis(&reqAnlData);
        if(iAnalysisType == DATA_SHEET_TYPE)
          { // Do some additional analysis for the data sheet.
          m_pDoc->m_pAnalyze->vCalc_datasheet(&reqAnlData);
          }
        }
      else
        {
        iRet = FAIL; // Reading data file failed
        break;
        }
      }
    }
  }

/********************************************************************
iCenter_text - Calculate the offset of the X coordinate within the column
               for where to start printing the text so that the text is
               centered in the column.

  inputs: CString object containing the text to be centered.
          Number of pixels in the column where the text is to be centered.

  return: Offset of the X corrdinate within the column of where to start
          displaying the text.
********************************************************************/
int CReportView::iCenter_text(CString cstrItem, int iColWidth)
  {
  CSize cs;
  int iOffset;

  cs = m_pDC->GetTextExtent(cstrItem);
  if(iColWidth > cs.cx)
    iOffset = (iColWidth - cs.cx) / 2;
  else
    iOffset = 0;

  return(iOffset);
  }

/********************************************************************
iCreate_summary_table - Create the table of summary data.

                        Center the Summary table centered between the
                        left and right coordinates of the graphs.

  inputs: X coordinate of where to start the table.
          Y coordinate of where to start the table.
          Width of the summary graphs.

  return: Y coordinate of last line displayed.
********************************************************************/
int CReportView::iCreate_summary_table(int iXStart, int iYStart, int iSummGrphsWidth)
  {
  CString cstrItem1, cstrItem2;
  CSize csText1, csText2;
  int iCol[CONTROL_FREQ_BANDS], iY, iX, iX1;
  short int iIndex, iTableWidth, iXTableStart, iColSpacePad;
  float *pfControl, *pfPctGrp1;
  CFont cfFont;
  int iDCSave = -1;

  iY = iYStart;
  iX = iXStart;

  if(m_pDoc->m_uReportItemToView == RPT_VIEW_ITEM_NONE)
    {  // Part of the report
    if(g_pConfig->iGet_item(CFG_SUMM_GRAPHS_FIRST_PAGE) == 1
      || m_iIncludeCoreReport != 0)
      { // On first page of report.
      // Set a smaller text.
      // Save the DC so it can be put back after the title.  We do this
      // because we are changing the font.
      iDCSave = ::SaveDC(m_pDC->m_hDC);
      // Create a large, bold font.
      if(m_pDC->IsPrinting() != 0)
        vSet_font(NULL, m_pDC, &cfFont, FW_THIN, 7); // Smaller font for printing.
      else
        vSet_font(NULL, m_pDC, &cfFont, FW_BOLD, 11);
      m_pDC->SelectObject(&cfFont);
      }
    if(g_pConfig->iGet_item(CFG_SUMM_GRAPHS_OTHER_PAGE) == 1)
      //|| m_iIncludeCoreReport != 0)
      {
      // Create a large, bold font.
      if(m_pDC->IsPrinting() != 0)
        {
        iDCSave = ::SaveDC(m_pDC->m_hDC);
        vSet_font(NULL, m_pDC, &cfFont, FW_NORMAL, 11);
        m_pDC->SelectObject(&cfFont);
        }
     }
    }

  if(m_pDC->IsPrinting() != 0)
    iColSpacePad = 100; // printing.
  else
    iColSpacePad = 20;
  // Calculate X so the table is centered under the graphs.
  csText1 = m_pDC->GetTextExtent(g_pLang->cstrLoad_string(ITEM_G_MINUTES));
  //iTableWidth = 0;
  iTableWidth = (short int)csText1.cx + iColSpacePad;
  for(iIndex = 0; iIndex < CONTROL_FREQ_BANDS; ++iIndex)
    {
    csText1 = m_pDC->GetTextExtent(cstrGet_col_title(s_uColTitles[iIndex]));
    iTableWidth += (short int)csText1.cx + iColSpacePad;
    }
  iXTableStart = ((iSummGrphsWidth - iTableWidth) / 2) + iXStart;
  iX = iXTableStart;
 
    // First line of Horizontal table header.
  //cstrItem1.LoadString(IDS_MINUTES);
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_MINUTES);
  csText1 = m_pDC->GetTextExtent(cstrItem1);
  m_pDC->TextOut(iX, iY, cstrItem1);
  for(iIndex = 0; iIndex < CONTROL_FREQ_BANDS; ++iIndex)
    {
    iX += csText1.cx + iColSpacePad;
    iCol[iIndex] = iX; // + csText1.cx + 20;
    //cstrItem1.LoadString(s_uColTitles[iIndex]);
    cstrItem1 = cstrGet_col_title(s_uColTitles[iIndex]);
    csText1 = m_pDC->GetTextExtent(cstrItem1);
    m_pDC->TextOut(iX, iY, cstrItem1);
    }
    // Second line of table header.
  //cstrItem1.LoadString(IDS_CONTROL);
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_CONTROL);
  csText1 = m_pDC->GetTextExtent(cstrItem1);
  //cstrItem2.LoadString(IDS_PATIENT);
  cstrItem2 = g_pLang->cstrLoad_string(ITEM_G_PATIENT);
  csText2 = m_pDC->GetTextExtent(cstrItem2);
  iY += csText1.cy;
  for(iIndex = 0; iIndex < CONTROL_FREQ_BANDS; ++iIndex)
    {
    iX = iCol[iIndex];
    m_pDC->TextOut(iX, iY, cstrItem1);
    m_pDC->TextOut(iX + csText1.cx + 5, iY, cstrItem2);
    }
  iX1 = iX + csText1.cx + 5 + csText2.cx;
  iX = iXTableStart; //iXStart;
    // Draw straight line between table header and data.
  iY += (csText1.cy + 2);
  m_pDC->MoveTo(iX, iY);
  m_pDC->LineTo(iX1, iY);
  iY += 2;
    // Table data.
    // Baseline label
  //cstrItem1.LoadString(IDS_BL);
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_BL);
  m_pDC->TextOut(iX, iY, cstrItem1);
  pfControl =  (float *)g_fControl;
  pfPctGrp1 = m_adData.fPctGrp1;
  for(iIndex = 0; iIndex < CONTROL_FREQ_BANDS; ++iIndex)
    {
    iX = iCol[iIndex];
    cstrItem1.Format("%.0f-%.0f", *pfControl, *(pfControl + CONTROL_FREQ_BANDS));
    iX1 = iCenter_text(cstrItem1, csText1.cx) + iX;
    m_pDC->TextOut(iX1, iY, cstrItem1);
    cstrItem1.Format("%.0f", *pfPctGrp1);
    iX1 = iCenter_text(cstrItem1, csText1.cx) + iX + csText1.cx + 5;
    m_pDC->TextOut(iX1, iY, cstrItem1);
    pfControl += (2 * CONTROL_FREQ_BANDS);
    ++pfPctGrp1;
    }
    // First 10 minute period.
  iY += (csText1.cy + 2);
  iX = iXTableStart; //iXStart;
  //cstrItem1.LoadString(IDS_MIN_10);
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_MIN_10);
  m_pDC->TextOut(iX, iY, cstrItem1);
  pfControl =  (float *)g_fControl;
  pfControl += 1;
  pfPctGrp1 = &m_adData.fPctGrp1[theApp.m_iMaxFreqBands];
  //pfPctGrp1 = &m_adData.fPctGrp1[MAX_FREQ_BANDS];
  for(iIndex = 0; iIndex < CONTROL_FREQ_BANDS; ++iIndex)
    {
    iX = iCol[iIndex];
    cstrItem1.Format("%.0f-%.0f", *pfControl, *(pfControl + CONTROL_FREQ_BANDS));
    iX1 = iCenter_text(cstrItem1, csText1.cx) + iX;
    m_pDC->TextOut(iX1, iY, cstrItem1);
    cstrItem1.Format("%.0f", *pfPctGrp1);
    iX1 = iCenter_text(cstrItem1, csText1.cx) + iX + csText1.cx + 5;
    m_pDC->TextOut(iX1, iY, cstrItem1);
    pfControl += (2 * CONTROL_FREQ_BANDS);
    ++pfPctGrp1;
    }
    // Second 10 minute period.
  iY += (csText1.cy + 2);
  iX = iXTableStart; //iXStart;
  //cstrItem1.LoadString(IDS_MIN_20);
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_MIN_20);
  m_pDC->TextOut(iX, iY, cstrItem1);
  pfControl =  (float *)g_fControl;
  pfControl += 2;
  pfPctGrp1 = &m_adData.fPctGrp1[2 * theApp.m_iMaxFreqBands];
  //pfPctGrp1 = &m_adData.fPctGrp1[2 * MAX_FREQ_BANDS];
  for(iIndex = 0; iIndex < CONTROL_FREQ_BANDS; ++iIndex)
    {
    iX = iCol[iIndex];
    cstrItem1.Format("%.0f-%.0f", *pfControl, *(pfControl + CONTROL_FREQ_BANDS));
    iX1 = iCenter_text(cstrItem1, csText1.cx) + iX;
    m_pDC->TextOut(iX1, iY, cstrItem1);
    cstrItem1.Format("%.0f", *pfPctGrp1);
    iX1 = iCenter_text(cstrItem1, csText1.cx) + iX + csText1.cx + 5;
    m_pDC->TextOut(iX1, iY, cstrItem1);
    pfControl += (2 * CONTROL_FREQ_BANDS);
    ++pfPctGrp1;
    }

    // Third 10 minute period.
  iY += (csText1.cy + 2);
  iX = iXTableStart; //iXStart;
  //cstrItem1.LoadString(IDS_MIN_30);
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_MIN_30);
  m_pDC->TextOut(iX, iY, cstrItem1);
  pfControl =  (float *)g_fControl;
  pfControl += 3;
  pfPctGrp1 = &m_adData.fPctGrp1[3 * theApp.m_iMaxFreqBands];
  //pfPctGrp1 = &m_adData.fPctGrp1[3 * MAX_FREQ_BANDS];
  for(iIndex = 0; iIndex < CONTROL_FREQ_BANDS; ++iIndex)
    {
    iX = iCol[iIndex];
    cstrItem1.Format("%.0f-%.0f", *pfControl, *(pfControl + CONTROL_FREQ_BANDS));
    iX1 = iCenter_text(cstrItem1, csText1.cx) + iX;
    m_pDC->TextOut(iX1, iY, cstrItem1);
    cstrItem1.Format("%.0f", *pfPctGrp1);
    iX1 = iCenter_text(cstrItem1, csText1.cx) + iX + csText1.cx + 5;
    m_pDC->TextOut(iX1, iY, cstrItem1);
    pfControl += (2 * CONTROL_FREQ_BANDS);
    ++pfPctGrp1;
    }
  iY += csText1.cy;
  
  if(m_pDoc->m_uReportItemToView == RPT_VIEW_ITEM_NONE)
    { // On page 1 of core report.
    // Restore the DC (put back the original font.
    if(iDCSave > 0)
      ::RestoreDC(m_pDC->m_hDC, iDCSave);
    }
  else
    { // On separate page.
    // Leave blank line at bottom of page.
    iY += csText1.cy;
    }
  
  return(iY);
  }

/********************************************************************
iCreate_summary_graphs - Create the summary graphs.
                         Display top row of 2 first then second row of 2.
                         At the bottom of the summary graphs, display the
                           Summary table centered between the left and right
                           coordinates of the graphs.

  inputs: X coordinate of where to start the graphs.
          Y coordinate of where to start the graphs.

  return: Y coordinate of bottom of the last graph.
********************************************************************/
int CReportView::iCreate_summary_graphs(int iXStart, int iYStart)
  {
  RECT rGraph; 
  int iY, iX, iSummGraphHorizSep, iSummGraphWidth, iSummGraphHeight;
  int iScreenHeight, iTotalWidth;
  unsigned short uIndex;
  bool bDisplayGraph;

  iY = iYStart;
  iX = iXStart;

    // If the summary graphs have moved to a different Y coordinate delete
    // the summary graph windows so that we will recreate them later.
  if(m_iSummaryGraphStart != iY)
    {
    for(uIndex = 0; uIndex < CONTROL_FREQ_BANDS; ++uIndex)
      {
      if(m_hPESum[uIndex] != NULL)
        {
        PEdestroy(m_hPESum[uIndex]);
        m_hPESum[uIndex] = NULL;
        }
      }
    m_iSummaryGraphStart = iY;
    }
    // Calculate summary graph size and position.
  if(m_pDoc->m_uReportItemToView == RPT_VIEW_ITEM_NONE)
    { // Part of the report
      // Calculate horizontal distance between summary graphs.
    iSummGraphHorizSep = (int)((float)m_iRightWidth * 0.01F); //0.02F);
      // Calculate summary graph width.
    iSummGraphWidth = (m_iRightWidth / 2) - ( 2 * iSummGraphHorizSep);
      // Summary graph width to height is 3 to 2.
    iSummGraphHeight = (iSummGraphWidth * 2) / 3;
    }
  else
    { // called from a toolbar item.
    iScreenHeight = m_pDC->GetDeviceCaps(VERTRES);
    iScreenHeight -= iY;
    if(g_pConfig->iGet_item(CFG_SUMM_TABLE_ON_SCREEN) == 1)
      {
      // Leave room for summary table at bottom.
      CSize csText = m_pDC->GetTextExtent("1");
      iScreenHeight -= csText.cy * 10;
      }
    iSummGraphHeight = iScreenHeight * 4 / 10;
    iSummGraphWidth = (iSummGraphHeight / 2) * 3;
    iSummGraphHorizSep = (int)((float)(iSummGraphWidth * 2) * 0.01F);
    }

    // First line, first graph.
  rGraph.top = iY;
  rGraph.bottom = iY + iSummGraphHeight;
  rGraph.right = iX + iSummGraphWidth;
  rGraph.left = iX;

    // Find out if the graphs will actually be visible. This only applies
    // if we are not printing.  If printing, the graphs will always be
    // visible.
  if((m_pDC->IsPrinting() == 0 && m_pDC->RectVisible(&rGraph) != 0)
  || m_pDC->IsPrinting() != 0)
    bDisplayGraph = true; // Graph visible.
  else
    bDisplayGraph = false; // Graph not visible.

    // Create all graph windows first, whether they are visible or not.
    // Display graphs only if they are visible.
  if(m_hPESum[0] == NULL)
    m_hPESum[0] = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rGraph, m_hWnd, IDR_MAINFRAME);
  if(m_hPESum[0] != NULL)
    {
    if(bDisplayGraph == true)
      {
      vSet_graph_summ_characteristics(m_hPESum[0], s_uGraphTitles[0]);
      vDraw_summ_graph(m_hPESum[0], 0);
      PEreinitialize(m_hPESum[0]);
      PEresetimage(m_hPESum[0], 0, 0);
      }
    if(m_pDC->IsPrinting() != 0)
      vPrint_graph(m_hPESum[0], &rGraph); // Print the graph.
    }

    // First line, second graph.
  rGraph.left = rGraph.right + iSummGraphHorizSep;
  rGraph.right = rGraph.right + iSummGraphWidth;
  if(m_hPESum[1] == NULL)
    m_hPESum[1] = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rGraph, m_hWnd, IDR_MAINFRAME);
  if(m_hPESum[1] != NULL)
    {
    if(bDisplayGraph == true)
      {
      vSet_graph_summ_characteristics(m_hPESum[1], s_uGraphTitles[1]);
      vDraw_summ_graph(m_hPESum[1], 1);
      PEreinitialize(m_hPESum[1]);
      PEresetimage(m_hPESum[1], 0, 0);
      }
    if(m_pDC->IsPrinting() != 0)
      vPrint_graph(m_hPESum[1], &rGraph); // Print the graph.
    }

    // Second line, first graph.
    // Summary graph vertical separation is 2 * horizontal separation.
  rGraph.top = rGraph.bottom + (iSummGraphHorizSep * 2); // + csTextSize.cy;
  rGraph.bottom = rGraph.top + iSummGraphHeight;
  rGraph.right = iX + iSummGraphWidth;
  rGraph.left = iX;
  if(m_hPESum[2] == NULL)
    m_hPESum[2] = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rGraph, m_hWnd, IDR_MAINFRAME);
  if(m_hPESum[2] != NULL)
    {
    if(bDisplayGraph == true)
      {
      vSet_graph_summ_characteristics(m_hPESum[2], s_uGraphTitles[2]);
      vDraw_summ_graph(m_hPESum[2], 2);
      PEreinitialize(m_hPESum[2]);
      PEresetimage(m_hPESum[2], 0, 0);
      }
    if(m_pDC->IsPrinting() != 0)
      vPrint_graph(m_hPESum[2], &rGraph); // Print the graph.
    }

    // Second line, second graph.
  rGraph.left = rGraph.right + iSummGraphHorizSep;
  rGraph.right = rGraph.right + iSummGraphWidth;
  iTotalWidth = m_iRightWidth; //rGraph.right - iX + iSummGraphHorizSep;
  if(m_hPESum[3] == NULL)
    m_hPESum[3] = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rGraph, m_hWnd, IDR_MAINFRAME);
  if(m_hPESum[3] != NULL)
    {
    if(bDisplayGraph == true)
      {
      vSet_graph_summ_characteristics(m_hPESum[3], s_uGraphTitles[3]);
      vDraw_summ_graph(m_hPESum[3], 3);
      PEreinitialize(m_hPESum[3]);
      PEresetimage(m_hPESum[3], 0, 0);
      }
    if(m_pDC->IsPrinting() != 0)
      vPrint_graph(m_hPESum[3], &rGraph); // Print the graph.
    }
    
    // Leave a blank line between the summary graphs and table.
  CSize csText1;
  csText1 = m_pDC->GetTextExtent("A");
  iY = csText1.cy + rGraph.bottom;
  // Display the summary table.
  iY = iCreate_summary_table(iXStart, iY, iTotalWidth);
  
  return(iY);
  //return(rGraph.bottom);
  }

/********************************************************************
vDraw_summ_graph - Draw one summary graph.

  inputs: Window handle of the graph being drawn.
          Index into m_hPESum array of the graph to draw.

  return: None.
********************************************************************/
void CReportView::vDraw_summ_graph(HWND hGraph, short int iGrphIndx)
  {
  int iCnt, iMin, iMaxPeriods;
  float fX; //, fY;
  float *pfControl, *pfPctGrp1;

  if(hGraph != NULL)
    {
    if(m_pDoc->m_uMaxPeriods > MAX_PERIODS)
      iMaxPeriods = MAX_PERIODS;
    else
      iMaxPeriods = m_pDoc->m_uMaxPeriods;
      // Set the number of subsets (lines to draw).
    PEnset(hGraph, PEP_nSUBSETS, 3);
    PEnset(hGraph, PEP_nPOINTS, iMaxPeriods); //m_pDoc->m_uMaxPeriods);
      // Get the starting position of the data.
    pfControl =  (float *)g_fControl;
    pfControl += (iGrphIndx * 2 * CONTROL_FREQ_BANDS);
    pfPctGrp1 = &m_adData.fPctGrp1[iGrphIndx]; // * MAX_FREQ_BANDS];
      // Plot the points.
    iMin = 0;

//    for(iCnt = 0; iCnt < m_pDoc->m_uMaxPeriods; ++iCnt)
    for(iCnt = 0; iCnt < iMaxPeriods; ++iCnt)
      {
      fX = (float)iMin;
        // High control.
      PEvsetcellEx(hGraph, PEP_faXDATA, 0, iCnt, &fX);
      PEvsetcellEx(hGraph, PEP_faYDATA, 0, iCnt, pfControl + CONTROL_FREQ_BANDS); // + CONTROL_FREQ_BANDS);
        // Low control.
      PEvsetcellEx(hGraph, PEP_faXDATA, 2, iCnt, &fX);
      PEvsetcellEx(hGraph, PEP_faYDATA, 2, iCnt, pfControl);
        // Patient
      PEvsetcellEx(hGraph, PEP_faXDATA, 1, iCnt, &fX);
      PEvsetcellEx(hGraph, PEP_faYDATA, 1, iCnt, pfPctGrp1);
      iMin += 10;
      ++pfControl; // += (2 * CONTROL_FREQ_BANDS);
      pfPctGrp1 += theApp.m_iMaxFreqBands;
      //pfPctGrp1 += MAX_FREQ_BANDS;
      }
    }
  }

/********************************************************************
iCreate_data_sheet - Create the data sheet report.

  inputs: X coordinate of where to start the data sheet.
          Y coordinate of where to start the data sheet.

  return: Y coordinate of where to start the next line (includes one
          empty line).
********************************************************************/
int CReportView::iCreate_data_sheet(int iXStart, int iYStart, int iPage, int iTotalPages)
  {
  CFont cfFont;
  TEXTMETRIC tm;
  CString cstrItem1, cstrDesc;
  CSize csText1;
  short int iIndex, iIndex1, iDataIndex, iColIndex, iSize;
  int iY, iY1, iX, iX1, iWidth, iHeight, iRowTitleWidth, iLeftTitleY;
  int iCol[30], iFirstRowColTitles, iYFirstRow, iMaxColumns, iStartIndex;
  int iTotalColumns; //iCol[7];
  unsigned uNumOfPoints, *puArray;
  float fStdDev, fMean, fPct, fCPM, fCPMInc;
  bool bLastPage;

  iY = iYStart;
  iX = iXStart;

  if(m_pDC->IsPrinting() != 0)
    { // Printing.  Use a smaller font.
#ifndef _200_CPM_HARDWARE
    vSet_font(NULL, m_pDC, &cfFont, FW_THIN, 10);
#else
    if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
      vSet_font(NULL, m_pDC, &cfFont, FW_THIN, 8);
    else
      vSet_font(NULL, m_pDC, &cfFont, FW_THIN, 10);
#endif
    m_pDC->SelectObject(&cfFont);
   // // Put in landscape mode.
    // This is for default printer, don't know how to set landscape
    // mode for the currently selected printer.
   //PRINTDLG pd;
   //pd.lStructSize=(DWORD)sizeof(PRINTDLG);
   //BOOL bRet=AfxGetApp()->GetPrinterDeviceDefaults(&pd);
   //if(bRet)
   //{
   //   // protect memory handle with ::GlobalLock and ::GlobalUnlock
   //   DEVMODE FAR *pDevMode=(DEVMODE FAR *)::GlobalLock(pd.hDevMode);
   //   // set orientation to landscape
   //   pDevMode->dmOrientation=DMORIENT_LANDSCAPE;
   //   ::GlobalUnlock(pd.hDevMode);
   //}
    }
  m_pDC->GetTextMetrics(&tm);

    // Find out where to start the first line of the table header.
    // First line of Horizontal table header. Distributes average
    // power by frequency region is the longest.
  if(m_pDC->IsPrinting() == 0)
    { // Not printing
    //iRowTitleWidth = m_iLineExt / 4; // Not printing.
    //iX += (iRowTitleWidth + (4 * tm.tmAveCharWidth)); //20;
    iRowTitleWidth = m_iLineExt / 5; // Not printing.
    iX += (iRowTitleWidth + (5 * tm.tmAveCharWidth)); //20;
    }
  else
    { // Printing
#ifndef _200_CPM_HARDWARE
    iRowTitleWidth = m_iLineExt / 4; // Printing.
    //iX += (iRowTitleWidth + (1 * tm.tmAveCharWidth)); //20;
#else
    if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
      iRowTitleWidth = m_iLineExt / 5; // Printing.
    else
      iRowTitleWidth = m_iLineExt / 4; // Printing.
#endif
    iX += (iRowTitleWidth + (1 * tm.tmAveCharWidth)); //20;
    }

  bLastPage = false;
  if(iPage < 0)
    {
    iMaxColumns = 0;
    iStartIndex = 0;
    bLastPage = true;
    }
  else if(iPage == 1)
    { // Set limits for this page.
    if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
      iMaxColumns = 6;
    else
      iMaxColumns = 7;
    iStartIndex = 0;
    if(iTotalPages == iPage)
      bLastPage = true;
    }
  else// if(iPage == 2)
    { // Set limits for this page.
    if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
      iMaxColumns = 6;
    else
      iMaxColumns = 7;
    iStartIndex = iMaxColumns * (iPage - 1);
    if(iTotalPages == iPage)
      bLastPage = true;
    // Use m_iLineExt to determine how many columns go on a page.
    }

    // First format widest column title so we get the max column width.
#ifndef _200_CPM_HARDWARE
  cstrDesc = g_pLang->cstrLoad_string(ITEM_G_375_100CPM);
#else
  if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM || theApp.m_iFreqLimit == FREQ_LIMIT_15_CPM)
    cstrDesc = g_pLang->cstrLoad_string(ITEM_G_375_100CPM);
  else
    cstrDesc.Format("180.0 - 200.0 %s", g_pLang->cstrLoad_string(ITEM_G_CPM));
#endif
  cstrItem1.Format("(%s)", cstrDesc);
  csText1 = m_pDC->GetTextExtent(cstrItem1);
  iWidth = csText1.cx;
  iHeight = csText1.cy;
  iX1 = iX;
    // Create the column starting locations.
  if(m_pDoc->m_pdPatient.uPatientType == PAT_HUMAN)
    { // Human
    iSize = sizeof(s_uDSColTitles1) / sizeof(unsigned);
    iFirstRowColTitles = iSize;
#ifdef _200_CPM_HARDWARE
    if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM
      || theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
      {
      if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
        iSize = MAX_FREQ_BANDS_60_CPM;
      else
        iSize = MAX_FREQ_BANDS_200_CPM;
      iFirstRowColTitles -= 2;  // Last to column titles go at the end.
      }
#endif
    iTotalColumns = iSize;
    puArray = s_uDSColTitles1;
    }
  else
    { // Dog
    iSize = sizeof(s_uDSColTitlesDog1) / sizeof(unsigned);
    puArray = s_uDSColTitlesDog1;
    }
  if(iPage > 0 && iSize > iMaxColumns)
    iSize = iMaxColumns;
  for(iIndex = 0; iIndex < iSize; ++iIndex)
    {
    iCol[iIndex] = iX1;
    iX1 += (iWidth + (1 * tm.tmAveCharWidth)); // Printing
    }
    // Last column starting location is for "Period" and it requires less
    // space, so don't leave as much space between columns.
  iX1 -= (2 * tm.tmAveCharWidth);
  iCol[iIndex] = iX1;

    // First row of column titles
  //for(iIndex = 0; iIndex < iSize; ++iIndex)
  if(iPage < 0 || iPage == 1)
    {
    for(iIndex = 0; iIndex < iFirstRowColTitles; ++iIndex)
      {
      //cstrItem1.LoadString(*(puArray + iIndex)); //s_uDSColTitles1[iIndex]);
      cstrItem1 = cstrGet_col_title(*(puArray + iIndex));
      iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iIndex];
      m_pDC->TextOut(iX1, iY, cstrItem1);
      }
    }
  iYFirstRow = iY;
  iY += iHeight;
    // Second row of column titles
  if(m_pDoc->m_pdPatient.uPatientType == PAT_HUMAN)
    { // Human
    iSize = sizeof(s_uColTitles) / sizeof(unsigned);
    iFirstRowColTitles = iSize;
#ifdef _200_CPM_HARDWARE
    if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM
      || theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
      {
      if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
        iSize = MAX_FREQ_BANDS_60_CPM;
      else
        iSize = MAX_FREQ_BANDS_200_CPM;
      iFirstRowColTitles -= 2;  // Last to column titles go at the end.
      }
#endif
    iTotalColumns = iSize;
    puArray = s_uColTitles;
    }
  else
    { // Dog
    iSize = sizeof(s_uColTitlesDog) / sizeof(unsigned);
    puArray = s_uColTitlesDog;
    }
  if(iPage > 0 && iSize > iMaxColumns)
    iSize = iMaxColumns;
  if(iPage < 0 || iPage == 1)
    {
    for(iIndex = 0; iIndex < iFirstRowColTitles; ++iIndex)
      {
      cstrDesc = cstrGet_col_title(*(puArray + iIndex));
      cstrItem1.Format("(%s)", cstrDesc);
      iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iIndex];
      m_pDC->TextOut(iX1, iY, cstrItem1);
      }
    }

    int iStart, iCnt;
  if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM || theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
    {
    int iColIndex;
    // Add extra column titles for the extra CPM.
    if(iPage < 0 || iPage == 1)
      {
      fCPM = 15.0F;
      fCPMInc = 5.0F;
      iStart = iFirstRowColTitles;
      if(iPage < 0 || iPage == iTotalPages)
        iCnt = iSize - 2;
      else
        iCnt = iSize;
      iColIndex = iFirstRowColTitles;
      }
    else if(iPage >= 2)
      {
      // calculate the  starting CPM.
      fCPM = 15.0F + 5.0F;
      for(int i = 0; i < (iStartIndex - 4 - 1); ++i)
        {
        if(fCPM < 100.0F)
          fCPM += 10.0F; //(float)((iStartIndex - 4 - 1) * 10);
        else
          fCPM += 20.0F; //(float)((iStartIndex - 4 - 1) * 20);
        }
      //fCPM = 15.0F + 5.0F + (float)((iStartIndex - 4 - 1) * 10);
      if(fCPM < 100.0F)
        fCPMInc = 10.0F;
      else
        fCPMInc = 20.0F;
      iStart = iStartIndex;
      iCnt = iStart + iMaxColumns;
      //iTotalColumns -= 6;
      //iCnt = iStartIndex + iSize;
      if(iCnt > iTotalColumns)
        iCnt = iTotalColumns - 2;
      //iCnt += (iStart - 2);
      iColIndex = 0;
      }
    
    //for(iIndex = iFirstRowColTitles; iIndex < (iSize - 2); ++iIndex)
    for(iIndex = iStart; iIndex < iCnt; ++iIndex, ++iColIndex)
      {
      cstrDesc.Format("%.1f - %.1f %s", fCPM, fCPM + fCPMInc, g_pLang->cstrLoad_string(ITEM_G_CPM));
      fCPM += fCPMInc;
      if(fCPM < 100.0F)
        fCPMInc = 10.0F;
      else
        fCPMInc = 20.0F;
      cstrItem1.Format("(%s)", cstrDesc);
      iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex];
      m_pDC->TextOut(iX1, iY, cstrItem1);
      }
    // Extra first row column titles.
    if(bLastPage == true)
      {
      cstrItem1 = cstrGet_col_title(s_uDSColTitles1[iFirstRowColTitles]);
      iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex];
      m_pDC->TextOut(iX1, iYFirstRow, cstrItem1);
      cstrItem1 = cstrGet_col_title(s_uDSColTitles1[iFirstRowColTitles + 1]);
      iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex + 1];
      m_pDC->TextOut(iX1, iYFirstRow, cstrItem1);
      // Extra second row column titles.
      int iMaxFreq;
      if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
        iMaxFreq = 60;
      else
        iMaxFreq = 200;
      cstrItem1.Format("(1 - %d %s)", iMaxFreq, g_pLang->cstrLoad_string(ITEM_G_CPM));
      //cstrItem1.Format("(%s)", cstrGet_col_title(s_uColTitles[iFirstRowColTitles]));
      iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex];
      m_pDC->TextOut(iX1, iY, cstrItem1);
      cstrItem1.Format("(%s)", cstrGet_col_title(s_uColTitles[iFirstRowColTitles + 1]));
      iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex + 1];
      m_pDC->TextOut(iX1, iY, cstrItem1);
      }
    }
  if(m_pDC->IsPrinting() == 0)
    { // Not printing.
      // This is a wide report, so make sure we have the horizontal scrollbar
      // if needed.
      // If we make a horizontal scroll bar, make it just a little longer
      // to make sure we see everything by scrolling.
    csText1 = m_pDC->GetTextExtent(cstrItem1);
    iX1 += csText1.cx;
    if(m_csViewSize.cx < iX1)
      m_csViewSize.cx = iX1 + (int)((float)iX1 * 0.10F); // 10% wider;
    }

  iY += (iHeight * 2);

  if(iPage < 0 || iPage == 1)
    {
    iStart = FREQ_BAND1;
    if(iPage < 0 || iPage == iTotalPages)
      {
      if(theApp.m_iFreqLimit == FREQ_LIMIT_15_CPM)
        iCnt = (int)m_pDoc->m_pAnalyze->m_uSummBand1;
      else
        iCnt = (int)m_pDoc->m_pAnalyze->m_uSummBand1;
      }
    else
      {
      //iCnt = 5;
      iCnt = iMaxColumns - 1;
      }
    }
  else if(iPage >= 2)
    {
    iStart = FREQ_BAND1 + (iMaxColumns * (iPage - 1)); //6;
    //if(iPage == iTotalPages)
    //  iCnt = (int)m_pDoc->m_pAnalyze->m_uSummBand1;
    //else
      iCnt = iStart + iMaxColumns - 1; //6;
      if(iCnt >= iTotalColumns)
        iCnt = iTotalColumns - 2;
    }

    // Distribution of average power by frequency region
  iY1 = iY;
  //cstrDesc.LoadString(IDS_DS_DIST_AVG_PWR_15_50);
  cstrDesc = g_pLang->cstrLoad_string(ITEM_G_DS_DIST_AVG_PWR_15_50);
  iLeftTitleY = iDisplay_multiline(iXStart, iY1, iRowTitleWidth, cstrDesc);
  for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
    { // Go through this loop for each timeband specified.
    iDataIndex = iIndex * theApp.m_iMaxFreqBands; //MAX_FREQ_BANDS;
    iColIndex = 0;
    //for(iIndex1 = FREQ_BAND1; iIndex1 <= FREQ_BAND7; ++iIndex1)
    //for(iIndex1 = FREQ_BAND1; iIndex1 <= (short)m_pDoc->m_pAnalyze->m_uSummBand1; ++iIndex1)
    for(iIndex1 = iStart; iIndex1 <= (short)iCnt; ++iIndex1)
      { // Go through this loop for each frequency band.
//      if(iIndex1 != FREQ_BAND5 && iIndex1 != FREQ_BAND6)
      if(bSkip_datasheet_column(iIndex1) == false)
        {
#ifdef _200_CPM_HARDWARE
        cstrItem1.Format("%3.4f", m_adData.fPctGrp1[iDataIndex + iIndex1]);
#else
        cstrItem1.Format("%3.2f", m_adData.fPctGrp1[iDataIndex + iIndex1]);
#endif
        iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex];
        m_pDC->TextOut(iX1, iY, cstrItem1);
        ++iColIndex;
        }
      }
    if(bLastPage == true)
      {
        // Add the timeband start and end minutes.
      cstrItem1 = cstrGet_timeband_start_end_min(iIndex);
  //    cstrItem1.Format("%.1f to %.1f", m_pDoc->m_pdPatient.fGoodMinStart[iIndex] + (iIndex * 10),
  //                    m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] + (iIndex * 10));
      iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex];
      m_pDC->TextOut(iX1, iY, cstrItem1);
      }
    iY += iHeight;
    }
  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // For research version, the number of lines in the left side title may
      // be more than the number of periods.  If so, we have to position the
      // next Y to past where the left side title ends.
    if(iY < iLeftTitleY)
      iY = iLeftTitleY;
    }
  iY += (iHeight + 2);
    // Ratio of average powers by frequency range.
  iY1 = iY;
  //cstrDesc.LoadString(IDS_DS_RATIO_AVG_PWR);
  cstrDesc = g_pLang->cstrLoad_string(ITEM_G_DS_RATIO_AVG_PWR);
  iLeftTitleY = iDisplay_multiline(iXStart, iY1, iRowTitleWidth, cstrDesc);
  for(iIndex = 1; iIndex < m_uMaxPeriods; ++iIndex)
    { // Go through this loop for each timeband specified.
    iDataIndex = iIndex * theApp.m_iMaxFreqBands; //MAX_FREQ_BANDS;
    iColIndex = 0;
    //for(iIndex1 = FREQ_BAND1; iIndex1 <= FREQ_BAND7; ++iIndex1)
    //for(iIndex1 = FREQ_BAND1; iIndex1 <= (short)m_pDoc->m_pAnalyze->m_uSummBand1; ++iIndex1)
    for(iIndex1 = iStart; iIndex1 <= (short)iCnt; ++iIndex1)
      { // Go through this loop for each frequency band.
//      if(iIndex1 != FREQ_BAND5 && iIndex1 != FREQ_BAND6)
      if(bSkip_datasheet_column(iIndex1) == false)
        {
        cstrItem1.Format("%.2f", *(m_adData.pfRatioAvgPwr + iDataIndex + iIndex1));
        iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex];
        m_pDC->TextOut(iX1, iY, cstrItem1);
        ++iColIndex;
        }
      }
     if(bLastPage == true)
      {
     // Add the timeband start and end minutes.
      cstrItem1 = cstrGet_timeband_start_end_min(iIndex);
  //    cstrItem1.Format("%.1f to %.1f", m_pDoc->m_pdPatient.fGoodMinStart[iIndex] + (iIndex * 10),
  //                    m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] + (iIndex * 10));
      iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex];
      m_pDC->TextOut(iX1, iY, cstrItem1);
      }
    iY += iHeight;
    }
  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // For research version, the number of lines in the left side title may
      // be more than the number of periods.  If so, we have to position the
      // next Y to past where the left side title ends.
    if(iY < iLeftTitleY)
      iY = iLeftTitleY;
    }
  iY += (iHeight + 2);
  /***/

    // Distribution of average power by frequency range
  iY1 = iY;
  //cstrDesc.LoadString(IDS_DS_DIST_AVG_PWR_RNG);
  cstrDesc = g_pLang->cstrLoad_string(ITEM_G_DS_DIST_AVG_PWR_RNG);
  iLeftTitleY = iDisplay_multiline(iXStart, iY1, iRowTitleWidth, cstrDesc);
  for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
    { // Go through this loop for each timeband specified.
    iDataIndex = iIndex * theApp.m_iMaxFreqBands; //MAX_FREQ_BANDS;
    iColIndex = 0;
    //for(iIndex1 = FREQ_BAND1; iIndex1 <= FREQ_BAND7; ++iIndex1)
    //for(iIndex1 = FREQ_BAND1; iIndex1 <= (short)m_pDoc->m_pAnalyze->m_uSummBand1; ++iIndex1)
    for(iIndex1 = iStart; iIndex1 <= (short)iCnt; ++iIndex1)
      { // Go through this loop for each frequency band.
//      if(iIndex1 != FREQ_BAND5 && iIndex1 != FREQ_BAND6)
      if(bSkip_datasheet_column(iIndex1) == false)
        {
        cstrItem1.Format("%.4e", m_adData.fAvgPwr[iDataIndex + iIndex1]);
        m_pDC->TextOut(iCol[iColIndex], iY, cstrItem1);
        ++iColIndex;
        }
      }
    if(bLastPage == true)
      {
        // Add the timeband start and end minutes.
      cstrItem1 = cstrGet_timeband_start_end_min(iIndex);
  //    cstrItem1.Format("%.1f to %.1f", m_pDoc->m_pdPatient.fGoodMinStart[iIndex] + (iIndex * 10),
  //                     m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] + (iIndex * 10));
      iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex];
      m_pDC->TextOut(iX1, iY, cstrItem1);
      }
    iY += iHeight;
    }
  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // For research version, the number of lines in the left side title may
      // be more than the number of periods.  If so, we have to position the
      // next Y to past where the left side title ends.
    if(iY < iLeftTitleY)
      iY = iLeftTitleY;
    }
  iY += (iHeight + 2);

    // Average dominant frequency.
    // The average dominant frequency for each time band
    // falls in one frequency range.  The other frequency
    // ranges for the time band are blank.
  iY1 = iY;
  //cstrDesc.LoadString(IDS_DS_AVG_DOM_FREQ);
  cstrDesc = g_pLang->cstrLoad_string(ITEM_G_DS_AVG_DOM_FREQ);
  iLeftTitleY = iDisplay_multiline(iXStart, iY1, iRowTitleWidth, cstrDesc);
  iDataIndex = 0;
  for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
    { // Go through this loop for each timeband specified.
    uNumOfPoints = uGet_num_epochs(m_pDoc->m_pdPatient.fGoodMinStart[iIndex],
      m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]);
    //uNumOfPoints = ((unsigned)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
    //                - (unsigned)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 3);
    fStdDev = fStd_dev(uNumOfPoints, m_adData.pfDomFreq + iDataIndex, &fMean);
    cstrItem1.Format("%.2f (%.2f)", fMean, fStdDev);
    iDataIndex += uNumOfPoints;
    if(m_pDoc->m_pdPatient.uPatientType == PAT_HUMAN)
      { // Human
#ifndef _200_CPM_HARDWARE
      if(fMean < H_FREQ_BAND2_LOW_CPM)
        iIndex1 = 0;
      else if(fMean < H_FREQ_BAND3_LOW_CPM)
        iIndex1 = 1;
      else if(fMean < H_FREQ_BAND4_LOW_CPM)
        iIndex1 = 2;
      else
        iIndex1 = 3;
#else
      float fFreqBandLowCPM[17];
      int iMaxIndex;
      fFreqBandLowCPM[0] = H_FREQ_BAND2_LOW_CPM;
      fFreqBandLowCPM[1] = H_FREQ_BAND3_LOW_CPM;
      fFreqBandLowCPM[2] = H_FREQ_BAND4_LOW_CPM;
      fFreqBandLowCPM[3] = H_FREQ_BAND5_LOW_CPM;
      fFreqBandLowCPM[4] = H_FREQ_BAND6_LOW_CPM;
      fFreqBandLowCPM[5] = H_FREQ_BAND7_LOW_CPM;
      fFreqBandLowCPM[6] = H_FREQ_BAND8_LOW_CPM;
      fFreqBandLowCPM[7] = H_FREQ_BAND9_LOW_CPM;
      fFreqBandLowCPM[8] = H_FREQ_BAND10_LOW_CPM;
      fFreqBandLowCPM[9] = H_FREQ_BAND11_LOW_CPM;
      fFreqBandLowCPM[10] = H_FREQ_BAND12_LOW_CPM;
      fFreqBandLowCPM[11] = H_FREQ_BAND13_LOW_CPM;
      fFreqBandLowCPM[12] = H_FREQ_BAND14_LOW_CPM;
      fFreqBandLowCPM[13] = H_FREQ_BAND15_LOW_CPM;
      fFreqBandLowCPM[14] = H_FREQ_BAND16_LOW_CPM;
      fFreqBandLowCPM[15] = H_FREQ_BAND17_LOW_CPM;
      fFreqBandLowCPM[16] = H_FREQ_BAND18_LOW_CPM;
      if(theApp.m_iFreqLimit == FREQ_LIMIT_15_CPM)
        iMaxIndex = 3;
      else if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
        iMaxIndex = 8;
      else
        iMaxIndex = 17;
      for(iIndex1 = 0; iIndex1 < iMaxIndex; ++iIndex1)
        {
        if(fMean < fFreqBandLowCPM[iIndex1])
          break;
        }

#endif
      }
    else
      { // Dog
      if(fMean < D_FREQ_BAND3_LOW_CPM)
        iIndex1 = 0;
      else if(fMean < D_FREQ_BAND4_LOW_CPM)
        iIndex1 = 1;
      else 
        iIndex1 = 2;
      }
    // Make sure this is on the correct page.
    if(iPage < 0 || (iIndex1 >= ((iPage - 1) * 6) && iIndex1 < (iPage * 6)))
      {
      iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iIndex1];
      m_pDC->TextOut(iX1, iY, cstrItem1);
     }

    //bool bTextOut = false;
    //if(iPage < 0 || (iPage == 1 && bLastPage == true))
    //  bTextOut = true;
    //else if (iPage == 2)
    //  {
    //  if(iColIndex >= 6)
    //    bTextOut = true;
    //  }
    //if(bTextOut == true)
    //  {
    //  iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex];
    //  m_pDC->TextOut(iX1, iY, cstrItem1);
    //  }
    if(bLastPage)
      {
        // Add the timeband start and end minutes.
      cstrItem1 = cstrGet_timeband_start_end_min(iIndex);
  //    cstrItem1.Format("%.1f to %.1f", m_pDoc->m_pdPatient.fGoodMinStart[iIndex] + (iIndex * 10),
  //                     m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] + (iIndex * 10));
      if(m_pDoc->m_pdPatient.uPatientType == PAT_HUMAN)
        iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex]; //m_pDoc->m_pAnalyze->m_iNumFreqBands + 1]; //NUM_FREQ_BANDS - 1];
      else
        iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex]; //m_pDoc->m_pAnalyze->m_iNumFreqBands + 1]; //NUM_FREQ_BANDS - 2];
      m_pDC->TextOut(iX1, iY, cstrItem1);
      }
    iY += iHeight;
    }
  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // For research version, the number of lines in the left side title may
      // be more than the number of periods.  If so, we have to position the
      // next Y to past where the left side title ends.
    if(iY < iLeftTitleY)
      iY = iLeftTitleY;
    }
  iY += (iHeight + 2);

    // Percentage time with dominant frequency.
  iY1 = iY;
  //cstrDesc.LoadString(IDS_DS_PCT_DOM_FREQ);
  cstrDesc = g_pLang->cstrLoad_string(ITEM_G_DS_PCT_DOM_FREQ);
  iLeftTitleY = iDisplay_multiline(iXStart, iY1, iRowTitleWidth, cstrDesc);
  for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
    { // Go through this loop for each timeband specified.
    uNumOfPoints = uGet_num_epochs(m_pDoc->m_pdPatient.fGoodMinStart[iIndex],
      m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]);
    //uNumOfPoints = ((unsigned)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
    //                - (unsigned)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 3);
    //unsigned int uNumHalfMins = (unsigned int)(m_pDoc->m_pdPatient.fGoodMinStart[iIndex] / 0.5F);
    //if((uNumHalfMins % 2) != 0)
    //  --uNumOfPoints;
    iDataIndex = iIndex * theApp.m_iMaxFreqBands; //MAX_FREQ_BANDS;
    iColIndex = 0;
    //for(iIndex1 = FREQ_BAND1; iIndex1 <= FREQ_BAND4; ++iIndex1)
    //for(iIndex1 = FREQ_BAND1; iIndex1 <= (short)m_pDoc->m_pAnalyze->m_uSummBand1 - 1; ++iIndex1)
    for(iIndex1 = iStart; iIndex1 <= (short)(iCnt - 1); ++iIndex1)
      {
      if(bSkip_datasheet_column(iIndex1) == false)
        {
        fPct = *(m_adData.pfPctDomFreq + iDataIndex + iIndex1)
               / (float)uNumOfPoints;
        cstrItem1.Format("%.0f  %%", fPct * (float)100.0);
        iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex];
        m_pDC->TextOut(iX1, iY, cstrItem1);
        ++iColIndex;
        }
      }
    if(bLastPage == true)
      {
        // Add the timeband start and end minutes.
      cstrItem1 = cstrGet_timeband_start_end_min(iIndex);
  //    cstrItem1.Format("%.1f to %.1f", m_pDoc->m_pdPatient.fGoodMinStart[iIndex] + (iIndex * 10),
  //                     m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] + (iIndex * 10));
      if(m_pDoc->m_pdPatient.uPatientType == PAT_HUMAN)
        iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex + 1]; //NUM_FREQ_BANDS - 1];
      else
        iX1 = iCenter_text(cstrItem1, iWidth) + iCol[iColIndex + 1]; //NUM_FREQ_BANDS - 2];
      m_pDC->TextOut(iX1, iY, cstrItem1);
      }
    iY += iHeight;
    }

  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // For research version, display/print stimulation medium
      // Title
    iX = iXStart + (4 * tm.tmAveCharWidth);
    iY += (iHeight + 2);
    //cstrItem1.LoadString(IDS_STIMULATION_MEDIUM);
    cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_STIMULATION_MEDIUM);
    m_pDC->TextOut(iXStart, iY, cstrItem1);
    iY += csText1.cy;
      // Stimulation medium
    if(m_pDoc->m_pdPatient.cstrStimMedDesc != "")
      {
      m_pDC->TextOut(iX, iY, m_pDoc->m_pdPatient.cstrStimMedDesc);
      iY += csText1.cy;
        // Amount and units of stimulation medium.
      cstrItem1.Format("%u %s", m_pDoc->m_pdPatient.uWaterAmount,
                       m_pDoc->m_pdPatient.cstrWaterUnits);
      }
    else
      { // No stimulation medium.
      //cstrItem1.LoadString(IDS_NO_STIM_MEDIUM);
      cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_NO_STIM_MEDIUM);
      }
    m_pDC->TextOut(iX, iY, cstrItem1);
    iY += (2 * csText1.cy);
    }

  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // For research version, the number of lines in the left side title may
      // be more than the number of periods.  If so, we have to position the
      // next Y to past where the left side title ends.
    if(iY < iLeftTitleY)
      iY = iLeftTitleY;
    }
  iY += (iHeight + 2);
/***/

  return(iY);
  }

/********************************************************************
bSkip_datasheet_column - Determine if datashed column should be displayed
                         or skipped.  Takes into consideration both human
                         and dog.

  inputs: Datasheet column index.

  return: true if datasheet column should be skipped, otherwise false.
********************************************************************/
bool CReportView::bSkip_datasheet_column(int iColIndex)
  {
  bool bSkip = false;

  ////if(iColIndex == FREQ_BAND5 || iColIndex == FREQ_BAND6)
    ////bSkip = true;
  if(m_pDoc->m_pdPatient.uPatientType == PAT_DOG)
    {
    if(iColIndex == FREQ_BAND1)
      bSkip = true;
    }
  return(bSkip);
  }

/********************************************************************
iCreate_RSA - Draw the RSA graph.

              Modified to draw 2 graphs is specified in the report configuration.
              If 2 graphs, first grapht is alway all frequencies (unfiltered),
              and the second graph is filtered.

  inputs: X coordinate of where to start the graph.
          Y coordinate of where to start the graph.
          Graph number: 1 for one graph or the first of 2
                        2 for a second graph

  return: Y coordinate of the bottom of the graph.
********************************************************************/
int CReportView::iCreate_RSA(int iXStart, int iYStart, int iGraphNum)
  {
  CDC *pDC;
  CWnd *pcwndRSA;
  CGraphRSA *pGraphRsa = NULL;
  int iYEnd, iOldMapMode, iRSAGraphWidth;


    // Calculate RSA graph size and position.
    // For width, leave ~10% on each side.
    // Make the graph square.
  iRSAGraphWidth = m_iRightWidth; // - (int)((float)m_iRightWidth * 0.2F);
  iYEnd = iYStart + m_iRightWidth - (int)((float)m_iRightWidth * 0.3F);
  int iScreenHeight = m_pDC->GetDeviceCaps(VERTRES);
  iYEnd = iYStart + (int)((float)m_iRightWidth * 0.45F);
  if(m_pDC->IsPrinting() != 0)
    {
    if(iYEnd > iScreenHeight)
      iYEnd = iScreenHeight;
    }

  m_rsaData.rWin.left = iXStart; // + (int)((float)m_iRightWidth * 0.10F);
  m_rsaData.rWin.right = m_rsaData.rWin.left + iRSAGraphWidth;
  m_rsaData.rWin.top = iYStart;
  m_rsaData.rWin.bottom = iYEnd;

    // If the RSA graph has moved to a different Y coordinate delete
    // the RSA graph windows so that we will recreate it later.
  if(iGraphNum == 1)
    {
    if(m_iRSAGraphStart != iYStart)
      {
      if(m_pcwndRSA != NULL)
        {
        delete m_pcwndRSA;
        m_pcwndRSA = NULL;
        }
      m_pcwndRSA = new CWnd();
      m_pcwndRSA->Create(NULL, NULL, 0/*WS_VISIBLE | WS_CHILD*/, m_rsaData.rWin, this, IDR_MAINFRAME);
      }
    m_iRSAGraphStart = iYStart;
    pcwndRSA = m_pcwndRSA;
    }
  else
    {
    if(m_iRSAGraphStart1 != iYStart)
      {
      if(m_pcwndRSA1 != NULL)
        {
        delete m_pcwndRSA1;
        m_pcwndRSA1 = NULL;
        }
      m_pcwndRSA1 = new CWnd();
      m_pcwndRSA1->Create(NULL, NULL, 0/*WS_VISIBLE | WS_CHILD*/, m_rsaData.rWin, this, IDR_MAINFRAME);
      }
    m_iRSAGraphStart1 = iYStart;
    pcwndRSA = m_pcwndRSA1;
    }

  if(m_pDC->IsPrinting() == 0)
    { // Not printing
    //if((iGraphNum == 1 && m_pDC->RectVisible(&m_rsaData.rWin) != FALSE) || iGraphNum == 2)
    if(m_pDC->RectVisible(&m_rsaData.rWin) == TRUE)
      {
      //if(iGraphNum == 1)
      //  {
      //  //if(m_pcwndRSA == NULL)
      //  //  {
      //  //  m_pcwndRSA = new CWnd();
      //  //  m_pcwndRSA->Create(NULL, NULL, 0/*WS_VISIBLE | WS_CHILD*/, m_rsaData.rWin, this, IDR_MAINFRAME);
      //  //  }
      //  pcwndRSA = m_pcwndRSA;
      //  }
      //else
      //  {
      //  //if(m_pcwndRSA1 == NULL)
      //  //  {
      //  //  m_pcwndRSA1 = new CWnd();
      //  //  m_pcwndRSA1->Create(NULL, NULL, 0/*WS_VISIBLE | WS_CHILD*/, m_rsaData.rWin, this, IDR_MAINFRAME);
      //  //  }
      //  pcwndRSA = m_pcwndRSA1;
      //  }
      //if(pcwndRSA != NULL
      //&& m_pDC->RectVisible(&m_rsaData.rWin) != FALSE)
      if(pcwndRSA != NULL)
        { // Create a window to display the RSA.  With out a window, it doesn't
          // get display or scrolled properly
        pDC = pcwndRSA->GetDC();
        pcwndRSA->GetClientRect(&m_rsaData.rWin);
      
        pGraphRsa = new CGraphRSA();
        pGraphRsa->vDraw_graph(pDC, &m_rsaData, m_pDoc);
      
        pcwndRSA->ReleaseDC(pDC);
        pcwndRSA->ShowWindow(SW_SHOW);
        }
      }
    }
  else
    { // Printing
    iOldMapMode = m_pDC->SetMapMode(MM_ANISOTROPIC);

    pGraphRsa = new CGraphRSA();
    pGraphRsa->vDraw_graph(m_pDC, &m_rsaData, m_pDoc);

    m_pDC->SetMapMode(iOldMapMode); // Put the original map mode back.
    }
  if(pGraphRsa != NULL)
    delete pGraphRsa;
  return(iYEnd);
  }

/********************************************************************
iCreate_sel_raw_data_graphs - Graph the raw data for the selected good
                              minutes.  1 graph per period.

  inputs: X coordinate of where to start the graph.
          Y coordinate of where to start the graph.
          Page number the graphs start on

  return: Y coordinate of the bottom of the graph.
********************************************************************/
int CReportView::iCreate_sel_raw_data_graphs(int iXStart, int iYStart, short int *piPage)
  {
  HWND hPEGrph;
  RECT rGrphWin;
  CSize csText1;
  CString cstrTitle, cstrBaseText;
  GRAPH_INFO gi;
  bool bDisplayGraph;
  int iHeight, iWidth, iYTitle, iY, iLastY;
  long lDataOffset;
  unsigned short uIndex, uNumResearchGraphs;

    // If the summary graphs have moved to a different Y coordinate delete
    // the summary graph windows so that we will recreate them later.
  if(m_iSelRawDataGraphStartY != iYStart)
    {
    for(uIndex = 0; uIndex < m_uMaxPeriods; ++uIndex)
      {
      if(m_hPESelRawData[uIndex] != NULL)
        {
        PEdestroy(m_hPESelRawData[uIndex]);
        m_hPESelRawData[uIndex] = NULL;
        }
      }
    m_iSelRawDataGraphStartY = iYStart;
    }

  if(m_pDC->IsPrinting() == 0)
    {
    GetClientRect(&rGrphWin); // Not printing
    iLastY = rGrphWin.bottom;
    }
  else
    {
    rGrphWin.right = m_iLineExt; // Printing
    iLastY = m_pDC->GetDeviceCaps(LOGPIXELSY);
    }

  rGrphWin.top = iYStart;
//  rGrphWin.right -= rGrphWin.left; //(iXStart * 2);
  if(m_pDC->IsPrinting() == 0)
    { // not printing.
    rGrphWin.left = iXStart * 2;
    if(m_iRawDataGraphWidth == 0)
      m_iRawDataGraphWidth = rGrphWin.right - rGrphWin.left - (iXStart * 2);
    iHeight = m_iRawDataGraphWidth / 4;
    iWidth = m_iRawDataGraphWidth;
    }
  else
    { // Printing. Calculate the graph height.
    rGrphWin.left = iXStart * 4;
    iWidth = iCalc_print_graph_width(rGrphWin.left);
    iHeight = iCalc_print_graph_height(iYStart);
    }
  rGrphWin.right = rGrphWin.left + iWidth;
  rGrphWin.bottom = rGrphWin.top + iHeight;
//  rGrphWin.bottom = rGrphWin.top + (rGrphWin.right / 4); //200;

    // Find out if the graphs will actually be visible. This only applies
    // if we are not printing.  If printing, the graphs will always be
    // visible.
  if((m_pDC->IsPrinting() == 0 && m_pDC->RectVisible(&rGrphWin) != 0)
  || m_pDC->IsPrinting() != 0)
    bDisplayGraph = true; // Graph visible.
  else
    bDisplayGraph = false; // Graph not visible.

  gi.uAttrib = MINUTES_FRACTIONS;
  gi.bAddXAxisLabel = true;
  uNumResearchGraphs = 0;
  for(gi.uPeriod = 0; gi.uPeriod < m_uMaxPeriods; ++gi.uPeriod)
    {
    if(theApp.m_ptStudyType == PT_RESEARCH)
      { // For research study, label the periods.
      if(gi.uPeriod == PERIOD_BL)
        cstrTitle = g_pLang->cstrLoad_string(ITEM_G_BASELINE);
        //cstrTitle.LoadString(IDS_BASELINE);
      else
        {
        //cstrBaseText.LoadString(IDS_POST_STIM_PERIOD);
        cstrBaseText = g_pLang->cstrLoad_string(ITEM_G_POST_STIM_PERIOD);
        cstrTitle.Format("%s %d", cstrBaseText, gi.uPeriod);
        }
        // Update where on the screen the graph for the period starts.
      csText1 = m_pDC->GetTextExtent(cstrTitle);
      iYTitle = rGrphWin.top;
      rGrphWin.top += (csText1.cy * 2);
      rGrphWin.bottom = rGrphWin.top + iHeight;
      m_pDC->TextOut(iXStart, iYTitle, cstrTitle);
      }
    if(m_hPESelRawData[gi.uPeriod] == NULL)
      m_hPESelRawData[gi.uPeriod] = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rGrphWin, m_hWnd, IDR_MAINFRAME);
    if(bDisplayGraph == true)
      { // Either not printing and window is visible or printing.
      hPEGrph = m_hPESelRawData[gi.uPeriod];
      gi.fStartMin = m_pDoc->m_pdPatient.fGoodMinStart[gi.uPeriod];
      gi.fMinOnGraph = m_pDoc->m_pdPatient.fGoodMinEnd[gi.uPeriod] - gi.fStartMin;
      if(theApp.m_ptStudyType == PT_RESEARCH)
        { // Research study.
          // The start min for graphing is offset from the period start.
        gi.fStartMin -= m_pDoc->m_pdPatient.fPeriodStart[gi.uPeriod];
        }
      gi.iNumDataPoints = (short int)(gi.fMinOnGraph * (float)DATA_POINTS_PER_MIN_1);
        // Make the graph annotation for the start minute based on the period.
//      gi.fGraphStartMin = gi.fStartMin + (float)(gi.uPeriod * MIN_PER_PERIOD);
      gi.fGraphStartMin = gi.fStartMin + m_pDoc->fGet_period_start_min(gi.uPeriod);
        // Plot the points.
      if(m_pDoc->iRead_period(gi.uPeriod, m_pDoc->m_fData) == SUCCESS)
        {
        gi.fMinEGG = m_pDoc->m_pdPatient.fMinEGG;
        gi.fMaxEGG = m_pDoc->m_pdPatient.fMaxEGG;
        gi.fMinResp = m_pDoc->m_pdPatient.fMinResp;
        gi.fMaxResp = m_pDoc->m_pdPatient.fMaxResp;
        lDataOffset = (long)(gi.fStartMin * (float)(DATA_POINTS_PER_MIN_1 * MAX_CHANNELS));
        if((lDataOffset % 2) != 0)
          --lDataOffset; // Data offset is odd, EGG points start on even offsets.
        gi.pfData =  m_pDoc->m_fData + lDataOffset;
//                     (long)(gi.fStartMin * (float)(DATA_POINTS_PER_MIN_1 * MAX_CHANNELS));
          // Draw the graph.
        m_pDoc->vDraw_raw_data_graph(hPEGrph, &gi);
        if(m_pDC->IsPrinting() != 0)
          vPrint_graph(hPEGrph, &rGrphWin);
        }
      } // end of display graph == true

    if(theApp.m_ptStudyType == PT_RESEARCH)
      { // Research Study
      if(uNumResearchGraphs == 2)
        { // Done one page, go to next page.
        uNumResearchGraphs = 0;
        iY = iEnd_of_page(iXStart, rGrphWin.bottom, *piPage); // End of page processing.
        ++(*piPage);
        rGrphWin.top = iY;
        }
      else
        {
        rGrphWin.top = rGrphWin.bottom + 10;
        ++uNumResearchGraphs;
        }
      iLastY = rGrphWin.bottom;
      rGrphWin.bottom = rGrphWin.top + iHeight;
      }
    else
      { // Standard study.
      if(gi.uPeriod < (m_uMaxPeriods - 1))
        { // Don't update the graph rectangle after the last graph is displayed.
        rGrphWin.top = rGrphWin.bottom + 10;
        rGrphWin.bottom = rGrphWin.top + iHeight; //(rGrphWin.right / 4); //200;
        }
      }
    } // End of for each period.
  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Research Study.
      // If last page didn't contain a full page of graphs, do last page processing.
    if(uNumResearchGraphs <= 2 && uNumResearchGraphs > 0)// && bDisplayGraph == true)
      { // Done one page, go to next page.
      iY = iEnd_of_page(iXStart, iLastY, *piPage); // End of page processing.
      ++(*piPage);
      }
    rGrphWin.bottom = iY;
    }
  return(rGrphWin.bottom);
  }

/********************************************************************
vAnalyze_data_for_RSA - Analyze the data to get the results needed for
                        the RSA.

  inputs: None.

  return: None.
********************************************************************/
void CReportView::vAnalyze_data_for_RSA()
  {
  ANALYZED_DATA adData;
  REQ_ANALYZE_DATA reqAnlData;
  short int iIndex, iIndex1;
  unsigned short uSize;
  float *pfData, fLastMinute, fStartMinute;
  bool bAddZeroLines;

    // Read the INI file for the channel to analyze.
  m_pDoc = (CEGGSASDoc *)GetDocument();
  adData.pfRawData = NULL;
    // First find out how many data points will be needed and allocate an array.
    // total CPM * 4 so that we get four points for each cpm.
  m_rsaData.iNumY = 0; // Maximum number of graphs.
  m_rsaData.iNumX = m_pDoc->m_pAnalyze->m_iMaxFreqPoints; //((15 - 0) * 4) + 1; // Number of frequency points.
  uSize = 0;
  for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
    {
    if(m_pDoc->m_pdPatient.fGoodMinStart[iIndex] != INV_MINUTE)
      { // Now find out how many data points for this period.
        // Add and extra 2 for the blanks lines separating pre and post water periods.
      uSize += (((unsigned short)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
                - (unsigned short)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 2 + 2)
                * m_rsaData.iNumX);
      }
    }
  if(m_rsaData.pfData != NULL)
    {
    HeapFree(GetProcessHeap(), 0, (LPVOID)m_rsaData.pfData);
    m_rsaData.pfData = NULL;
    }
  if((m_rsaData.pfData = (float *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
  uSize * sizeof(float))) != NULL)
    {
    pfData = m_rsaData.pfData;
    bAddZeroLines = false;
      // For each period that has good minutes defined.
    for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
      {
      if(m_pDoc->m_pdPatient.fGoodMinStart[iIndex] != INV_MINUTE)
        { // Now for each valid period, analyze all the 4 minute epochs.
        // Read in and graph the baseline period.
        if(m_pDoc->iRead_period(iIndex, m_pDoc->m_fData) == SUCCESS)
          {
          if(bAddZeroLines == true)
            { // Add 2 lines of 0's to separate pre and post water periods.
            for(iIndex1 = 0; iIndex1 < (m_rsaData.iNumX * 2); ++iIndex1)
              {
              *pfData = (float)0.0;
              ++pfData;
              }
            m_rsaData.iNumY += 2;
            bAddZeroLines = false;
            }
          reqAnlData.iChannel = 0; // EGG channel.
          reqAnlData.iNumChannelsInFile = 2;
          reqAnlData.iType = RSA_TYPE;
          reqAnlData.padData = &adData;
          reqAnlData.bUsePartMinAtDataEnd = false;
          fLastMinute = m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] - 4;
          fStartMinute = m_pDoc->m_pdPatient.fGoodMinStart[iIndex];
          while(fStartMinute <= fLastMinute)
            {
            if(theApp.m_ptStudyType == PT_STANDARD)
              reqAnlData.fEpochNum = fStartMinute; // Standard study
            else // Research study
              reqAnlData.fEpochNum = fStartMinute - m_pDoc->m_pdPatient.fPeriodStart[iIndex];
            reqAnlData.uRunningEpochCnt = 0;
            if(m_pDoc->m_pAnalyze->bAnalyze_data(&reqAnlData) == true)
              { // Save the data for graphing.
              memcpy((unsigned char *)pfData, adData.fData, m_rsaData.iNumX * sizeof(float));
              pfData += m_rsaData.iNumX;
              ++m_rsaData.iNumY;
              }
            else
              break;
            ++fStartMinute;
            }
          if(iIndex == PERIOD_BL)
            bAddZeroLines = true;
          }
        else
          break;
        }
      }
    }
  }

/********************************************************************
iAdd_facility - Get the facility information and add it to the report.

  inputs: X coordinate of where to start the facility information.
          Y coordinate of where to start the facility information.

  return: Y coordinate of the last line of the facility information.
********************************************************************/
int CReportView::iAdd_facility(int iXStart, int iYStart)
  {
  CString cstrItem1;
  CSize csText1;
  FACILITY_INFO fi;
  int iX, iY;

  iY = iYStart;
  fi.lFacilityID = INV_LONG;
  csText1 = m_pDC->GetTextExtent("A");
  // First check for a logo and if one, display/print it.
  if(g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_LOGO) == true)
    { // Logo is supposed to be in report.
    CImage Img;
    int iWidth, iHeight;
    
    cstrItem1 = g_pConfig->cstrGet_item(CFG_LOGO);
    Img.Load(cstrItem1); // Load the image.
    // Center the image.
    iWidth = 200; // Img.GetWidth()
    // If image width is smaller that default, set it to the smaller size.
    if(iWidth > Img.GetWidth())
      iWidth = Img.GetWidth();
    iHeight = 200; // Img.GetHeight()
    if(iHeight > Img.GetHeight())
      iHeight = Img.GetHeight();
      // Center the image.
    iX = (m_iLineExt - iWidth) / 2;
    // Display/print the imagel
    Img.StretchBlt(m_pDC->m_hDC, iX, iYStart, iWidth, iHeight);
    iY += iHeight;
    }
  if(m_pDoc->m_pDB->iGet_facility_info(&fi) == SUCCESS)
    { // Have facility information, display it.
    if(fi.cstrName.IsEmpty() == FALSE
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_NAME) == true)
    {
      iX = iCenter_text(fi.cstrName, m_iLineExt); //m_csViewSize.cx);
      m_pDC->TextOut(iX, iY, fi.cstrName);
      //csText1 = m_pDC->GetTextExtent(fi.cstrName);
      iY += csText1.cy;
    }
    if(fi.cstrAddr1.IsEmpty() == FALSE
      && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_ADDR1) == true)
      {
      iX = iCenter_text(fi.cstrAddr1, m_iLineExt); //m_csViewSize.cx);
      m_pDC->TextOut(iX, iY, fi.cstrAddr1);
      iY += csText1.cy;
      }
    if(fi.cstrAddr2.IsEmpty() == FALSE
      && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_ADDR2) == true)
      {
      iX = iCenter_text(fi.cstrAddr2, m_iLineExt); //m_csViewSize.cx);
      m_pDC->TextOut(iX, iY, fi.cstrAddr2);
      iY += csText1.cy;
      }
      // On one line, city, state zipcode
    cstrItem1 = "";
    if(fi.cstrCity.IsEmpty() == FALSE
      && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_CITY) == true)
      cstrItem1 = fi.cstrCity;
    if(fi.cstrState.IsEmpty() == FALSE
      && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_STATE) == true)
    {
      if(cstrItem1 != "")
        cstrItem1 += (", " + fi.cstrState);
      else
        cstrItem1 = fi.cstrState;
    }
    if(fi.cstrZipcode.IsEmpty() == FALSE
      && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_ZIP_CODE) == true)
    {
      if(cstrItem1 != "")
        cstrItem1 += (" " + fi.cstrZipcode);
      else
        cstrItem1 = fi.cstrState;
    }
    if(cstrItem1 != "")
    {
      iX = iCenter_text(cstrItem1, m_iLineExt); //m_csViewSize.cx);
      m_pDC->TextOut(iX, iY, cstrItem1);
    }
    iY += csText1.cy;
      //{
      //cstrItem1.Format("%s, %s %s", fi.cstrCity, fi.cstrState, fi.cstrZipcode);
      //iX = iCenter_text(cstrItem1, m_iLineExt); //m_csViewSize.cx);
      //m_pDC->TextOut(iX, iY, cstrItem1);
      //iY += csText1.cy;
      //}
    if(fi.cstrPhone.IsEmpty() == FALSE
      && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_PHONE_NUM) == true)
      {
      iX = iCenter_text(fi.cstrPhone, m_iLineExt); //m_csViewSize.cx);
      m_pDC->TextOut(iX, iY, fi.cstrPhone);
      iY += csText1.cy;
      }
    if(iY != iYStart)
      iY += csText1.cy; // Added at least one line of facility info.
    }
  return(iY);
  }

/********************************************************************
iAdd_title - Add a title, "Electrogastrography Report"

             Put it in a larger font and make it bold.

  inputs: X coordinate of where to start.
          Y coordinate of where to start.

  return: Y coordinate of the last line used.
********************************************************************/
int CReportView::iAdd_title(int iXStart, int iYStart)
  {
  CFont cfFont;
  int iX, iY, iDCSave;
  CString cstrItem;
  CSize csText1;
  
  iY = iYStart;
  // Save the DC so it can be put back after the title.  We do this
  // because we are changing the font.
  iDCSave = ::SaveDC(m_pDC->m_hDC);
  // Create a large, bold font.
  vSet_font(NULL, m_pDC, &cfFont, FW_BOLD, 18);
  m_pDC->SelectObject(&cfFont);
  
  cstrItem = g_pLang->cstrLoad_string(ITEM_G_EGG_RPT_TITLE); 
  csText1 = m_pDC->GetTextExtent(cstrItem);
  // Skip a line before the title.
  iY += csText1.cy;
  // Center the title.
  iX = iCenter_text(cstrItem, m_iLineExt);
  // Display/print the title.
  m_pDC->TextOut(iX, iYStart, cstrItem);
  // Skip a line after the title.
  iY += (2 * csText1.cy);
  // Restore the DC (put back the original font.
  if(iDCSave > 0)
    ::RestoreDC(m_pDC->m_hDC, iDCSave);
  return(iY);
  }

/********************************************************************
iAdd_left_side - Put in the left side of the report.
                   Patient Name
                   Study date
                   Physician and technician names
                   Indications
                   Diagnosis
                   Recommendations
                   Signature

  inputs: X coordinate of where to start the left side.
          Y coordinate of where to start the left side.

  return: Y coordinate of the last line of the left side.
********************************************************************/
int CReportView::iAdd_left_side(int iXStart, int iYStart)
  {
  CString cstrItem1, cstrTemp;
  CSize csText1;
  TEXTMETRIC tm;
  POINT pt;
  CFont cfFont;
  int iX, iY, iDCSave;
  bool bSkipLine;
  bool bHideStudyDate;

  if(m_pDC->IsPrinting() != 0)
    { // Printing.  Use a smaller font.
      // Save the DC so we can easily restore the font.
    iDCSave = ::SaveDC(m_pDC->m_hDC);
    vSet_font(NULL, m_pDC, &cfFont, FW_THIN, 10);
    m_pDC->SelectObject(&cfFont);
    }
  iY = iYStart;
  m_pDC->GetTextMetrics(&tm);
  iX = iXStart + (4 * tm.tmAveCharWidth);
  // Set the height of the text.
  csText1 = m_pDC->GetTextExtent("A");
  
  if(m_iIncludeCoreReport != 0 || m_iIncludePatientInfo != 0)
    {
      // Patient Name
    cstrItem1 = "";
    if(m_pDoc->m_pdPatient.bUseResrchRefNum == TRUE)
    {
      if(g_pConfig->bGet_inc_rpt_item(INC_RPT_RSRCH_REF_NUM) == true)
        {
        cstrItem1.Format("%s: %s", g_pLang->cstrLoad_string(ITEM_G_REF_NUM),
          m_pDoc->m_pdPatient.cstrResearchRefNum);
        //cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_REF_NUM);
        m_pDC->TextOut(iXStart, iY, cstrItem1);
        iY += csText1.cy;
        }
    }
    else
    {
      if(g_pConfig->bGet_inc_rpt_item(INC_RPT_PATIENT_FIRST_NAME) == true
        || g_pConfig->bGet_inc_rpt_item(INC_RPT_PATIENT_MI) == true
        || g_pConfig->bGet_inc_rpt_item(INC_RPT_PATIENT_LAST_NAME) == true)
        {
        vFormat_patient_name(&m_pDoc->m_pdPatient, cstrItem1, true);
        m_pDC->TextOut(iXStart, iY, cstrItem1);
        iY += csText1.cy;

        if(g_pConfig->bGet_inc_rpt_item(INC_RPT_ADDR1) == true)
          {
          m_pDC->TextOut(iXStart, iY, m_pDoc->m_pdPatient.cstrAddress1);
          iY += csText1.cy;
          }
        if(g_pConfig->bGet_inc_rpt_item(INC_RPT_ADDR2) == true)
          {
          m_pDC->TextOut(iXStart, iY, m_pDoc->m_pdPatient.cstrAddress2);
          iY += csText1.cy;
          }
        // Format city, state, zipcode on one line.
        cstrItem1 = "";
        if(g_pConfig->bGet_inc_rpt_item(INC_RPT_CITY) == true)
          {
          cstrItem1 = m_pDoc->m_pdPatient.cstrCity;
          }
        if(g_pConfig->bGet_inc_rpt_item(INC_RPT_STATE) == true)
          {
          if(cstrItem1 != "")
            cstrItem1 += ", ";
          cstrItem1 += m_pDoc->m_pdPatient.cstrState;
          }
        if(g_pConfig->bGet_inc_rpt_item(INC_RPT_ZIP_CODE) == true)
          {
          if(cstrItem1 != "")
            cstrItem1 += " ";
          cstrItem1 += m_pDoc->m_pdPatient.cstrZipCode;
          }
        if(cstrItem1 != "")
          {
          m_pDC->TextOut(iXStart, iY, cstrItem1);
          iY += csText1.cy;
          }
        }
    }
    iY += csText1.cy;
    // Add other demographic items if checked.
    g_pLang->vStart_section(SEC_DEMOGRAPHICS);

    cstrTemp = "";
    if(g_pConfig->bGet_inc_rpt_item(INC_RPT_MALE_FEMALE) == true)
      { // Male/female is in report, get the text for it.
      if(m_pDoc->m_pdPatient.bFemale == TRUE)
        cstrTemp = g_pLang->cstrGet_text(ITEM_DG_FEMALE);
      else if(m_pDoc->m_pdPatient.bMale == TRUE)
        cstrTemp = g_pLang->cstrGet_text(ITEM_DG_MALE);
      }
    if(g_pConfig->bGet_inc_rpt_item(INC_RPT_DATE_OF_BIRTH) == true)
    { // Date of birth is in report.
      cstrItem1.Format("%s: %s", g_pLang->cstrGet_text(ITEM_DG_DATE_OF_BIRTH),
        m_pDoc->m_pdPatient.cstrDateOfBirth);
      // If male/female is in report, add it after the date of birth.
      if(cstrTemp.IsEmpty() == false)
        cstrItem1 += (", " + cstrTemp);
      m_pDC->TextOut(iXStart, iY, cstrItem1);
      iY += (2 * csText1.cy);
    }
    else
    { // Date of birth is NOT in report.
      if(cstrTemp.IsEmpty() == false)
      { // Male/female is in report.
        m_pDC->TextOut(iXStart, iY, cstrTemp);
        iY += (2 * csText1.cy);
      }
    }
    if(g_pConfig->bGet_inc_rpt_item(INC_RPT_SSN) == true)
    {
      cstrItem1.Format("%s: %s", g_pLang->cstrGet_text(ITEM_DG_SSN),
        m_pDoc->m_pdPatient.cstrSocialSecurityNum);
      m_pDC->TextOut(iXStart, iY, cstrItem1);
      iY += (2 * csText1.cy);
    }
    if(g_pConfig->bGet_inc_rpt_item(INC_RPT_PHONE_NUM) == true)
      {
      cstrItem1.Format("%s: %s", g_pLang->cstrGet_text(ITEM_DG_PHONE_NUM),
        m_pDoc->m_pdPatient.cstrPhone);
      m_pDC->TextOut(iXStart, iY, cstrItem1);
      iY += (2 * csText1.cy);
      }
    if(g_pConfig->bGet_inc_rpt_item(INC_RPT_HOSP_NUM) == true)
      {
      cstrItem1.Format("%s: %s", g_pLang->cstrGet_text(ITEM_DG_HOSPITAL_NUM),
        m_pDoc->m_pdPatient.cstrHospitalNum);
      m_pDC->TextOut(iXStart, iY, cstrItem1);
      iY += (2 * csText1.cy);
      }
    
      // Study date
#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
#if EGGSAS_READER == READER
    bHideStudyDate = true;
#elif EGGSAS_READER == READER_E
    bHideStudyDate = true;
#else
  if(m_pDoc->m_pdPatient.cstrDate.GetLength() <= 10)
    bHideStudyDate = false;
  else
    bHideStudyDate = true;
#endif
#else
  bHideStudyDate = false;
#endif
  if(bHideStudyDate == false)
    {
    cstrItem1.Format("%s: %s", g_pLang->cstrLoad_string(ITEM_G_DATE_OF_STUDY),
      m_pDoc->m_pdPatient.cstrDate);
    m_pDC->TextOut(iXStart, iY, cstrItem1);
    iY += (2 * csText1.cy);
    }
      // Physician and technician names.
    bSkipLine = false;
    if(g_pConfig->bGet_inc_rpt_item(INC_ATTENDING_PHYSICIAN) == true)
      {
      cstrItem1.Format("%s: %s", g_pLang->cstrLoad_string(ITEM_G_ATTENDING_PHYSICIAN),
        m_pDoc->m_pdPatient.cstrAttendPhys);
      m_pDC->TextOut(iXStart, iY, cstrItem1);
      iY += csText1.cy;
      bSkipLine = true;
      }
    if(g_pConfig->bGet_inc_rpt_item(INC_REFERING_PHYSICIAN) == true)
      {
      cstrItem1.Format("%s: %s", g_pLang->cstrLoad_string(ITEM_G_REFERING_PHYSICIAN),
        m_pDoc->m_pdPatient.cstrReferPhys);
      m_pDC->TextOut(iXStart, iY, cstrItem1);
      iY += csText1.cy;
      bSkipLine = true;
      }
    if(g_pConfig->bGet_inc_rpt_item(INC_TECHNICIAN) == true)
      {
      cstrItem1.Format("%s: %s", g_pLang->cstrLoad_string(ITEM_G_TECHNICIAN),
        m_pDoc->m_pdPatient.cstrTechnician);
      m_pDC->TextOut(iXStart, iY, cstrItem1);
      iY += csText1.cy;
      bSkipLine = true;
      }
    if(bSkipLine == true)
      iY += csText1.cy;
      // Indications
    g_pLang->vStart_section(SEC_PRESTUDY_INFO);
    cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_INDICATIONS);
    m_pDC->TextOut(iXStart, iY, cstrItem1);
    iY += csText1.cy;
    cstrItem1.Empty();
    if(m_pDoc->m_pdPatient.bNausea == TRUE)
      {
      cstrItem1 = g_pLang->cstrGet_text(ITEM_PSI_NAUSEA);
      m_pDC->TextOut(iX, iY, cstrItem1);
      iY += csText1.cy;
      }
    if(m_pDoc->m_pdPatient.bVomiting == TRUE)
      {
      cstrItem1 = g_pLang->cstrGet_text(ITEM_PSI_VOMITING);
      m_pDC->TextOut(iX, iY, cstrItem1);
      iY += csText1.cy;
      }
    //if(m_pDoc->m_pdPatient.bBloating == TRUE)
    //  {
    //  //cstrItem1.LoadString(IDS_BLOATING);
    //  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_BLOATING);
    //  m_pDC->TextOut(iX, iY, cstrItem1);
    //  iY += csText1.cy;
    //  }
    if(m_pDoc->m_pdPatient.bGerd == TRUE)
      {
      cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_GERD);
      m_pDC->TextOut(iX, iY, cstrItem1);
      iY += csText1.cy;
      }
    if(m_pDoc->m_pdPatient.bEructus == TRUE)
      {
      cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_ERUCTUS);
      m_pDC->TextOut(iX, iY, cstrItem1);
      iY += csText1.cy;
      }
    //if(m_pDoc->m_pdPatient.bEarlySateity == TRUE)
    //  {
    //  //cstrItem1.LoadString(IDS_EARLY_SATEITY);
    //  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_EARLY_SATEITY);
    //  m_pDC->TextOut(iX, iY, cstrItem1);
    //  iY += csText1.cy;
    //  }
    if(m_pDoc->m_pdPatient.bPPFullness == TRUE)
      {
      cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_PP_FULLNESS);
      m_pDC->TextOut(iX, iY, cstrItem1);
      iY += csText1.cy;
      }
    //if(m_pDoc->m_pdPatient.bDelayedGastricEmptying == TRUE)
    //  {
    //  //cstrItem1.LoadString(IDS_DELAYED_GASTRIC_EMPTYING);
    //  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_DELAYED_GASTRIC_EMPTYING);
    //  m_pDC->TextOut(iX, iY, cstrItem1);
    //  iY += csText1.cy;
    //  }
    if(m_pDoc->m_pdPatient.bGastroparesis == TRUE)
      {
      cstrItem1 = g_pLang->cstrGet_text(ITEM_PSI_GASTROPARESIS);
      m_pDC->TextOut(iX, iY, cstrItem1);
      iY += csText1.cy;
      }
    if(m_pDoc->m_pdPatient.bDyspepsiaBloatSatiety == TRUE)
      {
      cstrItem1 = g_pLang->cstrGet_text(ITEM_PSI_DYSPEPSIA_BLOAT_SATIETY);
      m_pDC->TextOut(iX, iY, cstrItem1);
      iY += csText1.cy;
      }
    if(m_pDoc->m_pdPatient.bDyspepsia == TRUE)
      {
      cstrItem1 = g_pLang->cstrGet_text(ITEM_PSI_DYSPEPSIA);
      m_pDC->TextOut(iX, iY, cstrItem1);
      iY += csText1.cy;
      }
    if(m_pDoc->m_pdPatient.bAnorexia == TRUE)
      {
      cstrItem1 = g_pLang->cstrGet_text(ITEM_PSI_ANOREXIA);
      m_pDC->TextOut(iX, iY, cstrItem1);
      iY += csText1.cy;
      }
    if(m_pDoc->m_pdPatient.bRefractoryGERD == TRUE)
      {
      cstrItem1 = g_pLang->cstrGet_text(ITEM_PSI_REFRACTORY_GERD);
      m_pDC->TextOut(iX, iY, cstrItem1);
      iY += csText1.cy;
      }
    if(m_pDoc->m_pdPatient.bWeightLoss == TRUE)
      {
      cstrItem1 = g_pLang->cstrGet_text(ITEM_PSI_WEIGHTLOSS);
      m_pDC->TextOut(iX, iY, cstrItem1);
      iY += csText1.cy;
      }
    if(m_pDoc->m_pdPatient.bPostDilationEval == TRUE)
      {
      cstrItem1 = g_pLang->cstrGet_text(ITEM_PSI_POST_DILATION_EVAL);
      m_pDC->TextOut(iX, iY, cstrItem1);
      iY += csText1.cy;
      }
    if(m_pDoc->m_pdPatient.bEvalEffectMed == TRUE)
      {
      cstrItem1 = g_pLang->cstrGet_text(ITEM_PSI_EVAL_EFFECT_MED);
      m_pDC->TextOut(iX, iY, cstrItem1);
      iY += csText1.cy;
      }
      // End of Indications
    if(m_pDoc->m_pdPatient.cstrIFPComments.IsEmpty() == FALSE)
      {
      iY = iDisplay_multiline(iX, iY, m_iLeftSideUseableWidth, m_pDoc->m_pdPatient.cstrIFPComments);
      iY += csText1.cy;
      }
    iY += csText1.cy;
    if(theApp.m_ptStudyType == PT_RESEARCH)
      { // For research version, display/print stimulation medium
        // Title
      //cstrItem1.LoadString(IDS_STIMULATION_MEDIUM);
      //m_pDC->TextOut(iXStart, iY, cstrItem1);
      //iY += csText1.cy;
        // Stimulation medium
      if(m_pDoc->m_pdPatient.cstrStimMedDesc != "")
        {
        cstrItem1.Format("%s: %s, %u %s", g_pLang->cstrLoad_string(ITEM_G_STIMULATION_MEDIUM),
          m_pDoc->m_pdPatient.cstrStimMedDesc, m_pDoc->m_pdPatient.uWaterAmount,
          m_pDoc->m_pdPatient.cstrWaterUnits);
        //cstrItem1 += m_pDoc->m_pdPatient.cstrStimMedDesc;
        //m_pDC->TextOut(iX, iY, m_pDoc->m_pdPatient.cstrStimMedDesc);
        //iY += csText1.cy;
        //  // Amount and units of stimulation medium.
        //cstrItem1.Format("%u %s", m_pDoc->m_pdPatient.uWaterAmount,
        //                m_pDoc->m_pdPatient.cstrWaterUnits);
        }
      else
        { // No stimulation medium.
        //cstrItem1.LoadString(IDS_NO_STIM_MEDIUM);
        cstrItem1.Format("%s: %s", g_pLang->cstrLoad_string(ITEM_G_STIMULATION_MEDIUM),
          g_pLang->cstrLoad_string(ITEM_G_NO_STIM_MEDIUM));
        //cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_NO_STIM_MEDIUM);
        }
      m_pDC->TextOut(iXStart, iY, cstrItem1);
      iY += (2 * csText1.cy);
      }
    else
      { // For water load, display the amount of water.
      if(m_pDoc->m_pdPatient.uWaterAmount > 0)
        {
        cstrItem1.Format("%s: %u", g_pLang->cstrLoad_string(ITEM_G_AMTWATERINGESTED),
          m_pDoc->m_pdPatient.uWaterAmount);

        //cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_AMTWATERINGESTED);
        //m_pDC->TextOut(iXStart, iY, cstrItem1);
        //iY += csText1.cy;
        //cstrItem1.Format("%u ", m_pDoc->m_pdPatient.uWaterAmount);
        g_pLang->vStart_section(SEC_STIMULATION_MEDIUM);
        if(m_pDoc->m_pdPatient.uWaterUnits == OUNCES)
          cstrItem1 += g_pLang->cstrGet_text(ITEM_SM_OZ);
        else if(m_pDoc->m_pdPatient.uWaterUnits == MILLILITERS)
          cstrItem1 += g_pLang->cstrGet_text(ITEM_SM_ML);
        else if(m_pDoc->m_pdPatient.uWaterUnits == GRAMS)
          cstrItem1 += g_pLang->cstrGet_text(ITEM_SM_GRAMS);
        else
          cstrItem1 += m_pDoc->m_pdPatient.cstrWaterUnits;
      }
      else
        { // No record of water ingested.
        cstrItem1.Format("%s: %s", g_pLang->cstrLoad_string(ITEM_G_NO_STIM_MEDIUM),
          m_pDoc->m_pdPatient.uWaterAmount);
        //cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_NO_STIM_MEDIUM);
        }
      m_pDC->TextOut(iXStart, iY, cstrItem1);
      iY += (2 * csText1.cy);
      }
    }
  else
    { // No patient information, display patient name.
      // Display/print the header information.
    iY = iAdd_header(iX, iY);
    }
    
  if(m_iIncludeCoreReport != 0 || m_iIncludeDiagRecom != 0)
  {
      // Diagnosis
    //cstrItem1.LoadString(IDS_DIAGNOSIS);
    cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_DIAGNOSIS);
    m_pDC->TextOut(iXStart, iY, cstrItem1);
    iY += csText1.cy;
    //if(m_pDoc->m_pdPatient.bUserDiagnosis == TRUE)
      iY = iDisplay_multiline(iX, iY, m_iLeftSideUseableWidth, m_pDoc->m_pdPatient.cstrDiagnosis);
    //else
    //  iY = iDisplay_multiline(iX, iY, m_iLeftSideUseableWidth, m_pDoc->m_pdPatient.cstrSuggestedDiagnosis);
    iY += (2 * csText1.cy);
      // Recommendations
    //cstrItem1.LoadString(IDS_RECOMMENDATIONS);
    cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_RECOMMENDATIONS);
    m_pDC->TextOut(iXStart, iY, cstrItem1);
    iY += csText1.cy;
    iY = iDisplay_multiline(iX, iY, m_iLeftSideUseableWidth, m_pDoc->m_pdPatient.cstrRecommendations);
    iY += (3 * csText1.cy);
  }
    // Signature
  pt.x = iXStart;
  pt.y = iY;
  m_pDC->MoveTo(pt);
  pt.x = m_iLeftWidth;
  pt.y = iY;
  m_pDC->LineTo(pt);
  iY += 2;
  //cstrItem1.LoadString(IDS_SIGNATURE);
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_SIGNATURE);
  m_pDC->TextOut(iXStart, iY, cstrItem1);
  //csText1 = m_pDC->GetTextExtent(cstrItem1);
  iY += csText1.cy;
  //cstrItem1.LoadString(IDS_DATE);
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_DATE);
  m_pDC->TextOut(iXStart + csText1.cx + 50, iY, cstrItem1);
  iY += csText1.cy;
  if(m_pDC->IsPrinting() != 0)
    { // Printing.  Restore the font.
    if(iDCSave > 0)
      ::RestoreDC(m_pDC->m_hDC, iDCSave);
    }
  return(iY);
  }

/********************************************************************
iDisplay_multiline - Display a buffer of text that takes up more than
                     one line.

  inputs: X coordinate of where to start the line.
          Y coordinate of where to start the line.
          Width (in pixels) that the line can take up.
          String to display.

  return: Y coordinate of the last line.
********************************************************************/
int CReportView::iDisplay_multiline(int iX, int iY, int iWidth, CString cstrInfo)
  {
  CString cstrTemp, cstrOrigInfo;
  TEXTMETRIC tm;
  CSize csText1;
  short int iMaxCharsOnLine;
  int iIndex, iCRIndex;

    // Get the maximum character width so we can figure out where to break
    // the line so if fits in the left side of the window.
  m_pDC->GetTextMetrics(&tm);
  iMaxCharsOnLine = (short int)(iWidth / tm.tmAveCharWidth);

  cstrOrigInfo = cstrInfo;
  iIndex = iMaxCharsOnLine;
  while(cstrOrigInfo.IsEmpty() == FALSE)
    { // Look for a carriage return/linefeed within the line.
    if((iCRIndex = cstrOrigInfo.Find(CR)) >= 0)
      { // Found a cr/lf, take out the line.
      cstrTemp = cstrOrigInfo.Left(iCRIndex);
      cstrOrigInfo.Delete(0, iCRIndex + 2);
      }
    else
      { // No cr/lf, use entire line.
      cstrTemp = cstrOrigInfo;
      cstrOrigInfo.Empty();
      }
    while(true)
      { // Separate the line into as many lines as needed to
        // fit in the specified width.
      if(cstrTemp.GetLength() <= iMaxCharsOnLine)
        { // All characters fit in the specified width.
        m_pDC->TextOut(iX, iY, cstrTemp); // Fits on one line.
        csText1 = m_pDC->GetTextExtent(cstrTemp);
        iY += csText1.cy;
        break;
        }
      else
        { // More characters than can fit. Separate the line.
        if(cstrTemp.GetAt(iIndex) != ' ')
          { // Last character that will fit in the width is not a space.
            // Back up until we find a space.
          while(iIndex > 0)
            {
            if(cstrTemp.GetAt(iIndex) == ' ')
              break;
            --iIndex;
            }
          }
        m_pDC->TextOut(iX, iY, cstrTemp.Left(iIndex));
        csText1 = m_pDC->GetTextExtent(cstrTemp);
        iY += csText1.cy;
        cstrTemp.Delete(0, iIndex + 1);
        }
      }
    }
  return(iY);
  }

/********************************************************************
vPrint_report - Print the report.

  inputs: None.

  return: None.
********************************************************************/
void CReportView::vPrint_report()
  {
  CPrintDialog *pdlg = NULL;
  CRect rPrint;
  BOOL bRet;
  DOCINFO docinfo;
  CString cstrMsg, cstrErr;

  DWORD dwFlags = PD_ALLPAGES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE | PD_NOSELECTION; // PD_USEDEVMODECOPIES
  pdlg = new CPrintDialog(FALSE, dwFlags); //, this);

/***************
    // Get the default printer and set it to portrait orientation.
  if(pdlg->GetDefaults() != 0 && pdlg->m_pd.hDevMode != 0)
    { // Get a pointer to the DEVMODE structure.
    DEVMODE FAR *pDevMode = (DEVMODE FAR *)::GlobalLock(pdlg->m_pd.hDevMode);
      // Set the orientation to portrait.
    if(pDevMode->dmOrientation != DMORIENT_LANDSCAPE)
      pDevMode->dmOrientation = DMORIENT_LANDSCAPE; // 2
    ::GlobalUnlock(pdlg->m_pd.hDevMode);
    }
    // GetDefaults() sets this flag which causes the dialog box not to
    // be displayed.  We have to reset it.
  pdlg->m_pd.Flags &= ~PD_RETURNDEFAULT;
**************/
  if(pdlg->DoModal() == IDOK)
    {
    pdlg->m_pd.hDC = pdlg->CreatePrinterDC();
    bRet = TRUE;
    }
  else
    bRet = FALSE;
  if(bRet == TRUE)
    { // Do our own printing.
    CDC dcPrint;
    dcPrint.Attach(pdlg->m_pd.hDC);
    dcPrint.m_bPrinting = TRUE;
        // Set up the document information structure.
    docinfo.cbSize = sizeof(docinfo);
    docinfo.lpszDocName = "EGGSAS REPORT";
    docinfo.lpszOutput = NULL;
    docinfo.lpszDatatype = NULL;
    docinfo.fwType = 0;
      // set up drawing rect to entire page (in logical coordinates)
    rPrint.SetRect(0, 0, dcPrint.GetDeviceCaps(HORZRES), dcPrint.GetDeviceCaps(VERTRES));

    dcPrint.DPtoLP(&rPrint);
//    dcPrint.SetBoundsRect(&rPrint, DCB_ACCUMULATE);
      // Start printing.
    if(dcPrint.StartDoc(&docinfo) < 0)
      {
      //cstrMsg.LoadString(IDS_ERR_PRINTER_INIT);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_ERR_PRINTER_INIT);
      //cstrErr.LoadString(IDS_ERROR1);
      cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERROR);
      MessageBox(cstrMsg, cstrErr, MB_OK | MB_ICONSTOP);
      }
    else
      { // Got the document started.
      if(dcPrint.StartPage() < 0)
        {
        //cstrMsg.LoadString(IDS_ERR_PRINTER_START_PAGE);
        cstrMsg = g_pLang->cstrLoad_string(ITEM_G_ERR_PRINTER_START_PAGE);
        //cstrErr.LoadString(IDS_ERROR1);
        cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERROR);
        MessageBox(cstrMsg, cstrErr, MB_OK | MB_ICONSTOP);
        dcPrint.AbortDoc();
        }
      else
        { // Got the page started.
        dcPrint.SetMapMode(MM_TEXT);
        OnDraw(&dcPrint);

        dcPrint.EndPage();
        dcPrint.EndDoc();
        }
      }
    }
  delete pdlg; // deleted when pInfo is deleted.

//	// Set this value after calling DoPreparePrinting to override
//	// value read from .INI file
//	return(FALSE); // bRet;
	
//// End of RSADemo
  }

/********************************************************************
vPrint_graph - Print a Proessentials graph.

  inputs: Window handle of the window containing the graph.
          Pointer to a RECT structure containing the rectangle
            the graph is in.

  return: None.
********************************************************************/
void CReportView::vPrint_graph(HWND hPE, RECT *prRect)
  {
  HMETAFILE hmfChart;
  CPoint cptOrigViewPortOrg;
  CSize csOrigViewPortExt;
  int iOldMapMode, iWidth, iHeight, iDCSave;

    // Save the DC so we can restore it later.  The printing a metafile
    // seems to corrupt the DC and afterwards, things don't work like
    // centering text and positioning text and drawing other graphs.
  iDCSave = ::SaveDC(m_pDC->m_hDC);
     // Set the mapping mode for graphics.
  iOldMapMode = m_pDC->SetMapMode(MM_ANISOTROPIC);

    // Calculate where the graphs should go.
  iWidth = prRect->right - prRect->left;
  iHeight = prRect->bottom - prRect->top;
    // Set top left of where graph starts.
  cptOrigViewPortOrg = m_pDC->SetViewportOrg(prRect->left, prRect->top);
  csOrigViewPortExt = m_pDC->SetViewportExt(iWidth, iHeight); // Size of graph.

    // Set the graph to match the size we are printing.
  PEresetimageEx(hPE, iWidth, iHeight, prRect->left, prRect->top);
    // Get the metafile so we can print it.
  hmfChart = PEgetmeta(hPE);
  m_pDC->PlayMetaFile(hmfChart); // Print the metafile.

  m_pDC->SetMapMode(iOldMapMode); // Put the original map mode back.

    // Set the view port origin and extent to what it was before the graphs
    // were displayed.
  m_pDC->SetViewportOrg(cptOrigViewPortOrg.x, cptOrigViewPortOrg.y);
  m_pDC->SetViewportExt(csOrigViewPortExt.cx, csOrigViewPortExt.cy);

    // Put the graph back to the size it is in the view window.
  PEresetimage(hPE, 0, 0);
    // Restore the DC>
  if(iDCSave > 0)
    ::RestoreDC(m_pDC->m_hDC, iDCSave);
  }

/********************************************************************
iEnd_of_page - Do end of page processing for both printing and displaying.

  inputs: X coordinate of there the end of the page starts.
          Y coordinate of last item on the page.
          Page number.

  return: Y coordinate of where the first line on the next page starts.
********************************************************************/
int CReportView::iEnd_of_page(int iX, int iY, short int iPage)
  {
  CString cstrItem1;
  CSize csText1;
  POINT pt;
  int iX1;

  if(m_pDC->IsPrinting() != 0)
    { // Printing
      // Print the page number at the bottom of the page.
    iY = m_pDC->GetDeviceCaps(VERTRES);
    iY -= m_pDC->GetDeviceCaps(LOGPIXELSY); // Approx 1 inch from bottom.
    }
  else
    { // Not printing.
    iY += 50;
    }
    // Display the page number.
  //cstrItem1.Format(IDS_PAGE_NUMBER, iPage, m_iNumPages);
  cstrItem1.Format(g_pLang->cstrLoad_string(ITEM_G_PAGE_NUMBER), iPage, m_iNumPages);
  iX1 = iCenter_text(cstrItem1, m_iLineExt);
  m_pDC->TextOut(iX1, iY, cstrItem1);
  if(m_pDC->IsPrinting() == 0)
    { // Not printing. Draw a line to indicate the page break.
    csText1 = m_pDC->GetTextExtent(cstrItem1);
    iY += (csText1.cy + 10);
    pt.x = iX;
    pt.y = iY;
    m_pDC->MoveTo(pt);
    pt.x = m_iLineExt - iX;
    pt.y = iY;
    m_pDC->LineTo(pt);
    iY += 50;
    }
  else //if(m_pDC->IsPrinting() != 0)
    { // Printing. Do a form feed to the next page.
    m_pDC->EndPage();
    if(iPage != m_iNumPages) 
      m_pDC->StartPage(); // Not the last page, start a new page.
    m_pDC->SetMapMode(MM_TEXT); // Set the original map mode to text.
    iY = m_pDC->GetDeviceCaps(LOGPIXELSY) / 2; // Printing. Leave about 1/2 inch at top.
    }
  m_bAddedEndOfPage = true;
  return(iY);
  }

/********************************************************************
iCreate_all_raw_data_graphs - Graph the all raw data.
                                - Each period is labeled.
                                - Each period has 4 minutes of data per
                                  graph and uses as many graphs as needed.

  inputs: X coordinate of where to start the graph.
          Y coordinate of where to start the graph.
          Page number the graphs start on

  return: Y coordinate of the bottom of the graph.
********************************************************************/
int CReportView::iCreate_all_raw_data_graphs(int iX, int iY, short int *piPage)
  {
  HWND hPEGrph;
  RECT rGrphWin, rTemp;
  CString cstrTitle, cstrBaseText;
  CSize csText1;
  GRAPH_INFO gi;
  unsigned short uIndex, uNumGraphs, uGraphIndex, uNumResearchGraphs;
  long lOffset;
  int iHeight, iYTitle, iWidth, iLastY;
  bool bDisplayGraph;

  if(m_phPERawData != NULL)
    { // Draw the graphs.
    if(m_pDC->IsPrinting() == 0)
      {
      if(m_iRawDataGraphStartY != iY) // && m_iRawDataGraphStartY >= 0)
        { // Location on the screen has changed. Reinitialize all graphs
          // for the new location.
        for(uIndex = 0; uIndex < m_uNumRawDataGraphs; ++uIndex)
          {
          if(*(m_phPERawData + uIndex) != NULL)
            {
            PEdestroy(*(m_phPERawData + uIndex));
            *(m_phPERawData + uIndex) = NULL;
            }
          }
        }
      m_iRawDataGraphStartY = iY;
      GetClientRect(&rGrphWin); // Not printing
      }
    else
      rGrphWin.right = m_iLineExt; // Printing

    rGrphWin.top = iY;
    if(m_pDC->IsPrinting() == 0)
      { // not printing.
      rGrphWin.left = iX * 2;
      if(m_iRawDataGraphWidth == 0)
        m_iRawDataGraphWidth = rGrphWin.right - rGrphWin.left - (iX * 2);
      iWidth = m_iRawDataGraphWidth;
      iHeight = m_iRawDataGraphWidth / 4;
      }
    else
      { // Printing. Calculate the graph height and width.
      rGrphWin.left = iX * 4;
      iWidth = iCalc_print_graph_width(rGrphWin.left);
      iHeight = iCalc_print_graph_height(iY);
      }
    rGrphWin.right = rGrphWin.left + iWidth;
    rGrphWin.bottom = rGrphWin.top + iHeight;
//    rGrphWin.bottom = rGrphWin.top + (m_iRawDataGraphWidth / 4); //(rGrphWin.right / 4); //200;
//    iHeight = rGrphWin.bottom - rGrphWin.top;

    gi.uAttrib = MINUTES_INTEGERS;
    gi.bAddXAxisLabel = true;
    if(theApp.m_ptStudyType == PT_STANDARD)
      { // Study is standard
      gi.fMinOnGraph = 4.0F;
      }
    else
      { // Research study
      gi.fMinOnGraph = (float)m_pDoc->m_iMinPerGraph;
      }
    uGraphIndex = 0;
    uNumResearchGraphs = 0;
      // In Research study, a new page can appear anywhere in the raw data graphs.
      // So put the header at the start of the baseline page and then put the header
      // at each new page as we go through and graph the data.
    if(theApp.m_ptStudyType == PT_RESEARCH)
     rGrphWin.top = iAdd_header(iX, rGrphWin.top);
    for(gi.uPeriod = 0; gi.uPeriod < m_uMaxPeriods /*TOTAL_PERIODS*/; ++gi.uPeriod)
      {
      m_bAddedEndOfPage = false;
        // Display/print the header information only for a standard study.
      if(theApp.m_ptStudyType == PT_STANDARD)
        rGrphWin.top = iAdd_header(iX, rGrphWin.top);
      else
        { // Research version.
        if(uNumResearchGraphs == 0)
          {  // Add header to new page.
          rGrphWin.top = iAdd_header(iX, rGrphWin.top);
          iY = rGrphWin.top;
          }
        }
      if(m_pDoc->m_pdPatient.fPeriodLen[gi.uPeriod] != INV_PERIOD_LEN)
        { // Period is used, graph it.
          // Get number of graphs in period.
        if(theApp.m_ptStudyType == PT_STANDARD)
          { // Study is standard
          if(m_pDoc->m_pdPatient.fPeriodLen[gi.uPeriod] == (float)(MIN_PER_PERIOD * 2))
            uNumGraphs = 5; // Period has extra 10 minutes.
          else
            uNumGraphs = 3;
          }
        else
          { // Research study
          uNumGraphs = (unsigned short)(m_pDoc->m_pdPatient.fPeriodLen[gi.uPeriod]
                       / (float)m_pDoc->m_iMinPerGraph);
          if(fmod(m_pDoc->m_pdPatient.fPeriodLen[gi.uPeriod],
          (float)m_pDoc->m_iMinPerGraph) > (float)0)
            ++uNumGraphs;
          }
          // Find out if the graphs for this period will actually be visible.
          // This only applies if we are not printing.  If printing, the
          // graphs will always be visible.
          // If any graph in the period is visible, graph the entire period.
        rTemp = rGrphWin;
        for(uIndex = 0; uIndex < uNumGraphs; ++uIndex)
          {
          if(m_pDC->RectVisible(&rTemp) != 0)
            break;
          rTemp.top = rTemp.bottom + 10;
          rTemp.bottom = rTemp.top + iHeight;
          }
        if((m_pDC->IsPrinting() == 0 && uIndex < uNumGraphs)
        || m_pDC->IsPrinting() != 0)
          {
          if(m_pDoc->iRead_period(gi.uPeriod, m_pDoc->m_fData) == SUCCESS)
            {
            gi.fMinEGG = m_pDoc->m_pdPatient.fMinEGG;
            gi.fMaxEGG = m_pDoc->m_pdPatient.fMaxEGG;
            gi.fMinResp = m_pDoc->m_pdPatient.fMinResp;
            gi.fMaxResp = m_pDoc->m_pdPatient.fMaxResp;
            bDisplayGraph = true; // Graph visible.
            }
          else
            bDisplayGraph = false; // No data, graph not visible.
          }
        else
          bDisplayGraph = false; // Graph not visible.
          // Print a title line for the period.
        if(theApp.m_ptStudyType == PT_STANDARD)
          { // Study is standard
          switch(gi.uPeriod)
            {
            case PERIOD_BL:
              //cstrTitle.LoadString(IDS_BASELINE);
              cstrTitle = g_pLang->cstrLoad_string(ITEM_G_BASELINE);
              break;
            case PERIOD_10:
              //cstrTitle.LoadString(IDS_FIRST_10_MIN_PD);
              cstrTitle = g_pLang->cstrLoad_string(ITEM_G_FIRST_10_MIN_PD);
              break;
            case PERIOD_20:
              //cstrTitle.LoadString(IDS_SECOND_10_MIN_PD);
              cstrTitle = g_pLang->cstrLoad_string(ITEM_G_SECOND_10_MIN_PD);
              break;
            case PERIOD_30:
              //cstrTitle.LoadString(IDS_THIRD_10_MIN_PD);
              cstrTitle = g_pLang->cstrLoad_string(ITEM_G_THIRD_10_MIN_PD);
              break;
            case PERIOD_40:
              //cstrTitle.LoadString(IDS_FOURTH_10_MIN_PD);
              cstrTitle = g_pLang->cstrLoad_string(ITEM_G_FOURTH_10_MIN_PD);
              break;
            default:
              cstrTitle.Empty();
              break;
            }
          }
        else
          { // Research study
          if(gi.uPeriod == PERIOD_BL)
            cstrTitle = g_pLang->cstrLoad_string(ITEM_G_BASELINE);
           //cstrTitle.LoadString(IDS_BASELINE);
          else
            {
            //cstrBaseText.LoadString(IDS_POST_STIM_PERIOD);
            cstrBaseText = g_pLang->cstrLoad_string(ITEM_G_POST_STIM_PERIOD);
            cstrTitle.Format("%s %d", cstrBaseText, gi.uPeriod);
            }
          }
          // Display the title line if the graph is going to be displayed.
        if(bDisplayGraph == true)
          m_pDC->TextOut(iX, rGrphWin.top, cstrTitle);
          // Update where on the screen the first graph for the period starts.
        csText1 = m_pDC->GetTextExtent(cstrTitle);
        iYTitle = rGrphWin.top;
        rGrphWin.top += (csText1.cy * 2);
        rGrphWin.bottom = rGrphWin.top + iHeight;
          // Draw the each graph in the period.
        for(uIndex = 0; uIndex < uNumGraphs; ++uIndex, ++uGraphIndex)
          { // Graph each 4 minutes in period.
          m_bAddedEndOfPage = false;
          if(theApp.m_ptStudyType == PT_RESEARCH)
            {
            if(uIndex > 0 && uNumResearchGraphs == 0)
              {  // Add header to new page.
              rGrphWin.top = iAdd_header(iX, rGrphWin.top);
              iY = rGrphWin.top;
              }
            }
            // If the graph window doesn't exist yet, create it.
          if(m_phPERawData[uGraphIndex] == NULL)
            m_phPERawData[uGraphIndex] = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE,
                                                  &rGrphWin, m_hWnd, IDR_MAINFRAME);
          hPEGrph = m_phPERawData[uGraphIndex];

          if(bDisplayGraph == true)
            {
            if(theApp.m_ptStudyType == PT_STANDARD)
              { // Study is standard
              gi.fStartMin = m_pDoc->m_pdPatient.fPeriodStart[gi.uPeriod]
                                  + (float)(uIndex * 4);
                // Make the graph annotation for the start minute based on the period.
              gi.fGraphStartMin = gi.fStartMin;
                // Determine how many minutes go on the graph.
              if(uIndex == 2 && uNumGraphs == 3)
                gi.iNumDataPoints = 2; // Last graph of a 10 minute period.
              else
                gi.iNumDataPoints = 4;
              gi.iNumDataPoints *= DATA_POINTS_PER_MIN_1;
              }
            else
              { // Research study
              gi.fStartMin = m_pDoc->m_pdPatient.fPeriodStart[gi.uPeriod]
                               + (uIndex * m_pDoc->m_iMinPerGraph);

              float fNumDataPoints =(float)m_pDoc->m_iMinPerGraph;
              float fMinRemaining = m_pDoc->m_pdPatient.fPeriodLen[gi.uPeriod]
                                     - (float)(m_pDoc->m_iMinPerGraph * uIndex);
              if(fMinRemaining < fNumDataPoints)
                gi.iNumDataPoints = (int)(fMinRemaining * (float)DATA_POINTS_PER_MIN_1);
              else
                gi.iNumDataPoints = (int)(fNumDataPoints * (float)DATA_POINTS_PER_MIN_1);
                // Make the graph annotation for the start minute based on the period.
              gi.fGraphStartMin = gi.fStartMin;
              }
    
              // Get pointer into the data buffer for where the data starts.
            lOffset = (uIndex * 4);
            gi.pfData =  m_pDoc->m_fData + 
                         (long)(lOffset * DATA_POINTS_PER_MIN_1 * MAX_CHANNELS);
              // Draw the graph.
            m_pDoc->vDraw_raw_data_graph(hPEGrph, &gi);
            if(m_pDC->IsPrinting() != 0)
              vPrint_graph(hPEGrph, &rGrphWin);
            }

          if(theApp.m_ptStudyType == PT_STANDARD)
            { // Study is standard
            if(uIndex == 2)
              { // Done one page, go to next page.
              iY = iEnd_of_page(iX, rGrphWin.bottom, *piPage); // End of page processing.
              ++(*piPage);
              rGrphWin.top = iY;
              }
            else
              rGrphWin.top = rGrphWin.bottom + 10;
            }
          else
            { // Research Study
            if(uNumResearchGraphs == 2)
              { // Done one page, go to next page.
              uNumResearchGraphs = 0;
              iY = iEnd_of_page(iX, rGrphWin.bottom, *piPage); // End of page processing.
              ++(*piPage);
              rGrphWin.top = iY;
//              rGrphWin.bottom = rGrphWin.top + iHeight; //(rGrphWin.right / 4); //200;
//                // Add header to new page.
//              rGrphWin.top = iAdd_header(iX, rGrphWin.top);
              }
            else
              {
              rGrphWin.top = rGrphWin.bottom + 10;
              ++uNumResearchGraphs;
              }
            iLastY = rGrphWin.bottom;
            }
          rGrphWin.bottom = rGrphWin.top + iHeight; //(rGrphWin.right / 4); //200;
          } // end of for each graph in period.
        }
      if(theApp.m_ptStudyType == PT_STANDARD)
        { // Study is standard
        if(uNumGraphs > 3)
          { // Done last page for this period, go to next page.
          iY = iEnd_of_page(iX, rGrphWin.bottom, *piPage); // End of page processing.
          ++(*piPage);
          rGrphWin.top = iY;
          rGrphWin.bottom = rGrphWin.top + iHeight; //(rGrphWin.right / 4); //200;
          }
        }
      else
        { // Research study
        if(uNumResearchGraphs > 3)
          { // Done last page for this period, go to next page.
          iY = iEnd_of_page(iX, rGrphWin.bottom, *piPage); // End of page processing.
          ++(*piPage);
          rGrphWin.top = iY;
          rGrphWin.bottom = rGrphWin.top + iHeight; //(rGrphWin.right / 4); //200;
          }
        }
      } // end of for each period.
    if(theApp.m_ptStudyType == PT_RESEARCH)
      { // Research Study.
        // If last page didn't contain a full page of graphs, do last page processing.
      if(uNumResearchGraphs <= 2 && uNumResearchGraphs > 0)
        { // Done one page, go to next page.
        iY = iEnd_of_page(iX, iLastY, *piPage); // End of page processing.
        ++(*piPage);
        }
      }
    }
  if(theApp.m_ptStudyType == PT_RESEARCH)
    {
    rGrphWin.bottom = iY; // Set start of next page.
    }
  return(rGrphWin.bottom);
  }

/********************************************************************
iAlloc_raw_data_graphs - Allocate and array to hold the windows for
                         all the all raw data graphs.
                         Also count the number of pages need to display
                         and print them.
                         The number of pages needed is:
                           - 1 page per period.
                           - 2 pages for a 20 minute baseline.

  inputs: None.

  return: Number of pages needed to print the graphs.
          -1 if an error.
********************************************************************/
short int CReportView::iAlloc_raw_data_graphs(void)
  {
  short unsigned uIndex;
  unsigned uNumGraphs, uTotalGraphs;
  short int iNumPages;

    // Count the number of raw data graphs needed.
    // Also count the number of pages needed.
    //  1 page per period.
    //  For a 20 minute baseline, 2 pages.
  uTotalGraphs = 0;
  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Study is standard
    for(uIndex = 0, iNumPages = 0; uIndex < TOTAL_PERIODS; ++uIndex)
      {
      if(m_pDoc->m_pdPatient.fGoodMinStart[uIndex] != INV_MINUTE)
        {
        uNumGraphs = (unsigned)m_pDoc->m_pdPatient.fPeriodLen[uIndex] / 4;
        if(uNumGraphs > 0)
          {
            // TODO account for half minutes.
          if(((unsigned)m_pDoc->m_pdPatient.fPeriodLen[uIndex] % 4) != 0)
            ++uNumGraphs;
          uTotalGraphs += uNumGraphs;
          ++iNumPages;
          if(uNumGraphs > NUM_RAW_DATA_GRAPHS_PER_PAGE)
            ++iNumPages;
          }
        }
      }
    }
  else
    { // Research study
    for(uIndex = 0, iNumPages = 0; uIndex < m_uMaxPeriods; ++uIndex)
      {
      if(m_pDoc->m_pdPatient.fGoodMinStart[uIndex] != INV_MINUTE)
        {
        uNumGraphs = (unsigned)m_pDoc->m_pdPatient.fPeriodLen[uIndex]
                     / m_pDoc->m_iMinPerGraph;
        if((float)(uNumGraphs * m_pDoc->m_iMinPerGraph)
        < m_pDoc->m_pdPatient.fPeriodLen[uIndex])
          ++uNumGraphs; // Partial graph.
        uTotalGraphs += uNumGraphs;
        }
      }
    iNumPages = uTotalGraphs / NUM_RAW_DATA_GRAPHS_PER_PAGE;
    if((uTotalGraphs % NUM_RAW_DATA_GRAPHS_PER_PAGE) != 0)
      ++iNumPages;
    }
  if(m_uNumRawDataGraphs == 0)
    { // Haven't drawn the graphs yet.
    m_uNumRawDataGraphs = uTotalGraphs;
      // Allocate an array to hold the windows for the raw data graphs.
    if(m_phPERawData != NULL)
      GlobalFree(m_phPERawData);
    if((m_phPERawData = (HWND *)GlobalAlloc(GPTR, sizeof(HWND) * m_uNumRawDataGraphs))
    == NULL)
      iNumPages = -1;
    }
  return(iNumPages);
  }

/********************************************************************
vReset_view - Resets the view to the beginning of the report and resets
              the document size to just the size of the screen.

              This call must be followed by a call to a function that
              causes repainting of the report.

  inputs: None.

  return: None.
********************************************************************/
void CReportView::vReset_view(void)
  {
  RECT rClient;

/****************************
  unsigned short uIndex;

    // Clear out the summary graphs.
  for(uIndex = 0; uIndex < CONTROL_FREQ_BANDS; ++uIndex)
    {
    if(m_hPESum[uIndex] != NULL)
      {
      PEdestroy(m_hPESum[uIndex]);
      m_hPESum[uIndex] = NULL;
      }
    }

  if(m_pcwndRSA != NULL)
    {
    delete m_pcwndRSA;
    m_pcwndRSA = NULL;
    }
***************************/

  GetClientRect(&rClient);
  m_csViewSize.cx = rClient.right;
  m_csViewSize.cy = rClient.bottom;
  ResyncScrollSizes();
  }

/********************************************************************
iCalc_print_graph_height - Calculate the height of a raw data graph
                           for printing.

                           - Get the number of pixels vertically,
                           - Subtract top margin.
                           - Subtract twice the pixels that we leave for the
                             page number and
                             bottom margin so that the graphs don't run in to
                             the page number.
                           - Divide by 4 so there is space for each of the
                             4 graphs.

  inputs: Y pixel on the page where the graphs start (top margin).

  return: Height of a graph.
********************************************************************/
int CReportView::iCalc_print_graph_height(int iYStart)
  {
  int iHeight;

  iHeight = (m_pDC->GetDeviceCaps(VERTRES) - iYStart
             - (2 * m_pDC->GetDeviceCaps(LOGPIXELSY))) / 4;
  return(iHeight);
  }

/********************************************************************
iCalc_print_graph_width - Calculate the width of a raw data graph
                          for printing.

                           - Get the number of pixels horizontally
                           - Subtract twice the left margin so the graph
                             is horizontally centered.

  inputs: X pixel on the page where the graphs start (left margin).

  return: Width of a graph.
********************************************************************/
int CReportView::iCalc_print_graph_width(int iXStart)
  {
  int iWidth;

  iWidth = m_iLineExt - (2 * iXStart);
  return(iWidth);
  }

/********************************************************************
iAdd_header - Put at the top of the report.
                   Patient Name
                   Study date

  inputs: X coordinate of where to start the header.
          Y coordinate of where to start the header.

  return: Y coordinate of the last line of the header.
********************************************************************/
int CReportView::iAdd_header(int iXStart, int iYStart)
  {
  CString cstrItem1;
  CSize csText1;
  TEXTMETRIC tm;
  CFont cfFont;
  int iX, iY, iDCSave;
  bool bHideStudyDate;

  if(m_pDC->IsPrinting() != 0)
    { // Printing.  Use a smaller font.
      // Save the DC so we can easily restore the font.
    iDCSave = ::SaveDC(m_pDC->m_hDC);
    vSet_font(NULL, m_pDC, &cfFont, FW_THIN, 10);
    m_pDC->SelectObject(&cfFont);
    }
  iY = iYStart;
  m_pDC->GetTextMetrics(&tm);
  iX = iXStart + (4 * tm.tmAveCharWidth);
    // Patient Name
  if(m_pDoc->m_pdPatient.bUseResrchRefNum == TRUE)
    cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_REF_NUM);
    //cstrItem1.LoadString(IDS_REF_NUM);
  else
    cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_PATIENT_NAME);
    //cstrItem1.LoadString(IDS_PATIENT_NAME);
  cstrItem1 += ":";
  m_pDC->TextOut(iXStart, iY, cstrItem1);
  csText1 = m_pDC->GetTextExtent(cstrItem1);
  iX += (csText1.cx + tm.tmAveCharWidth);
  //vFormat_patient_name(&m_pDoc->m_pdPatient, cstrItem1, true);
  if(m_pDoc->m_pdPatient.bUseResrchRefNum == TRUE)
    cstrItem1 = m_pDoc->m_pdPatient.cstrResearchRefNum;
  else
    vFormat_patient_name(&m_pDoc->m_pdPatient, cstrItem1, true);
   //cstrItem1.Format("%s %s", m_pDoc->m_pdPatient.cstrFirstName, m_pDoc->m_pdPatient.cstrLastName);
  //theApp.vLog("Patient name: %s, x = %d, y = %d", cstrItem1, iX, iY);
  m_pDC->TextOut(iX, iY, cstrItem1);
  csText1 = m_pDC->GetTextExtent(cstrItem1);
  iX += (csText1.cx + (4 * tm.tmAveCharWidth));
    // Study date
  //cstrItem1.LoadString(IDS_DATE_OF_STUDY);
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_DATE_OF_STUDY);
  cstrItem1 += ":";
  m_pDC->TextOut(iX, iY, cstrItem1);
#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
#if EGGSAS_READER == READER
    bHideStudyDate = true;
#elif EGGSAS_READER == READER_E
    bHideStudyDate = true;
#else
  if(m_pDoc->m_pdPatient.cstrDate.GetLength() <= 10)
    bHideStudyDate = false;
  else
    bHideStudyDate = true;
#endif
#else
  bHideStudyDate = false;
#endif
  if(bHideStudyDate == false)
    {
    csText1 = m_pDC->GetTextExtent(cstrItem1);
    iX += (csText1.cx + tm.tmAveCharWidth);
    m_pDC->TextOut(iX, iY, m_pDoc->m_pdPatient.cstrDate);
    }
  iY += (2 * csText1.cy);

  if(m_pDC->IsPrinting() != 0)
    { // Printing.  Restore the font.
    if(iDCSave > 0)
      ::RestoreDC(m_pDC->m_hDC, iDCSave);
    }
  return(iY);
  }

/********************************************************************
cstrGet_timeband_start_end_min - Get the start and end minute of the
                                 timeband (period).

  inputs: Timeband (period) number.

  return: CString object containing the start and end minute.
********************************************************************/
CString CReportView::cstrGet_timeband_start_end_min(int iIndex)
  {
  CString cstrMinutes;
//  float fStart;

  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard version
    cstrMinutes.Format("%.1f to %.1f", 
                       m_pDoc->m_pdPatient.fGoodMinStart[iIndex]
                         + m_pDoc->m_pdPatient.fPeriodStart[iIndex],
                       m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
                         + m_pDoc->m_pdPatient.fPeriodStart[iIndex]);
                       //m_pDoc->m_pdPatient.fGoodMinStart[iIndex] + (iIndex * 10),
                       //m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] + (iIndex * 10));
    }
  else
    { // Research version
//    fStart = m_pDoc->m_pdPatient.fPeriodStart[iIndex];
    cstrMinutes.Format("%.1f to %.1f", 
                       m_pDoc->m_pdPatient.fGoodMinStart[iIndex], // + fStart,
                       m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]); // + fStart);
    }
  return(cstrMinutes);
  }
  
/********************************************************************
vFree_data_sheet_arrays - Free the memory used by the arrays allocated
                          for the data sheet calculations.

  inputs: None.

  return: None.
********************************************************************/
void CReportView::vFree_data_sheet_arrays()
  {

  if(m_adData.pfPwrSum != NULL)
    {
    HeapFree(GetProcessHeap(), 0, (LPVOID)m_adData.pfPwrSum);
    m_adData.pfPwrSum = NULL;
    }

  if(m_adData.pfRatioAvgPwr != NULL)
    {
    HeapFree(GetProcessHeap(), 0, (LPVOID)m_adData.pfRatioAvgPwr);
    m_adData.pfRatioAvgPwr = NULL;
    }
  
  if(m_adData.pfDomFreq != NULL)
    {
    HeapFree(GetProcessHeap(), 0, (LPVOID)m_adData.pfDomFreq);
    m_adData.pfDomFreq = NULL;
    }

  if(m_adData.pfPctDomFreq != NULL)
    {
    HeapFree(GetProcessHeap(), 0, (LPVOID)m_adData.pfPctDomFreq);
    m_adData.pfPctDomFreq = NULL;
    }
  }

int CReportView::iCreate_event_log(int iXStart, int iY)
  {
  CFont cfFont;
  TEXTMETRIC tm;
  CString cstrItem, cstrBaseTitle;
  CSize csText1;
  CEvnt *pEvent;
  short int iIndex;
  int iX, iX1, iHeight, iStartSecond, iEndSecond, iDescWidth;
  unsigned uIndex;

//  iY = iYStart;
//  iX = iXStart;

  if(m_pDC->IsPrinting() != 0)
    { // Printing.  Use a smaller font.
    vSet_font(NULL, m_pDC, &cfFont, FW_THIN, 10);
    m_pDC->SelectObject(&cfFont);
    }
  m_pDC->GetTextMetrics(&tm);

  iX = iXStart + (4 * tm.tmAveCharWidth);
  iX1 = iX + (40 * tm.tmAveCharWidth);

    // Print the key to the indications first.
  cstrItem = g_pLang->cstrLoad_string(ITEM_G_EVENT_LOG); //"Event Log";
  csText1 = m_pDC->GetTextExtent(cstrItem);
  iHeight = csText1.cy;
  m_pDC->TextOut(iXStart, iY, cstrItem);
  iY += (2 * iHeight);

  cstrItem = g_pLang->cstrLoad_string(ITEM_G_INDICATION_KEY); //"Key:";
  m_pDC->TextOut(iXStart, iY, cstrItem);
  iY += iHeight;

  cstrItem = g_pLang->cstrLoad_string(ITEM_G_COUGH_KEY); //"C: Cough";
  m_pDC->TextOut(iX, iY, cstrItem);
  cstrItem = g_pLang->cstrLoad_string(ITEM_G_INGESTION_KEY); //"I: Ingestion";;
  m_pDC->TextOut(iX1, iY, cstrItem);
  iY += iHeight;
  
  cstrItem = g_pLang->cstrLoad_string(ITEM_G_MOVEMENT_KEY); //"M: Movement";
  m_pDC->TextOut(iX, iY, cstrItem);
  cstrItem = g_pLang->cstrLoad_string(ITEM_G_NAUSEA_KEY); //"N: Nausea";
  m_pDC->TextOut(iX1, iY, cstrItem);
  iY += iHeight;

  cstrItem = g_pLang->cstrLoad_string(ITEM_G_PAIN_KEY); //"P: Pain";
  m_pDC->TextOut(iX, iY, cstrItem);
  cstrItem = g_pLang->cstrLoad_string(ITEM_G_TALKING_KEY); //"T: Talking";
  m_pDC->TextOut(iX1, iY, cstrItem);
  iY += iHeight;

  cstrItem = g_pLang->cstrLoad_string(ITEM_G_VOMITING_KEY); //"V: Vomiting";
  m_pDC->TextOut(iX, iY, cstrItem);
  cstrItem = g_pLang->cstrLoad_string(ITEM_G_OTHER_KEY); //"O: Other";
  m_pDC->TextOut(iX1, iY, cstrItem);
  iY += (2 * iHeight);

    // Start column for description.
  iX1 = iX + (20 * tm.tmAveCharWidth);

    // Calculate the description width. Leave a right margin.
//  csText1 = m_pDC->GetWindowExt();
//  csText1 = m_pDC->GetViewportExt();
  iDescWidth = m_csViewSize.cx - iX1 - (10 * tm.tmAveCharWidth);

    // Baseline
  //cstrItem.LoadString(IDS_BASELINE);
  cstrItem = g_pLang->cstrLoad_string(ITEM_G_BASELINE);
  m_pDC->TextOut(iXStart, iY, cstrItem);
  iY += iHeight;

  for(iIndex = 0; iIndex < m_pDoc->m_pdPatient.Events.m_BLEvents.GetCount(); ++iIndex)
    {
    pEvent = m_pDoc->m_pdPatient.Events.m_BLEvents[iIndex];
    cstrItem = cstrFmt_event_line(pEvent);
    m_pDC->TextOut(iX, iY, cstrItem);
    iY = iDisplay_multiline(iX1, iY, iDescWidth, pEvent->m_cstrDescription);
    if(pEvent->m_cstrDescription.IsEmpty() == TRUE)
      iY += iHeight;
//    m_pDC->TextOut(iX1, iY, pEvent->m_cstrDescription);
//    iY += iHeight;
    }

  iY += iHeight;
    // Post-stimulation periods
  //cstrBaseTitle.LoadString(IDS_POST_STIM_PERIOD);
  cstrBaseTitle = g_pLang->cstrLoad_string(ITEM_G_POST_STIM_PERIOD);
  for(uIndex = 0; uIndex < m_pDoc->m_pdPatient.uNumPeriods; ++uIndex)
    {
    cstrItem.Format("%s %d", cstrBaseTitle, uIndex + 1);
    m_pDC->TextOut(iXStart, iY, cstrItem);
    iY += iHeight;
    iStartSecond = (int)(m_pDoc->m_pdPatient.fPeriodStart[uIndex + 1] * (float)60.0);
    iEndSecond = iStartSecond
                 + (int)(m_pDoc->m_pdPatient.fPeriodLen[uIndex + 1] * (float)60.0);
    for(iIndex = 0; iIndex < m_pDoc->m_pdPatient.Events.m_PSEvents.GetCount(); ++iIndex)
      {
      pEvent = m_pDoc->m_pdPatient.Events.m_PSEvents[iIndex];
      if(pEvent->m_iSeconds >= iStartSecond && pEvent->m_iSeconds <= iEndSecond)
        {
        cstrItem = cstrFmt_event_line(pEvent);
        m_pDC->TextOut(iX, iY, cstrItem);
//        m_pDC->TextOut(iX1, iY, pEvent->m_cstrDescription);
        iY = iDisplay_multiline(iX1, iY, iDescWidth, pEvent->m_cstrDescription);
        if(pEvent->m_cstrDescription.IsEmpty() == TRUE)
          iY += iHeight;
//        iY += iHeight;
        }
      }
    iY += iHeight;
    }
  return(iY);
  }

/********************************************************************
cstrFmt_event_line - Format a line of Event data for the listbox.
                     Format:
                       - Minutes:Seconds
                       - dash
                       - Indications.

  inputs: Pointer to a CEvnt object for the event.
  
  return: CString object containing for formatted line.
********************************************************************/
CString CReportView::cstrFmt_event_line(CEvnt *pEvent)
  {
  CString cstrLine;
  int iMinutes;

  iMinutes = pEvent->m_iSeconds / 60;
  cstrLine.Format("%d:%02d - %s", iMinutes, pEvent->m_iSeconds - (iMinutes * 60),
                  pEvent->m_cstrIndications);
  return(cstrLine);
  }

/********************************************************************
cstrGet_col_title - Get the column title text based on the resouce ID.

  inputs: Resource ID of the column title text.
  
  return: CString object containing the column title text.
********************************************************************/
CString CReportView::cstrGet_col_title(unsigned int uiID)
  {
  CString cstrTitle, cstrItem;

  switch(uiID)
    {
    case IDS_1_25CPM:
      cstrItem = ITEM_G_1_25CPM;
      break;
    case IDS_25_375CPM:
      cstrItem = ITEM_G_25_375CPM;
      break;
    case IDS_375_100CPM:
      cstrItem = ITEM_G_375_100CPM;
      break;
    case IDS_100_150CPM:
      cstrItem = ITEM_G_100_150CPM;
      break;
    case IDS_1_15CPM:
      cstrItem = ITEM_G_1_15CPM;
      break;
    case IDS_MINUTES:
      cstrItem = ITEM_G_MINUTES;
      break;
    case IDS_BRADY:
      cstrItem = ITEM_G_BRADY;
      break;
    case IDS_NORMAL:
      cstrItem = ITEM_G_NORMAL;
      break;
    case IDS_TACHY:
      cstrItem = ITEM_G_TACHY;
      break;
    case IDS_DUOD:
      cstrItem = ITEM_G_DUOD;
      break;
    case IDS_ENTIRE_RANGE:
      cstrItem = ITEM_G_ENTIRE_RANGE;
      break;
    case IDS_PERIOD:
      cstrItem = ITEM_G_PERIOD;
      break;
    case IDS_0_300:
      cstrItem = ITEM_G_0_300;
      break;
    case IDS_325_600:
      cstrItem = ITEM_G_325_600;
      break;
    case IDS_625_1500:
      cstrItem = ITEM_G_625_1500;
      break;
    case IDS_0_15:
      cstrItem = ITEM_G_0_15;
      break;
    case IDS_SUMM_BRADY_TITLE:
      cstrItem = ITEM_G_SUMM_BRADY_TITLE;
      break;
    case IDS_SUMM_NORMAL_TITLE:
      cstrItem = ITEM_G_SUMM_NORMAL_TITLE;
      break;
     case IDS_SUMM_TACHY_TITLE:
      cstrItem = ITEM_G_SUMM_TACHY_TITLE;
      break;
    case IDS_SUMM_DUOD_TITLE:
      cstrItem = ITEM_G_SUMM_DUOD_TITLE;
      break;
    default:
      cstrItem = "";
      break;
    }
  if(cstrItem != "")
    cstrTitle = g_pLang->cstrLoad_string(cstrItem);
  else
    cstrTitle = "";
  return(cstrTitle);
  }

//unsigned int CReportView::uGet_num_epochs(float fStartMin, float fEndMin)
//  {
//  unsigned int uNumHalfMins, uNumOfPoints;
//
//  uNumOfPoints = ((unsigned)fEndMin - (unsigned)fStartMin - 3);
//  uNumHalfMins = (unsigned int)(fStartMin / 0.5F);
//  if((uNumHalfMins % 2) != 0)
//    --uNumOfPoints;
//  return(uNumOfPoints);
//  }