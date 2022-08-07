/********************************************************************
EGGSAS.h

Copyright (C) 2003 - 2021 3CPM Company, Inc.  All rights reserved.

  main header file for the EGGSAS application.

HISTORY:
03-OCT-03  RET  New.
03-MAR-04  RET  Added new item to INI file in section INI_SEC_EGGDRIVER:
                  INI_ITEM_USE_FAKE_DATA
05-FEB-05  RET  Add defines for new items in the INI file for:
                   INI_ITEM_CORE_REPORT, INI_ITEM_DIAG_RECOM, INI_ITEM_RSA,
                   INI_ITEM_SUMM_GRAPHS, INI_ITEM_RAW_SIG_SEL_MIN
10-FEB-05  RET  Change PATIENT_DATA structure to add a middle initial
                  and delay gastric emptying.
03-MAR-05  RET  Add ZERO_POINTS_MAX_COUNT.
10-MAY-07  RET  Research Version
                  Add defines: INI_ITEM_PATIENT_INFO, POST_STIMULATION_MAX_TIME_HRS,
                               INV_PERIOD_LEN, INV_PERIOD_START, MAX_TOTAL_PERIODS,
                               DEFAULT_MIN_PER_GRAPH, INI_ITEM_MIN_PER_GRAPH,
                               INI_ITEM_BACKUP_PATH, PROGRAM_PATH, INI_ITEM_EVENT_LOG
                  POST_STIMULATION_MAX_TIME_MIN, POST_STIMULATION_MAX_TIME_SEC
                  Add to PATIENT_DATA structure:
                    uNumPeriods, cstrStimMedDesc, uDataPtsInBaseline,
                    bPause, PostStimDataPoints
                  Change PATIENT_DATA structure so that period arrays are
                    big enough to hold the maximum periods in the
                    post-stimulation study plus the baseline.
                  Change data file structure.
                    Add defines: DATA_FILE_VER_R, PROGRAM_NAME_R1
                    Add structure BASELINE_TRAILER
                  Add class data m_cstrProgramPath.
05-FEB-08  RET  Version 2.03
                  Add typedef for WHAT_IF structure.
                  Add user message, WMUSER_DISPLAY_WHATIF, for displaying
                    data selected in the What If dialog box.
03-JUL-08  RET  Version 2.05
                  Changed define for GRAMS from 2 to 3.
28-AUG-08  RET  Version 2.05c
                  Add defines: STANDARD_HELP_PATH, RESEARCH_HELP_PATH,
                    HELP_FILE_HLP, HELP_FILE_CNT
                  Add class variables: m_cstrStandardHelpFilePath
                    m_cstrResearchHelpFilePath
14-OCT-08  RET  Version 2.07a
                  Add defines: PT_NONE, PT_DOG, PT_HUMAN, PATIENT_TYPE_DOG
                  Change PATIENT_DATA structure to include uPatientType.
19-OCT-08  RET  Version 2.07b
                  Add class variable: m_bAllowDogStudies
                  Add define INI_ITEM_ALLOW_DOG_STUDIES.
07-N0V-08  RET  Version 2.08a
                  Add define EGGSAS_READER, LOCAL_DB, IMPORT_DB, EXPORT_STUDIES_PGM.
                  Add INI defines INI_SEC_READER, INI_ITEM_READER_DB_PATH.
07-DEC-08  RET  Version 2.08b
                  Research program
                    Add define STUDY_EXPORT_DIR.
14-JAN-09  RET  Version CDDRdr.2.08c
                  Change the defines used to conditionally compile for Reader.
                    Add: READER_NONE, READER, READER_DEPOT.
                  Change EGGSAS_READER to be one of the above.
26-JAN-09  RET  Version CDDRdr.2.08f
                  Change HELP_FILE_HLP and HELP_FILE_CNT to be just the file extension.
                  Add HELP_FILE, READER_HELP_FILE, CENTRAL_DATA_DEPOT_READER_HELP_FILE
26-OCT-10 RET
                  Add INI item INI_ITEM_SIGNALCORR.
01-FEB-11 RET
               Add program subtype define EGGSAS_SUBTYPE.
                 Add defines for subtypes: SUBTYPE_NONE, SUBTYPE_READER_DEMO
22-FEB-11  RET
               Add define to the INI file for the driver to hold the Device ID.
                 INI_ITEM_DEVICE_ID
27-AUG-12  RET
             Implement new database table DiagnosisDesc
               Modify structures: PATIENT_DATA
02-SEP-12  RET
             Implement new database table for Recommendations.
               Modify structures: PATIENT_DATA
13-SEP-12  RET
             Changes to indications.
               Add to structure PATIENT_DATA:
                 bGastroparesis, bDyspepsiaBloatSatiety, bDyspepsia,
                 bAnorexia, bRefractoryGERD, bWeightLoss, bPostDilationEval,
                 bEvalEffectMed
22-SEP-12  RET
03-OCT-12  RET
             Change for 60/200 CPM
               Add defines: _200_CPM_HARDWARE
               Add enumerate list: eFrequencyRange
               Add variable: m_iFreqLimit, m_iMaxFreqBands, m_bFirstTimeInTenMinView

25-FEB-13  RET
               Add to structure PATIENT_DATA: bMale, bFemale, bHide
13-MAR-13  RET
             Add to structures: PATIENT_DATA, WHAT_IF: uFrequency
02-JUN-16  RET
             Add feature to hide study dates.
               Add define: SUBTYPE_HIDE_STUDY_DATES
5-JUL-20  RET
            Correct problem with not showing study dates in any version.
            EGGSAS_SUBTYPE is set to SUBTYPE_HIDE_STUDY_DATES for CDDR
            EGGSAS_SUBTYPE is set to SUBTYPE_NONE for all other versions.
            Made a comment for this when changing EGGSAS_READER.
6-JUL-20  RET
            Change definition of HELP_FILE_HLP to ".chm" from ".hlp".
            ".hlp" files are no longer supported in Win10.
16-NOV-20  RET
             Add new version CDDRE
               Add define: READER_DEPOT_E
********************************************************************/

