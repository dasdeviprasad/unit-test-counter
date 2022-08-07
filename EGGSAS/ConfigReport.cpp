/********************************************************************
ConfigReport.cpp

Copyright (C) 2003,2004,2005,2007,2011,2012,2013,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CConfigReport class.

  This class allows the user to configure reports.

  The report options are:
    Core report: always checked so its always displayed and printed.
    Entire raw data signal
    Data sheet

HISTORY:
14-NOV-03  RET  New.
16-MAR-04  RET  Add help button and message handler.
05-FEB-05  RET  Change dialog box so user can select any individual items
                  for the report.
                  Add check boxes and items to the INI file for:
                    Diagnosis/Recommendations, RSA, Summary Graphs,
                    Raw Signal of Selected Minutes.
                Add method: vEnable_rpt_sections().
10-MAY-07  RET  Research Version
                  Change OnInitDialog() to
                    Read in Patient Info indicator from the INI file
                      for the Patient Information checkbox.
                    Disable and uncheck core reports and summary graphs.
                  Change OnOK() to save Patient Info indicator to the INI file.
27-SEP-11  RET
             Changes to write an ASCII raw data file and not allow any analysis.
               Disable the report items that relate to analysis.
                 Change method: OnInitDialog() 
16-FEB-12  RET  Version 
                  Changes for foreign languages.
22-AUG-12  RET
             Modify the report configuration so that the RSA and the Summary
             Graphs can either be on the first page of the report or on a
             subsequent page.
               Add Event handlers: OnBnClickedRptRsaFirstPage(),
                 OnBnClickedRptRsaOtherPage(), OnBnClickedRptSummGrphsFirstPage(),
                 OnBnClickedRptSummGrphsOtherPage()
               Change methods: OnInitDialog(), OnOK(), bReport_changed(),
                 vEnable_rpt_sections(), vSet_text()
18-SEP-12  RET
             Change to select the select raw signal if the core report
               is selected.
                 Change methods: OnInitDialog(), OnBnClickedCoreReport()
03-OCT-12  RET
             Change for 60/200 CPM
               For 200 CPM, RSA doesn't fit on page 1 of report, so always make
               if go on a subsequent page.
                 Change methods: OnInitDialog(), OnBnClickedCoreReport(),
                   vEnable_rpt_sections()
13-DEC-12  RET
             Change to allow summary graphs to be drawn for research version.
               Change methods: OnInitDialog(), OnBnClickedCoreReport(),
                 vEnable_rpt_sections()
14-DEC-12  RET
             Change to filter out frequencies below 10 cpm in the RSA.
               Add checkbox for printing/viewing both RSAs.
                 Add event handler: OnBnClickedRptRsaIncludeBoth()
                 Add variable: m_bBothRSA, m_bOrigBothRSA
               Change methods: OnInitDialog(), OnBnClickedCoreReport(),
                 vEnable_rpt_sections(), OnOK(), OnBnClickedRptRsaFirstPage(),
                 OnBnClickedRptRsaOtherPage(), vSet_text()
19-MAR-13  RET
             Changes for frequency range seleclion.
               Hide the "Include both filtered and unfiltered RSA..." checkbox
               if program is not built for 60/200 cpm hardware.
                 Change methods: OnInitDialog(), vEnable_rpt_sections()
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedConfigrptHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "ConfigReport.h"
#include ".\configreport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigReport dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the active document.
********************************************************************/
CConfigReport::CConfigReport(CEGGSASDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CConfigReport::IDD, pParent)
  , m_bCoreReport(FALSE)
  , m_bDiagRecom(FALSE)
  , m_bChkRSAFirstPage(FALSE)
  , m_bDataSheet(FALSE)
  , m_bChkSummGraphsFirstPage(FALSE)
  , m_bRawSigSelMin(FALSE)
  , m_bEntireRawSignal(FALSE)
  , m_bPatientInfo(FALSE)
  , m_bEventLog(FALSE)
  , m_bChkRSAOtherPage(FALSE)
  , m_bChkSummGrphsOtherPage(FALSE)
  , m_bBothRSA(FALSE)
  {
	//{{AFX_DATA_INIT(CConfigReport)
	//}}AFX_DATA_INIT

  m_pDoc = pDoc;
  }


