/********************************************************************
SpreadSheet.cpp

Copyright (C) 2007,2012,2016 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CSpreadSheet class.

  This class creates a comma delimited file of the Data Sheet report
  ready for importing into a spreadsheet.

HISTORY:
28-JUL-07  RET  New.
21-FEB-12  RET  Version 
                  Changes for foreign languages.
03-OCT-12  RET
             Change for 60/200 CPM
               Change methods: vCreate_data_sheet()
09-JUN-16  RET
             Add feature to hide study dates.
               Change the date of the spreadsheet file to be an earlier date.
                 Change methods: Destructor
               If the date is encrypted, don't put it in the spreadsheet.
                 Change method: vCreate_data_sheet()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "MainFrm.h"
#include "EGGSASDoc.h"
#include "analyze.h"
#include "spreadsheet.h"
#include "util.h"

// Array of Text IDs for the left column titles.
static unsigned s_uLeftColTitleID[] = 
  {
  IDS_SS_LEFT_TITLE_1, IDS_SS_LEFT_TITLE_2, IDS_SS_LEFT_TITLE_3,
  IDS_SS_LEFT_TITLE_4, IDS_SS_LEFT_TITLE_5, IDS_SS_LEFT_TITLE_6,
  IDS_SS_LEFT_TITLE_7, IDS_SS_LEFT_TITLE_8, IDS_SS_LEFT_TITLE_9,
  IDS_SS_LEFT_TITLE_10, IDS_SS_LEFT_TITLE_11
  };

/////////////////////////////////////////////////////////////////////
//// Used for creating a spreadsheet file (.xls)
#ifdef USE_SPREADSHEET_PROGRAM
  // Commonly used OLE variants.
COleVariant
   covTrue((short)TRUE),
   covFalse((short)FALSE),
   covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
#endif
/////////////////////////////////////////////////////////////////////

/********************************************************************
Constructor

  Initialize class data.
  Create a default file name.
  Display a "Save As" file dialog box so user can choose.
  Create the spread sheet file.

  inputs: Pointer to the document object.
********************************************************************/
CSpreadSheet::CSpreadSheet(CEGGSASDoc *pDoc)
  {
  CString cstrMsg;

	m_pDoc = pDoc;

    // Make sure all pointers to allocated data are reset.
  m_adData.pfRawData = NULL;
  m_adData.pfPwrSum = NULL;
  m_adData.pfDomFreq = NULL;
  m_adData.pfRatioAvgPwr = NULL;
  m_adData.pfPctDomFreq = NULL;
  m_pcfSSFile = NULL;
    // Number of post_stimulation periods plus 1 baseline period.
  m_uMaxPeriods = m_pDoc->m_pdPatient.uNumPeriods + 1;
    // Analyze the data for the Data Sheet.
  vAnalyze_data();
  if(bSelect_path() == true)
    {
    if(bCreate_file() == true)
      {
      vCreate_data_sheet();
        // Display a message indicating the Spreadsheet file has been created along
        // with the path and file name.  Do this even if there was a file write
        // error because the user may be able to see at least some of the data.
      //cstrMsg.LoadString(IDS_SS_FILE_CREATED);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_SS_FILE_CREATED);
      cstrMsg += "\r\n";
      cstrMsg += m_cstrSSPathFile;
      AfxMessageBox(cstrMsg);
      }
    }

/////////////////////////////////////////////////////////////////////
//// Used for creating a spreadsheet file (.xls)
#ifdef USE_SPREADSHEET_PROGRAM
    // Copy the spreadsheet template to a spreadsheet with the
    // file name of the patient and date (same file name as the
    // data file but with a ".xls" extension.
  cstrDestFileName = m_pDoc->cstrCreate_data_file_name(&m_pDoc->m_pdPatient, FILE_TYPE_SS);
  cstrSrcFileName.Format("%s\\%s", theApp.m_cstrProgramPath, SPREADSHEET_NAME);
  if(iCopy_one_file((LPCTSTR)cstrDestFileName, (LPCTSTR)cstrSrcFileName, NULL, TRUE)
  == SUCCESS)
    {
    try
      {
        // Start the spreadsheet program
      if(!m_appExcel.CreateDispatch("Excel.Application"))
        AfxMessageBox("Cannot start Excel and get Application object.");
      else
        {
        CString cstrExcelVersion = m_appExcel.get_Version();
          // Get the Workbooks collection.
        m_Books = m_appExcel.get_Workbooks();
        m_Book = m_Books.Open(cstrDestFileName,
                       covOptional, covOptional, covOptional, covOptional, covOptional,
                       covOptional, covOptional, covOptional, covOptional, covOptional,
                       covOptional, covOptional, covOptional, covOptional );
        m_Sheets = m_Book.get_Sheets();
        m_Sheet = m_Sheets.get_Item(COleVariant((short)1));
    
          // Put the Data Sheet into the spreadsheet
        vCreate_data_sheet();
        m_Book.Save(); // Save the spreadsheet.
        }
      } // end of try
    catch(COleException *e)
      {
      char szErr[1024];     // For the Try...Catch error message.
      CString cstrErr;
      e->GetErrorMessage(szErr, sizeof(szErr), NULL);
      cstrErr.Format("COleException. SCODE: %08lx\n%s", (long)e->m_sc, szErr);
      AfxMessageBox(cstrErr, MB_OK);
//      ::MessageBox(NULL, buf, "COleException", MB_SETFOREGROUND | MB_OK);
      }
    catch(COleDispatchException *e)
      {
      CString cstrErr;
      cstrErr.Format("COleDispatchException. SCODE: %08lx\n%s",
                    (long)e->m_wCode, e->m_strDescription);
      AfxMessageBox(cstrErr, MB_OK);
//      char buf[1024];     // For the Try...Catch error message.
//      sprintf(buf,
//             "COleDispatchException. SCODE: %08lx, Description: \"%s\".",
//             (long)e->m_wCode,(LPSTR)e->m_strDescription.GetBuffer(512));
//      ::MessageBox(NULL, buf, "COleDispatchException",
//                         MB_SETFOREGROUND | MB_OK);
      }
    catch(CException *e)
      {
      char szErr[1024];     // For the Try...Catch error message.
      CString cstrErr;
      e->GetErrorMessage(szErr, sizeof(szErr), NULL);
      cstrErr.Format("General Exception: %s", szErr);
      AfxMessageBox(cstrErr, MB_OK);
//      ::MessageBox(NULL, "General Exception caught.", "Catch-All",
//                         MB_SETFOREGROUND | MB_OK);
      }
    if(m_appExcel.m_lpDispatch != NULL)
      {
        // Display the spreadsheet.
        //Make the application visible and give the user control of
        //Microsoft Excel.
      m_appExcel.put_Visible(TRUE);
      m_appExcel.put_UserControl(TRUE);
      }
/**********************
    if(m_Books.m_lpDispatch != NULL)
      m_Books.ReleaseDispatch();
    if(m_Book.m_lpDispatch != NULL)
      m_Book.ReleaseDispatch();
    if(m_Sheets.m_lpDispatch != NULL)
      m_Sheets.ReleaseDispatch();
    if(m_Sheet.m_lpDispatch != NULL)
      m_Sheet.ReleaseDispatch();
    if(m_appExcel.m_lpDispatch != NULL)
      m_appExcel.ReleaseDispatch();
************************/
    }
#endif
/////////////////////////////////////////////////////////////////////
  }

