/********************************************************************
MainFrm.cpp

Copyright (C) 2003 - 2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CMainFrame class.

  This class implements the main window of the application. It holds the
  toolbar and the menus.  It also provides the message handlers for all
  the menu items.


HISTORY:
08-NOV-03  RET  New.
10-MAY-07  RET  Research Version
                  Add menu handlers for:
                    File Resume Study
                    Edit Stimulation medium and quantity
                    View Stimulation medium and quantity
                    GoTo next stimulation period
                    GoTo previous stimulation period
                    Change number of minutes per graph
                  Add functions:
                    cstrDelete_menu_item_by_menu_pos()
                    vDelete_toolbar_button()
05-FEB-08  RET  Version 2.03
                  Add methods:
                    OnEditWhatIf(), OnUpdateEditWhatIf(), vDisplay_what_if()
                  Remove methods:
                    OnAnalyzePostwater40(), OnUpdateAnalyzePostwater40(),
                    OnViewPostwater40(), OnUpdateViewPostwater40()
30-JUN-08  RET  Version 2.05
                  Enable File Print Preview to create a report view.
                    Add methods: vView_report(), OnFilePrintPreview(),
                      OnUpdateFilePrintPreview().
                  Enable File Print to print a report view.
                    Add methods: OnUpdateFilePrint(), OnFilePrint()
                  Change OnUpdateEditWhatIf() to enable the menu item if
                    there is a data file.
                  Change OnUpdateAnalyzeRsa() and OnUpdateViewRsa() to
                    only enable RSA if the current period is analyzed.
05-N0V-08  RET  Version 2.08a
                  Add File Menu events for starting the Study Export
                    program, OnFileExportStudies(), OnUpdateFileExportStudies().
                    Change vCreate_standard_toolbar(), vCreate_research_toolbar()
                      to remove the New study toolbar item.
                  Add help menu events for new menu item "About 3CPM EGGSAS Reader":
                    OnHelpAboutReader(), OnUpdateHelpAboutReader
14-JAN-09  RET  Version CDDRdr.2.08c
                  Add event handlers:
                    OnHelpAbout3cpmeggsascentraldatadepotreader()
                    OnUpdateHelpAbout3cpmeggsascentraldatadepotreader()
13-FEB-09  RET  Version 2.08i
                  Add methods: OnUpdateFileNewPatient(), OnUpdateFileExistPatient(),
                               vEnable_menu_item(), vEnable_toolbar_button()
23-SEP-11  RET
             Changes to write an ASCII raw data file and not allow any analysis.
               Add event handlers: OnFileSaverawdatainascii(),
                                   OnUpdateFileSaverawdatainascii(),
                                   OnFileSaverawdatainasciiWL(),
                                   OnUpdateFileSaverawdatainasciiWL()
             For Data File Dump disable all menu items that have to do with analysis.
               Change event handlers:
                 OnUpdateAnalyzeBaseline(), OnUpdateAnalyzeCurrent(),                  OnUpdateAnalyzeDiagnosis(), OnUpdateAnalyzePostwater10(),                  OnUpdateAnalyzePostwater20(), OnUpdateAnalyzePostwater30(),                  OnUpdateAnalyzeRsa(),
                 OnUpdateEditRespiration(), OnUpdateEditSuggestedDiag(),                  OnUpdateEditRecommendations(), OnUpdateEditWhatIf(),
                 OnUpdateEditPatientDiagHist()
                 OnUpdateViewDataSheet(), OnUpdateViewEvents(),                  OnUpdateViewPatientDiagHist(), OnUpdateViewRecommendations(),                  OnUpdateViewRespiration(), OnUpdateViewRsa(), OnUpdateViewSuggestedDiag(),
                 OnUpdateViewSummGraph(), OnViewDataSheet(),
                 OnUpdateReportCreatespreadsheetofdatasheet()
24-AUG-12  RET
             Change OnEditAmtWater() to redraw the report if user changed
               the amount of water or the units.
13-DEC-12  RET
             Change to allow summary graphs to be drawn for research version.
               Change methods: OnUpdateViewSummGraph(), OnViewSummGraph()
14-DEC-12  RET
             Change to filter out frequencies below 10 cpm in the RSA.
               Add method: vCheck_menu_item()
13-JAN-13  RET
             Create 60/200 Research/waterload version
             Remove Filter RSA toolbar icon for waterload study.
               Change methods: vCreate_standard_toolbar(), vCreate_research_toolbar()
27-FEB-13  RET
             Changes for hiding patients.
               Change so menu item is disabled if viewing hidden patients.
                Change methods: OnUpdateAnalyzeCurrent(), OnUpdateEditWhatIf()
                  OnUpdateAnalyzeDiagnosis(), OnUpdateAnalyzePostwater10(),
                  OnUpdateAnalyzePostwater20(), OnUpdateAnalyzePostwater30(),
                  OnUpdateEditAmtWater(), OnUpdateEditPatientDiagHist(),
                  OnUpdateEditPatientMeds(), OnUpdateEditPreStudyInfo(),
                  OnUpdateEditSuggestedDiag(), OnUpdateEditRecommendations(),
                  OnUpdateEditRespiration(), OnUpdateEditStimulationMedium(),
16-MAR-13  RET
             Changes for frequency range selection.
               Change to force the datasheet and report to be recalculated
               if a flag has been set.
                 Change methods: OnViewDataSheet(), vView_report(),
                   vCreate_standard_toolbar(), vCreate_research_toolbar()
16-MAR-13  RET
             Changes for frequency range selection.
               Change methods: OnUpdateViewSummGraph(), OnUpdateViewDataSheet(),
                 OnUpdateEditWhatIf(), OnViewSummGraph()
02-JUN-16  RET
             Add feature to hide study dates.
               Add methods: OnReportShowHideStudyDates(), 
                 OnUpdateReportShowHideStudyDates(),
                 vDisplay_hide_show_study_dates()
08-JUL-20  RET
             Change starting the StudyExport program to add the current
             path as a command line parameter.
               Change method: OnFileExportStudies()
********************************************************************/

#include "stdafx.h"
#include <process.h>
#include "EGGSAS.h"
#include "MainFrm.h"

#include "analyze.h"
#include "EGGSASDoc.h"
#include "GraphRSA.h"
#include "RSAView.h"
#include "TenMinView.h"
#include "ReportView.h"
#include "FreqRange.h"
#include ".\mainfrm.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame *g_pFrame; // Pointer to this object.

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_ANALYZE_RSA, OnAnalyzeRsa)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_ANALYZE_BASELINE, OnAnalyzeBaseline)
	ON_UPDATE_COMMAND_UI(ID_ANALYZE_BASELINE, OnUpdateAnalyzeBaseline)
	ON_COMMAND(ID_ANALYZE_CURRENT, OnAnalyzeCurrent)
	ON_UPDATE_COMMAND_UI(ID_ANALYZE_CURRENT, OnUpdateAnalyzeCurrent)
	ON_COMMAND(ID_ANALYZE_DIAGNOSIS, OnAnalyzeDiagnosis)
	ON_UPDATE_COMMAND_UI(ID_ANALYZE_DIAGNOSIS, OnUpdateAnalyzeDiagnosis)
	ON_COMMAND(ID_ANALYZE_POSTWATER_10, OnAnalyzePostwater10)
	ON_UPDATE_COMMAND_UI(ID_ANALYZE_POSTWATER_10, OnUpdateAnalyzePostwater10)
	ON_COMMAND(ID_ANALYZE_POSTWATER_20, OnAnalyzePostwater20)
	ON_UPDATE_COMMAND_UI(ID_ANALYZE_POSTWATER_20, OnUpdateAnalyzePostwater20)
	ON_COMMAND(ID_ANALYZE_POSTWATER_30, OnAnalyzePostwater30)
	ON_UPDATE_COMMAND_UI(ID_ANALYZE_POSTWATER_30, OnUpdateAnalyzePostwater30)
	ON_UPDATE_COMMAND_UI(ID_ANALYZE_RSA, OnUpdateAnalyzeRsa)
	ON_COMMAND(ID_EDIT_AMT_WATER, OnEditAmtWater)
	ON_UPDATE_COMMAND_UI(ID_EDIT_AMT_WATER, OnUpdateEditAmtWater)
	ON_COMMAND(ID_EDIT_PATIENT_DEMOGRAHICS, OnEditPatientDemograhics)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PATIENT_DEMOGRAHICS, OnUpdateEditPatientDemograhics)
	ON_COMMAND(ID_EDIT_PATIENT_DIAG_HIST, OnEditPatientDiagHist)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PATIENT_DIAG_HIST, OnUpdateEditPatientDiagHist)
	ON_COMMAND(ID_EDIT_PATIENT_MEDS, OnEditPatientMeds)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PATIENT_MEDS, OnUpdateEditPatientMeds)
	ON_COMMAND(ID_EDIT_PRE_STUDY_INFO, OnEditPreStudyInfo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PRE_STUDY_INFO, OnUpdateEditPreStudyInfo)
	ON_COMMAND(ID_EDIT_SUGGESTED_DIAG, OnEditSuggestedDiag)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SUGGESTED_DIAG, OnUpdateEditSuggestedDiag)
	ON_COMMAND(ID_GOTO_END, OnGotoEnd)
	ON_UPDATE_COMMAND_UI(ID_GOTO_END, OnUpdateGotoEnd)
	ON_COMMAND(ID_GOTO_NEXT_PERIOD, OnGotoNextPeriod)
	ON_UPDATE_COMMAND_UI(ID_GOTO_NEXT_PERIOD, OnUpdateGotoNextPeriod)
	ON_COMMAND(ID_GOTO_PREV_PERIOD, OnGotoPrevPeriod)
	ON_UPDATE_COMMAND_UI(ID_GOTO_PREV_PERIOD, OnUpdateGotoPrevPeriod)
	ON_COMMAND(ID_GOTO_START, OnGotoStart)
	ON_UPDATE_COMMAND_UI(ID_GOTO_START, OnUpdateGotoStart)
	ON_COMMAND(ID_VIEW_AMT_WATER, OnViewAmtWater)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AMT_WATER, OnUpdateViewAmtWater)
	ON_COMMAND(ID_VIEW_BASELINE, OnViewBaseline)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BASELINE, OnUpdateViewBaseline)
	ON_COMMAND(ID_VIEW_PATIENT_DEMOGRAHICS, OnViewPatientDemograhics)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PATIENT_DEMOGRAHICS, OnUpdateViewPatientDemograhics)
	ON_COMMAND(ID_VIEW_PATIENT_DIAG_HIST, OnViewPatientDiagHist)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PATIENT_DIAG_HIST, OnUpdateViewPatientDiagHist)
	ON_COMMAND(ID_VIEW_PATIENT_MEDS, OnViewPatientMeds)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PATIENT_MEDS, OnUpdateViewPatientMeds)
	ON_COMMAND(ID_VIEW_POSTWATER_10, OnViewPostwater10)
	ON_UPDATE_COMMAND_UI(ID_VIEW_POSTWATER_10, OnUpdateViewPostwater10)
	ON_COMMAND(ID_VIEW_POSTWATER_20, OnViewPostwater20)
	ON_UPDATE_COMMAND_UI(ID_VIEW_POSTWATER_20, OnUpdateViewPostwater20)
	ON_COMMAND(ID_VIEW_POSTWATER_30, OnViewPostwater30)
	ON_UPDATE_COMMAND_UI(ID_VIEW_POSTWATER_30, OnUpdateViewPostwater30)
	ON_COMMAND(ID_VIEW_PRE_STUDY_INFO, OnViewPreStudyInfo)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PRE_STUDY_INFO, OnUpdateViewPreStudyInfo)
	ON_COMMAND(ID_VIEW_SUGGESTED_DIAG, OnViewSuggestedDiag)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SUGGESTED_DIAG, OnUpdateViewSuggestedDiag)
	ON_COMMAND(ID_FILE_NEW_PATIENT, OnFileNewPatient)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_EXIST_PATIENT, OnFileExistPatient)
	ON_COMMAND(ID_REPORT_VIEW, OnReportView)
	ON_UPDATE_COMMAND_UI(ID_REPORT_VIEW, OnUpdateReportView)
	ON_COMMAND(ID_REPORT_PRINT, OnReportPrint)
	ON_UPDATE_COMMAND_UI(ID_REPORT_PRINT, OnUpdateReportPrint)
	ON_COMMAND(ID_REPORT_FAC_INFO, OnReportFacInfo)
	ON_UPDATE_COMMAND_UI(ID_REPORT_FAC_INFO, OnUpdateReportFacInfo)
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_RECOMMENDATIONS, OnViewRecommendations)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RECOMMENDATIONS, OnUpdateViewRecommendations)
	ON_COMMAND(ID_EDIT_RECOMMENDATIONS, OnEditRecommendations)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RECOMMENDATIONS, OnUpdateEditRecommendations)
	ON_COMMAND(ID_REPORT_CONFIGURE1, OnReportConfigure)
	ON_UPDATE_COMMAND_UI(ID_REPORT_CONFIGURE1, OnUpdateReportConfigure)
	ON_COMMAND(ID_GOTO_CURRENT_PERIOD, OnGotoCurrentPeriod)
	ON_UPDATE_COMMAND_UI(ID_GOTO_CURRENT_PERIOD, OnUpdateGotoCurrentPeriod)
	ON_COMMAND(ID_FILE_BACKUP, OnFileBackup)
	ON_UPDATE_COMMAND_UI(ID_FILE_BACKUP, OnUpdateFileBackup)
	ON_COMMAND(ID_EDIT_RESPIRATION, OnEditRespiration)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RESPIRATION, OnUpdateEditRespiration)
	ON_COMMAND(ID_VIEW_RESPIRATION, OnViewRespiration)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESPIRATION, OnUpdateViewRespiration)
	ON_COMMAND(ID_VIEW_RSA, OnViewRsa)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RSA, OnUpdateViewRsa)
	//}}AFX_MSG_MAP

    ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)

	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
  ON_UPDATE_COMMAND_UI(ID_VIEW_SUMM_GRAPH, OnUpdateViewSummGraph)
  ON_COMMAND(ID_VIEW_SUMM_GRAPH, OnViewSummGraph)
  ON_UPDATE_COMMAND_UI(ID_VIEW_DATA_SHEET, OnUpdateViewDataSheet)
  ON_COMMAND(ID_VIEW_DATA_SHEET, OnViewDataSheet)
