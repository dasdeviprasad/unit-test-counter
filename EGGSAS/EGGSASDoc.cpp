/********************************************************************
EGGSASDoc.cpp

Copyright (C) 2003 - 2021 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CEGGSASDoc class.

  This class provides the main document services for the program.
    - Message handlers for the menu items.
    - Reading patient data files.


HISTORY:
03-OCT-03  RET  New.
23-FEB-04  RET  Change vEditPatientMeds(), vEditPatientDiagHist(),
                  vViewPatientDiagHist(), vViewPatientMeds():
                    to not select the patient and study if its the same one
                    that is currently selected.
02-MAR-04  RET  Change vDraw_raw_data_graph() to draw the respiration graph
                  in black (from light grey).
                Change vFileNewPatient() to call vInit_artifact().
03-MAR-04  RET  Change vGraph_data() so that artifact is not identified for
                  periods after the baseline if the baseline has not been
                  anaylzed.
06-FEB-05  RET  Changes to use the research reference number.
                  Changed methods:
                    vEditPatientDemograhics(), vFileNewPatient(),
                    iSelectPatient()
10-FEB-05  RET  Changes for middle initial and delay gastric emptying:
                  iCompare_patient(), vCopy_patient(), vInit_patient_data()
                  iInit_data_file()
16-APR-05  RET  Change the following methods to clear the logos from the
                  screen.                    vFileNewPatient(), iSelectPatient(), vReportFacInfo(),
                    vReportConfigure(), vBackup()
10-MAY-07  RET  Research Version
                  Change OnNewDocument():
                    To look for research version.
                    To modify menu items for research version.
                  Change vInit_patient_data() to initialize uNumPeriods.
                  Change vSet_add_periods() to add/remove the correct
                    number of periods from the menus.
                  Add OnCmdMsg() to process commands for the post-stimulation
                    period menu items.
                  Change constructor to initialize: m_uAnalyzeMenuPostStimPeriodIDs[],
                    m_uViewMenuPostStimPeriodIDs[], m_uNumPeriodMenuItems
                  Add methods: vReset_periods_analyzed(), vFileResumeStudy(),
                    iResume_data_file(), vCreate_data_file_name()
                  Change data file structure.
                    Change bValid_file_hdr() to be valid with research data file.
                  Add item to INI file, general section, for the number of
                    minutes per graph.
                  Add vSet_research_title() to put "Research version" in
                    the title bar.
25-AUG-07  RET  Version 2.02
                  Change vDraw_raw_data_graph() to change the color 
                    of the Event annotation in the graph.
05-FEB-08  RET  Version 2.03
                  Add SaveAs as a What If dialog box from the Edit menu and
                  from the toolbar.
                  Add methods:
                    vWhatIf(), vRemove_whatif_from_display(), iCompare_whatif(),
                    vFileSaveWhatIf(), vDisplay_what_if(), vSet_rsrch_pds_analyzed()
                  Add class varialbles:
                    m_bUsingWhatIf, m_wifOrigData, m_wifOrigStudyData
                  Modify the following methods:
                    vFileNewPatient(), vFileResumeStudy(), iSelectPatient(),
                    iCheck_to_Save(), OnFileSave()
                  Change vAnalyzeDiagnosis() to give the Diagnosis dialog box the
                    user diagnosis as in vEditSuggestedDiag().

                Change how program treats extending the study for an extra 10 minutes.
                  Instead of adding a 4th period and disallowing certain actions such
                  as suggested diagnosis, make the 3rd period 20 minutes long and
                  allow all actions.
                Change vSet_add_periods()
                 - Add a parameter to indicate if this is a new study or a
                   previous study.
                 - If the extra 10 minute flag is set
                    - Change the database fields so that the third period is 20 minutes.
                    - Don't set the Manual mode flag.  This will cause the program to
                      perform all actions as if there wasn't a 4th period.
                    - Don't enable any 4th period items.
                    - If this study has already been done (not a new study) and the
                      database hasn't been changed yet, save the database.  This will
                      correct studies done previous to this change transparent to the user.
                Change vGotoNextPeriod(), vGotoEnd() to not use a 4th period.
19-FEB-08  RET  Version 2.04
                  Removed "Revert to original" menu item from the file menu.
29-JUN-08  RET  Version 2.05
                  Add class variable m_iStudyType.
                    Change vFileResumeStudy(), vFileNewPatient() to set it.
                  Add WhatIf scenarios to the select patient dialog box.
                    Change: vWhatIf(), iSelectPatient()
                  Add method: bIs_period_analyzed().
27-JUL-08  RET  Version 2.05a
                  Change vWhatIf()to make sure the 10 Min View is the currently
                    active view before displaying the WhatIF the user selected.
28-AUG-08  RET  Version 2.05c
                  Change cstrCreate_data_file_name() to create a file name
                    using the research reference number if it is being used.
                  Change the following methods to make sure the proper help file
                    is loaded.
                      OnNewDocument(), vFileNewPatient(), iSelectPatient()
14-OCT-08  RET  Version 2.07a
                  Change vInit_patient_data() to initialize the patient type to Human.
19-OCT-08  RET  Version 2.07b
                  Change OnNewDocument() to read from INI file if dog studies
                    are allowed.
07-N0V-08  RET  Version 2.08a
                  Add 2 File menu items, Export Studies and Import Studies.
                  Changed vSetup_menus() to Delete the File menu items that
                    are not appropriate for EGGSAS and Reader.
                  Change iRead_period() to use the data file with the correct
                    path if user is reading a remote database.
14-JAN-09  RET  Version CDDRdr.2.08c
                  Change usage of EGGSAS_READER.
16-JAN-09  RET  Version CDDRdr.2.08e
                  Change iSelectPatient(), vAnalyzeBaseline(),
                    vAnalyzeCurrent(), vAnalyzePostwater10(),
                    vAnalyzePostwater20(), vAnalyzePostwater30(),
                    vAnalyzePostwater40() to always set m_uMenuType = SM_VIEW;
                    for the Central Data Depot Reader.
                  Fix problem in vGotoNextPeriod().  If user is on last period
                    and view RSA or DataSheet and then selected next period,
                    the screen would not show the last period again and would be
                    blank except for logo.
23-JAN-09  RET  Version 2.08f
                  Automatically save the study after it is completed.
                    Change vFileNewPatient(), vFileResumeStudy().
03-FEB-09  RET  Version 2.08g
                  Change vSetup_menus() to put Backup/Restore into CDDReader.
07-FEB-09  RET  Version RWL.2.08h
                  Add method vInit_study_data_for_study_type().
                  Change vFileNewPatient() to call new method for RWL programs.
                    If the user is creating a new study for an existing patient, the
                    study data would already have been set up for a research study
                    in iSelectPatient().  Therefore it has to corrected if the
                    user selected a WaterLoad study.
13-FEB-09  RET  Version 2.08i
                  Change cstrSelect_reader_db() to start the open file dialog box
                    from "C:" if the reader db path from the INI file is empty.
                  Add a check to OnNewDocument() to make sure the database is OK.
                    - Add methods vDisplay_version_error(), iCheck_db().
                    - If database is not OK, don't allow studies to be done.
16-FEB-09  RET  Version 2.08j
                  Change creation of data sheet report to properly calculate
                    period minutes.  A problem was found if an extra 10 minutes
                    of baseline is recorded.  The minutes are calculate without
                    taking into consideration the extra 10 minutes, so that the
                    minutes for the postwater study are off by 10 minutes.
25-MAY-09  RET  Version 2.08l
                  Fix problem with water load version where RSA is not enabled
                    after a study is completed until the study is reloaded.
                    Change bIs_period_analyzed() to enable RSA if any period
                      is analyzed.
26-OCT-10 RET
                 Add ability to set the number of days the program will run for.
                   Add item ("signalcorr") to the EGGDRIVER section of the INI file which
                     specifies the number of days that the program will run. It is
                     set to zero if there is no expiration.
                   The expiration date is encoded in the "pgmkey" entry in the general
                     section.  This is initially set to an invalid date.
                   When this item is non-zero and the expiration date in the "pgmkey"
                     entry is an invalid date, the expiration date is calculated from
                     the "signalcorr" (value - 20).  The expiration date is then calculated
                     and encoded into the "pgmkey" entry and written back to the INI file.
                   When there is an expiration date, a message is displayed when the
                     program starts which dispays the expiration date.
                   The expiration date is also displayed in the About dialog box.
24-NOV-10  RET
                Change title to display "Research - Waterload Version" for the RWL versions.
                  vSet_title()
01-FEB-11 RET
                Change OnNewDocument() to display an opening screen for the Demo
                  Reader program.
23-SEP-11  RET
             Begin changes to write an ASCII raw data file and not allow any analysis.
               Change methods: OnNewDocument(), cstrCreate_data_file_name(),
                               vSetup_menus(), bGet_data_graph(), bGoToCmdMsg(),
                               vDraw_raw_data_graph(), vFileResumeStudy(),
                               vFileNewPatient()
               Add methods: vClose_data_file(), vSave_ascii_data_file()
21-FEB-12  RET  Version 
                  Changes for foreign languages.
28-FEB-12  RET  Version 
             Add settings menu for languages and data storage.
               Add methods: iReOpen_data_file()
               Change methods: vClose_data_file()
24-AUG-12  RET
             Change vEditAmtWater() to record a change in the water load or units.
03-OCT-12  RET
             Change for 60/200 CPM
               Change vFileNewPatient() to display the select frequency range dialog
                 box before a new study.
               Add Settings menu item for selecting the frequency range.
                 Add menu item handler OnSettingsFrequencyrange().
13-DEC-12  RET
             Change to allow summary graphs to be drawn for research version.
               Add method: bCan_graph_summary_graphs()
14-DEC-12  RET
             Change to filter out frequencies below 10 cpm in the RSA.
               OnViewFilterRsa(), OnUpdateViewFilterRsa(), OnSettingsFilterrsa(),
               OnUpdateSettingsFilterrsa()
13-JAN-13  RET
             Create 60/200 Research/waterload version
               Change methods: vFileNewPatient(), iSelectPatient()
             If RSA or Report is displayed and user changes frequency, redisplay.
               Change methods: OnSettingsFrequencyrange(), OnViewFilterRsa(),
                 OnSettingsFilterrsa()
               Add methods: vUpdate_display_for_RSA()
25-JAN-13  RET
             Add running the orphan data file recovery program at the start
             of this program.
               Change methods: OnNewDocument()
25-FEB-13  RET
            Add checkboxes to Demographics for Male and Female and to hide a patient.
               Change methods: iCompare_patient(), vCopy_patient()
27-FEB-13  RET
             Changes for hiding patients.
               Change methods: iSelectPatient(), vFileNewPatient(), vDraw_raw_data_graph()
               Add methods: OnViewViewhiddenpatientfiles()
                 OnUpdateViewViewhiddenpatientfiles()
13-MAR-13  RET
             Changes for frequency range seleclion.
              Add Frequency to studies.
                Change methods: iSelectPatient()
                Add event handler: OnUpdateSettingsFrequencyrange()
16-MAR-13  RET
             Changes for frequency range selection.
               Change methods: iSelectPatient(), OnSettingsFrequencyrange(),
                 vSetup_menus()
               Add methods: vSet_freq_range(),bGet_study_type_freq_range()
17-MAR-13  RET
              Changes for frequency range selection.
               Change methods: vFileNewPatient(), iSelectPatient(), iCheck_to_Save(),
                 vCopy_patient(), iCompare_whatif(), vFileSaveWhatIf()
02-JUN-16  RET
             Add feature to hide study dates.
               Change methods: OnNewDocument(), cstrGet_date_of_study(),
                 vSetup_menus()
01-JUL-16  RET
             Found and fixed problem in select a reader database (also used by import).
             When the directory is changed the original directory was never put back
             and this resulted in the export program not starting after importing from
             a different directory.
              Change method: cstrSelect_reader_db()
28-JUL-16  RET
             Add feature to hide study dates.
               Add show/hide study dates menu item to the waterload report menu and
               add event handlers to handle the selecting of the menu item.
                 Add event handlers: OnReportShow33093(), OnUpdateReportShow33093()
16-OCT-20  RET Version 2.09i
             Make selecting an alternate database for the research and water load
             versions work.
               Change methods: iRead_period(), cstrCreate_data_file_name()
14-NOV-20  RET
             Add ability to work with a remote database.
             Changed settings to allow the user to specify the remote database.
               Change methods: OnNewDocument()
16-NOV-20  RET
             Add new version CDDRE
               Change methods: OnNewDocument(), cstrGet_date_of_study(), vSetup_menus(),
                 vClose_data_file
22-JAN-21  RET
             Don't initialize the frequency member of PATIENT_DATA data structure
             if running a 200cpm test.  It will already have been selected.
               Change method: vInit_patient_data()


Description of the ASCII raw data file.
•	First section is for the EGG signal
  ­	Line 1:  "BaseLine EGG"
  ­	Line 2 - N: 1 minute of comma delimited raw baseline EGG data per line.
  ­	Line N+1: "Study EGG"
  ­	Line N+2 - N + X: 1 minute of comma delimited raw EGG study data per line.
  ­	Blank line separating first and second sections.
•	Second section is for the Respiration signal
  ­	Line 1:  "BaseLine Respiration"
  ­	Line 2 - N: 1 minute of comma delimited raw baseline respiration data per line.
  ­	Line N+1: "Study Respiration"
  ­	Line N+2 - N + X: 1 minute of comma delimited raw respiration study data per line.
  •	All data is formatted as decimal numbers with 4 decimal places.
•	The name of the ASCII raw data file is the same as the existing data file but with “RawData” prepended to the filename (i.e. before the patient name) and a “.csv” extension.  The “RawData” is used to distinguish the raw data CSV file from the Data Sheet spreadsheet file.
  ­	It is created in the same folder as the existing data file.

********************************************************************/

#include "stdafx.h"
#include <process.h>
#include "EGGSAS.h"

#include "MainFrm.h"
#include "EGGSASDoc.h"
#include "analyze.h"
#include "Driver.h"
#include "Dbaccess.h"
#include "TenMinView.h"
#include "Demographics.h"
#include "PreStudyInfo.h"
#include "EquipTest.h"
#include "Baseline.h"
#include "WaterLoad.h"
#include "RespRate.h"
#include "PostWaterTest.h"
#include "SelectPatient.h"
#include "DiagnosticHistory.h"
#include "Medications.h"
#include "Diagnosis.h"
#include "FacilityInfo.h"
#include "Recommendations.h"
#include "ConfigReport.h"
#include "RespEdit.h"
#include "BckupRest.h"
#include "GraphRSA.h"
#include "ReportView.h"
#include "util.h"
#include "StimulationMedium.h"
#include "SelectStudyType.h"
#include "ChgMinPerGraph.h"
#include "RespEditResearch.h"
#include "spreadsheet.h"
#include "EventList.h"
#include "WhatIf.h"
#include "ImportRemoteDB.h"
#include "RawDataDump.h"
#include "FreqRange.h"
  // For graphing
#include "Pegrpapi.h"
#include "Pemessag.h"
#include ".\eggsasdoc.h"
//#include ".\eggsasdoc.h"
#include "Settings.h"
#include "FilterRSA.h"
#include "SelStudyTypeFreq.h"
#include "HideStudyDates.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame *g_pFrame;

/////////////////////////////////////////////////////////////////////////////
// CEGGSASDoc

IMPLEMENT_DYNCREATE(CEGGSASDoc, CDocument)

BEGIN_MESSAGE_MAP(CEGGSASDoc, CDocument)
	//{{AFX_MSG_MAP(CEGGSASDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	//}}AFX_MSG_MAP
  ON_COMMAND(ID_SETTINGS, OnSettings)
  //ON_UPDATE_COMMAND_UI(ID_SETTINGS, OnUpdateSettings)
  ON_COMMAND(ID_SETTINGS_FREQUENCYRANGE, OnSettingsFrequencyrange)
  ON_COMMAND(ID_VIEW_FILTER_RSA, OnViewFilterRsa)
  ON_UPDATE_COMMAND_UI(ID_VIEW_FILTER_RSA, OnUpdateViewFilterRsa)
  ON_COMMAND(ID_SETTINGS_FILTERRSA, OnSettingsFilterrsa)
  ON_UPDATE_COMMAND_UI(ID_SETTINGS_FILTERRSA, OnUpdateSettingsFilterrsa)
  ON_COMMAND(ID_VIEW_VIEWHIDDENPATIENTFILES, OnViewViewhiddenpatientfiles)
  ON_UPDATE_COMMAND_UI(ID_VIEW_VIEWHIDDENPATIENTFILES, OnUpdateViewViewhiddenpatientfiles)
  ON_UPDATE_COMMAND_UI(ID_SETTINGS_FREQUENCYRANGE, OnUpdateSettingsFrequencyrange)
  ON_COMMAND(ID_REPORT_SHOW33093, OnReportShow33093)
  ON_UPDATE_COMMAND_UI(ID_REPORT_SHOW33093, OnUpdateReportShow33093)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CEGGSASDoc, CDocument)
	//{{AFX_DISPATCH_MAP(CEGGSASDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//      DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IEGGSAS to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {7A84FAF3-FBD1-41B1-807B-58AD2F1CB769}
static const IID IID_IEGGSAS =
{ 0x7a84faf3, 0xfbd1, 0x41b1, { 0x80, 0x7b, 0x58, 0xad, 0x2f, 0x1c, 0xb7, 0x69 } };

BEGIN_INTERFACE_MAP(CEGGSASDoc, CDocument)
	INTERFACE_PART(CEGGSASDoc, IID_IEGGSAS, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEGGSASDoc construction/destruction

/********************************************************************
Constructor

  Initialize class data.
********************************************************************/
CEGGSASDoc::CEGGSASDoc()
  {

  // TODO: add one-time construction code here

	EnableAutomation();

    // Initialize the Menu IDs for the post-stimulation period menu items.
  vInit_stim_period_menus();

	AfxOleLockApp();
}

/********************************************************************
Destructor

  If there is an open patient data file, close it.
  Clean up the artifact lists and delete the object.
  Delete the Analyze object.
  delete the database object (closes the connection).
********************************************************************/
CEGGSASDoc::~CEGGSASDoc()
  {

  // Currently have an open data file.  Close it.
  vClose_data_file(false);

  // Make sure any artifact list is cleaned up.
  if(m_pArtifact != NULL)
    delete m_pArtifact;

  if(m_pAnalyze != NULL)
    delete m_pAnalyze;

  if(m_pDriver != NULL)
    delete m_pDriver;

  if(m_pDB != NULL)
    delete m_pDB;

  if(g_pLang != NULL)
    delete g_pLang;

  if(g_pConfig != NULL)
    delete g_pConfig;

    // Remove events.
  vFree_event_list(m_pdPatient.Events.m_PSEvents);
  vFree_event_list(m_pdPatient.Events.m_BLEvents);

  vFree_event_list(m_pdSavedPatient.Events.m_PSEvents);
  vFree_event_list(m_pdSavedPatient.Events.m_BLEvents);

	AfxOleUnlockApp();
  }

/********************************************************************
OnNewDocument

  Start a new document.
  Get a pointer to the view.
  Initialize document data.
********************************************************************/
BOOL CEGGSASDoc::OnNewDocument()
  {
  POSITION pos;
  CString cstrBaseText, cstrText;

	if (!CDocument::OnNewDocument())
		return FALSE;
	// TODO: add reinitialization code here
  m_p10MinView = NULL;
  m_bReAnalyze = false;
	// (SDI documents will reuse this document)

  //// pgmkey item in the INI file.
    // String length is 75 characters.
    //
    // Display a message at startup: (31) must be a B
    //    Message is from the String resource at value IDS_OPEN_SCREEN_NOTICE
    //            1         2         3         4         5         6         7
    //  012345678901234567890123456789012345678901234567890123456789012345678901234
    // "04E101D6972539-05673AC2376-0480B44E6A-0549A54B902-39098-B510B4D52070-2308ED"
    //
    // The numbers/letters at the following indexes must remain fixed.
    //     4=0, 6=D, 13=9, 27=0, 35=6, 43=5, 54=8, 66=7, 72=8
    //
    // EXPIRATION DATE
    // Expiration date positions: Format is mm/dd/yyyy
    // Slashes are not encoded in the string.
    //  month (19)(32)
    //  day (7)(64)
    //  year (22)(47)(71)(12)
    //  For fun, encoded month = month + 20
    //           encoded day = day + 30
    //  Current expiration date 01/10/2004 (21 40 2014)
    //            1         2         3         4         5         6         7
    //  012345678901234567890123456789012345678901234567890123456789012345678901234
    // "04E101D4972549-05672AC2376-0480B14E6A-0549A54B902-39098-B510B4D50070-2308ED"
    //
    // No expiration date requires both an invalid month and invalid day.
    //            1         2         3         4         5         6         7
    //  012345678901234567890123456789012345678901234567890123456789012345678901234
    // "04E101D6972539-05673AC2376-0480B44E6A-0549A54B902-39098-B510B4D52070-2308ED"
    //
    // -------------------------------------------------------------------------
    // Values that indicate which version is running.
    // ----------------------------------------
    // For research version
    //   Use 3 locations of specific values.
    //     Locations: (9) = 5
    //                (36) = E
    //                (58) = A
    //            1         2         3         4         5         6         7
    //  012345678901234567890123456789012345678901234567890123456789012345678901234
    //  04E101D6952539-05673AC2376-0480A44E6E-0549A54B902-39098-B5A0B4D52070-2308ED
    // ----------------------------------------
    // For Standard Version
    //   Use 3 locations of specific values.
    //     Locations: (5) = 7
    //                (24) = C
    //                (61) = 2
    //            1         2         3         4         5         6         7
    //  012345678901234567890123456789012345678901234567890123456789012345678901234
    // "04E107D6972539-05673AC23C6-0480A44E6A-0549A54B902-39098-B510B2D52070-2308ED"
    // ----------------------------------------
    // For ASCII Data file Version
    //   Use 3 locations of specific values.
    //     Locations: (15) = 9
    //                (52) = B
    //                (69) = 4
    //            1         2         3         4         5         6         7
    //  012345678901234567890123456789012345678901234567890123456789012345678901234
    // "04E101D6972539-95673AC2376-0480A44E6A-0549A54B902-39B98-B510B4D52070-4308ED"
    // For ASCII Data file Version and Research Version
    //            1         2         3         4         5         6         7
    //  012345678901234567890123456789012345678901234567890123456789012345678901234
    // "04E101D6952539-95673AC2376-0480A44E6E-0549A54B902-39B98-B5A0B4D52070-4308ED"
    // For ASCII Data file Version and Standard Version
    //            1         2         3         4         5         6         7
    //  012345678901234567890123456789012345678901234567890123456789012345678901234
    // "04E107D6972539-95673AC23C6-0480A44E6A-0549A54B902-39B98-B510B2D52070-4308ED"
    // For ASCII Data file Version and Both Research and Standard versions
    //            1         2         3         4         5         6         7
    //  012345678901234567890123456789012345678901234567890123456789012345678901234
    //  04E107D6952539-95673AC23C6-0480A44E6E-0549A54B902-39B98-B5A0B2D52070-4308ED

//    // "04E107D6972539-05673AC23C6-0480B44E6A-0549A54B902-39098-B510B2D52070-2308ED"
    //
    // ----------------------------------------
    // Both Research and Standard versions
    //            1         2         3         4         5         6         7
    //  012345678901234567890123456789012345678901234567890123456789012345678901234
    //  04E107D6952539-05673AC23C6-0480A44E6E-0549A54B902-39098-B5A0B2D52070-2308ED
    //
    // Use 2 locations for indicating that the Device ID has been generated.
    // These values are defined in driver.h
    //     Locations: (2) NO Device ID = E, Device ID = A
    //                (63) NO Device ID = 5, Device ID = 7
    // -------------------------------------------------------------------------
    //
    //
    // All used locations:
    //     2, 4, 5, 6, 7, 9 12, 13, 15, 19, 22, 24, 27, 31, 32, 35, 36, 43, 47, 52, 54, 58,
    //     61, 63, 64, 66, 69, 71, 72

    //            1         2         3         4         5         6         7
    //  012345678901234567890123456789012345678901234567890123456789012345678901234
    //  04E107D6972539-05673AC23C6-0480A44E6A-0549A54B902-39098-B510B2D52070-2308ED
    //    .                                                            .
    //  04E107D6952539-05673AC23C6-0480A44E6E-0549A54B902-39098-B5A0B2D52070-2308ED ; Both

  CString cstrPgmKey, cstrTemp;
  time_t tExpDate, tCurDate;
  struct tm tmExpDate, tmCurDate;
  bool bValid;
  int iMonth, iDay, iYear;
  int iExpireDays;

  //cstrPgmKey = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_PGMKEY, "");
  cstrPgmKey = g_pConfig->cstrGet_item(CFG_PGMKEY);
  // Number days before expiration.
  //iExpireDays = AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER, INI_ITEM_SIGNALCORR, 0) - 20;
  iExpireDays = g_pConfig->iGet_item(CFG_SIGNALCORR) - 20;

  // Decode expriation date from the pgmkey in the INI file.
  iMonth = (cstrPgmKey.GetAt(19) - '0') * 10;
  iMonth += (cstrPgmKey.GetAt(32) - '0');
  iMonth -= 20;
  iDay = (cstrPgmKey.GetAt(7) - '0') * 10;
  iDay += (cstrPgmKey.GetAt(64) - '0');
  iDay -= 30;
  cstrTemp = cstrPgmKey.GetAt(22);
  cstrTemp += cstrPgmKey.GetAt(47);
  cstrTemp += cstrPgmKey.GetAt(71);
  cstrTemp += cstrPgmKey.GetAt(12);
  iYear = atoi(cstrTemp);

  if(cstrPgmKey.GetAt(4) != '0' || cstrPgmKey.GetAt(6) != 'D'
  || cstrPgmKey.GetAt(13) != '9' || cstrPgmKey.GetAt(27) != '0'
  || cstrPgmKey.GetAt(35) != '6' || cstrPgmKey.GetAt(43) != '5'
  || cstrPgmKey.GetAt(54) != '8' || cstrPgmKey.GetAt(66) != '7'
  || cstrPgmKey.GetAt(72) != '8')
    bValid = false; // Invalid program.
  else
    { // Fixed locations in the pgmkey are valid.
    bValid = false;

    if(iExpireDays > 0 || (iMonth >= 1 && iMonth <= 12
    && iDay >= 1 && iDay <= 31))
    { // The program is set to expire.
      // Check to see if we already set an expiration date.
      if(iMonth > 12 && iDay > 31)
      { // No expiration date yet.  Set it.
      CTime ctCurrent;
      int iValue;

      ctCurrent = CTime::GetCurrentTime();
      CTimeSpan ctsExpire(iExpireDays, 0, 0, 0);
      ctCurrent += ctsExpire;
      // Encode the date.
      iMonth = ctCurrent.GetMonth();
      iMonth += 20;
      iDay = ctCurrent.GetDay();
      iDay += 30;
      iYear = ctCurrent.GetYear();
      // Put the date in the INI file.
      iValue = iMonth / 10;
      cstrPgmKey.SetAt(19, iValue + '0'); // Month
      iValue = iMonth % 10;
      cstrPgmKey.SetAt(32, iValue + '0'); // Month
      iValue = iDay / 10;
      cstrPgmKey.SetAt(7, iValue + '0'); // Day
      iValue = iDay % 10;
      cstrPgmKey.SetAt(64, iValue + '0'); // Day
      iValue = iYear / 1000;
      cstrPgmKey.SetAt(22, iValue + '0'); // Year
      iValue = iYear % 1000;
      iValue = iValue / 100;
      cstrPgmKey.SetAt(47, iValue + '0'); // Year
      iValue = iYear % 100;
      iValue = iValue / 10;
      cstrPgmKey.SetAt(71, iValue + '0'); // Year
      iValue = iYear % 10;
      cstrPgmKey.SetAt(12, iValue + '0'); // Year
      // Write the program key back to the INI file.
      //AfxGetApp()->WriteProfileString(INI_SEC_GENERAL, INI_ITEM_PGMKEY, cstrPgmKey);
      g_pConfig->vWrite_item(CFG_PGMKEY, cstrPgmKey);
      // Reset the month and day so it can be used below.
      iMonth -= 20;
      iDay -= 30;
      }
    }

    if(iMonth > 12 && iDay > 31)
      bValid = true; // no expiration date.
    else if(iMonth >= 1 && iMonth <= 12
    && iDay >= 1 && iDay <= 31)
      {// Program has an expiration date.
      // Get the current date.
      CTime ctCurrent;
      ctCurrent = CTime::GetCurrentTime();
      ctCurrent.GetGmtTm(&tmCurDate);
      tmCurDate.tm_hour = 0;
      tmCurDate.tm_min = 0;
      tmCurDate.tm_sec = 0;
      tmCurDate.tm_isdst = 0;
      tCurDate = mktime(&tmCurDate);
      tmExpDate.tm_mday = iDay;
      tmExpDate.tm_mon = iMonth - 1;
      tmExpDate.tm_year = iYear - 1900;
      tmExpDate.tm_hour = 0;
      tmExpDate.tm_min = 0;
      tmExpDate.tm_sec = 0;
      tmExpDate.tm_isdst = 0;
      tExpDate = mktime(&tmExpDate);
      if(tExpDate >= tCurDate)
        { // Expiration date hasn't been reached yet.
        CTime ctExpire (iYear, iMonth, iDay, 0, 0, 0);
        CString cstrMsg, cstrTitle;

        bValid = true;
        // Display date when program stops running.
        //cstrTitle.LoadString(IDS_WARNING);
        cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
        g_cstrExpirationDate = ctExpire.Format("%b %d, %Y");
        //cstrMsg.Format(IDS_EXPIRE_DATE, g_cstrExpirationDate);
        cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_EXPIRE_DATE), g_cstrExpirationDate);
        theApp.vLog(cstrMsg);
        AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrTitle, MB_OK);
        }
      }
    }
  if(bValid == false)
    { // Program is invalid.
    CString cstrMsg, cstrTitle;
    //cstrTitle.LoadString(IDS_ERROR1);
    cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    //cstrMsg.LoadString(IDS_EXPIRED);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_EXPIRED);
    theApp.vLog(cstrMsg);
    AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrTitle, MB_OK);
    ::PostQuitMessage(0); // Terminate the program.
    }
    // Check for and display a startup message.
  if(cstrPgmKey.GetAt(31) == 'B')
    {
    CString cstrMsg, cstrTitle;
    cstrTitle = ""; //.LoadString(IDS_ERROR);
    //cstrMsg.LoadString(IDS_OPEN_SCREEN_NOTICE);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_OPEN_SCREEN_NOTICE)
      + "\n" + g_pLang->cstrLoad_string(ITEM_G_OPEN_SCREEN_NOTICE1);
    AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrTitle, MB_OK);
    }