void CConfigReport::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
//{{AFX_DATA_MAP(CConfigReport)
//}}AFX_DATA_MAP
DDX_Check(pDX, IDC_CORE_REPORT, m_bCoreReport);
DDX_Check(pDX, IDC_RPT_DIAG_RECOM, m_bDiagRecom);
DDX_Check(pDX, IDC_RPT_RSA_FIRST_PAGE, m_bChkRSAFirstPage);
DDX_Check(pDX, IDC_RPT_DATA_SHEET, m_bDataSheet);
DDX_Check(pDX, IDC_RPT_SUMM_GRPHS_FIRST_PAGE, m_bChkSummGraphsFirstPage);
DDX_Check(pDX, IDC_RPT_RAW_SIG_SEL_MIN, m_bRawSigSelMin);
DDX_Check(pDX, IDC_RPT_ENTIRE_RAW_SIGNAL, m_bEntireRawSignal);
DDX_Check(pDX, IDC_RPT_PATIENT_INFO, m_bPatientInfo);
DDX_Check(pDX, IDC_RPT_EVENT_LOG, m_bEventLog);
DDX_Check(pDX, IDC_RPT_RSA_OTHER_PAGE, m_bChkRSAOtherPage);
DDX_Check(pDX, IDC_RPT_SUMM_GRPHS_OTHER_PAGE, m_bChkSummGrphsOtherPage);
DDX_Check(pDX, IDC_RPT_RSA_INCLUDE_BOTH, m_bBothRSA);
}

BEGIN_MESSAGE_MAP(CConfigReport, CDialog)
	//{{AFX_MSG_MAP(CConfigReport)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_CONFIGRPT_HELP, OnBnClickedConfigrptHelp)
  ON_BN_CLICKED(IDC_CORE_REPORT, OnBnClickedCoreReport)
  ON_BN_CLICKED(IDC_RPT_RSA_FIRST_PAGE, OnBnClickedRptRsaFirstPage)
  ON_BN_CLICKED(IDC_RPT_RSA_OTHER_PAGE, OnBnClickedRptRsaOtherPage)
  ON_BN_CLICKED(IDC_RPT_SUMM_GRPHS_FIRST_PAGE, OnBnClickedRptSummGrphsFirstPage)
  ON_BN_CLICKED(IDC_RPT_SUMM_GRPHS_OTHER_PAGE, OnBnClickedRptSummGrphsOtherPage)
  ON_BN_CLICKED(IDC_RPT_RSA_INCLUDE_BOTH, OnBnClickedRptRsaIncludeBoth)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigReport message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Read the report configuration from the INI file and set the dialog
      box fields appropriately.
