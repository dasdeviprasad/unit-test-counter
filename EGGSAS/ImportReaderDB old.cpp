/********************************************************************
ImportReaderDB.cpp

Copyright (C) 2008, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CImportReaderDB class.

  This class provides functionality for importing a reader database into
  the local database.


HISTORY:
12-NOV-08  RET  New.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "Dbaccess.h"

#include "ImportReaderDB.h"

CImportReaderDB::CImportReaderDB(CEGGSASDoc *pDoc)
  {

  m_pDoc = pDoc;
  }

CImportReaderDB::~CImportReaderDB(void)
  {
  }

void CImportReaderDB::vImport_reader_db()
  {
  CDbaccess *pDB;
  CString cstrDBPath, cstrCurrentDBPath;

    // Display File Open dialog box to select the reader database.
  cstrDBPath = m_pDoc->cstrSelect_reader_db();
  if(cstrDBPath.IsEmpty() == FALSE)
    { // User selected a reader database, import it.
      // Find out what database the program's database access object is using and
      // save it.
    cstrCurrentDBPath = m_pDoc->m_pDB->cstrGet_db_file_path();
      // Set the program's database access object to the local database and
      // create a new databaes access object connected to the reader database.
    if(cstrCurrentDBPath != DSN_DEFAULT)
      m_pDoc->vChange_db(LOCAL_DB, "");
    pDB = new CDbaccess(cstrDBPath);
      // Connected to both databases.
      // Import the reader database.
    iImport_db(m_pDoc->m_pDB, pDB);
      // Restore the program's database access object.
    if(cstrCurrentDBPath != DSN_DEFAULT)
      m_pDoc->vChange_db(LOCAL_DB, cstrCurrentDBPath);
    delete pDB;
    }
  }

/********************************************************************
iImport_db - Import a database into the local database.

               For each patient
                 Import the patient table and if the patient record
                   already exist, as to overwrite it.
                 If new patient or overwritten patient, import the
                   patient info table.
                 For each study
                   Get the data file name from the remote DB.
                   If data data file already exists locally, as to
                     overwrite it.
                   If new data file, or overwriting data file
                     Import study data and automatically overwrite if it
                       already exists.
                     Delete all WhatIf scenarios for the study and import
                       the ones from the remote database.
                     Delete all Event records for the study and import
                       the ones from the remote database.


  inputs: Pointer to a database object for the local database.
          Pointer to Dialoga database object for the database to be imported.

  return: SUCCESS if the import is successful.
          FAIL if there is an error.
********************************************************************/
int CImportReaderDB::iImport_db(CDbaccess *pDBLocal, CDbaccess *pDBRemote)
  {
  int iRet = SUCCESS;
  int iPatientPos, iStudyPos, iSts;
  short int iFCSts;
  FIND_PATIENT_INFO fpiData;
  PATIENT_DATA PatientData;
  PATIENT_INFO piInfo;
  FIND_NEXT_STUDY fns;
  STUDY_DATA_RECORD StudyData;
  long lPatientIDLocal, lPatientIDRemote, lStudyIDLocal, lStudyIDRemote;
  bool bOverwrite;
  CString cstrTemp, cstrTemp1, cstrTitle, cstrDest, cstrSrc;

    // For each patient in remote db
  iPatientPos = FIRST_RECORD;
  while(pDBRemote->iFind_next_patient(iPatientPos, &fpiData) == SUCCESS)
    {
    lPatientIDRemote = fpiData.lPatientID;
    bOverwrite = true;
    iSts = SUCCESS;
       // Save patient in local db (if already exists, ask to overwrite)
    if(pDBLocal->iFind_patient_name(&fpiData) == SUCCESS)
      { // Patient already exists in the local database, ask to overwite.
      lPatientIDLocal = fpiData.lPatientID;
      if(fpiData.bUseResrchRefNum == TRUE)
        cstrTemp.Format(IDS_OVERWRITE_PATIENT, fpiData.cstrResearchRefNum);
      else
        {
        cstrTemp1.Format("%s %s", fpiData.cstrFirstName, fpiData.cstrLastName);
        cstrTemp.Format(IDS_OVERWRITE_PATIENT, cstrTemp1);
        }
      cstrTitle.LoadString(IDS_WARNING);
      if(AfxGetApp()->m_pActiveWnd->MessageBox(cstrTemp, cstrTitle, MB_YESNO) == IDNO)
        bOverwrite = false;
      }
    if(bOverwrite == true)
      { // Either new patient or its OK to overwrite existing patient.
      PatientData.cstrFirstName = fpiData.cstrFirstName;
      PatientData.cstrLastName = fpiData.cstrLastName;
      PatientData.cstrMI = fpiData.cstrMI;
      PatientData.bUseResrchRefNum = fpiData.bUseResrchRefNum;
      PatientData.cstrResearchRefNum = fpiData.cstrResearchRefNum;
      PatientData.lPatientID = fpiData.lPatientID;
      if((iSts = pDBLocal->iSave_patient(&PatientData)) == SUCCESS)
        {  // Save patient info if overwrite or new patient
        if((iSts = pDBRemote->iGet_patient_info(&piInfo)) == SUCCESS)
          {
          piInfo.lPatientID = lPatientIDLocal;
          iSts = pDBLocal->iSave_patient_info(&piInfo);
          }
        }
      }
    if(iSts == SUCCESS)
      { // Import all studies for this patient.
      iStudyPos = FIRST_RECORD;
      fns.lPatientID = lPatientIDRemote;
      while(pDBRemote->iFind_next_study(iStudyPos, &fns) == SUCCESS)
        {
        lStudyIDRemote = fns.lStudyID;
        StudyData.lStudyID = lStudyIDRemote;
        StudyData.cstrDateOfStudy.Empty();
        if(pDBRemote->iGet_study_data(&StudyData) == SUCCESS)
          { // Copy data file
          cstrTemp.Format(IDS_OVERWRITE_STUDY, fpiData.cstrFirstName,
                          fpiData.cstrLastName, StudyData.cstrDateOfStudy);
            // If data file already exists, ask to overwrite.
            // If overwrite OK or new study, copy file and save study.
          cstrDest.Format("%s\\%s", pDBLocal->cstrGet_db_path(), StudyData.cstrDataFile);
          cstrSrc.Format("%s\\%s", pDBRemote->cstrGet_db_path(), StudyData.cstrDataFile);
          iFCSts = iCopy_file(cstrSrc, cstrDest, cstrTemp);
          if(iFCSts == FILE_COPY_NEW || iFCSts == FILE_COPY_OVERWRITTEN)
            { // File successfully copied, either new or overwritten.
              // Import the study and overwrite if it already exists.
            if(iFCSts == FILE_COPY_OVERWRITTEN)
              { // Data file already exists, look for the study.
              lStudyIDLocal = pDBLocal->lFind_study(StudyData.cstrDataFile);
              StudyData.lStudyID = lStudyIDLocal;
              }
            else
              StudyData.lStudyID = INV_LONG; // New study.
              // Save the study.
            if(pDBLocal->iSave_study_data(&StudyData) == SUCCESS)
              { // Import the WhatIf records for this study.
                // If existing study not overwritten, DON'T IMPORT WHATIF RECORDS.
              if(bOverwrite == true)
                { // Either overwritten existing study, or a new study.
                iImport_whatif(lStudyIDLocal, lStudyIDRemote, pDBLocal, pDBRemote, iFCSts);
                iImport_events(lStudyIDLocal, lStudyIDRemote, pDBLocal, pDBRemote, iFCSts);
                }
              }
            }
          }
        iStudyPos = NEXT_RECORD;
        }
      }
    iPatientPos = NEXT_RECORD;
    }
  return(iRet);
  }