//#ifndef EGGSAS_READER
#if EGGSAS_READER == READER_NONE
  //// Not reader version
    // Get whether this is a regular or research version.
  theApp.m_ptPgmType = PT_NONE;
  if(cstrPgmKey.GetAt(5) == '7' && cstrPgmKey.GetAt(24) == 'C'
  && cstrPgmKey.GetAt(61) == '2')
    { // Standard version of program
    theApp.m_ptPgmType = PT_STANDARD;
    theApp.m_ptStudyType = PT_STANDARD;
    m_iMinPerGraph = DEFAULT_MIN_PER_GRAPH;
    }
  if(cstrPgmKey.GetAt(9) == '5' && cstrPgmKey.GetAt(36) == 'E'
  && cstrPgmKey.GetAt(58) == 'A')
    { // Indicates research version.
      // If also standard version, then this is both.
    if(theApp.m_ptPgmType == PT_STANDARD)
      theApp.m_ptPgmType = PT_STD_RSCH;
    else
      theApp.m_ptPgmType = PT_RESEARCH;
    theApp.m_ptStudyType = PT_RESEARCH;
    //m_iMinPerGraph = AfxGetApp()->GetProfileInt(INI_SEC_GENERAL,
    //                 INI_ITEM_MIN_PER_GRAPH, DEFAULT_MIN_PER_GRAPH);
    m_iMinPerGraph = g_pConfig->iGet_item(CFG_MINUTESPERGRAPH);
    }
  if(theApp.m_ptPgmType == PT_NONE)
    { // Default is standard version.
    theApp.m_ptPgmType = PT_STANDARD;
    theApp.m_ptStudyType = PT_STANDARD;
    m_iMinPerGraph = DEFAULT_MIN_PER_GRAPH;
    }
  // Look for the version that creates an ASCII data file.
  if(cstrPgmKey.GetAt(15) == '9' && cstrPgmKey.GetAt(52) == 'B'
  && cstrPgmKey.GetAt(69) == '4')
    {
    g_bAsciiDataFile = true;
    }

    // Read INI item to determine if dog studies are allowed.
  //cstrTemp = AfxGetApp()->GetProfileString(INI_SEC_ANALYSIS, INI_ITEM_ALLOW_DOG_STUDIES, "");
  cstrTemp = g_pConfig->cstrGet_item(CFG_ALLOWDOGSTUDIES);
  if(cstrTemp == "48DYN72")
    theApp.m_bAllowDogStudies = true;
  else
    theApp.m_bAllowDogStudies = false;
#else
  //// Reader version
#if EGGSAS_SUBTYPE == SUBTYPE_READER_DEMO
    CString cstrMsg, cstrTitle;
    cstrTitle = ""; //.LoadString(IDS_ERROR);
    //cstrMsg.LoadString(IDS_DEMO_OPEN_SCREEN_NOTICE);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_DEMO_OPEN_SCREEN_NOTICE);
    AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrTitle, MB_OK);
#endif
  // Reader version always handles all types of studies.
  theApp.m_ptPgmType = PT_STD_RSCH;
  theApp.m_ptStudyType = PT_RESEARCH;
  theApp.m_bAllowDogStudies = true;
  //m_iMinPerGraph = AfxGetApp()->GetProfileInt(INI_SEC_GENERAL,
  //                 INI_ITEM_MIN_PER_GRAPH, DEFAULT_MIN_PER_GRAPH);
  m_iMinPerGraph = g_pConfig->iGet_item(CFG_MINUTESPERGRAPH);
#endif


  vSetup_menus();
    // Only set up help files if the program type is for both standard and research.
  if(theApp.m_ptPgmType == PT_STD_RSCH)
    theApp.vSetup_help_files();

//  SetTitle("");
  SetPathName("c:", FALSE);
    // Get a pointer to the view.
  pos = GetFirstViewPosition();
  m_p10MinView = (CTenMinView *)GetNextView(pos);
  m_pAnalyze = new CAnalyze(this);

  cstrTemp = g_pConfig->cstrGet_item(CFG_REMOTE_DB_PATH);

    //CString cstrMsg;
    //cstrMsg.Format("Remote DB path from INI file: %s", cstrTemp);
    //AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, "DEBUG", MB_OK);

  if(cstrTemp.IsEmpty() == true)
    m_pDB = new CDbaccess(DSN_DEFAULT);
  else
    m_pDB = new CDbaccess(cstrTemp);
  m_pDB->iDBConnect();
  if(iCheck_db() == FAIL)
    { // Not the correct database.
    vDisplay_version_error(m_pDB); // Display an error message.
       // Don't allow studies to be done.  This will disable the
       // new study on new patient and existing patient menu items.
    m_bAllowStudies = false;
#if EGGSAS_READER == READER_NONE
      // Disable the "New Study" menu item.
    g_pFrame->vEnable_menu_item(0, 0, false);
      // Disable/hide the New Study button on the toolbar.
    g_pFrame->vEnable_toolbar_button(ID_FILE_NEW, FALSE);
#endif
    }
  else
    m_bAllowStudies = true;
  m_pDriver = NULL;
  m_pcfDataFile = NULL;
  m_pArtifact = NULL;
  m_bHaveFile = FALSE;
  m_bHavePatient = FALSE;
//  m_bBaseLineAnalyzed = FALSE;
//  m_bManualMode = false;
  m_uMaxPeriods = MAX_PERIODS;
  m_bReportView = false;
  m_uReportItemToView = RPT_VIEW_ITEM_NONE;
  m_bUsingWhatIf = false; // Not viewing any What If scenario.

  vInit_patient_data();
  vCopy_patient(&m_pdSavedPatient, &m_pdPatient);

  vSet_title("");

#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
#if EGGSAS_READER == READER_DEPOT
  // For CDDR, prompt the user the first time for a password.
  if(g_pConfig->m_cstrHideStudyDatesPassword.IsEmpty())
    { // On startup the first time there is no password so
      // show the Hide Study Dates dialog box for user to
      // create a password.
    PostMessage(g_pFrame->m_hWnd, WMUSER_DISPLAY_HIDE_SHOW_STUDY_DATES, 0, 0);
    }
#elif EGGSAS_READER == READER_DEPOT_E
  if(g_pConfig->m_cstrHideStudyDatesPassword.IsEmpty())
    { // On startup the first time there is no password so
      // show the Hide Study Dates dialog box for user to
      // create a password.
    PostMessage(g_pFrame->m_hWnd, WMUSER_DISPLAY_HIDE_SHOW_STUDY_DATES, 0, 0);
    }
#else
  // For all other versions (Research version and Reader), the study dates
  // are permanenty hidden.
  // Create the GUID for the hide study dates, leave the password
  // uninitialized and set the hide study dates flag.
  vCreate_hide_study_dates_GUID();
#endif
#endif

//  if(theApp.m_ptStudyType == PT_RESEARCH)
//    {  // Research version
//      // Change the title to reflect the research version.
//    vSet_research_title();
//    }
  // Start the orphan data file recovery program.
  // If there are no orphan data files, the program will automatically exit.
  _spawnl(_P_NOWAIT, theApp.m_cstrIniFilePath + "\\OrphanFileRecovery.exe", "OrphanFileRecovery.exe", "/E", NULL);

	return TRUE;
  }

/////////////////////////////////////////////////////////////////////////////
// CEGGSASDoc serialization

void CEGGSASDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEGGSASDoc diagnostics

#ifdef _DEBUG
void CEGGSASDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEGGSASDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEGGSASDoc Message handlers

/********************************************************************
OnFileSave

  Message handler for Saving the current patient.
********************************************************************/
void CEGGSASDoc::OnFileSave() 
  {
	
  if(m_bUsingWhatIf == true)
    vFileSaveWhatIf();
  else
    vFileSave();
  }

/********************************************************************
OnUpdateFileSave

  Message handler for updating the save patient menu item.
    Enable/disable the menu item based on having a patient and a 
      patient data file.
********************************************************************/
void CEGGSASDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
  {

  pCmdUI->Enable(m_bHavePatient | m_bHaveFile);
  }

/********************************************************************
OnFileSaveAs

  Message handler for Saving the current patient under a different name.
    NOT USED.
********************************************************************/
void CEGGSASDoc::OnFileSaveAs() 
  {
  }

/********************************************************************
OnUpdateFileSaveAs

  Message handler for updating the save patient under a different name
  menu item.
    Enable/disable the menu item based on having a patient and a 
      patient data file.
  NOT USED.
********************************************************************/
void CEGGSASDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
  {

  pCmdUI->Enable(m_bHavePatient | m_bHaveFile);
  }

/////////////////////////////////////////////////////////////////////////////
// CEGGSASDoc commands

/********************************************************************
vFileNewPatient - Provides the processing for the New Patient message
                  handler.  Guides the user through a complete study.

  inputs: The study type: New study for a new patient, new study for an
            existing patient.

  return: None.
********************************************************************/
void CEGGSASDoc::vFileNewPatient(short int iStudyType) 
  {
  CString cstrName;
  CDemographics *pdlg;
  CPreStudyInfo *pPreStudyInfoDlg;
  CEquipTest *pEquipDlg;
  CBaseline *pBaselineDlg;
  CWaterLoad *pWaterLoadDlg;
  CStimulationMedium *pdlgR;
  CPostWaterTest *pPostWaterDlg;
  //CSelectStudyType *pSelStudyType;
  int iDlgSts;
  short unsigned uSts, uAction;
  bool bSts = true;

  //vGet_view();
  //m_p10MinView->vShow_logo(false);

//
//  if(bSts == true)
//    {
//    g_pFrame->vSetup_for_msg();

  vGet_view();
  m_p10MinView->vShow_logo(false);

  m_iStudyType = iStudyType;
  // We are now viewing active patients.
  g_bHiddenPatients = FALSE;
  if(iCheck_to_Save() != IDCANCEL)
    { // Ok to start a new patient.
    m_bHaveFile = FALSE;
      // Remove any What If fields and data from the display.
    vRemove_whatif_from_display();
    m_uCurrentPeriod = PERIOD_BL;
    m_uPreviousPeriod = PERIOD_NONE;
#ifdef _200_CPM_HARDWARE
    m_p10MinView->vInit();
    m_pdPatient.uFrequency = 0;
      m_p10MinView->vShow_study_type();
      m_p10MinView->vShow_standard_fields(false);
      m_p10MinView->vShow_research_fields(false);
      vSetup_menus_fields();
    // For a new study on a new patient, get the study type and recording frequency.
    if(iStudyType == STUDY_NEW_PATIENT)
      bSts = bGet_study_type_freq_range();
#else
   // using 15cpm hardware only.
    if(theApp.m_ptPgmType == PT_STD_RSCH)
      { // Research version.
      CSelectStudyType *pSelStudyType;

      pSelStudyType = new CSelectStudyType();
      pSelStudyType->DoModal();
      theApp.m_ptStudyType = pSelStudyType->ptGet_study_type();
      delete pSelStudyType;
      theApp.vSetup_help_files();
        // Initialize the study data for the study type the user just selected.
        // If the user is creating a new study for an existing patient, the
        // study data would already have been set up for a research study.  Therefore
        // we have to correct that here if the user selected a WaterLoad study.
      vInit_study_data_for_study_type();
      }
#endif
    //if(bSts == true)
    //  {
        // Find out the study type.
      if(theApp.m_ptPgmType == PT_STD_RSCH)
        { // Research version.
        //pSelStudyType = new CSelectStudyType();
        //pSelStudyType->DoModal();
        //theApp.m_ptStudyType = pSelStudyType->ptGet_study_type();
        //delete pSelStudyType;
        theApp.vSetup_help_files();
          // Initialize the study data for the study type the user just selected.
          // If the user is creating a new study for an existing patient, the
          // study data would already have been set up for a research study.  Therefore
          // we have to correct that here if the user selected a WaterLoad study.
        vInit_study_data_for_study_type();
  //#ifdef _200_CPM_HARDWARE
  //    // For a new study on a new patient, get the study type and recording frequency.
  //      if(iStudyType == STUDY_NEW_PATIENT)
  //        bSts = bGet_study_type_freq_range();
  //#endif
        }
  //    else
  //      theApp.m_ptStudyType = PT_STANDARD; // Standard version.
      //  {
    if(bSts == true)
      {
      m_p10MinView->vShow_study_type();
      m_p10MinView->vShow_standard_fields(false);
      m_p10MinView->vShow_research_fields(false);
      vSetup_menus_fields();
      if(m_iStudyType == STUDY_NEW_PATIENT)
        {
        vInit_patient_data(); // Reset all the patient data for new patients.
        vCopy_patient(&m_pdSavedPatient, &m_pdPatient);
        }
        // Start with demographics.
        // If this is not a new patient, put the patient demographics in the dialog box.
      if(m_iStudyType != STUDY_NEW_PATIENT)
        { // New study, existing patient
        pdlg = new CDemographics(SM_NEW_STUDY_EXIST_PATIENT, this);
        pdlg->vSet_demographics(&m_pdPatient);
        }
      else
        pdlg = new CDemographics(SM_NEW_STUDY_NEW_PATIENT, this); // new patient.
      if(pdlg->DoModal() == IDOK)
        { // Get the demographics information.
        pdlg->vGet_demographics(&m_pdPatient);
          // Set the date of study.
        m_pdPatient.cstrDate = cstrGet_date_of_study();
          // Get the exit status
        if((uSts = pdlg->uGet_exit_sts()) == DLG_CLOSE_SAVE)
          { // Save the data to the database.
  //        theApp.vLog("Save patient and patient info to database");
          if(m_pDB->iSave_patient(&m_pdPatient) == SUCCESS)
            m_pDB->iSave_patient_info(&m_pdPatient);
  //        theApp.vLog("call vInit_patient_data()");
          vInit_patient_data(); // Reset all the patient data for new patients.
          m_bHavePatient = false;
          }
        delete pdlg;
          // Create the file name and initialize the data file.
        if(uSts == DLG_OK && iInit_data_file() == SUCCESS)
          { // Data file initialized.
            // See if we already have a study record with this Patient ID and date.
            // If so, save the study ID so we overwrite it when the study is saved.
          m_pdPatient.lStudyID = m_pDB->lFind_study_data(m_pdPatient.lPatientID, 
                                  m_pdPatient.cstrDate);
          if(theApp.m_ptStudyType == PT_RESEARCH)
            { // In case this study is being overwritten, delete all events.
            m_pDB->iDelete_events(m_pdPatient.lStudyID);
            }
          m_bHavePatient = TRUE;
            // Set title to document.
          vFormat_patient_name(&m_pdPatient, cstrName, false);
  //        SetTitle(cstrName);
          vSet_title(cstrName);
          m_p10MinView->vSet_patient_info(&m_pdPatient);
            // Pre-study information
          pPreStudyInfoDlg = new CPreStudyInfo(this, SM_NEW_STUDY);
          if(pPreStudyInfoDlg->DoModal() == IDOK)
            {
            delete pPreStudyInfoDlg;
              // Run the equipment test.
            pEquipDlg = new CEquipTest(this);
            iDlgSts = pEquipDlg->DoModal();
            delete pEquipDlg;
            if(iDlgSts == IDOK)
              { // Get the baseline.
              pBaselineDlg = new CBaseline(this);
              iDlgSts = pBaselineDlg->DoModal();
              uAction = pBaselineDlg->uGet_action();
              if(theApp.m_ptStudyType == PT_RESEARCH)
                { // Research version.
                m_pdPatient.uDataPtsInBaseline = pBaselineDlg->uGet_points_in_baseline();
                  // Set the period for the baseline.
                m_pdPatient.fPeriodStart[PERIOD_BL] = (float)0.0;
                m_pdPatient.fPeriodLen[PERIOD_BL] = fRound_down_to_half_min(
                    (float)m_pdPatient.uDataPtsInBaseline / (float)DATA_POINTS_PER_MIN_1);
                m_uPreviousPeriod = PERIOD_NONE;
                }
              else
                m_pdPatient.uDataPtsInBaseline = 0;
              delete pBaselineDlg;

                //{
                //CString cstrMsg;
                //cstrMsg.Format("@@@ DEBUG: vFileNewPatient(baseline): Period 0 len: %.1f, Period 0 start: %.1f.",
                //   m_pdPatient.fPeriodLen[0], m_pdPatient.fPeriodStart[0]);
                //theApp.vLog(cstrMsg);
                //}

              if(iDlgSts == IDOK && uAction != BL_ACTION_PAUSE)
                { // Now for the water loading phase.
                m_pdPatient.bPause = FALSE;
                if(uAction == BL_ACTION_STIM_MED)
                  {
                  if(theApp.m_ptStudyType == PT_RESEARCH)
                    { // Research version.
                    pdlgR = new CStimulationMedium(this, SM_NEW_STUDY);
                    iDlgSts = pdlgR->DoModal();
                    pdlgR->vGet_stim_med_amount(&m_pdPatient);
                    delete pdlgR;
                    }
                  else
                    { // Standard version
                    pWaterLoadDlg = new CWaterLoad(this, SM_NEW_STUDY, TYPE_INGEST_WATER);
                    iDlgSts = pWaterLoadDlg->DoModal();
                    pWaterLoadDlg->vGet_water_amount(&m_pdPatient);
                    delete pWaterLoadDlg;
                    }

                //{
                //CString cstrMsg;
                //cstrMsg.Format("@@@ DEBUG: vFileNewPatient(waterload): Period 0 len: %.1f, Period 0 start: %.1f.",
                //   m_pdPatient.fPeriodLen[0], m_pdPatient.fPeriodStart[0]);
                //theApp.vLog(cstrMsg);
                //}

                  }
                if(iDlgSts == IDOK)
                  { // Post water testing
                  pPostWaterDlg = new CPostWaterTest(this);
                  iDlgSts = pPostWaterDlg->DoModal();
                  delete pPostWaterDlg;

                    //{
                    //CString cstrMsg;
                    //cstrMsg.Format("@@@ DEBUG: vFileNewPatient(postwater): Period 0 len: %.1f, Period 0 start: %.1f.",
                    //   m_pdPatient.fPeriodLen[0], m_pdPatient.fPeriodStart[0]);
                    //theApp.vLog(cstrMsg);
                    //}

                  if(iDlgSts == IDOK)
                    { // Post water testing complete.
                      // Now graph the baseline period and make sure all menu
                      // items are enabled.
                    if(m_pdPatient.cstrFile.IsEmpty() == FALSE)
                      { // Set up any additional periods if we need to.
                      vSet_add_periods(STUDY_TYPE_NEW);
                        // Read in and graph the baseline period.
                      m_bHaveFile = TRUE;
                      if(theApp.m_ptStudyType == PT_STANDARD)
                        vInit_artifact();
                      else
                        m_p10MinView->vInit_period_good_mins(); // Research study.
                      //// Currently have an open data file.  Close it.
                      //vClose_data_file();
                      vFileSave(); // Save the study to the database.
                      if(g_bAsciiDataFile == true)
                        vSave_ascii_data_file();
                      vAnalyzeBaseline();
                      }
                    }
                  }
                }
              else
                {
                if(iDlgSts == IDOK && uAction == BL_ACTION_PAUSE)
                  { // User paused study.
                  //// Currently have an open data file.  Close it.
                  //vClose_data_file();
                  m_pdPatient.bPause = TRUE;
                  m_bHaveFile = TRUE;
                  vFileSave(); // Save the study to the database.
  //                vInit_artifact();
                  vAnalyzeBaseline();
                  }
                }
              }
            }
          else
            delete pPreStudyInfoDlg;
          }
        } // End of DoModal() for demographics.
      else
        delete pdlg;  // Canceled the demographics information.
      }
    } // end of iCheck_to_Save()
//  theApp.vLog("Leave vFileNewPatient");
  }

/********************************************************************
vFileResumeStudy - Provides the processing for resuming a previously
                   paused study.  Guides the user through to completion
                   of the study.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vFileResumeStudy()
  {
  CEquipTest *pEquipDlg;
  CPostWaterTest *pPostWaterDlg;
  int iDlgSts;

  vGet_view();
  m_p10MinView->vShow_logo(false);
  m_bHaveFile = FALSE;
  m_iStudyType = STUDY_RESUME;
  if(iCheck_to_Save() != IDCANCEL)
    { // Patient and study data has already been retrieved.
    m_uCurrentPeriod = PERIOD_BL;
    m_uPreviousPeriod = PERIOD_NONE;
      // Open the data file for appending.
    if(iResume_data_file() == SUCCESS)
      { // Data file exists and is opened.
        // Remove any What If fields and data from the display.
      vRemove_whatif_from_display();
      m_p10MinView->vShow_standard_fields(false);
      m_p10MinView->vShow_research_fields(false);
      vSetup_menus_fields();
      m_bHaveFile = TRUE;
/******************
  if(m_pDriver == NULL)
    { // We don't have the driver initialized yet, so initialize it.
    m_pDriver = new CDriver();
    m_pDriver->iInit();
    }
  else
      // Driver has been initialized, but make sure it is reset.
    m_pDriver->iReInitEGG();
*************************/
      pEquipDlg = new CEquipTest(this);
      iDlgSts = pEquipDlg->DoModal();
      delete pEquipDlg;
      if(iDlgSts == IDOK)
        {
          // Go do post-stimulation testing
        pPostWaterDlg = new CPostWaterTest(this);
        iDlgSts = pPostWaterDlg->DoModal();
        delete pPostWaterDlg;
        if(iDlgSts == IDOK)
          { // Post stimulation testing complete.
            // Now graph the baseline period and make sure all menu
            // items are enabled.
           if(m_pdPatient.cstrFile.IsEmpty() == FALSE)
             { // Set up any additional periods if we need to.
             vSet_add_periods(STUDY_TYPE_NEW);
               // Read in and graph the baseline period.
             m_bHaveFile = TRUE;
             m_p10MinView->vInit_period_good_mins(); // Research study.
             vFileSave(); // Save the study to the database.
             if(g_bAsciiDataFile == true)
               vSave_ascii_data_file();
             vAnalyzeBaseline();
             }
          }
        else
          { // Canceled the resume study.  Set up for analysis of baseline.
          vGraph_data(true, SM_EDIT);
          }
        }
      }
    }
  }

/********************************************************************
iSelectPatient - Read in all the patient and study data (from the database).
                 Display the baseline raw data graph.

  inputs: Study type:
            STUDY_EXIST_PATIENT: Existing patient to perform a new study.
            STUDY_NONE: Existing patient, existing study.
          Study ID (from the database).  If not INV_LONG, then the
            patient database table hasn't been read yet to get the name.
            Default is INV_LONG.
          TRUE to view hidden patients, FALSE to view non-hidden patients.
            Default if FALSE.
  return: None.
********************************************************************/
short int CEGGSASDoc::iSelectPatient(short int iStudyType, long lStudyID, BOOL bHiddenPatients) 
  {
  CSelectPatient *pdlg;
  CString cstrName;
  WHAT_IF wi;
  short int iIndex, iRet, iDBSts;
  int iSts;
  long lPatientID, lWhatIfID;
  bool bSts = true;

  vGet_view();
  m_p10MinView->vShow_logo(false);
   // Check to see if last patient was saved.
  if((iSts = iCheck_to_Save()) != IDCANCEL)
    { // Ok to select a new patient.
#ifdef _200_CPM_HARDWARE
    m_bHaveFile = FALSE;
    vRemove_whatif_from_display();
    m_uCurrentPeriod = PERIOD_BL;
    m_uPreviousPeriod = PERIOD_NONE;
    m_pdPatient.uFrequency = 0;
    m_p10MinView->vInit();
    m_p10MinView->vShow_study_type();
    m_p10MinView->vShow_standard_fields(false);
    m_p10MinView->vShow_research_fields(false);
    vSetup_menus_fields();
    // For a new study on an existing patient, get the study type and recording frequency.
    if(iStudyType == STUDY_EXIST_PATIENT)
      bSts = bGet_study_type_freq_range();
#endif
    if(bSts == true)
      {
      lWhatIfID = INV_LONG;
      if(lStudyID == INV_LONG)
        {
        pdlg = new CSelectPatient(this, iStudyType, bHiddenPatients);
        if((iSts = pdlg->DoModal()) == IDOK)
          { // Initialize the patient data.
          g_bHiddenPatients = bHiddenPatients;
          vInit_patient_data();
          pdlg->vGet_patient_info(&m_pdPatient, &lWhatIfID);
          }
        delete pdlg;
        }
      else
        {
        lPatientID = m_pdPatient.lPatientID;
          // Initialize the patient data.
        vInit_patient_data();
        m_pdPatient.lPatientID = lPatientID;
        m_pdPatient.lStudyID = lStudyID;
        if(m_pDB != NULL && m_pDB->iGet_patient(&m_pdPatient) == SUCCESS)
          iSts = IDOK;
        else
          iSts = IDCANCEL;
        }
      if(iSts == IDOK)
        { // Get the patient name and date.
          // Remove any What If fields and data from the display.
        vRemove_whatif_from_display();
          // Display the hourglass cursor.
        SetCursor(LoadCursor(NULL, IDC_WAIT));
        ShowCursor(TRUE);
          // If this is a new study on an existing patient, remove the graphs.
        if(iStudyType == STUDY_EXIST_PATIENT || iStudyType == STUDY_RESUME)
          m_p10MinView->vInit();
        // Currently have an open data file.  Close it.
        vClose_data_file(false);
        m_uCurrentPeriod = PERIOD_BL;
        m_uPreviousPeriod = PERIOD_NONE;
        m_bHavePatient = TRUE;
          // Get the rest of the patient info from the database.
        m_pdPatient.cstrFile.Empty();
        if(m_pDB != NULL)
          {
          if((iDBSts = m_pDB->iFind_patient(&m_pdPatient)) == SUCCESS
          && (iDBSts = m_pDB->iGet_patient_info(&m_pdPatient)) == SUCCESS)
            {
            m_pdPatient.bHavePatientInfo = true;
            if(m_pdPatient.cstrLastName == PATIENT_TYPE_DOG)
              m_pdPatient.uPatientType = PAT_DOG;
            else
              m_pdPatient.uPatientType = PAT_HUMAN;
            if(iStudyType == STUDY_NONE || iStudyType == STUDY_RESUME)
              { // This is not a new study, so get the study data.
              if((iDBSts = m_pDB->iGet_study_data(&m_pdPatient)) == SUCCESS)
                {

                if(m_pdPatient.uDataPtsInBaseline == 0)
                  theApp.m_ptStudyType = PT_STANDARD;
                else
                  theApp.m_ptStudyType = PT_RESEARCH;
                //int iFreq;
                //// Set the frequency range.
                //if(m_pdPatient.uFrequency == 15)
                //  iFreq = FREQ_LIMIT_15_CPM;
                //else if(m_pdPatient.uFrequency == 60)
                //  iFreq = FREQ_LIMIT_60_CPM;
                //else
                //  iFreq = FREQ_LIMIT_200_CPM;
                //theApp.vSet_freq_range(iFreq);
                // Now see what has been analyzed.
                if(theApp.m_ptStudyType == PT_RESEARCH)
                  { // This is a research study type.  If its a WhatIF study,
                    // get the WhatIF data.
                  if(lWhatIfID != INV_LONG)
                    { // This is a WhatIF, set up for it.
                    wi.lWhatIfID = lWhatIfID;
                    if(m_pDB->iGet_whatif_data(&wi) == SUCCESS)
                      { // Got the data.  Update the patient data.
                      vSet_freq_range(wi.uFrequency);
                      m_wifOrigData.lStudyID = wi.lStudyID;
                      m_wifOrigData.lWhatIfID = lWhatIfID;
                      m_pdPatient.uNumPeriods = wi.uNumPeriods;
                      m_wifOrigData.uNumPeriods = wi.uNumPeriods;
                      m_wifOrigData.cstrDateTime = wi.cstrDateTime;
                      m_wifOrigData.cstrDescription = wi.cstrDescription;
                      m_pdPatient.uFrequency = wi.uFrequency;
                      m_wifOrigData.uFrequency = wi.uFrequency;
                        // Copy the what if data to the patient data.
                      memcpy(m_pdPatient.fPeriodStart, wi.fPeriodStart, sizeof(wi.fPeriodStart));
                      memcpy(m_pdPatient.fPeriodLen, wi.fPeriodLen, sizeof(wi.fPeriodLen));
                      memcpy(m_pdPatient.fGoodMinStart, wi.fGoodMinStart, sizeof(wi.fGoodMinStart));
                      memcpy(m_pdPatient.fGoodMinEnd, wi.fGoodMinEnd, sizeof(wi.fGoodMinEnd));
                      memcpy(m_pdPatient.uRespRate, wi.uRespRate, sizeof(wi.uRespRate));
                        // Save the original what if data.
                      memcpy(m_wifOrigData.fPeriodStart, wi.fPeriodStart, sizeof(wi.fPeriodStart));
                      memcpy(m_wifOrigData.fPeriodLen, wi.fPeriodLen, sizeof(wi.fPeriodLen));
                      memcpy(m_wifOrigData.fGoodMinStart, wi.fGoodMinStart, sizeof(wi.fGoodMinStart));
                      memcpy(m_wifOrigData.fGoodMinEnd, wi.fGoodMinEnd, sizeof(wi.fGoodMinEnd));
                      memcpy(m_wifOrigData.uRespRate, wi.uRespRate, sizeof(wi.uRespRate));
                      m_bUsingWhatIf = false;
                      }
                    }
                  else
                    { // Not a WhatIf
                    vSet_freq_range(m_pdPatient.uFrequency);
                    }
                  }
               
                  // Determine if this is a standard or research study.
                if(m_pdPatient.uDataPtsInBaseline == 0)
                  theApp.m_ptStudyType = PT_STANDARD;
                else
                  {
                  theApp.m_ptStudyType = PT_RESEARCH;
                    // Read in the events.
                  CEvnt *pEvent;
                  long lStudyID;
                  pEvent = new CEvnt();
                  lStudyID = m_pdPatient.lStudyID;
                  while(m_pDB->iGet_event(lStudyID, pEvent) == SUCCESS)
                    {
                    if(pEvent->m_iPeriodType == EVENT_BASELINE)
                      m_pdPatient.Events.m_BLEvents.Add(pEvent);
                    else
                      m_pdPatient.Events.m_PSEvents.Add(pEvent);
                    pEvent = new CEvnt();
                    lStudyID = INV_LONG;
                    }
                  delete pEvent;  // Delete the last one, its not used.
                  }
                vSetup_menus_fields();
                theApp.vSetup_help_files();
                if(theApp.m_ptStudyType == PT_STANDARD)
                  { // Standard study
                    // Also make sure the starting and ending periods are valid.
                  for(iIndex = 0; iIndex < MAX_PERIODS; ++iIndex)
                    {
                    if(m_pdPatient.fGoodMinStart[iIndex] != INV_MINUTE)
                      m_pdPatient.bPeriodsAnalyzed[iIndex] = true;
                    if(m_pdPatient.fPeriodLen[iIndex] == (float)0)
                      {
                      m_pdPatient.fPeriodLen[iIndex] = (float)10;
                      m_pdPatient.fPeriodStart[iIndex] = (float)(iIndex * 10);
                      }
                    }
                  }
                else
                  { // Research study.
                  vSet_rsrch_pds_analyzed();
                  }
                }
              }
            }
          }
        else
          { // No database access.
          }
        m_uMenuType = SM_EDIT;
#if EGGSAS_READER == READER_DEPOT
      if(m_bUsingWhatIf == false)
        m_uMenuType = SM_VIEW;
#endif
        vCopy_patient(&m_pdSavedPatient, &m_pdPatient);
        m_bHaveFile = FALSE;
        m_p10MinView->vSet_patient_info(&m_pdPatient);
        if(m_pdPatient.cstrFile.IsEmpty() == FALSE)
          { // Read in and graph the baseline period.
          if(iRead_period(m_uCurrentPeriod, m_fData) == SUCCESS)
            { // Read in the file, now graph it.
            m_p10MinView->vEnable_fields(TRUE);
            if(theApp.m_ptStudyType == PT_STANDARD)
              vInit_artifact();
            else
              vInit_stim_period_menus();
            vGraph_data(true, m_uMenuType);
            m_bHaveFile = TRUE;
            if(theApp.m_ptStudyType == PT_RESEARCH)
              { // Research file.
                // Find the maximum number of periods.
              m_uMaxPeriods = m_pdPatient.uNumPeriods + 1;
                // Check/uncheck checkboxes for period and good minutes.
              m_p10MinView->vInit_period_good_mins();
              }
            UpdateAllViews(NULL);
            }
          }
          // Restore the arrow cursor.
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        ShowCursor(TRUE);
        iRet = SUCCESS;
          // Set up any additional periods if we need to.
        vSet_add_periods(STUDY_TYPE_EXISTING);
  //      m_uMenuType = SM_EDIT;
  //#if EGGSAS_READER == READER_DEPOT
  //      m_uMenuType = SM_VIEW;
  //#endif
          // Set the window title.
        vFormat_patient_name(&m_pdPatient, cstrName, false);
  //      cstrName.Format("%s %s", m_pdPatient.cstrFirstName, m_pdPatient.cstrLastName);
  //      SetTitle(cstrName);
  //      if(theApp.m_ptPgmType == PT_RESEARCH || theApp.m_ptPgmType == PT_STD_RSCH)
  //        vSet_research_title();
        vSet_title(cstrName);
        if(theApp.m_ptStudyType == PT_RESEARCH)
          {
          if(lWhatIfID != INV_LONG)
            { // This is a WhatIF.
            vWhatIf(lWhatIfID);
#if EGGSAS_READER == READER_DEPOT
            m_uMenuType = SM_EDIT;
#endif
            }
          vSet_rsrch_pds_analyzed();
          m_p10MinView->vAdd_menu_items_for_period();
          // Restore RSA filtering.
          g_bFilterRSA = g_pConfig->m_bFilterRSA;
        }
        else
          { // Water load study.  Set frequency to 15 cpm.
          g_bFilterRSA = false; // RSA filtering is turned off.
          //  Set frequency to 15 cpm.
          theApp.vSet_freq_range(FREQ_LIMIT_15_CPM);
          g_pConfig->vWrite_item(CFG_FREQ_RANGE, FREQ_LIMIT_15_CPM);
          }
        }
      else
        iRet = FAIL;
      }
    else
      iRet = FAIL;
    } // End of iCheck_to_Save()
  else
    {
    if(iSts == IDCANCEL)
      iRet = FAIL;
    else
      iRet = SUCCESS;
    }
  return(iRet);
  }

