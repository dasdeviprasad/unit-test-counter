/********************************************************************
Config.cpp

Copyright (C) 2012,2016,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CConfig class.

  This class reads in the EGGSAS.INI file and is used as the interface for all classes
  to the ini file (instead of accessing the ini file directly).


HISTORY:
13-FEB-12  RET  Version 
                  New.
22-AUG-12  RET
                Add INI item to the Report section demographic and facility items:
                  Array of 1's and 0's (comma delimited) to indicate if
                    individual demographic and facility items are to be
                    included in the report.
                Change methods: vRead()
                Add methods: iGet_inc_rpt_item(), vSet_inc_rpt_item(),
                  vWrite_inc_rpt_item(), vCopy_rpt_item(),
                  vIncRptItems_string_to_array()
                Modify the report configuration dialog box.
                  Change methods: vRead(), iGet_item(), vWrite_item()
03-OCT-12  RET
             Change for 60/200 CPM
               Add new item to config file for frequency range selection.
                 Change methods: vRead(), iGet_item(), vWrite_item()
14-DEC-12  RET
             Change to filter out frequencies below 10 cpm in the RSA.
               Add new item to config file for filtering RSA and
               printing/viewing both RSAs.
                 Change methods: vRead(), iGet_item(), vWrite_item(),
                   iGet_item() overload for int,
               Add method: bGet_item(), vWrite_item() overload for booleans
02-JUN-16  RET
             Add feature to hide study dates.
               Add item to the INI file:
                 section: GENERAL
                 Item: INI_ITEM_HIDE_STUDY_DATES
               Add methods: vCreate_guid(), vWrite_study_date_info(), vGet_study_date_info()
               Change method: vRead()
14-NOV-20  RET
             Add ability to work with a remote database.
             Changed settings to allow the user to specify the remote database.
              Change methods: vRead(), cstrGet_item(), vWrite_item()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "Config.h"
#include <stdio.h>
//#include "Iptypes.h"
//#include "Iphlpapi.h"
#include ".\config.h"
#include "util.h"

/********************************************************************
Constructor/destructor
********************************************************************/
CConfig::CConfig(void)
  {
  }

CConfig::~CConfig(void)
  {
  }

