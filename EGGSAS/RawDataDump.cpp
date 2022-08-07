/********************************************************************
RawDataDump.cpp

Copyright (C) 2011,2012,2013 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CRawDataDump class.

  This class provides the data and methods for saving the raw data from a study
  to a comma delimited file in ASCII.  This file is suitable to be read in
  a spreadsheet.


HISTORY:
27-SEP-11  RET  New.
21-FEB-12  RET  Version 
                  Changes for foreign languages.
20-FEB-13  RET  Version 
             Remove text between sections and replace with blank lines.
               Change method: vSave_ascii_data_file()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "RawDataDump.h"
#include "util.h"

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the active document.
********************************************************************/
CRawDataDump::CRawDataDump(CEGGSASDoc *pDoc)
  {

  m_pDoc = pDoc;
  }

/********************************************************************
Destructor

  Clean up class data.
********************************************************************/
CRawDataDump::~CRawDataDump(void)
  {
  }


/********************************************************************
vSave_ascii_data_file - Save the raw data in an ASCII file.
                        See top of file for a description of the file format.

  inputs: None.

  return: None.
********************************************************************/
void CRawDataDump::vSave_ascii_data_file()
  {
  CString cstrFile, cstrLine;
  CFile *pcfFile = NULL;
  int iSts, iCnt, iNumStudyPeriods;

  // Create and open the ASCII file.
  cstrFile = m_pDoc->cstrCreate_data_file_name(&m_pDoc->m_pdPatient, FILE_TYPE_RAW_DATA);
  cstrFile = cstrSelect_raw_data_file_location(cstrFile);
  if(cstrFile.IsEmpty() == false)
    {
    try
      {
      pcfFile = new CFile(cstrFile,
                          CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite);
      }
    catch(CFileException *pfe)
      { // Got a file creation error.
      CString cstrMsg, cstrErrTitle;
      //cstrMsg.Format(IDS_ERR_CREATE_FILE, cstrFile, strerror(pfe->m_lOsError));
      cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_CREATE_FILE), cstrFile, strerror(pfe->m_lOsError));
      theApp.vLog(cstrMsg);
      //cstrErrTitle.LoadString(IDS_ERROR1);
      cstrErrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
      AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErrTitle, MB_OK | MB_ICONSTOP);
      pfe->Delete();
      if(pcfFile != NULL)
        {
        pcfFile->Close(); // Close the file.
        delete pcfFile;
        pcfFile = NULL;
        }
      }
    if(pcfFile != NULL)
      {
      // Figure out the number of periods of data.
      if(theApp.m_ptStudyType == PT_STANDARD)
        iNumStudyPeriods = 3;
      else
        iNumStudyPeriods = m_pDoc->m_pdPatient.uNumPeriods;
      // Write "BaseLine EGG"
//      if(iWrite_to_ascii_data_file(pcfFile, "BaseLine EGG") == SUCCESS)
      if(iWrite_to_ascii_data_file(pcfFile, "") == SUCCESS)
        {
        //// Save EGG data
        // Open the EGG data file, go past header and read the baseline data.
        if(m_pDoc->iRead_period(0, m_pDoc->m_fData) == SUCCESS)
          {
          // Read every other float (EGG only) for 256 points
          //   convert to ASCII and write to ASCII file.
          iSts = iWrite_to_ascii_data(pcfFile, m_pDoc->m_fData,
            m_pDoc->m_uDataPointsInPeriod, true);
          }
        }
      if(iSts == SUCCESS)
        {
          // Go to next section of EGG data file.
          // Write "Study EGG"
//        if(iWrite_to_ascii_data_file(pcfFile, "Study EGG") == SUCCESS)
        if(iWrite_to_ascii_data_file(pcfFile, "") == SUCCESS)
          {
          // Add each period of study data.
          for(iCnt = 1; iCnt <= iNumStudyPeriods; ++iCnt)
            {
            if(m_pDoc->iRead_period(iCnt, m_pDoc->m_fData) == SUCCESS)
              {
              // Read every other float (EGG only) for 256 points
              //   convert to ASCII and write to ASCII file.
              iSts = iWrite_to_ascii_data(pcfFile, m_pDoc->m_fData,
                m_pDoc->m_uDataPointsInPeriod, true);
              }
            else
              break;
            }
          }
        }

      ////// Save Respiration data
      // Write "BaseLine Respiration"
//      if(iWrite_to_ascii_data_file(pcfFile, "BaseLine Respiration") == SUCCESS)
      if(iWrite_to_ascii_data_file(pcfFile, "") == SUCCESS)
        {
        // Open the EGG data file, go past header and read the baseline data.
        if(m_pDoc->iRead_period(0, m_pDoc->m_fData) == SUCCESS)
          {
          // Start at second point and read every other float (Respiration only)
          //   for 256 points, convert to ASCII and write to ASCII file.
          iSts = iWrite_to_ascii_data(pcfFile, m_pDoc->m_fData,
            m_pDoc->m_uDataPointsInPeriod, false);
          }
        }
      if(iSts == SUCCESS)
        {
          // Go to next section of EGG data file.
          // Write "Study Respiration"
//        if(iWrite_to_ascii_data_file(pcfFile, "Study Respiration") == SUCCESS)
        if(iWrite_to_ascii_data_file(pcfFile, "") == SUCCESS)
          {
          // Add each period of study data.
          for(iCnt = 1; iCnt <= iNumStudyPeriods; ++iCnt)
            {
            if(m_pDoc->iRead_period(iCnt, m_pDoc->m_fData) == SUCCESS)
              {
              // Start at second point and read every other float (Respiration only)
              //   for 256 points convert to ASCII and write to ASCII file.
              iSts = iWrite_to_ascii_data(pcfFile, m_pDoc->m_fData, 
                m_pDoc->m_uDataPointsInPeriod, false);
              }
            else
              break;
            }
          }
        }
      pcfFile->Close(); // Close the ASCII data file.
      delete pcfFile;
      }
    }
  }