/********************************************************************
Destructor

  Clean up class data.
    - Free all allocated memory used for the data analyses.
    - Close the spreadsheet file.
********************************************************************/
  CSpreadSheet::~CSpreadSheet(void)
    {

    vFree_data_sheet_arrays();

    if(m_pcfSSFile != NULL)
      { // Close the data file.
      m_pcfSSFile->Close(); // Close the file.
      delete m_pcfSSFile;
#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
#if EGGSAS_READER == READER
        // Set the file time to an earlier time so user won't be able
        // to tell if the date of the study from the file time.
        vSet_file_date_time(m_cstrSSPathFile);
#else
      if(g_pConfig->m_bHideStudyDates == true)
        {
        // Set the file time to an earlier time so user won't be able
        // to tell if the date of the study from the file time.
        vSet_file_date_time(m_cstrSSPathFile);
        }
#endif
#endif
      }
    }

/********************************************************************
vFree_data_sheet_arrays - Free the memory used by the arrays allocated
                          for the data sheet calculations.

  inputs: None.

  return: None.
********************************************************************/
void CSpreadSheet::vFree_data_sheet_arrays()
  {

  if(m_adData.pfPwrSum != NULL)
    {
    HeapFree(GetProcessHeap(), 0, (LPVOID)m_adData.pfPwrSum);
    m_adData.pfPwrSum = NULL;
    }

  if(m_adData.pfRatioAvgPwr != NULL)
    {
    HeapFree(GetProcessHeap(), 0, (LPVOID)m_adData.pfRatioAvgPwr);
    m_adData.pfRatioAvgPwr = NULL;
    }
  
  if(m_adData.pfDomFreq != NULL)
    {
    HeapFree(GetProcessHeap(), 0, (LPVOID)m_adData.pfDomFreq);
    m_adData.pfDomFreq = NULL;
    }

  if(m_adData.pfPctDomFreq != NULL)
    {
    HeapFree(GetProcessHeap(), 0, (LPVOID)m_adData.pfPctDomFreq);
    m_adData.pfPctDomFreq = NULL;
    }
  }

/********************************************************************
vAnalyze_data - Analyze the data for the Data Sheet.

  inputs: None.

  return: None.
********************************************************************/
void CSpreadSheet::vAnalyze_data()
  {
  REQ_ANALYZE_DATA reqAnlData;
  short int iIndex, iRet = SUCCESS;
  unsigned short uSize, iMaxFreqPts, uEpochCnt;
  float fLastMinute, fStartMinute;

    // First find out how many data points will be needed and allocate an array.
    // total CPM * 4 so that we get four points for each cpm.
  iMaxFreqPts = ((15 - 0) * 4) + 1;
  uSize = 0;
  for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
    {
    if(m_pDoc->m_pdPatient.fGoodMinStart[iIndex] != INV_MINUTE)
      { // Now find out how many data points for this period.
        // Add and extra 2 for the blanks lines separating pre and post water periods.
      uSize += (((unsigned short)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
                - (unsigned short)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 2 + 2)
                * iMaxFreqPts);
      }
    else
      iRet = FAIL;
    }
  if(iRet == SUCCESS)
    { // Allocate data arrays needed for the data sheet.
    vFree_data_sheet_arrays();
    if((m_adData.pfPwrSum = (float *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
    (m_uMaxPeriods * sizeof(float) * theApp.m_iMaxFreqBands))) == NULL
    || (m_adData.pfRatioAvgPwr = (float *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
    (m_uMaxPeriods * sizeof(float) * theApp.m_iMaxFreqBands))) == NULL
    || (m_adData.pfDomFreq = (float *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
    uSize * sizeof(float))) == NULL
    || (m_adData.pfPctDomFreq = (float *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
    (m_uMaxPeriods * sizeof(float) * theApp.m_iMaxFreqBands))) == NULL)
      iRet = FAIL;
    }
  if(iRet == SUCCESS)
    {
    for(iIndex = 0; iIndex < MAX_TOTAL_PERIODS * theApp.m_iMaxFreqBands; ++iIndex)
      m_adData.fAvgPwr[iIndex] = 0.0F;

    uEpochCnt = 0;
      // For each period that has good minutes defined.
    for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
      { // Now for each valid period, analyze all the 4 minute epochs.
        // Read in and graph the baseline period.
      if(m_pDoc->iRead_period(iIndex, m_pDoc->m_fData) == SUCCESS)
        {
        reqAnlData.iChannel = 0; // EGG channel.
        reqAnlData.iNumChannelsInFile = 2;
        reqAnlData.iType = DATA_SHEET_TYPE;
        reqAnlData.padData = &m_adData;
        reqAnlData.bUsePartMinAtDataEnd = false;
        reqAnlData.uTimeBand = iIndex;
        fLastMinute = m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] - 4;
        fStartMinute = m_pDoc->m_pdPatient.fGoodMinStart[iIndex];
        while(fStartMinute <= fLastMinute)
          {
          if(theApp.m_ptStudyType == PT_STANDARD)
            reqAnlData.fEpochNum = fStartMinute; // Standard study
          else // Research study
            reqAnlData.fEpochNum = fStartMinute - m_pDoc->m_pdPatient.fPeriodStart[iIndex];
          reqAnlData.uRunningEpochCnt = uEpochCnt;
          if(m_pDoc->m_pAnalyze->bAnalyze_data(&reqAnlData) == true)
            { // Save the data for graphing.
//            memcpy((unsigned char *)pfData, adData.fData, iMaxFreqPts * sizeof(float));
//            pfData += m_iMaxFreqPts;
            }
          else
            {
            iRet = FAIL;
            break;
            }
          ++fStartMinute;
          ++uEpochCnt;
          }
        reqAnlData.uRunningEpochCnt = uGet_num_epochs(m_pDoc->m_pdPatient.fGoodMinStart[iIndex],
          m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]);
        //reqAnlData.uRunningEpochCnt = (unsigned short)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
        //                               - (unsigned short)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 3; //2;
        m_pDoc->m_pAnalyze->vCalc_diagnosis(&reqAnlData);
          // Do some additional analysis for the data sheet.
        m_pDoc->m_pAnalyze->vCalc_datasheet(&reqAnlData);
        }
      else
        {
        iRet = FAIL; // Reading data file failed
        break;
        }
      }
    }
  }

