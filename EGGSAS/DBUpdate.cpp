/********************************************************************
DBUpdate.cpp

Copyright (C) 2019 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CDBUpdate class.

  This class updates an older version of a database
  to the lastest version.


HISTORY:
09-FEB-19  RET
             New
********************************************************************/
#include "stdafx.h"
#include "EGGSAS.h"
#include "dbaccessupd.h"
#include "dbupdate.h"

CDBUpdate::CDBUpdate(void)
  {
  }

CDBUpdate::~CDBUpdate(void)
  {
  }

/********************************************************************
iUpdate_database - Update the database.
                   This is the first version, so we only have to update
                     from the previous version and not worry about earlier
                     versions.

  inputs: Database to update.  Contains path and name.

  output: Updates database tables.

  return: SUCCESS if the database is updated.
          FAIL if there was an error.
********************************************************************/
int CDBUpdate::iUpdate_database(CString cstrUpdDB)
  {
  CString cstrVersion, cstrDBBackupFileName, cstrErr;
  short int iRet, iMajorVersion, iMinorVersion;
  bool bContinue = true;
  CDBAccessUpd *pDB;

  pDB = new CDBAccessUpd(cstrUpdDB);
  theApp.vLog("DB to update: %s", cstrUpdDB);
  if((iRet = pDB->iGet_version(cstrVersion)) == SUCCESS)
    {
    iMajorVersion = pDB->iGet_major_version(cstrVersion);
    iMinorVersion = pDB->iGet_minor_version(cstrVersion);
    if(iMajorVersion < DB_VERSION_MAJOR || iMinorVersion < DB_VERSION_MINOR)
      { // make a backup copy of the database.
      cstrDBBackupFileName = cstrUpdDB;
      cstrDBBackupFileName += ".bak";
      if(CopyFile(cstrUpdDB, cstrDBBackupFileName, FALSE) == FALSE)
        {
        cstrErr.Format("Error backing up database: %s", cstrGet_sys_err_msg());
        theApp.vLog(cstrErr);
        //if(MessageBox("You are about to update your database and a backup copy can't be made\nDo you want to continue without a backup copy?",
        //  "ERROR", MB_YESNO) == IDNO)
        //  bContinue = false;
        bContinue = true;
        }
      if(bContinue == true)
        { // Update the patient table.
        iRet = SUCCESS;
        if(iMajorVersion == 1 && iMinorVersion == 0)
          {
          iRet = FAIL;
          if((iRet = pDB->iUpdate_patient_100_101()) >= SUCCESS)
            {
            theApp.vLog("Patient table updated");
            if(iRet == SUCCESS_WITH_INFO)
              theApp.vLog(pDB->m_cstrInfo);
              // Update the study table
            if((iRet = pDB->iUpdate_study_100_101()) >= SUCCESS)
              {
              theApp.vLog("StudyData table updated");
              if(iRet == SUCCESS_WITH_INFO)
                theApp.vLog(pDB->m_cstrInfo);
              iRet = SUCCESS;
              iMajorVersion = 1;
              iMinorVersion = 1;
                // Update the version table.
              if((iRet = pDB->iUpdate_version()) >= SUCCESS)
                {
                theApp.vLog("Version updated");
                if(iRet == SUCCESS_WITH_INFO)
                  theApp.vLog(pDB->m_cstrInfo);
                iRet = SUCCESS;
                }
              else
                theApp.vLog(pDB->m_cstrErr); // Version table fail
              }
            else
              theApp.vLog(pDB->m_cstrErr); // StudyData table fail
            }
          else
            theApp.vLog(pDB->m_cstrErr); // Patient table fail
          }
        if(iRet == SUCCESS && iMajorVersion == 1 && iMinorVersion == 1)
          {
          if((iRet = pDB->iUpdate_study_101_200()) >= SUCCESS)
            {
            theApp.vLog("StudyData table updated");
            if(iRet == SUCCESS_WITH_INFO)
              theApp.vLog(pDB->m_cstrInfo);
              // Update the version table.
            if((iRet = pDB->iUpdate_version()) >= SUCCESS)
              {
              theApp.vLog("Version updated");
              if(iRet == SUCCESS_WITH_INFO)
                theApp.vLog(pDB->m_cstrInfo);
              iRet = SUCCESS;
              iMajorVersion = 2;
              iMinorVersion = 0;
              }
            }
          }
        if(iRet == SUCCESS && iMajorVersion == 2 && iMinorVersion == 0)
          {
          if((iRet = pDB->iAdd_event_table_201()) >= SUCCESS)
            {
            theApp.vLog("Events table added");
            if(iRet == SUCCESS_WITH_INFO)
              theApp.vLog(pDB->m_cstrInfo);
              // Update the version table.
            if((iRet = pDB->iUpdate_version()) >= SUCCESS)
              {
              theApp.vLog("Version updated");
              if(iRet == SUCCESS_WITH_INFO)
                theApp.vLog(pDB->m_cstrInfo);
              iRet = SUCCESS;
              iMajorVersion = 2;
              iMinorVersion = 1;
              }
            }
          }
        if(iRet == SUCCESS && iMajorVersion == 2 && iMinorVersion == 1)
          {
          if((iRet = pDB->iAdd_whatif_table_202()) >= SUCCESS)
            {
            theApp.vLog("WhatIf table added");
            if(iRet == SUCCESS_WITH_INFO)
              theApp.vLog(pDB->m_cstrInfo);
              // Update the study table.
            if((iRet = pDB->iUpdate_study_201_202()) >= SUCCESS)
              {
              theApp.vLog("Study table updated");
              if(iRet == SUCCESS_WITH_INFO)
                theApp.vLog(pDB->m_cstrInfo);
                // Update the version table.
              if((iRet = pDB->iUpdate_version()) >= SUCCESS)
                {
                theApp.vLog("Version updated");
                if(iRet == SUCCESS_WITH_INFO)
                  theApp.vLog(pDB->m_cstrInfo);
                iRet = SUCCESS;
                iMajorVersion = 2;
                iMinorVersion = 2;
                }
              }
            }
          }
        if(iRet == SUCCESS && iMajorVersion == 2 && iMinorVersion == 2)
          { // Move research reference numbers from patient info table
            // to patient table.
          if((iRet = pDB->iUpdate_patient_202_203()) >= SUCCESS)
            {
            theApp.vLog("PATIENT and PATIENTINFO tables updated.");
            if(iRet == SUCCESS_WITH_INFO)
              theApp.vLog(pDB->m_cstrInfo);
              // Update the version table.
            if((iRet = pDB->iUpdate_version()) >= SUCCESS)
              {
              theApp.vLog("Version updated");
              if(iRet == SUCCESS_WITH_INFO)
                theApp.vLog(pDB->m_cstrInfo);
              iRet = SUCCESS;
              iMajorVersion = 2;
              iMinorVersion = 3;
              }
            }
          }
        if(iRet == SUCCESS && iMajorVersion == 2 && iMinorVersion == 3)
          {
          if((iRet = pDB->iUpdate_patient_203_204()) >= SUCCESS)
            {
            theApp.vLog("DiagnosisDesc table created.");
            theApp.vLog("ICDCode table created.");
            theApp.vLog("Recommendations table created.");
            theApp.vLog("StudyData table updated.");
            theApp.vLog("Diagnosis key phrases and descriptions were added.");
            theApp.vLog("ICD codes were added.");
            if(iRet == SUCCESS_WITH_INFO)
              theApp.vLog(pDB->m_cstrInfo);
              // Update the version table.
            if((iRet = pDB->iUpdate_version()) >= SUCCESS)
              {
              theApp.vLog("Version updated");
              if(iRet == SUCCESS_WITH_INFO)
                theApp.vLog(pDB->m_cstrInfo);
              iRet = SUCCESS;
              iMajorVersion = 2;
              iMinorVersion = 4;
              }
            }
          }
        if(iRet == SUCCESS && iMajorVersion == 2 && iMinorVersion == 4)
          {
          if((iRet = pDB->iUpdate_patient_204_205()) >= SUCCESS)
            {
            theApp.vLog("PATIENT and PATIENTINFO tables updated.");
            if(iRet == SUCCESS_WITH_INFO)
              theApp.vLog(pDB->m_cstrInfo);
              // Update the version table.
            if((iRet = pDB->iUpdate_version()) >= SUCCESS)
              {
              theApp.vLog("Version updated");
              if(iRet == SUCCESS_WITH_INFO)
                theApp.vLog(pDB->m_cstrInfo);
              iRet = SUCCESS;
              iMajorVersion = 2;
              iMinorVersion = 5;
              }
            }
          }

        if(iRet == SUCCESS && iMajorVersion == 2 && iMinorVersion == 5)
          {
          if((iRet = pDB->iUpdate_patient_205_206()) >= SUCCESS)
            {
            theApp.vLog("StudyData and WhatIf tables updated.");
            if(iRet == SUCCESS_WITH_INFO)
              theApp.vLog(pDB->m_cstrInfo);
              // Update the version table.
            if((iRet = pDB->iUpdate_version()) >= SUCCESS)
              {
              theApp.vLog("Version updated");
              if(iRet == SUCCESS_WITH_INFO)
                theApp.vLog(pDB->m_cstrInfo);
              iRet = SUCCESS;
              iMajorVersion = 2;
              iMinorVersion = 6;
              }
            }
          }

        if(iRet == FAIL)
          { // Failed in updating the database, restore the backup version.
          theApp.vLog("Database update failed, original database restored");
          if(CopyFile(cstrDBBackupFileName, cstrUpdDB, FALSE) == FALSE)
            {
            cstrErr.Format("Error restoring database: %s", cstrGet_sys_err_msg());
            theApp.vLog(cstrErr);
            }
          else
            DeleteFile(cstrDBBackupFileName); // Delete the backup up copy.
          }
        }
      theApp.vLog("*** Finished updating database ***");
      }
    else
      {
      theApp.vLog("Database is up to date.");
      iRet = SUCCESS;
      }
    }

  //pDB->sqlrDB_disconnect();
  delete pDB;
  return iRet;
  }

  /********************************************************************
cstrGet_sys_err_msg - Get the system error message for the error that
                      just occurred.

  inputs: None.

  output: None.

  return: CString object containing the system error message.
********************************************************************/
CString CDBUpdate::cstrGet_sys_err_msg(void)
  {
  CString cstrSysErrMsg;
  char szMsg[256];

  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0L, szMsg, 256, NULL);
  cstrSysErrMsg = szMsg;
  return(cstrSysErrMsg);
  }
