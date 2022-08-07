/********************************************************************
MainFrm.h

Copyright (C) 2003,2004,2007,2008,2009,2011,2016 3CPM Company, Inc.  All rights reserved.

  Header file for the CMainFrame class.

HISTORY:
03-OCT-03  RET  New.
10-MAY-07  RET  Research Version
                  Add function prototypes.
05-FEB-08  RET  Version 2.03
                  Add prototypes for new messages.
                  Add menu commands to the message map:
                    OnEditWhatIf(), OnUpdateEditWhatIf(), vDisplay_what_if()
                  Remove menu commands from the message map:
                    OnAnalyzePostwater40(), OnUpdateAnalyzePostwater40(),
                    OnViewPostwater40(), OnUpdateViewPostwater40()
30-JUN-08  RET  Version 2.05
                  Enable File Print Preview to create a report view.
                    Add prototypes for methods: vView_report(),
                      OnFilePrintPreview(), OnUpdateFilePrintPreview().
13-FEB-09  RET  Version 2.08i
                  Add method prototypes:
                    OnUpdateFileNewPatient(), OnUpdateFileExistPatient(),
                    vEnable_menu_item(), vEnable_toolbar_button()
23-SEP-11  RET
             Changes to write an ASCII raw data file and not allow any analysis.
               Add event handler prototypes: OnFileSaverawdatainascii(),
                 OnUpdateFileSaverawdatainascii(), OnFileSaverawdatainasciiWL(),
                 OnUpdateFileSaverawdatainasciiWL()
02-JUN-16  RET
             Add feature to hide study dates.
               Add prototypes for methods: OnReportShowHideStudyDates(), 
                 OnUpdateReportShowHideStudyDates(), vDisplay_hide_show_study_dates()
********************************************************************/

#if !defined(AFX_MAINFRM_H__70F3BD78_14DE_4493_AF4C_41F24244B60F__INCLUDED_)
#define AFX_MAINFRM_H__70F3BD78_14DE_4493_AF4C_41F24244B60F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

  // Defines for the view that is currently being displayed.
#define VIEWID_NONE 0
#define VIEWID_TEN_MIN 1
#define VIEWID_RSA 2
#define VIEWID_REPORT 3

class CMainFrame : public CFrameWnd
{
friend class CEGGSASDoc;  
	
//protected: // create from serialization only
public:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  unsigned short m_uViewID; // Current view.
  CString m_cstrMenuIDLabel; // Holds the label for the fourth postwater period
                             // menu item when it is removed from the menus.

// Operations
public:
  void vActivate_ten_min_view();
  void vSetup_for_msg();
  CString cstrDelete_menu_item(unsigned uMenuID, unsigned uMenuItemID);
  void vAdd_menu_item(unsigned uMenuID, unsigned uMenuItemID,
                      unsigned uInsertBeforeID, CString cstrText);
  CString cstrDelete_menu_item_by_menu_pos(unsigned uPos, unsigned uMenuItemID);
  void vDelete_menu_item(unsigned uPos, unsigned uPos1);
  void vEnable_menu_item(unsigned uPos, unsigned uPos1, bool bEnable);
  void vEnable_toolbar_button(int iCmdID, BOOL bEnable);
  void vEnable_menu_item_by_id(unsigned uPos, unsigned uID, bool bEnable);

