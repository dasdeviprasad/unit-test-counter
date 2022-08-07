/********************************************************************
DBAccess.cpp

Copyright (C) 2009 - 2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CDBAccess class.

  This class provides access to the Microsoft Access database via
  SQL and an ODBC driver.


HISTORY:
03-NOV-08  RET  New.
Version 1.03
  05-FEB-09  RET  Change iFind_next_study() to get the data file name.
09-JUN-16 RET Versin 1.06
            Update database access methods for the most current database version, 2.06.
              Change methods: iSave_patient(), iGet_next_patient(), iSave_patient_info(),
                iSave_study_data(), iSave_whatif_data()
              Change defines: SQL_PATIENT_TABLE_ORDER, SQL_INSERT_PATIENT,
                SQL_INSERT_PATIENT_INFO, SQL_INSERT_STUDY_DATA, SQL_WHATIF_INSERT_RECORD
            Fix bug in getting next WhatIf record.  Using wrong statement handle on subsequent
            calls to get next record.
              Change method: iFind_next_whatif()
20-JAN-18  RET
             Provide search criteria for listing patients.
               Change methods: iFind_next_patient(), iFind_next_study()
               Add methods: bLook_for_study_date(), ctMake(), vParse_date()
               Add variables: m_cstrSearchSQL, m_iDateSrchType, m_ctSrchDate,
                 m_cstrSrchDate
               Add defines: DB_SRCH_NONE, DB_SRCH_GREATER_THAN, DB_SRCH_LESS_THAN,
                 DB_SRCH_EQUAL_TO
19-MAY-18  RET Version 1.09
             Add database search button to main screen.  This allows the user
						 to select studies to export based on names/research reference numbers
						 and/or dates.  Both allows use of wild cards.
						   Change methods: iFind_next_study()
							 Add methods: bLook_for_study_date(), ctMake(), vParse_date()
27-NOV-20  RET
            Changes to allow using a remote database.
              Change method: sqlrDB_connect()
********************************************************************/

#include "StdAfx.h"
#include "StudyExport.h"

#include "DBAccess.h"

//// SQL statements for Patient table.
#define SQL_FIND_PATIENT \
_T("Select PatientID from Patient where LastName='%s' AND FirstName='%s' \
AND MiddleInitial='%s' AND ResearchRefNum='%s'")

#define SQL_PATIENT_TABLE_ORDER \
_T("Select PatientID,LastName,FirstName,MiddleInitial,UseResrchRefNum,ResearchRefNum,Hide \
from Patient ORDER by LastName,FirstName,MiddleInitial,ResearchRefNum ASC")

#define SQL_GET_PATIENT \
_T("SELECT * from Patient where PatientID=%lu")

#define SQL_INSERT_PATIENT \
_T("INSERT INTO Patient (LastName,FirstName,MiddleInitial,UseResrchRefNum,ResearchRefNum,Hide) \
VALUES ('%s','%s','%s',%d,'%s',%d)")

//// SQL statements for PatientInfo table.
#define SQL_INSERT_PATIENT_INFO \
_T("INSERT INTO PatientInfo (PatientID,Address1,Address2,City,State,ZipCode,Phone,DateOfBirth,\
HospitalNumber,SocialSecurityNum,Male,Female) \
VALUES (%lu,'%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,%d)")

#define SQL_PATIENT_TABLE_INFO \
_T("SELECT * from PatientInfo where PatientID=%lu")

//// SQL statements for StudyData table.
#define SQL_GET_STUDY_DATA_FOR_ID \
_T("SELECT * from StudyData where StudyID=%lu")

#define SQL_INSERT_STUDY_DATA \
_T("INSERT INTO StudyData (PatientID,DataFile,DateOfStudy,MealTime,WaterAmount,\
WaterAmountUnits,PeriodStart,PeriodLen,StartMinutes,EndMinutes,TestMaxSignal,\
TestMinSignal,Extra10Min,SuggestedDiagnosis,Bloating,EarlySateity,Eructus,Gerd,\
Nausea,PostPrandialFullness,Vomiting,AttendingPhysician,ReferingPhysician,\
Technician,ManualMode,RespirationRate,UserDiagnosis,IFPComments,Diagnosis,\
Recommendations,Medications,DelayedGastricEmptying,BLDataPoints,Pause,\
PostStimDataPoints,StimulationMedium,DiagKeyPhrase,ICDCode,RecommendationKeyPhrase,\
Gastroparesis,DyspepsiaBloatSatiety,Dyspepsia,Anorexia,\
RefractoryGERD,WeightLoss,PostDilationEval,EvalEffectMed,Frequency) \
VALUES (%lu,'%s','%s','%s',%d,'%s','%s','%s','%s','%s',%d,%d,%d,'%s',%d,%d,%d,%d,%d,%d,%d,\
'%s','%s','%s',%d,'%s',%d,'%s','%s','%s','%s',%d,%lu,%d,%lu,'%s','%s','%s','%s',\
%d,%d,%d,%d,%d,%d,%d,%d,%d)")

//_T("INSERT INTO StudyData (PatientID,DataFile,DateOfStudy,MealTime,WaterAmount,\
//WaterAmountUnits,PeriodStart,PeriodLen,StartMinutes,EndMinutes,TestMaxSignal,\
//TestMinSignal,Extra10Min,SuggestedDiagnosis,Bloating,EarlySateity,Eructus,Gerd,\
//Nausea,PostPrandialFullness,Vomiting,AttendingPhysician,ReferingPhysician,\
//Technician,ManualMode,RespirationRate,UserDiagnosis,IFPComments,Diagnosis,\
//Recommendations,Medications,DelayedGastricEmptying,BLDataPoints,Pause,\
//PostStimDataPoints,StimulationMedium) \
//VALUES (%lu,'%s','%s','%s',%d,'%s','%s','%s','%s','%s',%d,%d,%d,'%s',%d,%d,%d,%d,%d,%d,%d,\
//'%s','%s','%s',%d,'%s',%d,'%s','%s','%s','%s',%d,%lu,%d,%lu,'%s')")

//// SQL statements for WhatIf table.
#define SQL_WHATIF_GET_RECORD \
_T("SELECT * from WhatIf where WhatIfID=%lu")

#define SQL_WHATIF_GET_DESC_ID \
_T("SELECT * from WhatIf where StudyID=%lu")

#define SQL_WHATIF_INSERT_RECORD \
_T("INSERT INTO WhatIf (StudyID,Description,DateTimeSaved,NumPeriods,PeriodStart,PeriodLen,\
StartMinutes,EndMinutes,RespirationRate,Frequency) \
VALUES (%lu,'%s','%s',%d,'%s','%s','%s','%s','%s',%d)")