/********************************************************************
vRead - Read in the entire eggsas.ini file.

inputs: None.

return: None.
********************************************************************/
void CConfig::vRead()
  {
  CString cstrIncRptItems;
  int iValue;

  m_bHideStudyDates = false;

  // Make sure the program has the correct INI file to read.
  theApp.vSet_ini_file("eggsas.ini");

  // Read in the entire INI file.
  // General Section
  m_cstrDataFilePath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_DATAPATH, "");
  m_cstrPgmKey = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_PGMKEY, "");
  m_iDefaultWaterLoadUnits = AfxGetApp()->GetProfileInt(INI_SEC_GENERAL,
    INI_ITEM_WATERLOAD_UNITS, OUNCES);
  m_iMinPerGraph = AfxGetApp()->GetProfileInt(INI_SEC_GENERAL, INI_ITEM_MIN_PER_GRAPH,
    DEFAULT_MIN_PER_GRAPH);
  m_cstrBackupPath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_BACKUP_PATH, "");
  m_cstrCulture = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_LANGUAGE, "en-US");
  m_cstrDataFileStorage = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_DATA_FILE_STORAGE,
                      m_cstrDataFilePath);
  iValue = (short int)AfxGetApp()->GetProfileInt(INI_SEC_GENERAL, INI_ITEM_FILTER_RSA, 0);
  if(iValue == 0)
    m_bFilterRSA = false;
  else
    m_bFilterRSA = true;
  m_cstrRemoteDBPath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_PATH, "");
  m_cstrRemoteDBPasswd = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_PASSWD, "");
  m_cstrRemoteDBDefPasswd = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_DEF_PASSWD, "");

  vGet_study_date_info();

  // EGG Driver section
  m_fArtWindow = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
    INI_ITEM_ARTIFACTWINDOW, 1000);
  m_fStableEGGWindow = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
                                INI_ITEM_STABLEWINDOW_EGG, 1000);
  m_fStableRespWindow = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
                                INI_ITEM_STABLEWINDOW_RESP, 500);
  m_uStableTime = (unsigned)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
    INI_ITEM_STABLETIME, 30);
  m_iDoElectrodeTest = AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
    INI_ITEM_ELECTRODE_TEST, 1);
  m_fRespMult = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
    INI_ITEM_RESP_MULT, 1000);
  m_iExpireDays = AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER, INI_ITEM_SIGNALCORR, 0);
  m_fPosImpedanceLimit = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
                                           INI_ITEM_POS_IMPEDANCE_LIMIT, 200);
  m_fNegImpedanceLimit = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
                                           INI_ITEM_NEG_IMPEDANCE_LIMIT, 200);
  m_iDoBatteryCheck = AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
                                           INI_ITEM_BATTERY_CHECK, 1);
  m_iUseFakeData = AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
                                              INI_ITEM_USE_FAKE_DATA, 0);
  m_cstrDevID = AfxGetApp()->GetProfileString(INI_SEC_EGGDRIVER, INI_ITEM_DEVICE_ID, "");
  m_iFreqRange = AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER, INI_ITEM_FREQ_RANGE, 15);

  // Analysis section
  m_iChannel = (short int)AfxGetApp()->GetProfileInt(INI_SEC_ANALYSIS, INI_ITEM_CHANNEL, 1);
  m_cstrAllowDogStudies = AfxGetApp()->GetProfileString(INI_SEC_ANALYSIS,
    INI_ITEM_ALLOW_DOG_STUDIES, "");

  // Report section
  m_iIncludeCoreReport = AfxGetApp()->GetProfileInt(INI_SEC_REPORT, INI_ITEM_CORE_REPORT, 1);
  m_iIncludeEntireRawSignal = AfxGetApp()->GetProfileInt(INI_SEC_REPORT,
    INI_ITEM_ENTIRE_RAW_SIGNAL, 0);
  m_iIncludeDataSheet = AfxGetApp()->GetProfileInt(INI_SEC_REPORT, INI_ITEM_DATASHEET, 0);
  m_iIncludeDiagRecom = AfxGetApp()->GetProfileInt(INI_SEC_REPORT, INI_ITEM_DIAG_RECOM, 0);
  m_iIncludeRSA = AfxGetApp()->GetProfileInt(INI_SEC_REPORT, INI_ITEM_RSA, 0);
  m_iIncludeSummGraphs = AfxGetApp()->GetProfileInt(INI_SEC_REPORT, INI_ITEM_SUMM_GRAPHS, 0);
  m_iIncludeRawSigSelMin = AfxGetApp()->GetProfileInt(INI_SEC_REPORT,
    INI_ITEM_RAW_SIG_SEL_MIN, 0);
  m_iIncludePatientInfo = AfxGetApp()->GetProfileInt(INI_SEC_REPORT,
    INI_ITEM_PATIENT_INFO, 0);
  m_iIncludeEventLog = AfxGetApp()->GetProfileInt(INI_SEC_REPORT, INI_ITEM_EVENT_LOG, 0);
  cstrIncRptItems = AfxGetApp()->GetProfileString(INI_SEC_REPORT,
    INI_ITEM_INC_REPORT_ITEMS, "");
  vIncRptItems_string_to_array(cstrIncRptItems);
  m_iRsaFirstPage = AfxGetApp()->GetProfileInt(INI_SEC_REPORT, INI_ITEM_RSA_FIRST_PAGE, 0);
  m_iRsaOtherPage = AfxGetApp()->GetProfileInt(INI_SEC_REPORT, INI_ITEM_RSA_OTHER_PAGE, 0);
  m_iSummGraphsFirstPage = AfxGetApp()->GetProfileInt(INI_SEC_REPORT,
    INI_ITEM_SUMM_GRAPHS_FIRST_PAGE, 0);
  m_iSummGraphsOtherPage = AfxGetApp()->GetProfileInt(INI_SEC_REPORT,
    INI_ITEM_SUMM_GRAPHS_OTHER_PAGE, 0);
  m_cstrLogo = AfxGetApp()->GetProfileString(INI_SEC_REPORT, INI_ITEM_LOGO, "");
  m_iSummTableOnScreen = AfxGetApp()->GetProfileInt(INI_SEC_REPORT,
    INI_ITEM_SUMM_TABLE_ON_SCREEN, 0);
  m_iBothRSA = AfxGetApp()->GetProfileInt(INI_SEC_REPORT, INI_ITEM_BOTH_RSA, 0);


  // Version I section
  m_cstrScaleFactor = AfxGetApp()->GetProfileString(INI_SEC_VERSIONI,
    INI_ITEM_SCALEFACTOR, "1.0");
  m_iPostwaterStart = AfxGetApp()->GetProfileInt(INI_SEC_VERSIONI,
    INI_ITEM_POSTWATER_START, 20);
  
  // Reader section
  m_cstrReaderDBPath = AfxGetApp()->GetProfileString(INI_SEC_READER,
    INI_ITEM_READER_DB_PATH, "");
  }