/********************************************************************
iCopy_file - Copy a file from the source to the destination.

  inputs: String containing the source file and path.
          String containing the destination file and path.

  return: TRUE if the file was successfully copied.
          FALSE if there was an error or the file was not overwritten.
********************************************************************/
short int CImportReaderDB::iCopy_file(LPCTSTR strSrc, LPCTSTR strDest,
CString cstrOverwriteInfo)
  {
  short int iRet = FILE_COPY_FAIL;

    // Fail if already exists.  make user decide to overwrite.
  if(CopyFile(strSrc, strDest, TRUE) == FALSE)
    { // Failed copying file.
    DWORD dwErr;
    char szMsg[256];
    CString cstrErr, cstrTitle;
  
    dwErr = GetLastError();
    if(dwErr == ERROR_FILE_EXISTS)
      { // File already exists, ask to overwrite.
      if(cstrOverwriteInfo.IsEmpty() == TRUE)
        cstrErr.Format(IDS_OVERWRITE_FILE, strDest);
      else
        cstrErr = cstrOverwriteInfo;
      cstrTitle.LoadString(IDS_ERROR);
      if(AfxGetApp()->m_pActiveWnd->MessageBox(cstrErr, cstrTitle, MB_YESNO) == IDYES)
        {
        if(CopyFile(strSrc, strDest, FALSE) == FALSE)
          dwErr = GetLastError();
        else
          iRet = FILE_COPY_OVERWRITTEN;
        }
      else
        iRet = FILE_COPY_NOT; // Not overwriting file.
      }
    if(dwErr != ERROR_FILE_EXISTS)
      { // File copy error.
      iRet = FILE_COPY_FAIL;
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, 0L, szMsg, 256, NULL);
      cstrErr.Format(IDS_FILE_COPY_ERR, strSrc, strDest);
      cstrErr += "\r\n";
      cstrErr += szMsg;
      cstrTitle.LoadString(IDS_ERROR);
      AfxGetApp()->m_pActiveWnd->MessageBox(cstrErr, cstrTitle);
      theApp.vLog((LPCTSTR)cstrErr);
      }
    }
  return(iRet);
  }