#if !defined(AFX_EGGSAS_H__A435D4C8_6A7B_43E9_B712_B90D7955075C__INCLUDED_)
#define AFX_EGGSAS_H__A435D4C8_6A7B_43E9_B712_B90D7955075C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Events.h"

#include "limits.h"
#include "float.h"

#include "sysdefs.h"
#include "Config.h"
#include "Language.h"

#include "resource.h"       // main symbols
#include "resource.hm"      // symbols for help system.

#include "htmlhelp.h"

// Define to make a Reader only program.
#define READER_NONE 0
#define READER 1
#define READER_DEPOT 2
#define READER_DEPOT_E 4

// For READER_DEPOT, set EGGSAS_SUBTYPE to SUBTYPE_HIDE_STUDY_DATES
// For all other version set EGGSAS_SUBTYPE to SUBTYPE_NONE
#define EGGSAS_READER   READER_NONE //READER_DEPOT_E // READER //READER_NONE //READER_DEPOT

// Subtype to the above main types.
#define SUBTYPE_NONE 0
#define SUBTYPE_READER_DEMO 1
#define SUBTYPE_HIDE_STUDY_DATES 2
#define EGGSAS_SUBTYPE   SUBTYPE_NONE //SUBTYPE_HIDE_STUDY_DATES //SUBTYPE_NONE

// Comment out if not a reader program.
// Reader program can change any thing in the database.
//#define EGGSAS_READER
// Central Data Depot Reader can only change the What IFs.
//#define EGGSAS_READER_DEPOT

//// Defines for using 200 CPM hardware
// Define to make program use the 1-200 cpm hardware
#define _200_CPM_HARDWARE
// Defines for frequency limit.
enum eFrequencyRange
  {
  FREQ_LIMIT_15_CPM = 0,
  FREQ_LIMIT_60_CPM,
  FREQ_LIMIT_200_CPM,
  };

//// End of Defines for using 200 CPM hardware

// If this is defined, put the EZEM logo and the 3CPM logo on the opening screen.
// If this is not defined, put the 3CPM logo only on the screen.
//#define EZEM_LOGO

/////////////////////////////////////////////////////////////////////
//// General system defines

  // Name of export studies program.