/********************************************************************
cstrGet_item - Get the value of an item.

inputs: Item ID from Config_items enumerated list.

return: CString containing the value.
********************************************************************/
CString CConfig::cstrGet_item(int iItem)
  {
  CString cstrItem = "", cstrItem1;
  char szItem[1024];

  switch(iItem)
    {
    case CFG_DATAPATH:
      cstrItem = m_cstrDataFilePath;
      break;
    case CFG_PGMKEY:
      cstrItem = m_cstrPgmKey;
      break;
    case CFG_BACKUPPATH:
      cstrItem = m_cstrBackupPath;
      break;
    case CFG_DEVID:
      cstrItem = m_cstrDevID;
      break;
    case CFG_ALLOWDOGSTUDIES:
      cstrItem = m_cstrAllowDogStudies;
      break;
    case CFG_SCALEFACTOR:
      cstrItem = m_cstrScaleFactor;
      break;
    case CFG_READERDBPATH:
      cstrItem = m_cstrReaderDBPath;
      break;
    case CFG_DATA_FILE_STORAGE:
      cstrItem = m_cstrDataFileStorage;
      break;
    case CFG_LOGO:
      cstrItem = m_cstrLogo;
      break;
    case CFG_REMOTE_DB_PATH:
      cstrItem = m_cstrRemoteDBPath;
      break;
    case CFG_REMOTE_DB_PASSWD:
      cstrItem = m_cstrRemoteDBPasswd;
      break;
    case CFG_REMOTE_DB_DEF_PASSWD:
      cstrItem = m_cstrRemoteDBDefPasswd;
      break;
   }
  memcpy(szItem, cstrItem, cstrItem.GetLength() + 1);
  cstrItem1 = szItem;
  return(cstrItem1);
  }

/********************************************************************
iGet_item - Get the value of an item.

inputs: Item ID from Config_items enumerated list.

return: Integer containing the value.
********************************************************************/
int CConfig::iGet_item(int iItem)
  {
  int iResult;

  switch(iItem)
    {
    case CFG_WATERLOADUNITS:
      iResult = m_iDefaultWaterLoadUnits;
      break;
    case CFG_MINUTESPERGRAPH:
      iResult = m_iMinPerGraph;
      break;
    case CFG_STABLETIME:
      iResult = m_uStableTime;
      break;
    case CFG_ELECTRODETEST:
      iResult = m_iDoElectrodeTest;
      break;
    case CFG_SIGNALCORR:
      iResult = m_iExpireDays;
      break;
    case CFG_BATTERYCHECK:
      iResult = m_iDoBatteryCheck;
      break;
    case CFG_USEFAKEDATA:
      iResult = m_iUseFakeData;
      break;
    case CFG_CHANNEL:
      iResult = m_iChannel;
      break;
    case CFG_COREREPORT:
      iResult = m_iIncludeCoreReport;
      break;
    case CFG_ENTIRERAWSIGNAL:
      iResult = m_iIncludeEntireRawSignal;
      break;
     case CFG_DATASHEET:
      iResult = m_iIncludeDataSheet;
      break;
    case CFG_DIAGRECOMM:
      iResult = m_iIncludeDiagRecom;
      break;
    case CFG_RSA:
      iResult = m_iIncludeRSA;
      break;
    case CFG_SUMMGRAPHS:
      iResult = m_iIncludeSummGraphs;
      break;
    case CFG_RAWSIGSELMIN:
      iResult = m_iIncludeRawSigSelMin;
      break;
    case CFG_PATIENTINFO:
      iResult = m_iIncludePatientInfo;
      break;
    case CFG_EVENTLOG:
      iResult = m_iIncludeEventLog;
      break;
    case CFG_POSTWATERSTART:
      iResult = m_iPostwaterStart;
      break;
    case CFG_RSA_FIRST_PAGE:
      iResult = m_iRsaFirstPage;
      break;
    case CFG_RSA_OTHER_PAGE:
      iResult = m_iRsaOtherPage;
      break;
    case CFG_SUMM_GRAPHS_FIRST_PAGE:
      iResult = m_iSummGraphsFirstPage;
      break;
    case CFG_SUMM_GRAPHS_OTHER_PAGE:
      iResult = m_iSummGraphsOtherPage;
      break;
    case CFG_SUMM_TABLE_ON_SCREEN:
      iResult = m_iSummTableOnScreen;
      break;
    case CFG_FREQ_RANGE:
      iResult = m_iFreqRange;
      break;
    case CFG_BOTH_RSA:
      iResult = m_iBothRSA;
      break;
   }
  return(iResult);
  }