short int CImportReaderDB::iImport_whatif(long lStudyIDLocal, long lStudyIDRemote,
CDbaccess *pDBLocal, CDbaccess *pDBRemote, short int iStudyType)
  {
  short int iPos, iRet = SUCCESS;
  WHAT_IF_RECORD wi;

    // If overwritten study, delete all WhatIf records for the study.
  if(iStudyType == FILE_COPY_OVERWRITTEN)
    { // Existing study overwritten, delete all WhatIf records.
    iRet = pDBLocal->iDelete_whatif_for_study(lStudyIDLocal);
    }
  if(iRet == SUCCESS)
    { // If new study or overwritten study, import all WhatIf records.
    iPos = FIRST_RECORD;
    wi.lStudyID = lStudyIDRemote;
    while(pDBRemote->iFind_next_whatif(iPos, &wi) == SUCCESS)
      {
      wi.lWhatIfID = INV_LONG;
      wi.lStudyID = lStudyIDLocal;
      if((iRet = pDBLocal->iSave_whatif_data(&wi)) == FAIL)
        break;
      iPos = NEXT_RECORD;
      }
    }

  return(iRet);
  }

short int CImportReaderDB::iImport_events(long lStudyIDLocal, long lStudyIDRemote,
CDbaccess *pDBLocal, CDbaccess *pDBRemote, short int iStudyType)
  {
  short int iRet = SUCCESS;
  EVENT_RECORD Event;

    // If overwritten study, delete all WhatIf records for the study.
  if(iStudyType == FILE_COPY_OVERWRITTEN)
    { // Existing study overwritten, delete all WhatIf records.
    iRet = pDBLocal->iDelete_events(lStudyIDLocal);
    }
  if(iRet == SUCCESS)
    { // If new study or overwritten study, import all WhatIf records.
    Event.lStudyID = lStudyIDRemote;
    while(pDBRemote->iGet_event(&Event) == SUCCESS)
      {
      Event.lStudyID = lStudyIDLocal;
      if((iRet = pDBLocal->iSave_event(&Event)) == FAIL)
        break;
      Event.lStudyID = INV_LONG;
      }
    }

  return(iRet);
  }