  void CMainFrame::vChange_menu_item_by_pos(unsigned uPos, CString cstrMenuText);
  void vChange_menu_item_by_pos_pos(unsigned uPos, unsigned uPos1, CString cstrMenuText);
  void vChange_menu_item_by_ID(unsigned uID, CString cstrMenuText);
  void vSet_toolbar_text(int iCmdID, char *pszText); //CString cstrText);
  void vCheck_menu_item(unsigned uID, bool bCheck);
  void vHide_toolbar_item(unsigned int uiID);

//  void vDelete_toolbar_button(UINT uToolBarCmd);
  void vCreate_research_toolbar();
  void vCreate_standard_toolbar();
  void vGet_status_bar_rect(RECT *pRect);
  void vView_report();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

//protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
//protected:
public:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAnalyzeRsa();
	afx_msg void OnFileOpen();
	afx_msg void OnAnalyzeBaseline();
	afx_msg void OnUpdateAnalyzeBaseline(CCmdUI* pCmdUI);
	afx_msg void OnAnalyzeCurrent();
	afx_msg void OnUpdateAnalyzeCurrent(CCmdUI* pCmdUI);
	afx_msg void OnAnalyzeDiagnosis();
	afx_msg void OnUpdateAnalyzeDiagnosis(CCmdUI* pCmdUI);
	afx_msg void OnAnalyzePostwater10();
	afx_msg void OnUpdateAnalyzePostwater10(CCmdUI* pCmdUI);
	afx_msg void OnAnalyzePostwater20();
	afx_msg void OnUpdateAnalyzePostwater20(CCmdUI* pCmdUI);
	afx_msg void OnAnalyzePostwater30();
	afx_msg void OnUpdateAnalyzePostwater30(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAnalyzeRsa(CCmdUI* pCmdUI);
	afx_msg void OnEditAmtWater();
	afx_msg void OnUpdateEditAmtWater(CCmdUI* pCmdUI);
	afx_msg void OnEditPatientDemograhics();
	afx_msg void OnUpdateEditPatientDemograhics(CCmdUI* pCmdUI);
	afx_msg void OnEditPatientDiagHist();
	afx_msg void OnUpdateEditPatientDiagHist(CCmdUI* pCmdUI);
	afx_msg void OnEditPatientMeds();
	afx_msg void OnUpdateEditPatientMeds(CCmdUI* pCmdUI);
	afx_msg void OnEditPreStudyInfo();
	afx_msg void OnUpdateEditPreStudyInfo(CCmdUI* pCmdUI);
	afx_msg void OnEditSuggestedDiag();
	afx_msg void OnUpdateEditSuggestedDiag(CCmdUI* pCmdUI);
	afx_msg void OnGotoEnd();
	afx_msg void OnUpdateGotoEnd(CCmdUI* pCmdUI);
	afx_msg void OnGotoNextPeriod();
	afx_msg void OnUpdateGotoNextPeriod(CCmdUI* pCmdUI);
	afx_msg void OnGotoPrevPeriod();
	afx_msg void OnUpdateGotoPrevPeriod(CCmdUI* pCmdUI);
	afx_msg void OnGotoStart();
	afx_msg void OnUpdateGotoStart(CCmdUI* pCmdUI);
	afx_msg void OnViewAmtWater();
	afx_msg void OnUpdateViewAmtWater(CCmdUI* pCmdUI);
	afx_msg void OnViewBaseline();
	afx_msg void OnUpdateViewBaseline(CCmdUI* pCmdUI);
	afx_msg void OnViewPatientDemograhics();
	afx_msg void OnUpdateViewPatientDemograhics(CCmdUI* pCmdUI);
	afx_msg void OnViewPatientDiagHist();
	afx_msg void OnUpdateViewPatientDiagHist(CCmdUI* pCmdUI);
	afx_msg void OnViewPatientMeds();
	afx_msg void OnUpdateViewPatientMeds(CCmdUI* pCmdUI);
	afx_msg void OnViewPostwater10();
	afx_msg void OnUpdateViewPostwater10(CCmdUI* pCmdUI);
	afx_msg void OnViewPostwater20();
	afx_msg void OnUpdateViewPostwater20(CCmdUI* pCmdUI);
	afx_msg void OnViewPostwater30();
	afx_msg void OnUpdateViewPostwater30(CCmdUI* pCmdUI);
	afx_msg void OnViewPreStudyInfo();
	afx_msg void OnUpdateViewPreStudyInfo(CCmdUI* pCmdUI);
	afx_msg void OnViewSuggestedDiag();
	afx_msg void OnUpdateViewSuggestedDiag(CCmdUI* pCmdUI);
	afx_msg void OnFileNewPatient();
	afx_msg void OnFileNew();
	afx_msg void OnFileExistPatient();
	afx_msg void OnReportView();
	afx_msg void OnUpdateReportView(CCmdUI* pCmdUI);
	afx_msg void OnReportPrint();
	afx_msg void OnUpdateReportPrint(CCmdUI* pCmdUI);
	afx_msg void OnReportFacInfo();
	afx_msg void OnUpdateReportFacInfo(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnViewRecommendations();
	afx_msg void OnUpdateViewRecommendations(CCmdUI* pCmdUI);
	afx_msg void OnEditRecommendations();
	afx_msg void OnUpdateEditRecommendations(CCmdUI* pCmdUI);
	afx_msg void OnReportConfigure();
	afx_msg void OnUpdateReportConfigure(CCmdUI* pCmdUI);
	afx_msg void OnGotoCurrentPeriod();
	afx_msg void OnUpdateGotoCurrentPeriod(CCmdUI* pCmdUI);
	afx_msg void OnFileBackup();
	afx_msg void OnUpdateFileBackup(CCmdUI* pCmdUI);
	afx_msg void OnEditRespiration();
	afx_msg void OnUpdateEditRespiration(CCmdUI* pCmdUI);
	afx_msg void OnViewRespiration();
	afx_msg void OnUpdateViewRespiration(CCmdUI* pCmdUI);
	afx_msg void OnViewRsa();
	afx_msg void OnUpdateViewRsa(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnUpdateViewSummGraph(CCmdUI *pCmdUI);
  afx_msg void OnViewSummGraph();
  afx_msg void OnUpdateViewDataSheet(CCmdUI *pCmdUI);
  afx_msg void OnViewDataSheet();
//  afx_msg void OnFileResumeStudy();
//  afx_msg void OnUpdateFileResumeStudy(CCmdUI *pCmdUI);
//  afx_msg void OnEditStimMedium();
//  afx_msg void OnUpdateEditStimMedium(CCmdUI *pCmdUI);
//  afx_msg void OnViewStimMedium();
//  afx_msg void OnUpdateViewStimMedium(CCmdUI *pCmdUI);
  afx_msg void OnGotoPreviousStimPeriod();
  afx_msg void OnUpdateGotoPreviousStimPeriod(CCmdUI *pCmdUI);
  afx_msg void OnGotoNextStimPeriod();
  afx_msg void OnUpdateGotoNextStimPeriod(CCmdUI *pCmdUI);
  afx_msg void OnViewMinPerGraph();
  afx_msg void OnUpdateViewMinPerGraph(CCmdUI *pCmdUI);
  afx_msg void OnFileResumeStudy();
  afx_msg void OnUpdateFileResumeStudy(CCmdUI *pCmdUI);
  afx_msg void OnEditStimulationMedium();
  afx_msg void OnUpdateEditStimulationMedium(CCmdUI *pCmdUI);
  afx_msg void OnViewStimulationMedium();
  afx_msg void OnUpdateViewStimulationMedium(CCmdUI *pCmdUI);
  virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
  afx_msg void OnReportCreatespreadsheetofdatasheet();
  afx_msg void OnUpdateReportCreatespreadsheetofdatasheet(CCmdUI *pCmdUI);
  afx_msg void OnViewEvents();
  afx_msg void OnUpdateViewEvents(CCmdUI *pCmdUI);
  afx_msg void OnEditWhatIf();
  afx_msg void OnUpdateEditWhatIf(CCmdUI *pCmdUI);
  afx_msg LRESULT CMainFrame::vDisplay_what_if(WPARAM wParam, LPARAM lParam);
  afx_msg void OnFilePrintPreview();
  afx_msg void OnUpdateFilePrintPreview(CCmdUI *pCmdUI);
  afx_msg void OnFilePrint();
  afx_msg void OnUpdateFilePrint(CCmdUI *pCmdUI);
  afx_msg void OnFileExportStudies();
  afx_msg void OnUpdateFileExportStudies(CCmdUI *pCmdUI);
  afx_msg void OnFileImportstudies();
  afx_msg void OnUpdateFileImportstudies(CCmdUI *pCmdUI);
  afx_msg void OnHelpAboutReader();
  afx_msg void OnUpdateHelpAboutReader(CCmdUI *pCmdUI);
  afx_msg void OnHelpAbout3cpmeggsascentraldatadepotreader();
  afx_msg void OnUpdateHelpAbout3cpmeggsascentraldatadepotreader(CCmdUI *pCmdUI);
  afx_msg void OnUpdateFileNewPatient(CCmdUI *pCmdUI);
  afx_msg void OnUpdateFileExistPatient(CCmdUI *pCmdUI);
  afx_msg void OnFileSaverawdatainascii();
  afx_msg void OnUpdateFileSaverawdatainascii(CCmdUI *pCmdUI);
  afx_msg void OnFileSaverawdatainasciiWL();
  afx_msg void OnUpdateFileSaverawdatainasciiWL(CCmdUI *pCmdUI);
  afx_msg void OnUpdateReportShowHideStudyDates(CCmdUI *pCmdUI);
  afx_msg void OnReportShowHideStudyDates();

    afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
//protected:
public:
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  LRESULT vDisplay_hide_show_study_dates(WPARAM wParam, LPARAM lParam);
};
//	afx_msg void OnAnalyzePostwater40();
//	afx_msg void OnUpdateAnalyzePostwater40(CCmdUI* pCmdUI);
//	afx_msg void OnViewPostwater40();
//	afx_msg void OnUpdateViewPostwater40(CCmdUI* pCmdUI);

//	afx_msg void OnHelp();

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__70F3BD78_14DE_4493_AF4C_41F24244B60F__INCLUDED_)