//  ON_COMMAND(ID_FILE_RESUME_STUDY, OnFileResumeStudy)
//  ON_UPDATE_COMMAND_UI(ID_FILE_RESUME_STUDY, OnUpdateFileResumeStudy)
//  ON_COMMAND(ID_EDIT_STIM_MEDIUM, OnEditStimMedium)
//  ON_UPDATE_COMMAND_UI(ID_EDIT_STIM_MEDIUM, OnUpdateEditStimMedium)
//  ON_COMMAND(ID_VIEW_STIM_MEDIUM, OnViewStimMedium)
//  ON_UPDATE_COMMAND_UI(ID_VIEW_STIM_MEDIUM, OnUpdateViewStimMedium)
  ON_COMMAND(ID_GOTO_PREVIOUS_STIM_PERIOD, OnGotoPreviousStimPeriod)
  ON_UPDATE_COMMAND_UI(ID_GOTO_PREVIOUS_STIM_PERIOD, OnUpdateGotoPreviousStimPeriod)
  ON_COMMAND(ID_GOTO_NEXT_STIM_PERIOD, OnGotoNextStimPeriod)
  ON_UPDATE_COMMAND_UI(ID_GOTO_NEXT_STIM_PERIOD, OnUpdateGotoNextStimPeriod)
  ON_COMMAND(ID_VIEW_MIN_PER_GRAPH, OnViewMinPerGraph)
  ON_UPDATE_COMMAND_UI(ID_VIEW_MIN_PER_GRAPH, OnUpdateViewMinPerGraph)
  ON_COMMAND(ID_FILE_RESUME_STUDY, OnFileResumeStudy)
  ON_UPDATE_COMMAND_UI(ID_FILE_RESUME_STUDY, OnUpdateFileResumeStudy)
  ON_COMMAND(ID_EDIT_STIMULATION_MEDIUM, OnEditStimulationMedium)
  ON_UPDATE_COMMAND_UI(ID_EDIT_STIMULATION_MEDIUM, OnUpdateEditStimulationMedium)
  ON_COMMAND(ID_VIEW_STIMULATION_MEDIUM, OnViewStimulationMedium)
  ON_UPDATE_COMMAND_UI(ID_VIEW_STIMULATION_MEDIUM, OnUpdateViewStimulationMedium)
  ON_COMMAND(ID_REPORT_CREATESPREADSHEETOFDATASHEET, OnReportCreatespreadsheetofdatasheet)
  ON_UPDATE_COMMAND_UI(ID_REPORT_CREATESPREADSHEETOFDATASHEET, OnUpdateReportCreatespreadsheetofdatasheet)
  ON_COMMAND(ID_VIEW_EVENTS, OnViewEvents)
  ON_UPDATE_COMMAND_UI(ID_VIEW_EVENTS, OnUpdateViewEvents)
  ON_COMMAND(ID_EDIT_WHAT_IF, OnEditWhatIf)
  ON_UPDATE_COMMAND_UI(ID_EDIT_WHAT_IF, OnUpdateEditWhatIf)
  ON_MESSAGE(WMUSER_DISPLAY_WHATIF, vDisplay_what_if)
  ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
  ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
  ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
  ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
  ON_COMMAND(ID_FILE_EXPORT_STUDIES, OnFileExportStudies)
  ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_STUDIES, OnUpdateFileExportStudies)
  ON_COMMAND(ID_FILE_IMPORTSTUDIES, OnFileImportstudies)
  ON_UPDATE_COMMAND_UI(ID_FILE_IMPORTSTUDIES, OnUpdateFileImportstudies)
  ON_COMMAND(ID_HELP_ABOUT_READER, OnHelpAboutReader)
  ON_UPDATE_COMMAND_UI(ID_HELP_ABOUT_READER, OnUpdateHelpAboutReader)
  ON_COMMAND(ID_HELP_ABOUT3CPMEGGSASCENTRALDATADEPOTREADER, OnHelpAbout3cpmeggsascentraldatadepotreader)
  ON_UPDATE_COMMAND_UI(ID_HELP_ABOUT3CPMEGGSASCENTRALDATADEPOTREADER, OnUpdateHelpAbout3cpmeggsascentraldatadepotreader)
  ON_UPDATE_COMMAND_UI(ID_FILE_NEW_PATIENT, OnUpdateFileNewPatient)
  ON_UPDATE_COMMAND_UI(ID_FILE_EXIST_PATIENT, OnUpdateFileExistPatient)
  ON_COMMAND(ID_FILE_SAVERAWDATAINASCII, OnFileSaverawdatainascii)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVERAWDATAINASCII, OnUpdateFileSaverawdatainascii)
  ON_COMMAND(ID_FILE_SAVERAWDATAINASCII33072, OnFileSaverawdatainasciiWL)
  ON_UPDATE_COMMAND_UI(ID_FILE_SAVERAWDATAINASCII33072, OnUpdateFileSaverawdatainasciiWL)

  ON_COMMAND(ID_REPORT_SHOW33091, OnReportShowHideStudyDates)
  ON_UPDATE_COMMAND_UI(ID_REPORT_SHOW33091, OnUpdateReportShowHideStudyDates)
  ON_MESSAGE(WMUSER_DISPLAY_HIDE_SHOW_STUDY_DATES, vDisplay_hide_show_study_dates)

  END_MESSAGE_MAP()


  // ID_REPORT_SHOW33091
/********************
  ON_COMMAND(ID_FILE_EXPORT_STUDIES, OnFileExportStudies)
  ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_STUDIES, OnUpdateFileExportStudies)
  // Global help commands
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)

	ON_COMMAND(ID_HELP_FINDER, OnHelp)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, OnHelp)
	ON_COMMAND(ID_DEFAULT_HELP, OnHelp)

	ON_COMMAND(ID_ANALYZE_POSTWATER_40, OnAnalyzePostwater40)
	ON_UPDATE_COMMAND_UI(ID_ANALYZE_POSTWATER_40, OnUpdateAnalyzePostwater40)
	ON_COMMAND(ID_VIEW_POSTWATER_40, OnViewPostwater40)
	ON_UPDATE_COMMAND_UI(ID_VIEW_POSTWATER_40, OnUpdateViewPostwater40)

******************/

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_uViewID = VIEWID_TEN_MIN;
  m_pDoc = NULL;

  g_pFrame = this;
  EnableToolTips(TRUE);

	return 0;
  }

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers for menu items

/********************************************************************
OnFileOpen

  Message handler for file open menu item.
    Call the current document to select a patient..
********************************************************************/
void CMainFrame::OnFileOpen() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->iSelectPatient(STUDY_NONE);
  }

/********************************************************************
OnAnalyzeRsa

  Message handler for RSA menu item.
    Create RSA view and make it active.
********************************************************************/
void CMainFrame::OnAnalyzeRsa() 
  {
  CRuntimeClass* pNewViewClass;
	
  if(g_pFrame == NULL)
    g_pFrame = this;

  if(m_uViewID != VIEWID_RSA)
    {	// Set the child window ID of the active view to AFX_IDW_PANE_FIRST.
      // This is necessary so that CFrameWnd::RecalcLayout will allocate
      // this "first pane" to that portion of the frame window's client
      // area not allocated to control bars.  Set the child ID of
      // the previously active view to some other ID; we will use the
      // command ID as the child ID.
    CView* pOldActiveView = GetActiveView();
    ::SetWindowLong(pOldActiveView->m_hWnd, GWL_ID, ID_FILE_OPEN);

    pNewViewClass = RUNTIME_CLASS(CRSAView);
      // create the new view
    CCreateContext context;
    context.m_pNewViewClass = pNewViewClass;
    context.m_pCurrentDoc = GetActiveDocument();
    CView* pNewView = STATIC_DOWNCAST(CView, CreateView(&context));
    if (pNewView != NULL)
      { // the new view is there, but invisible and not active...
      pNewView->ShowWindow(SW_SHOW);
      pNewView->OnInitialUpdate();
      SetActiveView(pNewView);
      RecalcLayout();
      m_uViewID = VIEWID_RSA;
        // finally destroy the old view...
      pOldActiveView->DestroyWindow();
      }
    }
  }

/********************************************************************
OnUpdateAnalyzeRsa

  Message handler for updating analyzing the RSA menu item.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateAnalyzeRsa(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    pCmdUI->Enable(m_pDoc->m_bHaveFile == TRUE
      && m_pDoc->bIs_period_analyzed(m_pDoc->m_uCurrentPeriod));
    }
  }

/********************************************************************
OnAnalyzeBaseline

  Message handler for analyzing the baseline period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnAnalyzeBaseline() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vAnalyzeBaseline();
  }

/********************************************************************
OnUpdateAnalyzeBaseline

  Message handler for updating analyzing the baseline period menu items.
    Enable/disable the menu item based on having a patient data file.
    Check the menu item if the period has been analyzed.
********************************************************************/
void CMainFrame::OnUpdateAnalyzeBaseline(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(g_bHiddenPatients == FALSE)
      pCmdUI->Enable(m_pDoc->m_bHaveFile);
    else
      pCmdUI->Enable(FALSE);
    //pCmdUI->Enable(m_pDoc->m_bHaveFile);

      // If baseline has been analyzed, check the menu item.
    if((m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_BL]) == true)
      pCmdUI->SetCheck(1);
    else
      pCmdUI->SetCheck(0);
    }
  }