// SQL statements for Events table.
#define SQL_INSERT_EVENTS \
_T("INSERT INTO Events (StudyID,DataPoint,Seconds,Type,PeriodType,Description) \
VALUES (%lu,%lu,%lu,'%s',%d,'%s')")

#define SQL_GET_EVENTS \
_T("SELECT * from Events where StudyID=%lu ORDER by Seconds ASC")

/********************************************************************
Constructor

  Initialize class data.

********************************************************************/
CDBAccess::CDBAccess(const wchar_t *pszDSN)
{

  m_cstrDSN = pszDSN;
  m_bDBConnected = false;

  m_sqlhEnv = NULL; // Database environment handle.
  m_sqlhDbc = NULL; // Database connection handle.
  m_sqlhStmt = NULL; // Database statement handle.
  m_sqlhStmt1 = NULL; // Database statement handle.
	m_ctSrchDate = 0;
	m_iDateSrchType = DB_SRCH_NONE;
	m_cstrSrchDate.Empty();
}

/********************************************************************
Destructor

  Clean up class data.
  Disconnect from the database.
********************************************************************/
CDBAccess::~CDBAccess(void)
{
  sqlrDB_disconnect();
}


/********************************************************************
iDBConnect - Connect to the database.

  inputs: None.

  return: SUCCESS if connect to the database.
          FAIL if not connected to the database.
********************************************************************/
short CDBAccess::iDBConnect(void)
  {
  SQLRETURN sqlRet;
  short int iRet;

  sqlRet = sqlrDB_connect();
  if(SQL_SUCCEEDED(sqlRet))
    iRet = SUCCESS;
  else
    iRet = FAIL;
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// ODBC functions

/********************************************************************
sqlrDB_connect - Connect to the database.
                 Use SQLDriverConnect to attempt a connection to the 
                 indicated datasource.

  inputs: None.

  return: Return from ODBC functions.
********************************************************************/
SQLRETURN CDBAccess::sqlrDB_connect(void)
  {
  CString cstrConnStr;
  short int iOutConnLen;

  if(m_bDBConnected == false)
    {
    m_sqliHandleType = SQL_HANDLE_ENV;
      // Set up the database for accessing.
      // Allocate an environment handle.
    m_sqlrSts = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &m_sqlhEnv);
    if(SQL_SUCCEEDED(m_sqlrSts))
      {  // Set environment variables (only need to set ODBC version).
      m_sqlrSts = SQLSetEnvAttr(m_sqlhEnv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3,
                               SQL_IS_INTEGER);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {  // Allocate a database connection handle.
        m_sqlrSts = SQLAllocHandle(SQL_HANDLE_DBC, m_sqlhEnv, &m_sqlhDbc);
        if(SQL_SUCCEEDED(m_sqlrSts))
          { // For regular runtime database, use the existing DSN.
            // For the export database, connect without a DSN.
          if(m_cstrDSN == LOCAL_DSN_DEFAULT)
            cstrConnStr.Format(_T("DSN=%s"), m_cstrDSN);
          else
					{
						if(m_cstrDSN.Find(_T(".mdb")) < 0)
              cstrConnStr.Format(_T("Driver={Microsoft Access Driver (*.mdb)};DBQ=%s.mdb"), m_cstrDSN);
						else
              cstrConnStr.Format(_T("Driver={Microsoft Access Driver (*.mdb)};DBQ=%s"), m_cstrDSN);
					}
          m_sqlrSts = SQLDriverConnect(m_sqlhDbc, NULL, (SQLTCHAR *)(LPCTSTR)cstrConnStr,
                                       SQL_NTS, m_sqlOutConnStr, 1024, &iOutConnLen, SQL_DRIVER_COMPLETE);
          m_sqliHandleType = SQL_HANDLE_DBC;
          if(!SQL_SUCCEEDED(m_sqlrSts))
            { // Got an error.
            vReport_err(m_sqlhDbc);
            vFree_env_handle(); // Free the environment handle.
            vFree_db_conn_handle(); // Free the database connection handle.
            }
          else
            {
            m_bDBConnected = true; // Connected to the database.
            }
          }
        else
          {
          vReport_err(m_sqlhDbc);  // Got an error.
          vFree_env_handle(); // Free the environment handle.
          }
        }
      else
        {
        vReport_err(m_sqlhEnv);  // Got an error.
        vFree_env_handle(); // Free the environment handle.
        }
      }
    else //if(!SQL_SUCCEEDED(m_sqlrSts))
      vReport_err(m_sqlhEnv);  // Got an error.
    }
  else
    m_sqlrSts = SQL_SUCCESS; // Database already connected.
  return(m_sqlrSts);
  }

/********************************************************************
sqlrDB_disconnect - DisConnect from the database.

  inputs: None.

  return: Return from ODBC functions.
********************************************************************/
SQLRETURN CDBAccess::sqlrDB_disconnect(void)
  {

  if(m_sqlhDbc != NULL)
    {
    m_sqliHandleType = SQL_HANDLE_DBC;
    m_sqlrSts = SQLDisconnect(m_sqlhDbc);
    if(!SQL_SUCCEEDED(m_sqlrSts))
      vReport_err(m_sqlhDbc);  // Got an error.
    vFree_db_conn_handle(); // Free the database connection handle.
    }
  vFree_env_handle(); // Free the environment handle.
  m_bDBConnected = false;
  return(m_sqlrSts);
  }

/********************************************************************
vReport_err - Report a database error.  The message is obtained from data
              kept by the ODBC driver.

  inputs: Handle for the function that got the error.

  return: None.
********************************************************************/
void CDBAccess::vReport_err(SQLHANDLE sqlhHandle)
  {
  SQLRETURN sqlrRet;
  SQLWCHAR sqlszState[10], sqlszMsgText[SQL_ERR_MSG_LEN];
  SQLINTEGER sqliNativeErrorPtr;
  SQLSMALLINT sqliMsgLen;
  CString cstrErr, cstrMsg;

  if(iDBConnect() == SUCCESS)
    {
    if(sqlhHandle != NULL)
      {
      sqlrRet = SQLGetDiagRec(m_sqliHandleType, sqlhHandle, 1, sqlszState,
                             &sqliNativeErrorPtr, sqlszMsgText, SQL_ERR_MSG_LEN, &sqliMsgLen);
      if(sqlrRet == SQL_ERROR)
        cstrMsg.LoadString(IDS_SQL_ERROR);
      else if(sqlrRet == SQL_INVALID_HANDLE)
        cstrMsg.LoadString(IDS_SQL_INVALID_HANDLE);
      else if(sqlrRet == SQL_NO_DATA)
        cstrMsg.LoadString(IDS_SQL_ERR_UNKNOWN);
      else
        cstrMsg = sqlszMsgText;
  
      cstrErr.Format(IDS_FMT_DATABASE_ERROR, cstrMsg);
      theApp.vLog(cstrErr);
      cstrErr.LoadString(IDS_ERROR);
      theApp.m_pActiveWnd->MessageBox(cstrMsg, cstrErr, MB_OK);
      }
    }
  }

