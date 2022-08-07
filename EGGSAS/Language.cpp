/********************************************************************
Language.cpp

Copyright (C) 2012,2013,2106 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CLanguage class.

  This class manages the language files and getting text from the language file.

  Language file format.
    - A language file is named for its culture.
      Ex: for US English, the name is en-US.ini
    - The language file is an INI file and is read/written as such.
    - The language file is divided into sections.
    - The first section is for the languages that can be used.
    - The second section is for the menus and toolbars.
    - The remaining sections (except the last) is for dialog boxes.
      Each dialog box has its own section.
    - The last section is for general purpose strings.

HISTORY:
23-FEB-12  RET  New
02-SEP-12  RET
             Add data sheet spreadsheet to waterload report menu.
               Change method: vLoad_menus()
05-OCT-12  RET
             Change for 60/200 CPM
               Add settings menu item
                 Change method: vLoad_menus()
13-DEC-12  RET
             Change to allow summary graphs to be drawn for research version.
               Change method: vLoad_menus()
14-DEC-12  RET
             Change to filter out frequencies below 10 cpm in the RSA.
               Change method: vLoad_menus(), pstrGet_tooltip()
25-FEB-13  RET
             Add getting the text for Viewing hidden patient files.
               Change method: vLoad_menus()
06-JUN-16  RET
             Add text to the language file for the Hide/show study dates dialog box.
               Change method: vLoad_menus()
********************************************************************/

#include "stdafx.h"
#include "stdio.h"
#include "eggsas.h"
#include "MainFrm.h"
#include "Dbaccess.h"
#include "SelectPatient.h"
#include "Language.h"

extern CMainFrame *g_pFrame;



/********************************************************************
Constructor

  Initialize class data.

********************************************************************/
CLanguage::CLanguage(void)
  {

  m_cstrINISection = "";
  // Get the language from EGGSAS.INI file.
  m_cstrCulture = g_pConfig->m_cstrCulture;
  }

/********************************************************************
Destructor

  Clean up class data.
    Empty the arrays containing the language list and culture list.

********************************************************************/
CLanguage::~CLanguage(void)
  {

  m_cstraLang.RemoveAll();
  m_cstraCultures.RemoveAll();
  }

/********************************************************************
vSet_lang_file - Set up the language file and the INI file for the program.

  inputs: None.
  
  Output: None.

  return: None.
********************************************************************/
void CLanguage::vSet_lang_file(void)
  {
  CString cstrLanguage;

  cstrLanguage = "Language\\" + m_cstrCulture + ".ini";
  theApp.vSet_ini_file((char *)(LPCTSTR)cstrLanguage);
  }

/********************************************************************
vStart_section - Set the section where language text will be used from.
                 To get text from this section, use the method: 
                   cstrGet_text()

  inputs: CString object containing the Section name.
  
  Output: None.

  return: None.
********************************************************************/
void CLanguage::vStart_section(CString cstrSection)
  {

  m_cstrINISection = cstrSection;
  vSet_lang_file();
  }

/********************************************************************
vEnd_section - End using the section setup by vStart_section().

  inputs: None.
  
  Output: None.

  return: None.
********************************************************************/
void CLanguage::vEnd_section()
  {

  //m_cstrINISection = "";
  theApp.vSet_ini_file("eggsas.ini");
  }