/********************************************************************
OnAnalyzeCurrent

  Message handler for analyzing the current period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnAnalyzeCurrent() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vAnalyzeCurrent();
  }

/********************************************************************
OnUpdateAnalyzeCurrent

  Message handler for updating analyzing the current period menu items.
    Enable/disable the menu item based:
      Baseline period
        on having a patient data file.
      postwater period
        on the baseline period having been analyzed.
********************************************************************/
void CMainFrame::OnUpdateAnalyzeCurrent(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(g_bHiddenPatients == FALSE)
      {
      if(m_pDoc->m_uCurrentPeriod == PERIOD_BL)
        pCmdUI->Enable(m_pDoc->m_bHaveFile);
      else
        pCmdUI->Enable((m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_BL]));
      }
    else
      pCmdUI->Enable(FALSE);

    //if(m_pDoc->m_uCurrentPeriod == PERIOD_BL)
    //  pCmdUI->Enable(m_pDoc->m_bHaveFile);
    //else
    //  pCmdUI->Enable((m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_BL]));
    }
  }

/********************************************************************
OnAnalyzeDiagnosis

  Message handler for making a diagnosis.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnAnalyzeDiagnosis() 
  {
	
//  vSetup_for_msg(); // Make sure everything is set up to process this message.
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vAnalyzeDiagnosis();
  if(m_uViewID == VIEWID_REPORT && m_pDoc->m_bReportChanged == true)
    { // The report is being view and has changed, redisplay it.
    ((CReportView *)GetActiveView())->vReset_view();
    GetActiveView()->Invalidate();
    }
  }

/********************************************************************
OnUpdateAnalyzeDiagnosis

  Message handler for updating making a diagnosis menu items.
    Enable/disable the menu item based on having analyzed all periods.
********************************************************************/
void CMainFrame::OnUpdateAnalyzeDiagnosis(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(theApp.m_ptStudyType == PT_STANDARD)
      { // Standard study
      if(g_bHiddenPatients == FALSE)
        {
        if(m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_BL] == true
        && m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_10] == true    
        && m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_20] == true    
        && m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_30] == true    
          )
          pCmdUI->Enable(TRUE);
        else
          pCmdUI->Enable(FALSE);
        }
      else
        pCmdUI->Enable(FALSE);



      //if(m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_BL] == true
      //&& m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_10] == true    
      //&& m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_20] == true    
      //&& m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_30] == true    
      //  )
      //  pCmdUI->Enable(TRUE);
      //else
      //  pCmdUI->Enable(FALSE);
      }
    else
      { // Research study
      if(g_bHiddenPatients == FALSE)
        pCmdUI->Enable(m_pDoc->bIs_study_analyzed());
      else
        pCmdUI->Enable(FALSE);

      //pCmdUI->Enable(m_pDoc->bIs_study_analyzed());
      }
    }
  }

/********************************************************************
OnAnalyzePostwater10

  Message handler for analyzing the postwater first 10 minute period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnAnalyzePostwater10() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vAnalyzePostwater10();
  }

/********************************************************************
OnUpdateAnalyzePostwater10

  Message handler for updating analyzing the postwater first 10 minute
  period menu items.
    Enable/disable the menu item based on having analyzed the baseline period.
    Check the menu item if the period has been analyzed.
********************************************************************/
void CMainFrame::OnUpdateAnalyzePostwater10(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(g_bHiddenPatients == FALSE)
      pCmdUI->Enable((m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_BL]));
    else
      pCmdUI->Enable(FALSE);
    //pCmdUI->Enable((m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_BL]));
      // If 10 minute postwater period has been analyzed, check the menu item.
    if(m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_10] == true)
      pCmdUI->SetCheck(1);
    else
      pCmdUI->SetCheck(0);
    }
  }

/********************************************************************
OnAnalyzePostwater20

  Message handler for analyzing the postwater second 10 minute period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnAnalyzePostwater20() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vAnalyzePostwater20();
  }

/********************************************************************
OnUpdateAnalyzePostwater20

  Message handler for updating analyzing the postwater second 10 minute
  period menu items.
    Enable/disable the menu item based on having analyzed the baseline period.
    Check the menu item if the period has been analyzed.
********************************************************************/
void CMainFrame::OnUpdateAnalyzePostwater20(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(g_bHiddenPatients == FALSE)
      pCmdUI->Enable((m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_BL]));
    else
      pCmdUI->Enable(FALSE);
    //pCmdUI->Enable((m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_BL]));
      // If 10 minute postwater period has been analyzed, check the menu item.
    if((m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_20]) == true)
      pCmdUI->SetCheck(1);
    else
      pCmdUI->SetCheck(0);
    }
  }

/********************************************************************
OnAnalyzePostwater30

  Message handler for analyzing the postwater third 10 minute period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnAnalyzePostwater30() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vAnalyzePostwater30();
  }

/********************************************************************
OnUpdateAnalyzePostwater30

  Message handler for updating analyzing the postwater third 10 minute
  period menu items.
    Enable/disable the menu item based on having analyzed the baseline period.
    Check the menu item if the period has been analyzed.
********************************************************************/
void CMainFrame::OnUpdateAnalyzePostwater30(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(g_bHiddenPatients == FALSE)
      pCmdUI->Enable((m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_BL]));
    else
      pCmdUI->Enable(FALSE);
    //pCmdUI->Enable((m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_BL]));
      // If 10 minute postwater period has been analyzed, check the menu item.
    if((m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_30]) == true)
      pCmdUI->SetCheck(1);
    else
      pCmdUI->SetCheck(0);
    }
  }

/********************************************************************
OnAnalyzePostwater40

  Message handler for analyzing the postwater fourth 10 minute period.
    Call the current document to process.
********************************************************************/
/**************************************
void CMainFrame::OnAnalyzePostwater40() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vAnalyzePostwater30();
  }
***********************************/

/********************************************************************
OnUpdateAnalyzePostwater40

  Message handler for updating analyzing the postwater fourth 10 minute
  period menu items.
    Enable/disable the menu item based on having analyzed the baseline period.
    Check the menu item if the period has been analyzed.
********************************************************************/
/**************************
void CMainFrame::OnUpdateAnalyzePostwater40(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable((m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_BL]));
    // If 10 minute postwater period has been analyzed, check the menu item.
  if((m_pDoc->m_pdPatient.bPeriodsAnalyzed[PERIOD_40]) == true)
    pCmdUI->SetCheck(1);
  else
    pCmdUI->SetCheck(0);
  }
**************************/

/********************************************************************
OnEditAmtWater

  Message handler for editing the amount of water ingested.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnEditAmtWater() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vEditAmtWater();
  if(m_uViewID == VIEWID_REPORT && m_pDoc->m_bReportChanged == true)
    { // The report is being view and has changed, redisplay it.
    ((CReportView *)GetActiveView())->vReset_view();
    GetActiveView()->Invalidate();
    }
  }

/********************************************************************
OnUpdateEditAmtWater

  Message handler for updating the edit amount of water ingested menu items.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateEditAmtWater(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bHiddenPatients == FALSE)
    pCmdUI->Enable(m_pDoc->m_bHaveFile);
  else
    pCmdUI->Enable(FALSE);
  }

/********************************************************************
OnEditPatientDemograhics

  Message handler for editing the patient demographics.
    Call the current document to process.
    If the report is being displayed and the demographics have changed
      redisplay the report.
********************************************************************/
void CMainFrame::OnEditPatientDemograhics() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vEditPatientDemograhics();
  if(m_uViewID == VIEWID_REPORT && m_pDoc->m_bReportChanged == true)
    { // The report is being view and has changed, redisplay it.
    ((CReportView *)GetActiveView())->vReset_view();
    GetActiveView()->Invalidate();
    }
  }

/********************************************************************
OnUpdateEditPatientDemograhics

  Message handler for updating the edit patient demographics menu items.
    Enable/disable the menu item based on having a patient.
********************************************************************/
void CMainFrame::OnUpdateEditPatientDemograhics(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHavePatient);
  }

/********************************************************************
OnEditPatientDiagHist

  Message handler for editing the patient diagnostic history.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnEditPatientDiagHist() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vEditPatientDiagHist();
  }

/********************************************************************
OnUpdateEditPatientDiagHist

  Message handler for updating the edit patient diagnostic history menu items.
    Enable/disable the menu item based on having a patient.
********************************************************************/
void CMainFrame::OnUpdateEditPatientDiagHist(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(g_bHiddenPatients == FALSE)
      pCmdUI->Enable(m_pDoc->m_bHavePatient);
    else
      pCmdUI->Enable(FALSE);
    //pCmdUI->Enable(m_pDoc->m_bHavePatient);
    }
  }

/********************************************************************
OnEditPatientMeds

  Message handler for editing the patient medications.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnEditPatientMeds() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vEditPatientMeds();
  }

/********************************************************************
OnUpdateEditPatientMeds

  Message handler for updating the edit patient medications menu items.
    Enable/disable the menu item based on having a patient.
********************************************************************/
void CMainFrame::OnUpdateEditPatientMeds(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bHiddenPatients == FALSE)
    pCmdUI->Enable(m_pDoc->m_bHavePatient);
  else
    pCmdUI->Enable(FALSE);

  //pCmdUI->Enable(m_pDoc->m_bHavePatient);
  }

/********************************************************************
OnEditPreStudyInfo

  Message handler for editing the prestudy information.
    Call the current document to process.
    If the report is being displayed and the prestudy info has changed
      redisplay the report.
********************************************************************/
void CMainFrame::OnEditPreStudyInfo() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vEditPreStudyInfo();
  if(m_uViewID == VIEWID_REPORT && m_pDoc->m_bReportChanged == true)
    { // The report is being view and has changed, redisplay it.
    ((CReportView *)GetActiveView())->vReset_view();
    GetActiveView()->Invalidate();
    }
  }

/********************************************************************
OnUpdateEditPreStudyInfo

  Message handler for updating the edit prestudy information menu items.
    Enable/disable the menu item based on having a patient.
********************************************************************/
void CMainFrame::OnUpdateEditPreStudyInfo(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bHiddenPatients == FALSE)
    pCmdUI->Enable(m_pDoc->m_bHavePatient);
  else
    pCmdUI->Enable(FALSE);
  //pCmdUI->Enable(m_pDoc->m_bHavePatient);
  }

/********************************************************************
OnEditSuggestedDiag

  Message handler for editing the suggested diagnosis.
    Call the current document to process.
    If the report is being displayed and the suggested diagnosis has changed
      redisplay the report.
********************************************************************/
void CMainFrame::OnEditSuggestedDiag() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vEditSuggestedDiag();
  if(m_uViewID == VIEWID_REPORT && m_pDoc->m_bReportChanged == true)
    { // The report is being view and has changed, redisplay it.
    ((CReportView *)GetActiveView())->vReset_view();
    GetActiveView()->Invalidate();
    }
  }

/********************************************************************
OnUpdateEditSuggestedDiag

  Message handler for updating the edit suggested diagnosis menu items.
    Enable/disable the menu item based:
      - on having analyzed all the periods.
      - There is a suggested diagnosis.
********************************************************************/
void CMainFrame::OnUpdateEditSuggestedDiag(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(theApp.m_ptStudyType == PT_STANDARD)
      { // Standard study
      if(g_bHiddenPatients == FALSE)
        pCmdUI->Enable(
          !m_pDoc->m_pdPatient.cstrSuggestedDiagnosis.IsEmpty()
          && m_pDoc->bAre_standard_periods_analyzed());
      else
        pCmdUI->Enable(FALSE);

      //pCmdUI->Enable(
      //  !m_pDoc->m_pdPatient.cstrSuggestedDiagnosis.IsEmpty()
      //  && m_pDoc->bAre_standard_periods_analyzed());
      }
    else
      { // Research study
      if(g_bHiddenPatients == FALSE)
        pCmdUI->Enable(m_pDoc->m_bHavePatient && m_pDoc->bIs_study_analyzed());
      else
        pCmdUI->Enable(FALSE);

      //pCmdUI->Enable(m_pDoc->m_bHavePatient && m_pDoc->bIs_study_analyzed());
      }
    }
  }

