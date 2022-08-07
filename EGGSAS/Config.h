/********************************************************************
Config.h

Copyright (C) 2012,2013,2016,2020 3CPM Company, Inc.  All rights reserved.

  Header file for the CConfig class.

HISTORY:
13-FEB-12  RET  Version 
                  New.
22-AUG-12  RET
                Add INI item to the Report section demographic and facility items:
                  Add define: INI_ITEM_INC_REPORT_ITEMS
                  Add enumerated list: Include_rpt_items
                  Add class variable: m_iIncludeRptItems[]
                  Change enumerated list Config_items:
                    Add: CFG_INC_REPORT_ITEMS
                Modify the report configuration dialog box.
                  Add defines: INI_ITEM_RSA_FIRST_PAGE, INI_ITEM_RSA_OTHER_PAGE,
                    INI_ITEM_SUMM_GRAPHS_FIRST_PAGE, INI_ITEM_SUMM_GRAPHS_OTHER_PAGE
                  Add class variables: m_iRsaFirstPage, m_iRsaOtherPage,
                    m_iSummGraphsFirstPage, m_iSummGraphsOtherPage
                  Change enumerated list Config_items: CFG_RSA_FIRST_PAGE,
                    CFG_RSA_OTHER_PAGE, CFG_SUMM_GRAPHS_FIRST_PAGE,
                    CFG_SUMM_GRAPHS_OTHER_PAGE
18-SEP-12  RET
             Add Include in Report Checkboxes for the attending physician,
               the refering physician and the technician.
                 Add to enumerated list Include_rpt_items:
                   INC_ATTENDING_PHYSICIAN, INC_REFERING_PHYSICIAN,
                   INC_TECHNICIAN
03-OCT-12  RET
             Change for 60/200 CPM
               Add INI item to report section: INI_ITEM_FREQ_RANGE
               Add to Config_items enumerated list: CFG_FREQ_RANGE
               Add variable: m_iFreqRange
14-DEC-12  RET
             Change to filter out frequencies below 10 cpm in the RSA.
               Add INI items: INI_ITEM_FILTER_RSA, INI_ITEM_BOTH_RSA
               Change Config_items to add: CFG_FILTER_RSA, CFG_BOTH_RSA
               Add class variables: m_bFilterRSA, m_iBothRSA
25-FEB-13  RET
             Add INI item to report section: INC_RPT_MALE_FEMALE
29-MAY-16  RET
             Changes for hiding study dates.
               Add define: INI_ITEM_HIDE_STUDY_DATES
                 HIDE_STUDY_DATE_0, HIDE_STUDY_DATE_1, HIDE_STUDY_DATE_2,
                 HIDE_STUDY_DATE_3, HIDE_STUDY_DATE_4, HIDE_STUDY_DATE_5,
                 HIDE_STUDY_DATE_6, HIDE_STUDY_DATE_7, HIDE_STUDY_DATE_8,
                 HIDE_STUDY_DATE_9, HIDE_STUDY_DATE_10, HIDE_STUDY_DATE_11,
                 HIDE_STUDY_DATE_12, HIDE_STUDY_DATE_13, HIDE_STUDY_DATE_14,
                 HIDE_STUDY_DATE_15, HIDE_STUDY_DATE_16, HIDE_STUDY_DATE_17,
                 HIDE_STUDY_DATE_18, HIDE_STUDY_DATE_19, HIDE_STUDY_DATE_FLAG,
                 HIDE_STUDY_DATE_PASSWORD_LEN
               Add class variables: m_cstrHideStudyDatesPassword, m_bHideStudyDates,
                 m_cstrHideStudyDatesINIInfo
14-NOV-20  RET
             Add ability to work with a remote database.
             Changed settings to allow the user to specify the remote database.
               INI file definitions: INI_ITEM_REMOTE_DB_PATH, INI_ITEM_REMOTE_DB_PASSWD,
                 INI_ITEM_REMOTE_DB_DEF_PASSWD
               Add to Config_items enumerated list: CFG_REMOTE_DB_PATH, CFG_REMOTE_DB_PASSWD,
                 CFG_REMOTE_DB_DEF_PASSWD
               Add class variables: m_cstrRemoteDBPath, m_cstrRemoteDBPasswd,
                 m_cstrRemoteDBDefPasswd
********************************************************************/
#pragma once