/********************************************************************
vAnalyzeBaseline

  Message handler for analyzing the baseline period.
    Display the baseline raw data graph.
    Select good minutes
    Present the good minutes to the user to be accepted, rejected, or modified
********************************************************************/
void CEGGSASDoc::vAnalyzeBaseline() 
  {
  unsigned short uPrevPeriodOrig;

  vGet_view();
    // Draw the raw data graph.
  uPrevPeriodOrig = m_uPreviousPeriod;
  m_uPreviousPeriod = m_uCurrentPeriod;
  m_uCurrentPeriod = PERIOD_BL;
  m_uMenuType = SM_EDIT;
#if EGGSAS_READER == READER_DEPOT
  if(m_bUsingWhatIf == false)
    m_uMenuType = SM_VIEW;
#endif
  if(bGet_data_graph(true, m_uMenuType) == false)
    { // Didn't graph the baseline.  Restore the original period.
    m_uCurrentPeriod = m_uPreviousPeriod;
    m_uPreviousPeriod = uPrevPeriodOrig;
    }
  }

/********************************************************************
vViewPreStudyInfo - Provides the processing for the CMainFrame
                    message handler for viewing the prestudy information.
                      - Display the prestudy information dialog box
                        for viewing only.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vViewPreStudyInfo() 
  {
  CPreStudyInfo *pPreStudyInfoDlg;
	
  pPreStudyInfoDlg = new CPreStudyInfo(this, SM_VIEW);
  pPreStudyInfoDlg->DoModal();
  delete pPreStudyInfoDlg;
  } 

/********************************************************************
vEditPreStudyInfo - Provides the processing for the CMainFrame
                    message handler for veditingiewing the prestudy information.
                      - Display the prestudy information dialog box
                        for editing.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vEditPreStudyInfo() 
  {
  CPreStudyInfo *pPreStudyInfoDlg;
	
  pPreStudyInfoDlg = new CPreStudyInfo(this, SM_EDIT);
  if(pPreStudyInfoDlg->DoModal() == IDOK)
    m_bReportChanged = pPreStudyInfoDlg->bChanged();
  delete pPreStudyInfoDlg;
  } 
/********************************************************************
vEditPatientDemograhics - Provides the processing for the CMainFrame
                          message handler for editing the patient demographics.
                           - Display the patient demographics dialog box
                             for editing.
                           - If the user changed the use of the research
                             reference number, make the change of
                             name/reference number where ever it is
                             being displayed.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vEditPatientDemograhics() 
  {
  CString cstrName;
  CDemographics *pdlg;
	
  pdlg = new CDemographics(SM_EDIT, this);
  pdlg->vSet_demographics(&m_pdPatient);
  if(pdlg->DoModal() == IDOK)
    {
    pdlg->vGet_demographics(&m_pdPatient);
    m_bReportChanged = pdlg->bChanged();
      // If user changed the use of the research reference number,
      // Change the user name for display.
    vFormat_patient_name(&m_pdPatient, cstrName, false);
//    SetTitle(cstrName);
    vSet_title(cstrName);
    if(g_pFrame->m_uViewID == VIEWID_TEN_MIN)
      m_p10MinView->vSet_patient_info(&m_pdPatient);
    }
  delete pdlg;
  }

/********************************************************************
vViewPatientDemograhics - Provides the processing for the CMainFrame
                          message handler for viewing the patient demographics.
                           - Display the patient demographics dialog box
                             for viewing only.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vViewPatientDemograhics() 
  {
  CDemographics *pdlg;
	
  pdlg = new CDemographics(SM_VIEW, this);
  pdlg->vSet_demographics(&m_pdPatient);
  pdlg->DoModal();
  delete pdlg;
  }

/********************************************************************
vEditPatientDiagHist - Provides the processing for the CMainFrame
                       message handler for editing the patient diagnostic
                       history.
                         - Display the patient diagnostic history dialog box
                           for editing.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vEditPatientDiagHist() 
  {
  CDiagnosticHistory *pdlg;
	
  pdlg = new CDiagnosticHistory(this, SM_EDIT);
  if(pdlg->DoModal() == IDOK)
    {
    if(pdlg->m_lStudyID != INV_LONG && pdlg->m_lStudyID != m_pdPatient.lStudyID)
      iSelectPatient(STUDY_NONE, pdlg->m_lStudyID); // User wants to view a study.
    }
  delete pdlg;
  }

/********************************************************************
vViewPatientDiagHist - Provides the processing for the CMainFrame
                       message handler for viewing the patient diagnostic
                       history.
                         - Display the patient diagnostic history dialog box
                           for viewing only.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vViewPatientDiagHist() 
  {
  CDiagnosticHistory *pdlg;
	
  pdlg = new CDiagnosticHistory(this, SM_VIEW);
  if(pdlg->DoModal() == IDOK)
    {
    if(pdlg->m_lStudyID != INV_LONG && pdlg->m_lStudyID != m_pdPatient.lStudyID)
      iSelectPatient(STUDY_NONE, pdlg->m_lStudyID); // User wants to view a study.
    }
  delete pdlg;
  }

/********************************************************************
vEditPatientMeds - Provides the processing for the CMainFrame
                   message handler for editing the patient medications.
                     - Display the patient medications dialog box
                       for editing.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vEditPatientMeds() 
  {
  CMedications *pdlg;
	
  pdlg = new CMedications(this, SM_EDIT);
  if(pdlg->DoModal() == IDOK)
    {
    if(pdlg->m_lStudyID != INV_LONG && pdlg->m_lStudyID != m_pdPatient.lStudyID)
      iSelectPatient(STUDY_NONE, pdlg->m_lStudyID); // User wants to view a study.
    }
  delete pdlg;
  }

/********************************************************************
vViewPatientMeds - Provides the processing for the CMainFrame
                   message handler for viewing the patient medications.
                     - Display the patient medications dialog box
                       for viewing.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vViewPatientMeds() 
  {
  CMedications *pdlg;
	
  pdlg = new CMedications(this, SM_VIEW);
  if(pdlg->DoModal() == IDOK)
    {
    if(pdlg->m_lStudyID != INV_LONG && pdlg->m_lStudyID != m_pdPatient.lStudyID)
      iSelectPatient(STUDY_NONE, pdlg->m_lStudyID); // User wants to view a study.
    }
  delete pdlg;
  }

/********************************************************************
OnAnalyzeCurrent - Provides the processing for the CMainFrame
                   message handler for analyzing the current period.
                     - Display the baseline raw data graph.
                     - Select good minutes
                     - Present the selected good minutes to the user to be
                       accepted, rejected, or modified

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vAnalyzeCurrent() 
  {

  vGet_view();
  m_uMenuType = SM_EDIT;
#if EGGSAS_READER == READER_DEPOT
  if(m_bUsingWhatIf == false)
    m_uMenuType = SM_VIEW;
#endif
  vGraph_data(true, m_uMenuType);
  }

/********************************************************************
vAnalyzeDiagnosis - Provides the processing for the CMainFrame
                    message handler for making a diagnosis.
                      - Display the making a diagnosis dialgog box.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vAnalyzeDiagnosis() 
  {
  CDiagnosis *pdlg;
	
//  vGet_view();
  pdlg = new CDiagnosis(this, SM_NEW_STUDY);
  pdlg->vSet_diagnosis(&m_pdPatient);
  if(pdlg->DoModal() == IDOK)
    {
    pdlg->vGet_diagnosis(&m_pdPatient);
    m_bReportChanged = pdlg->bChanged();
    }
  delete pdlg;
  }

/********************************************************************
vAnalyzePostwater10 - Provides the processing for the CMainFrame
                      message handler for analyzing the postwater first
                      10 minute period.
                        - Display the postwater first 10 minute period raw
                          data graph.
                        - Select good minutes
                        - Present the selected good minutes to the user to be
                          accepted, rejected, or modified

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vAnalyzePostwater10() 
  {

  vGet_view();
  m_uCurrentPeriod = PERIOD_10;
  m_uMenuType = SM_EDIT;
#if EGGSAS_READER == READER_DEPOT
  if(m_bUsingWhatIf == false)
    m_uMenuType = SM_VIEW;
#endif
  bGet_data_graph(true, m_uMenuType);
  }

/********************************************************************
vAnalyzePostwater20 - Provides the processing for the CMainFrame
                      message handler for analyzing the postwater second
                      10 minute period.
                        - Display the postwater second 10 minute period raw
                          data graph.
                        - Select good minutes
                        - Present the selected good minutes to the user to be
                          accepted, rejected, or modified

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vAnalyzePostwater20() 
  {

  vGet_view();
  m_uCurrentPeriod = PERIOD_20;
  m_uMenuType = SM_EDIT;
#if EGGSAS_READER == READER_DEPOT
  if(m_bUsingWhatIf == false)
    m_uMenuType = SM_VIEW;
#endif
  bGet_data_graph(true, m_uMenuType);
  }

/********************************************************************
vAnalyzePostwater30 - Provides the processing for the CMainFrame
                      message handler for analyzing the postwater third
                      10 minute period.
                        - Display the postwater third 10 minute period raw
                          data graph.
                        - Select good minutes
                        - Present the selected good minutes to the user to be
                          accepted, rejected, or modified

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vAnalyzePostwater30() 
  {

  vGet_view();
  m_uCurrentPeriod = PERIOD_30;
  m_uMenuType = SM_EDIT;
#if EGGSAS_READER == READER_DEPOT
  if(m_bUsingWhatIf == false)
    m_uMenuType = SM_VIEW;
#endif
  bGet_data_graph(true, m_uMenuType);
  }

/********************************************************************
vAnalyzePostwater40 - Provides the processing for the CMainFrame
                      message handler for analyzing the postwater fourth
                      10 minute period.
                        - Display the postwater fourth 10 minute period raw
                          data graph.
                        - Select good minutes
                        - Present the selected good minutes to the user to be
                          accepted, rejected, or modified

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vAnalyzePostwater40() 
  {

  vGet_view();
  m_uCurrentPeriod = PERIOD_40;
  m_uMenuType = SM_EDIT;
#if EGGSAS_READER == READER_DEPOT
  if(m_bUsingWhatIf == false)
    m_uMenuType = SM_VIEW;
#endif
  bGet_data_graph(true, m_uMenuType);
  }

/********************************************************************
vEditSuggestedDiag - Provides the processing for the CMainFrame
                     message handler for editing the suggested diagnosis.
                        - Display the suggested diagnosis dialog box.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vEditSuggestedDiag() 
  {
  CDiagnosis *pdlg;
	
  vGet_view();
  pdlg = new CDiagnosis(this, SM_EDIT);
  pdlg->vSet_diagnosis(&m_pdPatient);
  if(pdlg->DoModal() == IDOK)
    {
    pdlg->vGet_diagnosis(&m_pdPatient);
    m_bReportChanged = pdlg->bChanged();
    }
  delete pdlg;
  }

/********************************************************************
vEditRecommendations - Provides the processing for the CMainFrame
                       message handler for editing the recommendations.
                        - Display the recommendations dialog box.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vEditRecommendations()
  {
  CRecommendations *pdlg;
	
  vGet_view();
  pdlg = new CRecommendations(this, SM_EDIT);
  pdlg->vSet_recommendations(m_pdPatient.cstrRecommendationKeyPhrase,
    m_pdPatient.cstrRecommendations);
  if(pdlg->DoModal() == IDOK)
    {
    m_bReportChanged = pdlg->bChanged();
    pdlg->vGet_recommendations(m_pdPatient.cstrRecommendationKeyPhrase,
      m_pdPatient.cstrRecommendations);
    }
  delete pdlg;
  }

/********************************************************************
vGotoNextPeriod - Provides the processing for the CMainFrame
                  message handler for going to the next 10 minute period.
                    - Display the raw data graph for the next 10 minute period.
                    - Select good minutes
                    - Present the selected good minutes to the user to be
                       accepted, rejected, or modified

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vGotoNextPeriod() 
  {
  unsigned short uPrevPeriodOrig; // uMaxPeriod, 

  vGet_view();
  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard version
//    if(m_pdPatient.bExtra10Min == TRUE)
//      uMaxPeriod = PERIOD_40;
//    else
//      uMaxPeriod = PERIOD_30;
//    if(m_uCurrentPeriod < uMaxPeriod)
    if(m_uCurrentPeriod < PERIOD_30)
      ++m_uCurrentPeriod;
    bGet_data_graph(true, m_uMenuType);
    }
  else
    { // Research version
    bool bIncrementPeriod;
      // TODO: Maximum number of periods is variable.
    //if(m_uCurrentPeriod < m_pdPatient.uNumPeriods && m_pdPatient.uNumPeriods >= 1)
    if(m_pdPatient.uNumPeriods >= 1)
      {
      if(m_pdPatient.fPeriodLen[m_uCurrentPeriod + 1] == INV_PERIOD_LEN
        || m_uCurrentPeriod == m_pdPatient.uNumPeriods)
        bIncrementPeriod = false;
      else
        bIncrementPeriod = true;
      //if(m_uCurrentPeriod == 0
      //|| m_pdPatient.fPeriodLen[m_uCurrentPeriod + 1] != INV_PERIOD_LEN)
        //{ // Go to next period there is one.  If the current period is the
        //  // baseline, there is always a next period.
        if(bIncrementPeriod == true)
          {
          uPrevPeriodOrig = m_uPreviousPeriod;
          m_uPreviousPeriod = m_uCurrentPeriod;
          ++m_uCurrentPeriod;
          }
        if(bGet_data_graph(true, m_uMenuType) == false)
          { // Didn't graph the period.
            // Restore the original period if we incremented the period.
//          --m_uCurrentPeriod;
          if(bIncrementPeriod == true)
            m_uPreviousPeriod = uPrevPeriodOrig;
          }
        //}
      }
    }
  }

/********************************************************************
vGotoEnd - Provides the processing for the CMainFrame
           message handler for going to the last 10 minute period.
             - Display the raw data graph for the last 10 minute period.
             - Select good minutes
             - Present the selected good minutes to the user to be
                accepted, rejected, or modified

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vGotoEnd() 
  {

  vGet_view();
  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard version
//    if(m_pdPatient.bExtra10Min == TRUE)
//      m_uCurrentPeriod = PERIOD_40;
//    else
    m_uCurrentPeriod = PERIOD_30;
    bGet_data_graph(true, m_uMenuType);
    }
  else
    { // Research version
    if(m_pdPatient.uNumPeriods >= 1)
      {
      m_uPreviousPeriod = m_uCurrentPeriod;
      m_uCurrentPeriod = m_pdPatient.uNumPeriods;
      bGet_data_graph(true, m_uMenuType);
      }
    }
  }

/********************************************************************
vGotoPrevPeriod - Provides the processing for the CMainFrame
                  message handler for going to the previous 10 minute period.
                    - Display the raw data graph for the previous 10 minute
                      period.
                    - Select good minutes
                    - Present the selected good minutes to the user to be
                       accepted, rejected, or modified

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vGotoPrevPeriod() 
  {

  vGet_view();
  if(m_uCurrentPeriod > PERIOD_BL)
    {
    m_uPreviousPeriod = m_uCurrentPeriod;
    --m_uCurrentPeriod;
    }
  bGet_data_graph(true, m_uMenuType);
  }

/********************************************************************
vGotoStart - Provides the processing for the CMainFrame
             message handler for going to the first 10 minute period.
               - Display the raw data graph for the first 10 minute
                 period.
               - Select good minutes
               - Present the selected good minutes to the user to be
                  accepted, rejected, or modified

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vGotoStart() 
  {

  vGet_view();
  m_uPreviousPeriod = m_uCurrentPeriod;
  m_uCurrentPeriod = PERIOD_BL;
  bGet_data_graph(true, m_uMenuType);
  }

/********************************************************************
vGotoCurrent - Provides the processing for the CMainFrame
               message handler for going to the current 10 minute period.
                 - Display the raw data graph for the current 10 minute
                   period.
                 - Select good minutes
                 - Present the selected good minutes to the user to be
                    accepted, rejected, or modified

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vGotoCurrent()
  {

  vGet_view();
  m_uPreviousPeriod = m_uCurrentPeriod;
  bGet_data_graph(true, m_uMenuType);
  }

/********************************************************************
vViewBaseline - Provides the processing for the CMainFrame
                message handler for viewing the baseline period.
                 - Display the raw data graph for the baseline period.
                 - Select good minutes
                 - Do not allow the user to accept, reject, or modify.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vViewBaseline() 
  {

  vGet_view();
  m_uPreviousPeriod = m_uCurrentPeriod;
  m_uCurrentPeriod = PERIOD_BL;
  m_uMenuType = SM_VIEW;
  bGet_data_graph(true, SM_VIEW);
  }

/********************************************************************
vViewPostwater10 - Provides the processing for the CMainFrame
                   message handler for viewing the postwater first 10
                   minute period.
                    - Display the raw data graph for the postwater first
                      10 minute period.
                    - Select good minutes
                    - Do not allow the user to accept, reject, or modify.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vViewPostwater10() 
  {

  vGet_view();
  m_uCurrentPeriod = PERIOD_10;
  m_uMenuType = SM_VIEW;
  bGet_data_graph(true, SM_VIEW);
  }

/********************************************************************
vViewPostwater20 - Provides the processing for the CMainFrame
                   message handler for viewing the postwater second 10
                   minute period.
                    - Display the raw data graph for the postwater second
                      10 minute period.
                    - Select good minutes
                    - Do not allow the user to accept, reject, or modify.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vViewPostwater20() 
  {

  vGet_view();
  m_uCurrentPeriod = PERIOD_20;
  m_uMenuType = SM_VIEW;
  bGet_data_graph(true, SM_VIEW);
  }

/********************************************************************
vViewPostwater30 - Provides the processing for the CMainFrame
                   message handler for viewing the postwater third 10
                   minute period.
                    - Display the raw data graph for the postwater third
                      10 minute period.
                    - Select good minutes
                    - Do not allow the user to accept, reject, or modify.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vViewPostwater30() 
  {

  vGet_view();
  m_uCurrentPeriod = PERIOD_30;
  m_uMenuType = SM_VIEW;
  bGet_data_graph(true, SM_VIEW);
  }

/********************************************************************
vViewPostwater40 - Provides the processing for the CMainFrame
                   message handler for viewing the postwater fourth 10
                   minute period.
                    - Display the raw data graph for the postwater fourth
                      10 minute period.
                    - Select good minutes
                    - Do not allow the user to accept, reject, or modify.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vViewPostwater40() 
  {

  vGet_view();
  m_uCurrentPeriod = PERIOD_40;
  m_uMenuType = SM_VIEW;
  bGet_data_graph(true, SM_VIEW);
  }

/********************************************************************
vViewSuggestedDiag - Provides the processing for the CMainFrame
                     message handler for viewing suggested diagnosis.
                      - Display the suggested diagnosis dialog box
                        for viewing.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vViewSuggestedDiag() 
  {
  CDiagnosis *pdlg;
	
  vGet_view();
  pdlg = new CDiagnosis(this, SM_VIEW);
  pdlg->vSet_diagnosis(&m_pdPatient);
  pdlg->DoModal();
  delete pdlg;
  }

/********************************************************************
vViewRecommendations - Provides the processing for the CMainFrame
                       message handler for viewing recommendations.
                        - Display the recommendations dialog box for viewing.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vViewRecommendations()
  {
  CRecommendations *pdlg;
	
  vGet_view();
  pdlg = new CRecommendations(this, SM_VIEW);
  pdlg->vSet_recommendations(m_pdPatient.cstrRecommendationKeyPhrase,
    m_pdPatient.cstrRecommendations);
  pdlg->DoModal();
  delete pdlg;
  }

void CEGGSASDoc::vViewEvents()
  {
  CEventList *pdlg;

  pdlg = new CEventList(this);
  pdlg->DoModal();
  delete pdlg;
  }

/********************************************************************
vEditAmtWater - Provides the processing for the CMainFrame
                message handler for editing the amount of water ingested.
                 - Display the amount of water ingested dialog box
                   for editing.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vEditAmtWater() 
  {
  CWaterLoad *pdlgS;
  CStimulationMedium *pdlgR;

  if(theApp.m_ptStudyType == PT_RESEARCH)
    {
    pdlgR = new CStimulationMedium(this, SM_EDIT);
    if(pdlgR->DoModal() == IDOK)
      pdlgR->vGet_stim_med_amount(&m_pdPatient);
    delete pdlgR;
    }
  else
    { // Standard version
    pdlgS = new CWaterLoad(this, SM_EDIT, TYPE_WATER_AMOUNT);
    if(pdlgS->DoModal() == IDOK)
      {
      pdlgS->vGet_water_amount(&m_pdPatient);
      m_bReportChanged = pdlgS->bChanged();
      }
    delete pdlgS;
    }
  }

/********************************************************************
vViewAmtWater - Provides the processing for the CMainFrame
                message handler for viewing the amount of water ingested.
                 - Display the amount of water ingested dialog box
                   for viewing.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vViewAmtWater() 
  {
  CWaterLoad *pdlg;
  CStimulationMedium *pdlgR;

  if(theApp.m_ptStudyType == PT_RESEARCH)
    {
    pdlgR = new CStimulationMedium(this, SM_VIEW);
    pdlgR->DoModal();
    delete pdlgR;
    }
  else
    { // Standard version
    pdlg = new CWaterLoad(this, SM_VIEW, TYPE_AMT_OF_WATER);
    pdlg->DoModal();
    delete pdlg;
    }
  }

/********************************************************************
vReportFacInfo - Provides the processing for the CMainFrame
                 message handler for viewing/changing the facility information.
                   - Display the facility information dialog box
                     for viewing/changing.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vReportFacInfo()
  {
  CFacilityInfo *pdlg;

  vGet_view();
  m_p10MinView->vShow_logo(false);
  pdlg = new CFacilityInfo(this);
  if(pdlg->DoModal() == IDOK)
    {
    m_bReportChanged = pdlg->bChanged();
    }
  delete pdlg;
  }

/********************************************************************
vReportConfigure - Provides the processing for the CMainFrame
                   message handler for viewing/changing the report
                   configuration.
                     - Display the report configuration dialog box
                       for viewing/changing.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vReportConfigure()
  {
  CConfigReport *pdlg;

  vGet_view();
  m_p10MinView->vShow_logo(false);
  pdlg = new CConfigReport(this);
  if(pdlg->DoModal() == IDOK)
    {
    m_bReportChanged = pdlg->bReport_changed();
    }
  delete pdlg;
  }

/********************************************************************
vReportSpreadsheetOfDataSheet - Provides the processing for the CMainFrame
                                message handler for creating a spreadsheet
                                from the Data Sheet.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vReportSpreadsheetOfDataSheet()
  {
  CSpreadSheet *pss;

  pss = new CSpreadSheet(this);
  delete pss;

  }

/********************************************************************
vViewRespiration - Provides the processing for the CMainFrame
                   message handler for viewing the respiration.
                     - Display the respiration dialog box for viewing.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vViewRespiration()
  {

  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard study.
    CRespEdit *pdlg;
  
    pdlg = new CRespEdit(this, SM_VIEW);
    pdlg->vSet_respiration(&m_pdPatient.uRespRate[PERIOD_10]);
    pdlg->DoModal();
    delete pdlg;
    }
  else
    { // Research study
    CRespEditResearch *pdlg;
  
    pdlg = new CRespEditResearch(this, SM_VIEW);
    pdlg->DoModal();
    delete pdlg;
    }
  }

/********************************************************************
vEditRespiration - Provides the processing for the CMainFrame
                   message handler for editing the respiration.
                     - Display the respiration dialog box for editing.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vEditRespiration()
  {

  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard study.
    CRespEdit *pdlg;
  
    pdlg = new CRespEdit(this, SM_EDIT);
    pdlg->vSet_respiration(&m_pdPatient.uRespRate[PERIOD_10]);
    if(pdlg->DoModal() == IDOK)
      pdlg->vGet_respiration(&m_pdPatient.uRespRate[PERIOD_10]);
    delete pdlg;
    }
  else
    { // Research study
    CRespEditResearch *pdlg;
  
    pdlg = new CRespEditResearch(this, SM_EDIT);
    if(pdlg->DoModal() == IDOK)
      pdlg->vGet_respiration(&m_pdPatient.uRespRate[PERIOD_10]);
    delete pdlg;
    }
  }

/********************************************************************
vBackup - Provides the processing for the CMainFrame
          message handler for performing a backup or restore.
            - Display the performing a backup or restore dialog box.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vBackup()
  {
  CBckupRest *pdlg;

  vGet_view();
  m_p10MinView->vShow_logo(false);
  pdlg = new CBckupRest(this);
  pdlg->DoModal();
  delete pdlg;
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
bOk_to_close - Determine if its OK to exit the program or select another
               patient based on whether the user needs to save the current
               patient information/study data.

  inputs: None.

  return: true if its OK to exit the program or select another patient.
          false if its not OK to exit the program or select another patient.
********************************************************************/
bool CEGGSASDoc::bOk_to_close()
  {
  bool bRet;

  if(iCheck_to_Save() != IDCANCEL)
    bRet = true;
  else
    bRet = false;

  return(bRet);
  }