/********************************************************************
OnEditRecommendations

  Message handler for editing the recommendations.
    Call the current document to process.
    If the report is being displayed and the recommendations have changed
      redisplay the report.
********************************************************************/
void CMainFrame::OnEditRecommendations() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vEditRecommendations();
  if(m_uViewID == VIEWID_REPORT && m_pDoc->m_bReportChanged == true)
    { // The report is being view and has changed, redisplay it.
    ((CReportView *)GetActiveView())->vReset_view();
    GetActiveView()->Invalidate();
    }
  }

/********************************************************************
OnUpdateEditRecommendations

  Message handler for updating the edit recommendations menu items.
    Enable/disable the menu item based:
      - on having analyzed all the periods.
      - There is a suggested diagnosis.
********************************************************************/
void CMainFrame::OnUpdateEditRecommendations(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(theApp.m_ptStudyType == PT_STANDARD)
      { // Standard study
      if(g_bHiddenPatients == FALSE)
        pCmdUI->Enable(
          !m_pDoc->m_pdPatient.cstrSuggestedDiagnosis.IsEmpty()
          && m_pDoc->bAre_standard_periods_analyzed());
      else
        pCmdUI->Enable(FALSE);

      //pCmdUI->Enable(
      //  !m_pDoc->m_pdPatient.cstrSuggestedDiagnosis.IsEmpty()
      //  && m_pDoc->bAre_standard_periods_analyzed());
      }
    else
      { // Research study
      if(g_bHiddenPatients == FALSE)
        pCmdUI->Enable(m_pDoc->m_bHavePatient && m_pDoc->bIs_study_analyzed());
      else
        pCmdUI->Enable(FALSE);
      //pCmdUI->Enable(m_pDoc->m_bHavePatient && m_pDoc->bIs_study_analyzed());
      }
    }
  }

/********************************************************************
OnEditRespiration

  Message handler for editing the respiration.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnEditRespiration() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vEditRespiration();
  }

/********************************************************************
OnUpdateEditRespiration

  Message handler for updating the edit respiration menu item.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateEditRespiration(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(g_bHiddenPatients == FALSE)
      pCmdUI->Enable(m_pDoc->m_bHaveFile);
    else
      pCmdUI->Enable(FALSE);
    //pCmdUI->Enable(m_pDoc->m_bHaveFile);
    }
  }

/********************************************************************
OnGotoEnd

  Message handler for going to the last 10 minute period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnGotoEnd() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vGotoEnd();
  }

/********************************************************************
OnUpdateGotoEnd

  Message handler for updating the go to last 10 minute period menu items.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateGotoEnd(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }

/********************************************************************
OnGotoNextPeriod

  Message handler for going to the next 10 minute period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnGotoNextPeriod() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vGotoNextPeriod();
  }

/********************************************************************
OnUpdateGotoNextPeriod

  Message handler for updating the go to next 10 minute period menu items.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateGotoNextPeriod(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }

/********************************************************************
OnGotoPrevPeriod

  Message handler for going to the previous 10 minute period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnGotoPrevPeriod() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vGotoPrevPeriod();
  }

/********************************************************************
OnUpdateGotoPrevPeriod

  Message handler for updating the go to previous 10 minute period menu items.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateGotoPrevPeriod(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }

/********************************************************************
OnGotoStart

  Message handler for going to the first 10 minute period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnGotoStart() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vGotoStart();
  }

/********************************************************************
OnUpdateGotoStart

  Message handler for updating the go to first 10 minute period menu items.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateGotoStart(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }

/********************************************************************
OnGotoCurrentPeriod

  Message handler for going to the current 10 minute period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnGotoCurrentPeriod() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vGotoCurrent();
  }

/********************************************************************
OnUpdateGotoCurrentPeriod

  Message handler for updating the go to current 10 minute period menu items.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateGotoCurrentPeriod(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }

/********************************************************************
OnViewAmtWater

  Message handler for viewing the amount of water ingested.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnViewAmtWater() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vViewAmtWater();
  }

/********************************************************************
OnUpdateViewAmtWater

  Message handler for updating the view amount of water ingested menu items.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateViewAmtWater(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }

/********************************************************************
OnViewBaseline

  Message handler for viewing the baseline period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnViewBaseline() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vViewBaseline();
  }

/********************************************************************
OnUpdateViewBaseline

  Message handler for updating the view baseline period menu items.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateViewBaseline(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }

/********************************************************************
OnViewPatientDemograhics

  Message handler for viewing the patient demographics.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnViewPatientDemograhics() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vViewPatientDemograhics();
  }

/********************************************************************
OnUpdateViewPatientDemograhics

  Message handler for updating the view patient demographics menu items.
    Enable/disable the menu item based on having a patient.
********************************************************************/
void CMainFrame::OnUpdateViewPatientDemograhics(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHavePatient);
  }

/********************************************************************
OnViewPatientDiagHist

  Message handler for viewing the patient diagnostic history.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnViewPatientDiagHist() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vViewPatientDiagHist();
  }

/********************************************************************
OnUpdateViewPatientDiagHist

  Message handler for updating the view patient diagnostic history menu items.
    Enable/disable the menu item based on having a patient.
********************************************************************/
void CMainFrame::OnUpdateViewPatientDiagHist(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    pCmdUI->Enable(m_pDoc->m_bHavePatient);
  }

/********************************************************************
OnViewPatientMeds

  Message handler for viewing the patient medications.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnViewPatientMeds() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vViewPatientMeds();
  }

/********************************************************************
OnUpdateViewPatientMeds

  Message handler for updating the view patient medications menu items.
    Enable/disable the menu item based on having a patient.
********************************************************************/
void CMainFrame::OnUpdateViewPatientMeds(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHavePatient);
  }

/********************************************************************
OnViewPostwater10

  Message handler for viewing the postwater first 10 minute period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnViewPostwater10() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vViewPostwater10();
  }

/********************************************************************
OnUpdateViewPostwater10

  Message handler for updating the view postwater first 10 minute period
  menu items.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateViewPostwater10(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }

/********************************************************************
OnViewPostwater20

  Message handler for viewing the postwater second 10 minute period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnViewPostwater20() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vViewPostwater20();
  }

/********************************************************************
OnUpdateViewPostwater20

  Message handler for updating the view postwater second 10 minute period
  menu items.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateViewPostwater20(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }

/********************************************************************
OnViewPostwater30

  Message handler for viewing the postwater third 10 minute period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnViewPostwater30() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vViewPostwater30();
  }

/********************************************************************
OnUpdateViewPostwater30

  Message handler for updating the view postwater third 10 minute period
  menu items.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateViewPostwater30(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }

/********************************************************************
OnViewPostwater40

  Message handler for viewing the postwater fourth 10 minute period.
    Call the current document to process.
********************************************************************/
/********************************************************************
void CMainFrame::OnViewPostwater40() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vViewPostwater40();
  }
********************************************************************/

/********************************************************************
OnUpdateViewPostwater40

  Message handler for updating the view postwater fourth 10 minute period
  menu items.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
/********************************************************************
void CMainFrame::OnUpdateViewPostwater40(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }
********************************************************************/

/********************************************************************
OnViewPreStudyInfo

  Message handler for viewing the prestudy information.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnViewPreStudyInfo() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vViewPreStudyInfo();
  }

/********************************************************************
OnUpdateViewPreStudyInfo

  Message handler for updating the view prestudy information menu items.
    Enable/disable the menu item based on having a patient.
********************************************************************/
void CMainFrame::OnUpdateViewPreStudyInfo(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHavePatient);
  }

/********************************************************************
OnViewSuggestedDiag

  Message handler for viewing the suggested diagnosis.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnViewSuggestedDiag() 
  {
	
//  vSetup_for_msg(); // Make sure everything is set up to process this message.
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vViewSuggestedDiag();
  }

/********************************************************************
OnUpdateViewSuggestedDiag

  Message handler for updating the view suggested diagnosis menu items.
    Enable/disable the menu item based:
      - on having analyzed all the periods.
      - There is a suggested diagnosis.
********************************************************************/
void CMainFrame::OnUpdateViewSuggestedDiag(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(theApp.m_ptStudyType == PT_STANDARD)
      { // Standard study
      pCmdUI->Enable(
        !m_pDoc->m_pdPatient.cstrSuggestedDiagnosis.IsEmpty()
        && m_pDoc->bAre_standard_periods_analyzed());
      }
    else
      { // Research study
      pCmdUI->Enable(m_pDoc->bIs_study_analyzed());
      }
    }
  }

/********************************************************************
OnViewRecommendations

  Message handler for viewing the recommendations
    Call the current document to process.
********************************************************************/
void CMainFrame::OnViewRecommendations() 
  {
	
//  vSetup_for_msg(); // Make sure everything is set up to process this message.
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vViewRecommendations();
  }

/********************************************************************
OnUpdateViewRecommendations

  Message handler for updating the view recommendations menu items.
    Enable/disable the menu item based:
      - on having analyzed all the periods.
      - There is a suggested diagnosis.
********************************************************************/
void CMainFrame::OnUpdateViewRecommendations(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(theApp.m_ptStudyType == PT_STANDARD)
      { // Standard study
      pCmdUI->Enable(
        !m_pDoc->m_pdPatient.cstrSuggestedDiagnosis.IsEmpty()
        && m_pDoc->bAre_standard_periods_analyzed());
      }
    else
      { // Research study
      pCmdUI->Enable(m_pDoc->bIs_study_analyzed());
      }
    }
  }

/********************************************************************
OnViewRespiration

  Message handler for viewing the respiration
    Call the current document to process.
********************************************************************/
void CMainFrame::OnViewRespiration() 
  {
	
//  vSetup_for_msg(); // Make sure everything is set up to process this message.
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vViewRespiration();
  }

/********************************************************************
OnUpdateViewRespiration

  Message handler for updating the view respiration menu item.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateViewRespiration(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }

/********************************************************************
OnViewRsa

  Message handler for viewing the RSA
    Call the analyze RSA message handler.
********************************************************************/
void CMainFrame::OnViewRsa() 
  {
	
  OnAnalyzeRsa();
  }

/********************************************************************
OnUpdateViewRsa

  Message handler for updating the view RSA menu item.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateViewRsa(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
//  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    pCmdUI->Enable(m_pDoc->m_bHaveFile == TRUE
      && m_pDoc->bIs_period_analyzed(m_pDoc->m_uCurrentPeriod));
  }

/********************************************************************
OnFileNewPatient

  Message handler for a new patient.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnFileNewPatient() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vFileNewPatient(STUDY_NEW_PATIENT);
  }

/********************************************************************
OnFileNew

  Message handler for a new patient.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnFileNew() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vFileNewPatient(STUDY_NEW_PATIENT);
  }

/********************************************************************
OnUpdateFileNewPatient

  Message handler for updating the New Study on New patient menu item.
    Enable/disable the menu item based on if studies are allowed.
********************************************************************/
void CMainFrame::OnUpdateFileNewPatient(CCmdUI *pCmdUI)
  {

  if(m_pDoc->m_bAllowStudies == true)
    pCmdUI->Enable(TRUE);
  else
    pCmdUI->Enable(FALSE);
  }

/********************************************************************
OnFileExistPatient

  Message handler for an existing patient.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnFileExistPatient() 
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.

  if(m_pDoc->iSelectPatient(STUDY_EXIST_PATIENT, INV_LONG, FALSE) == SUCCESS)
    m_pDoc->vFileNewPatient(STUDY_EXIST_PATIENT);
  }

/********************************************************************
OnUpdateFileExistPatient

  Message handler for updating the New Study on Existing patient menu item.
    Enable/disable the menu item based on if studies are allowed.
********************************************************************/
void CMainFrame::OnUpdateFileExistPatient(CCmdUI *pCmdUI)
  {
  if(m_pDoc->m_bAllowStudies == true)
    pCmdUI->Enable(TRUE);
  else
    pCmdUI->Enable(FALSE);
  }