/********************************************************************
iGet_item - Get the value of an item.

inputs: Item ID from Config_items enumerated list.

return: Integer containing the value.
********************************************************************/
bool CConfig::bGet_inc_rpt_item(int iItem)
  {
    bool bRet;

    if(iItem < INC_RPT_LAST)
    {
      if(m_iIncludeRptItems[iItem] != 0)
        bRet = true;
      else
        bRet = false;
    }
    else
      bRet = false;
    return(bRet);
  }

/********************************************************************
iGet_item - Get the value of an item.

inputs: Item ID from Config_items enumerated list.

return: Integer containing the value.
********************************************************************/
void CConfig::vSet_inc_rpt_item(int iItem, BOOL bValue)
  {

    if(iItem < INC_RPT_LAST)
    {
      if(bValue == TRUE)
        m_iIncludeRptItems[iItem] = 1;
      else
        m_iIncludeRptItems[iItem] = 0;
    }
  }

/********************************************************************
fGet_item - Get the value of an item.

inputs: Item ID from Config_items enumerated list.

return: float containing the value.
********************************************************************/
float CConfig::fGet_item(int iItem)
  {
  float fResult;

    switch(iItem)
    {
    case CFG_ARTIFACTWINDOW:
      fResult = m_fArtWindow;
      break;
    case CFG_STABLEEGGWINDOW:
      fResult = m_fStableEGGWindow;
      break;
    case CFG_STABLERESPWINDOW:
      fResult = m_fStableRespWindow;
      break;
    case CFG_RESPMULT:
      fResult = m_fRespMult;
      break;
    case CFG_POSIMPEDANCELIMIT:
      fResult = m_fPosImpedanceLimit;
      break;
    case CFG_NEGIMPEDANCELIMIT:
      fResult = m_fNegImpedanceLimit;
      break;
    }

  return(fResult);
  }

/********************************************************************
bGet_item - Get the value of an item.

inputs: Item ID from Config_items enumerated list.

return: Boolean containing the value.
********************************************************************/
bool CConfig::bGet_item(int iItem)
  {
  bool bResult;

    switch(iItem)
    {
    case CFG_FILTER_RSA:
      bResult = m_bFilterRSA;
      break;
    }

  return(bResult);
  }