/********************************************************************
iRead_period - Open the patient data file and read in the entire 10
               minute period.

  inputs: Period to read in.
          Pointer to a data array that gets filled in with the data from
            the file.

  return: SUCCESS if the data was read in.
          FAIL if there was an error.
********************************************************************/
short int CEGGSASDoc::iRead_period(short unsigned uPeriod, float *pfData)
  {
  CString cstrErrMsg, cstrErrTitle, cstrFile;
  DATA_FILE_HEADER dhfHdr;
  BASELINE_TRAILER btTrailer;
  unsigned uNumBytesRead, uNumBytesToRead, uIndex, uIndex1;
  long lFileOffSet, lDataStart;
  short int iRet;
  float fMinutes;

//// TEMPORARY
// Old data file contains integers.
  short int iData[DATA_POINTS_PER_MIN_1 * 10 * sizeof(short int) * 4];
  memset(iData, 0, sizeof(iData));
//// END OF TEMPORARY

  if(m_pcfDataFile == NULL)
    {
//#ifdef EGGSAS_READER
//#if EGGSAS_READER > READER_NONE // Version 2.09i
    CString m_cstrDataFilePath, cstrPath;
    int iIndex;

    //cstrPath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_DATAPATH, "");
    cstrPath = g_pConfig->cstrGet_item(CFG_DATAPATH);

    // We have to create the data file name if user is accessing an exported database.
    m_cstrDataFilePath = m_pDB->cstrGet_db_path();
    cstrFile = m_pdPatient.cstrFile;
    if(m_cstrDataFilePath != cstrPath)
      { // User on remote database.  Redo the data file name.
      if((iIndex = m_pdPatient.cstrFile.ReverseFind('\\')) > 0)
        cstrFile = m_cstrDataFilePath
                   + m_pdPatient.cstrFile.Right(m_pdPatient.cstrFile.GetLength() - iIndex);
      }
//#else // Version 2.09i
//    cstrFile = m_pdPatient.cstrFile;
//#endif
    if((iRet = iOpen_file_get_hdr(&m_pcfDataFile, cstrFile, &dhfHdr, true))
    == SUCCESS)
      {
      if(bValid_file_hdr(&dhfHdr) == true)
        { // EGGSAP/EGGSAS data file,  Calculate start of data.
        m_lFileDataStart = sizeof(dhfHdr) + dhfHdr.uiPatientDataSize;
        m_uDataFileVer = dhfHdr.uiDataFileVer;
          // Further validation. If standard version and research file, not OK.
        if(theApp.m_ptPgmType == PT_STD_RSCH)
          { // research/standard version
          if(dhfHdr.uiDataFileVer == DATA_FILE_VER_R
          && strcmp(dhfHdr.szProgName, PROGRAM_NAME_R1) == 0)
            theApp.m_ptStudyType = PT_RESEARCH;
          else
            theApp.m_ptStudyType = PT_STANDARD;
          }
        else if(theApp.m_ptPgmType == PT_RESEARCH)
          { // Research version.
          if(dhfHdr.uiDataFileVer == DATA_FILE_VER 
          && strcmp(dhfHdr.szProgName, PROGRAM_NAME) == 0)
            { // Standard version file.
            //cstrErrMsg.LoadString(IDS_INV_STUDY_FOR_VERSION);
            cstrErrMsg = g_pLang->cstrLoad_string(ITEM_G_INV_STUDY_FOR_VERSION);
            iRet = FAIL;
            }
          }
        else
          { // Standard version
          if(dhfHdr.uiDataFileVer == 6 && strcmp(dhfHdr.szProgName, PROGRAM_NAME_R1) == 0)
            { // Research version file.
            //cstrErrMsg.LoadString(IDS_INV_STUDY_FOR_VERSION);
            cstrErrMsg = g_pLang->cstrLoad_string(ITEM_G_INV_STUDY_FOR_VERSION);
           iRet = FAIL;
            }
//          else
//            theApp.m_ptStudyType = PT_STANDARD;
          }
        }
      else
        { // Invalid header.
        //cstrErrMsg.Format(IDS_ERR_INVALID_HDR, m_pcfDataFile->GetFileName());
        cstrErrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_INVALID_HDR), m_pcfDataFile->GetFileName());
        iRet = FAIL;
        }
      }
    }
  else
    iRet = SUCCESS;
  if(iRet == SUCCESS)
    {
    if(m_uDataFileVer >= DATA_FILE_VER)
      { // Version II.  Calculate the number of bytes read because baseline may
        // be extended an extra 10 minutes.
      //// TODO different calc for research version.
      if(theApp.m_ptStudyType == PT_STANDARD)
        uNumBytesToRead = (unsigned)m_pdPatient.fPeriodLen[uPeriod] * BYTES_PER_MINUTE_1 * 2;
      else
        { // Research version.
        if(uPeriod == 0)
          uNumBytesToRead = m_pdPatient.uDataPtsInBaseline * sizeof(float) * 2;
        else
          { ////  TODO allow for half minutes.
            // If period hasn't been analyzed, then this is the last period and get the
            // number of data samples from m_pdPatient.
          if(m_pdPatient.fPeriodLen[uPeriod] == INV_PERIOD_LEN)
            { // Period hasn't been analyzed.
            if(uPeriod == 1) // First period.
              uNumBytesToRead = m_pdPatient.uPostStimDataPoints * sizeof(float);
            else if(uPeriod == m_pdPatient.uNumPeriods) // Last period.
              {
              uNumBytesToRead = (m_pdPatient.uPostStimDataPoints * sizeof(float))
                - (unsigned)m_pcfDataFile->GetPosition();
              }
            else
              { // A middle period.  Has just been inserted.
                // Previous period must have been analyzed.
                // Conditions: Current period is analyzed, current period not analyzed.
              if(m_pdPatient.fPeriodLen[uPeriod + 1] == INV_PERIOD_LEN)
                fMinutes = m_p10MinView->m_fPeriodStartMin;
              else
                fMinutes = m_pdPatient.fPeriodStart[uPeriod + 1];
              fMinutes -= (m_pdPatient.fPeriodStart[uPeriod - 1]
                           + m_pdPatient.fPeriodLen[uPeriod - 1]);
              uNumBytesToRead = (unsigned)(fMinutes * (float)(BYTES_PER_MINUTE_1 * 2));
              }
            }
          else
            { // Period has been analyzed.
            uNumBytesToRead = (unsigned)
                (m_pdPatient.fPeriodLen[uPeriod] * (float)(BYTES_PER_MINUTE_1 * 2));
            }
          }
        }
      m_uDataPointsInPeriod = uNumBytesToRead / sizeof(float);
      }
    else
      uNumBytesToRead = DATA_POINTS_PER_MIN_1 * 10 * sizeof(short int) * 4;
      //// TODO different calc for research version.
      //// For the first post-stimulation period, this is the start of a
      //// minute regardless of where the baseline ended.
    if(theApp.m_ptStudyType == PT_STANDARD)
      lDataStart = (long)m_pdPatient.fPeriodStart[uPeriod] * (BYTES_PER_MINUTE_1 * 2);
    else
      { // Research version
      if(uPeriod == 0)
        lDataStart = 0; //  Baseline.
      else
        { // Data start for post-stimulation data = 
          //    Number of points in the baseline
          //    + baseline trailer.
          //    + number of points in all periods until this one.
        lDataStart = m_pdPatient.uDataPtsInBaseline * sizeof(float) * 2;
        lDataStart += (long)(sizeof(BASELINE_TRAILER));
        lDataStart += (long)(
            m_pdPatient.fPeriodStart[uPeriod] * (float)(BYTES_PER_MINUTE_1 * 2));
        }
      }
    if(m_uDataFileVer < DATA_FILE_VER)
      { // For version I files, skip the minutes for post-water periods specified
        // in the INI file.
      int iPostwaterStart;
      //iPostwaterStart = AfxGetApp()->GetProfileInt(INI_SEC_VERSIONI, INI_ITEM_POSTWATER_START, 20);
      iPostwaterStart = g_pConfig->iGet_item(CFG_POSTWATERSTART);
      if(uPeriod > 0)
        lDataStart += ((iPostwaterStart - 10) * (256 * sizeof(short int) * 4)); //(BYTES_PER_MINUTE_1 * 4));
      }
      // Position the file pointer to the start of the epoch.
    lFileOffSet = m_lFileDataStart + lDataStart;
    try
      {
      m_pcfDataFile->Seek(lFileOffSet, CFile::begin);
      iRet = SUCCESS;
      }
    catch(CFileException *pfe)
      { // Got an error trying to position the file pointer.
      //cstrErrMsg.Format(IDS_ERR_POSITION_FILE, uPeriod, 
      //                  m_pcfDataFile->GetFileName(), strerror(pfe->m_lOsError));
      cstrErrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_POSITION_FILE), uPeriod, 
                        m_pcfDataFile->GetFileName(), strerror(pfe->m_lOsError));
      pfe->Delete();
      iRet = FAIL;
      }
    if(iRet == SUCCESS)
      { // Got to the right place in the file, now read in the data.
      try
        { // Read an epoch of data from the file.
        if(m_uDataFileVer >= DATA_FILE_VER)
          uNumBytesRead = m_pcfDataFile->Read(m_fData, uNumBytesToRead);
        else
          uNumBytesRead = m_pcfDataFile->Read(iData, uNumBytesToRead);

        if(theApp.m_ptStudyType == PT_RESEARCH)
          {  // Research version
            // If this is the baseline we have just read, find out how
            // many minutes are in the study.
          if(m_uCurrentPeriod == PERIOD_BL)
            { // Baseline period.  Read in the baseline trailer.
            m_pcfDataFile->Read(&btTrailer, sizeof(BASELINE_TRAILER));
            m_fStudyLen = (float)btTrailer.lDataSamples
                          / (float)(DATA_POINTS_PER_MIN_1 * 2);
            }
          else
            { // Post-stimulation period.
            m_fStudyLen = (float)m_pdPatient.uPostStimDataPoints
                          / (float)(DATA_POINTS_PER_MIN_1 * 2);
            }
          }
        m_pdPatient.fMinEGG = (float)0.0;
        m_pdPatient.fMaxEGG = (float)0.0;
        m_pdPatient.fMinResp = (float)0.0;
        m_pdPatient.fMaxResp = (float)0.0;
        if(m_uDataFileVer >= DATA_FILE_VER)
          {  // Version II, only 2 channels. Floating point numbers
          m_pdPatient.uNumDataPts = uNumBytesRead / sizeof(float);
          for(uIndex = 0; uIndex < m_pdPatient.uNumDataPts; ++uIndex)
            {
            if(m_fData[uIndex] > m_pdPatient.fMaxEGG)
              { // New EGG maximum.
              if(m_pdPatient.fMaxEGG == (float)0.0)
                m_pdPatient.fMinEGG = m_fData[uIndex];
              m_pdPatient.fMaxEGG = m_fData[uIndex];
              }
            else if(m_fData[uIndex] < m_pdPatient.fMinEGG)
              m_pdPatient.fMinEGG = m_fData[uIndex];

            if(m_fData[++uIndex] > m_pdPatient.fMaxResp)
              { // New Respiration maximum.
              if(m_pdPatient.fMaxResp == (float)0.0)
                m_pdPatient.fMinResp = m_fData[uIndex];
              m_pdPatient.fMaxResp = m_fData[uIndex];
              }
            else if(m_fData[uIndex] < m_pdPatient.fMinResp)
              m_pdPatient.fMinResp = m_fData[uIndex];
            }
          }
        else
          { // Version I.  Convert integers to floats.
            // Read the scale factor from the INI file.
          CString cstrScaleFactor;
          float fScaleFactor;

          //cstrScaleFactor = AfxGetApp()->GetProfileString(INI_SEC_VERSIONI, INI_ITEM_SCALEFACTOR, "1.0");
          cstrScaleFactor = g_pConfig->cstrGet_item(CFG_SCALEFACTOR);
          fScaleFactor = (float)atof(cstrScaleFactor);
          m_pdPatient.uNumDataPts = uNumBytesRead / sizeof(short int);
          for(uIndex = 0, uIndex1 = 0; uIndex1 < m_pdPatient.uNumDataPts; )
            {
            m_fData[uIndex] = (float)iData[uIndex1] * fScaleFactor;
            if(m_fData[uIndex] > m_pdPatient.fMaxEGG)
              { // New EGG maximum.
              if(m_pdPatient.fMaxEGG == (float)0.0)
                m_pdPatient.fMinEGG = m_fData[uIndex];
              m_pdPatient.fMaxEGG = m_fData[uIndex];
              }
            else if(m_fData[uIndex] < m_pdPatient.fMinEGG)
              m_pdPatient.fMinEGG = m_fData[uIndex];
            m_fData[++uIndex] = (float)iData[++uIndex1] * fScaleFactor;
            if(m_fData[uIndex] > m_pdPatient.fMaxResp)
              { // New Respiration maximum.
              if(m_pdPatient.fMaxResp == (float)0.0)
                m_pdPatient.fMinResp = m_fData[uIndex];
              m_pdPatient.fMaxResp = m_fData[uIndex];
              }
            else if(m_fData[uIndex] < m_pdPatient.fMinResp)
              m_pdPatient.fMinResp = m_fData[uIndex];
            uIndex1 += 3;
            ++uIndex;
            }
          }
        m_pdPatient.uNumDataPts = uIndex;
        }
      catch(CFileException *pfe)
        { // Got a file read error.
        if(pfe->m_cause != CFileException::endOfFile)
          { // Not an end of file.
          //cstrErrMsg.Format(IDS_ERR_READ_FILE, m_pcfDataFile->GetFileName(), 
          //                 strerror(pfe->m_lOsError));
          cstrErrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_READ_FILE), m_pcfDataFile->GetFileName(), 
                           strerror(pfe->m_lOsError));
          }
        else
          cstrErrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_UNEXPECTED_EOF),
                            m_pcfDataFile->GetFileName());
          //cstrErrMsg.Format(IDS_ERR_UNEXPECTED_EOF, m_pcfDataFile->GetFileName());
        iRet = FAIL;
        pfe->Delete();
        }
      }
    }
  if(iRet == FAIL)
    { // Got an error, display an error message.
    //cstrErrTitle.LoadString(IDS_ERROR1);
    cstrErrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    theApp.vLog(cstrErrMsg);
    m_p10MinView->MessageBox(cstrErrMsg, cstrErrTitle);
    }
  return(iRet);
  }

/********************************************************************
iOpen_file_get_hdr - Open a patient data file and read in the file header.
                         
  inputs: Pointer to a pointer to a CFile object for the open data file.
            The pointer to the object points to the CFile object upon exit.
          Reference to a CString object containing the file name to open.
          Pointer to a DATA_FILE_HEADER structure that gets file header
            information.
          Flag to indicate if error messages should be displayed in a message box.
            true to display error messages, otherwise false.

  return: SUCCESS if the file was opened and read.
          FAIL if there was an error.
********************************************************************/
short int CEGGSASDoc::iOpen_file_get_hdr(CFile **ppcfFile, CString &cstrFile,
DATA_FILE_HEADER *pdhfHdr, bool bDisplayErr)
  {
  CString cstrErrMsg, cstrErrTitle;
  int iRet;

  m_fStudyLen = (float)0;
  try
    { // Open the file.
    *ppcfFile = new CFile(cstrFile, CFile::modeRead | CFile::shareDenyWrite);
    iRet = SUCCESS;
    }
  catch(CFileException *pfe)
    { // Got a file open error.
    //cstrErrMsg.Format(IDS_ERR_OPEN_FILE1, cstrFile, strerror(pfe->m_lOsError));
    cstrErrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_OPEN_FILE), cstrFile, strerror(pfe->m_lOsError));
    pfe->Delete();
    *ppcfFile = NULL;
    iRet = FAIL;
    }
  if(iRet == SUCCESS)
    { // Check to see if this is a EGGSAP data file or SPEC9 data file.
    try
      { // Read an epoch of data from the file.
      if((*ppcfFile)->Read(pdhfHdr, sizeof(DATA_FILE_HEADER)) < sizeof(DATA_FILE_HEADER))
        { // Data file is too small.
        //cstrErrMsg.Format(IDS_NO_DATA_IN_FILE, cstrFile);
        cstrErrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_NO_DATA_IN_FILE), cstrFile);
        iRet = FAIL;
        }
      }
    catch(CFileException *pfe)
      { // Got a file read error.
      if(pfe->m_cause != CFileException::endOfFile)
        { // Not an end of file.
        //cstrErrMsg.Format(IDS_ERR_READ_FILE, cstrFile, strerror(pfe->m_lOsError));
        cstrErrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_READ_FILE), cstrFile, strerror(pfe->m_lOsError));
        }
      else
        cstrErrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_UNEXPECTED_EOF), cstrFile);
        //cstrErrMsg.Format(IDS_ERR_UNEXPECTED_EOF, cstrFile);
      iRet = FAIL;
      pfe->Delete();
      }
    }
  if(iRet != SUCCESS)
    { // Got an error, display an error message.
    theApp.vLog(cstrErrMsg);
    m_cstrErrMsg = cstrErrMsg;
    if(bDisplayErr == true)
      {
      //cstrErrTitle.LoadString(IDS_ERROR1);
      cstrErrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
      m_p10MinView->MessageBox(cstrErrMsg, cstrErrTitle);
      }
    }
  return(iRet);
  }

/********************************************************************
bValid_file_hdr - Determine if the data file is valid for a program
                  and data file version.

                  Valid programs and data file versions.
                  Program name     data file version
                  ------------     -----------------
                    EGGSAP                 1
                    EGGSAS                 2
                    EGGSAS-4               3
                    EGGSAS-4               4
///// FOR VERSION II, this is 3CPM.
                         
  inputs: Pointer to a DATA_FILE_HEADER structure containing the data
            file header information.

  return: TRUE if the data file is valid.
          FALSE if the data file is not valid.
********************************************************************/
bool CEGGSASDoc::bValid_file_hdr(DATA_FILE_HEADER *pdhfHdr)
  {
  bool bRet;

  if(pdhfHdr->iHdrStart == DATA_FILE_HDR_START
  && ((pdhfHdr->uiDataFileVer == 1 && strcmp(pdhfHdr->szProgName, PROGRAM_NAME_1) == 0)
  || (pdhfHdr->uiDataFileVer == 2 && strcmp(pdhfHdr->szProgName, PROGRAM_NAME_2) == 0)
  || (pdhfHdr->uiDataFileVer == 3 && strcmp(pdhfHdr->szProgName, PROGRAM_NAME_3) == 0)
  || (pdhfHdr->uiDataFileVer == 4 && strcmp(pdhfHdr->szProgName, PROGRAM_NAME_3) == 0)
  || (pdhfHdr->uiDataFileVer == 5 && strcmp(pdhfHdr->szProgName, PROGRAM_NAME) == 0)
  || (pdhfHdr->uiDataFileVer == 6 && strcmp(pdhfHdr->szProgName, PROGRAM_NAME_R1) == 0)))
    bRet = true;
  else
    bRet = false;
  return(bRet);
  }

/********************************************************************
bGet_data_graph - Read in the data for the current period and graph it.
                  If the input indicates to analyze the data, analyze it
                  before graphing it.
                         
  inputs: true - analyze the data, false - don't analyze the data
          The menu used to get here (VIEW, EDIT(edit and analyze))

  return: true if graphed the period, otherwise false.
********************************************************************/
bool CEGGSASDoc::bGet_data_graph(bool bAnalyze, unsigned uMenuType)
  {
  bool bGoToPeriod = true;

  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Research study
    CString cstrMsg, cstrTitle;
    int iSts;

    if(m_uPreviousPeriod != PERIOD_NONE && m_uPreviousPeriod != m_uCurrentPeriod)
      { // Periods changed.
        // If period or good minutes changed and not saved, display
        // error message and ask do you want to save.
      if(m_p10MinView->m_bSetGoodMinutes == FALSE)
//      && m_pdPatient.fGoodMinStart[m_uCurrentPeriod] != INV_MINUTE)
        {
        if(g_bAsciiDataFile == false)
          {
          //cstrTitle.LoadString(IDS_ERROR1);
          cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
          //cstrMsg.LoadString(IDS_GOOD_MINUTES_UNCHECKED);
          cstrMsg = g_pLang->cstrLoad_string(ITEM_G_GOOD_MINUTES_UNCHECKED);
          iSts = m_p10MinView->MessageBox(cstrMsg, cstrTitle, MB_OK);
          bGoToPeriod = false; // Stay in current period.
          }
        else
          bGoToPeriod = true; // Ok to go to current period for data dump version.
        }
      if(bGoToPeriod == true && m_p10MinView->m_bSetPeriodLength == FALSE)
        {
        //cstrTitle.LoadString(IDS_ERROR1);
        cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
        //cstrMsg.LoadString(IDS_PERIOD_LENGTH_UNCHECKED);
        cstrMsg = g_pLang->cstrLoad_string(ITEM_G_PERIOD_LENGTH_UNCHECKED);
//        cstrMsg = "The Period Length box was left unchecked.  You must return and check the box before moving on to another period.";
//        cstrMsg = "You have NOT set the .\n\nSelect Ok to revert to previous values and go to selected period.\nSelect Cancel to stay in the current period.";
        iSts = m_p10MinView->MessageBox(cstrMsg, cstrTitle, MB_OK);
        bGoToPeriod = false; // Stay in current period.
        }
      if(bGoToPeriod == false)
        { // Stay in current period.
//        float fStartMinute, fEndMinute;
        m_uCurrentPeriod = m_uPreviousPeriod;
        //m_p10MinView->UpdateData(TRUE);
        //m_p10MinView->bValidate_good_minutes(&fStartMinute, &fEndMinute,
        //  m_p10MinView->m_cstrStartGoodMin, m_p10MinView->m_cstrEndGoodMin);
        //m_pdPatient.fGoodMinStart[m_uCurrentPeriod] = fStartMinute;
        //m_pdPatient.fGoodMinEnd[m_uCurrentPeriod] = fEndMinute;

//        bGoToPeriod = true;
        }
      else //if(bGoToPeriod == true)
        { // Check/uncheck checkboxes for period and good minutes.
        m_p10MinView->vInit_period_good_mins();
        }
      }
    }

  if(bGoToPeriod == true)
    { // Read in and graph the baseline period.
    if(iRead_period(m_uCurrentPeriod, m_fData) == SUCCESS)
      vGraph_data(bAnalyze, uMenuType); // Read in the file, now graph it.
    }
  return(bGoToPeriod);
  }

/********************************************************************
vGraph_data - graph the data for the current period.  Assumes the data
              has already been read from the file.
              If the input indicates to analyze the data, analyze it
              before graphing it.
                         
  inputs: true - analyze the data, false - don't analyze the data
          The menu used to get here (VIEW, EDIT(edit and analyze))

  return: None.
********************************************************************/
void CEGGSASDoc::vGraph_data(bool bAnalyze, unsigned uMenuType)
  {

  if(theApp.m_ptStudyType == PT_RESEARCH)
    {  // Research version
    m_p10MinView->vInit_research_data();
    }
  else
    {
    if(bAnalyze == true)
      { // Analyze the data before graphing it.
      if(m_pArtifact != NULL)
        { // Identify artifact for:
          //   - baseline period
          //   - Periods other than baseline, but only if the baseline period
          //     has been analyzed.
        if(m_uCurrentPeriod == PERIOD_BL
        || (m_uCurrentPeriod != PERIOD_BL
        && (m_pdPatient.bPeriodsAnalyzed[PERIOD_BL]) == true))
          m_pArtifact->vArtifact_id(m_uCurrentPeriod, m_fData, m_pdPatient.uNumDataPts);
        }
      }
    }
  m_p10MinView->vSet_patient_info(&m_pdPatient);
  m_p10MinView->vGraph_period(m_uCurrentPeriod, m_fData, &m_pdPatient, uMenuType);
  m_p10MinView->vEnable_fields(TRUE);
  UpdateAllViews(NULL);
  }

