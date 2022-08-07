/********************************************************************
ReportView.h

Copyright (C) 2003,2004,2005,2007,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CReportView class.

HISTORY:
08-NOV-03  RET  New.
20-FEB-04  RET  Add class variables:
                  m_iSummaryGraphStart, m_iRSAGraphStart, m_iSelRawDataGraphStartY
05-FEB-05  RET  Add class variables:
                  m_iIncludeEntireRawSignal, m_iIncludeDataSheet, m_iIncludeCoreReport,
                  m_iIncludeDiagRecom, m_iIncludeRSA, m_iIncludeSummGraphs
                  m_iIncludeRawSigSelMin
06-FEB-05  RET  Changes to add viewing the summary graphs and the data sheet
                  from the View menu and icons.
                  Add definitions:
                    RPT_VIEW_ITEM_NONE, RPT_VIEW_ITEM_SUMM_GRPHS,
                    RPT_VIEW_ITEM_DATA_SHEET
05-MAY-07  RET  Changed size of m_hPESelRawData[} from MAX_PERIODS to 
                  TOTAL_PERIODS to allow the fourth period.
10-MAY-07  RET  Research Version
                  Add class variables: m_iIncludePatientInfo, m_uMaxPeriods
                  Add prototype for cstrGet_timeband_start_end_min()
29-JUL-08  RET  Version 2.05b
                  Add class variable m_bAddedEndOfPage.
21-FEB-12  RET  Version 
                  Changes for foreign languages.
14-DEC-12  RET
             Change to filter out frequencies below 10 cpm in the RSA.
                  Add class variable m_iRSAGraphStart1, m_pcwndRSA1.
********************************************************************/

#if !defined(AFX_REPORTVIEW_H__E4FE9F64_A366_4C30_8BF2_E3A8F532C54F__INCLUDED_)
#define AFX_REPORTVIEW_H__E4FE9F64_A366_4C30_8BF2_E3A8F532C54F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

  // Colors for the patient and control range for the graphs.
#define PATIENT_COLOR        BLUE_RGB
#define CONTROL_RANGE_COLOR  DARK_RED_RGB

#define SUMM_GRPH_WIDTH 300 // Width of 1 summary graph.
#define SUMM_GRAPH_HEIGHT 200 // Height of 1 summary graph
#define SUMM_GRPH_HORIZ_SEP 10 // Separation between 2 horizontal summary graphs.
#define SUMM_GRPH_VERT_SEP 20 // Separation between 2 vertical summary graphs.

#define NUM_RAW_DATA_GRAPHS_PER_PAGE 3 // Number of raw data graphs per page.

// Defines for items that can be view via the View menu.
#define RPT_VIEW_ITEM_NONE 0
#define RPT_VIEW_ITEM_SUMM_GRPHS 1
#define RPT_VIEW_ITEM_DATA_SHEET 2

/////////////////////////////////////////////////////////////////////////////
// CReportView view

class CReportView : public CScrollView
{
protected:
	CReportView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CReportView)

// Attributes
public:
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  CDC *m_pDC; // Pointer to the CDC object that the report is being
              // drawn on.
  ANALYZED_DATA m_adData; // Hold information and data for analysis.
  HWND m_hPESum[CONTROL_FREQ_BANDS]; // Contains the windows handles for
                                     // the summary graphs.
  HWND m_hPESelRawData[MAX_TOTAL_PERIODS]; // Contains the windows handles for
                                           // the raw data graphs for the selected
                                           // minutes.
  HWND *m_phPERawData; // Pointer to a array of window handles for the
                       // raw data graphs.
  unsigned m_uNumRawDataGraphs; // Number of raw data graphs.
  CSize m_csViewSize; // Size of the report.
  int m_iLeftWidth; // Width in pixels of the left side.
  int m_iRightWidth; // Width in pixels fo the right side.

  RSA_DATA m_rsaData; // Data needed by the RSA graphing class.
  CWnd *m_pcwndRSA; // Window to draw the RSA in for displaying on the screen.
  CWnd *m_pcwndRSA1; // Window to draw a second RSA in for displaying on the screen.

  int m_iLineExt;  // Width of display (or paper) in pixels.
  int m_iLeftSideUseableWidth; // Useable width of the left side.

  short int m_iNumPages; // Number of pages in the report.
  int m_iRawDataGraphStartY; // Y coordinate where the raw data graphs start.
  int m_iRawDataGraphWidth; // Width in pixels of a raw data graph.
  int m_iSummaryGraphStart; // Y coordinate where the summary graphs start.
  int m_iRSAGraphStart; // Y coordinate where the first RSA graph starts.
  int m_iRSAGraphStart1; // Y coordinate where the second RSA graph starts.
  int m_iSelRawDataGraphStartY; // Y coordinate where the raw data graphs
                                // for selected minutes start.
    // The following items are for including the various sections in the report.
  int m_iIncludeEntireRawSignal;
  int m_iIncludeDataSheet;
  int m_iIncludeCoreReport;
  int m_iIncludeDiagRecom;
  int m_iIncludeRSA;
  int m_iIncludeSummGraphs;
  int m_iIncludeRawSigSelMin;
  unsigned short m_uMaxPeriods; // Maximum number of periods (timebands).
    // Research Version only.
  int m_iIncludePatientInfo;
  int m_iIncludeEventLog;
    // End of page flag.  true if end of page was added, otherwise false.
  bool m_bAddedEndOfPage;

// Operations
public:
  void ResyncScrollSizes();
  void vSet_graph_summ_characteristics(HWND hGraph, unsigned int iTitleResourceID);
  void vAnalyze_data(short int iAnalysisType);

//  void vAnalyze_data_for_summary();
  int iCenter_text(CString cstrItem, int iColWidth);
  int iCreate_summary_table(int iXStart, int iYStart, int iSummGrphsWidth);
  int iCreate_summary_graphs(int iXStart, int iYStart);
  void vDraw_summ_graph(HWND hGraph, short int iGrphIndx);
  int iCreate_data_sheet(int iXStart, int iYStart, int iPage, int iTotalPages);
  void vAnalyze_data_for_RSA();
  int iCreate_RSA(int iXStart, int iYStart, int iGraphNum);
  int iCreate_sel_raw_data_graphs(int iXStart, int iYStart, short int *piPage);
  int iCreate_event_log(int iXStart, int iY);
  int iAdd_facility(int iXStart, int iYStart);
  int iAdd_left_side(int iXStart, int iYStart);
  int iDisplay_multiline(int iX, int iY, int iWidth, CString cstrInfo);
  void vPrint_report();
  void vPrint_graph(HWND hPE, RECT *prRect);
  int iEnd_of_page(int iX, int iY, short int iPage);
  int iCreate_all_raw_data_graphs(int iX, int iY, short int *piPage);
  short int iAlloc_raw_data_graphs(void);
  void vReset_view(void);
  int iCalc_print_graph_height(int iYStart);
  int iCalc_print_graph_width(int iXStart);
  CString cstrGet_timeband_start_end_min(int iIndex);
  void vFree_data_sheet_arrays();
  CString cstrFmt_event_line(CEvnt *pEvent);
  bool bSkip_datasheet_column(int iColIndex);
  CString cstrGet_col_title(unsigned int uiID);
  int iAdd_title(int iXStart, int iYStart);
  void vCreate_RSA_wins(int iYStart);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CReportView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CReportView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  int iAdd_header(int iXStart, int iYStart);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTVIEW_H__E4FE9F64_A366_4C30_8BF2_E3A8F532C54F__INCLUDED_)