********************************************************************/
BOOL CConfigReport::OnInitDialog() 
  {
  int iValue;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();
    // Get the initial checkbox values from the INI file.
    // Core report
  iValue = g_pConfig->iGet_item(CFG_COREREPORT);
  if(iValue == 0)
    m_bCoreReport = FALSE;
  else
    m_bCoreReport = TRUE;
	m_bOrigCoreReport = m_bCoreReport;
    // Include data sheet
  iValue = g_pConfig->iGet_item(CFG_DATASHEET);
  if(iValue == 0 || g_bAsciiDataFile == true)
    m_bDataSheet = FALSE;
  else
    m_bDataSheet = TRUE;
	m_bOrigDataSheet = m_bDataSheet;
    // Include entire raw signal
  iValue = g_pConfig->iGet_item(CFG_ENTIRERAWSIGNAL);
  if(iValue == 0)
    m_bEntireRawSignal = FALSE;
  else
    m_bEntireRawSignal = TRUE;
	m_bOrigEntireRawSignal = m_bEntireRawSignal;
    // Include diagnosis/recommendations
  iValue = g_pConfig->iGet_item(CFG_DIAGRECOMM);
  if(iValue == 0 || g_bAsciiDataFile == true)
    m_bDiagRecom = FALSE;
  else
    m_bDiagRecom = TRUE;
	m_bOrigDiagRecom = m_bDiagRecom;
    // Include RSA on first page of report
  iValue = g_pConfig->iGet_item(CFG_RSA_FIRST_PAGE);
  if(iValue == 0 || g_bAsciiDataFile == true)
    m_bChkRSAFirstPage = FALSE;
  else
    m_bChkRSAFirstPage = TRUE;
  m_bOrigRSAFirstPage = m_bChkRSAFirstPage;
    // Include RSA on a subsequent page of report
  iValue = g_pConfig->iGet_item(CFG_RSA_OTHER_PAGE);
  if(iValue == 0 || g_bAsciiDataFile == true)
    m_bChkRSAOtherPage = FALSE;
  else
    m_bChkRSAOtherPage = TRUE;
  m_bOrigRSAOtherPage = m_bChkRSAOtherPage;
    // Include Summary Graphs on first page of report
  iValue = g_pConfig->iGet_item(CFG_SUMM_GRAPHS_FIRST_PAGE);
  if(iValue == 0 || g_bAsciiDataFile == true)
    m_bChkSummGraphsFirstPage = FALSE;
  else
    m_bChkSummGraphsFirstPage = TRUE;
	m_bOrigSummGraphsFirstPage = m_bChkSummGraphsFirstPage;
    // Include Summary Graphs on a subsequent page of report
  iValue = g_pConfig->iGet_item(CFG_SUMM_GRAPHS_OTHER_PAGE);
  if(iValue == 0 || g_bAsciiDataFile == true)
    m_bChkSummGrphsOtherPage = FALSE;
  else
    m_bChkSummGrphsOtherPage = TRUE;
	m_bOrigSummGraphsFirstPage = m_bChkSummGrphsOtherPage;

    // Include raw signal of selected minutes.
  if(m_bCoreReport == TRUE)
    m_bRawSigSelMin = TRUE; // Core report is selected, select selected raw minutes.
  else
    {
    iValue = g_pConfig->iGet_item(CFG_RAWSIGSELMIN);
    if(iValue == 0 || g_bAsciiDataFile == true)
      m_bRawSigSelMin = FALSE;
    else
      m_bRawSigSelMin = TRUE;
	  m_bOrigRawSigSelMin = m_bRawSigSelMin;
    }

  //m_bCoreReport = FALSE; // Don't allow core report.
    // Get include/not include Patient Info from the INI file.
  iValue = g_pConfig->iGet_item(CFG_PATIENTINFO);
  if(iValue == 0)
    m_bPatientInfo = FALSE;
  else
    m_bPatientInfo = TRUE;
	m_bOrigPatientInfo = m_bPatientInfo;

  iValue = g_pConfig->iGet_item(CFG_BOTH_RSA);
  if(iValue == 0)
    m_bBothRSA = FALSE;
  else
    m_bBothRSA = TRUE;
	m_bOrigBothRSA = m_bBothRSA;
	
  if(theApp.m_ptStudyType == PT_RESEARCH)
    {
    iValue = g_pConfig->iGet_item(CFG_EVENTLOG);
    if(iValue == 0 || g_bAsciiDataFile == true)
      m_bEventLog = FALSE;
    else
      m_bEventLog = TRUE;
		m_bOrigEventLog = m_bEventLog;
    }

  // Always disable exept for new 60/200 cpm hardware and a research study
  GetDlgItem(IDC_RPT_RSA_INCLUDE_BOTH)->EnableWindow(FALSE);

  vEnable_rpt_sections(m_bCoreReport);

  if(theApp.m_ptStudyType == PT_RESEARCH)
    {
    if(m_pDoc->bCan_graph_summary_graphs() == false)
      {
      GetDlgItem(IDC_RPT_SUMM_GRPHS_FIRST_PAGE)->EnableWindow(FALSE);
      GetDlgItem(IDC_RPT_SUMM_GRPHS_OTHER_PAGE)->EnableWindow(FALSE);
      }
    //GetDlgItem(IDC_CORE_REPORT)->EnableWindow(FALSE);
    //GetDlgItem(IDC_RPT_DIAG_RECOM)->EnableWindow(FALSE);
    if(g_bAsciiDataFile == true)
      {
      GetDlgItem(IDC_RPT_RSA)->EnableWindow(FALSE);
      GetDlgItem(IDC_RPT_DATA_SHEET)->EnableWindow(FALSE);
      GetDlgItem(IDC_RPT_RAW_SIG_SEL_MIN)->EnableWindow(FALSE);
      GetDlgItem(IDC_RPT_EVENT_LOG)->EnableWindow(FALSE);
      }
    //m_bChkSummGraphsFirstPage = FALSE;
    }
  else // if(theApp.m_ptStudyType == PT_STANDARD)
    {
    //GetDlgItem(IDC_RPT_PATIENT_INFO)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_RPT_EVENT_LOG)->ShowWindow(SW_HIDE);
    if(g_bAsciiDataFile == true)
      {
      GetDlgItem(IDC_RPT_RSA_FIRST_PAGE)->EnableWindow(FALSE);
      GetDlgItem(IDC_RPT_RSA_OTHER_PAGE)->EnableWindow(FALSE);
      GetDlgItem(IDC_RPT_DATA_SHEET)->EnableWindow(FALSE);
      GetDlgItem(IDC_RPT_RAW_SIG_SEL_MIN)->EnableWindow(FALSE);
      GetDlgItem(IDC_RPT_SUMM_GRPHS_FIRST_PAGE)->EnableWindow(FALSE);
      GetDlgItem(IDC_RPT_SUMM_GRPHS_OTHER_PAGE)->EnableWindow(FALSE);
      //GetDlgItem(IDC_CORE_REPORT)->EnableWindow(FALSE);
      GetDlgItem(IDC_RPT_DIAG_RECOM)->EnableWindow(FALSE);
      }
    }

#ifdef _200_CPM_HARDWARE
  // If the 200 cpm range is selected:
  //   The RSA will not be in the first page of the core report.
  //   Always enable the check box for the RSA on a subsequent page.
  if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
    {
    GetDlgItem(IDC_RPT_RSA_FIRST_PAGE)->EnableWindow(FALSE);
    if(m_bCoreReport == TRUE)
      {
      GetDlgItem(IDC_RPT_RSA_OTHER_PAGE)->EnableWindow(FALSE);
      m_bChkRSAOtherPage = TRUE;
      m_bChkRSAFirstPage = FALSE;
      }
    }
  if(theApp.m_ptStudyType == PT_STANDARD)
    GetDlgItem(IDC_RPT_RSA_INCLUDE_BOTH)->EnableWindow(FALSE);
#else
  GetDlgItem(IDC_RPT_RSA_INCLUDE_BOTH)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_RPT_RSA_INCLUDE_BOTH)->EnableWindow(FALSE);