/********************************************************************
OnReportView

  Message handler for viewing the report.
    Create Report view and make it active.
********************************************************************/
void CMainFrame::OnReportView() 
  {
  vView_report();
  }

/********************************************************************
OnUpdateReportView

  Message handler for updating the Report menu item.
    Enable/disable the menu item based on having analyzed all the periods.
********************************************************************/
void CMainFrame::OnUpdateReportView(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(m_pDoc->bAre_standard_periods_analyzed())
    pCmdUI->Enable(TRUE);
  else
    pCmdUI->Enable(FALSE);
  }

/********************************************************************
OnFilePrintPreview

  Message handler for Print preview.
    Create Report view and make it active.
********************************************************************/
void CMainFrame::OnFilePrintPreview()
  {
  vView_report();
  }

/********************************************************************
OnUpdateFilePrintPreview

  Message handler for updating the Print preview menu item.
    Enable/disable the menu item based on having analyzed all the periods.
********************************************************************/
void CMainFrame::OnUpdateFilePrintPreview(CCmdUI *pCmdUI)
  {
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(m_pDoc->bAre_standard_periods_analyzed())
    pCmdUI->Enable(TRUE);
  else
    pCmdUI->Enable(FALSE);
  }

/********************************************************************
OnReportPrint

  Message handler for the print report menu item.
    Call the Report view for printing.
********************************************************************/
void CMainFrame::OnReportPrint() 
  {
  CReportView *pRptView;

  if(m_uViewID == VIEWID_REPORT)
    {
    if(m_pDoc == NULL)
      m_pDoc = (CEGGSASDoc *)GetActiveDocument();
    m_pDoc->m_uReportItemToView = RPT_VIEW_ITEM_NONE;

    pRptView = (CReportView *)GetActiveView();
    pRptView->vPrint_report();
    }
  }

/********************************************************************
OnUpdateReportPrint

  Message handler for updating the Report menu item.
    Enable/disable the menu item based on the Report view being active.
********************************************************************/
void CMainFrame::OnUpdateReportPrint(CCmdUI* pCmdUI) 
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(m_pDoc->m_bReportView == true)
    {
    if(m_pDoc->m_uReportItemToView == RPT_VIEW_ITEM_NONE)
      pCmdUI->Enable(TRUE); // Actually viewing report
    else
      pCmdUI->Enable(FALSE); // Viewing from View menu.
    }
  else
    pCmdUI->Enable(FALSE);
  }

/********************************************************************
OnFilePrint

  Message handler for the File Print menu item.
    Call the Report view for printing.
********************************************************************/
void CMainFrame::OnFilePrint()
  {
  CReportView *pRptView;

  if(m_uViewID == VIEWID_REPORT)
    {
    if(m_pDoc == NULL)
      m_pDoc = (CEGGSASDoc *)GetActiveDocument();
    m_pDoc->m_uReportItemToView = RPT_VIEW_ITEM_NONE;

    pRptView = (CReportView *)GetActiveView();
    pRptView->vPrint_report();
    }
  }

/********************************************************************
OnUpdateFilePrint

  Message handler for updating the File Print menu item.
    Enable/disable the menu item based on the Report view being active.
********************************************************************/
void CMainFrame::OnUpdateFilePrint(CCmdUI *pCmdUI)
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(m_pDoc->m_bReportView == true)
    {
    if(m_pDoc->m_uReportItemToView == RPT_VIEW_ITEM_NONE)
      pCmdUI->Enable(TRUE); // Actually viewing report
    else
      pCmdUI->Enable(FALSE); // Viewing from View menu.
    }
  else
    pCmdUI->Enable(FALSE);
  }

/********************************************************************
OnReportCreatespreadsheetofdatasheet

  Message handler for an creating and viewing a spreadsheet created
  from the data sheet.
    Create a spreadsheet and start the program to display the spreadsheet.
********************************************************************/
void CMainFrame::OnReportCreatespreadsheetofdatasheet()
  {

  if(g_pFrame == NULL)
    g_pFrame = this;
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vReportSpreadsheetOfDataSheet();
  }

/********************************************************************
OnUpdateReportCreatespreadsheetofdatasheet

  Message handler for updating the Report Create Spreadsheet of Data Sheet menu item.
    Enable/disable the menu item based on having analyzed all the periods.
********************************************************************/
void CMainFrame::OnUpdateReportCreatespreadsheetofdatasheet(CCmdUI *pCmdUI)
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(m_pDoc->bAre_standard_periods_analyzed())
      pCmdUI->Enable(TRUE);
    else
      pCmdUI->Enable(FALSE);
    }
  }

/********************************************************************
OnReportConfigure

  Message handler for the configure report menu item.
    Call the current document to process.
    If the report is being displayed and the configuration has changed
      redisplay the report.
********************************************************************/
void CMainFrame::OnReportConfigure() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vReportConfigure();
  if(m_uViewID == VIEWID_REPORT && m_pDoc->m_bReportChanged == true)
    { // The report is being viewed and has changed, redisplay it.
    ((CReportView *)GetActiveView())->vReset_view();
    GetActiveView()->Invalidate();
    }
  }

/********************************************************************
OnUpdateReportConfigure

  Message handler for updating the Configure Report menu item.
    Always enabled.
********************************************************************/
void CMainFrame::OnUpdateReportConfigure(CCmdUI* pCmdUI) 
  {
	
  pCmdUI->Enable(TRUE);
  }

/********************************************************************
OnReportFacInfo

  Message handler for the Facility Information menu item.
    Call the current document to process.
    If the report is being displayed and the facility information has changed
      redisplay the report.
********************************************************************/
void CMainFrame::OnReportFacInfo() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vReportFacInfo();
  if(m_uViewID == VIEWID_REPORT && m_pDoc->m_bReportChanged == true)
    { // The report is being view and has changed, redisplay it.
    ((CReportView *)GetActiveView())->vReset_view();
    GetActiveView()->Invalidate();
    }
  }

/********************************************************************
OnUpdateReportFacInfo

  Message handler for updating the Facility Information menu item.
    Always enabled.
********************************************************************/
void CMainFrame::OnUpdateReportFacInfo(CCmdUI* pCmdUI) 
  {
	
  pCmdUI->Enable(TRUE);
  }

/********************************************************************
OnClose

  Message handler for the program Exit menu item.
    Call the current document to determine if it is OK to close.
    Exit the program if OK to close.
********************************************************************/
void CMainFrame::OnClose() 
  {
	// TODO: Add your message handler code here and/or call default
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(m_pDoc->bOk_to_close() == true)
		CFrameWnd::OnClose();
  }

/********************************************************************
OnFileBackup

  Message handler for the File Backup or Restore menu item.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnFileBackup() 
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vBackup();
  }

/********************************************************************
OnUpdateFileBackup

  Message handler for updating the File Backup or Restore menu item.
    Always enabled.
********************************************************************/
void CMainFrame::OnUpdateFileBackup(CCmdUI* pCmdUI) 
  {
	
  pCmdUI->Enable(TRUE);
  }

/********************************************************************
OnFileExportStudies

  Message handler for the File Export Studies menu item.
    Start the ExportStudy program.
********************************************************************/
void CMainFrame::OnFileExportStudies()
  {
  CString cstrProgram, cstrCurDir;
	TCHAR szCurDir[1024];
  
  cstrProgram.Format("%s\\%s", STUDY_EXPORT_DIR, EXPORT_STUDIES_PGM);

  // Get the current directory and append the StudyExport directory to it
  // so that the StudyExport program has the correct directory for its
  // help file.
	GetCurrentDirectory(1024, (LPTSTR)szCurDir);
  cstrCurDir.Format("%s\\%s", szCurDir, STUDY_EXPORT_DIR);

  _spawnl(_P_NOWAIT, cstrProgram, EXPORT_STUDIES_PGM, cstrCurDir, NULL);
  }

/********************************************************************
OnUpdateFileExportStudies

  Message handler for updating the File Export Studies menu item.
    Always enabled.
********************************************************************/
void CMainFrame::OnUpdateFileExportStudies(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(TRUE);
  }

void CMainFrame::OnFileImportstudies()
  {
  m_pDoc->vImport_reader_db();
  }

void CMainFrame::OnUpdateFileImportstudies(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(TRUE);
  }

/********************************************************************
OnFileResumeStudy

For the research version

  Message handler for the File Resume Study menu item.
********************************************************************/
void CMainFrame::OnFileResumeStudy()
  {

  vSetup_for_msg(); // Make sure everything is set up to process this message.

  if(m_pDoc->iSelectPatient(STUDY_RESUME) == SUCCESS)
    m_pDoc->vFileResumeStudy();
  }

/***
void CMainFrame::OnFileResumeStudy()
  {

  vSetup_for_msg(); // Make sure everything is set up to process this message.

  if(m_pDoc->iSelectPatient(STUDY_RESUME) == SUCCESS)
    m_pDoc->vFileResumeStudy();
  }
***/

/********************************************************************
OnUpdateFileResumeStudy

For the research version

  Message handler for updating the File Resume Study menu item.
    Always enabled.
********************************************************************/
void CMainFrame::OnUpdateFileResumeStudy(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(TRUE);
  }
/**
void CMainFrame::OnUpdateFileResumeStudy(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(TRUE);
  }
**/

/********************************************************************
OnEditStimMedium

For the research version

  Message handler for the Edit Stimulation and quantity menu item.
********************************************************************/
void CMainFrame::OnEditStimulationMedium()
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vEditAmtWater();
  }
/**
void CMainFrame::OnEditStimMedium()
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vEditAmtWater();
  }
**/

/********************************************************************
OnUpdateEditStimMedium

For the research version

  Message handler for updating the Edit Stimulation and quantity  menu item.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateEditStimulationMedium(CCmdUI *pCmdUI)
  {
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bHiddenPatients == FALSE)
    pCmdUI->Enable(m_pDoc->m_bHaveFile);
  else
    pCmdUI->Enable(FALSE);
  //pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }
/***************
void CMainFrame::OnUpdateEditStimMedium(CCmdUI *pCmdUI)
  {
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }
*************/

/********************************************************************
OnViewStimMedium

For the research version

  Message handler for the View Stimulation and quantity menu item.
********************************************************************/
void CMainFrame::OnViewStimulationMedium()
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vViewAmtWater();
  }
/******************************
void CMainFrame::OnViewStimMedium()
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vViewAmtWater();
  }
*****************/

/********************************************************************
OnUpdateViewStimMedium

For the research version

  Message handler for updating the View Stimulation and quantity  menu item.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateViewStimulationMedium(CCmdUI *pCmdUI)
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }
/***********
void CMainFrame::OnUpdateViewStimMedium(CCmdUI *pCmdUI)
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }
*************/

