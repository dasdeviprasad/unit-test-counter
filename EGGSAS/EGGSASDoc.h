/********************************************************************
EGGSASDoc.h

Copyright (C) 2003,2004,2007,2008,2011,2013,2020 3CPM Company, Inc.  All rights reserved.

  Header file for the CEGGSASDoc class.

HISTORY:
03-OCT-03  RET  New.
10-MAY-07  RET  Research Version
                  Add defines for the main menu items by position in the menu.
                  Add class variables: m_uViewMenuPostStimPeriodIDs[],
                    m_uAnalyzeMenuPostStimPeriodIDs[], m_uNumPeriodMenuItems,
                    m_iMinPerGraph
                  Add defines: MENU_ID_START, MENU_ID_ARRAY_SIZE, STUDY_RESUME
                  Add defines: STUDY_TYPE_NEW, STUDY_TYPE_EXISTING
29-JUN-08  RET  Add class variable m_iStudyType.
23-SEP-11  RET
             Begin changes to write an ASCII raw data file and not allow any analysis.
               Add file type define: FILE_TYPE_RAW_DATA
               Add method prototype: vSave_ascii_data_file()
16-MAR-13  RET
             Changes for frequency range seleclion.
               Add variable: m_bReAnalyze
               Add method prototype: vSet_freq_range(), bGet_study_type_freq_range()
14-NOV-20  RET
             Add ability to work with a remote database.
             Changed settings to allow the user to specify the remote database.
               Change prototype: cstrSelect_reader_db()
*******************************************************************/

#if !defined(AFX_EGGSASDOC_H__DDF8835F_22E3_4CCE_BF00_9F8D086ED4AF__INCLUDED_)
#define AFX_EGGSASDOC_H__DDF8835F_22E3_4CCE_BF00_9F8D086ED4AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArtifactID.h"
#include "Driver.h"

// Definitions for the main menu items by position in the menu
#define MENU_POS_FILE     0
#define MENU_POS_EDIT     1
#define MENU_POS_VIEW     2
#define MENU_POS_GOTO     3
#define MENU_POS_ANALYZE  4
#define MENU_POS_REPORT   5
#define MENU_POS_HELP     6

// Definitions for what sub-menu user is in.
#define SM_NEW_STUDY_NEW_PATIENT    1
#define SM_NEW_STUDY_EXIST_PATIENT  2
#define SM_NEW_STUDY  3
#define SM_EDIT       4
#define SM_VIEW       5

// Definitions for the type of study
#define STUDY_NONE           0
#define STUDY_EXIST_PATIENT  1
#define STUDY_NEW_PATIENT    2
#define STUDY_RESUME         3

// Parameter definitions for the method vSet_add_periods() for the study type.
#define STUDY_TYPE_NEW 1 // This is a new study.
#define STUDY_TYPE_EXISTING 2 // This is an existing study.

// Structure to hold the data required to set the attributes for
// the strip charts.
typedef struct
  {
//  int iMaxPoints; // Number of points on the graph.
  int iTotalPoints; // Total number of points that are shown at one time
                    // on a strip chart.
  double dMinEGG; // Minimum Y scale value on EGG graph.
  double dMaxEGG; // Maximum Y scale value on EGG graph.
  double dMinResp; // Minimum Y scale value on Respiration graph.
  double dMaxResp; // Maximum Y scale value on Respiration graph.
  double dMaxXScale; // Maximum scale of the X axis.
  } STRIP_CHART_ATTRIB;

// The research version requires a variable number of post-stimulation periods.
// Create an array of 100 which will be more than enough.  This array holds the
// menu IDs for the menu items.  The menu IDs must start after
// _APS_NEXT_COMMAND_VALUE which is found in resource.h.  Unused array elements
// are set to zero.
// Actually need 2 arrays, one for the View menu and one for the Analyze menu.
// These arrays are defined as class members.
#define MENU_ID_START 35000 //32959
#define MENU_ID_ARRAY_SIZE MAX_TOTAL_PERIODS //PERIOD_CEILING

// Type of data file.
#define FILE_TYPE_EGG 1 // EGG data file containing raw data from study.
#define FILE_TYPE_SS  2 // Spreadsheet data file.
#define FILE_TYPE_RAW_DATA 3 // ASCII file containing Raw data

/////////////////////////////////////////////////////////////////////////////
// CEGGSASDoc Document

