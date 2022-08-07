/********************************************************************
EGGSAS.cpp

Copyright (C) 2003 - 2020 3CPM Company, Inc.  All rights reserved.

  Defines the class behaviors for the application.


HISTORY:
03-OCT-03  RET  New.
16-APR-05  RET  Add OnFilePrintSetup() handler in order to clear the logos
                  from the screen.
                Change OnAppAbout() to clear the logos from the screen.

VERSION HISTORY:
18-FEB-04  1.00.h  Prototype for EZEM.
09-MAR-04  1.00.j  Prototype for EZEM.
                   Add artificially generated test data.
17-MAR-04  1.00.k  Add help system.
21-APR-04  1.00.l  Use the low control range for 3cpm frequency range
                     to determine if signal is above or below normal
                     (rather than high control range).
18-APR-05  1.00.n  Fix EGG driver problem reported by EZEM by calling
                     the Pause() function rather than the Close() function.
                   Clear logos from opening screen when user first selects
                     an action.
25-AUG-07  RET  Version 2.02
                  Add second message box to pausing a study to tell user
                    how to resume study.
                  Change the color of the Event annotation in the graph.
                  Add field the 10 minute view windoe for the study type.
29-JUN-08 RET  Version 2.05
                 Fix problem with pausing and then resuming a study.  The
                   hardware was being reinitialized properly.
                 Put all stimulation medium data in report.  In left side
                   of first page and at bottom of data sheet.
                 Reset all periods and good minutes if creating a new WhatIF.
                 Remove from 10 minute view screen, delete and insert period
                   buttons and add reset periods button.
                 Add WhatIf scenarios to the select patient dialog box.
                 Change File menu "Select Patient" to "Open prior study".
                 Fix problem with stimulation medium dialog box where units and
                   corresponding checkboxes were not being handled properly.
27-JUL-08  RET  Version 2.05a
                  Fix bug that caused program to crash. User had a report displayed
                    and selected a WhatIf to display.
                  Fix bug where WhatIf description and fields are not displayed
                    when going back to 10 Min View after a report.
29-JUL-08  RET  Version 2.05b
                  Fix bug that caused and blank page to be inserted after the
                    raw data signal and before the events for the report.
28-AUG-08  RET  Version 2.05c
                  Add Research help file and add link to WhatIf dialog box.
                  There are 2 help files for the program, 1 for the standard
                    version and one for the research version.  If the program
                    is only one of the types, the help files are put in the
                    program directory by the setup program.  If the program can
                    be of either type, then the help files must be copied from
                    the proper subdirectory based on the study type.
                  Add method: vSetup_help_files().
15-SEP-08  RET  Version 2.06
                  Release of version 2.05c.
14-OCT-08  RET  Version 2.07a
                  Add Dog frequencies for analysis.
19-OCT-08  RET  Version 2.07b
                  Add item to INI file to specify if dog studies are allowed.
05-N0V-08  RET  Version 2.08a
                  Create Reader program.
                  Add File Menu item to start the Study Export program.
                  Change CAboutDlg::OnInitDialog() to use the correct program
                    name when accessing the version information.
30-NOV-08  RET  Version R.2.08b
                  Reader program
                    Add Help button to Import dialog box.
                  Research program
                    Change the path where the StudyExport program is
                      started from to Program\StudyExport.
                  Change connect function in db access class to make sure
                    the database file exists before connecting  to it.
14-JAN-09  RET  Version CDDRdr.2.08c
                  Create Limited Reader program called:
                    Central Data Depot Reader
                  This Program can only modify What IF scenarios.
                  Change usage of EGGSAS_READER.
23-JAN-09  RET  Version 2.08f
                  Automatically save the study after it is completed.
                    Change PostWaterTest.cpp.
                  Set up the help file path for the Reader and
                    Central Data Depot Reader programs.
                      Change InitInstance().
03-FEB-09  RET  Version 2.08g
                  Fixed problem in backup/restore that always showed an error
                    message when trying to do a restore.
                  Put backup/restore into CDDReader version.
12-FEB-09  RET  Version 2.08h
                  Put a list of studies to be imported in the Import dialog box
                    so user can select only some or all studies to import.
                  Add import the regular EGGSAS program.
13-FEB-09  RET  Version 2.08i
                  Don't allow studies if the database version is not what the
                    program is expecting.
                  Change the Import dialog box
                    - To display no database when initially started. And the file
                      open dialog box starts from "C:".
                    - Display error message if import database version is different
                      than the database version the program is expecting.
                    - Fix bug in the Import dialog box concerning properly deleting
                      the remote database object.
22-MAR-09  RET  Version 2.08k
                  Modify backup and restore to check and validate data files.
25-MAY-09  RET  Version 2.08l
                  Fix problem with water load version where RSA is not enabled
                    after a study is completed until the study is reloaded.
15-JUL-09  RET  Version 2.08m
                  Fix problem with putting a string into the database that contains
                    an apostrophe.
26-OCT-10 RET
                  Change the About dialog box to add the expiration date.
                    Add global variable g_cstrExpirationDate.
01-FEB-11  RET
                Change Import Studied dialog box to disable all buttons
                  allowing for selecting and importing studies for the Reader Demo.
                    ImportRemoteDB.cpp
                Add the displaying of an opening screen for the Demo
                  Reader program.
                    EGGSASDoc.cpp
                Add string ID (IDS_DEMO_OPEN_SCREEN_NOTICE) to the resource
                  file to hold the Demo opening screen text.
22-FEB-11  RET
                Add new hardware driver that keeps a device ID that must match
                  with the Device ID from the software.
                    Driver.cpp, Driver.h, util.cpp, util.h, EGGSAS.h
23-SEP-11  RET
             Begin changes to write an ASCII raw data file and not allow any analysis.
               Add global variable g_bAsciiDataFile
               Change classes:
                 EGGSASDoc.cpp, EGGSAS.h, EGGSASDoc.h, MainFrm.cpp, ConfigReport.cpp,
                 TenMinView.cpp, ReportView.cpp, util.cpp
               Add class: CRawDataDump: RawDataDump.cpp, RawDataDump.h
21-FEB-12  RET  Version 
             Changes for foreign languages.
               Add class: CConfig: Config.cpp, Config.h
               Add class: CLanguage: Language.cpp, Language.h
28-FEB-12  RET  Version 
             Add settings menu for languages and data storage.
             Settings dialog box allows for setting the language and the
               data storage path.
                 The data storage path is used only for storing data files.
                 All analyses are done used the data file stored on the 
                 host machine in the normal EGGSAS data directory.
                   Add class and dialog box: CSettings: Settings.cpp, Settings.h
22-AUG-12  RET
24-AUG-12
             Add checkboxes for each field in the demographic and facility
             diaog boxes.  If the checkbox is checked, that item will be included
             in the report.
              Change classes: Config.cpp/Config.h, Demographics.cpp/Demographics.h,
                FacilityInfo.cpp/FacilityInfo.h, Language.cpp/Language.h
              Change other files: en-US.ini
             Modify the report configuration dialog box.
              Change classes: Config.cpp/Config.h, ConfigReport.cpp/ConfigReport.h,
                Language.cpp/Language.h
              Change other files: eb-US.ini
             Change printing facility information to print only the items
               that were checked in the facility information dialog box.
                 ReportView.cpp
             Change printing left side of page 1 of report to print only the
               items that were checked in the demographic screen.
                 util.cpp, ReportView.cpp
             Add feature to report to print a logo at top of report.
               ReportView.cpp, FacilityInfo.cpp/FacilityInfo.h, Language.h
             Add title to the report after the facility information.
               ReportView.cpp, Language.h
             Make sure the Water load and units are printed for a
               water load study.
                 ReportView.cpp, Language.h, MainFrm.cpp, 
                 WaterLoad.cpp/WaterLoad.h, EGGSASDoc.cpp
25-AUG-12  RET
             Add back in Summary Table to always be printed at the bottom
             of the Summary Graphs.
               ReportView.cpp
27-AUG-12  RET
             Change viewing/printing the report so that RSA and Summary Graphs can
               either be printed on the first page or a subsequent page.
                 ReportView.cpp
             Implement new database table DiagnosisDesc
               Dbaccess.cpp/Dbaccess.h, Diagnosis.cpp/Diagnosis.h
31-AUG-12  RET
             Implement new database table for ICDCodes.
               Dbaccess.cpp/Dbaccess.h, Diagnosis.cpp/Diagnosis.h, Language.h
               Change other files: en-US.ini
02-SEP-12  RET
             Implement new database table Recommendations
               Dbaccess.cpp/Dbaccess.h, Recommendations.cpp/Recommendations.h,
                 Language.h
               Change other files: en-US.ini
             Add data sheet spreadsheet to waterload report menu.
               Language.cpp
13-SEP-12  RET
             Changes to indications.
               PreStudyInfo.cpp/PreStudyInfo.h, Dbaccess.cpp/Dbaccess.h,
               Language.h, ReportView.cpp, EGGSAS.h
               Change other files: en-US.ini

18-SEP-12  RET
             Change displaying/printing core report to include/exclude the
               selected raw signal, all the raw signal and the data sheet.
                 ReportView.cpp
             Change the left side of the first page of the report to:
               ReportView.cpp, util.cpp
             Change to select the select raw signal if the core report
               is selected.
                 ConfigReport.cpp
20-SEP-12  RET
             Redo how saving diagnosis descriptions, ICD codes and
             recommendations work based on bug found during test.
               Diagnosis.cpp, Recommendations.cpp
             For listing diagnosis key phrases, ICD codes and recommendation
             key phrases, instead of and empty line, use "Add new key phrase"
             and "Add new ICD code".
               Diagnosis.cpp, Recommendations.cpp, Language.h, en-US.ini
22-SEP-12  RET
03-OCT-12  RET
             Change for 60/200 CPM
               Change InitInstance() to initialize m_iFreqLimit and m_iMaxFreqBands.
                 analyze.cpp/analyze.h, Language.h, en-US.ini,
                 ReportView.cpp/ReportView.h, Driver.cpp, GraphRSA.cpp,
                 RSAView.cpp
               Add method: vSet_freq_range()
13-DEC-12  RET
             Change to allow summary graphs to be drawn for research version.
               EGGSASDoc.cpp, MainFrm.cpp, Language.cpp/Language.h, en-US.ini
14-DEC-12  RET
             Change to filter out frequencies below 10 cpm in the RSA.
               Add global variable: g_bFilterRSA
               Change files: ConfigReport.cpp/ConfigReport.h, Config.cpp/Config.h,
                 GraphRSA.cpp, EGGSASDoc.cpp/EGGSASDoc.h, ReportView.cpp/ReportView.h,
                 Language.cpp/Language.h, en-US.ini
               Add dialog box and class CFitlerRSA
                 FitlerRSA.cpp/FitlerRSA.h
13-JAN-13  RET
             Create 60/200 Research/waterload version
             If user selects new waterload or waterload patient:
               RSA filtering is turned off
               Frequency is set to 15 cpm.
             If user selects new research or research patient:
               RSA filtering is restored to its previous state
               Frequency doesn't change.
                 MainFrm.cpp, EGGSASDoc.cpp/EGGSASDoc.h
15-JAN-13  RET
             Add help button to the select frequency range dialog box.
               FreqRange.cpp/FreqRange.h
25-JAN-13  RET
             Add running the orphan data file recovery program at the start
             of this program.
               EGGSASDoc.cpp
             Version 2.09c
20-FEB-13  RET
             Change in Ascii data file:
               Remove text between sections and replace with blank lines.
                 RawDataDump.cpp
25-FEB-13  RET
             1)	In the Demographics section:
                 a.	After the “Date of Birth” section, add “Male” with check box
                    and “Female” with check box
                   i.	Allow only one to be checked
                   ii.	Include the checkbox after M/F selection to include in report
                 b.	Change title of “Social Security Number” to 
                   i.	“Patient ID or Social Security Number”
                 c.	The text  “Hospital Number” appears on the report if anything
                    is entered in the “Hospital” box.
                   i.	Remove “number” from appearing on the report
                 d.	Create a check box selection to allow patient file to be hidden
                   i.	If selected this will remove from the visible patients in the database
                       and place in a “hidden” section.
                   ii.	In the drop down menu under “View”
                     1.	Add command to “View hidden patient files”
                       a.	This will allow hidden files in the database to be seen and selected
                           for either opening to view the study only
                           or return it to the visible patient database.

               Demographics.cpp/Demographics.h, Config.h, Dbaccess.cpp/Dbaccess.h,
               Language.cpp/Language.h, en-US.ini, EGGSAS.h, EGGSASDoc.cpp
27-FEB-13  RET
             Changes for hiding patients.
               Add global variable: g_bHiddenPatients
                 SelectPatient.cpp/SelectPatient.h, EGGSASDoc.cpp/EGGSASDoc.h,
                 MainFrm.cpp
01-MAR-13  RET
             Add Male/female designation to report.
               ReportView.cpp
13-MAR-13  RET
             Changes for frequency range seleclion.
               - Don't ask for frequency when the program starts.
               - When starting a new study, instead of the frequency selection
                 dialog box use a new dialog box to as for 15cpm standard,
                 15cpm research, 60cpm research, 200cpm research.
               - Allow a frequency range change for an existing study only on
                 a WhatIF.
               - Frequency a study was recorded at needs to be remembered in the database.
               Add Frequency field to the Study table and the WhatIF table.
                 Dbaccess.cpp/Dbaccess.h, EGGSASDoc.cpp, EGGSAS.h, WhatIf.cpp
               Remove asking for the frequency range selection on program startup.
               Add displaying the frequency range to the study type.
                 TenMinView.cpp
               Add dialog box and class SelectStudyType.cpp/SelectStudyType.h
                 Change: Language.h, en-US.ini
16-MAR-13  RET
             Changes for frequency range seleclion.
              Add Set frequency toolbar button.
              Test and fix problems.
                MainFrm.cpp, EGGSASDoc.cpp/EGGSASDoc.h, WhatIf.cpp, TenMinView.cpp
19-MAR-13  RET
             Changes for frequency range seleclion.
               analyze.cppm, ConfigReport.cpp
02-JUN-16  RET
             Add feature to hide study dates.
               Change resource menu: IDR_MAINFRAME1
               Add resource dialog box: IDD_HIDE_STUDY_DATE
               Add files: HideStudyDates.cpp, HideStudyDates.h
               Modify files:
                 SelectPatient.cpp, ImportRemoteDB.cpp, ReportView.cpp, GraphRSA.cpp,
                 Config.cpp, config.h, util.cpp, util.h, Dbaccess.cpp,
                 Dbaccess.h, EGGSAS.h, EGGSASDoc.cpp, MainFrm.cpp, MainFrm.h,
                 IDR_MAINFRAME1
04-JUN-16  RET
             Add feature to hide study dates.
               Modify files:
                 util.cpp, util.h
06-JUN-16  RET
             Add text to the language file for the Hide/show study dates dialog box.
               Language.cpp, Language.h, en-US.ini
09-JUN-16  RET
             Add feature to hide study dates.
               TenMinView.cpp, WhatIf.cpp, SpreadSheet.cpp, Medications.cpp,
               DiagnosticHistory.cpp
28-JUL-16  RET
             Add feature to hide study dates.
               Hide study dates menu item was being displayed for research studies
               but not waterload studies.  This bug was introduced due to initially
               using research studies instead of waterload studies.
                 EGGSASDoc.cpp, IDR_MAINFRAME
               If user wants to toggle between showing and hiding study dates, the
               screen wasn't being updated and the user would have to reload the study.
               Now the screens are updated right away.
                 HideStudyDates.cpp
20-JAN-18  RET
             Provide search criteria for listing patients.
               Modify files: Dbaccess.cpp, Dbaccess.h, SelectPatient.cpp,
                 Language.h, en-US.ini
               Add form: IDD_SEARCH
               Add files: SearchDB.cpp, SearchDB.h
28-MAY-18  RET
             Add ability to rename an imported patient that already exists in the database.
               Add form: IDD_IMPORT_DUP_NAME
               Add files: ImportDupName.cpp, ImportDupName.h
               Change files: ImportRemoteDB.cpp, ImportRemoteDB.h en-US.ini,
                 Language.h
             Version: 2.09f-1.2
09-FEB-19  RET  Version 2.09f-1.3
             Add ability to update an imported database if it is of an older version.
               Change files: ImportRemoteDB.cpp
               Add classes: CDBUpdate (DBUpdate.cpp, DBUpdate.h)
                            CDBAccessUpd (DBAccessUpd.cpp, DBAccessUpd.h)
16-MAY-20  RET  Version 2.09f-1.4
             When selecting an existing patient to start a new study, program
             gets an error.  This was because it was looking for a date from
             the select patient list and there is no date because study dates
             are hidden.
               Change files: CSelectPatient.cpp
5-JUL-20  RET
            Correct problem with not showing study dates in any version.
               Change files: EGGSAS.h
            Change version to 2.09g.
6-JUL-20  RET
            Change definition of HELP_FILE_HLP to ".chm" from ".hlp".
            ".hlp" files are no longer supported in Win10.
               Change files: EGGSAS.h
            Change calls to WinHelp() to HtmlHelp().
              Change files: Baseline.cpp, BckupRest.cpp, ConfigReport.cpp, Demographics.cpp,
                Diagnosis.cpp, DiagnosticHistory.cpp, EquipTest.cpp, FacilityInfo.cpp,
                FreqRange.cpp, ImportRemoteDB.cpp, Medications.cpp, PostWaterTest.cpp,
                PreStudyInfo.cpp, Recommendations.cpp, RespEdit.cpp, RespEditResearch.cpp,
                SearchDB.cpp, SelectPatient.cpp, Settings.cpp, StimulationMedium.cpp,
                WaterLoad.cpp, WhatIf.cpp
08-JUL-20  RET
             Change starting the StudyExport program to add the current
             path as a command line parameter when it is started.  The StudyExport
             program was also changed so that it gets the correct path to its
             help file so it can be started from either EGGSAS or standalone.
               Change file: MainFrm.cpp
12-SEP-20  RET Version 2.09h
             CDDR version.  When a WhatIf study is in the study list and dates are
             hidden, any study selected after the WhatIf doesn't get displayed.  This was
             because a separate internal list of study dates is kept (so that the study
             dates are known regardless of whether or not the dates are displayed) and the
             WhatIf study date was not included in the date list.  When dates are hidden
             then the study does not have the correct date and so doesn't get displayed.
               Change files: CSelectPatient.cpp
16-OCT-20  RET Version 2.09i
             Make selecting an alternate database for the research and water load
             versions work.
               Change files: CSelectPatient.cpp, EGGSASDoc.cpp
14-NOV-20  RET
             Add ability to work with a remote database.
             Changed settings to allow the user to specify the remote database.
               Change files: Settings.cpp, Settings.h, Config.cpp, Config.h
                 Language.h, SelectPatient.cpp, EGGSASDoc.cpp, ImportRemoteDB.cpp,
                 en-US.ini
16-NOV-20  RET
             Add new version CDDRE
               Change files: EGGSASDoc.cpp, ImportRemoteDB.cpp, ReportView.cpp,
                 SelectPatient.cpp
******************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"

#include "MainFrm.h"
#include "TenMinView.h"
#include "EGGSASDoc.h"
#include "EGGSASView.h"
#include ".\eggsas.h"
#include "analyze.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Control data
// This is used for the Summary table.
//  First dimension: Baseline(high,low), min10(high,low), 
//                   min20(high,low), min30(high,low)
//  Second dimension: Frequency band 1 through 4
//  A value of -1 indicates there is no control value.  So control
//    values will neither be displayed nor graphed.
// These values should go into a file.
float g_fControl[CONTROL_MIN_HI_LO][CONTROL_FREQ_BANDS] =
  {
  {25.0, 47.0, 27.0, 23.0}, // Bradygastria (frequency band 1) low
  {61.0, 77.0, 65.0, 45.0}, //    high

  {18.0, 10.0, 23.0, 36.0}, // Normal (frequency band 2) low
  {54.0, 36.0, 57.0, 68.0}, //    high

  {7.0, 8.0, 7.0, 7.0},     // Tachygastria (frequency band 3) low
  {25.0, 18.0, 17.0, 17.0}, //    high

  {2.0, 0.0, 1.0, 0.0},     // Duodenal (frequency band 4) low
  {10.0, 6.0, 5.0, 8.0},    //    high
  };

CString g_cstrExpirationDate = "";

// True if the program should create an ASCII data file and prohibit any
// analysis.  This value is read from the pgmkey value in the ini file.
bool g_bAsciiDataFile = false;

/////////////////////////////////////////////////////////////////////////////
// CEGGSASApp

BEGIN_MESSAGE_MAP(CEGGSASApp, CWinApp)
	//{{AFX_MSG_MAP(CEGGSASApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
//	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
//	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEGGSASApp construction

CEGGSASApp::CEGGSASApp()
  {
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEGGSASApp object

CEGGSASApp theApp;

CMainFrame *g_pFrame = NULL;

CLanguage *g_pLang = NULL;

CConfig *g_pConfig = NULL;

bool g_bFilterRSA = false;

// TRUE if a hidden patients has been selected for viewing.
// FALSE if an active patient has been selected for viewing.
BOOL g_bHiddenPatients = FALSE;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {A2CB1B37-6578-41D8-B259-54D4050D5CE0}
static const CLSID clsid =
{ 0xa2cb1b37, 0x6578, 0x41d8, { 0xb2, 0x59, 0x54, 0xd4, 0x5, 0xd, 0x5c, 0xe0 } };

/////////////////////////////////////////////////////////////////////////////
// CEGGSASApp initialization

BOOL CEGGSASApp::InitInstance()
{
	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

/********************* Not needed for Visual Studio .NET
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
**********************/

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
//	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    // Use an INI file rather than the Registry. Set up the INI file name
    // and path.  It uses the current directory.
    // The INI file must be in the same directory as the executable program.
  m_szIniFile[0] = NULL;
  char szIniFile[100];
  GetCurrentDirectory(100, szIniFile); // Path program is in.

  //CString cstrMsg;
  //cstrMsg.Format("Current directory: %s", szIniFile);
  //AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg);

  m_cstrIniFilePath = szIniFile;
  vSet_ini_file("eggsas.ini");
  //strcat(szIniFile, "\\");
  //strcat(szIniFile, "eggsas.ini");
  //free((void *)m_pszProfileName);
  //m_pszProfileName = _strdup(szIniFile);

    //m_cstrDataFilePath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_DATAPATH, "");
    //m_cstrProgramPath = m_cstrDataFilePath + "\\" + "Raw Data Dump";
    //m_cstrHelpFile = "3CPM EGGSAS Raw Data Dump";
    //CString cstrHelpFile;
    //cstrHelpFile = m_cstrProgramPath + "\\ResearchHelp\\" + m_cstrHelpFile + HELP_FILE_HLP;

    //free((void*)m_pszHelpFilePath);
    //m_pszHelpFilePath=_tcsdup(_T(cstrHelpFile));


	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CEGGSASDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
    RUNTIME_CLASS(CTenMinView));