/********************************************************************
vLoad_menus - Set up the menus and toolbar with the selected language.

  inputs: ID of the menu to set with the language.
  
  Output: None.

  return: None.
********************************************************************/
void CLanguage::vLoad_menus(unsigned int uMenuID)
  {
  CString cstrTemp;

  vSet_lang_file();
  // Menu item common to both waterload and research versions.
  //// File menu
    // &File
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_FILE, "");
  g_pFrame->vChange_menu_item_by_pos(0, cstrTemp);
    // New study\tCtrl+N
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_NEW_STUDY, "");
  cstrTemp += "\t";
  cstrTemp += AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_CTRLN, "");
  g_pFrame->vChange_menu_item_by_pos_pos(0, 0, cstrTemp);
    // New patient
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_NEW_PATIENT, "");
  g_pFrame->vChange_menu_item_by_ID(ID_FILE_NEW_PATIENT, cstrTemp);
  // Existing patient
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_EXISTING_PATIENT, "");
  g_pFrame->vChange_menu_item_by_ID(ID_FILE_EXIST_PATIENT, cstrTemp);
  // &Open prior study...\tCtrl+O
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_OPEN_PRIOR, "");
  cstrTemp += "\t";
  cstrTemp += AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_CTRLO, "");
  g_pFrame->vChange_menu_item_by_ID(57601, cstrTemp);
  // Save patient\tCtrl+S
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_SAVE_PATIENT, "");
  cstrTemp += "\t";
  cstrTemp += AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_CTRLS, "");
  g_pFrame->vChange_menu_item_by_ID(57603, cstrTemp);
  // Save raw data
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_SAVE_RAW_DATA, "");
  g_pFrame->vChange_menu_item_by_ID(ID_FILE_SAVERAWDATAINASCII33072, cstrTemp);
    // &Backup or Restore
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_BACKUP, "");
  g_pFrame->vChange_menu_item_by_ID(ID_FILE_BACKUP, cstrTemp);
  // ExportStudies=Export Studies
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_EXPORT_STUDIES, "");
  g_pFrame->vChange_menu_item_by_ID(ID_FILE_EXPORT_STUDIES, cstrTemp);
  // ImportStudies=Import Studies
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_IMPORT_STUDIES, "");
  g_pFrame->vChange_menu_item_by_ID(ID_FILE_IMPORTSTUDIES, cstrTemp);
  // &print...\tCtrl+P
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_PRINT, "");
  cstrTemp += "\t";
  cstrTemp += AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_CTRLP, "");
  g_pFrame->vChange_menu_item_by_ID(57607, cstrTemp);
  // print Pre&view
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_PRINT_PREVIEW, "");
  g_pFrame->vChange_menu_item_by_ID(57609, cstrTemp);
  // p&rint Setup...
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_PRINT_SETUP, "");
  g_pFrame->vChange_menu_item_by_ID(57606, cstrTemp);
  // revert to &orginal
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_REVERT_TO_ORGINAL, "");
  g_pFrame->vChange_menu_item_by_ID(ID_FILE_REV_ORIG, cstrTemp);
  // recent File
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_RECENT_FILE, "");
  g_pFrame->vChange_menu_item_by_ID(57616, cstrTemp);
  // e&xit
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_EXIT, "");
  g_pFrame->vChange_menu_item_by_ID(57665, cstrTemp);
  //// End File menu

  //// Edit menu and View menu
  // &Edit
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_EDIT, "");
  g_pFrame->vChange_menu_item_by_pos(1, cstrTemp);
  // &Patient demographics
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_DEMOGRAPHICS, "");
  g_pFrame->vChange_menu_item_by_ID(ID_EDIT_PATIENT_DEMOGRAHICS, cstrTemp);
  g_pFrame->vChange_menu_item_by_ID(ID_VIEW_PATIENT_DEMOGRAHICS, cstrTemp);
  // Patient diagnostic &History
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_DIAG_HIST, "");
  g_pFrame->vChange_menu_item_by_ID(ID_EDIT_PATIENT_DIAG_HIST, cstrTemp);
  g_pFrame->vChange_menu_item_by_ID(ID_VIEW_PATIENT_DIAG_HIST, cstrTemp);
  // Patient &Medications
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_MEDICATIONS, "");
  g_pFrame->vChange_menu_item_by_ID(ID_EDIT_PATIENT_MEDS, cstrTemp);
  g_pFrame->vChange_menu_item_by_ID(ID_VIEW_PATIENT_MEDS, cstrTemp);
  // Pre-Study &Information
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_PRESTUDY_INFO, "");
  g_pFrame->vChange_menu_item_by_ID(ID_EDIT_PRE_STUDY_INFO, cstrTemp);
  g_pFrame->vChange_menu_item_by_ID(ID_VIEW_PRE_STUDY_INFO, cstrTemp);
  // Respiration
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_RESPIRATION, "");
  g_pFrame->vChange_menu_item_by_ID(ID_EDIT_RESPIRATION, cstrTemp);
  g_pFrame->vChange_menu_item_by_ID(ID_VIEW_RESPIRATION, cstrTemp);
  // &Suggested diagnosis
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_SUGGESTED_DIAG, "");
  g_pFrame->vChange_menu_item_by_ID(ID_EDIT_SUGGESTED_DIAG, cstrTemp);
  g_pFrame->vChange_menu_item_by_ID(ID_VIEW_SUGGESTED_DIAG, cstrTemp);
  // &Recommendations
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_RECOMMENDATIONS, "");
  g_pFrame->vChange_menu_item_by_ID(ID_EDIT_RECOMMENDATIONS, cstrTemp);
  g_pFrame->vChange_menu_item_by_ID(ID_VIEW_RECOMMENDATIONS, cstrTemp);
  // View hidden patient files.
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_VIEW_HIDDEN_PATIENTS, "");
  g_pFrame->vChange_menu_item_by_ID(ID_VIEW_VIEWHIDDENPATIENTFILES, cstrTemp);

  //// End Edit and View menu

  //// View Menu Only
  // &View
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_VIEW, "");
  g_pFrame->vChange_menu_item_by_pos(2, cstrTemp);
  // &Baseline period
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_BASELINE, "");
  g_pFrame->vChange_menu_item_by_ID(ID_VIEW_BASELINE, cstrTemp);
  g_pFrame->vChange_menu_item_by_ID(ID_ANALYZE_BASELINE, cstrTemp);
  // RSA
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_RSA, "");
  g_pFrame->vChange_menu_item_by_ID(ID_VIEW_RSA, cstrTemp);
  g_pFrame->vChange_menu_item_by_ID(ID_ANALYZE_RSA, cstrTemp);
  // Summary graphs
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_SUMMARY_GRAPHS, "");
  g_pFrame->vChange_menu_item_by_ID(ID_VIEW_SUMM_GRAPH, cstrTemp);
  // Data Sheet
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_DATA_SHEET, "");
  g_pFrame->vChange_menu_item_by_ID(ID_VIEW_DATA_SHEET, cstrTemp);
  //// End View Menu Only

  //// Goto Menu
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_GOTO, "");
  g_pFrame->vChange_menu_item_by_pos(3, cstrTemp);
  // &Start of study
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_START_OF_STUDY, "");
  g_pFrame->vChange_menu_item_by_ID(ID_GOTO_START, cstrTemp);
  // &Current period
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_CURRENT_PD, "");
  g_pFrame->vChange_menu_item_by_ID(ID_GOTO_CURRENT_PERIOD, cstrTemp);
  g_pFrame->vChange_menu_item_by_ID(ID_ANALYZE_CURRENT, cstrTemp);
  // &End of study
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_END_OF_STUDY, "");
  g_pFrame->vChange_menu_item_by_ID(ID_GOTO_END, cstrTemp);
  //// End Goto Menu

  //// Analyze menu
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_ANALYZE, "");
  g_pFrame->vChange_menu_item_by_pos(4, cstrTemp);
  // &Diagnosis
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_ANALYZE_DIAGNOSIS, "");
  g_pFrame->vChange_menu_item_by_ID(ID_ANALYZE_DIAGNOSIS, cstrTemp);
  //// Items are the same text as view menu and the goto menu.
  //// End Analyze menu

  //// Report menu
  // &Report
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_REPORT, "");
  g_pFrame->vChange_menu_item_by_pos(5, cstrTemp);
  // &View report
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_VIEW_REPORT, "");
  g_pFrame->vChange_menu_item_by_ID(ID_REPORT_VIEW, cstrTemp);
  // &Print Report
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_PRINT_REPORT, "");
  g_pFrame->vChange_menu_item_by_ID(ID_REPORT_PRINT, cstrTemp);
  // &Configure Report
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_CONFIG_REPORT, "");
  g_pFrame->vChange_menu_item_by_ID(ID_REPORT_CONFIGURE1, cstrTemp);
  // &Facility/practice information
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_FACILITY_INFO, "");
  g_pFrame->vChange_menu_item_by_ID(ID_REPORT_FAC_INFO, cstrTemp);
  // create SpreadSheet of Data Sheet
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_CREATE_SPREAD_SHEET_OF_DATA_SHEET, "");
  g_pFrame->vChange_menu_item_by_ID(32999, cstrTemp);


  // create Show/Hide study dates
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_SHOW_HIDE_STUDY_DATES, "");
  g_pFrame->vChange_menu_item_by_ID(ID_REPORT_SHOW33091, cstrTemp);

  //// End Report menu

  //// Help menu
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_HELP, "");
  g_pFrame->vChange_menu_item_by_pos(6, cstrTemp);
  // Help Topics
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_HELP_TOPICS, "");
  g_pFrame->vChange_menu_item_by_ID(57667, cstrTemp);
  // &About 3CPM EGGSAS...
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_ABOUT_EGGSAS, "");
  g_pFrame->vChange_menu_item_by_ID(57664, cstrTemp);
  // About 3CPM EGGSAS Reader...
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_ABOUT_READER, "");
  g_pFrame->vChange_menu_item_by_ID(ID_HELP_ABOUT_READER, cstrTemp);
  // About 3CPM EGGSAS Central Data Depot Reader
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_ABOUT_CENTRAL_DATA_­DEPOT, "");
  g_pFrame->vChange_menu_item_by_ID(ID_HELP_ABOUT3CPMEGGSASCENTRALDATADEPOTREADER, cstrTemp);
  //// End Help menu

  //// Settings menu
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_SETTINGS, "");
  g_pFrame->vChange_menu_item_by_pos(7, cstrTemp);
  // Settings
  //cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_SETTINGS, "");
  g_pFrame->vChange_menu_item_by_ID(ID_SETTINGS, cstrTemp);
  // Frequency range
  cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_FREQUENCY_RANGE, "");
  g_pFrame->vChange_menu_item_by_ID(ID_SETTINGS_FREQUENCYRANGE, cstrTemp);
  // Filter RSA
  cstrTemp = AfxGetApp()->GetProfileString(SEC_FILTERRSA, ITEM_FRSA_TITLE, "");
  g_pFrame->vChange_menu_item_by_ID(ID_SETTINGS_FILTERRSA, cstrTemp);
  //// End Settings menu

  if(uMenuID == IDR_MAINFRAME)
    { // Water load version or study.
    //// File menu
    //// End File menu
   
    //// Edit menu and View menu
    // &Amount of water ingested
    cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_AMOUNT_WATER, "");
    g_pFrame->vChange_menu_item_by_ID(ID_EDIT_AMT_WATER, cstrTemp);
    g_pFrame->vChange_menu_item_by_ID(ID_VIEW_AMT_WATER, cstrTemp);
    //// End Edit and View menu

    //// View Menu Only
    // &First post-water 10 minute period
    cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_FIRST_PERIOD, "");
    g_pFrame->vChange_menu_item_by_ID(ID_VIEW_POSTWATER_10, cstrTemp);
    g_pFrame->vChange_menu_item_by_ID(ID_ANALYZE_POSTWATER_10, cstrTemp);
    // &Second post-water 10 minute period
    cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_SECOND_PERIOD, "");
    g_pFrame->vChange_menu_item_by_ID(ID_VIEW_POSTWATER_20, cstrTemp);
    g_pFrame->vChange_menu_item_by_ID(ID_ANALYZE_POSTWATER_20, cstrTemp);
    // &Third post-water 10 minute period
    cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_THIRD_PERIOD, "");
    g_pFrame->vChange_menu_item_by_ID(ID_VIEW_POSTWATER_30, cstrTemp);
    g_pFrame->vChange_menu_item_by_ID(ID_ANALYZE_POSTWATER_30, cstrTemp);
    //// Summary graphs
    //cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_SUMMARY_GRAPHS, "");
    //g_pFrame->vChange_menu_item_by_ID(ID_VIEW_SUMM_GRAPH, cstrTemp);
    //// End View Menu Only

    //// Goto Menu
    // &Previous 10 minute period
    cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_PREV_10_MIN_PD, "");
    g_pFrame->vChange_menu_item_by_ID(ID_GOTO_PREV_PERIOD, cstrTemp);
    // &Next 10 minute period
    cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_NEXT_10_MIN_PD, "");
    g_pFrame->vChange_menu_item_by_ID(ID_GOTO_NEXT_PERIOD, cstrTemp);
    //// End Goto Menu

    //// Analyze menu
    //// End Analyze menu

    //// Report menu
    //// End Report menu

    //// Help menu
    //// End Help menu

    //// Settings menu
    //// End Settings menu

    }
  if(uMenuID == IDR_MAINFRAME1)
    { // Research version or study.
    //// File menu
    // Resume study
    cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_RESUME_STUDY, "");
    g_pFrame->vChange_menu_item_by_ID(32995, cstrTemp);
    //// End File menu

    //// Edit menu and View menu
    // stimulation medium and quantity
    cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_STIM_MEDIUM_QTY, "");
    g_pFrame->vChange_menu_item_by_ID(32996, cstrTemp); // edit menu
    g_pFrame->vChange_menu_item_by_ID(32997, cstrTemp); // view menu
    // what If ...
    cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_WHAT_IF, "");
    g_pFrame->vChange_menu_item_by_ID(33019, cstrTemp); // edit menu
    //// End Edit and View menu

    //// View Menu Only
    // Events
    cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_EVENTS, "");
    g_pFrame->vChange_menu_item_by_ID(33014, cstrTemp);
    // change number of minutes per graph
    cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_CHG_MIN_PER_GRPH, "");
    g_pFrame->vChange_menu_item_by_ID(32959, cstrTemp);
    //// End View Menu Only

    //// Goto Menu
    // previous period
    cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_PREV_PERIOD, "");
    g_pFrame->vChange_menu_item_by_ID(32954, cstrTemp);
    // next period
    cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_NXT_PERIOD, "");
    g_pFrame->vChange_menu_item_by_ID(32956, cstrTemp);
    //// End Goto Menu

    //// Analyze menu
    //// End Analyze menu

    //// Report menu
    // create SpreadSheet of Data Sheet
    cstrTemp = AfxGetApp()->GetProfileString(SEC_MENU, ITEM_M_CREATE_SPREAD_SHEET_OF_DATA_SHEET, "");
    g_pFrame->vChange_menu_item_by_ID(32999, cstrTemp);
    //// End Report menu

    //// Help menu
    //// End Help menu

    //// Settings menu
    //// End Settings menu

    }

  //// Read in the tool tip text and store.
  // The tool tip text is assigned dynamically at run time.
  // New study
  cstrNewStudy = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_NEW_STUDY, "");
  // Open prior study
  cstrOpenPriorStudy = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_OPEN_PRIOR_STUDY, "");
  // Save patient
  cstrSavePatient = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_SAVE_PATIENT, "");
  // Print report
  cstrPrintReport = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_PRINT_REPORT, "");
  // Start of study
  cstrStartOfStudy = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_START_OF_STUDY, "");
  // Previous 10 minute period
  cstrPrevious10MinutePeriod = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_PREV_10_MIN_PD, "");
  // Previous period
  cstrPreviousPeriod = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_PREV_PD, "");
  // Current period
  cstrCurrentPeriod = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_CURRENT_PERIOD, "");
  // Next 10 minute period
  cstrNext10MinutePeriod = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_NEXT_10_MIN_PD, "");
  // Next period
  cstrNextPeriod = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_NEXT_PD, "");
  // End of study
  cstrEndOfStudy = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_END_OF_STUDY, "");
  // RSA
  cstrRSA = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_RSA, "");
  // Summary graphs
  cstrSummaryGraphs = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_SUMMARY_GRAPHS, "");
  // Data Sheet
  cstrDataSheet = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_DATA_SHEET, "");
  // Diagnosis
  cstrDiagnosis = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_DIAGNOSIS, "");
  // Recommendations
  cstrRecommendations = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_RECOMMENDATIONS, "");
  // What If Scenario
  cstrWhatIfScenario = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_WHAT_IF_SCENARIO, "");
  // About
  cstrAbout = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_ABOUT, "");
  // Help Topics
  cstrHelpTopics = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_HELP_TOPICS, "");
  // Filter RSA
  cstrFilterRSA = AfxGetApp()->GetProfileString(SEC_TOOLTIPS, ITEM_TT_FILTERRSA, "");

  theApp.vSet_ini_file("eggsas.ini");
  }