/********************************************************************
bSelect_path - Display a "Save As" file dialog box so user can select
               a path and file name other than the defaults.
               If the selected file already exists, the user will be
               prompted to overwrite it by the file dialog box.
               The default file extension is ".csv".
               The default file path is the program's data file path.

  inputs: None.

  return: true if user selected OK from the file dialog box.
          false if user selected Cancel from the file dialog box.
********************************************************************/
bool CSpreadSheet::bSelect_path()
  {
  CFileDialog *pdlg;
  bool bRet;

  m_cstrFileName = m_pDoc->cstrCreate_data_file_name(&m_pDoc->m_pdPatient, FILE_TYPE_SS);
    // Display a "Save As" dialog box.
  pdlg = new CFileDialog(FALSE, "csv", m_cstrFileName,
                         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                         "Spreadsheet Files (*.csv)|*.csv|All Files (*.*)|*.*||");
    // Specify default path.
  pdlg->m_pOFN->lpstrInitialDir = theApp.m_cstrDataFilePath;
  if(pdlg->DoModal() == IDOK)
    { // User selected a path and file.
    m_cstrSSPathFile = pdlg->GetPathName(); // Destination Path and file
    m_cstrFileName = pdlg->GetFileName(); // Destination file name.
    bRet = true;
    }
  else
    bRet = false;

  return(bRet);
  }

/********************************************************************
bCreate_file - Create the Spreadsheet file for writing.  The file will
               be truncated if it already exists.

  inputs: None.

  return: true if the file was created.
          false if there was an error.  The error is written to the log
            file as well as being displayed.
********************************************************************/
bool CSpreadSheet::bCreate_file()
  {
  CString cstrMsg, cstrErrTitle;
  bool bRet = true;

  if(bRet == true)
    { // Open the data file for writing.
    try
      {
      m_pcfSSFile = new CFile(m_cstrSSPathFile, 
                     CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite);
      }
    catch(CFileException *pfe)
      { // Got a file creation error.
      //cstrMsg.Format(IDS_ERR_CREATE_FILE, m_cstrFileName, strerror(pfe->m_lOsError));
      cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_CREATE_FILE), m_cstrFileName, strerror(pfe->m_lOsError));
      theApp.vLog(cstrMsg);
      //cstrErrTitle.LoadString(IDS_ERROR1);
      cstrErrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
      AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErrTitle, MB_OK | MB_ICONSTOP);
      pfe->Delete();
      if(m_pcfSSFile != NULL)
        {
        m_pcfSSFile->Close(); // Close the file.
        delete m_pcfSSFile;
        m_pcfSSFile = NULL;
        }
      bRet = false;
      }
    }
  return(bRet);
  }

/********************************************************************
iWrite_to_ss - Write a line of text and an optional blank line to the file.
               If the line of text is empty, only write the blank line
               to the file if specified.
                         
  inputs: CString containing the line of text.  The line does not contain
            a carriage return or line feed.
          true to write an empty line after the current line.

  return: SUCCESS if the data was successfully written to the file.
          FAIL if a file write occurs.
********************************************************************/
short int CSpreadSheet::iWrite_to_ss(CString cstrLine, bool bEmptyLine)
  {
  short int iRet;
  unsigned char ucCRLF[2] = {0xD, 0xA};

  try
    {
    if(cstrLine.IsEmpty() == false)
      {
      m_pcfSSFile->Write((void *)(LPCTSTR)cstrLine, cstrLine.GetLength());
      m_pcfSSFile->Write((void *)ucCRLF, sizeof(ucCRLF));
      }
    if(bEmptyLine == true)
      m_pcfSSFile->Write((void *)ucCRLF, sizeof(ucCRLF)); // Add empty line.
    iRet = SUCCESS;
    }
  catch(CFileException *pfe)
    { // Got an error while writing to the file.  Error is displayed by caller.
    CString cstrMsg, cstrErrTitle;

    //cstrMsg.Format(IDS_ERR_WRITE_FILE, m_cstrFileName, strerror(pfe->m_lOsError));
    cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_WRITE_FILE), m_cstrFileName, strerror(pfe->m_lOsError));
    theApp.vLog(cstrMsg);
    //cstrErrTitle.LoadString(IDS_ERROR1);
    cstrErrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErrTitle, MB_OK | MB_ICONSTOP);
    pfe->Delete();
    iRet = FAIL;
    }
  return(iRet);
  }

/********************************************************************
vCreate_data_sheet - Create the data sheet as a comma delimited file.
                     The file can be imported into the spreadsheet program.

  inputs: None.

  return: None.
********************************************************************/
void CSpreadSheet::vCreate_data_sheet()
  {
  CString cstrItem, cstrItem1, cstrCell;
  short int iIndex, iIndex1, iDataIndex, iCol, iColIndex, iColTitleIndex;
  short int iWriteSts, iFreqColumns;
  unsigned uNumOfPoints;
  float fStdDev, fMean, fPct;

    // Add Patient Name and date of study.
  if(m_pDoc->m_pdPatient.bUseResrchRefNum == TRUE)
    cstrItem = g_pLang->cstrLoad_string(ITEM_G_REF_NUM);
  else
    cstrItem = g_pLang->cstrLoad_string(ITEM_G_PATIENT_NAME);
  cstrItem += ": ";
  //vFormat_patient_name(&m_pDoc->m_pdPatient, cstrItem1, true);
  if(m_pDoc->m_pdPatient.bUseResrchRefNum == TRUE)
    cstrItem1 = m_pDoc->m_pdPatient.cstrResearchRefNum;
  else
    vFormat_patient_name(&m_pDoc->m_pdPatient, cstrItem1, true);
  cstrItem += cstrItem1;
  if((iWriteSts = iWrite_to_ss(cstrItem, false)) == SUCCESS)
    {
    cstrItem = g_pLang->cstrLoad_string(ITEM_G_DATE_OF_STUDY);
    cstrItem += ": ";
    if(m_pDoc->m_pdPatient.cstrDate.GetLength() <= 10)
      cstrItem += m_pDoc->m_pdPatient.cstrDate;
    iWriteSts = iWrite_to_ss(cstrItem, true);
    }

#ifdef _200_CPM_HARDWARE
    if(theApp.m_iFreqLimit == FREQ_LIMIT_15_CPM)
      iFreqColumns = FREQ_BAND7 - 2;
    else if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
      iFreqColumns = MAX_FREQ_BANDS_60_CPM - 2;
    else //  if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
      iFreqColumns = MAX_FREQ_BANDS_200_CPM - 2;
#else
  iFreqColumns = FREQ_BAND7 - 2;
#endif

    // Add first row of column titles.
  cstrItem = ",";
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_BRADY);
  cstrItem += (cstrItem1 + ",");
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_NORMAL);
  cstrItem += (cstrItem1 + ",");
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_TACHY);
  cstrItem += (cstrItem1 + ",");
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_DUOD);
  cstrItem += (cstrItem1 + ",");