/********************************************************************
vWrite_item - Write an integer value to the INI file.

inputs: Item ID from Config_items enumerated list.
        Integer value.

return: None.
********************************************************************/
void CConfig::vWrite_item(int iItem, int iValue)
  {

  // Make sure the program has the correct INI file to read.
  theApp.vSet_ini_file("eggsas.ini");
  switch(iItem)
    {
    case CFG_MINUTESPERGRAPH:
      m_iMinPerGraph = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_GENERAL, INI_ITEM_MIN_PER_GRAPH, iValue);
      break;
    case CFG_COREREPORT:
      m_iIncludeCoreReport = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_CORE_REPORT, iValue);
     break;
    case CFG_DATASHEET:
      m_iIncludeDataSheet = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_DATASHEET, iValue);
      break;
    case CFG_ENTIRERAWSIGNAL:
      m_iIncludeEntireRawSignal = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_ENTIRE_RAW_SIGNAL, iValue);
      break;
    case CFG_DIAGRECOMM:
      m_iIncludeDiagRecom = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_DIAG_RECOM, iValue);
      break;
    case CFG_RSA:
      m_iIncludeRSA = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_RSA, iValue);
      break;
    case CFG_SUMMGRAPHS:
      m_iIncludeSummGraphs = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_SUMM_GRAPHS, iValue);
      break;
    case CFG_RAWSIGSELMIN:
      m_iIncludeRawSigSelMin = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_RAW_SIG_SEL_MIN, iValue);
      break;
    case CFG_PATIENTINFO:
      m_iIncludePatientInfo = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_PATIENT_INFO, iValue);
      break;
    case CFG_EVENTLOG:
      m_iIncludeEventLog = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_EVENT_LOG, iValue);
      break;
    case CFG_WATERLOADUNITS:
      m_iDefaultWaterLoadUnits = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_GENERAL, INI_ITEM_WATERLOAD_UNITS, iValue);
      break;
    case CFG_RSA_FIRST_PAGE:
      m_iRsaFirstPage = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_RSA_FIRST_PAGE, iValue);
      break;
    case CFG_RSA_OTHER_PAGE:
      m_iRsaOtherPage = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_RSA_OTHER_PAGE, iValue);
      break;
    case CFG_SUMM_GRAPHS_FIRST_PAGE:
      m_iSummGraphsFirstPage = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_SUMM_GRAPHS_FIRST_PAGE, iValue);
      break;
    case CFG_SUMM_GRAPHS_OTHER_PAGE:
      m_iSummGraphsOtherPage = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_SUMM_GRAPHS_OTHER_PAGE, iValue);
      break;
    case CFG_FREQ_RANGE:
      m_iFreqRange = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_EGGDRIVER, INI_ITEM_FREQ_RANGE, m_iFreqRange);
      break;
    case CFG_BOTH_RSA:
      m_iBothRSA = iValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_REPORT, INI_ITEM_BOTH_RSA, m_iBothRSA);
      break;
    }
  }

/********************************************************************
vWrite_item - Write an integer value to the INI file.

inputs: Item ID from Config_items enumerated list.
        CString value.

return: None.
********************************************************************/
void CConfig::vWrite_item(int iItem, CString cstrValue)
  {

  // Make sure the program has the correct INI file to read.
  theApp.vSet_ini_file("eggsas.ini");
  switch(iItem)
    {
    case CFG_BACKUPPATH:
      m_cstrBackupPath = cstrValue;
      AfxGetApp()->WriteProfileString(INI_SEC_GENERAL, INI_ITEM_BACKUP_PATH, cstrValue);
      break;
    case CFG_PGMKEY:
      m_cstrPgmKey = cstrValue;
      AfxGetApp()->WriteProfileString(INI_SEC_GENERAL, INI_ITEM_PGMKEY, cstrValue);
      break;
    case CFG_DEVID:
      m_cstrDevID = cstrValue;
      AfxGetApp()->WriteProfileString(INI_SEC_EGGDRIVER, INI_ITEM_DEVICE_ID, cstrValue);
      break;
    case CFG_READERDBPATH:
      m_cstrReaderDBPath = cstrValue;
      AfxGetApp()->WriteProfileString(INI_SEC_READER, INI_ITEM_READER_DB_PATH, cstrValue);
      break;
    case CFG_CULTURE:
      m_cstrCulture = cstrValue;
      AfxGetApp()->WriteProfileString(INI_SEC_GENERAL, INI_ITEM_LANGUAGE, cstrValue);
      break;
    case CFG_DATA_FILE_STORAGE:
      m_cstrDataFileStorage = cstrValue;
      AfxGetApp()->WriteProfileString(INI_SEC_GENERAL, INI_ITEM_DATA_FILE_STORAGE, cstrValue);
      break;
    case CFG_LOGO:
      m_cstrLogo = cstrValue;
      AfxGetApp()->WriteProfileString(INI_SEC_REPORT, INI_ITEM_LOGO, cstrValue);
      break;
    case CFG_REMOTE_DB_PATH:
      m_cstrRemoteDBPath = cstrValue;
      AfxGetApp()->WriteProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_PATH, cstrValue);
      break;
    }
  }

/********************************************************************
vWrite_item - Write a boolean value to the INI file.

inputs: Item ID from Config_items enumerated list.
        boolean value.

return: None.
********************************************************************/
void CConfig::vWrite_item(int iItem, bool bValue)
  {
  int iValue;

  // Convert boolean to integer for the INI file.
  if(bValue == true)
    iValue = 1;
  else
    iValue = 0;
  // Make sure the program has the correct INI file to read.
  theApp.vSet_ini_file("eggsas.ini");
  switch(iItem)
    {
    case CFG_FILTER_RSA:
      m_bFilterRSA = bValue;
      AfxGetApp()->WriteProfileInt(INI_SEC_GENERAL, INI_ITEM_FILTER_RSA, iValue);
      break;
    }
  }