/********************************************************************
vInit_patient_data - Initialize all data associated with a patient.
                         
  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vInit_patient_data()
  {
  CTime ctCurrent;
//  CEvnt *pEvent;
  short int iIndex;

    // Initialize the PATIENT_DATA structure for the current patient.
  m_pdPatient.lPatientID = INV_LONG;
	m_pdPatient.cstrLastName.Empty();
	m_pdPatient.cstrFirstName.Empty();
  m_pdPatient.cstrMI.Empty();
	m_pdPatient.cstrAddress1.Empty();
	m_pdPatient.cstrAddress2.Empty();
	m_pdPatient.cstrCity.Empty();
	m_pdPatient.cstrDateOfBirth.Empty();
	m_pdPatient.cstrHospitalNum.Empty();
	m_pdPatient.cstrPhone.Empty();
  m_pdPatient.bUseResrchRefNum = 0;
	m_pdPatient.cstrResearchRefNum.Empty();
	m_pdPatient.cstrSocialSecurityNum.Empty();
	m_pdPatient.cstrState.Empty();
	m_pdPatient.cstrZipCode.Empty();
    // Pre-study information
  ctCurrent = CTime::GetCurrentTime();
  m_pdPatient.cstrMealTime = cstrGet_hrs_mins(ctCurrent);
	m_pdPatient.cstrAttendPhys.Empty();
	m_pdPatient.cstrIFPComments.Empty();
	m_pdPatient.cstrReferPhys.Empty();
	m_pdPatient.cstrTechnician.Empty();
	m_pdPatient.bBloating = FALSE;
	m_pdPatient.bEarlySateity = FALSE;
	m_pdPatient.bEructus = FALSE;
	m_pdPatient.bGerd = FALSE;
	m_pdPatient.bNausea = FALSE;
	m_pdPatient.bPPFullness = FALSE;
	m_pdPatient.bVomiting = FALSE;
  m_pdPatient.bDelayedGastricEmptying = FALSE;
    // Water
  m_pdPatient.uWaterAmount = 0; // Amount of water
  m_pdPatient.uWaterUnits = 0; // Units for the amount of water.
    // Study data
  m_pdPatient.lStudyID = INV_LONG;
  m_pdPatient.cstrDate.Empty(); // Date of study.
  m_pdPatient.cstrFile.Empty(); // Name of file.
  m_pdPatient.uNumDataPts = 0; // Number of data points read from file for period.
  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard version
    //theApp.vLog("@@@ DEBUG: vInit_patient_data: WaterLoad study type");
    for(iIndex = 0; iIndex < MAX_MANUAL_MODE_PERIODS; ++iIndex)
      {
      m_pdPatient.fPeriodLen[iIndex] = (float)10;
      m_pdPatient.fPeriodStart[iIndex] = (float)(iIndex * 10);
      m_pdPatient.fGoodMinStart[iIndex] = INV_MINUTE;
      m_pdPatient.fGoodMinEnd[iIndex] = INV_MINUTE;
      m_pdPatient.uRespRate[iIndex] = 0;
      }
    }
  else
    { // Research version
    //theApp.vLog("@@@ DEBUG: vInit_patient_data: Research study type");
    for(iIndex = 0; iIndex < MAX_TOTAL_PERIODS; ++iIndex)
      {
      m_pdPatient.fPeriodLen[iIndex] = (float)INV_PERIOD_LEN;
      m_pdPatient.fPeriodStart[iIndex] = (float)INV_PERIOD_START;
      m_pdPatient.fGoodMinStart[iIndex] = INV_MINUTE;
      m_pdPatient.fGoodMinEnd[iIndex] = INV_MINUTE;
      m_pdPatient.uRespRate[iIndex] = 0;
      }
    }
  m_pdPatient.fMaxEGG = (float)0.0;
  m_pdPatient.fMinEGG = (float)0.0;
  m_pdPatient.fMaxResp = (float)0.0;
  m_pdPatient.fMinResp = (float)0.0;
  m_pdPatient.fTestMaxEGG = (float)0.0;
  m_pdPatient.fTestMinEGG = (float)0.0;
  m_pdPatient.fMeanBaseline = INV_FLOAT;

  vReset_periods_analyzed();
  m_pdPatient.bExtra10Min = FALSE;
  m_pdPatient.bManualMode = FALSE;

  m_pdPatient.bUserDiagnosis = FALSE;
  m_pdPatient.cstrDiagnosis.Empty();
  m_pdPatient.cstrSuggestedDiagnosis.Empty();

    // Close the patient file
  // Currently have an open data file.  Close it.
  vClose_data_file(false);

    // Reset data in the form view.
  if(m_p10MinView != NULL)
    m_p10MinView->vInit();

    // Clean up the artifact identification object.
  if(m_pArtifact != NULL)
    {
    delete m_pArtifact;
    m_pArtifact = NULL;
    }
//  if(theApp.m_ptStudyType == PT_STANDARD)
  m_pArtifact = new CArtifactID();

  m_pdPatient.bHavePatientInfo = false;
  m_pdPatient.uNumPeriods = 0;
  m_pdPatient.cstrStimMedDesc.Empty();
  m_pdPatient.uDataPtsInBaseline = 0;
  m_pdPatient.bPause = FALSE;
  m_pdPatient.uPostStimDataPoints = 0;
#ifndef _200_CPM_HARDWARE
  m_pdPatient.uFrequency = 15;
#endif
    // Free all events from the research version.  It is always done
    // because the previous study may have been a research study.
  vFree_event_list(m_pdPatient.Events.m_PSEvents);
  vFree_event_list(m_pdPatient.Events.m_BLEvents);

  m_pdPatient.uPatientType = PAT_HUMAN; // Default patient type.
  }

void CEGGSASDoc::vInit_study_data_for_study_type()
  {
  short int iIndex;

  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard version
    //theApp.vLog("@@@ DEBUG: vInit_patient_data: WaterLoad study type");
    for(iIndex = 0; iIndex < MAX_MANUAL_MODE_PERIODS; ++iIndex)
      {
      m_pdPatient.fPeriodLen[iIndex] = (float)10;
      m_pdPatient.fPeriodStart[iIndex] = (float)(iIndex * 10);
      m_pdPatient.fGoodMinStart[iIndex] = INV_MINUTE;
      m_pdPatient.fGoodMinEnd[iIndex] = INV_MINUTE;
      m_pdPatient.uRespRate[iIndex] = 0;
      }
    }
  else
    { // Research version
    //theApp.vLog("@@@ DEBUG: vInit_patient_data: Research study type");
    for(iIndex = 0; iIndex < MAX_TOTAL_PERIODS; ++iIndex)
      {
      m_pdPatient.fPeriodLen[iIndex] = (float)INV_PERIOD_LEN;
      m_pdPatient.fPeriodStart[iIndex] = (float)INV_PERIOD_START;
      m_pdPatient.fGoodMinStart[iIndex] = INV_MINUTE;
      m_pdPatient.fGoodMinEnd[iIndex] = INV_MINUTE;
      m_pdPatient.uRespRate[iIndex] = 0;
      }
    }
  }

/********************************************************************
vGet_view - Get a pointer to the 10 minute view.
                         
  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vGet_view()
  {
  POSITION pos;
    // Get a pointer to the view.
  pos = GetFirstViewPosition();
  m_p10MinView = (CTenMinView *)GetNextView(pos);
  }

/********************************************************************
vFileSave - Save the patient, patient info and study data in the
            appropriate database tables.
                         
  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vFileSave()
  {
  short int iSts, iIndex;

  if(m_pDB != NULL)
    {
      // Display the hourglass cursor.
    SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);
    if(m_pDB->iSave_patient(&m_pdPatient) == SUCCESS
    && m_pDB->iSave_patient_info(&m_pdPatient) == SUCCESS)
      {
      iSts = SUCCESS;
        // Save the study information only if there there is a
        // patient data file (e.g. don't save the study info if
        // the current study was cancelled.
      if(m_bHaveFile == TRUE)
        {
        iSts = m_pDB->iSave_study_data(&m_pdPatient);
        if(iSts == SUCCESS)
          { // Save the baseline events.
          for(iIndex = 0; iIndex < m_pdPatient.Events.m_BLEvents.GetCount(); ++iIndex)
            {
            iSts = m_pDB->iSave_event(m_pdPatient.lStudyID,
                                      m_pdPatient.Events.m_BLEvents[iIndex]);
            if(iSts != SUCCESS)
              break;
            }
          if(iSts == SUCCESS)
            { // Save the post-stimulation events.
            for(iIndex = 0; iIndex < m_pdPatient.Events.m_PSEvents.GetCount(); ++iIndex)
              {
              iSts = m_pDB->iSave_event(m_pdPatient.lStudyID,
                                        m_pdPatient.Events.m_PSEvents[iIndex]);
              if(iSts != SUCCESS)
                break;
              }
            }
          vCleanup_events_db();
          }
        }
      if(iSts == SUCCESS)
        { // All data base information saved.
        vCopy_patient(&m_pdSavedPatient, &m_pdPatient);
        }
      }
      // Restore the arrow cursor.
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    ShowCursor(TRUE);
    }
  }

/********************************************************************
cstrGet_date_of_study - Get the current date in the format: mm/dd/yyyy
                         
  inputs: None.

  return: CString object containing the current date.
********************************************************************/
CString CEGGSASDoc::cstrGet_date_of_study()
  {
  CTime ctDate;
  CString cstrDate;

  ctDate = CTime::GetCurrentTime();
  cstrDate = ctDate.Format("%m/%d/%Y");
#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
#if EGGSAS_READER == READER
    cstrDate = cstrEncode_study_date(cstrDate); // Endode the study date.
#else
  if(g_pConfig->m_bHideStudyDates == true)
    cstrDate = cstrEncode_study_date(cstrDate); // Endode the study date.
#endif
#endif
  return(cstrDate);
  }

/********************************************************************
iInit_data_file - Create the patient data file name.
                  If it already exists, ask if it should be overwritten.
                  Create the file and write the file header to it.
                         
  inputs: None.

  return: SUCCESS if the file is created.
          FAIL if the file already exists and not to be overwritten or
            there is a file error.
********************************************************************/
short int CEGGSASDoc::iInit_data_file()
  {
  CString cstrMsg, cstrErrTitle;
  DATA_FILE_HEADER dfhFileHdr;
  short int iRet = SUCCESS;

    // Create the file name.
  m_pdPatient.cstrFile = cstrCreate_data_file_name(&m_pdPatient, FILE_TYPE_EGG);
  try
    { // Open the file to see if it already exists.
    m_pcfDataFile = new CFile(m_pdPatient.cstrFile, CFile::modeRead | CFile::shareDenyWrite);
      // The file already exists, ask to overwrite it.
    //cstrMsg.Format(IDS_OVERWRITE_FILE, m_pdPatient.cstrFile);
    cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_OVERWRITE_FILE), m_pdPatient.cstrFile);
    //cstrErrTitle.LoadString(IDS_WARNING);
    cstrErrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
    if(AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErrTitle,
    MB_YESNO | MB_ICONQUESTION) == IDYES)
      { // User wants to overwrite the file.  Close the file and set a flag
        // to create it.
        // Currently have an open data file.  Close it.
        vClose_data_file(false);
      }
    else
      { // User doen't want to overwrite the data file.
      // Currently have an open data file.  Close it.
      vClose_data_file(false);
      iRet = FAIL;
      }
    }
  catch(CFileException *pfe)
    { // File doesn't exist. Not an error.
    pfe->Delete();
    delete m_pcfDataFile;
    m_pcfDataFile = NULL;
    }
  if(iRet == SUCCESS)
    {
    try
      { // Create the file for writing.  If the file already exists this
        // will truncate the file.
      m_pcfDataFile = new CFile(m_pdPatient.cstrFile, 
                 CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite);
        // The file has been successfully created.
        // Build the file header.
      dfhFileHdr.iHdrStart = DATA_FILE_HDR_START;
      if(theApp.m_ptStudyType == PT_STANDARD)
        { // Standard study.
        strcpy(dfhFileHdr.szProgName, PROGRAM_NAME); // Program name.
        dfhFileHdr.uiDataFileVer = DATA_FILE_VER;  // File version.
        }
      else
        { // Research study.
        strcpy(dfhFileHdr.szProgName, PROGRAM_NAME_R1); // Program name.
        dfhFileHdr.uiDataFileVer = DATA_FILE_VER_R;  // File version.
        }
      dfhFileHdr.uiPatientDataSize = 0; // Not used for this version.
      if(iWrite_data_file((const void *)&dfhFileHdr, sizeof(dfhFileHdr)) != SUCCESS)
        { // Error writing the data file.
        //cstrErrTitle.LoadString(IDS_ERROR1);
        cstrErrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
        theApp.vLog(m_cstrErrMsg);
        m_p10MinView->MessageBox(m_cstrErrMsg, cstrErrTitle, MB_OK | MB_ICONSTOP);
        iRet = FAIL;
        }
      }
    catch(CFileException *pfe)
      { // Got a file creation error.
      //cstrMsg.Format(IDS_ERR_CREATE_FILE, m_pdPatient.cstrFile, strerror(pfe->m_lOsError));
      cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_CREATE_FILE), m_pdPatient.cstrFile,
        strerror(pfe->m_lOsError));
      theApp.vLog(cstrMsg);
      //cstrErrTitle.LoadString(IDS_ERROR1);
      cstrErrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
      m_p10MinView->MessageBox(cstrMsg, cstrErrTitle, MB_OK | MB_ICONSTOP);
      pfe->Delete();
      delete m_pcfDataFile;
      m_pcfDataFile = NULL;
      iRet = FAIL;
      }
    }
  return(iRet);
  }

/********************************************************************
iResume_data_file - Open the patient data file name for appending.

                    Used for research study only.
                         
  inputs: None.

  return: SUCCESS if the file is opened.
          FAIL if the file can't be opened.
********************************************************************/
short int CEGGSASDoc::iResume_data_file()
  {
  CString cstrMsg, cstrErrTitle;
  DATA_FILE_HEADER dhfHdr;
  short int iRet = FAIL;
  ULONGLONG uFileLen, uBaselineSize;

  try
    { // Create the file for writing.  If the file already exists this
      // will truncate the file.
      // Currently have an open data file.  Close it.
    vClose_data_file(false);
      // Make sure the file is the proper EGGSAS version.
    if(iOpen_file_get_hdr(&m_pcfDataFile, m_pdPatient.cstrFile, &dhfHdr, true) == SUCCESS)
      {
      if(bValid_file_hdr(&dhfHdr) == true
      && strcmp(dhfHdr.szProgName, PROGRAM_NAME_R1) == 0
      && dhfHdr.uiDataFileVer == DATA_FILE_VER_R)
        { // Proper EGGSAS file version.
          // Currently have an open data file.  Close it.
          vClose_data_file(false);
          // Open the file for writing.
        m_pcfDataFile = new CFile(m_pdPatient.cstrFile, 
                                  CFile::modeWrite);// | CFile::shareDenyWrite);
          // Check if there is data past the baseline.  If so remove it.
        uFileLen = m_pcfDataFile->GetLength();
        uBaselineSize = sizeof(DATA_FILE_HEADER)
                        + (m_pdPatient.uDataPtsInBaseline * 2 * sizeof(float))
                        + sizeof(BASELINE_TRAILER);
        if(uFileLen > uBaselineSize)
          {  // Truncate the file to the end of the baseline.
          m_pcfDataFile->SetLength(uBaselineSize);
          }
          // Now position the file pointer to the end of the file and we are
          // ready for writing.
        m_pcfDataFile->SeekToEnd();
        iRet = SUCCESS;
        }
      }
    }
  catch(CFileException *pfe)
    { // Got a file creation error.
    //cstrMsg.Format(IDS_ERR_CREATE_FILE, m_pdPatient.cstrFile, strerror(pfe->m_lOsError));
    cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_CREATE_FILE), m_pdPatient.cstrFile,
      strerror(pfe->m_lOsError));
    theApp.vLog(cstrMsg);
    //cstrErrTitle.LoadString(IDS_ERROR1);
    cstrErrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    m_p10MinView->MessageBox(cstrMsg, cstrErrTitle, MB_OK | MB_ICONSTOP);
    pfe->Delete();
    delete m_pcfDataFile;
    m_pcfDataFile = NULL;
    }
  return(iRet);
  }

/********************************************************************
cstrCreate_data_file_name - Create the data file from the patient information.

  inputs: Pointer to a PATIENT_DATA structure containing valid patient
            information.
          Type of file to create (EGG file, spreadsheet file).

  return: CString object containing the file name.
********************************************************************/
CString CEGGSASDoc::cstrCreate_data_file_name(PATIENT_DATA *ppdPatient, int iFileType)
  {
  CString cstrName, cstrDataFilePath;

  if(ppdPatient->bUseResrchRefNum == FALSE)
    cstrName.Format("%s %s %s %s.", ppdPatient->cstrFirstName,
                    ppdPatient->cstrMI, ppdPatient->cstrLastName,
                    ppdPatient->cstrDate);
  else
    cstrName.Format("%s %s.", ppdPatient->cstrResearchRefNum, ppdPatient->cstrDate);
  // For the ASCII raw data file, insert at the start of the file name, RawData.
  if(iFileType == FILE_TYPE_RAW_DATA)
    cstrName.Insert(0, "RawData - ");
    // Add the extension
  if(iFileType == FILE_TYPE_EGG)
    cstrName += "egg";
  else
    cstrName += "csv";
    // Look for the forward slashes in the file name (in the date part) and change
    // to dashes.
  cstrName.Replace('/', '-');
  if(iFileType == FILE_TYPE_EGG)
    { // If there is a path, put in path. Only for data files.
    cstrDataFilePath = m_pDB->cstrGet_db_path();
    if(cstrDataFilePath.IsEmpty() == FALSE) // Version 2.09i
      {
      cstrName.Insert(0, "\\");
      cstrName.Insert(0, cstrDataFilePath);
      }
    //if(theApp.m_cstrDataFilePath.IsEmpty() == FALSE)
    //  {
    //  cstrName.Insert(0, "\\");
    //  cstrName.Insert(0, theApp.m_cstrDataFilePath);
    //  }
    }
  return(cstrName);
  }

/********************************************************************
iWrite_data_file - Write a buffer of data to the data file.
                         
  inputs: Pointer to a buffer containing data for the data file.
          Number of bytes of data to be written.

  return: SUCCESS if the data was successfully written to the file.
          FAIL if a file write occurs.
********************************************************************/
short int CEGGSASDoc::iWrite_data_file(const void *pData, unsigned uSize)
  {
  short int iRet;

  m_cstrErrMsg.Empty();
  try
    {
    m_pcfDataFile->Write(pData, uSize);
    iRet = SUCCESS;
    }
  catch(CFileException *pfe)
    { // Got an error while writing to the file.  Error is displayed by caller.
    //m_cstrErrMsg.Format(IDS_ERR_WRITE_FILE, m_pcfDataFile->GetFileName(), 
    //                  strerror(pfe->m_lOsError));
    m_cstrErrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_WRITE_FILE), m_pcfDataFile->GetFileName(), 
                      strerror(pfe->m_lOsError));
    pfe->Delete();
    iRet = FAIL;
    }
  return(iRet);
  }

/********************************************************************
iReOpen_data_file - Open an existing data file for recording additional data.
                         
  inputs: None.

  return: SUCCESS if the file was successfully opened.
          FAIL if there is an error.
********************************************************************/
short CEGGSASDoc::iReOpen_data_file(void)
  {
  short int iRet;

  try
    { // Open the file.
    vClose_data_file(false); // First make sure the data file has been closed.
    m_pcfDataFile = new CFile(m_pdPatient.cstrFile, 
                 CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite | CFile::modeNoTruncate);
    m_pcfDataFile->SeekToEnd();
    iRet = SUCCESS;
    }
  catch(CFileException *pfe)
    { // Got a file open error.
    CString cstrErrMsg;
    //cstrErrMsg.Format(IDS_ERR_OPEN_FILE1, cstrFile, strerror(pfe->m_lOsError));
    cstrErrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_OPEN_FILE), m_pdPatient.cstrFile,
      strerror(pfe->m_lOsError));
    pfe->Delete();
    iRet = FAIL;
    }
  return(iRet);
  }

/********************************************************************
vCopy_patient - Copy the PATIENT_DATA structure from the source to the
                destination.
                         
  inputs: Pointer to the destination PATIENT_DATA structure.
          Pointer to the source PATIENT_DATA structure.

  return: None.
********************************************************************/
void CEGGSASDoc::vCopy_patient(PATIENT_DATA *ppdDest, PATIENT_DATA *ppdSrc)
  {
  CEvnt *pEvent, *pEventNew;
  int iIndex;

    // Data base information.
  ppdDest->lPatientID = ppdSrc->lPatientID;
  ppdDest->lStudyID = ppdSrc->lStudyID;
    // Demographics
	ppdDest->cstrLastName = ppdSrc->cstrLastName;
	ppdDest->cstrFirstName = ppdSrc->cstrFirstName;
  ppdDest->cstrMI = ppdSrc->cstrMI;
	ppdDest->cstrAddress1 = ppdSrc->cstrAddress1;
	ppdDest->cstrAddress2 = ppdSrc->cstrAddress2;
	ppdDest->cstrCity = ppdSrc->cstrCity;
	ppdDest->cstrDateOfBirth = ppdSrc->cstrDateOfBirth;
	ppdDest->cstrHospitalNum = ppdSrc->cstrHospitalNum;
	ppdDest->cstrPhone = ppdSrc->cstrPhone;
  ppdDest->bUseResrchRefNum = ppdSrc->bUseResrchRefNum;
	ppdDest->cstrResearchRefNum = ppdSrc->cstrResearchRefNum;
	ppdDest->cstrSocialSecurityNum = ppdSrc->cstrSocialSecurityNum;
	ppdDest->cstrState = ppdSrc->cstrState;
	ppdDest->cstrZipCode = ppdSrc->cstrZipCode;
	ppdDest->bMale = ppdSrc->bMale;
	ppdDest->bFemale = ppdSrc->bFemale;
	ppdDest->bHide = ppdSrc->bHide;
    // Pre-study information
	ppdDest->cstrMealTime = ppdSrc->cstrMealTime;
	ppdDest->cstrAttendPhys = ppdSrc->cstrAttendPhys;
	ppdDest->cstrIFPComments = ppdSrc->cstrIFPComments;
	ppdDest->cstrReferPhys = ppdSrc->cstrReferPhys;
	ppdDest->cstrTechnician = ppdSrc->cstrTechnician;
	ppdDest->bBloating = ppdSrc->bBloating;
	ppdDest->bEarlySateity = ppdSrc->bEarlySateity;
	ppdDest->bEructus = ppdSrc->bEructus;
	ppdDest->bGerd = ppdSrc->bGerd;
	ppdDest->bNausea = ppdSrc->bNausea;
	ppdDest->bPPFullness = ppdSrc->bPPFullness;
	ppdDest->bVomiting = ppdSrc->bVomiting;
  ppdDest->bDelayedGastricEmptying = ppdSrc->bDelayedGastricEmptying;
    // Water
  ppdDest->uWaterAmount = ppdSrc->uWaterAmount;
  ppdDest->uWaterUnits = ppdSrc->uWaterUnits;
  ppdDest->cstrWaterUnits = ppdSrc->cstrWaterUnits;
    // Study data
  ppdDest->cstrDate = ppdSrc->cstrDate;
  ppdDest->cstrFile = ppdSrc->cstrFile;
  ppdDest->uNumDataPts = ppdSrc->uNumDataPts;
    // Length in minutes of each period.
  memcpy(ppdDest->fPeriodStart, ppdSrc->fPeriodStart, sizeof(ppdSrc->fPeriodStart));
  memcpy(ppdDest->fPeriodLen, ppdSrc->fPeriodLen, sizeof(ppdSrc->fPeriodLen));
    // Start and end of good minutes for each period.
  memcpy(ppdDest->fGoodMinStart, ppdSrc->fGoodMinStart, sizeof(ppdSrc->fGoodMinStart));
  memcpy(ppdDest->fGoodMinEnd, ppdSrc->fGoodMinEnd, sizeof(ppdSrc->fGoodMinEnd));
  ppdDest->fTestMaxEGG = ppdSrc->fTestMaxEGG;
  ppdDest->fTestMinEGG = ppdSrc->fTestMinEGG;
  ppdDest->fMaxEGG = ppdSrc->fMaxEGG;
  ppdDest->fMinEGG = ppdSrc->fMinEGG;
  ppdDest->fMaxResp = ppdSrc->fMaxResp;
  ppdDest->fMinResp = ppdSrc->fMinResp;
  ppdDest->fMeanBaseline = ppdSrc->fMeanBaseline;
  memcpy(ppdDest->bPeriodsAnalyzed, ppdSrc->bPeriodsAnalyzed,
         sizeof(ppdSrc->bPeriodsAnalyzed));
  ppdDest->cstrSuggestedDiagnosis = ppdSrc->cstrSuggestedDiagnosis;
  ppdDest->cstrDiagnosis = ppdSrc->cstrDiagnosis;
  ppdDest->iDiagType = ppdSrc->iDiagType;
  ppdDest->cstrRecommendations = ppdSrc->cstrRecommendations;
  ppdDest->cstrMedications = ppdSrc->cstrMedications;
  ppdDest->bExtra10Min = ppdSrc->bExtra10Min;
  ppdDest->bHavePatientInfo = ppdSrc->bHavePatientInfo;
  ppdDest->bManualMode = ppdSrc->bManualMode;
    // Respiration.
  memcpy(ppdDest->uRespRate, ppdSrc->uRespRate, sizeof(ppdSrc->uRespRate));
    // Research study
  ppdDest->uNumPeriods = ppdSrc->uNumPeriods;
  ppdDest->cstrStimMedDesc = ppdSrc->cstrStimMedDesc;
  ppdDest->uDataPtsInBaseline = ppdSrc->uDataPtsInBaseline;
  ppdDest->bPause = ppdSrc->bPause;
  ppdDest->uPostStimDataPoints = ppdSrc->uPostStimDataPoints;
  ppdDest->uFrequency = ppdSrc->uFrequency;
    // Copy the events.
  vFree_event_list(ppdDest->Events.m_PSEvents);
  vFree_event_list(ppdDest->Events.m_BLEvents);

  if(ppdSrc->Events.m_BLEvents.GetCount() > 0)
    { // Baseline events.
    for(iIndex = 0; iIndex < ppdSrc->Events.m_BLEvents.GetCount(); ++iIndex)
      {
      pEventNew = new CEvnt();
      pEvent = ppdSrc->Events.m_BLEvents[iIndex];
      pEventNew->m_cstrDescription = pEvent->m_cstrDescription;
      pEventNew->m_cstrIndications = pEvent->m_cstrIndications;
      pEventNew->m_iDataPointIndex = pEvent->m_iDataPointIndex;
      pEventNew->m_iPeriodType = pEvent->m_iPeriodType;
      pEventNew->m_iSeconds = pEvent->m_iSeconds;
      ppdDest->Events.m_BLEvents.Add(pEventNew);
      }
    }
  if(ppdSrc->Events.m_PSEvents.GetCount() > 0)
    { // Post-stimulation events.
    for(iIndex = 0; iIndex < ppdSrc->Events.m_PSEvents.GetCount(); ++iIndex)
      {
      pEventNew = new CEvnt();
      pEvent = ppdSrc->Events.m_PSEvents[iIndex];
      pEventNew->m_cstrDescription = pEvent->m_cstrDescription;
      pEventNew->m_cstrIndications = pEvent->m_cstrIndications;
      pEventNew->m_iDataPointIndex = pEvent->m_iDataPointIndex;
      pEventNew->m_iPeriodType = pEvent->m_iPeriodType;
      pEventNew->m_iSeconds = pEvent->m_iSeconds;
      ppdDest->Events.m_PSEvents.Add(pEventNew);
      }
    }
  }

/********************************************************************
iCompare_patient - Compare 2 PATIENT_DATA structures.
                         
  inputs: Pointer to the first PATIENT_DATA structure.
          Pointer to the second PATIENT_DATA structure.

  return: SUCCESS if the 2 data structures are the same.
          FAIL if they are different.
********************************************************************/
short int CEGGSASDoc::iCompare_patient(PATIENT_DATA *ppd1, PATIENT_DATA *ppd2)
  {
  CEvnt *pEvent1, *pEvent2;
  short int iIndex, iRet = SUCCESS;

    // Demographics
	if(ppd1->cstrLastName != ppd2->cstrLastName
	|| ppd1->cstrFirstName != ppd2->cstrFirstName
  || ppd1->cstrMI != ppd2->cstrMI
	|| ppd1->cstrAddress1 != ppd2->cstrAddress1
	|| ppd1->cstrAddress2 != ppd2->cstrAddress2
	|| ppd1->cstrCity != ppd2->cstrCity
	|| ppd1->cstrDateOfBirth != ppd2->cstrDateOfBirth
	|| ppd1->cstrHospitalNum != ppd2->cstrHospitalNum
	|| ppd1->cstrPhone != ppd2->cstrPhone
	|| ppd1->cstrResearchRefNum != ppd2->cstrResearchRefNum
  || ppd1->bUseResrchRefNum != ppd2->bUseResrchRefNum
	|| ppd1->cstrSocialSecurityNum != ppd2->cstrSocialSecurityNum
	|| ppd1->cstrState != ppd2->cstrState
	|| ppd1->cstrZipCode != ppd2->cstrZipCode
  || ppd1->bMale != ppd2->bMale
  || ppd1->bFemale != ppd2->bFemale
  || ppd1->bHide != ppd2->bHide)
    iRet = FAIL;
    // Pre-study information
	else if(ppd1->cstrMealTime != ppd2->cstrMealTime
	|| ppd1->cstrAttendPhys != ppd2->cstrAttendPhys
	|| ppd1->cstrIFPComments != ppd2->cstrIFPComments
	|| ppd1->cstrReferPhys != ppd2->cstrReferPhys
	|| ppd1->cstrTechnician != ppd2->cstrTechnician
	|| ppd1->bBloating != ppd2->bBloating
	|| ppd1->bEarlySateity != ppd2->bEarlySateity
	|| ppd1->bEructus != ppd2->bEructus
	|| ppd1->bGerd != ppd2->bGerd
	|| ppd1->bNausea != ppd2->bNausea
	|| ppd1->bPPFullness != ppd2->bPPFullness
	|| ppd1->bVomiting != ppd2->bVomiting
  || ppd1->bDelayedGastricEmptying != ppd2->bDelayedGastricEmptying)
    iRet = FAIL;
    // Water
  else if(ppd1->uWaterAmount != ppd2->uWaterAmount
  || ppd1->uWaterUnits != ppd2->uWaterUnits
  || ppd1->cstrWaterUnits != ppd2->cstrWaterUnits
  || ppd1->cstrStimMedDesc != ppd2->cstrStimMedDesc)
    iRet = FAIL;
  else
    { // Study data
    if(m_bUsingWhatIf == false)
      { // Only compare if not using a What If scenario.
      if(memcmp(ppd1->fPeriodStart, ppd2->fPeriodStart, sizeof(ppd2->fPeriodStart)) != 0
      || memcmp(ppd1->fPeriodLen, ppd2->fPeriodLen, sizeof(ppd2->fPeriodLen)) != 0
      || memcmp(ppd1->fGoodMinStart, ppd2->fGoodMinStart, sizeof(ppd2->fGoodMinStart)) != 0
      || memcmp(ppd1->fGoodMinEnd, ppd2->fGoodMinEnd, sizeof(ppd2->fGoodMinEnd)) != 0
      || memcmp(ppd1->uRespRate, ppd2->uRespRate, sizeof(ppd2->uRespRate)) != 0
        // Research study
      || ppd1->uNumPeriods != ppd2->uNumPeriods)
        iRet = FAIL;
      }
    }
  if(iRet == SUCCESS)
    {
    if(ppd1->cstrDate != ppd2->cstrDate
    || ppd1->cstrFile != ppd2->cstrFile
//    || ppd1->uNumDataPts != ppd2->uNumDataPts
    || ppd1->fTestMaxEGG != ppd2->fTestMaxEGG
    || ppd1->fTestMinEGG != ppd2->fTestMinEGG
//    || ppd1->fMaxEGG != ppd2->fMaxEGG
//    || ppd1->fMinEGG != ppd2->fMinEGG
//    || ppd1->fMaxResp != ppd2->fMaxResp
//    || ppd1->fMinResp != ppd2->fMinResp
//    || ppd1->fMeanBaseline != ppd2->fMeanBaseline
//    || ppd1->uPeriodsAnalyzed != ppd2->uPeriodsAnalyzed
    || ppd1->cstrSuggestedDiagnosis != ppd2->cstrSuggestedDiagnosis
    || ppd1->cstrDiagnosis != ppd2->cstrDiagnosis
    || ppd1->iDiagType != ppd2->iDiagType
    || ppd1->cstrRecommendations != ppd2->cstrRecommendations
    || ppd1->cstrMedications != ppd2->cstrMedications
    || ppd1->bExtra10Min != ppd2->bExtra10Min
    || ppd1->bHavePatientInfo != ppd2->bHavePatientInfo
    || ppd1->bManualMode != ppd2->bManualMode
      // Research study
//    || ppd1->uNumPeriods != ppd2->uNumPeriods
    || ppd1->uDataPtsInBaseline != ppd2->uDataPtsInBaseline
    || ppd1->bPause != ppd2->bPause
    || ppd1->uPostStimDataPoints != ppd2->uPostStimDataPoints)
     iRet = FAIL;
    }
  if(iRet == SUCCESS)
    { // Compare events.
      // Baseline events.
    if(ppd1->Events.m_BLEvents.GetCount() != ppd2->Events.m_BLEvents.GetCount())
      iRet = FAIL;
    else if(ppd1->Events.m_BLEvents.GetCount() > 0)
      {
      for(iIndex = 0; iIndex < ppd1->Events.m_BLEvents.GetCount(); ++iIndex)
        {
        pEvent1 = ppd1->Events.m_BLEvents[iIndex];
        pEvent2 = ppd2->Events.m_BLEvents[iIndex];
        if(pEvent1->m_iDataPointIndex != pEvent2->m_iDataPointIndex
        || pEvent1->m_cstrDescription != pEvent2->m_cstrDescription
        || pEvent1->m_cstrIndications != pEvent2->m_cstrIndications)
          {
          iRet = FAIL;
          break;
          }
        }
      }
    if(iRet == SUCCESS)
      { // Post-stimulation events.
      if(ppd1->Events.m_PSEvents.GetCount() != ppd2->Events.m_PSEvents.GetCount())
        iRet = FAIL;
      else if(ppd1->Events.m_PSEvents.GetCount() > 0)
        {
        for(iIndex = 0; iIndex < ppd1->Events.m_PSEvents.GetCount(); ++iIndex)
          {
          pEvent1 = ppd1->Events.m_PSEvents[iIndex];
          pEvent2 = ppd2->Events.m_PSEvents[iIndex];
          if(pEvent1->m_iDataPointIndex != pEvent2->m_iDataPointIndex
          || pEvent1->m_cstrDescription != pEvent2->m_cstrDescription
          || pEvent1->m_cstrIndications != pEvent2->m_cstrIndications)
            {
            iRet = FAIL;
            break;
            }
          }
        }
      }
    }
  return(iRet);
  }