#define EXPORT_STUDIES_PGM "StudyExport.exe"
#define STUDY_EXPORT_DIR "StudyExport"

#define LOCAL_DB 1 // Local database
#define IMPORT_DB 2 // Import database (exported from another system)

#define	WMUSER_DISPLAY_WHATIF WM_USER+1 // Displays What If scenario
#define	WMUSER_DISPLAY_HIDE_SHOW_STUDY_DATES WM_USER+2 // Displays the Hide/Show study dates dialog box.

#define INV_MINUTE        -1.0F // Represents an invalid designation of a minute.
#define INV_PERIOD_LEN    -1.0F // Invalid period length.
#define INV_PERIOD_START  -1.0F // Invalid period start.

//// Defines for items in the INI file.
#define WATER_LOAD_UNITS_OUNCES 0 // ounces
#define WATER_LOAD_UNITS_MILLILITERS 1 // milliliters

#define DEFAULT_MIN_PER_GRAPH 4 // Default number of minutes per graph.
//// End of INI file definitions

// Patient types
#define PAT_NONE  0
#define PAT_DOG   1
#define PAT_HUMAN 2

#define PATIENT_TYPE_DOG "_DOG_"

/////////////////////////////////////////////////////////////////////
//// Defines for the data collected during an EGG.
#define MAX_CHANNELS 2
#define MIN_PER_PERIOD 10 // Minutes per period.
#define DATA_POINT_SIZE sizeof(float) //sizeof(short int)
#define DATA_POINTS_PER_SECOND 4.267F
  // Data points per minute for one channel.
#define DATA_POINTS_PER_MIN_1 256 //260 //
#define DATA_POINTS_PER_HALF_MIN (DATA_POINTS_PER_MIN_1 / 2)
  // Data points per minute for all channels.
#define DATA_POINTS_PER_MIN_ALL (DATA_POINTS_PER_MIN_1 * MAX_CHANNELS)
#define MINUTES_PER_EPOCH 4
#define HALF_MINUTES_PER_EPOCH (MINUTES_PER_EPOCH * 2)
  // Bytes of data per minute for one channel.
#define BYTES_PER_MINUTE_1 (DATA_POINTS_PER_MIN_1 * DATA_POINT_SIZE)
  // Bytes of data per minute for all channels.
#define BYTES_PER_MINUTE_ALL (DATA_POINTS_PER_MIN_ALL * DATA_POINT_SIZE)
  // Bytes per epoch for one channel.
#define BYTES_PER_EPOCH_1 (BYTES_PER_MINUTE_1 * MINUTES_PER_EPOCH)
  // Bytes per epoch for all channels.
#define BYTES_PER_EPOCH_ALL (BYTES_PER_MINUTE_ALL * MINUTES_PER_EPOCH)
  // Data points per epoch for one channel.
#define DATA_POINTS_PER_EPOCH_1 (DATA_POINTS_PER_MIN_1 * MINUTES_PER_EPOCH)
  // Data points per epoch for all channels.
#define DATA_POINTS_PER_EPOCH_ALL (DATA_POINTS_PER_MIN_ALL * MINUTES_PER_EPOCH)
  // Number of bytes in a period
#define BYTES_PER_PERIOD (BYTES_PER_MINUTE_1 * MIN_PER_PERIOD)

  // Research version
  // Post-stimulation time period in hours
#define POST_STIMULATION_MAX_TIME_HRS 3
  // Post-stimulation time period in minutes
#define POST_STIMULATION_MAX_TIME_MIN (POST_STIMULATION_MAX_TIME_HRS * 60)
  // Post-stimulation time period in seconds.
#define POST_STIMULATION_MAX_TIME_SEC (POST_STIMULATION_MAX_TIME_MIN * 60)
/////////////////////////////////////////////////////////////////////