/********************************************************************
vWrite_inc_rpt_item - Write the include in report array to the INI file.

inputs: None.

return: None.
********************************************************************/
void CConfig::vWrite_inc_rpt_item(void)
  {
    CString cstrValues = "";

    // Convert array to a string of 1's and 0's.
    for(int i = 0; i < INC_RPT_LAST; ++i)
    {
      if(m_iIncludeRptItems[i] == 0)
        cstrValues += "0"; // Item not checked
      else
        cstrValues += "1"; // Item checked
      if(i < (INC_RPT_LAST - 1))
        cstrValues += ","; // comma delimited
    }
    theApp.vSet_ini_file("eggsas.ini");
    AfxGetApp()->WriteProfileString(INI_SEC_REPORT, INI_ITEM_INC_REPORT_ITEMS, cstrValues);
  }

/********************************************************************
vCopy_rpt_item - Copy the include in report array to the specified buffer.

inputs: Source buffer.
        Destination buffer

return: None.
********************************************************************/
void CConfig::vCopy_rpt_item(short int * piSrc, short int * piDest)
  {
    for(int i = 0; i < INC_RPT_LAST; ++i, ++piSrc, ++piDest)
      *piDest = *piSrc;
  }

/********************************************************************
vIncRptItems_string_to_array - Convert the string containing the include
                               in report array to a short int array.

inputs: CString containing the array.

return: None.
********************************************************************/
void CConfig::vIncRptItems_string_to_array(CString cstrItems)
  {
    CString cstrToken;
    int iIndex, iPos = 0;

    if(cstrItems == "")
    { // The array is empty, all items unchecked.
      for(int i = 0; i < INC_RPT_LAST; ++i)
        m_iIncludeRptItems[i] = 0;
     }
    else
    {
      iIndex = 0;
      cstrToken = cstrItems.Tokenize(",", iPos);
      while (cstrToken != "" && iIndex < INC_RPT_LAST)
      {
        // convert string to number.
        if(cstrToken == "0")
          m_iIncludeRptItems[iIndex] = 0; // Item unchecked
        else
          m_iIncludeRptItems[iIndex] = 1; // Item checked
        ++iIndex;
        cstrToken = cstrItems.Tokenize(",", iPos); // Get next item.
      }
      // Array is larger that the number of items in the INI file.
      // This would be the case if new items have been added.
      while (iIndex < INC_RPT_LAST)
      {
        m_iIncludeRptItems[iIndex] = 0; // Item unchecked
        ++iIndex;
      }



    }
  }