//		RUNTIME_CLASS(CEGGSASView));
	AddDocTemplate(pDocTemplate);

	// Connect the COleTemplateServer to the document template.
	//  The COleTemplateServer creates new documents on behalf
	//  of requesting OLE containers by using information
	//  specified in the document template.
	m_server.ConnectTemplate(clsid, pDocTemplate, TRUE);
		// Note: SDI applications register server objects only if /Embedding
		//   or /Automation is present on the command line.

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

  g_pConfig = new CConfig();
  g_pConfig->vRead();
  g_bFilterRSA = g_pConfig->m_bFilterRSA;

  // Get the program type from the INI file to see if its the Raw Data Dump program.
  CString cstrPgmKey;
  bool bRDD;
  //cstrPgmKey = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_PGMKEY, "");
  cstrPgmKey = g_pConfig->cstrGet_item(CFG_PGMKEY);
  if(cstrPgmKey.GetAt(15) == '9' && cstrPgmKey.GetAt(52) == 'B'
  && cstrPgmKey.GetAt(69) == '4')
    bRDD = true;
  else
    bRDD = false;
    // Read the data file path from the INI file and save the path and if it
    // doesn't exist, create it.
  //m_cstrDataFilePath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_DATAPATH, "");
  m_cstrDataFilePath = g_pConfig->cstrGet_item(CFG_DATAPATH);