/********************************************************************
iCheck_to_Save - Check the current PATIENT_DATA structure against the
                 saved one to determine if there have been any changes.
                 If there have been changed, ask the user if the patient
                 should be saved.

                 If there is no patient name, then there is no need to save.

  inputs: None.

  return: IDYES if the patient data has been saved.
          IDCANCEL, IDNO if data is not saved.
********************************************************************/
int CEGGSASDoc::iCheck_to_Save()
  {
  CString cstrMsg, cstrTitle;
  int iRet = IDNO;

  if(m_bHaveFile == TRUE)
    {
    if(m_pdPatient.cstrLastName.IsEmpty() == FALSE
      || (m_pdPatient.bUseResrchRefNum == TRUE
      && m_pdPatient.cstrResearchRefNum.IsEmpty() == FALSE))
      { // There is currently a patient.
        // Check the What If scenario.  This MUST be done before the patient data.
        // If a What If scenario is loaded, saving the patient data will copy back
        // into patient data the original patient data that was overwritten by the
        // What If data.
      if(m_bUsingWhatIf == true)
        {
        if(iCompare_whatif(&m_pdPatient, &m_wifOrigData) == FAIL)
          { // The What If data has changed, ask if user wants to save it.
          //cstrMsg.LoadString(IDS_WANT_TO_SAVE_WHATIF);
          cstrMsg = g_pLang->cstrLoad_string(ITEM_G_WANT_TO_SAVE_WHATIF);
          //cstrTitle.LoadString(IDS_WARNING);
          cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
          iRet = AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrTitle, MB_YESNOCANCEL);
          if(iRet == IDYES)
            vFileSaveWhatIf(); // Save the What If data.
          }
        }
        // Compare the saved patient data with the current patient data.
      if(iCompare_patient(&m_pdSavedPatient, &m_pdPatient) == FAIL)
        { // Patient data has changed, ask if user wants to save it first.
        //cstrMsg.LoadString(IDS_WANT_TO_SAVE_PATIENT);
        cstrMsg = g_pLang->cstrLoad_string(ITEM_G_WANT_TO_SAVE_PATIENT);
        //cstrTitle.LoadString(IDS_WARNING);
        cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
        iRet = AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrTitle, MB_YESNOCANCEL);
        //iRet =AfxMessageBox(cstrMsg, MB_YESNOCANCEL | MB_ICONQUESTION);
        if(iRet == IDYES)
          {
          if(m_bUsingWhatIf == true)
            { // If a What If scenario is displayed, copy the original data that was
              // overwritten by the What If data back to the patient data.
            m_pdPatient.uNumPeriods = m_wifOrigStudyData.uNumPeriods;
            memcpy(m_pdPatient.fPeriodStart, m_wifOrigStudyData.fPeriodStart,
                  sizeof(m_pdPatient.fPeriodStart));
            memcpy(m_pdPatient.fPeriodLen, m_wifOrigStudyData.fPeriodLen,
                  sizeof(m_pdPatient.fPeriodLen));
            memcpy(m_pdPatient.fGoodMinStart, m_wifOrigStudyData.fGoodMinStart,
                  sizeof(m_pdPatient.fGoodMinStart));
            memcpy(m_pdPatient.fGoodMinEnd, m_wifOrigStudyData.fGoodMinEnd,
                  sizeof(m_pdPatient.fGoodMinEnd));
            }
          vFileSave(); // Save the patient data.
          }
        }
      }
    }
  else
    iRet = IDYES;
  return(iRet);
  }

/********************************************************************
vSet_strip_chart_attrib - Set the strip chart attributes for the
                          recording strip charts.

  inputs: Handle to the strip chart.
          Pointer to a STRIP_CHART_ATTRIB structure containing the
            attributes to set.

  return: None.
********************************************************************/
void CEGGSASDoc::vSet_strip_chart_attrib(HWND hPE, STRIP_CHART_ATTRIB *pscaGraph)
  {
  CString cstrText;
  double m_dMinXScale, m_dMaxXScale, dAxisCntrl, dValue;
  float fVAlue, fMAPArray[2];
  int iMASArray[2];

  PEnset(hPE, PEP_nSUBSETS, 2); // 2 sets of data, EGG and Respiration.
    // Set the maximum X-axis scale.  It displays quarter seconds.
  PEnset(hPE, PEP_nPOINTS, pscaGraph->iTotalPoints); //iMaxPoints);

    // Set the subset line types for each set of data.
  int nLineTypes[] = {PELT_THINSOLID, PELT_THINSOLID};
  PEvset(hPE, PEP_naSUBSETLINETYPES, nLineTypes, 2);

    // Set Manual X scale
  PEnset(hPE, PEP_nMANUALSCALECONTROLX, PEMSC_MINMAX);
  m_dMinXScale = 0.0F;
  PEvset(hPE, PEP_fMANUALMINX, &m_dMinXScale, 1);
  m_dMaxXScale = pscaGraph->dMaxXScale; // Numbers are displayed every half minute.
  PEvset(hPE, PEP_fMANUALMAXX, &m_dMaxXScale, 1);

    // Set values to be placed every second and tick marks every quarter second.
  dAxisCntrl = 1.0F; // Multiples of grid tick marks where values are placed.
  PEvset(hPE, PEP_fMANUALXAXISLINE, &dAxisCntrl, 1);
  dAxisCntrl = 0.5F; // Where grid tick marks are placed.
  PEvset(hPE, PEP_fMANUALXAXISTICK, &dAxisCntrl, 1);
  PEnset(hPE, PEP_bMANUALXAXISTICKNLINE, TRUE);

    // Set a NULL data value other than 0 so the graph display values of 0. As this
    // may be the case if nothing is connected.
  dValue = -20000.0F;
  PEvset(hPE, PEP_fNULLDATAVALUE, &dValue, 1);

    // Clear out default data //
  fVAlue = 0;
  PEvsetcellEx(hPE, PEP_faXDATA, 0, 0, &fVAlue);
  PEvsetcellEx(hPE, PEP_faXDATA, 0, 1, &fVAlue);
  PEvsetcellEx(hPE, PEP_faXDATA, 0, 2, &fVAlue);
  PEvsetcellEx(hPE, PEP_faXDATA, 0, 3, &fVAlue);

  PEvsetcellEx(hPE, PEP_faYDATA, 0, 0, &fVAlue);
  PEvsetcellEx(hPE, PEP_faYDATA, 0, 1, &fVAlue);
  PEvsetcellEx(hPE, PEP_faYDATA, 0, 2, &fVAlue);
  PEvsetcellEx(hPE, PEP_faYDATA, 0, 3, &fVAlue);

    // Set background and graphs shades.
  PEnset(hPE, PEP_bBITMAPGRADIENTMODE, FALSE);
  PEnset(hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_INSET);

  PEszset(hPE, PEP_szMAINTITLE, ""); // Blank title.
  PEszset(hPE, PEP_szSUBTITLE, ""); // no subtitle

    // X axis title.
  //cstrText.LoadString(IDS_MINUTES);
  cstrText = g_pLang->cstrLoad_string(ITEM_G_MINUTES);
  PEszset(hPE, PEP_szXAXISLABEL, (char *)(const char *)cstrText);

    // Disable random points to export listbox.
  PEnset(hPE, PEP_bNORANDOMPOINTSTOEXPORT, TRUE);
  PEnset(hPE, PEP_bFOCALRECT, FALSE); // No focal rect.
  PEnset(hPE, PEP_bALLOWBAR, FALSE); // Bar Graph is not available.
  PEnset(hPE, PEP_bALLOWPOPUP, FALSE); // User can not access popup menu.
  PEnset(hPE, PEP_bPREPAREIMAGES, TRUE); // Prepare images in memory.
  PEnset(hPE, PEP_bCACHEBMP, TRUE); // Store prepared image for possible re-use.
  PEnset(hPE, PEP_bFIXEDFONTS, TRUE); // Keep font size fixed.

    // subset colors (EGG, Respiration)
  DWORD dwArray[2] = {RED_RGB, BLACK_RGB};
  PEvsetEx(hPE, PEP_dwaSUBSETCOLORS, 0, 2, dwArray, 0);
    // Set the subset labels that appear at the top of the graph.
  //cstrText.LoadString(IDS_EGG);
  cstrText = g_pLang->cstrLoad_string(ITEM_G_EGG);
  PEvsetcell(hPE, PEP_szaSUBSETLABELS, 0, (char *)(const char *)cstrText);
  //cstrText.LoadString(IDS_RESPIRATION);
  cstrText = g_pLang->cstrLoad_string(ITEM_G_RESPIRATION);
  PEvsetcell(hPE, PEP_szaSUBSETLABELS, 1, (char *)(const char *)cstrText);

    // Set up the double graphs.  EGG on top and Respiration on bottom.
    // Separate the 2 graphs.
  PEnset(hPE, PEP_nMULTIAXISSTYLE, PEMAS_SEPARATE_AXES);
  PEnset(hPE, PEP_nMULTIAXISSEPARATORSIZE, 75);

    // Y axis title.
  //cstrText.LoadString(IDS_RAW_WAVFRM_Y_AXIS_LBL);
  cstrText = g_pLang->cstrLoad_string(ITEM_G_RAW_WAVFRM_Y_AXIS_LBL);
  PEszset(hPE, PEP_szYAXISLABEL, (char *)(const char *)cstrText);
    // Two graphs are to be drawn in this window (EGG and respiration).
    // Designate which graph uses the top part of the window and which
    // uses the bottom part of the window.
  iMASArray[0] = 1;   // EGG signal is top graph.
  iMASArray[1] = 1;   // Respiration signal is bottom graph.
  PEvset(hPE, PEP_naMULTIAXESSUBSETS, iMASArray, 2);
  
    // Designate axes proportions.  The percentage of the window that
    // each graph uses.
  fMAPArray[0] = (float)0.6; // Percentage of window top graph uses.
  fMAPArray[1] = (float)0.4; // Percentage of window bottom graph uses.
  PEvset(hPE, PEP_faMULTIAXESPROPORTIONS, fMAPArray, 2);

    // Y-axis label for the top graph.
  PEnset(hPE, PEP_nWORKINGAXIS, 0); // Set first y axis parameters
    // Set the minimum and maximum Y-axis scale values for EGG.
  PEnset(hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
  PEvset(hPE, PEP_fMANUALMINY, &pscaGraph->dMinEGG, 1);
  PEvset(hPE, PEP_fMANUALMAXY, &pscaGraph->dMaxEGG, 1);

    // Y-axis label for the bottom graph.
  PEnset(hPE, PEP_nWORKINGAXIS, 1); // Set second y axis parameters
  //cstrText.LoadString(IDS_RAW_WAVFRM_Y_AXIS_LBL);
  cstrText = g_pLang->cstrLoad_string(ITEM_G_RAW_WAVFRM_Y_AXIS_LBL);
//  cstrText.LoadString(IDS_VOLTS);
  PEszset(hPE, PEP_szYAXISLABEL, (char *)(const char *)cstrText);
    // Set the minimum and maximum Y-axis scale values for Respiration.
  PEnset(hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
//  dValue = (double)0.0; //pscaGraph->dMinEGG / (double)2.0;
//  PEvset(hPE, PEP_fMANUALMINY, &dValue, 1);
//  dValue = pscaGraph->dMaxEGG / (double)2.0;
//  PEvset(hPE, PEP_fMANUALMAXY, &dValue, 1);
  PEvset(hPE, PEP_fMANUALMINY, &pscaGraph->dMinResp, 1);
  PEvset(hPE, PEP_fMANUALMAXY, &pscaGraph->dMaxResp, 1);

  PEnset(hPE, PEP_nWORKINGAXIS, 0); // Always reset working axis to 0.

  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Set up any Research only graph attributes.
      // Allow data point hotspots for mouse clicks.
    PEnset(hPE, PEP_bALLOWDATAHOTSPOTS, TRUE);
    }

  }

/********************************************************************
vSet_add_periods - If study has extra 10 minutes (4th period) adjust data
                   so there is only 3 periods by making the 3rd period
                   20 minutes and making sure the good start and end minutes
                   of the 4th period are invalid.  If this is an existing
                   study and it is just being adjusted, save it.

  inputs: Type of study, new or existing.

  return: None.
********************************************************************/
void CEGGSASDoc::vSet_add_periods(int iStudyType)
  {
  CString cstrBaseText, cstrText;
  UINT uID;

  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard study
    //theApp.vLog("@@@ DEBUG: vSet_add_periods: WaterLoad study type");
    m_uMaxPeriods = MAX_PERIODS;
    m_pdPatient.bManualMode = FALSE;
    if(m_pdPatient.bExtra10Min == TRUE)
      {
        // Check the database to see if we need to adjust the 3rd period.
        // Set 3rd period length to 20.
        // The start and end minutes of the 4th period to -1.0
      if(m_pdPatient.fPeriodLen[PERIOD_30] == 10)
        { // Hasn't been changed, change.
        m_pdPatient.fPeriodLen[PERIOD_30] = 20;
        m_pdPatient.fGoodMinStart[PERIOD_40] = -1.0F;
        m_pdPatient.fGoodMinEnd[PERIOD_40] = -1.0F;
        if(iStudyType == STUDY_TYPE_EXISTING)
          { // This is a study done previous to this change, so make the
            // change permanent in the database.
          m_pDB->iSave_study_data(&m_pdPatient);
          }
        }
      }
/**************************************
    if(m_pdPatient.bExtra10Min == TRUE)
      {
      cstrText.Empty();
      m_pdPatient.bManualMode =   TRUE;
      m_uMaxPeriods = MAX_MANUAL_MODE_PERIODS;
        // Add the analyze fourth 10 minute menu item.
      g_pFrame->vAdd_menu_item(ID_ANALYZE_BASELINE, ID_ANALYZE_POSTWATER_40, 0, cstrText);
        // Add the view fourth 10 minute menu item.
      g_pFrame->vAdd_menu_item(ID_VIEW_BASELINE, ID_VIEW_POSTWATER_40, 0, cstrText);
      }
    else
      {
        // Remove the analyze fourth 10 minute menu item.
      g_pFrame->cstrDelete_menu_item(ID_ANALYZE_BASELINE, ID_ANALYZE_POSTWATER_40);
        // Remove the view fourth 10 minute menu item.
      g_pFrame->cstrDelete_menu_item(ID_VIEW_BASELINE, ID_VIEW_POSTWATER_40);
      m_pdPatient.bManualMode = FALSE;
      m_uMaxPeriods = MAX_PERIODS;
      }
************************************/
    }
  else
    { // Research version
    //theApp.vLog("@@@ DEBUG: vSet_add_periods: Research study type");
    if(m_uNumPeriodMenuItems < m_pdPatient.uNumPeriods && m_pdPatient.uNumPeriods >= 1)
      { // Need to add menu items.
      //cstrBaseText.LoadString(IDS_POST_STIM_PERIOD);
      cstrBaseText = g_pLang->cstrLoad_string(ITEM_G_POST_STIM_PERIOD);
      uID = (m_uNumPeriodMenuItems * 2) + MENU_ID_START;
      while(m_uNumPeriodMenuItems < m_pdPatient.uNumPeriods)
        {
        cstrText.Format("%s %d", cstrBaseText, m_uNumPeriodMenuItems + 1);
          // Add to the Analyze menu.
        m_uAnalyzeMenuPostStimPeriodIDs[m_uNumPeriodMenuItems] = uID;
        g_pFrame->vAdd_menu_item(ID_ANALYZE_BASELINE, uID, 0, cstrText);
        ++uID;
          // Add to the View menu.
        m_uViewMenuPostStimPeriodIDs[m_uNumPeriodMenuItems] = uID;
        g_pFrame->vAdd_menu_item(ID_VIEW_BASELINE, uID, 0, cstrText);
        ++uID;

        ++m_uNumPeriodMenuItems;
        }
      }
    }
  }

/********************************************************************
vRemove_period_menu_item - Remove a post-stimulation period from the
                           View and Analyze menus.

                           Used for Research Study only.

  inputs: Period that is being deleted.

  output: Modifies the View and Analyze dropdown menus.

  return: None.
********************************************************************/
void CEGGSASDoc::vRemove_period_menu_item(unsigned short uPeriod)
  {
  unsigned short uCnt, uID;
  CString cstrBaseText, cstrText;

    // First remove all menu items.
  for(uCnt = 0; uCnt < m_uNumPeriodMenuItems; ++uCnt)
    {
    g_pFrame->cstrDelete_menu_item(ID_ANALYZE_BASELINE, m_uAnalyzeMenuPostStimPeriodIDs[uCnt]);
    g_pFrame->cstrDelete_menu_item(ID_VIEW_BASELINE, m_uViewMenuPostStimPeriodIDs[uCnt]);
    }
    // Reduce the number of menu items.
  --m_uNumPeriodMenuItems;
    // Add in all  menu items.
  //cstrBaseText.LoadString(IDS_POST_STIM_PERIOD);
  cstrBaseText = g_pLang->cstrLoad_string(ITEM_G_POST_STIM_PERIOD);
  uID = MENU_ID_START;
  for(uCnt = 0; uCnt < m_uNumPeriodMenuItems; ++uCnt)
    {
    cstrText.Format("%s %d", cstrBaseText, uCnt + 1);
      // Add to the Analyze menu.
    m_uAnalyzeMenuPostStimPeriodIDs[uCnt] = uID;
    g_pFrame->vAdd_menu_item(ID_ANALYZE_BASELINE, uID, 0, cstrText);
    ++uID;
      // Add to the View menu.
    m_uViewMenuPostStimPeriodIDs[uCnt] = uID;
    g_pFrame->vAdd_menu_item(ID_VIEW_BASELINE, uID, 0, cstrText);
    ++uID;
    }
  }

/********************************************************************
vDraw_raw_data_graph - Draw the raw data graph.

                       The graph window is split into 2 with the top graph for
                       the EGG signal and the bottom graph for the
                       respiration signal.

  inputs: The window handle of the graph.
          Pointer to a GRAPH_INFO structure that contains the information
            needed to draw the graph.

  output: The pointer to the data in the GRAPH_INFO structure is incremented
            for each data item graphed.  This change affects the caller so if
            the caller needs the original data pointer, it must be saved
            outside this structure.

  return: None.
********************************************************************/
void CEGGSASDoc::vDraw_raw_data_graph(HWND hPEGrph, GRAPH_INFO *pgi)
  {
  CString cstrTitle, cstrTickMarks, cstrTemp;
  int iCnt, iLineStyles[2], iMASArray[2], *piTypes, *piTypesPtr; // iTypes[(MIN_PER_PERIOD * 2) + 1]
  int iMinute, iStartSeconds, iEndSeconds;
  double dValue, *pdXLoc, *pdXLocPtr; // fXLoc[(MIN_PER_PERIOD * 2) + 1]
  char *pszPrefix = "|h";
  float fX, fY, fMAPArray[2];
  DWORD dwArray[2], *pdwTickColor, *pdwTickColorPtr; //, dwTickColor[(MIN_PER_PERIOD * 2) + 1];
  unsigned uNumAnnotations, uNumEvents;
  CEvntArray *pEvents;
  CEvnt *pEvent;

   // Set the number of subsets (lines to draw).
 PEnset(hPEGrph, PEP_nSUBSETS, 2);
   // Set the number of data points.
 PEnset(hPEGrph, PEP_nPOINTS, pgi->iNumDataPoints);
   // Plot the points.
 for(iCnt = 0; iCnt < pgi->iNumDataPoints; ++iCnt)
   {
   fX = pgi->fStartMin + ((float)iCnt / (float)DATA_POINTS_PER_MIN_1);
     // EGG Signal
   PEvsetcellEx(hPEGrph, PEP_faXDATA, 0, iCnt, &fX);
   fY = *pgi->pfData;
   ++pgi->pfData;
     // Don't graph anything above the maximum or below the minumum.
   if(fY < pgi->fMinEGG)
     fY = pgi->fMinEGG;
   else if(fY > pgi->fMaxEGG)
     fY = pgi->fMaxEGG;
   PEvsetcellEx(hPEGrph, PEP_faYDATA, 0, iCnt, &fY);
    // Respiration Signal
   PEvsetcellEx(hPEGrph, PEP_faXDATA, 1, iCnt, &fX);
   fY = *pgi->pfData;
   ++pgi->pfData;
     // Don't graph anything above the maximum or below the minumum.
   if(fY < pgi->fMinResp)
     fY = pgi->fMinResp;
   else if(fY > pgi->fMaxResp)
     fY = pgi->fMaxResp;
   PEvsetcellEx(hPEGrph, PEP_faYDATA, 1, iCnt, &fY);
   }

    // Remove plotting method from pop-up menu.
  PEnset(hPEGrph, PEP_bALLOWPLOTCUSTOMIZATION, FALSE);
  PEszset(hPEGrph, PEP_szMAINTITLE, ""); // No graph title.
    // Set an empty subtitle so that if the if the very top of the Y axis
    // has a label, it doesn't get cut off.
  PEszset(hPEGrph, PEP_szSUBTITLE, " "); // no subtitle
  //cstrTitle.LoadString(IDS_MINUTES);
  cstrTitle = g_pLang->cstrLoad_string(ITEM_G_MINUTES);
  PEszset(hPEGrph, PEP_szXAXISLABEL, (char *)(const char *)cstrTitle);

  PEnset(hPEGrph, PEP_nFONTSIZE, PEFS_LARGE); //PEFS_MEDIUM PEFS_LARGE // Set to large font size.

    // Label the X axis using minutes starting with the first minute of the 
    // 4-minute epoch.
    // Remove everything on the X-axis except the label.
  PEnset(hPEGrph, PEP_nSHOWXAXIS, PESA_LABELONLY); //PESA_EMPTY);
    // Set the minimum and maximum X-axis scale values.
  PEnset(hPEGrph, PEP_nMANUALSCALECONTROLX, PEMSC_MINMAX);
  dValue = (double)pgi->fStartMin;
  PEvset(hPEGrph, PEP_fMANUALMINX, &dValue, 1);
  dValue = (double)(pgi->fStartMin + pgi->fMinOnGraph);
  PEvset(hPEGrph, PEP_fMANUALMAXX, &dValue, 1);

    // Now build the data used for drawing the X-axis tick marks
    // and delineating the artifact.
  cstrTickMarks.Empty();
  uNumAnnotations = ((unsigned)pgi->fMinOnGraph * 2) + 1;
    
    // Count the number of events on this graph.
  uNumEvents = 0;
    // Count number of events in this graph and add to number of annotations.
  if(pgi->uPeriod == 0)
    pEvents = &m_pdPatient.Events.m_BLEvents;
  else
    pEvents = &m_pdPatient.Events.m_PSEvents;
  if(pEvents->GetCount() > 0)
    { // There are events.  Count the ones in this graph.
    iStartSeconds = (int)(pgi->fStartMin * (float)60.0);
    iEndSeconds = iStartSeconds + (int)(pgi->fMinOnGraph * (float)60.0);
    if((pgi->fStartMin + pgi->fMinOnGraph)
    > m_pdPatient.fPeriodStart[pgi->uPeriod] + m_pdPatient.fPeriodLen[pgi->uPeriod])
      { // The period ends before the minutes on graph.
      iEndSeconds = (int)(m_pdPatient.fPeriodStart[pgi->uPeriod] * (float)60.0)
                    + (int)(m_pdPatient.fPeriodLen[pgi->uPeriod] * (float)60.0);
      }
    for(int i = 0; i < pEvents->GetCount(); ++i)
      {
      pEvent = pEvents->GetAt(i);
      if(pEvent->m_iSeconds >= iStartSeconds && pEvent->m_iSeconds <= iEndSeconds)
        ++uNumEvents;
      }
    }

  unsigned uTotalAnnotations;
  uTotalAnnotations = uNumAnnotations + uNumEvents;
    // Allocate arrays to hold the data needed for annotations.
  if((piTypes = (int *)GlobalAlloc(GPTR, sizeof(int) * uTotalAnnotations))
  != NULL
  && (pdXLoc = (double *)GlobalAlloc(GPTR, sizeof(double) * uTotalAnnotations))
  != NULL
  && (pdwTickColor = (DWORD *)GlobalAlloc(GPTR, sizeof(DWORD) * uTotalAnnotations))
  != NULL)
    {
    piTypesPtr = piTypes;
    pdXLocPtr = pdXLoc;
    pdwTickColorPtr = pdwTickColor;

    for(iCnt = 0; iCnt < (short int)uNumAnnotations; ++iCnt)
      {
      if(cstrTickMarks.IsEmpty() == FALSE)
        cstrTickMarks += "\t";
      if((iCnt % 2) == 0)
        { // On the minute
        if((pgi->uAttrib & MINUTES_FRACTIONS) != 0)
          cstrTemp.Format("%s%.1f", pszPrefix, pgi->fGraphStartMin + (iCnt / 2));
        else
          {
          if(theApp.m_ptStudyType == PT_STANDARD)
            { // Standard version
            cstrTemp.Format("%s%d", pszPrefix, (int)pgi->fGraphStartMin + (iCnt / 2));
            }
          else
            { // Research version
            cstrTemp.Format("%s%.1f", pszPrefix, pgi->fGraphStartMin + (iCnt / 2));
            }
          }
          // Specify type of tick mark.
        *piTypesPtr = PELAT_GRIDLINE;; //iLineType;
          // Specify where on the X-axis the tick marks go.
        iMinute = iCnt / 2;
        *pdXLocPtr = (double)(pgi->fStartMin + iMinute);
        }
      else
        { // On the half minute
        cstrTemp = pszPrefix; //.Format("%s%s", pszPrefix, cstrArtifact);
          // Specify type of tick mark.
        *piTypesPtr = PELAT_GRIDTICK; //iLineType;
          // Specify where on the X-axis the tick marks go.
        *pdXLocPtr = (double)(pgi->fStartMin + iMinute) + (double)0.5;
        }
      cstrTickMarks += cstrTemp;
        // Set the color of the X-axis tick marks, BLACK_RGB.
      *pdwTickColorPtr = BLACK_RGB;; //dwColor;
      ++piTypesPtr;
      ++pdXLocPtr;
      ++pdwTickColorPtr;
      }
      // Now do the events.  Put annotations for events on this graph.
    if(pEvents->GetCount() > 0)
      { // There are events.  Count the ones in this graph.
      for(int i = 0; i < pEvents->GetCount(); ++i)
        {
        pEvent = pEvents->GetAt(i);
        if(pEvent->m_iSeconds >= iStartSeconds && pEvent->m_iSeconds <= iEndSeconds)
          {
          *piTypesPtr = PELAT_GRIDLINE; //iLineType;
          ++piTypesPtr;
          *pdXLocPtr = (double)(iStartSeconds + (pEvent->m_iSeconds - iStartSeconds))
                       / (double)60.0;
          cstrTickMarks += "\t";
          cstrTickMarks += pszPrefix; // "|H"; 
          // These statement will put the indications at the tick mark.  If use
          // add a \r\n to the setting of the bottom margin (PEP_szBOTTOMMARGIN)
          // so that the indications don't overwrite the minutes text.  This will
          // compress the graph so its narrower.
          // The prefix |H will put the events on the top of the graph.
//          cstrTickMarks += " "; //\r\n";
//          cstrTickMarks += (char)0xA;
//          cstrTickMarks += pEvent->m_cstrIndications;
          ++pdXLocPtr;
          *pdwTickColorPtr = EVENT_RGB;
          ++pdwTickColorPtr;
          }
        }
      }

    PEvset(hPEGrph, PEP_szaVERTLINEANNOTATIONTEXT,
           cstrTickMarks.GetBuffer(cstrTickMarks.GetLength()), uTotalAnnotations);
    cstrTickMarks.ReleaseBuffer();
    PEvset(hPEGrph, PEP_naVERTLINEANNOTATIONTYPE, piTypes, uTotalAnnotations);
    PEvset(hPEGrph, PEP_faVERTLINEANNOTATION, pdXLoc, uTotalAnnotations);
    PEvset(hPEGrph, PEP_dwaVERTLINEANNOTATIONCOLOR, pdwTickColor, uTotalAnnotations);
    HeapFree(GetProcessHeap(), 0, (LPVOID)piTypes);
    HeapFree(GetProcessHeap(), 0, (LPVOID)pdXLoc);
    HeapFree(GetProcessHeap(), 0, (LPVOID)pdwTickColor);
    }
    // Set flag so the above data is used.
  PEnset(hPEGrph, PEP_bSHOWANNOTATIONS, TRUE);
    // Increase line annotation text size
  PEnset(hPEGrph, PEP_nLINEANNOTATIONTEXTSIZE, 100); //150);
    // Set a bottom margin so we have enough room to display the label.
  PEszset(hPEGrph, PEP_szBOTTOMMARGIN, "a");
    // Set the colors for the graphs.
  dwArray[0] = RED_RGB; //BLACK_RGB; // Draw the main graph in red.
  dwArray[1] = BLACK_RGB; //LTGRAY_RGB; // Draw respiration graph in light gray.
  PEvset(hPEGrph, PEP_dwaSUBSETCOLORS, dwArray, 2);
    // Don't draw any grid lines.
  PEnset(hPEGrph, PEP_nGRIDLINECONTROL, PEGLC_NONE);
    // Remove the subset legends.
  iCnt = -1;
  PEvset(hPEGrph, PEP_naSUBSETSTOLEGEND, &iCnt, 1);
    
  iLineStyles[0] = PELT_THINSOLID; // Line style for EGG signal.
  iLineStyles[1] = PELT_DOT;  // Line style for respiration.
  PEvset(hPEGrph, PEP_naSUBSETLINETYPES, iLineStyles, 2);

    // Get label for Y-axis.
  //cstrTitle.LoadString(IDS_RAW_WAVFRM_Y_AXIS_LBL);
  cstrTitle = g_pLang->cstrLoad_string(ITEM_G_RAW_WAVFRM_Y_AXIS_LBL);

    // Two graphs are to be drawn in this window (EGG and respiration).
    // Designate which graph uses the top part of the window and which
    // uses the bottom part of the window.
  iMASArray[0] = 1;   // EGG signal is top graph.
  iMASArray[1] = 1;   // Respiration signal is bottom graph.
  PEvset(hPEGrph, PEP_naMULTIAXESSUBSETS, iMASArray, 2);
  
    // Designate axes proportions.  The percentage of the window that
    // each graph uses.
  fMAPArray[0] = (float)0.6; // Percentage of window top graph uses.
  fMAPArray[1] = (float)0.4; // Percentage of window bottom graph uses.
  PEvset(hPEGrph, PEP_faMULTIAXESPROPORTIONS, fMAPArray, 2);

    // Y-axis label for the top graph.
  PEnset(hPEGrph, PEP_nWORKINGAXIS, 0); // Set first y axis parameters
  PEszset(hPEGrph, PEP_szYAXISLABEL, (char *)(const char *)cstrTitle);
    // Set the minimum and maximum Y-axis scale values for EGG.
  PEnset(hPEGrph, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
  dValue = (double)pgi->fMinEGG; // + (double)(pgi->fMinEGG * .1);
  PEvset(hPEGrph, PEP_fMANUALMINY, &dValue, 1);
  dValue = (double)pgi->fMaxEGG;
  PEvset(hPEGrph, PEP_fMANUALMAXY, &dValue, 1);

    // Y-axis label for the bottom graph.
  PEnset(hPEGrph, PEP_nWORKINGAXIS, 1); // Set second y axis parameters
  PEszset(hPEGrph, PEP_szYAXISLABEL, (char *)(const char *)cstrTitle);
    // Set the minimum and maximum Y-axis scale values for Respiration.
  PEnset(hPEGrph, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
  dValue = (double)pgi->fMinResp;
  PEvset(hPEGrph, PEP_fMANUALMINY, &dValue, 1);
  dValue = (double)pgi->fMaxResp; // - (double)(pgi->fMinEGG * .1);
  PEvset(hPEGrph, PEP_fMANUALMAXY, &dValue, 1);

  PEnset(hPEGrph, PEP_nWORKINGAXIS, 0); // Always reset working axis to 0.
    // Set line style separating the two graphs.
  PEnset(hPEGrph, PEP_nMULTIAXESSEPARATORS, PEMAS_THIN);

    // Separate the 2 graphs.
  PEnset(hPEGrph, PEP_nMULTIAXISSTYLE, PEMAS_SEPARATE_AXES);
  PEnset(hPEGrph, PEP_nMULTIAXISSEPARATORSIZE, 65); //75);

  PEnset(hPEGrph, 2944, 1); // Disable clipping

  PEnset(hPEGrph, PEP_nALLOWZOOMING, PEAZ_HORZANDVERT);

  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Set up any Research only graph attributes.
    if(g_bAsciiDataFile == false && g_bHiddenPatients == FALSE)
      { // Not data dump version and not viewing hidden patients.
      // Allow data point hotspots for mouse clicks.
      PEnset(hPEGrph, PEP_bALLOWDATAHOTSPOTS, TRUE);
      }
    }

  PEreinitialize(hPEGrph);
  PEresetimage(hPEGrph, 0, 0);
  }

/********************************************************************
fGet_period_start_min - Get the start minute for the period.

  inputs: Period (0 based) to get the start minute for.

  return: Start minute for the period.
********************************************************************/
float CEGGSASDoc::fGet_period_start_min(unsigned short uPeriod)
  {
/*******************************
  unsigned short uIndex;
  float fStartMin;

  fStartMin = 0;
  for(uIndex = 0; uIndex < uPeriod; ++uIndex)
    fStartMin += m_pdPatient.fPeriodLen[uIndex];
  return(fStartMin);
****************************/
  return(m_pdPatient.fPeriodStart[uPeriod]);
  }

/********************************************************************
vInit_artifact - Initialize the artifact object for the current data file.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vInit_artifact(void)
  {

  if(m_pArtifact != NULL)
    {
    m_pArtifact->vSet_baseline_limit(m_pdPatient.fTestMinEGG, m_pdPatient.fTestMaxEGG);
    if(m_pdPatient.fGoodMinStart[PERIOD_BL] != INV_MINUTE)
      { // Find the max baseline signal during the good minutes just selected.
      m_pdPatient.fMeanBaseline = m_pAnalyze->fFind_mean_signal(m_fData,
                                          m_pdPatient.fGoodMinStart[PERIOD_BL],
                                          m_pdPatient.fGoodMinEnd[PERIOD_BL]);
      m_pArtifact->vSet_artifact_limit(m_pdPatient.fMeanBaseline);
      }
    }
  }

/********************************************************************
vReset_periods_analyzed - Reset the periods analyzed array.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vReset_periods_analyzed()
  {
  short int iIndex;

  for(iIndex = 0; iIndex < MAX_TOTAL_PERIODS; ++iIndex)
    m_pdPatient.bPeriodsAnalyzed[iIndex] = false;
  }

/********************************************************************
bAre_standard_periods_analyzed - Check if the baseline and 3 post-water
                                 periods have been analyzed for the
                                 standard version.

  inputs: None.

  return: true if the 4 periods have been analyzed, otherwise false.
********************************************************************/
bool CEGGSASDoc::bAre_standard_periods_analyzed()
  {
  bool bRet = true;
  short int iIndex;

  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard version
    for(iIndex = 0; iIndex <= PERIOD_30; ++iIndex)
      {
      if(m_pdPatient.bPeriodsAnalyzed[iIndex] == false)
        {
        bRet = false;
        break;
        }
      }
    }
  else
    { // Research version
    for(iIndex = 0; iIndex <= m_pdPatient.uNumPeriods; ++iIndex)
      {
      if(m_pdPatient.bPeriodsAnalyzed[iIndex] == false)
        {
        bRet = false;
        break;
        }
      }
    }
  return(bRet);
  }