// Defines for the period.  The values for these defines are based on the
// period.  For example PERIOD_10 is the second 10 minute period (including
// the baseline).
#define PERIOD_NONE INV_SHORT
#define PERIOD_BL   0 // Baseline period
#define PERIOD_10   1 // First ten minute period
#define PERIOD_20   2 // Second ten minute period
#define PERIOD_30   3 // Third ten minute period
#define PERIOD_40   4 // Fourth ten minute period
#define MAX_PERIODS  (PERIOD_30 + 1)
#define MAX_MANUAL_MODE_PERIODS (PERIOD_40 + 1)
#define TOTAL_PERIODS  (PERIOD_40 + 1)
  // Maximum periods.
  // Post-stimulation max hours * 60 = max minutes.
  // max minutes / 4 minutes per period plus 1 for the baseline.
#define MAX_TOTAL_PERIODS (((POST_STIMULATION_MAX_TIME_HRS * 60) / 4) + 1)
//  // For research version
//#define PERIOD_CEILING 100 // Maximum number of periods.

//// Bits to define the periods.  These are ORed together so 1 byte can hold
//// the information needed to determine what periods have been analyzed.
//#define PERIOD_BL_BIT  (0x1 << PERIOD_BL)
//#define PERIOD_10_BIT  (0x1 << PERIOD_10)
//#define PERIOD_20_BIT  (0x1 << PERIOD_20)
//#define PERIOD_30_BIT  (0x1 << PERIOD_30)
//#define PERIOD_40_BIT  (0x1 << PERIOD_40)

#define PERIOD_MASK 0xF // Bits representing PERIOD_BL through PERIOD_30.