#ifdef _200_CPM_HARDWARE
    int iMax;
    if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM
    || theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
      {
      if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
        iMax = FREQ_BAND9;
      else
        iMax = FREQ_BAND18;
      for(int i = FREQ_BAND4; i < iMax; ++i)
        cstrItem += ",";
      }
#endif

  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_ENTIRE_RANGE);
  cstrItem += (cstrItem1 + ",");
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_PERIOD);
  cstrItem += cstrItem1;
  iWriteSts = iWrite_to_ss(cstrItem, false);

    // Add second row of column titles.
  if(iWriteSts == SUCCESS)
    {
    cstrItem = ",";
    cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_1_25CPM);
    cstrItem += (cstrItem1 + ",");
    cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_25_375CPM);
    cstrItem += (cstrItem1 + ",");
    cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_375_100CPM);
    cstrItem += (cstrItem1 + ",");
    cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_100_150CPM);
    cstrItem += (cstrItem1 + ",");

#ifdef _200_CPM_HARDWARE
    if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM
    || theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
      {
      int iMax;
      float fCPM, fCPMInc;
      if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
        iMax = FREQ_BAND9;
      else
        iMax = FREQ_BAND18;
      fCPM = 15.0F;
      fCPMInc = 5.0F;
      for(int i = FREQ_BAND4; i < iMax; ++i)
        {
        cstrItem1.Format("%.1f - %.1f %s", fCPM, fCPM + fCPMInc, g_pLang->cstrLoad_string(ITEM_G_CPM));
        fCPM += fCPMInc;
        if(fCPM < 100.0F)
          fCPMInc = 10.0F;
        else
          fCPMInc = 20.0F;
        cstrItem += (cstrItem1 + ",");
        }
      }