//// INI file definitions
  // General section
#define INI_SEC_GENERAL "GENERAL"
#define INI_ITEM_DATAPATH "datapath"
#define INI_ITEM_PGMKEY "pgmkey"
#define INI_ITEM_WATERLOAD_UNITS "waterloadunits"  // 0 = ounces, 1 = milliters
#define INI_ITEM_MIN_PER_GRAPH   "minutespergraph" // Number of minutes on each graph.
#define INI_ITEM_BACKUP_PATH "backuppath" // Path last used for backup.
#define INI_ITEM_LANGUAGE "Language" // Culture for the currently selected language
#define INI_ITEM_DATA_FILE_STORAGE "DataFileStorage" // Location for storing data files.
#define INI_ITEM_FILTER_RSA "FilterRSA" // Filter the RSA
#define INI_ITEM_HIDE_STUDY_DATES "HideStudyDates" // Holds password and flag for hiding study dates.
#define INI_ITEM_REMOTE_DB_PATH "RemoteDBPath" // Path of the remote database.
#define INI_ITEM_REMOTE_DB_PASSWD "RemoteDBPasswd" // Password for changing the remote database path.
#define INI_ITEM_REMOTE_DB_DEF_PASSWD "RemoteDBDefPasswd" // Default Password for changing the remote database path.
  // EGG driver section
#define INI_SEC_EGGDRIVER "EGGDRIVER"
#define INI_ITEM_ARTIFACTWINDOW "artifactwindow"
#define INI_ITEM_STABLEWINDOW_EGG "stableEGGwindow"
#define INI_ITEM_STABLEWINDOW_RESP "stableRespwindow"
#define INI_ITEM_STABLETIME "stabletime"
#define INI_ITEM_ELECTRODE_TEST "electrodetest"
#define INI_ITEM_RESP_MULT "respmult"
// Used to set an expiration date. The value minus 20 is the number of days the
// program will work for.  If there is no expiration date, the value is set to 0.
#define INI_ITEM_SIGNALCORR "signalcorr"
#define INI_ITEM_POS_IMPEDANCE_LIMIT "posimpedancelimit"
#define INI_ITEM_NEG_IMPEDANCE_LIMIT "negimpedancelimit"
#define INI_ITEM_BATTERY_CHECK "batterycheck"
#define INI_ITEM_USE_FAKE_DATA "usefakedata" // 0 - use hardware
                                             // 1 - generate artificial data.
#define INI_ITEM_DEVICE_ID "DevID"
#define INI_ITEM_FREQ_RANGE "FreqRange"
  // Analysis section
#define INI_SEC_ANALYSIS "ANALYSIS"
#define INI_ITEM_CHANNEL "channel" //channel to analyuze
#define INI_ITEM_ALLOW_DOG_STUDIES "AllowDogStudies"
  // Reports section
#define INI_SEC_REPORT "REPORT"
#define INI_ITEM_CORE_REPORT "corereport"
#define INI_ITEM_ENTIRE_RAW_SIGNAL "entirerawsignal"
#define INI_ITEM_DATASHEET "datasheet"
#define INI_ITEM_DIAG_RECOM "diagrecomm"
#define INI_ITEM_RSA "rsa"  // Old, not used anymore.
#define INI_ITEM_SUMM_GRAPHS "summgraphs" // Old, not used anymore.
#define INI_ITEM_RAW_SIG_SEL_MIN "rawsigselmin"
#define INI_ITEM_PATIENT_INFO "patientinfo"
#define INI_ITEM_EVENT_LOG "eventlog"
#define INI_ITEM_INC_REPORT_ITEMS "IncludeRptItems"
#define INI_ITEM_RSA_FIRST_PAGE "rsafirstpage"
#define INI_ITEM_RSA_OTHER_PAGE "rsaotherpage"
#define INI_ITEM_SUMM_GRAPHS_FIRST_PAGE "summgraphsfirstpage"
#define INI_ITEM_SUMM_GRAPHS_OTHER_PAGE "summgraphsotherpage"
#define INI_ITEM_LOGO "logo"
#define INI_ITEM_SUMM_TABLE_ON_SCREEN "summtableonscreen"
#define INI_ITEM_BOTH_RSA "BothRSA"

  // For compatibility with version 1.0