// Structure to hold the patient data (includes patient information,
// pre-study information and study information).
typedef struct
  {
    // Data base information.
  long lPatientID;
  long lStudyID;
    // Demographics
	CString	cstrLastName;
	CString	cstrFirstName;
  CString cstrMI; // Middle initial
	CString	cstrAddress1;
	CString	cstrAddress2;
	CString	cstrCity;
	CString	cstrDateOfBirth;
	CString	cstrHospitalNum;
	CString	cstrPhone;
  BOOL bUseResrchRefNum;
	CString	cstrResearchRefNum;
	CString	cstrSocialSecurityNum;
	CString	cstrState;
	CString	cstrZipCode;
    // Pre-study information
	CString	cstrMealTime;
	CString	cstrAttendPhys;
	CString	cstrIFPComments; // Indication for procedure comments.
	CString	cstrReferPhys;
	CString	cstrTechnician;
	BOOL bBloating; // Combine with bEarlySateity to become: dyspepsia - bloating and/or satiety - 536.80
	BOOL bEarlySateity; // Combine with bBloating to become: dyspepsia - bloating and/or satiety - 536.80
	BOOL bEructus; // No longer used
	BOOL bGerd; // No longer used
	BOOL bNausea;  // Becomes: nausea - 787.02
	BOOL bPPFullness; // No longer used
	BOOL bVomiting;  // Becomes: vomiting - 787.01
  BOOL bDelayedGastricEmptying; // Becomes: gastroparesis - 536.3
	BOOL bGastroparesis; // gastroparesis - 536.3  substitutes for "delayed emptying"
	BOOL bDyspepsiaBloatSatiety; // dyspepsia - bloating and/or satiety - 536.80 substitutes for bloating and satiety
	BOOL bDyspepsia; // dyspepsia - 536.80
	BOOL bAnorexia; // anorexia - 783.0
	BOOL bRefractoryGERD; // Refractory GERD – 530.11
	BOOL bWeightLoss; // Weight loss
	BOOL bPostDilationEval; // Post dilation evaluation
  BOOL bEvalEffectMed; // Evaluate effect of medication
    // Water
  unsigned short uWaterAmount; // Amount of water
  unsigned short uWaterUnits; // Units for the amount of water.
  CString cstrWaterUnits; // Text string for water units.
    // Study data
  CString cstrDate; // Date of study.
  CString cstrFile; // Name of file.
  unsigned uNumDataPts; // Number of data points read from file for period.
    // Length in minutes of each period.
//  unsigned short uPeriodStart[TOTAL_PERIODS];
//  unsigned short uPeriodLen[TOTAL_PERIODS];
  float fPeriodStart[MAX_TOTAL_PERIODS];
  float fPeriodLen[MAX_TOTAL_PERIODS];
    // Start and end of good minutes for each period.
  float fGoodMinStart[MAX_TOTAL_PERIODS]; //TOTAL_PERIODS];
  float fGoodMinEnd[MAX_TOTAL_PERIODS]; //TOTAL_PERIODS];
  float fTestMaxEGG; // Maximum EGG signal for the equipment test period.
  float fTestMinEGG; // Minimum EGG signal for the equipment test period.
  float fMaxEGG; // Maximum EGG signal for the current period.
  float fMinEGG; // Minimum EGG signal for the current period.
  float fMaxResp; // Maximum Respiration signal for the current period.
  float fMinResp; // Minimum Respiration signal for the current period.
  float fMeanBaseline; // Mean of the baseline good minutes. Used for artifact detection.
//  unsigned short uPeriodsAnalyzed; // Bits to indicate which periods have
//                                   // been analyzed.
  bool bPeriodsAnalyzed[MAX_TOTAL_PERIODS]; //PERIOD_CEILING]; // true indicate period has been analyzed.
  CString cstrSuggestedDiagnosis; // Suggested diagnosis.
  CString cstrDiagnosis; // Modified diagnosis (will be empty if suggested diagnosis accepted).
  BOOL bUserDiagnosis; // TRUE if user entered a diagnosis.
  short int iDiagType; // 0 - User accepted suggested diagnosis
                       // Not 0 - user modified suggested diagnosis.
  CString cstrRecommendations; // User entered recommendations.
  CString cstrMedications; // Medications.
  BOOL bExtra10Min; // TRUE if ran study an extra 10 minutes.
  bool bHavePatientInfo;  // true if we have the information from the PatientInfo database table.
  BOOL bManualMode; // TRUE - user is in manual mode for this study.
  short unsigned uRespRate[MAX_TOTAL_PERIODS]; //TOTAL_PERIODS]; // Holds the respiration rate for each period.
    // For research version
  short unsigned uNumPeriods; // Number of periods (minus baseline) user has defined.
  CString cstrStimMedDesc; // Stimulation medium description.
  unsigned uDataPtsInBaseline; // Number of data points in the Baseline.
                               // One data point consists of a EGG value and a respiration
                               // value.
  BOOL bPause; // TRUE - study has been paused.
  unsigned uPostStimDataPoints; // Number of data points in the post-stimulation study.
  CEvents Events;
  short unsigned uPatientType; // Human or Dog
     // Key phrase for the diagnosis description, link to DiagnosisDesc table
  CString cstrDiagnosisKeyPhrase;
  CString cstrICDCode; // ICD Code
     // Key phrase for the recommendation description, link to Recommendations table
  CString cstrRecommendationKeyPhrase;
  BOOL bMale; // true if patient is a male.
  BOOL bFemale; // true if patient is a female.
  BOOL bHide; // true if patient is hidden.
  unsigned uFrequency; // Frequency study was recorded at.
  } PATIENT_DATA;

// Structure for holding the data for a What If scenario.
typedef struct
  {
  long lWhatIfID;
  long lStudyID;
  CString cstrDescription;
  CString cstrDateTime;
  short unsigned uNumPeriods; // Number of periods (minus baseline) user has defined.
  float fPeriodStart[MAX_TOTAL_PERIODS];
  float fPeriodLen[MAX_TOTAL_PERIODS];
    // Start and end of good minutes for each period.
  float fGoodMinStart[MAX_TOTAL_PERIODS];
  float fGoodMinEnd[MAX_TOTAL_PERIODS];
  short unsigned uRespRate[MAX_TOTAL_PERIODS]; // Holds the respiration rate for each period.
  unsigned uFrequency; // Frequency for viewing the study.
  } WHAT_IF;

// Structure for writing patient information to the data file.  This is also 
// used to determine the file version.  For an old DOS file, the first integer
// should never be more that 4096 since a 12-bit A/D card is used.
//// THIS IS USED TO BE BACKWARD COMPATIBLE WITH OLD DATA FILES.
typedef struct
  {
  short int iHdrStart; // All bits are set.
  char szProgName[7];  // "EGGSAP".  A SPEC9 data file will have 7 counts from 
                       // the A/D board here.
  short unsigned int uiDataFileVer;  // The file version for an EGGSAP data file.
                                     // This starts a 1.  A SPEC9  data file
                                     // will have a count from the A/D board here.
  short unsigned int uiPatientDataSize; // Number of characters of patient
                                        //  information entered in the patient 
                                        // information window.
  } DATA_FILE_HEADER;