/********************************************************************
vFree_env_handle - Free the environment handle.

  inputs: None.

  return: None.
********************************************************************/
void CDBAccess::vFree_env_handle(void)
  {

  if(m_sqlhEnv != NULL)
    {
    m_sqliHandleType = SQL_HANDLE_ENV;
    m_sqlrSts = SQLFreeHandle(SQL_HANDLE_ENV, m_sqlhEnv);
    if(SQL_SUCCEEDED(m_sqlrSts))
      m_sqlhEnv = NULL;
    else
      vReport_err(m_sqlhEnv);  // Got an error.
    }
  }

/********************************************************************
vFree_db_conn_handle - Free the database connection handle.

  inputs: None.

  return: None.
********************************************************************/
void CDBAccess::vFree_db_conn_handle(void)
  {

  if(m_sqlhDbc != NULL)
    {
    m_sqliHandleType = SQL_HANDLE_DBC;
    m_sqlrSts = SQLFreeHandle(SQL_HANDLE_DBC, m_sqlhDbc);
    if(SQL_SUCCEEDED(m_sqlrSts))
      m_sqlhDbc = NULL;
    else
      vReport_err(m_sqlhDbc);  // Got an error.
    }
  }

/********************************************************************
sqlhGet_stmt_handle - Get a database statement handle.

  inputs: None.

  return: Handle to statement.
********************************************************************/
SQLHANDLE CDBAccess::sqlhGet_stmt_handle(void)
  {
  SQLHANDLE sqlhStmt;

  sqlhStmt = NULL;
  if(iDBConnect() == SUCCESS)
    {
    m_sqliHandleType = SQL_HANDLE_STMT;
    m_sqlrSts = SQLAllocHandle(SQL_HANDLE_STMT, m_sqlhDbc, &sqlhStmt); //&sqlhStmt);
    if(!SQL_SUCCEEDED(m_sqlrSts))
      { // Got an error, make sure the handle has been freed.
      vReport_err(sqlhStmt); //sqlhStmt);
      if(sqlhStmt != NULL)
        {
        SQLFreeHandle(SQL_HANDLE_STMT, sqlhStmt); //sqlhStmt);
        sqlhStmt = NULL;
        }
      }
    }
  return(sqlhStmt);
  }

/********************************************************************
sqlrFree_stmt_handle - Free a database statement handle.

  inputs: Database handle.

  return: Status of freeing the handle.
********************************************************************/
SQLRETURN CDBAccess::sqlrFree_stmt_handle(SQLHANDLE sqlhStmt)
  {
  SQLRETURN sqlrSts;

  sqlrSts = SQLFreeHandle(SQL_HANDLE_STMT, sqlhStmt);
  return(sqlrSts);
  }

/////////////////////////////////////////////////////////////////////
//// Methods for getting data from the Patient database table.

/********************************************************************
iFind_patient - Determine if a patient is in the "Patient" database
                     table for the patient name specified in the input.

  inputs: Pointer to FIND_PATIENT_INFO that contains the patient name.

  output: The lPatientID member of FIND_PATIENT_INFO gets set to the patient
          ID from the database.

  return: SUCCESS if the patient is in the table.
          FAIL if there is an error or the patient is not in the table.
********************************************************************/
short int CDBAccess::iFind_patient(PATIENT_TABLE *pPatient)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format(SQL_FIND_PATIENT, pPatient->cstrLastName,
                     pPatient->cstrFirstName, pPatient->cstrMI,
                     pPatient->cstrResearchRefNum);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          {
          SQLGetData(m_sqlhStmt, 1, SQL_C_LONG, &pPatient->lPatientID, 0, 0);
          iRet = SUCCESS;
          }
        else if(m_sqlrSts == SQL_NO_DATA)
          iRet = SUCCESS_NO_DATA; // No patient in the database.
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err(m_sqlhStmt);
    }
  return(iRet);
  }

/********************************************************************
iGet_next_patient - Get the patient information in the "Patient" database
                    table for the first or next patient ordered by
                    last name, then first name.
                    Getting the first record causes an SQL Select statement
                    to get all the patient records ordered by last name
                    then first name.

  inputs: Position in the record set from the table to get
            (first record or next record).
          Pointer to a PATIENT_TABLE structure that gets the data from the table entry.

  return: SUCCESS if there is another patient in the table.
          FAIL if there is an error or there are no more patients in the table.
********************************************************************/
short int CDBAccess::iGet_next_patient(short int iPos, PATIENT_TABLE *pPatientTable)
  {
  CString cstrSQL;
  int iField;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(iPos == FIRST_RECORD)
      { // Wants first record, so select all patients and return only first record.
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        //cstrSQL = SQL_PATIENT_TABLE_ORDER; //"Select PatientID,LastName,FirstName from Patient ORDER by LastName,FirstName ASC";
        if(m_cstrSearchSQL.IsEmpty() == true)
          cstrSQL = SQL_PATIENT_TABLE_ORDER;
        else
          cstrSQL = m_cstrSearchSQL;
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      else
        vReport_err(m_sqlhStmt);
      }
    else
      iRet = SUCCESS; // Get records after the first
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        iField = 1;
        SQLGetData(m_sqlhStmt, iField, SQL_C_LONG, &pPatientTable->lPatientID, 0, 0);
        iGet_string(m_sqlhStmt, ++iField, pPatientTable->cstrLastName);
        iGet_string(m_sqlhStmt, ++iField, pPatientTable->cstrFirstName);
        iGet_string(m_sqlhStmt, ++iField, pPatientTable->cstrMI);
        iGet_bool(m_sqlhStmt, ++iField, &pPatientTable->bUseResrchRefNum);
        iGet_string(m_sqlhStmt, ++iField, pPatientTable->cstrResearchRefNum);
        iGet_bool(m_sqlhStmt, ++iField, &pPatientTable->bHide);
        }
      else
        iRet = FAIL;
      }
    if(iRet == FAIL)
      sqlrFree_stmt_handle(m_sqlhStmt); // Done getting records.
    }
  return(iRet);
  }