/********************************************************************
pstrGet_tooltip - Get the tool tip for the specified toolbar ID.

  inputs: Toolbar ID.
  
  Output: None.

  return: LPSTR string containing the tooltip text.
********************************************************************/
LPSTR CLanguage::pstrGet_tooltip(unsigned int uID)
  {
  LPSTR pstrText;

  switch(uID)
    {
    case ID_FILE_NEW:
      pstrText = (LPSTR)(LPCTSTR)cstrNewStudy;
      break;
    case ID_FILE_OPEN:
      pstrText = (LPSTR)(LPCTSTR)cstrOpenPriorStudy;
      break;
    case ID_FILE_SAVE:
      pstrText = (LPSTR)(LPCTSTR)cstrSavePatient;
      break;
    case ID_REPORT_PRINT:
      pstrText = (LPSTR)(LPCTSTR)cstrPrintReport;
      break;
    case ID_GOTO_START:
      pstrText = (LPSTR)(LPCTSTR)cstrStartOfStudy;
      break;
    case ID_GOTO_PREV_PERIOD:
      pstrText = (LPSTR)(LPCTSTR)cstrPrevious10MinutePeriod;
      break;
    case ID_GOTO_PREVIOUS_STIM_PERIOD:
      pstrText = (LPSTR)(LPCTSTR)cstrPreviousPeriod;
      break;
    case ID_GOTO_CURRENT_PERIOD:
      pstrText = (LPSTR)(LPCTSTR)cstrCurrentPeriod;
      break;
    case ID_GOTO_NEXT_PERIOD:
      pstrText = (LPSTR)(LPCTSTR)cstrNext10MinutePeriod;
      break;
    case ID_GOTO_NEXT_STIM_PERIOD:
      pstrText = (LPSTR)(LPCTSTR)cstrNextPeriod;
      break;
    case ID_GOTO_END:
      pstrText = (LPSTR)(LPCTSTR)cstrEndOfStudy;
      break;
    case ID_ANALYZE_RSA:
      pstrText = (LPSTR)(LPCTSTR)cstrRSA;
      break;
    case ID_VIEW_SUMM_GRAPH:
      pstrText = (LPSTR)(LPCTSTR)cstrSummaryGraphs;
      break;
    case ID_VIEW_DATA_SHEET:
      pstrText = (LPSTR)(LPCTSTR)cstrDataSheet;
      break;
    case ID_ANALYZE_DIAGNOSIS:
      pstrText = (LPSTR)(LPCTSTR)cstrDiagnosis;
      break;
    case ID_EDIT_RECOMMENDATIONS:
      pstrText = (LPSTR)(LPCTSTR)cstrRecommendations;
      break;
    case ID_EDIT_WHAT_IF:
      pstrText = (LPSTR)(LPCTSTR)cstrWhatIfScenario;
      break;
    case ID_APP_ABOUT:
      pstrText = (LPSTR)(LPCTSTR)cstrAbout;
      break;
    case ID_HELP_FINDER:
      pstrText = (LPSTR)(LPCTSTR)cstrHelpTopics;
      break;
    case ID_VIEW_FILTER_RSA:
      pstrText = (LPSTR)(LPCTSTR)cstrFilterRSA;
      break;
    default:
      pstrText = "";
    }
  return(pstrText);
  }