// Structure used to mark the end of the baseline for the Research version.
// Since the baseline can be a variable length, the end must be marked in the
// data file, it case the study doesn't get recorded and it needs to be
// restored.
typedef struct
  {
  unsigned short uStartWord1; // All bits set.
  unsigned short uStartWord2; // All bits set.
  unsigned short uStartWord3; // All bits set.
  unsigned short uStartWord4; // All bits set.
  long lDataSamples; // Number of data samples.
  long lSpare; // Spare
  unsigned short uEndWord1; // All bits set.
  unsigned short uEndWord2; // All bits set.
  unsigned short uEndWord3; // All bits set.
  unsigned short uEndWord4; // All bits set.
  } BASELINE_TRAILER;

/////////////////////////////////////////////////////////////////////
//// Data file definitions.
#define DATA_FILE_VER    5  // Current file version for Standard version.
#define DATA_FILE_VER_R  6  // Current file version for Research version.
#define PROGRAM_NAME_1  "EGGSAP" // Program name for data file version 1.
#define PROGRAM_NAME_2  "EGGSAS" // Program name for data file version 2.
#define PROGRAM_NAME_3  "EGGSS4" // Program name for data file version 3 and 4.
#define PROGRAM_NAME    "EGGSV2" // Program name for data file version 5.
#define PROGRAM_NAME_R1 "EGGRV1" // Program name for Research Version, data file version 6
#define DATA_FILE_HDR_START -1 // Delimiter for file header.  

#define FILE_VER_SPEC9    0 // SPEC9 data file.
#define FILE_VER_EGGSAP   1 // EGGSAP data file.
#define FILE_VER_EGGSAS   2 // EGGSAS data file
#define FILE_VER_EGGSAS4  3 // EGGSAS-4 data file.  Adds Diagnosis, Medications,
                            // Comments, and Channel Usage
#define FILE_VER_EGGSAS4_1  4 // EGGSAS-4 data file.  Adds artifact minutes and
                              // descriptions
#define FILE_VER_EGGSAS_II  5 // EGGSAS Version II.
/////////////////////////////////////////////////////////////////////

// Units for amount of water/Stimulation medium
#define OUNCES 1
#define MILLILITERS 2
#define GRAMS 3

// Color definitions in terms of RGB mixes.
#define BLUE_RGB       RGB(0, 0, 255) // Blue
#define WHITE_RGB      RGB(255, 255, 255) // white
#define BLACK_RGB      RGB(0, 0, 0) // black
#define RED_RGB        RGB(255, 0, 0) // Red
#define GREEN_RGB      RGB(0, 128, 0) // Green
#define DARK_RED_RGB   RGB(175, 0, 0) //RGB(100, 0, 0) // dark red
#define LTGRAY_RGB     RGB(150, 150, 150) // Light gray.
#define EVENT_RGB      RGB(3, 147, 252)

// Array dimensions for the array containing control information for
// the periods and frequency bands.
#define CONTROL_FREQ_BANDS 4
#define CONTROL_MIN_HI_LO 8

// This structure holds the data used for drawing raw data graphs.
typedef struct
  {
  unsigned short uPeriod; // Period being graphed.
  float *pfData; // Pointer to an array of the data being graphed.
  float fStartMin; // Start minute of the data.
  float fGraphStartMin; // Start minute of the data for the graph annotation.
  float fMinOnGraph; // Number of minutes to graphs.
  int iNumDataPoints; // Number of data points in the array.
  float fMaxEGG; // Maximum EGG signal.
  float fMinEGG; // Minimum EGG signal.
  float fMaxResp; // Maximum Respiration signal.
  float fMinResp; // Minimum Respiration signal.
  bool bAddXAxisLabel; // true if an X axis label is to be added.
  unsigned uAttrib; // Various graph attributes.
  } GRAPH_INFO;