/**********************************************************
vGet_study_date_info - Get the Hide Study Date password,
                       the flag that says whether the study dates
                       are hidden or shown, and the length of the password.

Inputs: None

Return: None

NOTES:
Needs library: Rpcrt4.lib
Password is a max of 10 characters.
GUID:
  9bff6d0a-e611-4e38-a745-c35829ad192c
  012345678901234567890123456789012345

  Dashes are 8, 13, 18, 23

  Each character is 2 nibbles or 2 hex digits.  so each
  character takes up 2 positions in the GUID.

  Password is   GUID
     0           10
     1            4
     2            24
     3            6
     4            1
     5            32
     6            15
     7            12
     8            27
     9            31
     10           3
     11           25
     12           33
     13           11
     14           21
     15           20
     16           9
     17           28
     18           17
     19           26
     PasswordLen  19
     Flag          22

**********************************************************/
void CConfig::vGet_study_date_info(void)
  {
  //CString cstrValue;
	//GUID guid;    // generated GUID
 // unsigned char *pszGUID;
  //unsigned char ucItem, ucItem1;
  char cItem;
  unsigned char ucLen;

  //m_cstrHideStudyDatesPassword
  m_cstrHideStudyDatesINIInfo = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_HIDE_STUDY_DATES, "");

  if(m_cstrHideStudyDatesINIInfo.IsEmpty() == true)
    {
    vCreate_guid();
    m_cstrHideStudyDatesPassword = "";
    //cstrValue.Empty();
    //guid = GUID_NULL;
    //HRESULT hr = ::UuidCreate(&guid);
    //UuidToString(&guid, &pszGUID);
    //// First time, make sure the hide date flag is set.
    //*(pszGUID + HIDE_STUDY_DATE_FLAG) = '1';
    ////AfxGetApp()->WriteProfileString(INI_SEC_GENERAL, INI_ITEM_HIDE_STUDY_DATES, (char *)pszGUID);
    ////m_cstrHideStudyDatesINIInfo = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_HIDE_STUDY_DATES, "");
    }
  else
    {  // Decode the INI item to the password and On/Off flag.
    ucLen = m_cstrHideStudyDatesINIInfo[HIDE_STUDY_DATE_FLAG] - '0';
    if(ucLen == 0)
      m_bHideStudyDates = false;
    else
      m_bHideStudyDates = true;
    // Now get the password length.
    if(m_cstrHideStudyDatesINIInfo[HIDE_STUDY_DATE_PASSWORD_LEN] == 'A')
      ucLen = 10;
    else
      ucLen = m_cstrHideStudyDatesINIInfo[HIDE_STUDY_DATE_PASSWORD_LEN] - '0';
    if(ucLen > 0)
      {
      cItem = cDecode_char_from_string(m_cstrHideStudyDatesINIInfo, HIDE_STUDY_DATE_0, HIDE_STUDY_DATE_1);
      m_cstrHideStudyDatesPassword = cItem;
      if(ucLen > 1)
        {
        cItem = cDecode_char_from_string(m_cstrHideStudyDatesINIInfo, HIDE_STUDY_DATE_2, HIDE_STUDY_DATE_3);
        m_cstrHideStudyDatesPassword += cItem;
        if(ucLen > 2)
          {
          cItem = cDecode_char_from_string(m_cstrHideStudyDatesINIInfo, HIDE_STUDY_DATE_4, HIDE_STUDY_DATE_5);
          m_cstrHideStudyDatesPassword += cItem;
          if(ucLen > 3)
            {
            cItem = cDecode_char_from_string(m_cstrHideStudyDatesINIInfo, HIDE_STUDY_DATE_6, HIDE_STUDY_DATE_7);
            m_cstrHideStudyDatesPassword += cItem;
            if(ucLen > 4)
              {
              cItem = cDecode_char_from_string(m_cstrHideStudyDatesINIInfo, HIDE_STUDY_DATE_8, HIDE_STUDY_DATE_9);
              m_cstrHideStudyDatesPassword += cItem;
              if(ucLen > 5)
                {
                cItem = cDecode_char_from_string(m_cstrHideStudyDatesINIInfo, HIDE_STUDY_DATE_10, HIDE_STUDY_DATE_11);
                m_cstrHideStudyDatesPassword += cItem;
                }
              }
            }
          }
        }
      }
    if(ucLen > 6)
      {
      cItem = cDecode_char_from_string(m_cstrHideStudyDatesINIInfo, HIDE_STUDY_DATE_12, HIDE_STUDY_DATE_13);
      m_cstrHideStudyDatesPassword += cItem;
      if(ucLen > 7)
        {
        cItem = cDecode_char_from_string(m_cstrHideStudyDatesINIInfo, HIDE_STUDY_DATE_14, HIDE_STUDY_DATE_15);
        m_cstrHideStudyDatesPassword += cItem;
        if(ucLen > 8)
          {
          cItem = cDecode_char_from_string(m_cstrHideStudyDatesINIInfo, HIDE_STUDY_DATE_16, HIDE_STUDY_DATE_17);
          m_cstrHideStudyDatesPassword += cItem;
          if(ucLen > 9)
            {
            cItem = cDecode_char_from_string(m_cstrHideStudyDatesINIInfo, HIDE_STUDY_DATE_18, HIDE_STUDY_DATE_19);
            m_cstrHideStudyDatesPassword += cItem;
            }
          }
        }
      }
    }
  }