/********************************************************************
OnGotoPreviousStimPeriod

For the research version

  Message handler for going to the previous stimulation period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnGotoPreviousStimPeriod()
  {

  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vGotoPrevPeriod();
  }

/********************************************************************
OnUpdateGotoPreviousStimPeriod

For the research version

  Message handler for updating the go to previous stimulation period menu item.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateGotoPreviousStimPeriod(CCmdUI *pCmdUI)
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }

/********************************************************************
OnGotoNextStimPeriod

For the research version

  Message handler for going to the next stimulation period.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnGotoNextStimPeriod()
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vGotoNextPeriod();
  }

/********************************************************************
OnUpdateGotoNextStimPeriod

For the research version

  Message handler for updating the go to next stimulation period menu item.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateGotoNextStimPeriod(CCmdUI *pCmdUI)
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile);
  }

/********************************************************************
OnViewMinPerGraph

For the research version

  Message handler for change the number of minute per graph.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnViewMinPerGraph()
  {
	
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vChangeMinPerGraph();
  }

/********************************************************************
OnUpdateViewMinPerGraph

For the research version

  Message handler for updating the change the number of minute per graph menu item.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateViewMinPerGraph(CCmdUI *pCmdUI)
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  pCmdUI->Enable(m_pDoc->m_bHaveFile); // && (theApp.m_ptStudyType == PT_RESEARCH));
  }

/***********************************************************************
OnCmdMsg - Called by the framework to route and dispatch command messages
           and to handle the update of command user-interface objects.

           Here we extend the standard command routing to intercept command
           messages with variable command IDs.  The variable command IDs
           are those for the post-stimulation periods in the View and
           Analyze menus.

inputs: command ID
        command notification code
        used according to the value of nCode
        if not NULL, filled in with pTarget and pmf members of
          CMDHANDLERINFO structure, instead of dispatching the
          command.  Typically NULL.

Returns: nonzero if message handled, otherwise 0.

Comments:
   See the documentation for CCmdTarget::OnCmdMsg() for further
   information.  Command routing is also discussed in tech note #21.
***********************************************************************/
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
  {
  BOOL bHandled = FALSE;

  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Only for research version.
    if (pHandlerInfo == NULL
    && nID >= MENU_ID_START && nID <= (MENU_ID_START + (MENU_ID_ARRAY_SIZE * 2)))
      {
      if(g_pFrame == NULL)
        g_pFrame = this;  // Make we have a pointer to this object.

      if(m_pDoc == NULL)
        m_pDoc = (CEGGSASDoc *)GetActiveDocument(); // Get current document.

      bHandled = m_pDoc->bGoToCmdMsg(nID, nCode, pExtra, pHandlerInfo);
      }
    }
  //if(nID == 57603)
  //  { // Save patient file menu item.
  //  if (nCode == CN_UPDATE_COMMAND_UI)
  //    {
  //    CCmdUI *pCmdUI;
  //    pCmdUI = (CCmdUI *)pExtra;
  //    if(g_bHiddenPatients == TRUE)
  //      {
  //      pCmdUI->Enable(FALSE);
  //      bHandled = TRUE;
  //      }
  //    }
  //  }

  if(bHandled == TRUE)
    return(TRUE);
  else
    {
      // If we didn't process the command, call the base class
	    // version of OnCmdMsg so the message-map can handle the message
	  return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    }
  }

/********************************************************************
OnUpdateViewSummGraph

  Message handler for updating the View summary graph menu item.
    Enable/disable the menu item based on if all periods are analyzed.
********************************************************************/
void CMainFrame::OnUpdateViewSummGraph(CCmdUI *pCmdUI)
  {

  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(m_pDoc == NULL)
      m_pDoc = (CEGGSASDoc *)GetActiveDocument();
    if(theApp.m_ptStudyType == PT_RESEARCH)
      {
      pCmdUI->Enable(m_pDoc->m_bHaveFile == TRUE
        && m_pDoc->bIs_period_analyzed(m_pDoc->m_uCurrentPeriod));
      //pCmdUI->Enable(FALSE);
      }
    else
      {
      //if(m_pDoc == NULL)
      //  m_pDoc = (CEGGSASDoc *)GetActiveDocument();
      if(m_pDoc->m_bHaveFile == TRUE && m_pDoc->bAre_standard_periods_analyzed())
        pCmdUI->Enable(TRUE);
      else
        pCmdUI->Enable(FALSE);
      }
    }
  }

/********************************************************************
OnViewSummGraph

  Message handler for viewing the summary graph.
    Create Report view and make it active.
********************************************************************/
void CMainFrame::OnViewSummGraph()
  {
  CRuntimeClass* pNewViewClass;
  bool bOk;
	
  if(g_pFrame == NULL)
    g_pFrame = this;

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();

  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // For Research study, make sure its ok to draw them.
    if(m_pDoc->bCan_graph_summary_graphs() == true)
      bOk = true;
    else
      { // Need to go to a What IF to modify the periods.
      CString cstrTitle, cstrMsg;
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_GOTO_WHATIF1) + "\r\n"
        + g_pLang->cstrLoad_string(ITEM_G_GOTO_WHATIF2);
      cstrTitle = g_pLang->cstrLoad_string(ITEM_G_INFORMATION);
      if(AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg,
        //"The first 3 post water periods must be at least 10 minutes in length.\r\nSelect OK to go to a What If scenario to create the 10 minute periods.",
        cstrTitle, MB_OKCANCEL | MB_ICONQUESTION | MB_SYSTEMMODAL) == IDOK)
        { // display WhatIf
        m_pDoc->vWhatIf(INV_LONG);
        }
      bOk = false;
     }
    }
  else
    bOk = true;

  if(bOk == true)
    {
    if(m_uViewID != VIEWID_REPORT)
      { // Set the child window ID of the active view to AFX_IDW_PANE_FIRST.
        // This is necessary so that CFrameWnd::RecalcLayout will allocate
        // this "first pane" to that portion of the frame window's client
        // area not allocated to control bars.  Set the child ID of
        // the previously active view to some other ID; we will use the
        // command ID as the child ID.
      CView* pOldActiveView = GetActiveView();
      ::SetWindowLong(pOldActiveView->m_hWnd, GWL_ID, ID_FILE_OPEN);

      pNewViewClass = RUNTIME_CLASS(CReportView);
        // create the new view
      CCreateContext context;
      context.m_pNewViewClass = pNewViewClass;
      context.m_pCurrentDoc = GetActiveDocument();
      CView* pNewView = STATIC_DOWNCAST(CView, CreateView(&context));
      if (pNewView != NULL)
        { // the new view is there, but invisible and not active...
        pNewView->ShowWindow(SW_SHOW);
        pNewView->OnInitialUpdate();
        SetActiveView(pNewView);
        RecalcLayout();
        m_uViewID = VIEWID_REPORT;
          // finally destroy the old view...
        pOldActiveView->DestroyWindow();
        }
      //if(m_pDoc == NULL)
      //  m_pDoc = (CEGGSASDoc *)GetActiveDocument();
      m_pDoc->m_bReportChanged = false;
      m_pDoc->m_uReportItemToView = RPT_VIEW_ITEM_SUMM_GRPHS;
      }
    else
      {
      m_pDoc->m_uReportItemToView = RPT_VIEW_ITEM_SUMM_GRPHS;
      ((CReportView *)GetActiveView())->vReset_view();
      if(m_pDoc->m_bReAnalyze == true)
        {
        ((CReportView *)GetActiveView())->vAnalyze_data(DATA_SHEET_TYPE);
        //((CReportView *)GetActiveView())->vAnalyze_data_for_RSA();
        m_pDoc->m_bReAnalyze = false;
        }
      GetActiveView()->Invalidate();
      }
    }
  }

/********************************************************************
OnUpdateViewDataSheet

  Message handler for updating the View data sheet menu item.
    Enable/disable the menu item based on if all periods are analyzed.
********************************************************************/
void CMainFrame::OnUpdateViewDataSheet(CCmdUI *pCmdUI)
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(m_pDoc->m_bHaveFile == TRUE && m_pDoc->bAre_standard_periods_analyzed())
      pCmdUI->Enable(TRUE);
    else
      pCmdUI->Enable(FALSE);
    }
  }

/********************************************************************
OnViewDataSheet

  Message handler for viewing the data sheet.
    Create Report view and make it active.
********************************************************************/
void CMainFrame::OnViewDataSheet()
  {
  CRuntimeClass* pNewViewClass;
	
  if(g_pFrame == NULL)
    g_pFrame = this;

  if(m_uViewID != VIEWID_REPORT)
    { // Set the child window ID of the active view to AFX_IDW_PANE_FIRST.
      // This is necessary so that CFrameWnd::RecalcLayout will allocate
      // this "first pane" to that portion of the frame window's client
      // area not allocated to control bars.  Set the child ID of
      // the previously active view to some other ID; we will use the
      // command ID as the child ID.
    CView* pOldActiveView = GetActiveView();
    ::SetWindowLong(pOldActiveView->m_hWnd, GWL_ID, ID_FILE_OPEN);

    pNewViewClass = RUNTIME_CLASS(CReportView);
      // create the new view
    CCreateContext context;
    context.m_pNewViewClass = pNewViewClass;
    context.m_pCurrentDoc = GetActiveDocument();
    CView* pNewView = STATIC_DOWNCAST(CView, CreateView(&context));
    if (pNewView != NULL)
      { // the new view is there, but invisible and not active...
      pNewView->ShowWindow(SW_SHOW);
      pNewView->OnInitialUpdate();
      SetActiveView(pNewView);
      RecalcLayout();
      m_uViewID = VIEWID_REPORT;
        // finally destroy the old view...
      pOldActiveView->DestroyWindow();
      }
    if(m_pDoc == NULL)
      m_pDoc = (CEGGSASDoc *)GetActiveDocument();
    m_pDoc->m_bReportChanged = false;
    m_pDoc->m_uReportItemToView = RPT_VIEW_ITEM_DATA_SHEET;
    }
  else
    {
    m_pDoc->m_uReportItemToView = RPT_VIEW_ITEM_DATA_SHEET;
    ((CReportView *)GetActiveView())->vReset_view();
    if(m_pDoc->m_bReAnalyze == true)
      {
      ((CReportView *)GetActiveView())->vAnalyze_data(DATA_SHEET_TYPE);
      m_pDoc->m_bReAnalyze = false;
      }
    GetActiveView()->Invalidate();
    }
  }

/********************************************************************
OnViewEvents

  Message handler for viewing the events.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnViewEvents()
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vViewEvents();
  }

/********************************************************************
OnUpdateViewEvents

  Message handler for updating the view events menu items.
    Enable/disable the menu item based on having a patient.
********************************************************************/
void CMainFrame::OnUpdateViewEvents(CCmdUI *pCmdUI)
  {
  BOOL bEnable = FALSE;

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(g_bAsciiDataFile == true)
    pCmdUI->Enable(false); // Disable for data dump program.
  else
    {
    if(m_pDoc->m_bHavePatient == TRUE)
      {
      if(m_pDoc->m_pdPatient.Events.m_BLEvents.GetCount() > 0
      || m_pDoc->m_pdPatient.Events.m_PSEvents.GetCount() > 0)
        bEnable = TRUE;
      }
    pCmdUI->Enable(bEnable);
    }
  }

/********************************************************************
OnEditWhatIf

  Message handler for Editing the What If.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnEditWhatIf()
  {
	
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vWhatIf(INV_LONG);
  }

/********************************************************************
OnUpdateEditWhatIf

  Message handler for updating the Edit What If menu item.
    Enable/disable the menu item based on having a patient and if the
      study is analyzed.
********************************************************************/
void CMainFrame::OnUpdateEditWhatIf(CCmdUI *pCmdUI)
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Research study only
    if(g_bAsciiDataFile == true)
      pCmdUI->Enable(false); // Disable for data dump program.
    else
      {
      if(g_bHiddenPatients == FALSE)
        pCmdUI->Enable(m_pDoc->m_bHaveFile == TRUE && m_pDoc->m_bHavePatient);
      else
        pCmdUI->Enable(FALSE);
      //pCmdUI->Enable(m_pDoc->m_bHavePatient); // && m_pDoc->bIs_study_analyzed());
      }
    }
  }

/********************************************************************
OnHelpAboutReader

  Message handler for the "About 3CPM EGGSAS Reader..." help menu item.
    Start the About dialog box.
********************************************************************/
void CMainFrame::OnHelpAboutReader()
  {
  theApp.OnAppAbout();
  }

/********************************************************************
OnUpdateHelpAboutReader

  Message handler for updating the "About 3CPM EGGSAS Reader..." help menu item.
    Enable the menu item.
********************************************************************/
void CMainFrame::OnUpdateHelpAboutReader(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(TRUE);
  }

/********************************************************************
OnHelpAbout3cpmeggsascentraldatadepotreader

  Message handler for the "About 3CPM EGGSAS Central Data Depot Reader..."
    help menu item.
    Start the About dialog box.
********************************************************************/
void CMainFrame::OnHelpAbout3cpmeggsascentraldatadepotreader()
  {
  theApp.OnAppAbout();
  }