class CEGGSASDoc : public CDocument
{
friend class CTenMinView;  
friend class CAnalyze; 
friend class CDbaccess;
friend class CDriver;

protected: // create from serialization only
	CEGGSASDoc();
	DECLARE_DYNCREATE(CEGGSASDoc)

//	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
//						  AFX_CMDHANDLERINFO* pHandlerInfo);

// Data
public:
  CTenMinView *m_p10MinView;
  PATIENT_DATA m_pdPatient;
  PATIENT_DATA m_pdSavedPatient;
  CArtifactID *m_pArtifact;
  CAnalyze *m_pAnalyze;
  CDriver *m_pDriver; // Driver for hardware.
  CDbaccess *m_pDB;
//  float m_fData[DATA_POINTS_PER_MIN_1 * 11 * 2 * 2]; // Raw data from file (holds a 20 minute period).
    // Raw data from file (holds a maximum period).
    // Data Points per minute * Max period in minutes * 2 data points:Resp,EGG
    //  Add some extra space just in case.
  float m_fData[(DATA_POINTS_PER_MIN_1 * POST_STIMULATION_MAX_TIME_MIN * 2) + 512];
  CFile *m_pcfDataFile; // Pointer to the opened data file.
  long m_lFileDataStart; // Start of data in data file.
  unsigned short m_uDataFileVer; // File version read from data file.
  unsigned short m_uCurrentPeriod; // Current period being shown.
  CString m_cstrErrMsg; // Used to hold error messages.

    // Data items used to enable/disable menu items.
  BOOL m_bHaveFile;  // TRUE if read in a patient data file.
  BOOL m_bHavePatient; // TRUE if a patient has been selected.
  unsigned short m_uMaxPeriods; // Maximum number of periods (timebands).
  bool m_bReportView; // TRUE if viewing Report
  bool m_bReportChanged; // true if the report has changed.
  unsigned m_uMenuType; // Menu type that's graphing the data.
  unsigned m_uReportItemToView; // Used by View menu to force the report
                                // class to display only the item selected
                                // in the View menu.
    // Research version
  UINT m_uViewMenuPostStimPeriodIDs[MENU_ID_ARRAY_SIZE];
  UINT m_uAnalyzeMenuPostStimPeriodIDs[MENU_ID_ARRAY_SIZE];
  unsigned short m_uNumPeriodMenuItems;
  float m_fStudyLen; // Number of minutes in the post-stimulation study.
  unsigned short m_uPreviousPeriod; // Previous period being shown.
  int m_iMinPerGraph; // Number of minutes per graph.
  CString m_cstrViewMenuItemPostWater10; // View post-water first 10 minute period text.
  CString m_cstrViewMenuItemPostWater20; // View post-water second 10 minute period text.
  CString m_cstrViewMenuItemPostWater30; // View post-water third 10 minute period text.
  CString m_cstrAnlMenuItemPostWater10; // Analyze post-water first 10 minute period text.
  CString m_cstrAnlMenuItemPostWater20; // Analyze post-water second 10 minute period text.
  CString m_cstrAnlMenuItemPostWater30; // Analyze post-water third 10 minute period text.
  CString m_cstrViewMenuItemAmtWater; // View amount of water text.
  CString m_cstrEditMenuItemAmtWater; // Edit amount of water text.
  CString m_cstrGoToMenuItemPrev; // Go to previous period text.
  CString m_cstrGoToMenuItemNext; // Go to next period text.
  CString m_cstrFileResumeStudy; // File resume study text.
  CString m_cstrEditStimMedium; // Edit stimulation medium text.
  CString m_cstrViewStimMedium; // View stimulation medium text.
  CString m_cstrViewMinPerGrph; // View minutes per graph.
  CString m_cstrGoToMenuItemPrevStim; // Go to previous stimulation period text.
  CString m_cstrGoToMenuItemNextStim; // Go to next stimulation period text.
  bool m_bUsingWhatIf; // true if a What If Scenario is loaded, otherwise false.
  WHAT_IF m_wifOrigData; // Holds the original What If data for the last recall.
  WHAT_IF m_wifOrigStudyData; // Holds the original study data that was overwritten
                              // by the What If data.
  short int m_iStudyType; // Type of study being done (new, existing, resume).
  bool m_bAllowStudies; // true if studies are allowed.
  // Total number of data points in the current period.  Includes both
  // EGG points and Respiration points.
  unsigned m_uDataPointsInPeriod;

  // Set to true to force the datasheet to be recalculated.
  bool m_bReAnalyze;

// Attributes
public:

// Operations
public:
  void vInit_patient_data();
  short int iRead_period(short unsigned uPeriod, float *pfData);
  short int iOpen_file_get_hdr(CFile **ppcfFile, CString &cstrFile,
                               DATA_FILE_HEADER *pdhfHdr, bool bDisplayErr);
  bool bValid_file_hdr(DATA_FILE_HEADER *pdhfHdr);
  bool bGet_data_graph(bool bAnalyze, unsigned uMenuType);
  void vGraph_data(bool bAnalyze, unsigned uMenuType);
  CString cstrGet_date_of_study();
  short int iInit_data_file();
  short int iWrite_data_file(const void *pData, unsigned uSize);
  void vCopy_patient(PATIENT_DATA *ppdDest, PATIENT_DATA *ppdSrc);
  short int iCompare_patient(PATIENT_DATA *ppd1, PATIENT_DATA *ppd2);
  int iCheck_to_Save();
  void vSet_strip_chart_attrib(HWND hPE, STRIP_CHART_ATTRIB *pscaGraph);
  void vDraw_raw_data_graph(HWND hPEGrph, GRAPH_INFO *pgi);
  void vSet_add_periods(int iStudyType);
  float fGet_period_start_min(unsigned short uPeriod);
  void vInit_artifact(void);
  void vSet_research_title();
  void vSet_title(CString cstrName);
  void vChange_db(int iDB, LPCSTR strDB);
  void vImport_reader_db();
  CString cstrSelect_reader_db(LPCTSTR pszFileName = "");
  int iImport_db(CDbaccess *pDBLocal, CDbaccess *pDBRemote);