#endif

    if(theApp.m_iFreqLimit == FREQ_LIMIT_15_CPM)
      cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_1_15CPM);
    else if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
      cstrItem1 = "1 - 60 " + g_pLang->cstrLoad_string(ITEM_G_CPM);
    else // if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
      cstrItem1 = "1 - 200 " + g_pLang->cstrLoad_string(ITEM_G_CPM);
    cstrItem += (cstrItem1 + ",");
    //cstrItem1.LoadString(IDS_MINUTES);
    cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_MINUTES);
    cstrItem += cstrItem1;
    iWriteSts = iWrite_to_ss(cstrItem, true);
    }

  if(iWriteSts == SUCCESS)
    { // Distribution of average power by frequency region
    iColTitleIndex = 0;
    for(iIndex = 0; iIndex < m_uMaxPeriods && iWriteSts == SUCCESS; ++iIndex)
      { // Go through this loop for each timeband specified.
      if(iIndex < MIN_ROWS_DIST_AVG_PWR_FREQ_REG)
        { // Add the left side title.
        cstrItem = cstrGet_col_title(s_uLeftColTitleID[iColTitleIndex]);
        cstrItem += ",";
        ++iColTitleIndex;
        }
      else
        cstrItem = ",";
      iDataIndex = iIndex * theApp.m_iMaxFreqBands;
      //iDataIndex = iIndex * MAX_FREQ_BANDS;
      //for(iIndex1 = FREQ_BAND1; iIndex1 <= FREQ_BAND7; ++iIndex1)
      for(iIndex1 = FREQ_BAND1; iIndex1 <= iFreqColumns; ++iIndex1)
        { // Go through this loop for each frequency band.
        ////if(iIndex1 != FREQ_BAND5 && iIndex1 != FREQ_BAND6)
          {
#ifdef _200_CPM_HARDWARE
          cstrItem1.Format("%3.4f", m_adData.fPctGrp1[iDataIndex + iIndex1]);
#else
          cstrItem1.Format("%3.2f", m_adData.fPctGrp1[iDataIndex + iIndex1]);
#endif
          cstrItem += (cstrItem1 + ",");
          }
        }
        // Add the timeband start and end minutes.
      cstrItem += cstrGet_timeband_start_end_min(iIndex);
        // Add to spreadsheet file.
      iWriteSts = iWrite_to_ss(cstrItem, false);
      } // end of for each period.
    if(iWriteSts == SUCCESS)
      {
      if(iIndex < MIN_ROWS_DIST_AVG_PWR_FREQ_REG)
        { // Haven't finished adding the left side titles.
        while(iIndex < MIN_ROWS_DIST_AVG_PWR_FREQ_REG)
          {
          if((iWriteSts = iAdd_left_title(iColTitleIndex)) == FAIL)
            break;
          ++iColTitleIndex;
          ++iIndex;
          }
        }
      }
    }
    // Put in an empty line.
  if(iWriteSts == SUCCESS)
    {
    cstrItem.Empty();
    iWriteSts = iWrite_to_ss(cstrItem, true);
    }
  if(iWriteSts == SUCCESS)
    { // Ratio of average powers by frequency range.
    for(iIndex = 1; iIndex < m_uMaxPeriods; ++iIndex)
      { // Go through this loop for each timeband specified.
      if((iIndex - 1) < MIN_ROWS_RATIO_AVG_PWR)
        { // Add the left side title.
        //cstrItem.LoadString(s_uLeftColTitleID[iColTitleIndex]);
        cstrItem = cstrGet_col_title(s_uLeftColTitleID[iColTitleIndex]);
        cstrItem += ",";
        ++iColTitleIndex;
        }
      else
        cstrItem = ",";
      iDataIndex = iIndex * theApp.m_iMaxFreqBands;
      //iDataIndex = iIndex * MAX_FREQ_BANDS;
      //for(iIndex1 = FREQ_BAND1; iIndex1 <= FREQ_BAND7; ++iIndex1)
      for(iIndex1 = FREQ_BAND1; iIndex1 <= iFreqColumns; ++iIndex1)
        { // Go through this loop for each frequency band.
        ////if(iIndex1 != FREQ_BAND5 && iIndex1 != FREQ_BAND6)
          {
          cstrItem1.Format("%.2f", *(m_adData.pfRatioAvgPwr + iDataIndex + iIndex1));
          cstrItem += (cstrItem1 + ",");
          }
        }
        // Add the timeband start and end minutes.
      cstrItem += cstrGet_timeband_start_end_min(iIndex);
        // Add to spreadsheet file.
      iWriteSts = iWrite_to_ss(cstrItem, false);
      } // end of for each period.
    if(iWriteSts == SUCCESS)
      {
      if(--iIndex < MIN_ROWS_RATIO_AVG_PWR)
        { // Haven't finished adding the left side titles.
        while(iIndex < MIN_ROWS_RATIO_AVG_PWR)
          {
          if((iWriteSts = iAdd_left_title(iColTitleIndex)) == FAIL)
            break;
          ++iColTitleIndex;
          ++iIndex;
          }
        }
      }
    }
    // Put in an empty line.
  if(iWriteSts == SUCCESS)
    {
    cstrItem.Empty();
    iWriteSts = iWrite_to_ss(cstrItem, true);
    }
  if(iWriteSts == SUCCESS)
    { // Distribution of average power by frequency range
    for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
      { // Go through this loop for each timeband specified.
      if(iIndex < MIN_ROWS_DIST_AVG_PWR_FREQ_RANGE)
        { // Add the left side title.
        //cstrItem.LoadString(s_uLeftColTitleID[iColTitleIndex]);
        cstrItem = cstrGet_col_title(s_uLeftColTitleID[iColTitleIndex]);
        cstrItem += ",";
        ++iColTitleIndex;
        }
      else
        cstrItem = ",";
      iDataIndex = iIndex * theApp.m_iMaxFreqBands;
      //iDataIndex = iIndex * MAX_FREQ_BANDS;
      //for(iIndex1 = FREQ_BAND1; iIndex1 <= FREQ_BAND7; ++iIndex1)
      for(iIndex1 = FREQ_BAND1; iIndex1 <= iFreqColumns; ++iIndex1)
        { // Go through this loop for each frequency band.
        ////if(iIndex1 != FREQ_BAND5 && iIndex1 != FREQ_BAND6)
          {
          cstrItem1.Format("%.4e", m_adData.fAvgPwr[iDataIndex + iIndex1]);
          cstrItem += (cstrItem1 + ",");
          }
        }
        // Add the timeband start and end minutes.
      cstrItem += cstrGet_timeband_start_end_min(iIndex);
        // Add to spreadsheet file.
      iWriteSts = iWrite_to_ss(cstrItem, false);
      } // end of for each period.
    if(iWriteSts == SUCCESS)
      {
      if(iIndex < MIN_ROWS_DIST_AVG_PWR_FREQ_RANGE)
        { // Haven't finished adding the left side titles.
        while(iIndex < MIN_ROWS_DIST_AVG_PWR_FREQ_RANGE)
          {
          if((iWriteSts = iAdd_left_title(iColTitleIndex)) == FAIL)
            break;
          ++iColTitleIndex;
          ++iIndex;
          }
        }
      }
    }
    // Put in an empty line.
  if(iWriteSts == SUCCESS)
    {
    cstrItem.Empty();
    iWriteSts = iWrite_to_ss(cstrItem, true);
    }
  if(iWriteSts == SUCCESS)
    { // Average dominant frequency.
      // The average dominant frequency for each time band
      // falls in one frequency range.  The other frequency
      // ranges for the time band are blank.
    iDataIndex = 0;
    for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
      { // Go through this loop for each timeband specified.
      if(iIndex < MIN_ROWS_AVG_DOM_FREQ)
        { // Add the left side title.
        //cstrItem.LoadString(s_uLeftColTitleID[iColTitleIndex]);
        cstrItem = cstrGet_col_title(s_uLeftColTitleID[iColTitleIndex]);
        cstrItem += ",";
        ++iColTitleIndex;
        }
      else
        cstrItem = ",";
      uNumOfPoints = ((unsigned)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
                      - (unsigned)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 3);

      fStdDev = fStd_dev(uNumOfPoints, m_adData.pfDomFreq + iDataIndex, &fMean);
      iDataIndex += uNumOfPoints;
      cstrItem1.Format("%.2f (%.2f)", fMean, fStdDev);
#ifndef _200_CPM_HARDWARE
      if(fMean < H_FREQ_BAND2_LOW_CPM)
        iCol = 0;
      else if(fMean < H_FREQ_BAND3_LOW_CPM)
        iCol = 1;
      else if(fMean < H_FREQ_BAND4_LOW_CPM)
        iCol = 2;
      else
        iCol = 3;
#else
      float fFreqBandLowCPM[17];
      int iMaxIndex;
      fFreqBandLowCPM[0] = H_FREQ_BAND2_LOW_CPM;
      fFreqBandLowCPM[1] = H_FREQ_BAND3_LOW_CPM;
      fFreqBandLowCPM[2] = H_FREQ_BAND4_LOW_CPM;
      fFreqBandLowCPM[3] = H_FREQ_BAND5_LOW_CPM;
      fFreqBandLowCPM[4] = H_FREQ_BAND6_LOW_CPM;
      fFreqBandLowCPM[5] = H_FREQ_BAND7_LOW_CPM;
      fFreqBandLowCPM[6] = H_FREQ_BAND8_LOW_CPM;
      fFreqBandLowCPM[7] = H_FREQ_BAND9_LOW_CPM;
      fFreqBandLowCPM[8] = H_FREQ_BAND10_LOW_CPM;
      fFreqBandLowCPM[9] = H_FREQ_BAND11_LOW_CPM;
      fFreqBandLowCPM[10] = H_FREQ_BAND12_LOW_CPM;
      fFreqBandLowCPM[11] = H_FREQ_BAND13_LOW_CPM;
      fFreqBandLowCPM[12] = H_FREQ_BAND14_LOW_CPM;
      fFreqBandLowCPM[13] = H_FREQ_BAND15_LOW_CPM;
      fFreqBandLowCPM[14] = H_FREQ_BAND16_LOW_CPM;
      fFreqBandLowCPM[15] = H_FREQ_BAND17_LOW_CPM;
      fFreqBandLowCPM[16] = H_FREQ_BAND18_LOW_CPM;
      if(theApp.m_iFreqLimit == FREQ_LIMIT_15_CPM)
        iMaxIndex = 3;
      else if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
        iMaxIndex = 8;
      else
        iMaxIndex = 17;
      for(iCol = 0; iCol < iMaxIndex; ++iCol)
        {
        if(fMean < fFreqBandLowCPM[iCol])
          break;
        }
#endif
      for(iColIndex = 0; iColIndex < iCol; ++iColIndex)
        cstrItem += ",";
      cstrItem += cstrItem1;
      while(++iColIndex < (iFreqColumns + 2)) //(LAST_COLUMN - 1))
        cstrItem += ",";
        // Add the timeband start and end minutes.
      cstrItem += cstrGet_timeband_start_end_min(iIndex);
        // Add to spreadsheet file.
      iWriteSts = iWrite_to_ss(cstrItem, false);
      } // end of for each period.
    if(iWriteSts == SUCCESS)
      {
      if(iIndex < MIN_ROWS_AVG_DOM_FREQ)
        { // Haven't finished adding the left side titles.
        while(iIndex < MIN_ROWS_AVG_DOM_FREQ)
          {
          if((iWriteSts = iAdd_left_title(iColTitleIndex)) == FAIL)
            break;
          ++iColTitleIndex;
          ++iIndex;
          }
        }
      }
    }
    // Put in an empty line.
  if(iWriteSts == SUCCESS)
    {
    cstrItem.Empty();
    iWriteSts = iWrite_to_ss(cstrItem, true);
    }
  if(iWriteSts == SUCCESS)
    { // Percentage time with dominant frequency.
    for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
      { // Go through this loop for each timeband specified.
      if(iIndex < MIN_ROWS_PCT_TIME)
        { // Add the left side title.
        //cstrItem.LoadString(s_uLeftColTitleID[iColTitleIndex]);
        cstrItem = cstrGet_col_title(s_uLeftColTitleID[iColTitleIndex]);
        cstrItem += ",";
        ++iColTitleIndex;
        }
      else
        cstrItem = ",";
      uNumOfPoints = ((unsigned)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
                      - (unsigned)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 3);
      iDataIndex = iIndex * theApp.m_iMaxFreqBands;
      //iDataIndex = iIndex * MAX_FREQ_BANDS;
      //for(iIndex1 = FREQ_BAND1; iIndex1 <= FREQ_BAND4; ++iIndex1)
      for(iIndex1 = FREQ_BAND1; iIndex1 <= (iFreqColumns - 1); ++iIndex1)
        {
        fPct = *(m_adData.pfPctDomFreq + iDataIndex + iIndex1)
               / (float)uNumOfPoints;
        cstrItem1.Format("%.0f  %%", fPct * (float)100.0);
        cstrItem += (cstrItem1 + ",");
        }
        // Add the timeband start and end minutes.
      cstrItem += ("," + cstrGet_timeband_start_end_min(iIndex));
        // Add to spreadsheet file.
      iWriteSts = iWrite_to_ss(cstrItem, false);
      } // end of for each period.
    if(iWriteSts == SUCCESS)
      {
      if(iIndex < MIN_ROWS_PCT_TIME)
        { // Haven't finished adding the left side titles.
        while(iIndex < MIN_ROWS_PCT_TIME)
          {
          if((iWriteSts = iAdd_left_title(iColTitleIndex)) == FAIL)
            break;
          ++iColTitleIndex;
          ++iIndex;
          }
        }
      }
    }
  }