#if EGGSAS_READER == READER_NONE
  if(bRDD == false)
    {
    m_cstrProgramPath = m_cstrDataFilePath + "\\" + PROGRAM_PATH;
    m_cstrHelpFile = HELP_FILE;
    }
  else
    {
    m_cstrProgramPath = m_cstrDataFilePath + "\\" + "Raw Data Dump";
    m_cstrHelpFile = "3CPM EGGSAS"; // Raw Data Dump";
    //CString cstrHelpFile;
    //cstrHelpFile = m_cstrProgramPath + "\\ResearchHelp\\" + m_cstrHelpFile + HELP_FILE_HLP;
    }

#elif EGGSAS_READER == READER
  m_cstrProgramPath = m_cstrDataFilePath + "\\" + READER_PATH;
  m_cstrHelpFile = HELP_FILE; //READER_HELP_FILE;
#elif EGGSAS_READER == READER_DEPOT
  m_cstrProgramPath = m_cstrDataFilePath + "\\" + CENTRAL_DATA_DEPOT_READER_PATH;
  m_cstrHelpFile = HELP_FILE; //CENTRAL_DATA_DEPOT_READER_HELP_FILE;
#elif EGGSAS_READER == READER_DEPOT_E
  m_cstrProgramPath = m_cstrDataFilePath + "\\" + CENTRAL_DATA_DEPOT_READER_PATH;
  m_cstrHelpFile = HELP_FILE; //CENTRAL_DATA_DEPOT_READER_HELP_FILE;