  short int iSelectPatient(short int iStudyType, long lStudyID = INV_LONG, BOOL bHiddenPatients = FALSE);
  void vAnalyzeBaseline();
  void vAnalyzeCurrent();
  void vAnalyzeDiagnosis();
  void vAnalyzePostwater10();
  void vAnalyzePostwater20();
  void vAnalyzePostwater30();
  void vAnalyzePostwater40();
  void vEditAmtWater();
  void vEditPatientDemograhics();
  void vEditPatientDiagHist();
  void vEditPatientMeds();
  void vEditPreStudyInfo();
  void vEditSuggestedDiag();
  void vEditRecommendations();
  void vGotoEnd();
  void vGotoPrevPeriod();
  void vGotoStart();
  void vGotoNextPeriod();
  void vGotoCurrent();
  void vViewAmtWater();
  void vViewBaseline();
  void vViewPatientDemograhics();
  void vViewPatientDiagHist();
  void vViewPatientMeds();
  void vViewPostwater10();
  void vViewPostwater20();
  void vViewPostwater30();
  void vViewPostwater40();
  void vViewPreStudyInfo();
  void vViewSuggestedDiag();
  void vViewRecommendations();
  void vViewEvents();
  void vFileNewPatient(short int iStudyType);
  void vFileSave();
  void vReportFacInfo();
  void vReportConfigure();
  void vBackup();
  void vViewRespiration();
  void vEditRespiration();
  void vFileResumeStudy();
  BOOL bGoToCmdMsg(UINT nID, int nCode, void* pExtra,  AFX_CMDHANDLERINFO* pHandlerInfo);
  void vReportSpreadsheetOfDataSheet();
  void vWhatIf(long lWhatIfID);
  void vRemove_whatif_from_display();
  short int iCompare_whatif(PATIENT_DATA *ppd, WHAT_IF *pwif);
  void vFileSaveWhatIf();
  void vDisplay_what_if();

  bool bOk_to_close();
  void vGet_view();
  void vReset_periods_analyzed();
  bool bAre_standard_periods_analyzed();
  short int iResume_data_file();
  CString cstrCreate_data_file_name(PATIENT_DATA *ppdPatient, int iFileType);
  void vRemove_period_menu_item(unsigned short uPeriod);
  void vChangeMinPerGraph();
  bool bIs_study_analyzed();
  bool bIs_period_analyzed(unsigned uPeriod);
//  void vDelete_menu_items(ePgmType ptType);
  void vSetup_menus_fields();
//  void vAdd_menu_items(ePgmType ptType);
  void vInit_stim_period_menus();
  void vSetup_menus();
  void vFree_event_list(CEvntArray &Events);
  void vCleanup_events_db();
  void vSet_rsrch_pds_analyzed();
  void vInit_study_data_for_study_type();
  short iCheck_db();
  void vDisplay_version_error(CDbaccess *pDB);
  void vSave_ascii_data_file();
  void vClose_data_file(bool bCopyToDataFilePath);
  bool bCan_graph_summary_graphs();
  void vUpdate_display_for_RSA();
  void vSet_freq_range(int iFreqRange);
  bool bGet_study_type_freq_range();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEGGSASDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEGGSASDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEGGSASDoc)
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAs();
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CEGGSASDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
  afx_msg void OnSettings();
  //afx_msg void OnUpdateSettings(CCmdUI *pCmdUI);
  short iReOpen_data_file(void);
  afx_msg void OnSettingsFrequencyrange();
  afx_msg void OnViewFilterRsa();
  afx_msg void OnUpdateViewFilterRsa(CCmdUI *pCmdUI);
  afx_msg void OnSettingsFilterrsa();
  afx_msg void OnUpdateSettingsFilterrsa(CCmdUI *pCmdUI);
  afx_msg void OnViewViewhiddenpatientfiles();
  afx_msg void OnUpdateViewViewhiddenpatientfiles(CCmdUI *pCmdUI);
  afx_msg void OnUpdateSettingsFrequencyrange(CCmdUI *pCmdUI);
  void vShowHideStudyDates(void);
  void vCreate_hide_study_dates_GUID(void);
  afx_msg void OnReportShow33093();
  afx_msg void OnUpdateReportShow33093(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EGGSASDOC_H__DDF8835F_22E3_4CCE_BF00_9F8D086ED4AF__INCLUDED_)