/********************************************************************
cstrGet_timeband_start_end_min - Get the start and end minute of the
                                 timeband (period).

  inputs: Timeband (period) number.

  return: CString object containing the start and end minute.
********************************************************************/
CString CSpreadSheet::cstrGet_timeband_start_end_min(int iIndex)
  {
  CString cstrMinutes;

  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard version
    cstrMinutes.Format("%.1f to %.1f", 
                       m_pDoc->m_pdPatient.fGoodMinStart[iIndex] + (iIndex * 10),
                       m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] + (iIndex * 10));
    }
  else
    { // Research version
    cstrMinutes.Format("%.1f to %.1f", m_pDoc->m_pdPatient.fGoodMinStart[iIndex],
                       m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]);
    }
  return(cstrMinutes);
  }

/********************************************************************
iAdd_left_title - Add a title text to the left most column of the
                  spreadsheet.  This will be the only text on the
                  spreadsheet row.

  inputs: Index into the s_uLeftColTitleID[] array.

  return: SUCCESS if the data was successfully written to the file.
          FAIL if a file write occurs.
********************************************************************/
short int CSpreadSheet::iAdd_left_title(int iColTitleIndex)
  {
  CString cstrTitle;
  short int iWriteSts;

  //cstrTitle.LoadString(s_uLeftColTitleID[iColTitleIndex]);
  cstrTitle = cstrGet_col_title(s_uLeftColTitleID[iColTitleIndex]);
  iWriteSts = iWrite_to_ss(cstrTitle, false);
  return(iWriteSts);
  }

/////////////////////////////////////////////////////////////////////
//// Used for creating a spreadsheet file (.xls)
#ifdef USE_SPREADSHEET_PROGRAM