#define INI_SEC_VERSIONI "VERSIONI"
#define INI_ITEM_SCALEFACTOR "scalefactor"
#define INI_ITEM_POSTWATER_START "postwatertart"

  // Reader section
#define INI_SEC_READER "READER"
#define INI_ITEM_READER_DB_PATH "readerdbpath" // Last path user accessed for the reader database.

// defines for the Hide study dates password and flag.
#define HIDE_STUDY_DATE_0 10
#define HIDE_STUDY_DATE_1 4
#define HIDE_STUDY_DATE_2 24
#define HIDE_STUDY_DATE_3 6
#define HIDE_STUDY_DATE_4 1
#define HIDE_STUDY_DATE_5 32
#define HIDE_STUDY_DATE_6 15
#define HIDE_STUDY_DATE_7 12
#define HIDE_STUDY_DATE_8 27
#define HIDE_STUDY_DATE_9 31
#define HIDE_STUDY_DATE_10 3
#define HIDE_STUDY_DATE_11 25
#define HIDE_STUDY_DATE_12 33
#define HIDE_STUDY_DATE_13 11
#define HIDE_STUDY_DATE_14 21
#define HIDE_STUDY_DATE_15 20
#define HIDE_STUDY_DATE_16 9
#define HIDE_STUDY_DATE_17 28
#define HIDE_STUDY_DATE_18 17
#define HIDE_STUDY_DATE_19 26
#define HIDE_STUDY_DATE_FLAG 22
#define HIDE_STUDY_DATE_PASSWORD_LEN 19


// enumerated lists for accessing ini data items.
enum Config_items
  {
  // General section
  CFG_DATAPATH = 0,
  CFG_PGMKEY,
  CFG_WATERLOADUNITS,
  CFG_MINUTESPERGRAPH,
  CFG_BACKUPPATH,
  CFG_CULTURE,
  CFG_DATA_FILE_STORAGE,
  CFG_FILTER_RSA,
  CFG_REMOTE_DB_PATH,
  CFG_REMOTE_DB_PASSWD,
  CFG_REMOTE_DB_DEF_PASSWD,
  // EGG driver section
  CFG_ARTIFACTWINDOW,
  CFG_STABLEEGGWINDOW,
  CFG_STABLERESPWINDOW,
  CFG_STABLETIME,
  CFG_ELECTRODETEST,
  CFG_RESPMULT,
  CFG_SIGNALCORR,
  CFG_POSIMPEDANCELIMIT,
  CFG_NEGIMPEDANCELIMIT,
  CFG_BATTERYCHECK,
  CFG_USEFAKEDATA,
  CFG_DEVID,
  CFG_FREQ_RANGE,
  // Analysis section
  CFG_CHANNEL,
  CFG_ALLOWDOGSTUDIES,
  // Reports section
  CFG_COREREPORT,
  CFG_ENTIRERAWSIGNAL,
  CFG_DATASHEET,
  CFG_DIAGRECOMM,
  CFG_RSA,
  CFG_SUMMGRAPHS,
  CFG_RAWSIGSELMIN,
  CFG_PATIENTINFO,
  CFG_EVENTLOG,
  CFG_INC_REPORT_ITEMS,
  CFG_RSA_FIRST_PAGE,
  CFG_RSA_OTHER_PAGE,
  CFG_SUMM_GRAPHS_FIRST_PAGE,
  CFG_SUMM_GRAPHS_OTHER_PAGE,
  CFG_LOGO,
  CFG_SUMM_TABLE_ON_SCREEN,
  CFG_BOTH_RSA,

  // version 1.0 section
  CFG_SCALEFACTOR,
  CFG_POSTWATERSTART,
  // Reader section
  CFG_READERDBPATH,
  };