/********************************************************************
iWrite_to_ascii_data_file - Write a line of text to the file.
                         
  inputs: CFile pointer to the open file.
          CString containing the line of text.  The line does not contain
            a carriage return or line feed.

  return: SUCCESS if the data was successfully written to the file.
          FAIL if a file write occurs.
********************************************************************/
short int CRawDataDump::iWrite_to_ascii_data_file(CFile *pcfFile, CString cstrLine)
  {
  short int iRet;
  unsigned char ucCRLF[2] = {0xD, 0xA};

  try
    {
    if(cstrLine.IsEmpty() == false)
      {
      pcfFile->Write((void *)(LPCTSTR)cstrLine, cstrLine.GetLength());
      pcfFile->Write((void *)ucCRLF, sizeof(ucCRLF));
      }
    else
      pcfFile->Write((void *)ucCRLF, sizeof(ucCRLF)); // Add empty line.
    iRet = SUCCESS;
    }
  catch(CFileException *pfe)
    { // Got an error while writing to the file.  Error is displayed by caller.
    CString cstrMsg, cstrErrTitle;

    //cstrMsg.Format(IDS_ERR_WRITE_FILE, pcfFile->GetFilePath(), strerror(pfe->m_lOsError));
    cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_WRITE_FILE), pcfFile->GetFilePath(), strerror(pfe->m_lOsError));
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
iWrite_to_ascii_data - Write a period of data to the ASCII data file.
                       There are one half the number of data points to
                       write as specified in the third input.  That's because
                       the data buffer alternates between EGG and Resipiration
                       data. An EGG data is first, followed by a Resipiration
                       data point.
                         
  inputs: CFile pointer to the open file.
          Pointer to floating point data buffer containing data to write.
          Number of data points in buffer.
          true to write EGG data from buffer, false to write respiration
            data from buffer.

  return: SUCCESS if the data was successfully written to the file.
          FAIL if a file write occurs.
********************************************************************/
short int CRawDataDump::iWrite_to_ascii_data(CFile *pcfFile, float *pfData,
                                                unsigned uPoints, bool bEGG)
  {
  short int iRet = SUCCESS;
  unsigned uTotalCnt, uMinCnt;
  CString cstrLine, cstrPoint;
  bool bComma;

  if(bEGG == true)
    uTotalCnt = 0;
  else
    uTotalCnt = 1;
  while(uTotalCnt < uPoints && iRet == SUCCESS)
    {
    cstrLine = "";
    bComma = false;
    for(uMinCnt = 0; uMinCnt < DATA_POINTS_PER_MIN_1 && uTotalCnt < uPoints;
      ++uMinCnt, uTotalCnt += 2)
      {
      if(bComma == false)
        {
        cstrPoint.Format("%.4f", *(pfData + uTotalCnt));
        bComma = true;
        }
      else
        cstrPoint.Format(",%.4f", *(pfData + uTotalCnt));
      cstrLine += cstrPoint;
      }
    iRet = iWrite_to_ascii_data_file(pcfFile, cstrLine);
    }

  return(iRet);
  }

/********************************************************************
cstrSelect_raw_data_file_location - Ask user where to put the file.  The
                                    user may also change the file name.
                         
  inputs: CString containing the file name without any path.

  return: If user selects OK from the SaveAs dialog box:
            A CString object containing the complete path and file.
          If user selects Cancel from the SaveAs dialog box:
            Empty CString.
********************************************************************/
CString CRawDataDump::cstrSelect_raw_data_file_location(CString cstrFile)
  {
  CString cstrPathFile;

  while(bShow_save_as_dlgbox(cstrFile, cstrPathFile) == false)
    ;
  return(cstrPathFile);
  }

/********************************************************************
bShow_save_as_dlgbox - Display the SaveAs dialog box.
                       If file already exists, ask to overwrite.
                         
  inputs: CString containing the file name without any path.
          CString containing the path and file name selected in the SaveAs
            dialog box.

  return: true if user either selected OK or Cancel.
          false if there was an error, such as no file name that the
            SaveAs dialog box didn't catch.
********************************************************************/
bool CRawDataDump::bShow_save_as_dlgbox(CString cstrFile, CString &cstrSelFile)
  {
  CFileDialog *pdlg;
  CString cstrPath, cstrMsg, cstrErrTitle, cstrOrigFile;
  bool bRet = true;

   // Create an open file dialog box. Operator can select only a single file.
   // List the spreadsheet files, default extension is ".csv" and
   // prompt the user to overwrite if file exists.
  pdlg = new CFileDialog(FALSE, ".csv", cstrFile, 
                         OFN_OVERWRITEPROMPT, 
                         "Raw data files (*.csv)|All Files (*.*)|*.*||", NULL);
  if(pdlg->DoModal() == IDOK)
    {
    cstrSelFile = pdlg->GetPathName(); // User clicked OK, get the full path and file name.
    if(cstrGet_file_name(cstrSelFile).IsEmpty() == true)
      { // There is no file name.
      bRet = false;
      //cstrMsg.LoadString(IDS_ENTER_FILE_NAME);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_ENTER_FILE_NAME);
      //cstrErrTitle.LoadString(IDS_ERROR1);
      cstrErrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
      AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErrTitle, MB_OK | MB_ICONSTOP);
      }
    }
  else
    {
    cstrSelFile.Empty(); // User cancelled from SaveAs dialog box.
    }
  delete pdlg;
  return(bRet);
  }