#endif
  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.
********************************************************************/
void CConfigReport::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

/********************************************************************
OnOK

  Message handler for the OK button.

  Write the report configuration items to the INI file.
********************************************************************/
void CConfigReport::OnOK() 
  {
  CString cstrCaption, cstrMsg;
  int iValue;

  UpdateData(TRUE);

  if(theApp.m_ptStudyType == PT_STANDARD)
    {
    if(m_bCoreReport == FALSE && m_bPatientInfo == FALSE
    && m_bDataSheet == FALSE && m_bEntireRawSignal == FALSE
    && m_bDiagRecom == FALSE && m_bChkRSAFirstPage == FALSE
    && m_bChkSummGraphsFirstPage == FALSE && m_bRawSigSelMin == FALSE
    && m_bChkSummGrphsOtherPage == FALSE
    && m_bChkRSAOtherPage == FALSE)
      { // Everything has been unchecked, warn the user and check
        // the core report as a default.
      //cstrCaption.LoadString(IDS_WARNING);
      //cstrMsg.LoadString(IDS_CORE_RPT_SELECTED);
      cstrCaption = g_pLang->cstrLoad_string(ITEM_G_WARNING);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_CORE_RPT_SELECTED);
      MessageBox(cstrMsg, cstrCaption, MB_OK);
      m_bCoreReport = TRUE;
      }
    }

    // Update the INI file
  if(m_bCoreReport == FALSE)
    iValue = 0;
  else
    iValue = 1;
  //AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_CORE_REPORT, iValue);
  g_pConfig->vWrite_item(CFG_COREREPORT, iValue);

  if(m_bDataSheet == FALSE)
    iValue = 0;
  else
    iValue = 1;
  //AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_DATASHEET, iValue);
  g_pConfig->vWrite_item(CFG_DATASHEET, iValue);

  if(m_bEntireRawSignal == FALSE)
    iValue = 0;
  else
    iValue = 1;
  //AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_ENTIRE_RAW_SIGNAL, iValue);
  g_pConfig->vWrite_item(CFG_ENTIRERAWSIGNAL, iValue);

  if(m_bDiagRecom == FALSE)
    iValue = 0;
  else
    iValue = 1;
  //AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_DIAG_RECOM, iValue);
  g_pConfig->vWrite_item(CFG_DIAGRECOMM, iValue);
  
  if(m_bChkRSAFirstPage == FALSE)
    iValue = 0;
  else
    iValue = 1;
  //AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_RSA, iValue);
  g_pConfig->vWrite_item(CFG_RSA_FIRST_PAGE, iValue);

  if(m_bChkRSAOtherPage == FALSE)
    iValue = 0;
  else
    iValue = 1;
  g_pConfig->vWrite_item(CFG_RSA_OTHER_PAGE, iValue);
  // Update whether or not the RSA is in the report.
  if(m_bChkRSAFirstPage == TRUE || m_bChkRSAOtherPage == TRUE
    || m_bBothRSA == TRUE)
    g_pConfig->vWrite_item(CFG_RSA, 1);
  else
    g_pConfig->vWrite_item(CFG_RSA, 0);
  
  if(m_bChkSummGraphsFirstPage == FALSE)
    iValue = 0;
  else
    iValue = 1;
  //AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_SUMM_GRAPHS, iValue);
  g_pConfig->vWrite_item(CFG_SUMM_GRAPHS_FIRST_PAGE, iValue);

  if(m_bChkSummGrphsOtherPage == FALSE)
    iValue = 0;
  else
    iValue = 1;
  g_pConfig->vWrite_item(CFG_SUMM_GRAPHS_OTHER_PAGE, iValue);
  // Update whether or not the RSA is in the report.
  if(m_bChkSummGraphsFirstPage == TRUE || m_bChkSummGrphsOtherPage == TRUE)
    g_pConfig->vWrite_item(CFG_SUMMGRAPHS, 1);
  else
    g_pConfig->vWrite_item(CFG_SUMMGRAPHS, 0);

  if(m_bRawSigSelMin == FALSE)
    iValue = 0;
  else
    iValue = 1;
  //AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_RAW_SIG_SEL_MIN, iValue);
  g_pConfig->vWrite_item(CFG_RAWSIGSELMIN, iValue);

  if(m_bPatientInfo == FALSE)
    iValue = 0;
  else
    iValue = 1;
  //AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_PATIENT_INFO, iValue);
  g_pConfig->vWrite_item(CFG_PATIENTINFO, iValue);
  
  if(theApp.m_ptStudyType == PT_RESEARCH)
    {
    if(m_bEventLog == FALSE)
      iValue = 0;
    else
      iValue = 1;
    //AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_EVENT_LOG, iValue);
    g_pConfig->vWrite_item(CFG_EVENTLOG, iValue);
    }

  if(m_bBothRSA == FALSE)
    iValue = 0;
  else
    iValue = 1;
  g_pConfig->vWrite_item(CFG_BOTH_RSA, iValue);

	CDialog::OnOK();
  }