// enumerated lists for accessing demographic and facility info items
// to be included in report.  This list acts as an index into the
// array of items, m_iIncludeRptItems[].
enum Include_rpt_items
{
  INC_RPT_PATIENT_FIRST_NAME = 0,
  INC_RPT_PATIENT_MI,
  INC_RPT_PATIENT_LAST_NAME,
  INC_RPT_DATE_OF_BIRTH,
  INC_RPT_SSN,
  INC_RPT_ADDR1,
  INC_RPT_ADDR2,
  INC_RPT_CITY,
  INC_RPT_STATE,
  INC_RPT_ZIP_CODE,
  INC_RPT_PHONE_NUM,
  INC_RPT_HOSP_NUM,
  INC_RPT_RSRCH_REF_NUM,
  INC_RPT_FAC_NAME,
  INC_RPT_FAC_ADDR1,
  INC_RPT_FAC_ADDR2,
  INC_RPT_FAC_CITY,
  INC_RPT_FAC_STATE,
  INC_RPT_FAC_ZIP_CODE,
  INC_RPT_FAC_PHONE_NUM,
  INC_RPT_FAC_LOGO,
  INC_ATTENDING_PHYSICIAN,
  INC_REFERING_PHYSICIAN,
  INC_TECHNICIAN,
  INC_RPT_MALE_FEMALE,
  INC_RPT_LAST,
};

class CConfig
  {
  public:
    CConfig(void);
    virtual ~CConfig(void);

    // File Data.  These data items match the items in the INI file.
    // General section
    CString m_cstrDataFilePath;
    CString m_cstrPgmKey;
    int m_iDefaultWaterLoadUnits;
    int m_iMinPerGraph;
    CString m_cstrBackupPath;
    CString m_cstrCulture;
    CString m_cstrDataFileStorage;
    bool m_bFilterRSA;
    CString m_cstrHideStudyDatesINIInfo;
    CString m_cstrHideStudyDatesPassword;
    bool m_bHideStudyDates;
    CString m_cstrRemoteDBPath;
    CString m_cstrRemoteDBPasswd;
    CString m_cstrRemoteDBDefPasswd;

    // EGG Driver section
    float m_fArtWindow;
    float m_fStableEGGWindow;
    float m_fStableRespWindow;
    unsigned m_uStableTime;
    short int m_iDoElectrodeTest;
    float m_fRespMult;
    int m_iExpireDays;
    float m_fPosImpedanceLimit;
    float m_fNegImpedanceLimit;
    short int m_iDoBatteryCheck;
    short int m_iUseFakeData;
    CString m_cstrDevID;
    short int m_iFreqRange;

    // Analysis section
    short int m_iChannel;
    CString m_cstrAllowDogStudies;

    // Report section
    int m_iIncludeCoreReport;
    int m_iIncludeEntireRawSignal;
    int m_iIncludeDataSheet;
    int m_iIncludeDiagRecom;
    int m_iIncludeRSA;
    int m_iIncludeSummGraphs;
    int m_iIncludeRawSigSelMin;
    int m_iIncludePatientInfo;
    int m_iIncludeEventLog;
    short int m_iIncludeRptItems[INC_RPT_LAST];
    int m_iRsaFirstPage;
    int m_iRsaOtherPage;
    int m_iSummGraphsFirstPage;
    int m_iSummGraphsOtherPage;
    CString m_cstrLogo;
    int m_iSummTableOnScreen;
    int m_iBothRSA;


    // Version I section
    CString m_cstrScaleFactor;
    int m_iPostwaterStart;
    
    // Reader section
    CString m_cstrReaderDBPath;

    // Methods
    void vRead();
    CString cstrGet_item(int iItem);
    int iGet_item(int iItem);
    float fGet_item(int iItem);
    bool bGet_item(int iItem);
    void vWrite_item(int iItem, int iValue);
    void vWrite_item(int iItem, CString cstrValue);
    void vWrite_item(int iItem, bool bValue);
    bool bGet_inc_rpt_item(int iItem);
    void vSet_inc_rpt_item(int iItem, BOOL bValue);

    void vCopy_rpt_item(short int * piSrc, short int * piDest);
    void vIncRptItems_string_to_array(CString cstrItems);
    void vWrite_inc_rpt_item(void);
    void vGet_study_date_info(void);
    void vWrite_study_date_info(void);
    void vCreate_guid(void);
  };