/********************************************************************
iGet_patient - Get the  data from the "Patient" database table for
               the patient ID specified in the input.

  inputs: Pointer to a PATIENT_TABLE structure that contains the patient ID for
            the record to be retieved.  The structure also gets the
            data retrieved from the table.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDBAccess::iGet_patient(PATIENT_TABLE *pPatient)
  {
  CString cstrSQL;
  int iField;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format(SQL_GET_PATIENT, pPatient->lPatientID);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        { // Now get all the values.
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          {
          iRet = SUCCESS;
          iField = 1;
          m_sqlrSts = SQLGetData(m_sqlhStmt, iField, SQL_C_LONG, &pPatient->lPatientID, 0, 0);
          iGet_string(m_sqlhStmt, ++iField, pPatient->cstrLastName);
          iGet_string(m_sqlhStmt, ++iField, pPatient->cstrFirstName);
          iGet_string(m_sqlhStmt, ++iField, pPatient->cstrMI);
          iGet_bool(m_sqlhStmt, ++iField, &pPatient->bUseResrchRefNum);
          iGet_string(m_sqlhStmt, ++iField, pPatient->cstrResearchRefNum);
          iGet_bool(m_sqlhStmt, ++iField, &pPatient->bHide);
          }
        else
          vReport_err(m_sqlhStmt); // SQLFetch() failed.
        }
      else
        vReport_err(m_sqlhStmt); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err(m_sqlhStmt); // sqlhGet_stmt_handle() failed.
    }
  return(iRet);
  }

/********************************************************************
iSave_patient - Save the required data to the "Patient" database table.
                If the patient is not yet in the table, add the record,
                otherwise, do nothing.

  inputs: Pointer to a PATIENT_TABLE structure that contains the data to save to
            the table.

  return: SUCCESS if the data is saved to the table or nothing is done.
          FAIL if there is an error.
********************************************************************/
short int CDBAccess::iSave_patient(PATIENT_TABLE *pPatient)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(pPatient->lPatientID == INV_LONG)
      { // Add a new record to the database.
      cstrSQL.Format(SQL_INSERT_PATIENT, pPatient->cstrLastName, pPatient->cstrFirstName,
                     pPatient->cstrMI, pPatient->bUseResrchRefNum, pPatient->cstrResearchRefNum,
                     pPatient->bHide);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err(m_sqlhStmt);
        sqlrFree_stmt_handle(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          { // Now find the PatientID that was just assigned.
          cstrSQL.Format(_T("SELECT TOP 1 PatientID from Patient ORDER by PatientID DESC"));
          if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
            {
            m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
            if(SQL_SUCCEEDED(m_sqlrSts))
              {
              m_sqlrSts = SQLFetch(m_sqlhStmt);
              if(SQL_SUCCEEDED(m_sqlrSts))
                {
                m_sqlrSts = SQLGetData(m_sqlhStmt, 1, SQL_C_LONG, &pPatient->lPatientID, 0, 0);
                if(!SQL_SUCCEEDED(m_sqlrSts))
                  vReport_err(m_sqlhStmt);
                else
                  iRet = SUCCESS;
                }
              else
                vReport_err(m_sqlhStmt); // SQLFetch() failed.
              }
            else
              vReport_err(m_sqlhStmt); // SQLExecDirect() failed.
            sqlrFree_stmt_handle(m_sqlhStmt);
            }
          }
        }
      else
        vReport_err(m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// Methods for getting data from the StudyData database table.

/********************************************************************
iGet_patient_id_from_study - Get the patient ID in the "StudyInfo"
                             database table for the specified study ID.

  inputs: Study ID to get the patient ID for.
          Pointer to the patient ID.

  return: SUCCESS if the study ID is found and the patient ID is retrieved.
          FAIL if there is an error.
********************************************************************/
short int CDBAccess::iGet_patient_id_from_study(long lStudyID, long *plPatientID)
{
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format(_T("Select PatientID from StudyData where StudyID=%lu"), lStudyID);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        iRet = SUCCESS;
      }
    else
      vReport_err(m_sqlhStmt1);
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt1);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        SQLGetData(m_sqlhStmt1, 1, SQL_C_LONG, plPatientID, 0, 0);
        }
      else
        {
        if(m_sqlrSts == SQL_NO_DATA)
          iRet = SUCCESS_NO_DATA;
        else
          iRet = FAIL;
        }
      }
    if(iRet == FAIL || iRet == SUCCESS_NO_DATA)
      sqlrFree_stmt_handle(m_sqlhStmt1); // Done getting records.
    }
  return(iRet);
  }

/********************************************************************
iFind_next_study - Get the first or next patient study in the "StudyInfo"
                   database table for the first or next study
                   ordered by the date of the study.
                   Getting the first record causes an SQL Select
                   statement to get all the study records ordered
                   by date of study.

  inputs: Position in the record set from the table to get
            (first record or next record).
          Pointer to a FIND_NEXT_STUDY structure that gets the data.

  return: SUCCESS if there is a next study.
          FAIL if there is an error or the there are no more studies.
********************************************************************/
short int CDBAccess::iFind_next_study(short int iPos, FIND_NEXT_STUDY *pfns)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(iPos == FIRST_RECORD)
      { // Wants first record, so select all patients and return only first record.
      if((m_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
        {
        cstrSQL.Format(_T("Select StudyID,DateOfStudy,Pause,BLDataPoints,DataFile \
                       from StudyData where PatientID=%lu ORDER by DateOfStudy ASC"),
                       pfns->lPatientID);
        m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      else
        vReport_err(m_sqlhStmt1);
      }
    else
      iRet = SUCCESS; // Get records after the first
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt1);
      if(SQL_SUCCEEDED(m_sqlrSts))
			{
				while(true)
				{
					SQLGetData(m_sqlhStmt1, 1, SQL_C_LONG, &pfns->lStudyID, 0, 0);
					iGet_string(m_sqlhStmt1, 2, pfns->cstrDate);
					iGet_bool(m_sqlhStmt1, 3, &pfns->bPause);
					SQLGetData(m_sqlhStmt1, 4, SQL_C_LONG, &pfns->uDataPtsInBaseline, 0, 0);
					iGet_string(m_sqlhStmt1, 5, pfns->cstrFileName);
          if(bLook_for_study_date(pfns->cstrDate) == true)
            break; // Either there isn't a date search or the study date matches the search date.
          else
            { // Study date didn't match the search date.  Fetch the next record.
            m_sqlrSts = SQLFetch(m_sqlhStmt1);
            if(!SQL_SUCCEEDED(m_sqlrSts))
              {
              iRet = FAIL;
              break;
              }
            }
				}
			}
      else
        {
        if(m_sqlrSts == SQL_NO_DATA)
          iRet = SUCCESS_NO_DATA;
        else
          iRet = FAIL;
        }
      }
    if(iRet == FAIL || iRet == SUCCESS_NO_DATA)
      sqlrFree_stmt_handle(m_sqlhStmt1); // Done getting records.
    }
  return(iRet);
  }