/********************************************************************
vChangeMinPerGraph - Display a dialog box allowing the user to change
                     the number of minutes that are displayed on each
                     graph.
                     Update the INI file.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vChangeMinPerGraph()
  {
  CChgMinPerGraph *pdlg;
  int iOrigMinPerGraph;

  iOrigMinPerGraph = m_iMinPerGraph;
  pdlg = new CChgMinPerGraph(this);
  if(pdlg->DoModal() == IDOK)
    {
    m_iMinPerGraph = pdlg->iGet_min_per_graph();
    if(iOrigMinPerGraph != m_iMinPerGraph)
      { // Minutes per graph have changed.  Redisplay the current period.
      m_p10MinView->m_iNumMinOnGraph = m_iMinPerGraph;
      bGet_data_graph(true, m_uMenuType);
      }
    }
  delete pdlg;
  }

/********************************************************************
bIs_study_analyzed - Determine if a research study has been analyzed.
                     All periods must be defined and have good minutes
                     selected.

  inputs: None.

  return: true if the research study has been anaylzed, otherwise false.
********************************************************************/
bool CEGGSASDoc::bIs_study_analyzed()
  {
  bool bRet = true;
  int iIndex;

  if(m_pdPatient.uNumPeriods == 0)
    bRet = false;
  else
    {
    for(iIndex = 0; iIndex < m_pdPatient.uNumPeriods; ++iIndex)
      {
      if(m_pdPatient.fGoodMinStart[iIndex] == INV_MINUTE)
        { // Period has not been analyzed.
        bRet = false;
        break;
        }
      }
    }
  return(bRet);
  }

/********************************************************************
bIs_period_analyzed - Determine if a research study period has been analyzed.
                      The period must be defined and have good minutes
                      selected.

  inputs: None.

  return: true if the research study period has been anaylzed, otherwise false.
********************************************************************/
bool CEGGSASDoc::bIs_period_analyzed(unsigned uPeriod)
  {
  bool bRet = true;

  if(theApp.m_ptStudyType == PT_RESEARCH)
    {
    if(m_pdPatient.uNumPeriods == 0)
      bRet = false;
    else
      {
      if(m_pdPatient.fGoodMinStart[uPeriod] == INV_MINUTE)
        bRet = false; // Period has not been analyzed.
      }
    }
  else
    { // WaterLoad version.
      // Make sure there is at least one period analyzed.
    if(m_pdPatient.fGoodMinStart[PERIOD_BL] != INV_MINUTE
    || m_pdPatient.fGoodMinStart[PERIOD_10] != INV_MINUTE
    || m_pdPatient.fGoodMinStart[PERIOD_20] != INV_MINUTE
    || m_pdPatient.fGoodMinStart[PERIOD_30] != INV_MINUTE
    || m_pdPatient.fGoodMinStart[PERIOD_40] != INV_MINUTE)
      bRet = true;
    else
      bRet = false; // No Periods have been analyzed.
    }
  return(bRet);
  }

/********************************************************************
vSetup_menus - Set up the menus and toolbbar for either a research or
               standard study, whichever a user has selected.
               Only used if the program is the research version.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vSetup_menus()
  {
  CMenu menu;
  unsigned int uMenuID;

  if(theApp.m_ptPgmType == PT_STD_RSCH)
    { // Standard/Research version
    if(theApp.m_ptStudyType == PT_STANDARD)
      {  // standard study.
      uMenuID = IDR_MAINFRAME;
      menu.LoadMenu(IDR_MAINFRAME);
      g_pFrame->SetMenu(&menu);
      g_pFrame->vCreate_standard_toolbar();
      }
    else
      { // research study.
      uMenuID = IDR_MAINFRAME1;
      menu.LoadMenu(IDR_MAINFRAME1);
      g_pFrame->SetMenu(&menu);
      g_pFrame->vCreate_research_toolbar();
      }
    menu.Detach();
    }
  else if(theApp.m_ptPgmType == PT_RESEARCH)
    { // Research version.
    uMenuID = IDR_MAINFRAME1;
    menu.LoadMenu(IDR_MAINFRAME1);
    g_pFrame->SetMenu(&menu);
    g_pFrame->vCreate_research_toolbar();
    menu.Detach();
    }
  else //  if(theApp.m_ptPgmType == PT_STANDARD)
    {
    uMenuID = IDR_MAINFRAME;
    menu.LoadMenu(IDR_MAINFRAME);
    g_pFrame->SetMenu(&menu);
    g_pFrame->vCreate_standard_toolbar();
    menu.Detach();
    }
  // Now set up the text for the menu items.
  if(g_pLang != NULL)
    {
    g_pLang->vLoad_menus(uMenuID);
    g_pFrame->DrawMenuBar();
    }
//#ifdef EGGSAS_READER
#if EGGSAS_READER > READER_NONE
  //// Reader
    // Remove menu items that don't apply to Reader.
    // New study on new patient
  g_pFrame->cstrDelete_menu_item_by_menu_pos(0, ID_FILE_NEW_PATIENT);
    // New study on existing patient
  g_pFrame->cstrDelete_menu_item_by_menu_pos(0, ID_FILE_EXIST_PATIENT);
    // New study menu item.
  g_pFrame->vDelete_menu_item(0, 0);
  // For all reader version delete the Data Dump menu item.
  g_pFrame->vDelete_menu_item(0, 2);
  if(theApp.m_ptStudyType == PT_RESEARCH)
    {
      // Resume study
    g_pFrame->vDelete_menu_item(0, 2);
#if EGGSAS_READER == READER
      // Backup or restore
    g_pFrame->vDelete_menu_item(0, 3);
      // Export study
    g_pFrame->vDelete_menu_item(0, 3);
#elif EGGSAS_READER == READER_DEPOT
          // Export study
    g_pFrame->vDelete_menu_item(0, 4);
#elif EGGSAS_READER == READER_DEPOT_E
    //  // Backup or restore
    //g_pFrame->vDelete_menu_item(0, 3);
      // Export study
    g_pFrame->vDelete_menu_item(0, 4);
#endif
    }
  else
    { // WaterLoad
#if EGGSAS_READER == READER
      // Backup or restore
    g_pFrame->vDelete_menu_item(0, 3);
      // Export study
    g_pFrame->vDelete_menu_item(0, 3);
#elif EGGSAS_READER == READER_DEPOT
          // Export study
    g_pFrame->vDelete_menu_item(0, 4);
#elif EGGSAS_READER == READER_DEPOT_E
    //  // Backup or restore
    //g_pFrame->vDelete_menu_item(0, 3);
      // Export study
    g_pFrame->vDelete_menu_item(0, 4);
#endif
    }
    // Remove the "About 3CPM EGGSAS..." help menu item.
  g_pFrame->vDelete_menu_item(6, 2);
#if EGGSAS_READER == READER_DEPOT
    // Remove the "About 3CPM EGGSAS Reader..." help menu item.
  g_pFrame->vDelete_menu_item(6, 2);
#elif EGGSAS_READER == READER
    // Remove the "About 3CPM EGGSAS Central Data Depot Reader..." help menu item.
  g_pFrame->vDelete_menu_item(6, 3);
#elif EGGSAS_READER == READER_DEPOT_E
    // Remove the "About 3CPM EGGSAS Central Data Depot Reader..." help menu item.
  g_pFrame->vDelete_menu_item(6, 3);
//#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
//  // If version is hiding study dates, remove the menu
//  // item for hiding/showing study dates.
//  g_pFrame->vDelete_menu_item(5, 7); // Separator
//  g_pFrame->vDelete_menu_item(5, 7); // Menu item.
//#endif
#endif
#else
  //// Not reader
  FILE *pFile;
  CString cstrProgram;

  //// Delete import study
  //if(theApp.m_ptStudyType == PT_RESEARCH)
  //  g_pFrame->vDelete_menu_item(0, 7);
  //else
  //  g_pFrame->vDelete_menu_item(0, 6);
    // If the export studies program has not been installed, remove the menu item.
  //cstrProgram.Format("%s\\Program\\%s\\%s",
  //         AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_DATAPATH, ""),
  //         STUDY_EXPORT_DIR, EXPORT_STUDIES_PGM);
  cstrProgram.Format("%s\\Program\\%s\\%s", g_pConfig->cstrGet_item(CFG_DATAPATH),
                     STUDY_EXPORT_DIR, EXPORT_STUDIES_PGM);
  if((pFile = fopen(cstrProgram, "r")) != NULL)
    fclose(pFile);
  else
    { // Program doesn't exist, remove export menu item.
    if(theApp.m_ptStudyType == PT_RESEARCH)
      {
      //g_pFrame->vDelete_menu_item(0, 7); // Import
      g_pFrame->vDelete_menu_item(0, 7); //6);
      }
    else
      {
      //g_pFrame->vDelete_menu_item(0, 6); // Import
      g_pFrame->vDelete_menu_item(0, 6); //5);
      }
    }
    // Remove the "About 3CPM EGGSAS Reader..." help menu item.
  g_pFrame->vDelete_menu_item(6, 3);
    // Remove the "About 3CPM EGGSAS Central Data Depot Reader..." help menu item.
  g_pFrame->vDelete_menu_item(6, 3);

  // If this is not the Raw Data Dump version, delete the Data Dump menu item.
  if(g_bAsciiDataFile == false)
    g_pFrame->vDelete_menu_item(0, 3);

  g_pFrame->cstrDelete_menu_item_by_menu_pos(2, ID_VIEW_FILTER_RSA);
  
//#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
//  // If version is hiding study dates, remove the menu
//  // item for hiding/showing study dates.
//  g_pFrame->vDelete_menu_item(5, 7); // Separator
//  g_pFrame->vDelete_menu_item(5, 7); // Menu item.
//#endif

  //if(g_bHiddenPatients == TRUE)
  //  {
  //  g_pFrame->vEnable_menu_item_by_id(0, 57603, false); // Save patient vChange_menu_item_by_ID(57603, cstrTemp);
  //  if(theApp.m_ptStudyType == PT_RESEARCH)
  //    {
  //    }
  //  else
  //    {
  //    //g_pFrame->vEnable_menu_item(0, 0, false);
  //    }
  //  }
  //else
  //  {
  //  g_pFrame->vEnable_menu_item_by_id(0, 57603, true); // vChange_menu_item_by_ID(57603, cstrTemp);
  //  }

  //  }
  //if(theApp.m_ptPgmType == PT_STANDARD)
  //  {
  //    g_pFrame->vDelete_menu_item(7, 1); //5);
  //  }

#endif

#if EGGSAS_READER == READER_DEPOT
#if EGGSAS_SUBTYPE != SUBTYPE_HIDE_STUDY_DATES
  // If version is hiding study dates, remove the menu
  // item for hiding/showing study dates.
  g_pFrame->vDelete_menu_item(5, 7); // Separator
  g_pFrame->vDelete_menu_item(5, 7); // Menu item.
#endif
#elif EGGSAS_READER == READER_DEPOT_E
#if EGGSAS_SUBTYPE != SUBTYPE_HIDE_STUDY_DATES
  // If version is hiding study dates, remove the menu
  // item for hiding/showing study dates.
  g_pFrame->vDelete_menu_item(5, 7); // Separator
  g_pFrame->vDelete_menu_item(5, 7); // Menu item.
#endif
#else
  g_pFrame->vDelete_menu_item(5, 7); // Separator
  g_pFrame->vDelete_menu_item(5, 7); // Menu item.
#endif

#ifndef _200_CPM_HARDWARE

    //if(theApp.m_ptPgmType == PT_RESEARCH || theApp.m_ptPgmType == PT_STD_RSCH)
     if(theApp.m_ptStudyType == PT_RESEARCH)
     {
      g_pFrame->cstrDelete_menu_item_by_menu_pos(2, ID_VIEW_FILTER_RSA);
      g_pFrame->cstrDelete_menu_item_by_menu_pos(7, ID_SETTINGS_FREQUENCYRANGE);
      g_pFrame->cstrDelete_menu_item_by_menu_pos(7, ID_SETTINGS_FILTERRSA);
      g_pFrame->vHide_toolbar_item(ID_VIEW_FILTER_RSA);
      g_pFrame->vHide_toolbar_item(ID_SETTINGS_FREQUENCYRANGE);
      }
#endif
  }

/********************************************************************
vSetup_menus_fields - Set up the menus and toolbar for either a research or
                      standard study, whichever a user has selected.
                      Fill the combobox with the correct periods
                      Only used if the program is the research version.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vSetup_menus_fields()
  {

  vSetup_menus();
  if(theApp.m_ptStudyType == PT_STANDARD)
    { // 
    m_p10MinView->vBuild_standard_period_cbx();
    }
  }

/********************************************************************
vInit_stim_period_menus - Initialize the Menu IDs for the post-stimulation
                          period menu items.
                      Only used if the program is the research version.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vInit_stim_period_menus()
  {

  memset(m_uViewMenuPostStimPeriodIDs, 0, MENU_ID_ARRAY_SIZE);
  memset(m_uAnalyzeMenuPostStimPeriodIDs, 0, MENU_ID_ARRAY_SIZE);
  m_uNumPeriodMenuItems = 0;
  }

/********************************************************************
vSet_title - Set the title in the Main window's title bar.  Add
             research version if this is the research version of the
             program.

  inputs: String containing the name of the patient or if no patient, emtpy.

  return: None.
********************************************************************/
void CEGGSASDoc::vSet_title(CString cstrName)
  {
  CString cstrTitle;

  SetTitle(cstrName);
  cstrTitle = GetTitle();
//#ifdef EGGSAS_READER
#if EGGSAS_READER == READER
    //cstrTitle = GetTitle();
    cstrTitle += " - Reader";
    //SetTitle(cstrTitle);
#elif EGGSAS_READER == READER_DEPOT
    //cstrTitle = GetTitle();
    cstrTitle += " - Central Data Depot Reader";
    //SetTitle(cstrTitle);
#elif EGGSAS_READER == READER_DEPOT_E
    cstrTitle += " - Central Data Depot Reader Enabled";
#else
  if(theApp.m_ptPgmType == PT_RESEARCH)
    {
    //cstrTitle = GetTitle();
    cstrTitle += " - Research Version";
    //SetTitle(cstrTitle);
    }
  else if(theApp.m_ptPgmType == PT_STD_RSCH)
    {
    //cstrTitle = GetTitle();
    cstrTitle += " - Research - Waterload Version";
    //SetTitle(cstrTitle);
    }
  if(g_bAsciiDataFile == true)
    cstrTitle += " (Raw Data Dump)";
#endif
  SetTitle(cstrTitle);
  }

/********************************************************************
vFree_event_list - Free the objects in the specified event list and
                   then free the pointers in the event list.

  inputs: Pointer to an event list.

  return: None.
********************************************************************/
void CEGGSASDoc::vFree_event_list(CEvntArray &Events)
  {
  int iIndex;
  CEvnt *pEvent;

  for(iIndex = 0; iIndex < Events.GetCount(); ++iIndex)
    {
    pEvent = Events[iIndex];
    delete pEvent;
    }
  Events.RemoveAll();
  }

/***********************************************************************
bGoToCmdMsg - Called by the MainFrm object when it has to route and dispatch
              command messages with variable command IDs.  The variable
              command IDs are those for the post-stimulation periods in the
              View and Analyze menus.

inputs: command ID
        command notification code
        used according to the value of nCode
        if not NULL, filled in with pTarget and pmf members of
          CMDHANDLERINFO structure, instead of dispatching the
          command.  Typically NULL.

Returns: TRUE if message is handled, otherwise FALSE.
***********************************************************************/
BOOL CEGGSASDoc::bGoToCmdMsg(UINT nID, int nCode, void* pExtra,
							AFX_CMDHANDLERINFO* pHandlerInfo)
  {
  unsigned short uCnt, uPrevPeriodOrig;
  bool bHandled = false;

  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Only for research version.
      // If pHandlerInfo is NULL, then handle the message
    if (pHandlerInfo == NULL
    && nID >= MENU_ID_START && nID <= (MENU_ID_START + (MENU_ID_ARRAY_SIZE * 2)))
      {  // Filter the commands for the View and Anaylze post-stimulation periods.
      for(uCnt = 0; uCnt < m_uNumPeriodMenuItems; ++uCnt)
        {
        if(m_uViewMenuPostStimPeriodIDs[uCnt] == nID)
          { // View menu item.
          if (nCode == CN_COMMAND)
            {
            if(g_pFrame->m_uViewID != VIEWID_TEN_MIN)
              g_pFrame->vActivate_ten_min_view();

            vGet_view();
            uPrevPeriodOrig = m_uPreviousPeriod;
            m_uPreviousPeriod = m_uCurrentPeriod;
            m_uCurrentPeriod = uCnt + 1;
            m_uMenuType = SM_VIEW;
            if(bGet_data_graph(true, SM_VIEW) == false)
              { // Didn't graph the period, restore the current and previous periods.
              m_uCurrentPeriod = m_uPreviousPeriod;
              m_uPreviousPeriod = uPrevPeriodOrig;
              }
            bHandled = true;
            break;
            }
          else if (nCode == CN_UPDATE_COMMAND_UI)
            {
            CCmdUI *pCmdUI;
            pCmdUI = (CCmdUI *)pExtra;
            pCmdUI->Enable(m_bHaveFile); // && m_pdPatient.bPeriodsAnalyzed[m_uCurrentPeriod]);
            bHandled = true;
            break;
            }
          }
        else if(m_uAnalyzeMenuPostStimPeriodIDs[uCnt] == nID)
          { // Analyze menu item.
          if (nCode == CN_COMMAND)
            {
            if(g_pFrame->m_uViewID != VIEWID_TEN_MIN)
              g_pFrame->vActivate_ten_min_view();
            vGet_view();
            uPrevPeriodOrig = m_uPreviousPeriod;
            m_uPreviousPeriod = m_uCurrentPeriod;
            m_uCurrentPeriod = uCnt + 1;
            m_uMenuType = SM_EDIT;
            if(bGet_data_graph(true, SM_EDIT) == false)
              {
              m_uCurrentPeriod = m_uPreviousPeriod;
              m_uPreviousPeriod = uPrevPeriodOrig;
              }
            bHandled = true;
            break;
            }
          else if (nCode == CN_UPDATE_COMMAND_UI)
            {
            CCmdUI *pCmdUI;
            pCmdUI = (CCmdUI *)pExtra;
            if(g_bAsciiDataFile == true)
              pCmdUI->Enable(false);
            else
              {
              if(g_bHiddenPatients == FALSE)
                pCmdUI->Enable(m_bHaveFile);
              else
                pCmdUI->Enable(FALSE);
              //pCmdUI->Enable(m_bHaveFile); //m_pdPatient.bPeriodsAnalyzed[uCnt]);
              }
              // If post-stimulation period has been analyzed, check the menu item.
            if(m_pdPatient.bPeriodsAnalyzed[uCnt + 1] == true)
              pCmdUI->SetCheck(1);
            else
              pCmdUI->SetCheck(0);
            bHandled = true;
            break;
            }
          }
        } // End of for loop
      }
    }
  return(bHandled);
  }

/***********************************************************************
vCleanup_events_db - Remove any events in the database for this study that
                     aren't in the memory list. If there are events in the
                     database that are not in memory (user deleted them),
                     delete them from the database.

inputs: None.

Returns: None.
***********************************************************************/
void CEGGSASDoc::vCleanup_events_db()
  {
  CEvnt EventDB, *pEventMem;
  int iIndex;
  long lStudyID;
  bool bDelete;

  lStudyID = m_pdPatient.lStudyID;
  while(true)
    {
    if(m_pDB->iGet_event(lStudyID, &EventDB) == SUCCESS)
      { // Got an event for this study from the database.
      bDelete = true;
        // See if its in the memory list for the period type.
      if(EventDB.m_iPeriodType == EVENT_BASELINE)
        {
        for(iIndex = 0; iIndex < m_pdPatient.Events.m_BLEvents.GetCount(); ++iIndex)
          {
          pEventMem = m_pdPatient.Events.m_BLEvents[iIndex];
          if(EventDB.m_iDataPointIndex == pEventMem->m_iDataPointIndex)
            { // Found database event in memory.
            bDelete = false;
            break;
            }
          }
        }
      else
        {
        for(iIndex = 0; iIndex < m_pdPatient.Events.m_PSEvents.GetCount(); ++iIndex)
          {
          pEventMem = m_pdPatient.Events.m_PSEvents[iIndex];
          if(EventDB.m_iDataPointIndex == pEventMem->m_iDataPointIndex)
            { // Found database event in memory.
            bDelete = false;
            break;
            }
          }
        }
      if(bDelete == true)
        { // Delete the event from the database.
        m_pDB->iDelete_event(m_pdPatient.lStudyID, EventDB.m_iDataPointIndex,
                             EventDB.m_iPeriodType);
        }
      }
    else
      break;
    lStudyID = INV_LONG; // Get the next event.
    }
  }