/********************************************************************
OnBnClickedConfigrptHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CConfigReport::OnBnClickedConfigrptHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_CONFIGRPT_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_CONFIGRPT_HELP);
  }

/********************************************************************
OnBnClickedCoreReport - Handler for the "Core Report" checkbox.

   Enable/disable report item checkboxes.
********************************************************************/
void CConfigReport::OnBnClickedCoreReport()
  {

  UpdateData(TRUE);
  vEnable_rpt_sections(m_bCoreReport);
  m_bRawSigSelMin = TRUE;
  if(m_bCoreReport == TRUE)
    {
    m_bBothRSA = FALSE;
    if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
      m_bChkRSAOtherPage = TRUE;
    else
      {
      m_bChkRSAOtherPage = FALSE;
      m_bChkRSAFirstPage = TRUE;
      if(theApp.m_ptStudyType == PT_RESEARCH)
        {
        if(m_pDoc->bCan_graph_summary_graphs() == false)
          m_bChkSummGraphsFirstPage = FALSE;
        else
          m_bChkSummGraphsFirstPage = TRUE;
        }
      else
        m_bChkSummGraphsFirstPage = TRUE;
      m_bChkSummGrphsOtherPage = FALSE;
      }
    }
  UpdateData(FALSE);
  //if(m_bCoreReport == TRUE)
  //  { // Core Report box is checked, disable all other custom check boxes.
  //  vEnable_rpt_sections(FALSE);
  //  }
  //else
  //  { // Core Report box is not checked, enable all other custom check boxes.
  //  vEnable_rpt_sections(TRUE);
  //  }
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
bReport_changed - Determines if any of the report has changed.

  inputs: None.

  return: true if the report has changed.
          false if there is no change in the report.
********************************************************************/
bool CConfigReport::bReport_changed()
  {
  bool bRet;

	if(m_bOrigDataSheet != m_bDataSheet
	|| m_bOrigEntireRawSignal != m_bEntireRawSignal
  || m_bOrigCoreReport != m_bCoreReport
  || m_bOrigDiagRecom != m_bDiagRecom
  || m_bOrigRSAFirstPage != m_bChkRSAFirstPage
  || m_bOrigRSAOtherPage != m_bChkRSAOtherPage
  || m_bOrigSummGraphsFirstPage != m_bChkSummGraphsFirstPage
  || m_bOrigSummGrphsOtherPage != m_bChkSummGrphsOtherPage
  || m_bOrigRawSigSelMin != m_bRawSigSelMin
  || m_bPatientInfo != m_bOrigPatientInfo
  || m_bEventLog != m_bOrigEventLog)
    bRet = true;
  else
    bRet = false;

  return(bRet);
  }

/********************************************************************
vEnable_rpt_sections - Enable/disable the check boxes for the custom
                       report sections.

  inputs: TRUE if the check boxes are to be enabled, otherwise FALSE.

  return: None.
********************************************************************/
void CConfigReport::vEnable_rpt_sections(BOOL bCoreReport)
  {

  if(bCoreReport == TRUE)
    { // core report is checked.
    GetDlgItem(IDC_RPT_RSA_FIRST_PAGE)->EnableWindow(FALSE);
    GetDlgItem(IDC_RPT_RSA_OTHER_PAGE)->EnableWindow(FALSE);
    GetDlgItem(IDC_RPT_RSA_INCLUDE_BOTH)->EnableWindow(FALSE);
    GetDlgItem(IDC_RPT_SUMM_GRPHS_FIRST_PAGE)->EnableWindow(FALSE);
    GetDlgItem(IDC_RPT_SUMM_GRPHS_OTHER_PAGE)->EnableWindow(FALSE);
    GetDlgItem(IDC_RPT_PATIENT_INFO)->EnableWindow(FALSE);
    GetDlgItem(IDC_RPT_DIAG_RECOM)->EnableWindow(FALSE);
    GetDlgItem(IDC_RPT_DATA_SHEET)->EnableWindow(TRUE);
    GetDlgItem(IDC_RPT_RAW_SIG_SEL_MIN)->EnableWindow(TRUE);
    GetDlgItem(IDC_RPT_ENTIRE_RAW_SIGNAL)->EnableWindow(TRUE);
    }
  else
    { // Core report is not checked.
    GetDlgItem(IDC_RPT_RSA_FIRST_PAGE)->EnableWindow(TRUE);
    GetDlgItem(IDC_RPT_RSA_OTHER_PAGE)->EnableWindow(TRUE);
    GetDlgItem(IDC_RPT_RSA_INCLUDE_BOTH)->EnableWindow(TRUE);
    if(theApp.m_ptStudyType == PT_RESEARCH)
    {
      if(m_pDoc->bCan_graph_summary_graphs() == false)
      {
        GetDlgItem(IDC_RPT_SUMM_GRPHS_FIRST_PAGE)->EnableWindow(FALSE);
        GetDlgItem(IDC_RPT_SUMM_GRPHS_OTHER_PAGE)->EnableWindow(FALSE);
      }
      else
        {
        GetDlgItem(IDC_RPT_SUMM_GRPHS_FIRST_PAGE)->EnableWindow(TRUE);
        GetDlgItem(IDC_RPT_SUMM_GRPHS_OTHER_PAGE)->EnableWindow(TRUE);
        }
      if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
        GetDlgItem(IDC_RPT_RSA_FIRST_PAGE)->EnableWindow(FALSE);
    }
    else
    {
      GetDlgItem(IDC_RPT_SUMM_GRPHS_FIRST_PAGE)->EnableWindow(TRUE);
      GetDlgItem(IDC_RPT_SUMM_GRPHS_OTHER_PAGE)->EnableWindow(TRUE);
    }
    GetDlgItem(IDC_RPT_PATIENT_INFO)->EnableWindow(TRUE);
    GetDlgItem(IDC_RPT_DIAG_RECOM)->EnableWindow(TRUE);
    GetDlgItem(IDC_RPT_DATA_SHEET)->EnableWindow(TRUE);
    GetDlgItem(IDC_RPT_RAW_SIG_SEL_MIN)->EnableWindow(TRUE);
    GetDlgItem(IDC_RPT_ENTIRE_RAW_SIGNAL)->EnableWindow(TRUE);
    }
#ifndef _200_CPM_HARDWARE
  GetDlgItem(IDC_RPT_RSA_INCLUDE_BOTH)->EnableWindow(FALSE);
  GetDlgItem(IDC_RPT_RSA_INCLUDE_BOTH)->ShowWindow(SW_HIDE);
#endif
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CConfigReport::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_CUSTOM_REPORT);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_CONFIG_REPORT));

    GetDlgItem(IDC_CORE_REPORT)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_CORE_REPORT));
    GetDlgItem(IDC_STATIC_INSTRUCTIONS)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_INSTRUCTIONS));
    GetDlgItem(IDC_RPT_PATIENT_INFO)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_PATIENT_INFO));
    GetDlgItem(IDC_RPT_DIAG_RECOM)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_DIAG_RECOMM));
    //GetDlgItem(IDC_RPT_RSA)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_RSA));
    GetDlgItem(IDC_RPT_DATA_SHEET)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_DATA_SHEET));
    //GetDlgItem(IDC_RPT_SUMM_GRAPHS)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_SUMM_GRAPHS));
    GetDlgItem(IDC_RPT_RAW_SIG_SEL_MIN)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_RAW_SIGNAL_SEL_MIN));
    GetDlgItem(IDC_RPT_ENTIRE_RAW_SIGNAL)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_ENTIRE_RAW_SIGNAL));
    GetDlgItem(IDC_RPT_EVENT_LOG)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_EVENT_LOG));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CLOSE));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_CONFIGRPT_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));

    GetDlgItem(IDC_STATIC_INSTRUCTIONS2)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_INSTRUCTIONS2));
    GetDlgItem(IDC_STATIC_RSA_TITLE)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_RSA));
    GetDlgItem(IDC_RPT_RSA_FIRST_PAGE)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_FIRST_PAGE));
    GetDlgItem(IDC_RPT_RSA_OTHER_PAGE)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_OTHER_PAGE));
    GetDlgItem(IDC_STATIC_SUMM_GRPHS_TITLE)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_SUMM_GRAPHS));
    GetDlgItem(IDC_RPT_SUMM_GRPHS_FIRST_PAGE)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_FIRST_PAGE));
    GetDlgItem(IDC_RPT_SUMM_GRPHS_OTHER_PAGE)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_OTHER_PAGE));
    GetDlgItem(IDC_RPT_RSA_INCLUDE_BOTH)->SetWindowText(g_pLang->cstrGet_text(ITEM_CR_INCLUDE_BOTH_RSA));
    //g_pLang->vEnd_section();
    }
  }