/********************************************************************
iGet_study_data - Get all the record values for the study specified
                  by the study ID from the "StudyData" database
                  table.  The study ID is specified in the input.

  inputs: Pointer to a STUDY_DATA structure that contains the study ID
            and gets the data retrieved from the table.

  return: SUCCESS if the study data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDBAccess::iGet_study_data(STUDY_DATA *pStudy)
  {
  CString cstrSQL;
  int iField;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format(SQL_GET_STUDY_DATA_FOR_ID, pStudy->lStudyID);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        { // Now get all the values.
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          {
          iRet = SUCCESS;
          iField = 1;
          SQLGetData(m_sqlhStmt, iField, SQL_C_LONG, &pStudy->lStudyID, 0, 0);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &pStudy->lPatientID, 0, 0);
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrDataFile);
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrDateOfStudy);
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrMealTime);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_SHORT, &pStudy->uWaterAmount, 0, 0);
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrWaterAmountUnits);
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrPeriodStart); // Start period
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrPeriodLen); // Length of period
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrStartMinutes); // Start minutes
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrEndMinutes); // End minutes
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &pStudy->lTestMaxSignal, 0, 0);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &pStudy->lTestMinSignal, 0, 0);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bExtra10Min);
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrSuggestedDiagnosis);
            // BOOL requires a short.
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bBloating);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bEarlySateity);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bEructus);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bGerd);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bNausea);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bPostPrandialFullness);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bVomiting);
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrAttendingPhysician);
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrReferingPhysician);
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrTechnician);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bManualMode);
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrRespirationRate); // Respiration rate
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bUserDiagnosis);
          iGet_long_string(m_sqlhStmt, ++iField, pStudy->cstrIFPComments);
          iGet_long_string(m_sqlhStmt, ++iField, pStudy->cstrDiagnosis);
          iGet_long_string(m_sqlhStmt, ++iField, pStudy->cstrRecommendations);
          iGet_long_string(m_sqlhStmt, ++iField, pStudy->cstrMedications);
          iGet_bit(m_sqlhStmt, ++iField, &pStudy->bDelayedGastricEmptying);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &pStudy->uiBLDataPoints, 0, 0);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bPause);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &pStudy->uiPostStimDataPoints, 0, 0);
          iGet_long_string(m_sqlhStmt, ++iField, pStudy->cstrStimulationMedium);
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrDiagnosisKeyPhrase);
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrICDCode);
          iGet_string(m_sqlhStmt, ++iField, pStudy->cstrRecommendationKeyPhrase);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bGastroparesis);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bDyspepsiaBloatSatiety);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bDyspepsia);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bAnorexia);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bRefractoryGERD);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bWeightLoss);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bPostDilationEval);
          iGet_bool(m_sqlhStmt, ++iField, &pStudy->bEvalEffectMed);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &pStudy->uFrequency, 0, 0);

          // Frequency is a new field and may not have ever been set to a valid value.
          // If frequency is not a valid value, default it to 15cpm.
          if(pStudy->uFrequency == 0 || pStudy->uFrequency > 20000)
            pStudy->uFrequency = 15;
          }
        }
      else
        vReport_err(m_sqlhStmt); // SQLExecDirect failed
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err(m_sqlhStmt); // sqlhGet_stmt_handle() failed.
    }
  return(iRet);
  }

/********************************************************************
iSave_study_data - Save the study data to the "StudyData" database
                   table.  If the study is not yet in the table, add the
                   record, otherwise, do nothing.

  inputs: Pointer to a STUDY_DATA structure that contains the study data
             and an invalid study ID.  On return, the Study ID is the new
             study ID obtained when the record was added to the table.

  return: SUCCESS if the data is saved to the table or nothing is done.
          FAIL if there is an error.
********************************************************************/
short int CDBAccess::iSave_study_data(STUDY_DATA *pStudy)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(pStudy->lStudyID == INV_LONG)
      { // Insert new study.
        cstrSQL.Format(SQL_INSERT_STUDY_DATA, pStudy->lPatientID, pStudy->cstrDataFile, 
                     pStudy->cstrDateOfStudy,
                     pStudy->cstrMealTime, pStudy->uWaterAmount, pStudy->cstrWaterAmountUnits,
                     pStudy->cstrPeriodStart, pStudy->cstrPeriodLen, 
                     pStudy->cstrStartMinutes, pStudy->cstrEndMinutes,
                     pStudy->lTestMaxSignal, pStudy->lTestMinSignal, pStudy->bExtra10Min,
                     pStudy->cstrSuggestedDiagnosis, pStudy->bBloating, pStudy->bEarlySateity,
                     pStudy->bEructus, pStudy->bGerd, pStudy->bNausea, 
                     pStudy->bPostPrandialFullness,
                     pStudy->bVomiting, pStudy->cstrAttendingPhysician, 
                     pStudy->cstrReferingPhysician, pStudy->cstrTechnician,
                     pStudy->bManualMode, pStudy->cstrRespirationRate, pStudy->bUserDiagnosis,
                     pStudy->cstrIFPComments, pStudy->cstrDiagnosis,
                     pStudy->cstrRecommendations, pStudy->cstrMedications,
                     pStudy->bDelayedGastricEmptying, pStudy->uiBLDataPoints,
                     pStudy->bPause, pStudy->uiPostStimDataPoints,
                     pStudy->cstrStimulationMedium,
                     pStudy->cstrDiagnosisKeyPhrase,
                     pStudy->cstrICDCode,
                     pStudy->cstrRecommendationKeyPhrase,
                     pStudy->bGastroparesis, pStudy->bDyspepsiaBloatSatiety,
                     pStudy->bDyspepsia, pStudy->bAnorexia, pStudy->bRefractoryGERD,
                     pStudy->bWeightLoss, pStudy->bPostDilationEval,
                     pStudy->bEvalEffectMed, pStudy->uFrequency);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err(m_sqlhStmt);
        sqlrFree_stmt_handle(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          { // Now find the StudyID that was just assigned.
          cstrSQL.Format(_T("SELECT TOP 1 StudyID from StudyData where PatientID = %lu ORDER by StudyID DESC"),
                         pStudy->lPatientID);
          if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
            {
            m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
            if(SQL_SUCCEEDED(m_sqlrSts))
              {
              m_sqlrSts = SQLFetch(m_sqlhStmt);
              if(SQL_SUCCEEDED(m_sqlrSts))
                {
                m_sqlrSts = SQLGetData(m_sqlhStmt, 1, SQL_C_LONG, &pStudy->lStudyID, 0, 0);
                if(SQL_SUCCEEDED(m_sqlrSts))
                  iRet = SUCCESS;
                else
                  vReport_err(m_sqlhStmt);
                }
              else
                vReport_err(m_sqlhStmt); // SQLFetch() failed.
              }
            else
              vReport_err(m_sqlhStmt); // SQLExecDirect() failed.
            sqlrFree_stmt_handle(m_sqlhStmt);
            }
          }
        }
      else
        vReport_err(m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// Methods for getting data from the PatientInfo database table.

/********************************************************************
iGet_patient_info - Get the  data from the "PatientInfo" database table for
                    the patient ID specified in the input.

  inputs: Patien ID.
          Pointer to a PATIENT_INFO structure that gets the data retrieved
            from the table.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDBAccess::iGet_patient_info(long lPatientID, PATIENT_INFO *pPInfo)
  {
  SQLHSTMT sqlhStmt = NULL; // Database statement handle.
  CString cstrSQL;
  int iField;
  short int iRet = FAIL;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format(SQL_PATIENT_TABLE_INFO, lPatientID);
      m_sqlrSts = SQLExecDirect(sqlhStmt,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        { // Now get all the values.
        m_sqlrSts = SQLFetch(sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          {
          iRet = SUCCESS;
          iField = 1;
          iGet_string(sqlhStmt, ++iField, pPInfo->cstrAddress1);
          iGet_string(sqlhStmt, ++iField, pPInfo->cstrAddress2);
          iGet_string(sqlhStmt, ++iField, pPInfo->cstrCity);
          iGet_string(sqlhStmt, ++iField, pPInfo->cstrState);
          iGet_string(sqlhStmt, ++iField, pPInfo->cstrZipCode);
          iGet_string(sqlhStmt, ++iField, pPInfo->cstrPhone);
          iGet_string(sqlhStmt, ++iField, pPInfo->cstrDateOfBirth);
          iGet_string(sqlhStmt, ++iField, pPInfo->cstrHospitalNumber);
          iGet_string(sqlhStmt, ++iField, pPInfo->cstrSocialSecurityNum);
          iGet_bool(sqlhStmt, ++iField, &pPInfo->bMale);
          iGet_bool(sqlhStmt, ++iField, &pPInfo->bFemale);
          }
        else
          vReport_err(sqlhStmt); // SQLFetch() failed.
        }
      else
        vReport_err(sqlhStmt); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(sqlhStmt);
      }
    else
      vReport_err(sqlhStmt); // sqlhGet_stmt_handle() failed.
    }
  return(iRet);
  }

/********************************************************************
iSave_patient_info - Save the required data to the "PatientInfo" database
                     table. If the patient is not yet in the table, add the
                     record, otherwise, do nothing.

  inputs: Patient ID.
          Pointer to a PATIENT_INFO structcure that contains the data to save to
            the table.

  return: SUCCESS if the data is saved to the table or nothing is done.
          FAIL if there is an error.
********************************************************************/
short int CDBAccess::iSave_patient_info(long lPatientID, PATIENT_INFO *pPInfo)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    if(lPatientID != INV_LONG)
      { // Have a patient ID, see if we can find a record in this table.
      iRet = iFind_patient_info(lPatientID);
      }
    else
      iRet = FAIL;
    if(iRet == FAIL)
      { // Add a new record to the database.
      cstrSQL.Format(SQL_INSERT_PATIENT_INFO, lPatientID,
                     pPInfo->cstrAddress1, pPInfo->cstrAddress2, pPInfo->cstrCity,
                     pPInfo->cstrState, pPInfo->cstrZipCode, pPInfo->cstrPhone,
                     pPInfo->cstrDateOfBirth, pPInfo->cstrHospitalNumber,
                     pPInfo->cstrSocialSecurityNum, pPInfo->bMale, pPInfo->bFemale);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err(m_sqlhStmt);
        else
          iRet = SUCCESS;
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      else
        vReport_err(m_sqlhStmt);  // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/********************************************************************
iFind_patient_info - Determine if a patient is in the "PatientInfo" database
                     table for the patient ID specified in the input.

  inputs: Patient ID.

  return: SUCCESS if the patient is in the table.
          FAIL if there is an error or the patient is not in the table.
********************************************************************/
short int CDBAccess::iFind_patient_info(long lPatientID)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format(_T("Select PatientID from PatientInfo where PatientID=%lu"), lPatientID);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err(m_sqlhStmt); // sqlhGet_stmt_handle() failed.
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// Methods for accessing data in the WhatIf database table

/********************************************************************
iFind_next_whatif - Get the first or next record in the "WhatIf" database table
                    for the study ID specified in the input structure on
                    the first call.
                    Getting the first record causes an SQL Select
                    statement to get all the What If records ordered
                    by date/time.

  inputs: Position in the record set from the table to get
            (first record or next record).
          Pointer to a WHAT_IF structure that contains the Study ID on the
            first call and gets the record data.

  return: SUCCESS if there is a next WhatIf record for the Study ID.
          FAIL if there is an error or the there are no more studies.
********************************************************************/
short int CDBAccess::iFind_next_whatif(short int iPos, WHAT_IF *pwi)
  {
  CString cstrSQL;
  short int iRet;
  int iField;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(iPos == FIRST_RECORD)
      { // Wants first record, so select all patients and return only first record.
      if((m_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
        {
        cstrSQL.Format(SQL_WHATIF_GET_DESC_ID, pwi->lStudyID);
        m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        else
          vReport_err(m_sqlhStmt1);
        }
      else
        vReport_err(m_sqlhStmt1);
      }
    else
      iRet = SUCCESS; // Get records after the first
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt1);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
          iField = 1;
          SQLGetData(m_sqlhStmt1, iField, SQL_C_LONG, &pwi->lWhatIfID, 0, 0);
          SQLGetData(m_sqlhStmt1, ++iField, SQL_C_LONG, &pwi->lStudyID, 0, 0);
          iGet_string(m_sqlhStmt1, ++iField, pwi->cstrDescription);
          iGet_string(m_sqlhStmt1, ++iField, pwi->cstrDateTimeSaved);
          SQLGetData(m_sqlhStmt1, ++iField, SQL_C_SHORT, &pwi->uNumPeriods, 0, 0);
          iGet_string(m_sqlhStmt1, ++iField, pwi->cstrPeriodStart); // Start period
          iGet_string(m_sqlhStmt1, ++iField, pwi->cstrPeriodLen); // Length of period
          iGet_string(m_sqlhStmt1, ++iField, pwi->cstrStartMinutes); // Start minutes
          iGet_string(m_sqlhStmt1, ++iField, pwi->cstrEndMinutes); // End minutes
          iGet_string(m_sqlhStmt1, ++iField, pwi->cstrRespirationRate); // Respiration rate
          SQLGetData(m_sqlhStmt1, ++iField, SQL_C_LONG, &pwi->uFrequency, 0, 0);

          // If frequency is not a valid value, default it to 15cpm.
          if(pwi->uFrequency == 0 || pwi->uFrequency > 20000)
            pwi->uFrequency = 15;
        }
      else
        {
        if(m_sqlrSts == SQL_NO_DATA)
          iRet = SUCCESS_NO_DATA;
        else
          iRet = FAIL;
        }
      }
    if(iRet == FAIL || iRet == SUCCESS_NO_DATA)
      sqlrFree_stmt_handle(m_sqlhStmt1); // Done getting records.
    }
  return(iRet);
  }

/********************************************************************
iSave_whatif_data - Save the What If data to the "WhatIf" database
                    table.  If the What If is not yet in the table, add the
                    record, otherwise, do nothing.

  inputs: Pointer to a WHAT_IF structure that contains an invalid
            study ID plus the data to save.

  return: SUCCESS if the data is saved to the table or nothing is done.
          FAIL if there is an error.
********************************************************************/
short int CDBAccess::iSave_whatif_data(WHAT_IF *pwi)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(pwi->lWhatIfID == INV_LONG)
      { // Insert new study.
      cstrSQL.Format(SQL_WHATIF_INSERT_RECORD, pwi->lStudyID, pwi->cstrDescription,
                     pwi->cstrDateTimeSaved, pwi->uNumPeriods,
                     pwi->cstrPeriodStart, pwi->cstrPeriodLen,
                     pwi->cstrStartMinutes, pwi->cstrEndMinutes, pwi->cstrRespirationRate, pwi->uFrequency);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err(m_sqlhStmt);
        else
          iRet = SUCCESS;
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      else
        vReport_err(m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// Methods for accessing data in the Events database table

/********************************************************************
iSave_event - Save an event to the "Events" database table.

  inputs: Pointer to a EVENTS_DATA structure.  It contains the Study ID.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDBAccess::iSave_event(EVENTS_DATA *pEvent)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
  {
    cstrSQL.Format(SQL_INSERT_EVENTS, pEvent->lStudyID, pEvent->lDataPoint, pEvent->lSeconds,
                   pEvent->cstrType, pEvent->iPeriodType, pEvent->cstrDescription);
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(m_sqlrSts))
        {
        iRet = FAIL;
        vReport_err(m_sqlhStmt);
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      {
      iRet = FAIL;
      vReport_err(m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/********************************************************************
iGet_event - Get an Event from the Events table.
             If the StudyID is specified, the first event for the study
             ID is retrieved.  If the Study ID is invalid, the next
             event is retrieved for the original study ID.

  inputs: Pointer to a EVENTS_DATA structure which gets the event.
              It contains the Study ID.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDBAccess::iGet_event(EVENTS_DATA *pEvent)
  {
  CString cstrSQL;
  int iField;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(pEvent->lStudyID != INV_LONG)
      { // Get first event for the study.
      if((m_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
        {
        cstrSQL.Format(SQL_GET_EVENTS, pEvent->lStudyID);
        m_sqlrSts = SQLExecDirect(m_sqlhStmt1, (SQLWCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      }
    else
      iRet = SUCCESS; // Get next event for the study.
    if(iRet == SUCCESS)
      { // Now get all the values.
      m_sqlrSts = SQLFetch(m_sqlhStmt1);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        iRet = SUCCESS;
        iField = 3; // Skip first field (Event ID) and second field (study ID).
        m_sqlrSts = SQLGetData(m_sqlhStmt1, iField, SQL_C_LONG, &pEvent->lDataPoint, 0, 0);
        m_sqlrSts = SQLGetData(m_sqlhStmt1, ++iField, SQL_C_LONG, &pEvent->lSeconds, 0, 0);
        iGet_string(m_sqlhStmt1, ++iField, pEvent->cstrType);
        SQLGetData(m_sqlhStmt1, ++iField, SQL_C_SHORT, &pEvent->iPeriodType, 0, 0);
        iGet_long_string(m_sqlhStmt1, ++iField, pEvent->cstrDescription);
        }
      else
        {
        if(m_sqlrSts == SQL_NO_DATA)
          iRet = SUCCESS_NO_DATA;
        else
          iRet = FAIL;
        }
      }
    if(iRet == FAIL || iRet == SUCCESS_NO_DATA)
      { // Either error or no more records.
      if(m_sqlrSts != SQL_NO_DATA)
        vReport_err(m_sqlhStmt1); // Error.
      sqlrFree_stmt_handle(m_sqlhStmt1);
      }
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// Methods for getting data from a fetch statement.

/********************************************************************
iGet_string - Get a string value for the specified column from the current
              record retrieved in an SQL statement.

  inputs: Handle to the SQL statement used to get the record.
          Column in the database table to get the value for.
          Reference to a CString object that get the value.

  return: SUCCESS if value is retrieved from the record.
          FAIL if there is an error.
********************************************************************/
short int CDBAccess::iGet_string(SQLHSTMT sqlhStmt, short int iColumn, CString &cstrData)
  {
  SQLINTEGER sqliSize;
  char szData[256];
  short int iRet = FAIL;

  szData[0] = NULL;
  if(iDBConnect() == SUCCESS)
    {
    if((m_sqlrSts = SQLGetData(sqlhStmt, iColumn, SQL_C_CHAR, szData, sizeof(szData), &sqliSize))
    == SQL_SUCCESS)
      {
      cstrData = szData;
      iRet = SUCCESS;
      }
    else if(m_sqlrSts == SQL_NO_DATA)
      {
      cstrData.Empty();
      iRet = SUCCESS;
      }
    if(iRet == FAIL)
      vReport_err(sqlhStmt);
    }
  return(iRet);
  }

/********************************************************************
iGet_bool - Get a boolean (TRUE/FALSE) value for the specified column from the
            current record retrieved in an SQL statement.

  inputs: Handle to the SQL statement used to get the record.
          Column in the database table to get the value for.
          Pointer to a BOOL that gets the value read from the database record.

  return: SUCCESS.
********************************************************************/
short int CDBAccess::iGet_bool(SQLHSTMT sqlhStmt, short int iColumn, BOOL *pbValue)
  {
  short int iValue;

  m_sqlrSts = SQLGetData(sqlhStmt, iColumn, SQL_C_SHORT, &iValue, 0, 0);
  if(iValue != 0)
    *pbValue = TRUE;
  else
    *pbValue = FALSE;
  return(SUCCESS);
  }

/********************************************************************
iGet_long_string - Get a string value for the specified column from the
                   current record retrieved in an SQL statement.
                   The string may be too long for the SQL get data function
                   to return in one call (e.g. a memo field).  Therefore
                   this method can make multiple calls until the entire
                   string is retrieved.

  inputs: Handle to the SQL statement used to get the record.
          Column in the database table to get the value for.
          Reference to a CString object that get the value.

  return: SUCCESS if value is retrieved from the record.
          FAIL if there is an error.
********************************************************************/
short int CDBAccess::iGet_long_string(SQLHSTMT sqlhStmt, short int iColumn, CString &cstrData)
  {
  SQLINTEGER sqliSize;
  char szData[256];
  short int iRet = FAIL;

  cstrData.Empty();
  szData[0] = NULL;
  if(iDBConnect() == SUCCESS)
    { // Repeatedly get successive pieces of the string until all pieces are
      // retrieved.
    while((m_sqlrSts = SQLGetData(sqlhStmt, iColumn, SQL_C_CHAR, szData, sizeof(szData), &sqliSize))
    == SQL_SUCCESS_WITH_INFO)
      cstrData += szData;
    if(SQL_SUCCEEDED(m_sqlrSts))
      { // Got all pieces of the string.
      cstrData = szData;
      iRet = SUCCESS;
      }
    else if(m_sqlrSts == SQL_NO_DATA)
      {
      cstrData.Empty();
      iRet = SUCCESS;
      }
    else
      vReport_err(sqlhStmt);
    }
  return(iRet);
  }

/********************************************************************
iGet_bit - Get a boolean (TRUE/FALSE) value for the specified column from the
            current record retrieved in an SQL statement.

  inputs: Handle to the SQL statement used to get the record.
          Column in the database table to get the value for.
          Pointer to a BOOL that gets the value read from the database record.

  return: SUCCESS.
********************************************************************/
short int CDBAccess::iGet_bit(SQLHSTMT sqlhStmt, short int iColumn, BOOL *pbValue)
  {
  unsigned char ucValue;

  m_sqlrSts = SQLGetData(sqlhStmt, iColumn, SQL_C_BIT, &ucValue, 0, 0);
  if(ucValue != 0)
    *pbValue = TRUE;
  else
    *pbValue = FALSE;
  return(SUCCESS);
  }
///********************************************************************
//cstrFormat - Reformat a string being put in the database.
//               - Add an apostrophe be each existing apostrophe.
//
//  inputs: String being added to the database.
//
//  return: Reformatted string.
//********************************************************************/
//CString CDBAccess::cstrFormat(CString cstrItem)
//  {
//  CString cstrRet;
//  int iIndex;
//
//  iIndex = 0;
//  cstrRet = cstrItem;
//  while((iIndex = cstrRet.Find(_T("'"), iIndex)) >= 0)
//    {
//    cstrRet.Insert(iIndex, _T("'"));
//    iIndex += 2;
//    }
//  return (cstrRet);
//  }


/********************************************************************
bLook_for_study_date - Look to see if this study date matches the
                       search study date.

  inputs: Reference to a CString object containing the date of the study.

  return: true if the study date match the search study date, otherwise false.
********************************************************************/
bool CDBAccess::bLook_for_study_date(CString cstrDate)
{
  CString cstrToken, cstraSrchDate[3], cstraDate[3];
  CTime ctSrchDate;
  bool bRet;
  int iIndex;

  if(m_iDateSrchType != DB_SRCH_NONE)
    {
    if(m_iDateSrchType == DB_SRCH_GREATER_THAN)
      {
      ctSrchDate = ctMake(cstrDate); // Convert cstrDate into a CTime class.
      if(ctSrchDate > m_ctSrchDate)
        bRet = true;
      else
        bRet = false;
      }
    else if (m_iDateSrchType == DB_SRCH_LESS_THAN)
      {
      ctSrchDate = ctMake(cstrDate); // Convert cstrDate into a CTime class.
      if(ctSrchDate < m_ctSrchDate)
        bRet = true;
      else
        bRet = false;
      }
    else // if (m_iDateSrchType == DB_SRCH_EQUAL_TO)
      {
      // Separate the month, day, year from the dates.
      vParse_date(m_cstrSrchDate, cstraSrchDate, 3);
      vParse_date(cstrDate, cstraDate, 3);
      // Look at month.
      if(cstraSrchDate[0].Find(_T("*")) >= 0)
        bRet = true;
      else
        {
        CW2A pszDate0(cstraDate[0]);
        CW2A pszSrchDate0(cstraSrchDate[0]);
        if(atoi(pszSrchDate0) == atoi(pszDate0))
          bRet = true;
        else
          bRet = false;
        }
      if(bRet == true)
        { // Month matches, look at day.
        if(cstraSrchDate[1].Find(_T("*")) >= 0)
          bRet = true;
        else
          {
          CW2A pszDate1(cstraDate[1]);
					CW2A pszSrchDate1(cstraSrchDate[1]);
          if(atoi(pszSrchDate1) == atoi(pszDate1))
            bRet = true;
          else
            bRet = false;
          }
        if(bRet == true)
          { // Month and day match, look at year.
          if((iIndex = cstraSrchDate[2].Find(_T("*"))) >= 0)
            {
            if(iIndex == 0)
              bRet = true; // Only * in field.
            else
              {
              if(cstraSrchDate[2].Left(iIndex) == cstraDate[2].Left(iIndex))
                bRet = true;
              else
                bRet = false;
              }
            }
          else
            {
						CW2A pszDate2(cstraDate[2]);
						CW2A pszSrchDate2(cstraSrchDate[2]);
            if(atoi(pszSrchDate2) == atoi(pszDate2))
              bRet = true;
            else
              bRet = false;
            }
          }
        }
      }
    }
  else
    bRet = true; // No date search.
  return(bRet);
}

/********************************************************************
ctMake - Make a CTime object from a date string in the format: mm/dd/yyyy

  inputs: Reference to a CString object containing a date.

  return: CTime object.
********************************************************************/
CTime CDBAccess::ctMake(CString cstrDate)
{
  CString cstraDate[3]; // Month, day, year

  vParse_date(cstrDate, cstraDate, 3);
	CW2A pszDate0(cstraDate[0]);
	CW2A pszDate1(cstraDate[1]);
	CW2A pszDate2(cstraDate[2]);
  return(CTime(atoi(pszDate2), atoi(pszDate0), atoi(pszDate1), 0, 0, 0));
}

/********************************************************************
vParse_date - Separate the month, day, year from a date string in the format: mm/dd/yyyy

  inputs: Reference to a CString object containing a date.
          CString array that will get the month, day, year respectively.

  return: None.
********************************************************************/
void CDBAccess::vParse_date(CString cstrDate, CString cstraDate[], int iLen)
{
  int iCurPos;
  CString cstrToken;

  iCurPos = 0;
  cstrToken = cstrDate.Tokenize(_T("/"), iCurPos);
  for(int i = 0; cstrToken != "" && i < iLen; ++i)
    {
    cstraDate[i] = cstrToken;
    cstrToken = cstrDate.Tokenize(_T("/"), iCurPos);
    }
}