/**********************************************************
vWrite_study_date_info - Write the Hide Study Date password,
                         the flag that says whether the study dates
                         are hidden or shown, and the length of the password.

Inputs: None

Return: None
**********************************************************/
void CConfig::vWrite_study_date_info(void)
  {
  unsigned char ucLen, ucHide;

  ucLen = (unsigned char)m_cstrHideStudyDatesPassword.GetLength();
  m_cstrHideStudyDatesINIInfo.SetAt(HIDE_STUDY_DATE_PASSWORD_LEN, ucLen + '0');

  // Update the hide study dates flag.
  if(m_bHideStudyDates == true)
    ucHide = '1';
  else
    ucHide = '0';
  m_cstrHideStudyDatesINIInfo.SetAt(HIDE_STUDY_DATE_FLAG, ucHide);

  if(ucLen > 0)
    {
    vEncode_char_in_string(m_cstrHideStudyDatesPassword[0], m_cstrHideStudyDatesINIInfo,
      HIDE_STUDY_DATE_0, HIDE_STUDY_DATE_1);
    if(ucLen > 1)
      {
      vEncode_char_in_string(m_cstrHideStudyDatesPassword[1], m_cstrHideStudyDatesINIInfo,
        HIDE_STUDY_DATE_2, HIDE_STUDY_DATE_3);
      if(ucLen > 2)
        {
        vEncode_char_in_string(m_cstrHideStudyDatesPassword[2], m_cstrHideStudyDatesINIInfo,
          HIDE_STUDY_DATE_4, HIDE_STUDY_DATE_5);
        if(ucLen > 3)
          {
          vEncode_char_in_string(m_cstrHideStudyDatesPassword[3], m_cstrHideStudyDatesINIInfo,
            HIDE_STUDY_DATE_6, HIDE_STUDY_DATE_7);
          if(ucLen > 4)
            {
            vEncode_char_in_string(m_cstrHideStudyDatesPassword[4], m_cstrHideStudyDatesINIInfo,
              HIDE_STUDY_DATE_8, HIDE_STUDY_DATE_9);
            if(ucLen > 5)
              {
              vEncode_char_in_string(m_cstrHideStudyDatesPassword[5], m_cstrHideStudyDatesINIInfo,
                HIDE_STUDY_DATE_10, HIDE_STUDY_DATE_11);
              }
            }
          }
        }
      }
    }
  if(ucLen > 6)
    {
    vEncode_char_in_string(m_cstrHideStudyDatesPassword[6], m_cstrHideStudyDatesINIInfo,
      HIDE_STUDY_DATE_12, HIDE_STUDY_DATE_13);
    if(ucLen > 7)
      {
      vEncode_char_in_string(m_cstrHideStudyDatesPassword[7], m_cstrHideStudyDatesINIInfo,
        HIDE_STUDY_DATE_14, HIDE_STUDY_DATE_15);
      if(ucLen > 8)
        {
        vEncode_char_in_string(m_cstrHideStudyDatesPassword[8], m_cstrHideStudyDatesINIInfo,
          HIDE_STUDY_DATE_16, HIDE_STUDY_DATE_17);
        if(ucLen > 9)
          {
          vEncode_char_in_string(m_cstrHideStudyDatesPassword[9], m_cstrHideStudyDatesINIInfo,
            HIDE_STUDY_DATE_18, HIDE_STUDY_DATE_19);
          }
        }
      }
    }
  // Make sure the program has the correct INI file to read.
  theApp.vSet_ini_file("eggsas.ini");
  AfxGetApp()->WriteProfileString(INI_SEC_GENERAL, INI_ITEM_HIDE_STUDY_DATES, m_cstrHideStudyDatesINIInfo);
  }

/**********************************************************
vCreate_guid - Create a GUID for the Hide Study Dates items,
               set the flag that says whether the study dates
               are hidden or shown to hidden

Inputs: None

Return: None
**********************************************************/
void CConfig::vCreate_guid(void)
  {
  //GUID guid;    // generated GUID
  //unsigned char *pszGUID;
  CString cstrGUID;

  cstrGUID = cstrCreate_guid();
  //guid = GUID_NULL;
  //HRESULT hr = ::UuidCreate(&guid);
  //UuidToString(&guid, &pszGUID);
  // First time, make sure the hide date flag is set.
  //*(pszGUID + HIDE_STUDY_DATE_FLAG) = '1';
  m_bHideStudyDates = true;
  AfxGetApp()->WriteProfileString(INI_SEC_GENERAL, INI_ITEM_HIDE_STUDY_DATES, cstrGUID); //(char *)pszGUID);
  m_cstrHideStudyDatesINIInfo = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_HIDE_STUDY_DATES, "");
  }