#endif
  m_cstrStandardHelpFilePath = m_cstrProgramPath + "\\" + STANDARD_HELP_PATH;
  m_cstrResearchHelpFilePath = m_cstrProgramPath + "\\" + RESEARCH_HELP_PATH;
  if(CreateDirectory(m_cstrDataFilePath, NULL) == 0)
    {
    if(GetLastError() != ERROR_ALREADY_EXISTS)
      {
      AfxMessageBox("Can't create data directory, using current directory");
      m_cstrDataFilePath.Empty();
      }
    }
    CString cstrHelpFile = m_cstrProgramPath + "\\" + m_cstrHelpFile + HELP_FILE_HLP; //"C:\\3CPM\\Program\\3CPM EGGSAS.chm";
    free((void*)m_pszHelpFilePath);
    m_pszHelpFilePath = _tcsdup(_T(cstrHelpFile));
    //theApp.m_cstrHelpFile = "C:\\3CPM\\Program\\3CPM EGGSAS.chm";
    //theApp.m_pszHelpFilePath = "C:\\3CPM\\Program\\3CPM EGGSAS.chm";
    theApp.m_eHelpType = afxHTMLHelp;

	//CWinApp* pApp = AfxGetApp();
 // pApp->m_f

  g_pLang = new CLanguage();

  //m_bFirstTimeInTenMinView = true;
  vSet_freq_range(FREQ_LIMIT_15_CPM); //FREQ_LIMIT_200_CPM; //FREQ_LIMIT_60_CPM; //FREQ_LIMIT_15_CPM;