/********************************************************************
iCreate_data_sheet - Create the data sheet as a spreadsheet.
                     The spreadsheet is already formatted with column
                     and row titles, fonts and text positioning within
                     cells.

  inputs: None.

  return: None.
********************************************************************/
void CSpreadSheet::vCreate_data_sheet()
  {
  CString cstrItem, cstrItem1, cstrCell;
  CRange range;
  short int iIndex, iIndex1, iDataIndex, iCol, iRow, iColTitleIndex; // iColIndex, 
  unsigned uNumOfPoints;
  float fStdDev, fMean, fPct;

    // Add Patient Name and date of study.
  if(m_pDoc->m_pdPatient.bUseResrchRefNum == TRUE)
    cstrItem = g_pLang->cstrLoad_string(ITEM_G_REF_NUM);
    //cstrItem.LoadString(IDS_REF_NUM);
  else
    cstrItem = g_pLang->cstrLoad_string(ITEM_G_PATIENT_NAME);
    //cstrItem.LoadString(IDS_PATIENT_NAME);
  cstrItem += ": ";
  vFormat_patient_name(&m_pDoc->m_pdPatient, cstrItem1, true);
  cstrItem += cstrItem1;
  cstrCell = "A1";
  range = m_Sheet.get_Range(COleVariant(cstrCell),COleVariant(cstrCell));
  range.put_Value2(COleVariant(cstrItem));

  //cstrItem1.LoadString(IDS_DATE_OF_STUDY);
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_DATE_OF_STUDY);
  cstrItem1 += ": ";
  cstrItem1 += m_pDoc->m_pdPatient.cstrDate;
  cstrCell = "A2";
  range = m_Sheet.get_Range(COleVariant(cstrCell),COleVariant(cstrCell));
  range.put_Value2(COleVariant(cstrItem1));

    // Distribution of average power by frequency region
  iRow = START_ROW;
  iCol = START_COLUMN;
  iColTitleIndex = 0;
  for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
    { // Go through this loop for each timeband specified.
    if(iIndex < MIN_ROWS_DIST_AVG_PWR_FREQ_REG)
      { // Add the left side title.
      vAdd_left_title(iColTitleIndex, iRow);
      ++iColTitleIndex;
      }
    iDataIndex = iIndex * MAX_FREQ_BANDS;
    for(iIndex1 = FREQ_BAND1; iIndex1 <= FREQ_BAND7; ++iIndex1)
      { // Go through this loop for each frequency band.
      if(iIndex1 != FREQ_BAND5 && iIndex1 != FREQ_BAND6)
        {
        cstrItem1.Format("%3.2f", m_adData.fPctGrp1[iDataIndex + iIndex1]);
          // Add to spreadsheet.
        cstrCell.Format("%c%d", (char)iCol, iRow);
        range = m_Sheet.get_Range(COleVariant(cstrCell),COleVariant(cstrCell));
        range.put_Value2(COleVariant(cstrItem1));
        ++iCol;
        }
      }
      // Add the timeband start and end minutes.
    cstrItem1 = cstrGet_timeband_start_end_min(iIndex);
      // Add to spreadsheet.
    cstrCell.Format("%c%d", (char)iCol, iRow);
    range = m_Sheet.get_Range(COleVariant(cstrCell),COleVariant(cstrCell));
    range.put_Value2(COleVariant(cstrItem1));
    ++iRow;
    iCol = START_COLUMN;
    }
  if(iIndex < MIN_ROWS_DIST_AVG_PWR_FREQ_REG)
    { // Haven't finished adding the left side titles.
    while(iIndex < MIN_ROWS_DIST_AVG_PWR_FREQ_REG)
      {
      vAdd_left_title(iColTitleIndex, iRow);
      ++iColTitleIndex;
      ++iIndex;
      ++iRow;
      }
    }

    // Ratio of average powers by frequency range.
  ++iRow; // Skip a row.
  for(iIndex = 1; iIndex < m_uMaxPeriods; ++iIndex)
    { // Go through this loop for each timeband specified.
    if((iIndex - 1) < MIN_ROWS_RATIO_AVG_PWR)
      { // Add the left side title.
      vAdd_left_title(iColTitleIndex, iRow);
      ++iColTitleIndex;
      }
    iDataIndex = iIndex * MAX_FREQ_BANDS;
    for(iIndex1 = FREQ_BAND1; iIndex1 <= FREQ_BAND7; ++iIndex1)
      { // Go through this loop for each frequency band.
      if(iIndex1 != FREQ_BAND5 && iIndex1 != FREQ_BAND6)
        {
        cstrItem1.Format("%.2f", *(m_adData.pfRatioAvgPwr + iDataIndex + iIndex1));
          // Add to spreadsheet.
        cstrCell.Format("%c%d", (char)iCol, iRow);
        range = m_Sheet.get_Range(COleVariant(cstrCell),COleVariant(cstrCell));
        range.put_Value2(COleVariant(cstrItem1));
        ++iCol;
        }
      }
      // Add the timeband start and end minutes.
    cstrItem1 = cstrGet_timeband_start_end_min(iIndex);
      // Add to spreadsheet.
    cstrCell.Format("%c%d", (char)iCol, iRow);
    range = m_Sheet.get_Range(COleVariant(cstrCell),COleVariant(cstrCell));
    range.put_Value2(COleVariant(cstrItem1));
    ++iRow;
    iCol = START_COLUMN;
    }
  if(--iIndex < MIN_ROWS_RATIO_AVG_PWR)
    { // Haven't finished adding the left side titles.
    while(iIndex < MIN_ROWS_RATIO_AVG_PWR)
      {
      vAdd_left_title(iColTitleIndex, iRow);
      ++iColTitleIndex;
      ++iIndex;
      ++iRow;
      }
    }

    // Distribution of average power by frequency range
  ++iRow; // Skip a row.
  for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
    { // Go through this loop for each timeband specified.
    if(iIndex < MIN_ROWS_DIST_AVG_PWR_FREQ_RANGE)
      { // Add the left side title.
      vAdd_left_title(iColTitleIndex, iRow);
      ++iColTitleIndex;
      }
    iDataIndex = iIndex * MAX_FREQ_BANDS;
    for(iIndex1 = FREQ_BAND1; iIndex1 <= FREQ_BAND7; ++iIndex1)
      { // Go through this loop for each frequency band.
      if(iIndex1 != FREQ_BAND5 && iIndex1 != FREQ_BAND6)
        {
        cstrItem1.Format("%.4e", m_adData.fAvgPwr[iDataIndex + iIndex1]);
          // Add to spreadsheet.
        cstrCell.Format("%c%d", (char)iCol, iRow);
        range = m_Sheet.get_Range(COleVariant(cstrCell),COleVariant(cstrCell));
        range.put_Value2(COleVariant(cstrItem1));
        ++iCol;
        }
      }
      // Add the timeband start and end minutes.
    cstrItem1 = cstrGet_timeband_start_end_min(iIndex);
      // Add to spreadsheet.
    cstrCell.Format("%c%d", (char)iCol, iRow);
    range = m_Sheet.get_Range(COleVariant(cstrCell),COleVariant(cstrCell));
    range.put_Value2(COleVariant(cstrItem1));
    ++iRow;
    iCol = START_COLUMN;
    }
  if(iIndex < MIN_ROWS_DIST_AVG_PWR_FREQ_RANGE)
    { // Haven't finished adding the left side titles.
    while(iIndex < MIN_ROWS_DIST_AVG_PWR_FREQ_RANGE)
      {
      vAdd_left_title(iColTitleIndex, iRow);
      ++iColTitleIndex;
      ++iIndex;
      ++iRow;
      }
    }

    // Average dominant frequency.
  ++iRow; // Skip a row.
    // The average dominant frequency for each time band
    // falls in one frequency range.  The other frequency
    // ranges for the time band are blank.
  iDataIndex = 0;
  for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
    { // Go through this loop for each timeband specified.
    if(iIndex < MIN_ROWS_AVG_DOM_FREQ)
      { // Add the left side title.
      vAdd_left_title(iColTitleIndex, iRow);
      ++iColTitleIndex;
      }
    uNumOfPoints = ((unsigned)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
                    - (unsigned)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 3);

    fStdDev = fStd_dev(uNumOfPoints, m_adData.pfDomFreq + iDataIndex, &fMean);
    iDataIndex += uNumOfPoints;
    cstrItem1.Format("%.2f (%.2f)", fMean, fStdDev);
    if(fMean < FREQ_BAND2_LOW_CPM)
      iCol = 0;
    else if(fMean < FREQ_BAND3_LOW_CPM)
      iCol = 1;
    else if(fMean < FREQ_BAND4_LOW_CPM)
      iCol = 2;
    else
      iCol = 3;
      // Add to spreadsheet.
    cstrCell.Format("%c%d", (char)(iCol + START_COLUMN), iRow);
    range = m_Sheet.get_Range(COleVariant(cstrCell),COleVariant(cstrCell));
    range.put_Value2(COleVariant(cstrItem1));
      // Add the timeband start and end minutes.
    cstrItem1 = cstrGet_timeband_start_end_min(iIndex);
      // Add to spreadsheet.
    cstrCell.Format("%c%d", (char)LAST_COLUMN, iRow);
    range = m_Sheet.get_Range(COleVariant(cstrCell),COleVariant(cstrCell));
    range.put_Value2(COleVariant(cstrItem1));
    ++iRow;
    }
  if(iIndex < MIN_ROWS_AVG_DOM_FREQ)
    { // Haven't finished adding the left side titles.
    while(iIndex < MIN_ROWS_AVG_DOM_FREQ)
      {
      vAdd_left_title(iColTitleIndex, iRow);
      ++iColTitleIndex;
      ++iIndex;
      ++iRow;
      }
    }

    // Percentage time with dominant frequency.
  iCol = START_COLUMN;
  ++iRow; // Skip a row.
  for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
    { // Go through this loop for each timeband specified.
    if(iIndex < MIN_ROWS_PCT_TIME)
      { // Add the left side title.
      vAdd_left_title(iColTitleIndex, iRow);
      ++iColTitleIndex;
      }
    uNumOfPoints = ((unsigned)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
                    - (unsigned)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 3);
    iDataIndex = iIndex * MAX_FREQ_BANDS;
    for(iIndex1 = FREQ_BAND1; iIndex1 <= FREQ_BAND4; ++iIndex1)
      {
      fPct = *(m_adData.pfPctDomFreq + iDataIndex + iIndex1)
             / (float)uNumOfPoints;
      cstrItem1.Format("%.0f  %%", fPct * (float)100.0);
        // Add to spreadsheet.
      cstrCell.Format("%c%d", (char)iCol, iRow);
      range = m_Sheet.get_Range(COleVariant(cstrCell),COleVariant(cstrCell));
      range.put_Value2(COleVariant(cstrItem1));
      ++iCol;
      }
      // Add the timeband start and end minutes.
    cstrItem1 = cstrGet_timeband_start_end_min(iIndex);
      // Add to spreadsheet.
    cstrCell.Format("%c%d", (char)LAST_COLUMN, iRow);
    range = m_Sheet.get_Range(COleVariant(cstrCell),COleVariant(cstrCell));
    range.put_Value2(COleVariant(cstrItem1));
    ++iRow;
    iCol = START_COLUMN;
    }
  if(iIndex < MIN_ROWS_PCT_TIME)
    { // Haven't finished adding the left side titles.
    while(iIndex < MIN_ROWS_PCT_TIME)
      {
      vAdd_left_title(iColTitleIndex, iRow);
      ++iColTitleIndex;
      ++iIndex;
      ++iRow;
      }
    }
  }