/********************************************************************
cstrGet_text - Get the text for the specified ID and the section set
               by vStart_section().  This is used when filling in text
               for dialog boxes.

  inputs: CString object containing the text ID.
  
  Output: None.

  return: CString object containing the text for the ID.
********************************************************************/
CString CLanguage::cstrGet_text(CString cstrID)
  {
  CString cstrItem;

  vSet_lang_file();
  cstrItem = AfxGetApp()->GetProfileString(m_cstrINISection, cstrID, "");
  vEnd_section();
  return(cstrItem);
  //return(AfxGetApp()->GetProfileString(m_cstrINISection, cstrID, ""));
  }

/********************************************************************
cstrLoad_string - Get the text for the specified ID from the general
                  language section of the language file.

  inputs: CString object containing the text ID.
  
  Output: None.

  return: CString object containing the text for the ID.
********************************************************************/
CString CLanguage::cstrLoad_string(CString cstrID)
  {
  CString cstrItem;

  vSet_lang_file();
  cstrItem = AfxGetApp()->GetProfileString(SEC_GENERAL, cstrID, "");
  vEnd_section();
  return(cstrItem);

  //vSet_lang_file();
  //return(AfxGetApp()->GetProfileString(SEC_GENERAL, cstrID, ""));
  }

/********************************************************************
cstrLoad_string_fmt - Format a string using a string from the INI file
                      containing format specifiers.

  inputs: CString object containing the text ID with the formatting specifiers.
          List of arguments used by the format specifiers.
  
  Output: None.

  return: CString object containing the formatted text.
********************************************************************/
CString CLanguage::cstrLoad_string_fmt(CString cstrID, ...)
  {
  va_list marker;
  CString cstrResult, cstrFmt;

  vSet_lang_file();
  cstrFmt = AfxGetApp()->GetProfileString(m_cstrINISection, cstrID, "");
  vEnd_section();
  //cstrFmt = AfxGetApp()->GetProfileString(SEC_GENERAL, cstrID, "");
  if(cstrFmt.IsEmpty() == false)
    {
    va_start(marker, cstrID);  // Initialize variable arguments.
    cstrResult.FormatV(cstrFmt, marker);
    va_end(marker);
    }
  else
    cstrResult = "";

  return(cstrResult);
  }
  