/********************************************************************
OnBnClickedRptRsaFirstPage - Event handler for the Include RSA on the
                             First Page of the report checkbox.
                             
                             If this is checked, make sure the Include RSA
                             on a subsequent Page of the report is unchecked.

  inputs: None.

  return: None.
********************************************************************/
void CConfigReport::OnBnClickedRptRsaFirstPage()
  {
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    if(m_bChkRSAFirstPage == TRUE)
    {
      m_bChkRSAOtherPage = FALSE;
      m_bBothRSA = FALSE;
      UpdateData(FALSE);
    }
  }

/********************************************************************
OnBnClickedRptRsaOtherPage - Event handler for the Include RSA on a
                             subsequent Page of the report checkbox.
                             
                             If this is checked, make sure the Include RSA
                             on the First Page of the report is unchecked.

  inputs: None.

  return: None.
********************************************************************/
void CConfigReport::OnBnClickedRptRsaOtherPage()
  {
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    if(m_bChkRSAOtherPage == TRUE)
    {
      m_bChkRSAFirstPage = FALSE;
      m_bBothRSA = FALSE;
      UpdateData(FALSE);
    }
  }

/********************************************************************
OnBnClickedRptSummGrphsFirstPage - Event handler for the Include Summary
                                   Graphs on the First Page of the report
                                   checkbox.
                             
                                   If this is checked, make sure the Include
                                   Summary Graphs on a subsequent Page of the
                                   report is unchecked.

  inputs: None.

  return: None.
********************************************************************/
void CConfigReport::OnBnClickedRptSummGrphsFirstPage()
  {
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    if(m_bChkSummGraphsFirstPage == TRUE)
    {
      m_bChkSummGrphsOtherPage = FALSE;
      UpdateData(FALSE);
    }
  }

/********************************************************************
OnBnClickedRptSummGrphsFirstPage - Event handler for the Include Summary
                                   Graphs on a subsequent Page of the report
                                   checkbox.
                             
                                   If this is checked, make sure the Include
                                   Summary Graphs on the First Page of the
                                   report is unchecked.

  inputs: None.

  return: None.
********************************************************************/
void CConfigReport::OnBnClickedRptSummGrphsOtherPage()
  {
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    if(m_bChkSummGrphsOtherPage == TRUE)
    {
      m_bChkSummGraphsFirstPage = FALSE;
      UpdateData(FALSE);
    }
  }

void CConfigReport::OnBnClickedRptRsaIncludeBoth()
  {
  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  if(m_bBothRSA == TRUE)
    { // If printing/viewing both RSAs, they can't be on first page of report,
      // so uncheck the on first page checkbox and check RSA on second page.
    m_bChkRSAFirstPage = FALSE;
    m_bChkRSAOtherPage = FALSE;
    }
    UpdateData(FALSE);
 }