/********************************************************************
OnUpdateHelpAbout3cpmeggsascentraldatadepotreader

  Message handler for updating the "About 3CPM EGGSAS Central Data Depot Reader..."
    help menu item.
    Enable the menu item.
********************************************************************/
void CMainFrame::OnUpdateHelpAbout3cpmeggsascentraldatadepotreader(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(TRUE);
  }

/********************************************************************
vDisplay_what_if

  Message handler for the user message to display What If data.
    This will also be called if the user selects the original study
      from the What If dialog box.
********************************************************************/
LRESULT CMainFrame::vDisplay_what_if(WPARAM wParam, LPARAM lParam)
  {

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  vSetup_for_msg(); // Make sure everything is set up to process this message.
  m_pDoc->vDisplay_what_if();
  return(0);
  }

/******************
void CMainFrame::OnHelp(void)
  {


  //AfxGetApp()->WinHelp(HIDD_HELP_CONTENTS, HELP_CONTEXT);
//  theApp.vOn_help();
//  AfxGetApp()->OnHelp();
//  CWinApp::OnHelp();
//  MessageBox("The help file is not yet completed", "Information");
  }
*******************/

/////////////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
vActivate_ten_min_view - Activate the 10 minute view View Object.

  inputs: None.

  return: None.
********************************************************************/
void CMainFrame::vActivate_ten_min_view() 
  {
  CRuntimeClass* pNewViewClass;
	
  if(g_pFrame == NULL)
    g_pFrame = this;

    // Set the child window ID of the active view to AFX_IDW_PANE_FIRST.
    // This is necessary so that CFrameWnd::RecalcLayout will allocate
    // this "first pane" to that portion of the frame window's client
    // area not allocated to control bars.  Set the child ID of
    // the previously active view to some other ID; we will use the
    // command ID as the child ID.
  CView* pOldActiveView = GetActiveView();
  ::SetWindowLong(pOldActiveView->m_hWnd, GWL_ID, ID_ANALYZE_RSA);

  pNewViewClass = RUNTIME_CLASS(CTenMinView);
    // create the new view
  CCreateContext context;
  context.m_pNewViewClass = pNewViewClass;
  context.m_pCurrentDoc = GetActiveDocument();
  CView* pNewView = STATIC_DOWNCAST(CView, CreateView(&context));
  if (pNewView != NULL)
    { // the new view is there, but invisible and not active...
    pNewView->ShowWindow(SW_SHOW);
    pNewView->OnInitialUpdate();
    SetActiveView(pNewView);
    RecalcLayout();
    m_uViewID = VIEWID_TEN_MIN;
      // finally destroy the old view...
    pOldActiveView->DestroyWindow();
    }
  }

/********************************************************************
vSetup_for_msg - Make sure the 10 minute view is active.

  inputs: None.

  return: None.
********************************************************************/
void CMainFrame::vSetup_for_msg()
  {

  if(m_uViewID != VIEWID_TEN_MIN)
    vActivate_ten_min_view();

  if(g_pFrame == NULL)
    g_pFrame = this;

  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  }

/********************************************************************
cstrDelete_menu_item - Delete the specified item from the specified menu.

  inputs: The ID of the menu to delete the menu item from.
          The ID of the menu item to delete.

  return: CString object containing the text of the deleted menu item.
          If the menu item is not deleted the CString object is empty.
********************************************************************/
CString CMainFrame::cstrDelete_menu_item(unsigned uMenuID, unsigned uMenuItemID)
  {
/********************************/
  CMenu *pcmTopMenu, *pcmSubMenu = NULL;
  int iPos;

  m_cstrMenuIDLabel.Empty();
    // Locate the submenu
  pcmTopMenu = GetMenu(); //AfxGetApp()->m_pActiveWnd->GetMenu();
  for(iPos = pcmTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)
    {
    pcmSubMenu = pcmTopMenu->GetSubMenu(iPos);
    if(pcmSubMenu && pcmSubMenu->GetMenuItemID(0) == uMenuID)
      { // Before deleting the menu item, get its label so we can later
        // restore it.
      pcmSubMenu->GetMenuString(uMenuItemID, m_cstrMenuIDLabel, MF_BYCOMMAND);
      pcmSubMenu->DeleteMenu(uMenuItemID, MF_BYCOMMAND);
      break;
      }
    }
/*************************************/
  return(m_cstrMenuIDLabel);
  }

/********************************************************************
cstrDelete_menu_item_by_menu_pos - Delete the specified menu item by
                                   menu position and menu ID.

  inputs: Position of the horizontal menu item.
          The ID of the menu item to delete.

  return: CString object containing the text of the deleted menu item.
          If the menu item is not deleted the CString object is empty.
********************************************************************/
CString CMainFrame::cstrDelete_menu_item_by_menu_pos(unsigned uPos, unsigned uMenuItemID)
  {
/***********************************/
  CMenu *pcmSubMenu;

   // Get the submenu
  pcmSubMenu = GetMenu()->GetSubMenu(uPos);
    // Before deleting the menu item, get its label so we can later
    // restore it.
  pcmSubMenu->GetMenuString(uMenuItemID, m_cstrMenuIDLabel, MF_BYCOMMAND);
  if(m_cstrMenuIDLabel.IsEmpty() == FALSE)
    pcmSubMenu->DeleteMenu(uMenuItemID, MF_BYCOMMAND);
/****************************************/
  return(m_cstrMenuIDLabel);
  }

void CMainFrame::vDelete_menu_item(unsigned uPos, unsigned uPos1)
  {
  CMenu *pcmSubMenu;

   // Get the submenu
  pcmSubMenu = GetMenu()->GetSubMenu(uPos);
  pcmSubMenu->DeleteMenu(uPos1, MF_BYPOSITION);
  }

/********************************************************************
vChange_menu_item_by_pos - Change the menu text and prompt according to
                           its position in the menu.

  inputs: 0 based position in the menu.
          Menu text
          Prompt text

  return: None.
********************************************************************/
void CMainFrame::vChange_menu_item_by_pos(unsigned uPos, CString cstrMenuText)
  {
  CMenu *pMenu;
  
  pMenu = GetMenu();
  pMenu->ModifyMenu(uPos, MF_BYPOSITION | MF_STRING, 0, (LPCTSTR)cstrMenuText);
  }

void CMainFrame::vChange_menu_item_by_pos_pos(unsigned uPos, unsigned uPos1,
                                              CString cstrMenuText)
  {
  CMenu *pMenu, *pSubMenu;
  
  pMenu = GetMenu();
  pSubMenu = pMenu->GetSubMenu(uPos1);

  pSubMenu->ModifyMenu(uPos, MF_BYPOSITION | MF_STRING, 0, (LPCTSTR)cstrMenuText);
  }

void CMainFrame::vChange_menu_item_by_ID(unsigned uID, CString cstrMenuText)
  {
  CMenu *pMenu;
  //MENUINFO mi;
  
  pMenu = GetMenu();
  pMenu->ModifyMenu(uID, MF_BYCOMMAND | MF_STRING, uID, (LPCTSTR)cstrMenuText);
  }

/********************************************************************
vCheck_menu_item - Check/uncheck the menu item specified its menu ID.

  inputs: Menu ID of the item to be checked/unchecked.
          true to check the menu item, false to uncheck it.

  return: None.
********************************************************************/
void CMainFrame::vCheck_menu_item(unsigned uID, bool bCheck)
  {
  CMenu *pMenu;
  
  pMenu = GetMenu();
  if(bCheck == true)
    pMenu->CheckMenuItem(uID, MF_BYCOMMAND | MF_CHECKED);
  else
    pMenu->CheckMenuItem(uID, MF_BYCOMMAND | MF_UNCHECKED);
  }

  //CMenu *pcmTopMenu, *pcmSubMenu = NULL;
  ////int iPos;

  //  // Locate the submenu
  //pcmTopMenu = GetMenu(); //AfxGetApp()->m_pActiveWnd->GetMenu();
  //pcmTopMenu->ModifyMenu(MF_BYPOSITION, MF_STRING, uPos, cstrMenuText);
  //pcmSubMenu = pcmTopMenu->GetSubMenu(uPos);
  //pcmSubMenu->ModifyMenu();


  //for(iPos = pcmTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)
  //  {
  //  pcmSubMenu = pcmTopMenu->GetSubMenu(iPos);
  //  if(pcmSubMenu && pcmSubMenu->GetMenuItemID(0) == uMenuID)
  //    { // Got the menu item, change its text and prompt.
  //    break;
  //    }
  //  }
/***************************************************************/
        // Insert before separator bar.
/********************************************************************
vAdd_menu_item - Insert the specified menu item into the specified menu before
                 the specified menu item.

  inputs: The ID of the menu to insert the menu item into.
          The ID of the menu item to insert.
          The ID of the menu item that the new menu item gets inserted before.

  return: None.
********************************************************************/
void CMainFrame::vAdd_menu_item(unsigned uMenuID, unsigned uMenuItemID,
unsigned uInsertBeforeID, CString cstrText)
  {
  CMenu *pcmTopMenu, *pcmSubMenu = NULL;
  CString cstrMenuText;
  int iPos;
  bool bMenuItemExists;

  bMenuItemExists = false;
/********************************
    // First find out if the menu item already exists.
  pcmTopMenu = GetMenu(); //AfxGetApp()->m_pActiveWnd->GetMenu();
  for(iPos = pcmTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)
    {
    pcmSubMenu = pcmTopMenu->GetSubMenu(iPos);
    if(pcmSubMenu && pcmSubMenu->GetMenuItemID(0) == uMenuID)
      { // Found the top menu, now look for the actual menu item.
      pcmSubMenu->GetMenuString(uMenuItemID, cstrMenuText, MF_BYCOMMAND);
      if(cstrMenuText.IsEmpty() == FALSE)
        bMenuItemExists = true;
      break;
      }
    }
***********************************/
  if(bMenuItemExists == false)
    { // Menu item doesn't exist, so OK to insert it.
      // Locate the submenu
    pcmSubMenu = NULL;
    pcmTopMenu = GetMenu(); //AfxGetApp()->m_pActiveWnd->GetMenu();
    for(iPos = pcmTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)
      {
      pcmSubMenu = pcmTopMenu->GetSubMenu(iPos);
      if (pcmSubMenu && pcmSubMenu->GetMenuItemID(0) == uMenuID)
        {
        if(cstrText.IsEmpty() == true)
          cstrText = m_cstrMenuIDLabel;
        pcmSubMenu->InsertMenu(uInsertBeforeID, MF_BYCOMMAND, uMenuItemID, cstrText);
        break;
        }
      }
    }
  }