//  m_iFreqLimit = FREQ_LIMIT_15_CPM; //FREQ_LIMIT_200_CPM; //FREQ_LIMIT_60_CPM; //FREQ_LIMIT_15_CPM;
//#ifdef _200_CPM_HARDWARE
//  if(theApp.m_iFreqLimit == FREQ_LIMIT_15_CPM)
//    m_iMaxFreqBands = MAX_FREQ_BANDS_15_CPM;
//  else if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
//    m_iMaxFreqBands = MAX_FREQ_BANDS_60_CPM;
//  else
//    m_iMaxFreqBands = MAX_FREQ_BANDS_200_CPM;
//#else
//    m_iMaxFreqBands = MAX_FREQ_BANDS_15_CPM;
//#endif

	// Check to see if launched as OLE server
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Register all OLE server (factories) as running.  This enables the
		//  OLE libraries to create objects from other applications.
		COleTemplateServer::RegisterAll();

		// Application was run with /Embedding or /Automation.  Don't show the
		//  main window in this case.
		return TRUE;
	}

	// When a server application is launched stand-alone, it is a good idea
	//  to update the system registry in case it has been damaged.
	m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
	COleObjectFactory::UpdateRegistryAll();

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_MAXIMIZE);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CEGGSASApp message handlers

void CEGGSASApp::vSet_ini_file(char *pszFile)
  {
  char szIniFile[100];

  if(strcmp(m_szIniFile, pszFile) != 0)
    {
    strcpy(m_szIniFile, pszFile);
    //GetCurrentDirectory(100, szIniFile); // Path program is in.
    strcpy(szIniFile, m_cstrIniFilePath);
    strcat(szIniFile, "\\");
    strcat(szIniFile, pszFile); //"eggsas.ini");
    free((void *)m_pszProfileName);
    m_pszProfileName = _strdup(szIniFile);
    }
  }