/********************************************************************
vWhatIf - Provides functionality for What If Scenarios.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vWhatIf(long lWhatIfID) 
  {
  CWhatIf *pdlg;
  CString cstrDesc, cstrMsg, cstrTitle;
  int iState = WIFS_NONE, iRet;
  bool bSetUpMenus = false;
//  long lWhatIfID;
	
    // Check to see if the What If scenario or the original study has changed
    // and if so, ask user if it should be saved.  If yes, save it.
  if(lWhatIfID == INV_LONG)
    iRet = iCheck_to_Save(); // From WhatIF dialog box.
  else
    iRet = IDYES; // From Select Patient.
  if(iRet != IDCANCEL)
    { // Either no changes to last what if/original, user saved changes,
      // or user didn't want to save the changes.
    if(m_bUsingWhatIf == false)
      { // If the original study is displayed, save the data that will
        // get overwritten by the What If data.
      m_wifOrigStudyData.uNumPeriods = m_pdPatient.uNumPeriods;
      memcpy(m_wifOrigStudyData.fPeriodStart, m_pdPatient.fPeriodStart,
             sizeof(m_pdPatient.fPeriodStart));
      memcpy(m_wifOrigStudyData.fPeriodLen, m_pdPatient.fPeriodLen,
             sizeof(m_pdPatient.fPeriodLen));
      memcpy(m_wifOrigStudyData.fGoodMinStart, m_pdPatient.fGoodMinStart,
             sizeof(m_pdPatient.fGoodMinStart));
      memcpy(m_wifOrigStudyData.fGoodMinEnd, m_pdPatient.fGoodMinEnd,
             sizeof(m_pdPatient.fGoodMinEnd));
      memcpy(m_wifOrigStudyData.uRespRate, m_pdPatient.uRespRate,
             sizeof(m_pdPatient.uRespRate));
      }
    if(lWhatIfID == INV_LONG)
      {
      if(m_bUsingWhatIf == false)
        lWhatIfID = INV_LONG;
      else
        lWhatIfID = m_wifOrigData.lWhatIfID;
      pdlg = new CWhatIf(this, lWhatIfID);
      if(pdlg->DoModal() == IDOK)
        {
        if(g_pFrame->m_uViewID != VIEWID_TEN_MIN)
          g_pFrame->vActivate_ten_min_view();
        iState = pdlg->iGet_state();
        vInit_stim_period_menus();
        vSetup_menus();
        vGet_view();
        if(m_p10MinView != NULL)
          m_p10MinView->vAdd_menu_items_for_period();
        if(iState == WIFS_ORIGINAL)
          bSetUpMenus = true;
        }
      delete pdlg;
      }
    else
      iState = WIFS_OTHER;
    if(iState == WIFS_OTHER)
      {
      m_bUsingWhatIf = true;
      }
    else if(iState == WIFS_ORIGINAL)
      { // Original study.
      m_bUsingWhatIf = false;
        // Load the original study.
      cstrDesc = "";
        // Read in the study data to replace data modified by the
        // What If Scenarios.
      m_pDB->iGet_study_data(&m_pdPatient);
        // Copy the parts of the study data that were overwritten by the
        // what if's to the saved data.
      m_pdSavedPatient.uNumPeriods = m_pdPatient.uNumPeriods;
      memcpy(m_pdSavedPatient.fPeriodStart, m_pdPatient.fPeriodStart,
             sizeof(m_pdPatient.fPeriodStart));
      memcpy(m_pdSavedPatient.fPeriodLen, m_pdPatient.fPeriodLen,
             sizeof(m_pdPatient.fPeriodLen));
      memcpy(m_pdSavedPatient.fGoodMinStart, m_pdPatient.fGoodMinStart,
             sizeof(m_pdPatient.fGoodMinStart));
      memcpy(m_pdSavedPatient.fGoodMinEnd, m_pdPatient.fGoodMinEnd,
             sizeof(m_pdPatient.fGoodMinEnd));
      memcpy(m_pdSavedPatient.uRespRate, m_pdPatient.uRespRate,
             sizeof(m_pdPatient.uRespRate));
        // Set the periods analyzed or not analyzed.
      vSet_rsrch_pds_analyzed();
      if(bSetUpMenus == true)
        { // Reloading the original study from the dialog box.
        if(m_p10MinView != NULL)
          m_p10MinView->vAdd_menu_items_for_period();
        }
      }
    if(iState == WIFS_OTHER || iState == WIFS_ORIGINAL)
      { // Post a message to display the What If.  Do this because
        // if a report is being displayed, Windows needs time to
        // change the display from the report to the 10 min view.
      AfxGetMainWnd()->PostMessage(WMUSER_DISPLAY_WHATIF, 0, 0);
      }
    }
  }

/********************************************************************
vRemove_whatif_from_display - Removes the fields used to display the
                              What If information from the View.
                              Sets a class flag to false to indicate that
                              a What If scenario is no longer being displayed.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vRemove_whatif_from_display()
  {
  CString cstrDesc;

    if(m_bUsingWhatIf == true)
      { // Previous use of a What If scenario, remove it.
      m_bUsingWhatIf = false;
      if(m_p10MinView != NULL)
        {
        cstrDesc = "";
        m_p10MinView->vShow_WhatIf(false, cstrDesc);
        }
      }
  }

/********************************************************************
iCompare_whatif - Compares the originally loaded What If data to the
                  current data.

  inputs: Pointer to the PATIENT_DATA structure containing the current
            data for the What If scenario.
          Pointer to a WHAT_IF structure containing the original data
            of the What If scenario.

  return: SUCCESS if the 2 sets of data are equal.
          FAIL if any one item in the 2 sets of data is different.
********************************************************************/
short int CEGGSASDoc::iCompare_whatif(PATIENT_DATA *ppd, WHAT_IF *pwif)
  {
  short int iRet = SUCCESS;

  if(memcmp(ppd->fPeriodStart, pwif->fPeriodStart, sizeof(pwif->fPeriodStart)) != 0
  || memcmp(ppd->fPeriodLen, pwif->fPeriodLen, sizeof(pwif->fPeriodLen)) != 0
  || memcmp(ppd->fGoodMinStart, pwif->fGoodMinStart, sizeof(pwif->fGoodMinStart)) != 0
  || memcmp(ppd->fGoodMinEnd, pwif->fGoodMinEnd, sizeof(pwif->fGoodMinEnd)) != 0
  || memcmp(ppd->uRespRate, pwif->uRespRate, sizeof(pwif->uRespRate)) != 0
  || ppd->uFrequency != pwif->uFrequency)
    iRet = FAIL;
  else
    iRet = SUCCESS;
  return(iRet);
  }

/********************************************************************
vFileSave - Save the What If data in the appropriate database table.
                         
  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vFileSaveWhatIf()
  {
  WHAT_IF wi;

  if(m_pDB != NULL)
    {
      // Display the hourglass cursor.
    SetCursor(LoadCursor(NULL, IDC_WAIT));
    ShowCursor(TRUE);

    wi.uNumPeriods = m_pdPatient.uNumPeriods;
    memcpy(wi.fPeriodStart, m_pdPatient.fPeriodStart, sizeof(wi.fPeriodStart));
    memcpy(wi.fPeriodLen, m_pdPatient.fPeriodLen, sizeof(wi.fPeriodLen));
    memcpy(wi.fGoodMinStart, m_pdPatient.fGoodMinStart, sizeof(wi.fGoodMinStart));
    memcpy(wi.fGoodMinEnd, m_pdPatient.fGoodMinEnd, sizeof(wi.fGoodMinEnd));
    memcpy(wi.uRespRate, m_pdPatient.uRespRate, sizeof(wi.uRespRate));
    wi.cstrDateTime = m_wifOrigData.cstrDateTime;
    wi.cstrDescription = m_wifOrigData.cstrDescription;
    wi.lStudyID = m_wifOrigData.lStudyID;
    wi.lWhatIfID = m_wifOrigData.lWhatIfID;
    wi.uFrequency = m_pdPatient.uFrequency;
    if(m_pDB->iSave_whatif_data(&wi) == FAIL)
      {
      CString cstrMsg, cstrTitle;

      //cstrMsg.LoadString(IDS_FAILED_TO_SAVE_WHATIF);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_FAILED_TO_SAVE_WHATIF);
      //cstrTitle.LoadString(IDS_ERROR1);
      cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
      AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrTitle, MB_ICONERROR);
      }
    else
      { // Save successful.
        // Copy the data over to the original what if data.
      m_wifOrigData.uNumPeriods = m_pdPatient.uNumPeriods;
      m_wifOrigData.uFrequency = m_pdPatient.uFrequency;
      memcpy(m_wifOrigData.fPeriodStart, m_pdPatient.fPeriodStart,
             sizeof(m_pdPatient.fPeriodStart));
      memcpy(m_wifOrigData.fPeriodLen, m_pdPatient.fPeriodLen,
             sizeof(m_pdPatient.fPeriodLen));
      memcpy(m_wifOrigData.fGoodMinStart, m_pdPatient.fGoodMinStart,
             sizeof(m_pdPatient.fGoodMinStart));
      memcpy(m_wifOrigData.fGoodMinEnd, m_pdPatient.fGoodMinEnd,
             sizeof(m_pdPatient.fGoodMinEnd));
      memcpy(m_wifOrigData.uRespRate, m_pdPatient.uRespRate, sizeof(m_pdPatient.uRespRate));
      }

      // Restore the arrow cursor.
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    ShowCursor(TRUE);
    }
  }

/********************************************************************
vSet_rsrch_pds_analyzed - Set the periods analyzed based on the
                          Good minutes.
                         
  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vSet_rsrch_pds_analyzed()
  {
  int iIndex;

  for(iIndex = 0; iIndex < MAX_TOTAL_PERIODS; ++iIndex)
    {
    if(m_pdPatient.fGoodMinStart[iIndex] != INV_MINUTE)
      m_pdPatient.bPeriodsAnalyzed[iIndex] = true;
    else
      m_pdPatient.bPeriodsAnalyzed[iIndex] = false;
    }
  }

/********************************************************************
vDisplay_what_if - Display the What If scenario or the orginal study data
                   specified in the What If dialog box.
                   This is called as a result of the Window user
                   message WMUSER_DISPLAY_WHATIF.
                         
  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vDisplay_what_if()
  {
  CString cstrDesc;

  vGet_view();
#if EGGSAS_READER == READER_DEPOT
  if(m_bUsingWhatIf == true)
    m_uMenuType = SM_EDIT;
  else
    m_uMenuType = SM_VIEW;
#endif
  m_uCurrentPeriod = PERIOD_BL;
  m_uPreviousPeriod = m_uCurrentPeriod;
  bGet_data_graph(true, m_uMenuType);
  if(m_pdPatient.bPeriodsAnalyzed[m_uCurrentPeriod] == true)
    {
    m_p10MinView->m_bSetPeriodLength = TRUE;
    m_p10MinView->m_bSetGoodMinutes = TRUE;
    }
  else
    {
    if(m_uCurrentPeriod != PERIOD_BL)
      m_p10MinView->m_bSetPeriodLength = FALSE;
    m_p10MinView->m_bSetGoodMinutes = FALSE;
    }
  if(m_bUsingWhatIf == true)
    m_p10MinView->vShow_WhatIf(true, m_wifOrigData.cstrDescription);
  else
    {
    cstrDesc = "";
    m_p10MinView->vShow_WhatIf(false, cstrDesc);
    }
  }

/********************************************************************
cstrSelect_reader_db - Display a File Open dialog box for user to select
                       the reader database to use.

  inputs: Pointer to a path where the database exists.
            If path is empty, use the Export DSN.

  return: CString object containing the path and name of the database.
          CString object is empty if user didn't select a database.
********************************************************************/
CString CEGGSASDoc::cstrSelect_reader_db(LPCTSTR pszFileName)
  {
  CFileDialog *pdlg;
  CString cstrPath, cstrFile;
  int iIndex;
  char szCurDir[1024];

  GetCurrentDirectory(1024, szCurDir);
    // Get the path to start the Open File dialog box with.  If there is no last
    // path the user selected, use the data path.
  //cstrPath = AfxGetApp()->GetProfileString(INI_SEC_READER, INI_ITEM_READER_DB_PATH, "");
  cstrPath = g_pConfig->cstrGet_item(CFG_READERDBPATH);
  if(cstrPath.IsEmpty() == TRUE)
    cstrPath = "C:"; //AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_DATAPATH, "");


  if(pszFileName != "")
    cstrFile = pszFileName;
  else
    cstrFile = EXP_DSN_DEFAULT;

     // Create an open file dialog box. Operator can select only a single file.
  pdlg = new CFileDialog(TRUE, ".mdb", cstrFile, 
                         OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, 
                         "Patient databases (*.mdb)|*.mdb|All Files (*.*)|*.*||");
  pdlg->m_pOFN->lpstrInitialDir = cstrPath;
  if(pdlg->DoModal() == IDOK)
    { // User selected a database.
    cstrPath = pdlg->GetPathName();
      // Save the reader database path for the next time.
    if((iIndex = cstrPath.ReverseFind('\\')) > 0)
      {
      //AfxGetApp()->WriteProfileString(INI_SEC_READER, INI_ITEM_READER_DB_PATH, 
      //                                cstrPath.Left(iIndex));
      g_pConfig->vWrite_item(CFG_READERDBPATH, cstrPath.Left(iIndex));
      }
    }
  else
    cstrPath = "";
  delete pdlg;
    // Restore the directory we started from.  If backup/restore used a USB drive
    // this releases the USB drive so it can be removed.
  SetCurrentDirectory(szCurDir);
  return(cstrPath);
  }

/*******************************************************************
vChange_db - Change program's database.

  inputs: What to change database to, local or remote.
          CString object containing the path and name of remote database.
            Empty string if local database.

  return: None.
********************************************************************/
void CEGGSASDoc::vChange_db(int iDB, LPCSTR strDB)
  {
  
  if(iDB == LOCAL_DB)
    { // Local database
    delete m_pDB;
    m_pDB = new CDbaccess(DSN_DEFAULT);
    }
  else
    { // Remote database
    delete m_pDB;
    m_pDB = new CDbaccess(strDB);
    }
  }

/********************************************************************
vImport_reader_db - Import a reader database into the local database.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vImport_reader_db()
  {
  CImportRemoteDB *pImportDB;

  pImportDB = new CImportRemoteDB(this);
  pImportDB->DoModal();
  delete pImportDB;
  }

/********************************************************************
iCheck_db - Check the database to make sure its OK.
            Checks:
              - The version in the database is the same as the version
                this program is expecting.

  inputs: None.

  return: SUCCESS if database is OK.
          FAIL if something is wrong with the database.
********************************************************************/
short CEGGSASDoc::iCheck_db()
  {
  short iRet = FAIL;

  if(m_pDB != NULL)
    iRet = m_pDB->iValidate_version();

  return(iRet);
  }

/********************************************************************
vDisplay_version_error - Display an error message telling that the
                         database version number is wrong.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vDisplay_version_error(CDbaccess *pDB)
  {
  CString cstrVersion, cstrMsg, cstrMsg1, cstrTitle;

  //cstrTitle.LoadString(IDS_ERROR1);
  cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
  if(pDB != NULL)
    {
    if(pDB->iGet_version(cstrVersion) == SUCCESS)
      {
      //cstrMsg.Format(IDS_DB_VERSION_WRONG, cstrVersion, DB_VERSION);
      cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_DB_VERSION_WRONG), cstrVersion, DB_VERSION);
     }
    else
      {
      //cstrMsg.LoadString(IDS_DB_ACCESS_ERR);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_DB_ACCESS_ERR);
      }
    //cstrMsg1.LoadString(IDS_CONTACT_SUPPORT);
    cstrMsg1 = g_pLang->cstrLoad_string(ITEM_G_CONTACT_SUPPORT);
    cstrMsg += "\r\n\r\n" + cstrMsg1;
    AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrTitle, MB_ICONERROR);
    }
  }

/********************************************************************
vSave_ascii_data_file - Save the raw data in an ASCII file.
                        See top of file for a description of the file format.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vSave_ascii_data_file()
  {

  CRawDataDump *prdd = new CRawDataDump(this);
  prdd->vSave_ascii_data_file();
  delete prdd;
  }

/********************************************************************
vClose_data_file - Close the data file.  Copy the data file to the
                   data storage location if indicated by the input.

  inputs: true to copy the data file to the storage path, otherwise false.

  return: None.
********************************************************************/
void CEGGSASDoc::vClose_data_file(bool bCopyToDataFilePath)
  {
  CString cstrStoragePath, cstrDataPath;

  if(m_pcfDataFile != NULL)
    { // Currently have an open data file.  Close it.
    if(bCopyToDataFilePath == true)
      { // Copy the data file to the data file storage location.
      cstrStoragePath = g_pConfig->cstrGet_item(CFG_DATA_FILE_STORAGE);
      cstrDataPath = g_pConfig->cstrGet_item(CFG_DATAPATH);
      // Only copy to storage folder if there is one and its different than the data folder.
      if(cstrStoragePath != "" && cstrStoragePath.CompareNoCase(cstrDataPath) != 0)
        {
        // Copy the data file
        iCopy_one_file((LPCTSTR)cstrStoragePath, (LPCTSTR)cstrDataPath,
          m_pcfDataFile->GetFileName(), FALSE);
        }
      }
    m_pcfDataFile->Close(); // Close the file.
    delete m_pcfDataFile;
    m_pcfDataFile = NULL;

#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
  if(g_pConfig->m_bHideStudyDates == true)
    {
    // Set the file time to an earlier time so user won't be able
    // to tell if the date of the study from the file time.
    vSet_file_date_time(m_pdPatient.cstrFile);
    }
#endif
    }
  }
/********************************************************************
OnSettings - Handler for the Settings menu item.

    Display the settings dialog box.
    If the language has changed, reload the menus.

********************************************************************/
void CEGGSASDoc::OnSettings()
  {
  CString cstrLanguage;

  // TODO: Add your command handler code here
  // Save the current language first so we can see if it changed.
  cstrLanguage = g_pLang->cstrGet_language();
  CSettings *pdlg = new CSettings(this);
  pdlg->DoModal();
  delete pdlg;

  if(cstrLanguage != g_pLang->cstrGet_language())
    vSetup_menus();
  }

//void CEGGSASDoc::OnUpdateSettings(CCmdUI *pCmdUI)
//  {
//  // TODO: Add your command update UI handler code here
//  }

/********************************************************************
OnSettingsFrequencyrange - Handler for the Settings menu item Frequency Range.

    Display the Frequency Range dialog box.
    Set the frequency range based on the user selection.

********************************************************************/
void CEGGSASDoc::OnSettingsFrequencyrange()
  {
#ifdef _200_CPM_HARDWARE
  if(theApp.m_ptPgmType == PT_RESEARCH || theApp.m_ptPgmType == PT_STD_RSCH)
        {
        int iFreqRange, iOrigFreqRange;
        iOrigFreqRange = m_pdPatient.uFrequency; //g_pConfig->iGet_item(CFG_FREQ_RANGE);
        //theApp.m_bFirstTimeInTenMinView = false;
        CFreqRange *pdlg = new CFreqRange();
        pdlg->vSet_data(theApp.m_iFreqLimit); //OrigFreqRange);
        if(pdlg->DoModal() == IDOK)
          {
          iFreqRange = pdlg->iGet_freq_range();
          if(iFreqRange == FREQ_LIMIT_15_CPM)
            m_pdPatient.uFrequency = 15;
          else if(iFreqRange == FREQ_LIMIT_60_CPM)
            m_pdPatient.uFrequency = 60;
          else
            m_pdPatient.uFrequency = 200;
          theApp.vSet_freq_range(iFreqRange);
          if(iOrigFreqRange != m_pdPatient.uFrequency)
            vUpdate_display_for_RSA();
          if(m_p10MinView != NULL)
            {
            if(g_pFrame->m_uViewID == VIEWID_TEN_MIN)
              m_p10MinView->vShow_study_type();
            }
         if(g_pFrame->m_uViewID == VIEWID_REPORT)
           {
           if(m_uReportItemToView == RPT_VIEW_ITEM_DATA_SHEET)
             { // Viewing a datasheet.
             m_bReAnalyze = true; // force recalculation.
             g_pFrame->OnViewDataSheet();
             }
           else if(m_uReportItemToView == RPT_VIEW_ITEM_SUMM_GRPHS)
             { // Viewing summary graphs.
             m_bReAnalyze = true; // force recalculation.
             g_pFrame->OnViewSummGraph();
             }
           else
             { // viewing the report.
             m_bReAnalyze = true; // force recalculation.
             g_pFrame->vView_report();
             }
           }

          }
        delete pdlg;
        }
#endif
  }

/********************************************************************
bCan_graph_summary_graphs - Determine if summary graphs can be drawn
                            for a research study.

        Criteria: The first 3 postwater periods must be 10 minutes in
                  length and have been analyzed.

  inputs: None.

  return: true if the summary graphs can be drawn, otherwise false.
********************************************************************/
bool CEGGSASDoc::bCan_graph_summary_graphs()
  {
  bool bRet = false;


  if(m_pdPatient.fPeriodLen[1] == 10.0F && m_pdPatient.fPeriodLen[2] == 10.0F
    && m_pdPatient.fPeriodLen[3] == 10.0F)
    {
    if(bAre_standard_periods_analyzed() == true)
      bRet = true; // First 3 postwater periods are 10 minutes in lentgh.
    }
  return(bRet);
  }

/********************************************************************
OnViewFilterRsa - Handler for the View menu item Filter RSA.

    Toggles the Filter RSA flag.

********************************************************************/
void CEGGSASDoc::OnViewFilterRsa()
  {
  // Define a global flag to force filtering out frequencies below 10cpm.
  // This routine will toggle it on and off.
  if(g_bFilterRSA == false)
    g_bFilterRSA = true;
  else
    g_bFilterRSA = false;

  vUpdate_display_for_RSA();
  //if(g_pFrame->m_uViewID == VIEWID_RSA)
  //  { // Currently viewing the RSA only, refresh the RSA with the changed filtering.
  //  g_pFrame->m_uViewID = VIEWID_NONE;
  //  g_pFrame->OnAnalyzeRsa();
  //  }
  //else if(g_pFrame->m_uViewID == VIEWID_REPORT)
  //  { // Currently viewing the report.
  //  //If only one RSA is in the report, refresh it with the changed filtering.
  //  if(g_pConfig->iGet_item(CFG_BOTH_RSA) != 1)
  //    {
  //    g_pFrame->m_uViewID = VIEWID_NONE;
  //    g_pFrame->vView_report();
  //    }
  //  }
  }

/********************************************************************
OnUpdateViewFilterRsa - Handler for the updating View menu item Filter RSA.

    Enable/disable the menu item.

********************************************************************/
void CEGGSASDoc::OnUpdateViewFilterRsa(CCmdUI *pCmdUI)
  {
  pCmdUI->Enable(m_bHaveFile == TRUE && bIs_period_analyzed(m_uCurrentPeriod));
  }

/********************************************************************
OnSettingsFilterrsa - Handler for the Settings menu item for Filter RSA.

    Display the Filter RSA dialog box.
    Display a check mark on the menu item if filtering is enabled.

********************************************************************/
void CEGGSASDoc::OnSettingsFilterrsa()
  {
  CFilterRSA *pDlg;
  bool bOrigFilterRSA;

  bOrigFilterRSA = g_bFilterRSA;
  // Start the dialog box that allows user to specify filtered RSA
  pDlg = new CFilterRSA();
  pDlg->DoModal();
  delete pDlg;
  // If filtered, place a check mark next to the menu item.
  g_pFrame->vCheck_menu_item(ID_SETTINGS_FILTERRSA, g_bFilterRSA);

  if(bOrigFilterRSA != g_bFilterRSA)
    { // Filtering changed, update the display if we need to.
    vUpdate_display_for_RSA();
    //if(g_pFrame->m_uViewID == VIEWID_RSA)
    //  { // Currently viewing the RSA only, refresh the RSA with the changed filtering.
    //  g_pFrame->m_uViewID = VIEWID_NONE;
    //  g_pFrame->OnAnalyzeRsa();
    //  }
    //else if(g_pFrame->m_uViewID == VIEWID_REPORT)
    //  { // Currently viewing the report.
    //  //If only one RSA is in the report, refresh it with the changed filtering.
    //  if(g_pConfig->iGet_item(CFG_BOTH_RSA) != 1)
    //    {
    //    g_pFrame->m_uViewID = VIEWID_NONE;
    //    g_pFrame->vView_report();
    //    }
    //  }
    }
  }

/********************************************************************
OnUpdateViewFilterRsa - Handler for the updating Settings menu item 
                        for Filter RSA.

    Display a check mark on the menu item if filtering is enabled.

********************************************************************/
void CEGGSASDoc::OnUpdateSettingsFilterrsa(CCmdUI *pCmdUI)
  {

  if(theApp.m_ptStudyType == PT_RESEARCH)
    g_pFrame->vCheck_menu_item(ID_SETTINGS_FILTERRSA, g_pConfig->m_bFilterRSA);
  }

/********************************************************************
vUpdate_display_for_RSA - Udate the display if the RSA or the report is
                          currently displayed.

                          Used for 60/200 cpm hardware.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vUpdate_display_for_RSA()
  {

  if(g_pFrame->m_uViewID == VIEWID_RSA)
    { // Currently viewing the RSA only, refresh the RSA with the changed filtering.
    g_pFrame->m_uViewID = VIEWID_NONE;
    g_pFrame->OnAnalyzeRsa();
    }
  else if(g_pFrame->m_uViewID == VIEWID_REPORT && m_uReportItemToView == RPT_VIEW_ITEM_NONE)
    { // Currently viewing the report.
    //If only one RSA is in the report, refresh it with the changed filtering.
    if(g_pConfig->iGet_item(CFG_BOTH_RSA) != 1)
      {
      g_pFrame->m_uViewID = VIEWID_NONE;
      g_pFrame->vView_report();
      }
    }
  }

void CEGGSASDoc::OnViewViewhiddenpatientfiles()
  {
  // TODO: Add your command handler code here
    if(g_pFrame->m_uViewID != VIEWID_TEN_MIN)
    g_pFrame->vActivate_ten_min_view();

  iSelectPatient(STUDY_NONE, INV_LONG, TRUE);
  }

void CEGGSASDoc::OnUpdateViewViewhiddenpatientfiles(CCmdUI *pCmdUI)
  {
  // TODO: Add your command update UI handler code here
  pCmdUI->Enable(TRUE);
  }

void CEGGSASDoc::OnUpdateSettingsFrequencyrange(CCmdUI *pCmdUI)
  {
  // Enable the menu item for a whatif only.
  if(theApp.m_ptStudyType == PT_RESEARCH && m_bUsingWhatIf == true)
    pCmdUI->Enable(TRUE);
  else
    pCmdUI->Enable(FALSE);
  }


/********************************************************************
vSet_freq_range - Set the frequency range.

  inputs: Frequency range (15,60,2000)

  return: None.
********************************************************************/
void CEGGSASDoc::vSet_freq_range(int iFreqRange)
  {
  int iFreq;

  // Set the frequency range.
  if(iFreqRange == 15)
    iFreq = FREQ_LIMIT_15_CPM;
  else if(iFreqRange== 60)
    iFreq = FREQ_LIMIT_60_CPM;
  else
    iFreq = FREQ_LIMIT_200_CPM;
  theApp.vSet_freq_range(iFreq);
  }

/********************************************************************
bGet_study_type_freq_range - Get the recording frequency range and study type.

  inputs: None.

  return: None.
********************************************************************/
bool CEGGSASDoc::bGet_study_type_freq_range()
  {
  bool bRet = false;

#ifdef _200_CPM_HARDWARE
  int iFreqRange;
  // Restore RSA filtering.
  g_bFilterRSA = g_pConfig->m_bFilterRSA;
  if(theApp.m_ptPgmType == PT_STD_RSCH)
    {
    CSelStudyTypeFreq *pdlg = new CSelStudyTypeFreq();
    if(pdlg->DoModal() == IDOK)
      {
      bRet = true;
      iFreqRange = pdlg->uGet_freq_range_id();
      m_pdPatient.uFrequency = pdlg->uGet_freq_range();
      theApp.m_ptStudyType = pdlg->ptGet_study_type();
      }
    delete pdlg;
    }
  else
    {
    CFreqRange *pdlg = new CFreqRange();
    pdlg->vSet_data(theApp.m_iFreqLimit);
    if(pdlg->DoModal() == IDOK)
      {
      bRet = true;
      iFreqRange = pdlg->iGet_freq_range();
      if(iFreqRange == FREQ_LIMIT_15_CPM)
        m_pdPatient.uFrequency = 15;
      else if(iFreqRange == FREQ_LIMIT_60_CPM)
        m_pdPatient.uFrequency = 60;
      else
        m_pdPatient.uFrequency = 200;
      }
    }
  if(bRet == true)
    {
    g_pConfig->vWrite_item(CFG_FREQ_RANGE, iFreqRange);
    theApp.vSet_freq_range(iFreqRange);
    if(m_p10MinView != NULL)
      {
      if(g_pFrame->m_uViewID == VIEWID_TEN_MIN)
        m_p10MinView->vShow_study_type();
      }
    }
#endif
  return(bRet);
  }

/********************************************************************
vShowHideStudyDates - Display the Show/hide study dates dialog box.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vShowHideStudyDates(void)
  {
  CHideStudyDates *pdlg;

          //vGet_view();

    pdlg = new CHideStudyDates(this);
    pdlg->DoModal();
    delete pdlg;
  }

/********************************************************************
vCreate_hide_study_dates_GUID - Create a GUID to encode the study date into.
                                This is used the first time the program is
                                run for all versions except CDDR.

  inputs: None.

  return: None.
********************************************************************/
void CEGGSASDoc::vCreate_hide_study_dates_GUID(void)
  {

  // Create the hide study dates GUID and set hide study dates flag to hide
  // the study dates.
  g_pConfig->vCreate_guid();
  // Set the password to empty as it won't be used.
  g_pConfig->m_cstrHideStudyDatesPassword.Empty();
  // Write the GUID to the INI FILEL
  g_pConfig->vWrite_study_date_info();
  }

//LRESULT CEGGSASDoc::vDisplay_hide_show_study_dates(WPARAM wParam, LPARAM lParam)
//  {
//  vShowHideStudyDates();
//  return(0);
//  }

/********************************************************************
OnReportShow33093

  Message handler for displaying the Hide/Show Study Dates for the
  CDDR version for waterload.
    Call the current document to process.
********************************************************************/
void CEGGSASDoc::OnReportShow33093()
  {
  // TODO: Add your command handler code here
  vShowHideStudyDates();
  }

/********************************************************************
OnUpdateReportShowHideStudyDates

  Message handler for updating the Hide/Show Study Dates menu item.
  for the CDDR version for waterload.
  Put a check on the menu item if the study dates are hidden.
********************************************************************/
void CEGGSASDoc::OnUpdateReportShow33093(CCmdUI *pCmdUI)
  {
  // TODO: Add your command update UI handler code here
   pCmdUI->Enable(TRUE);
   if(g_pConfig->m_bHideStudyDates == false)
     pCmdUI->SetCheck(0);
   else
     pCmdUI->SetCheck(1);
  }