void CSpreadSheet::vAdd_left_title(int iColTitleIndex, int iRow)
  {
  CString cstrTitle, cstrCell;
  CRange range;

  //cstrTitle.LoadString(s_uLeftColTitleID[iColTitleIndex]);
  cstrTitle = cstrGet_col_title(s_uLeftColTitleID[iColTitleIndex]);
  cstrCell.Format("A%d", iRow);
  range = m_Sheet.get_Range(COleVariant(cstrCell),COleVariant(cstrCell));
  range.put_Value2(COleVariant(cstrTitle));
  }
#endif
/////////////////////////////////////////////////////////////////////

/********************************************************************
cstrGet_col_title - Get the row title text based on the resouce ID.

  inputs: Resource ID of the row title text.
  
  return: CString object containing the row title text.
********************************************************************/
CString CSpreadSheet::cstrGet_col_title(unsigned int uiID)
  {
  CString cstrTitle, cstrItem;

  switch(uiID)
    {
    case IDS_SS_LEFT_TITLE_1:
      cstrItem = ITEM_G_SS_LEFT_TITLE_1;
      break;
    case IDS_SS_LEFT_TITLE_2:
      cstrItem = ITEM_G_SS_LEFT_TITLE_2;
      break;
    case IDS_SS_LEFT_TITLE_3:
      cstrItem = ITEM_G_SS_LEFT_TITLE_3;
      break;
    case IDS_SS_LEFT_TITLE_4:
      cstrItem = ITEM_G_SS_LEFT_TITLE_4;
      break;
    case IDS_SS_LEFT_TITLE_5:
      cstrItem = ITEM_G_SS_LEFT_TITLE_5;
      break;
    case IDS_SS_LEFT_TITLE_6:
      cstrItem = ITEM_G_SS_LEFT_TITLE_6;
      break;
    case IDS_SS_LEFT_TITLE_7:
      cstrItem = ITEM_G_SS_LEFT_TITLE_7;
      break;
    case IDS_SS_LEFT_TITLE_8:
      cstrItem = ITEM_G_SS_LEFT_TITLE_8;
      break;
    case IDS_SS_LEFT_TITLE_9:
      cstrItem = ITEM_G_SS_LEFT_TITLE_9;
      break;
    case IDS_SS_LEFT_TITLE_10:
      cstrItem = ITEM_G_SS_LEFT_TITLE_10;
      break;
    case IDS_SS_LEFT_TITLE_11:
      cstrItem = ITEM_G_SS_LEFT_TITLE_11;
      break;
    default:
      cstrItem = "";
      break;
    }
  if(cstrItem != "")
    cstrTitle = g_pLang->cstrLoad_string(cstrItem);
  else
    cstrTitle = "";
  return(cstrTitle);
  }