/********************************************************************
vEnable_menu_item - Enable/disable a menu item.

inputs: Position of the horizontal menu item.
        Position of the vertical menu item.
        TRUE to enable the item, FALSE to disable the item.

return: None.
********************************************************************/
void CMainFrame::vEnable_menu_item(unsigned uPos, unsigned uPos1, bool bEnable)
  {
  CMenu *pcmSubMenu;

   // Get the submenu
  pcmSubMenu = GetMenu()->GetSubMenu(uPos);
  if(bEnable == true)
    pcmSubMenu->EnableMenuItem(uPos1, MF_BYPOSITION | MF_ENABLED);
  else
    pcmSubMenu->EnableMenuItem(uPos1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
  }

/********************************************************************
vEnable_menu_item - Enable/disable a menu item.

inputs: Position of the horizontal menu item.
        Position of the vertical menu item.
        TRUE to enable the item, FALSE to disable the item.

return: None.
********************************************************************/
void CMainFrame::vEnable_menu_item_by_id(unsigned uPos, unsigned uID, bool bEnable)
  {
  CMenu *pcmSubMenu;

   // Get the submenu
  pcmSubMenu = GetMenu()->GetSubMenu(uPos);
  if(bEnable == true)
    pcmSubMenu->EnableMenuItem(uID, MF_BYCOMMAND | MF_ENABLED);
  else
    pcmSubMenu->EnableMenuItem(uID, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
  }


/********************************************************************
vEnable_toolbar_button - Enable/disable (show/hide) a button on the toolbar.

inputs: Command ID of the button.
        TRUE to enable/show the button, FALSE to disable/hide the button.
           

return: None.
********************************************************************/
void CMainFrame::vEnable_toolbar_button(int iCmdID, BOOL bEnable)
  {

    // Get the toolbar control.
  CToolBarCtrl &toolctrl = m_wndToolBar.GetToolBarCtrl();
    // Enable/disable the button.
//  toolctrl.EnableButton(iCmdID, FALSE); // doesn't seem to work.
  if(bEnable == FALSE)
    toolctrl.HideButton(iCmdID, TRUE);
  else
    toolctrl.HideButton(iCmdID, FALSE);
  }

void CMainFrame::vSet_toolbar_text(int iCmdID, char *pszText)
  {
  //TBBUTTONINFO tbbi;
  TOOLINFO ti;
  CToolInfo cti;

    // Get the toolbar control.
  CToolBarCtrl &toolctrl = m_wndToolBar.GetToolBarCtrl();
  CString strText = m_wndToolBar.GetButtonText(0);

  CToolTipCtrl *pttc = toolctrl.GetToolTips();
  if(pttc != NULL)
    {
    pttc->GetToolInfo(cti, this, iCmdID);
    ti.lpszText = pszText;
    pttc->SetToolInfo(&ti);
    }
  //pttc->UpdateTipText(pszText, this, iCmdID);


  //toolctrl.GetButtonInfo(iCmdID, &tbbi);
  //tbbi.pszText = pszText; //(LPCTSTR)cstrText;
  //toolctrl.SetButtonInfo(iCmdID, &tbbi);
  }

/********************************************************************
vDelete_toolbar_button - Delete a button on the toolbar.

inputs: Command ID of the button.

return: None.

void CMainFrame::vDelete_toolbar_button(UINT uToolBarCmd)
  {

    // Get the toolbar control.
  CToolBarCtrl &toolctrl = m_wndToolBar.GetToolBarCtrl();
    // Get the index of the toolbar button based on the command ID.
  UINT uIndex = toolctrl.CommandToIndex(uToolBarCmd);
    // Delete the button.
  toolctrl.DeleteButton(uIndex);
  }
********************************************************************/

/********************************************************************
vCreate_research_toolbar - Create the toolbar for a research study.
                           Hide buttons used for the standard study and
                           show buttons used for the research study.

inputs: None.

return: None.
********************************************************************/
void CMainFrame::vCreate_research_toolbar()
  {

  m_wndToolBar.GetToolBarCtrl().HideButton(ID_GOTO_NEXT_PERIOD, TRUE);
  m_wndToolBar.GetToolBarCtrl().HideButton(ID_GOTO_PREV_PERIOD, TRUE);

  m_wndToolBar.GetToolBarCtrl().HideButton(ID_GOTO_NEXT_STIM_PERIOD, FALSE);
  m_wndToolBar.GetToolBarCtrl().HideButton(ID_GOTO_PREVIOUS_STIM_PERIOD, FALSE);
  m_wndToolBar.GetToolBarCtrl().HideButton(ID_EDIT_WHAT_IF, FALSE);

  m_wndToolBar.GetToolBarCtrl().HideButton(ID_VIEW_FILTER_RSA, FALSE);
  m_wndToolBar.GetToolBarCtrl().HideButton(ID_SETTINGS_FREQUENCYRANGE, FALSE);

//#ifdef EGGSAS_READER
#if EGGSAS_READER > READER_NONE
  m_wndToolBar.GetToolBarCtrl().HideButton(ID_FILE_NEW, TRUE);
#endif
  }

/********************************************************************
vCreate_standard_toolbar - Create the toolbar for a standard study.
                           Hide buttons used for the research study and
                           show buttons used for the standard study.

inputs: None.

return: None.
********************************************************************/
void CMainFrame::vCreate_standard_toolbar()
  {

  m_wndToolBar.GetToolBarCtrl().HideButton(ID_GOTO_NEXT_PERIOD, FALSE);
  m_wndToolBar.GetToolBarCtrl().HideButton(ID_GOTO_PREV_PERIOD, FALSE);

  m_wndToolBar.GetToolBarCtrl().HideButton(ID_GOTO_NEXT_STIM_PERIOD, TRUE);
  m_wndToolBar.GetToolBarCtrl().HideButton(ID_GOTO_PREVIOUS_STIM_PERIOD, TRUE);
  m_wndToolBar.GetToolBarCtrl().HideButton(ID_EDIT_WHAT_IF, TRUE);

  m_wndToolBar.GetToolBarCtrl().HideButton(ID_VIEW_FILTER_RSA, TRUE);
  m_wndToolBar.GetToolBarCtrl().HideButton(ID_SETTINGS_FREQUENCYRANGE, TRUE);


//#ifdef EGGSAS_READER
#if EGGSAS_READER > READER_NONE
  m_wndToolBar.GetToolBarCtrl().HideButton(ID_FILE_NEW, TRUE);
#endif
  }

void CMainFrame::vHide_toolbar_item(unsigned int uiID)
  {
  m_wndToolBar.GetToolBarCtrl().HideButton(uiID, TRUE);
  }

/********************************************************************
vGet_status_bar_rect - Get the rectangle describing the status bar window.

inputs: None.

return: None.
********************************************************************/
void CMainFrame::vGet_status_bar_rect(RECT *pRect)
  {

  m_wndStatusBar.GetWindowRect(pRect);
  }

/********************************************************************
vView_report - Create a report view and display the report.

inputs: None.

return: None.
********************************************************************/
void CMainFrame::vView_report()
  {
  CRuntimeClass* pNewViewClass;
	
  if(g_pFrame == NULL)
    g_pFrame = this;

  if(m_uViewID != VIEWID_REPORT)
    { // Set the child window ID of the active view to AFX_IDW_PANE_FIRST.
      // This is necessary so that CFrameWnd::RecalcLayout will allocate
      // this "first pane" to that portion of the frame window's client
      // area not allocated to control bars.  Set the child ID of
      // the previously active view to some other ID; we will use the
      // command ID as the child ID.
    CView* pOldActiveView = GetActiveView();
    ::SetWindowLong(pOldActiveView->m_hWnd, GWL_ID, ID_FILE_OPEN);

    pNewViewClass = RUNTIME_CLASS(CReportView);
      // create the new view
    CCreateContext context;
    context.m_pNewViewClass = pNewViewClass;
    context.m_pCurrentDoc = GetActiveDocument();
    CView* pNewView = STATIC_DOWNCAST(CView, CreateView(&context));
    if (pNewView != NULL)
      { // the new view is there, but invisible and not active...
      pNewView->ShowWindow(SW_SHOW);
      pNewView->OnInitialUpdate();
      SetActiveView(pNewView);
      RecalcLayout();
      m_uViewID = VIEWID_REPORT;
        // finally destroy the old view...
      pOldActiveView->DestroyWindow();
      }
    if(m_pDoc == NULL)
      m_pDoc = (CEGGSASDoc *)GetActiveDocument();
    m_pDoc->m_bReportChanged = false;
    m_pDoc->m_uReportItemToView = RPT_VIEW_ITEM_NONE;
    }
  else
    {
    m_pDoc->m_uReportItemToView = RPT_VIEW_ITEM_NONE;
    ((CReportView *)GetActiveView())->vReset_view();
    if(m_pDoc->m_bReAnalyze == true)
      {
      ((CReportView *)GetActiveView())->vAnalyze_data(DATA_SHEET_TYPE);
      ((CReportView *)GetActiveView())->vAnalyze_data_for_RSA();
      m_pDoc->m_bReAnalyze = false;
      }
    GetActiveView()->Invalidate();
    }
  }




/********************************************************************
OnFileSaverawdatainascii

  Message handler for saving the raw data to an ASCII file for the
  Research version.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnFileSaverawdatainascii()
  {
  // TODO: Add your command handler code here
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vSave_ascii_data_file();
  }

/********************************************************************
OnUpdateFileSaverawdatainascii

  Message handler for updating the Save Raw Data in ASCII menu item
  for the Research version.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateFileSaverawdatainascii(CCmdUI *pCmdUI)
  {
  // TODO: Add your command update UI handler code here
   pCmdUI->Enable(m_pDoc->m_bHaveFile == TRUE);
 }

/********************************************************************
OnFileSaverawdatainasciiWL

  Message handler for saving the raw data to an ASCII file for the
  Standard (WaterLoad) version.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnFileSaverawdatainasciiWL()
  {
  // TODO: Add your command handler code here
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vSave_ascii_data_file();
  }

/********************************************************************
OnUpdateFileSaverawdatainasciiWL

  Message handler for updating the Save Raw Data in ASCII menu item
  for the Standard (WaterLoad) version.
    Enable/disable the menu item based on having a patient data file.
********************************************************************/
void CMainFrame::OnUpdateFileSaverawdatainasciiWL(CCmdUI *pCmdUI)
  {
  // TODO: Add your command update UI handler code here
   pCmdUI->Enable(m_pDoc->m_bHaveFile == TRUE);
  }

/********************************************************************
OnReportShowHideStudyDates

  Message handler for displaying the Hide/Show Study Dates for the
  CDDR version for research.
    Call the current document to process.
********************************************************************/
void CMainFrame::OnReportShowHideStudyDates()
  {
  // TODO: Add your command handler code here
  if(m_pDoc == NULL)
    m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  m_pDoc->vShowHideStudyDates();
  }

/********************************************************************
OnUpdateReportShowHideStudyDates

  Message handler for updating the Hide/Show Study Dates menu item.
  for the CDDR version for research.
  Put a check on the menu item if the study dates are hidden.
********************************************************************/
void CMainFrame::OnUpdateReportShowHideStudyDates(CCmdUI *pCmdUI)
  {
  // TODO: Add your command update UI handler code here
   pCmdUI->Enable(TRUE);
   if(g_pConfig->m_bHideStudyDates == false)
     pCmdUI->SetCheck(0);
   else
     pCmdUI->SetCheck(1);
  }

/********************************************************************
vDisplay_hide_show_study_dates

  Message handler for updating the WMUSER_DISPLAY_HIDE_SHOW_STUDY_DATES message.
  For the CDDR version.
  Display the Hide/Show Study Dates
********************************************************************/
LRESULT CMainFrame::vDisplay_hide_show_study_dates(WPARAM wParam, LPARAM lParam)
  {
  OnReportShowHideStudyDates();
  //if(m_pDoc == NULL)
  //  m_pDoc = (CEGGSASDoc *)GetActiveDocument();
  ////vSetup_for_msg(); // Make sure everything is set up to process this message.
  //m_pDoc->vShowHideStudyDates();
  return(0);
  }

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
  {
  // TODO: Add your specialized code here and/or call the base class

  return CFrameWnd::OnCommand(wParam, lParam);
  }

BOOL CMainFrame::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
BOOL bHandled = FALSE;
LPSTR pstrText;

    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    //UINT nID =pNMHDR->idFrom;
    pstrText = g_pLang->pstrGet_tooltip(pNMHDR->idFrom);
    if(*pstrText != NULL)
      {
      pTTT->lpszText = pstrText;
      bHandled = TRUE;
      }

    //switch(nID)
    //  {
    //  case ID_FILE_NEW:
    //    pTTT->lpszText = g_pLang->pstrGet_tooltip(ID_FILE_NEW);
    //    bHandled = TRUE;
    //    break;
    //  }

    //if (pTTT->uFlags & TTF_IDISHWND)
    //{
    //    // idFrom is actually the HWND of the tool
    //    nID = ::GetDlgCtrlID((HWND)nID);
    //    if(nID)
    //    {
    //        pTTT->lpszText = MAKEINTRESOURCE(nID);
    //        pTTT->hinst = AfxGetResourceHandle();
    //        return(TRUE);
    //    }
    //}
    return(bHandled);
}