/********************************************************************
vGet_languages - Get all the languages from the language INI file.

  inputs: None.
  
  Output: None.

  return: None.
********************************************************************/
void CLanguage::vGet_languages(void)
  {
  CString cstrFileName, cstrLang, cstrCulture;
  // 1024, each key value is 6 characters including the NULL.
  // This is enough space for 170 languages.
  char szLanguages[1024], szLang[128];
  int iIndex;

  if(m_cstraLang.GetCount() != 0)
    m_cstraLang.RemoveAll();
  if(m_cstraCultures.GetCount() != 0)
    m_cstraCultures.RemoveAll();

  cstrFileName = theApp.m_cstrProgramPath + "\\Language\\" + m_cstrCulture + ".ini";
  //cstrLanguages = AfxGetApp()->GetProfileString(SEC_LANGUAGE, NULL, "");
  // Get the language keys.
  GetPrivateProfileString(SEC_LANGUAGE, NULL, "", szLanguages, 1024, cstrFileName);
  // Array contains each language key in the section.  Each key is terminated with a NULL
  // and the last key is terminated with 2 NULLs.
  // Now for each key, get the language.  Also keep this is an array.
  cstrCulture = "";
  if(szLanguages[0] != NULL)
    {
    iIndex = 0;
    while(true)
      {
      if(szLanguages[iIndex] != NULL)
        cstrCulture += szLanguages[iIndex];
      else
        {
        m_cstraCultures.Add(cstrCulture);
        GetPrivateProfileString(SEC_LANGUAGE, cstrCulture, "", szLang, 128, cstrFileName);
        //cstrLang = AfxGetApp()->GetProfileString(SEC_LANGUAGE, cstrCulture, "");
        m_cstraLang.Add(szLang);
        if(szLanguages[iIndex + 1] == NULL)
          break; // done the list.
        cstrCulture = "";
        }
      ++iIndex;
      }
    }
  m_iLangIndex = 0;
  }