/********************************************************************
vLog - Record errors and other activity in the log file.

  inputs: Constant string object containing a message and any
            formatting codes.
          Any arguments the message string requires for formatting.
   
  return: None.
********************************************************************/
void CEGGSASApp::vLog(LPCTSTR pstrFmt, ...)
  {
  va_list marker;
  CString cstrLog, cstrDate;
  FILE *pfLog;
  SYSTEMTIME SysTime;

  pfLog = fopen("eggsaslog.txt", "a+");
  if(pfLog != NULL)
    {
    va_start(marker, pstrFmt);  // Initialize variable arguments.
    cstrLog.FormatV(pstrFmt, marker);
    va_end(marker);
      // Insert the current date and time into the message.
    GetLocalTime(&SysTime);
      // Day/Month/Year  Hours:minutes:seconds
    cstrDate.Format("%02d/%02d/%d  %02d:%02d:%02d  ", SysTime.wDay, SysTime.wMonth,
           SysTime.wYear, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
    cstrLog.Insert(0, cstrDate);
      // Write to the log file.
    fputs(cstrLog, pfLog);
    fputs("\n", pfLog);
    fclose(pfLog);
    }
  }

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Methods
  bool bGet_version_info(CString &cstrProgName, char *pszInfoType, 
                         CString &cstrInfo);

protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  CString m_cstrProgramName;
  CString m_cstrProgramVersion;
  CString m_cstrCopyRight;
  CString m_cstrCopyRight1;
  CString m_cstrExpire;
  afx_msg void OnUpdateViewViewhiddenpatientfiles(CCmdUI *pCmdUI);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
, m_cstrProgramName(_T(""))
, m_cstrProgramVersion(_T(""))
, m_cstrCopyRight(_T(""))
, m_cstrCopyRight1(_T(""))
, m_cstrExpire(_T(""))
  {
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
//{{AFX_DATA_MAP(CAboutDlg)
//}}AFX_DATA_MAP
DDX_Text(pDX, IDC_PROGRAM_NAME, m_cstrProgramName);
DDX_Text(pDX, IDC_VERSION, m_cstrProgramVersion);
DDX_Text(pDX, IDC_COPYRIGHT, m_cstrCopyRight);
DDX_Text(pDX, IDC_COPYRIGHT1, m_cstrCopyRight1);
DDX_Text(pDX, IDC_EXPIRE, m_cstrExpire);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(ID_VIEW_VIEWHIDDENPATIENTFILES, OnUpdateViewViewhiddenpatientfiles)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
  {
  CString cstrName;

  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
#if EGGSAS_READER > READER_NONE
  //// Reader
#ifdef _DEBUG
  cstrName = "3CPM EGGSAS.EXE";
#else
#if EGGSAS_READER == READER
  cstrName = "3CPM EGGSAS.EXE"; //"3CPM EGGSAS Reader.EXE";
#elif EGGSAS_READER == READER_DEPOT
  cstrName = "3CPM EGGSAS.EXE"; //"3CPM EGGSAS Central Data Depot Reader.EXE";
#elif EGGSAS_READER == READER_DEPOT_E
  cstrName = "3CPM EGGSAS.EXE"; //"3CPM EGGSAS Central Data Depot Reader.EXE";
#endif
#endif
  SetWindowText("About 3CPM EGGSAS Reader");
//#ifdef _DEBUG
//  cstrName = "3CPM EGGSAS.EXE";
//#else
//  cstrName = "3CPM EGGSAS Reader.EXE";
//#endif
//  SetWindowText("About 3CPM EGGSAS Reader");
#else
  //// Not reader
  // Get the program type from the INI file to see if its the Raw Data Dump program.
  //CString cstrPgmKey;
  //cstrPgmKey = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_PGMKEY, "");
  //if(cstrPgmKey.GetAt(15) == '9' && cstrPgmKey.GetAt(52) == 'B'
  //&& cstrPgmKey.GetAt(69) == '4')
  //  cstrName = "3CPM EGGSAS Raw Data Dump.EXE"; // Raw Data Dump program
  //else
    cstrName = "3CPM EGGSAS.EXE"; // Regular program
#endif
    //theApp.vLog("About: cstrName = %s", cstrName);
  bGet_version_info(cstrName, "FileDescription", m_cstrProgramName);
  bGet_version_info(cstrName, "FileVersion", m_cstrProgramVersion);
	bGet_version_info(cstrName, "LegalCopyright", m_cstrCopyRight);
  m_cstrCopyRight1.LoadString(IDS_ALL_RIGHTS_RESERVED);
  if(g_cstrExpirationDate.IsEmpty() == false)
    m_cstrExpire.Format(g_pLang->cstrLoad_string(ITEM_G_EXPIRE_DATE), g_cstrExpirationDate);
    //m_cstrExpire.Format(IDS_EXPIRE_DATE, g_cstrExpirationDate);

  UpdateData(FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
bGet_version_info - Get the product and program information from the resource
                    file.

    inputs: Reference to a CString object containing the program named and
              extension.
            Reference to a CString object specifying what data item to get
              from the resource file.
            Reference to a CString object which gets the resource file
              text string.

    return: TRUE if the information was retrieved from the resource file.
            FALSE if an error occurred.  If there is an error, the last
              input is empty.
********************************************************************/
bool CAboutDlg::bGet_version_info(CString &cstrProgName, char *pszInfoType, 
CString &cstrInfo)
  {
  CString cstrTranslation, cstrStringFileInfo, cstrFmt;
	unsigned long lTemp, lInfoSize;
  unsigned char *pucVersionInfo;
  void *pvBuffer;
  unsigned int uiLen;
  unsigned long ulLangCodePage;
  bool bRet = TRUE;

    //theApp.vLog("About: bGet_version_info() enter, cstrProgName = %s, pszInfoType = %s", cstrProgName, pszInfoType);
  pucVersionInfo = NULL;
  cstrTranslation = "\\VarFileInfo\\Translation";
  //if((lInfoSize = GetFileVersionInfoSize((char *)(LPCTSTR)cstrProgName, &lTemp)) != 0L
  //&& (pucVersionInfo = (unsigned char *)HeapAlloc(GetProcessHeap(), 
  //HEAP_ZERO_MEMORY, lInfoSize)) != NULL
  //&& GetFileVersionInfo((char *)(LPCTSTR)cstrProgName, 0L,  lInfoSize, pucVersionInfo)
  //!= 0
  //&& VerQueryValue(pucVersionInfo, (char *)(LPCTSTR)cstrTranslation, &pvBuffer, &uiLen)
  //!= 0)
  if((lInfoSize = GetFileVersionInfoSize((char *)(LPCTSTR)cstrProgName, &lTemp)) != 0L)
    {
    //theApp.vLog("About: GetFileVersionInfoSize() successful");
    if((pucVersionInfo = (unsigned char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, lInfoSize)) != NULL)
      {
      //theApp.vLog("About: HeapAlloc() successful");
      if( GetFileVersionInfo((char *)(LPCTSTR)cstrProgName, 0L,  lInfoSize, pucVersionInfo) != 0)
        {
        //theApp.vLog("About: GetFileVersionInfo() successful");
        if( VerQueryValue(pucVersionInfo, (char *)(LPCTSTR)cstrTranslation, &pvBuffer, &uiLen) != 0)
          {
          //theApp.vLog("About: VerQueryValue() successful");
          //theApp.vLog("About: query %s", pszInfoType);
          ulLangCodePage = *((unsigned long *)pvBuffer);
          // high word is code page, low word is language.
          cstrFmt = "\\StringFileInfo\\%04x%04x\\%s";
          cstrStringFileInfo.Format(cstrFmt, LOWORD(ulLangCodePage), 
            HIWORD(ulLangCodePage), pszInfoType);
          if(VerQueryValue(pucVersionInfo, (char *)(LPCTSTR)cstrStringFileInfo, &pvBuffer, 
            &uiLen) != 0)
            cstrInfo = (char *)pvBuffer;
          else
            bRet = FALSE;
          //theApp.vLog("About: query result %s", cstrInfo);
          }
        else
          bRet = FALSE;
        }
      else
        bRet = FALSE;
      }
    else
      bRet = FALSE;
    }
  else
    bRet = FALSE;
  if(bRet == FALSE)
    {
    DWORD dw = GetLastError(); 
    theApp.vLog("About: GetFileVersionInfoSize() failed: GetLastError = %u\n", dw); 
    cstrInfo.Empty();
    }
  if(pucVersionInfo != NULL)
    HeapFree(GetProcessHeap(), 0, (LPVOID)pucVersionInfo);
  return(bRet);
  }

// App command to run the dialog
void CEGGSASApp::OnAppAbout()
  {
  POSITION pos;
	CSingleDocTemplate *pDocTemplate;
  CEGGSASDoc *pDoc; // Pointer to the current document.

    // Get the current view so we can remove the logos.
  if((pos = GetFirstDocTemplatePosition()) != NULL)
    {
    pDocTemplate = (CSingleDocTemplate *)GetNextDocTemplate(pos);
    if((pos = pDocTemplate->GetFirstDocPosition()) != NULL)
      {
      pDoc = (CEGGSASDoc *)pDocTemplate->GetNextDoc(pos);
      pDoc->vGet_view();
      pDoc->m_p10MinView->vShow_logo(FALSE);
      }
    }

	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
  }

void CEGGSASApp::vOn_help(void)
  {

  OnHelp();
  }


/********************************************************************
OnFilePrintSetup - Handler for starting up the printer setup dialog box.

 - Clear the logos from the screen.
 - Start the print setup dialog box.
********************************************************************/
void CEGGSASApp::OnFilePrintSetup()
  {
  POSITION pos;
	CSingleDocTemplate *pDocTemplate;
  CEGGSASDoc *pDoc; // Pointer to the current document.

    // Get the current view so we can remove the logos.
  if((pos = GetFirstDocTemplatePosition()) != NULL)
    {
    pDocTemplate = (CSingleDocTemplate *)GetNextDocTemplate(pos);
    if((pos = pDocTemplate->GetFirstDocPosition()) != NULL)
      {
      pDoc = (CEGGSASDoc *)pDocTemplate->GetNextDoc(pos);
      pDoc->vGet_view();
      pDoc->m_p10MinView->vShow_logo(FALSE);
      }
    }

	CWinApp::OnFilePrintSetup();
  }

/********************************************************************
vSetup_help_files - Copy the appropriate help files from the subdirectory
                    to the program directory.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASApp::vSetup_help_files()
  {
  CString cstrHelpFilePath, cstrSrcFile, cstrDestFile;


  //CString cstrPgmKey;
  //cstrPgmKey = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_PGMKEY, "");
  //if(cstrPgmKey.GetAt(15) == '9' && cstrPgmKey.GetAt(52) == 'B'
  //&& cstrPgmKey.GetAt(69) == '4')
  //  {
  //  CString cstrHelpFile;
  //  cstrHelpFile = m_cstrProgramPath + "\\ResearchHelp\\" + m_cstrHelpFile + HELP_FILE_HLP;
  //  free((void*)m_pszHelpFilePath);
  //  m_pszHelpFilePath=_tcsdup(_T(cstrHelpFile));
  //  }
  //else
    {
  if(theApp.m_ptPgmType == PT_STD_RSCH)
    {
    if(theApp.m_ptStudyType == PT_RESEARCH)
      { // Research help file.
      cstrHelpFilePath = m_cstrResearchHelpFilePath;
      }
    else
      { // Standard help files.
      cstrHelpFilePath = m_cstrStandardHelpFilePath;
      }
      // Copy the help files to the program directory.
    cstrSrcFile = cstrHelpFilePath + "\\" + m_cstrHelpFile + HELP_FILE_HLP;
    cstrDestFile = m_cstrProgramPath + "\\" + m_cstrHelpFile + HELP_FILE_HLP;
    CopyFile(cstrSrcFile, cstrDestFile, FALSE);
    cstrSrcFile = cstrHelpFilePath + "\\" + m_cstrHelpFile + HELP_FILE_CNT;
    cstrDestFile = m_cstrProgramPath + "\\" + m_cstrHelpFile + HELP_FILE_CNT;
    CopyFile(cstrSrcFile, cstrDestFile, FALSE);
    }
    }
  }

/********************************************************************
vSet_freq_range - Set the frequency range.
                  If built for the 60/200 CPM hardware, user chooses.
                  Otherwise, frequency range is 15 CPM.

  inputs: Frequency range from the eFrequencyRange in EGGSAS.h.

  return: None.
********************************************************************/
void CEGGSASApp::vSet_freq_range(int iFreqRange)
  {

  m_iFreqLimit = iFreqRange;
#ifdef _200_CPM_HARDWARE
  if(m_iFreqLimit == FREQ_LIMIT_15_CPM)
    m_iMaxFreqBands = MAX_FREQ_BANDS_15_CPM;
  else if(m_iFreqLimit == FREQ_LIMIT_60_CPM)
    m_iMaxFreqBands = MAX_FREQ_BANDS_60_CPM;
  else
    m_iMaxFreqBands = MAX_FREQ_BANDS_200_CPM;
#else
  m_iFreqLimit = FREQ_LIMIT_15_CPM;
  m_iMaxFreqBands = MAX_FREQ_BANDS_15_CPM;
#endif

  POSITION pos;
	CSingleDocTemplate *pDocTemplate;
  CEGGSASDoc *pDoc; // Pointer to the current document.

    // Get the current view so we can remove the logos.
  if((pos = GetFirstDocTemplatePosition()) != NULL)
    {
    pDocTemplate = (CSingleDocTemplate *)GetNextDocTemplate(pos);
    if((pos = pDocTemplate->GetFirstDocPosition()) != NULL)
      {
      pDoc = (CEGGSASDoc *)pDocTemplate->GetNextDoc(pos);
      if(pDoc->m_pAnalyze != NULL)
        pDoc->m_pAnalyze->vSet_freq_range(m_iFreqLimit);
      }
    }

  }
void CAboutDlg::OnUpdateViewViewhiddenpatientfiles(CCmdUI *pCmdUI)
  {
  // TODO: Add your command update UI handler code here
  }