//// Graph attributes for the GRAPH_INFO structure.
  // Bit 0 is how to display the minutes.
#define MINUTES_INTEGERS 0x0 // Display the minutes as integers.
#define MINUTES_FRACTIONS 0x1 // Display the minutes as fractions.

#define MINUTE_DELTA 0.5F //0.1F // Increment by which user can change the selected minutes.

  // Maximum amount of time that we should wait while getting zero
  // points from the driver.  This is based on a 250 millisecond timer.
#define ZERO_POINTS_MAX_COUNT 40  // 10 seconds.

  // The following enumerated list indicates the type of program and/or study.
enum ePgmType
  {
  PT_NONE,       // No program/study type.
  PT_STANDARD,   // Standard program/study type.
  PT_RESEARCH,   // Research program/study type.
  PT_STD_RSCH,   // Both Standard and Research program types.
  };

#define PROGRAM_PATH "Program" // Path where program is stored.
#define READER_PATH "Reader" // Path where the Reader is stored.
  // Path where the Central Data Depot Reader is stored.
#define CENTRAL_DATA_DEPOT_READER_PATH "Central Data Depot Reader"
#define STANDARD_HELP_PATH "StandardHelp" // directory name where standard help files are stored.
#define RESEARCH_HELP_PATH "ResearchHelp" // directory name where research help files are stored.

#define HELP_FILE "3CPM EGGSAS"
#define READER_HELP_FILE "3CPM EGGSAS Reader"
#define CENTRAL_DATA_DEPOT_READER_HELP_FILE "3CPM EGGSAS Central Data Depot Reader"
#define HELP_FILE_HLP ".chm" //".hlp"
#define HELP_FILE_CNT ".cnt"
//#define HELP_FILE_HLP "3CPM EGGSAS.hlp"
//#define HELP_FILE_CNT "3cpm eggsas.cnt"

/////////////////////////////////////////////////////////////////////////////
//// Global data references
extern CString g_cstrExpirationDate;
extern bool g_bAsciiDataFile;
extern CLanguage *g_pLang;
extern CConfig *g_pConfig;
extern bool g_bFilterRSA;
extern BOOL g_bHiddenPatients;

/////////////////////////////////////////////////////////////////////////////
// CEGGSASApp:
// See EGGSAS.cpp for the implementation of this class
//

class CEGGSASApp : public CWinApp
{
public:
	CEGGSASApp();

  void vLog(LPCTSTR pstrFmt, ...);
  void vOn_help(void);
  void vSetup_help_files();
  void vSet_ini_file(char *pszFile);
  void vSet_freq_range(int iFreqRange);

public: // Data
  char m_szIniFile[20]; // Name (no path) of the most recently used ini file.
  CString m_cstrDataFilePath; // Path where data files get stored.
  CString m_cstrProgramPath; // Path where the program exists.
  CString m_cstrStandardHelpFilePath; // Path where help files exist for the standard version.
  CString m_cstrResearchHelpFilePath; // Path where help files exist for the research version.
  CString m_cstrHelpFile; // Contains the root name of the help file.
  CString m_cstrIniFilePath; // Path where the eggsas.ini file is found.
    // Indicates if program is research version/standard version.
  ePgmType m_ptPgmType;
    // Indicates if study is research study/standard study.
  ePgmType m_ptStudyType;
  bool m_bAllowDogStudies; // true if dog studies are allowed, otherwise false.
  // Frequency limit.  default is 0 which is 15 CPM.  For use with 200 CPM hardware.
  int m_iFreqLimit;
  // For used with 200 cpm hardware.  Holds the maximum number of
  // frequency bands.
  int m_iMaxFreqBands;
  // Used by the select frequency range dialog box to force it to display
  // only once on program startup.
  //bool m_bFirstTimeInTenMinView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEGGSASApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	COleTemplateServer m_server;
		// Server object for document creation
	//{{AFX_MSG(CEGGSASApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
  afx_msg void OnFilePrintSetup();
};

extern CEGGSASApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EGGSAS_H__A435D4C8_6A7B_43E9_B712_B90D7955075C__INCLUDED_)