/********************************************************************
cstrGet_first_language - Get the first language from the list of
                         languages in the language file.

  inputs: None.
  
  Output: None.

  return: CString object containing the language.
********************************************************************/
CString CLanguage::cstrGet_first_language(void)
  {
  CString cstrLanguage;

  m_iLangIndex = 0;
  if(m_cstraLang.GetCount() == 0)
    cstrLanguage = "";
  else
    {
    cstrLanguage = m_cstraLang.GetAt(0);
    ++m_iLangIndex; // Set index for the next language.
    }
  return(cstrLanguage);
  }

/********************************************************************
cstrGet_next_language - Get the next language from the list of
                         languages in the language file.

  inputs: None.
  
  Output: None.

  return: CString object containing the language.
********************************************************************/
CString CLanguage::cstrGet_next_language(void)
  {
  CString cstrLanguage;

  if(m_iLangIndex >= m_cstraLang.GetCount())
    cstrLanguage = ""; // No more languages.
  else
    {
    cstrLanguage = m_cstraLang.GetAt(m_iLangIndex);
    ++m_iLangIndex; // Set index for the next language.
    }
  return(cstrLanguage);
  }

/********************************************************************
vSet_language - Set the language to use in the program.

  inputs: CString object containing the language.
  
  Output: None.

  return: None.
********************************************************************/
void CLanguage::vSet_language(CString cstrLanguage)
  {

  for(int i = 0; i < m_cstraLang.GetCount(); ++i)
    {
    if(m_cstraLang.GetAt(i) == cstrLanguage)
      { // Get the culture so we can create the language file.
      m_cstrCulture = m_cstraCultures.GetAt(i);
      // Write to the EGGSAS.INI file.
      g_pConfig->vWrite_item(CFG_CULTURE, m_cstrCulture);
      break;
      }
    }
  }

/********************************************************************
cstrGet_language - Get the language being used.

  inputs: None.
  
  Output: None.

  return: CString object containing the language.
********************************************************************/
CString CLanguage::cstrGet_language(void)
  {
  CString cstrLanguage;

  for(int i = 0; i < m_cstraCultures.GetCount(); ++i)
    {
    if(m_cstraCultures.GetAt(i) == m_cstrCulture)
      {
      cstrLanguage = m_cstraLang.GetAt(i);
      break;
      }
    }
  return(cstrLanguage);
  }
