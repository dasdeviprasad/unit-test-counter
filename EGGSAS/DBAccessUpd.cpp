/********************************************************************
DBAccessUpd.cpp

Copyright (C) 2019 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CDBAccessUpd class.

  This class provides access to the Microsoft Access database via
  SQL and an ODBC driver for updating an older version of a database
  to the lastest version.


HISTORY:
09-FEB-19  RET
             New
********************************************************************/
#include "stdafx.h"
#include "EGGSAS.h"
#include "dbaccessupd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SQL_LEN 100 // Max length of SQL statement.
#define SQL_ERR_MSG_LEN 512 // Max length of SQL error message.

// SQL statements for Version table.
#define SQL_GET_VERSION "SELECT * from Version"
#define SQL_UPDATE_VERSION "UPDATE Version set Version='%s'"

#define SQL_GET_PATIENT_ORDER \
"Select PatientID,LastName,FirstName,MiddleInitial from Patient ORDER by PatientID ASC";

#define SQL_UPDATE_PATIENT \
"UPDATE Patient set LastName='%s',FirstName='%s',MiddleInitial='%s',\
UseResrchRefNum=%d,ResearchRefNum='%s' where PatientID=%lu"

    // Database data.
static SQLRETURN s_sqlrSts; // Returns status from ODBC functions.
static SQLHENV s_sqlhEnv = NULL; // Database environment handle.
static SQLHDBC s_sqlhDbc = NULL; // Database connection handle.
static SQLSMALLINT s_sqliHandleType; // Handle type used by error handler.
static SQLHSTMT s_sqlhStmt = NULL; // Database statement handle.
static SQLHSTMT s_sqlhStmt1 = NULL; // Database statement handle.
static unsigned s_uQueryTimeout = 5; // Query time out is 5 seconds.

static SQLCHAR s_sqlOutConnStr[1024];

//extern CString g_cstrDBFile;


/********************************************************************
Constructor

  Initialize class data.

********************************************************************/
CDBAccessUpd::CDBAccessUpd(CString cstrUpdDB)
: m_cstrDSN(_T(""))
, m_bDBConnected(false)
, m_cstrErr(_T(""))
, m_cstrInfo(_T(""))
  {
    m_cstrDSN = cstrUpdDB;
  }

/********************************************************************
Destructor

  Clean up class data.
  Disconnect from the database.
********************************************************************/
CDBAccessUpd::~CDBAccessUpd(void)
  {
    // Disconnect from the database.
  sqlrDB_disconnect();
  }

/********************************************************************
iDBConnect - Connect to the database.

  inputs: None.

  return: SUCCESS if connect to the database.
          FAIL if not connected to the database.
********************************************************************/
short CDBAccessUpd::iDBConnect(void)
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

/********************************************************************
sqlrDB_connect - Connect to the database.
                 Use SQLDriverConnect to attempt a connection to the 
                 indicated datasource.

  inputs: None.

  return: Return from ODBC functions.
********************************************************************/
SQLRETURN CDBAccessUpd::sqlrDB_connect(void)
  {
  CString cstrConnStr;
  short int iOutConnLen;

  if(m_bDBConnected == false)
    {
    s_sqliHandleType = SQL_HANDLE_ENV;
      // Set up the database for accessing.
      // Allocate an environment handle.
    s_sqlrSts = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &s_sqlhEnv);
    if(SQL_SUCCEEDED(s_sqlrSts))
      {  // Set environment variables (only need to set ODBC version).
      s_sqlrSts = SQLSetEnvAttr(s_sqlhEnv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3,
                               SQL_IS_INTEGER);
      if(SQL_SUCCEEDED(s_sqlrSts))
        {  // Allocate a database connection handle.
        s_sqlrSts = SQLAllocHandle(SQL_HANDLE_DBC, s_sqlhEnv, &s_sqlhDbc);
        if(SQL_SUCCEEDED(s_sqlrSts))
          {
          cstrConnStr.Format(
              "Driver={Microsoft Access Driver (*.mdb)};DBQ=%s", m_cstrDSN);

          s_sqlrSts = SQLDriverConnect(s_sqlhDbc, NULL, (SQLTCHAR *)(const char *)cstrConnStr,
                                       SQL_NTS, s_sqlOutConnStr, 1024, &iOutConnLen, SQL_DRIVER_COMPLETE);
////                                    SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
          s_sqliHandleType = SQL_HANDLE_DBC;
          if(!SQL_SUCCEEDED(s_sqlrSts))
            { // Got an error.
            iReport_err(s_sqlhDbc, true);
            vFree_env_handle(); // Free the environment handle.
            vFree_db_conn_handle(); // Free the database connection handle.
            }
          else
            {
            m_bDBConnected = true; // Connected to the database.
//            iUpdate_patient();
            }
          }
        else
          {
          iReport_err(s_sqlhDbc, true);  // Got an error.
          vFree_env_handle(); // Free the environment handle.
          }
        }
      else
        {
        iReport_err(s_sqlhEnv, true);  // Got an error.
        vFree_env_handle(); // Free the environment handle.
        }
      }
    else //if(!SQL_SUCCEEDED(s_sqlrSts))
      iReport_err(s_sqlhEnv, true);  // Got an error.
    }
  else
    s_sqlrSts = SQL_SUCCESS; // Database already connected.
  return(s_sqlrSts);
  }

/********************************************************************
sqlrDB_disconnect - DisConnect from the database.

  inputs: None.

  return: Return from ODBC functions.
********************************************************************/
SQLRETURN CDBAccessUpd::sqlrDB_disconnect(void)
  {

  if(s_sqlhDbc != NULL)
    {
    s_sqliHandleType = SQL_HANDLE_DBC;
    s_sqlrSts = SQLDisconnect(s_sqlhDbc);
    if(!SQL_SUCCEEDED(s_sqlrSts))
      iReport_err(s_sqlhDbc, false);  // Got an error.
    vFree_db_conn_handle(); // Free the database connection handle.
    }
  vFree_env_handle(); // Free the environment handle.
  m_bDBConnected = false;
  return(s_sqlrSts);
  }

/********************************************************************
vFree_env_handle - Free the environment handle.

  inputs: None.

  return: None.
********************************************************************/
void CDBAccessUpd::vFree_env_handle(void)
  {

  if(s_sqlhEnv != NULL)
    {
    s_sqliHandleType = SQL_HANDLE_ENV;
    s_sqlrSts = SQLFreeHandle(SQL_HANDLE_ENV, s_sqlhEnv);
    if(SQL_SUCCEEDED(s_sqlrSts))
      s_sqlhEnv = NULL;
    else
      iReport_err(s_sqlhEnv, false);  // Got an error.
    }
  }

/********************************************************************
vFree_db_conn_handle - Free the database connection handle.

  inputs: None.

  return: None.
********************************************************************/
void CDBAccessUpd::vFree_db_conn_handle(void)
  {

  if(s_sqlhDbc != NULL)
    {
    s_sqliHandleType = SQL_HANDLE_DBC;
    s_sqlrSts = SQLFreeHandle(SQL_HANDLE_DBC, s_sqlhDbc);
    if(SQL_SUCCEEDED(s_sqlrSts))
      s_sqlhDbc = NULL;
    else
      iReport_err(s_sqlhDbc, false);  // Got an error.
    }
  }

/********************************************************************
sqlhGet_stmt_handle - Get a database statement handle.

  inputs: None.

  return: Handle to statement.
********************************************************************/
SQLHANDLE CDBAccessUpd::sqlhGet_stmt_handle(void)
  {
  SQLHANDLE sqlhStmt;

  sqlhStmt = NULL;
  if(iDBConnect() == SUCCESS)
    {
    s_sqliHandleType = SQL_HANDLE_STMT;
    s_sqlrSts = SQLAllocHandle(SQL_HANDLE_STMT, s_sqlhDbc, &sqlhStmt); //&sqlhStmt);
    if(!SQL_SUCCEEDED(s_sqlrSts))
      { // Got an error, make sure the handle has been freed.
      iReport_err(sqlhStmt, true); //sqlhStmt);
      if(sqlhStmt != NULL)
        {
        SQLFreeHandle(SQL_HANDLE_STMT, sqlhStmt); //sqlhStmt);
        sqlhStmt = NULL;
        }
      }
    }
//  ((CEGGSASApp *)AfxGetApp())->vLog("sqlhGet_stmt_handle: sqlhStmt = %lu", (long)sqlhStmt);
  return(sqlhStmt);
  }

/********************************************************************
sqlrFree_stmt_handle - Free a database statement handle.

  inputs: Database handle.

  return: Status of freeing the handle.
********************************************************************/
SQLRETURN CDBAccessUpd::sqlrFree_stmt_handle(SQLHANDLE sqlhStmt)
  {
  SQLRETURN sqlrSts;

//  ((CEGGSASApp *)AfxGetApp())->vLog("sqlrFree_stmt_handle: sqlhStmt = %lu", (long)sqlhStmt);
  sqlrSts = SQLFreeHandle(SQL_HANDLE_STMT, sqlhStmt);
  return(sqlrSts);
  }

/********************************************************************
cstrGet_db_name - Get the name of the database.

  inputs: None.

  return: CString object containing the name of the database.
********************************************************************/
CString CDBAccessUpd::cstrGet_db_name(void)
  {
  CString cstrName;

  cstrName = m_cstrDSN;
  cstrName += ".mdb";
  return(cstrName);
  }

/********************************************************************
cstrGet_db_path - Get the path of the database.

  inputs: None.

  return: CString object containing the path of the database.
            The path does not contain a trailing backslash.
********************************************************************/
CString CDBAccessUpd::cstrGet_db_path(void)
  {
  CString cstrPath;
  int iIndex;

  cstrPath.Empty();
  if(m_bDBConnected == true) //if(iDBConnect() == SUCCESS)
    { // Already connected, so we have a path to the database.
      // Get the path out of the connection string that resulted from 
      // connecting to the database.
    cstrPath = s_sqlOutConnStr;
    if((iIndex = cstrPath.Find("DBQ=")) >= 0)
      {
      cstrPath.Delete(0, (iIndex + (int)strlen("DBQ=")));
      if((iIndex = cstrPath.Find(';')) >= 0)
        cstrPath.Delete(iIndex, cstrPath.GetLength() - iIndex);
      if((iIndex = cstrPath.ReverseFind('\\')) >= 0)
        cstrPath.Delete(iIndex, cstrPath.GetLength() - iIndex);
      }
    }
  if(cstrPath.IsEmpty() == TRUE)
    { // Not connected or didn't get a path to the database,
      // use the data path from the INI file.
    cstrPath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_DATAPATH, "");
    }
  return(cstrPath);
  }

/********************************************************************
iGet_string - Get a string value for the specified column from the current
              record retrieved in an SQL statement.

  inputs: Handle to the SQL statement used to get the record.
          Column in the database table to get the value for.
          Reference to a CString object that get the value.

  return: SUCCESS if value is retrieved from the record.
          FAIL if there is an error.
********************************************************************/
short int CDBAccessUpd::iGet_string(SQLHSTMT sqlhStmt, short int iColumn, CString &cstrData)
  {
  SQLINTEGER sqliSize;
  char szData[256];
  short int iRet = FAIL;

  szData[0] = NULL;
  if(iDBConnect() == SUCCESS)
    {
    if((s_sqlrSts = SQLGetData(sqlhStmt, iColumn, SQL_C_CHAR, szData, sizeof(szData), &sqliSize))
    == SQL_SUCCESS)
      {
      cstrData = szData;
      iRet = SUCCESS;
      }
    else if(s_sqlrSts == SQL_NO_DATA)
      {
      cstrData.Empty();
      iRet = SUCCESS;
      }
    if(iRet == FAIL)
      iReport_err(sqlhStmt, true);
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
short int CDBAccessUpd::iGet_bool(SQLHSTMT sqlhStmt, short iColumn, BOOL * pbValue)
  {
  short int iValue;

  s_sqlrSts = SQLGetData(sqlhStmt, iColumn, SQL_C_SHORT, &iValue, 0, 0);
  if(iValue != 0)
    *pbValue = TRUE;
  else
    *pbValue = FALSE;
  return(SUCCESS);
  }

/********************************************************************
iReport_err - Report a database error.  The message is obtained from data
              kept by the ODBC driver.

  inputs: Handle for the function that got the error.
          true if we want to display a message box, otherwise false.

  return: SUCCESS if the error should not stop processing.
          FAIL if the error should stop processing.
********************************************************************/
short CDBAccessUpd::iReport_err(SQLHANDLE sqlhHandle, bool bDisplayMsgBox)
  {
  SQLRETURN sqlrRet;
  SQLCHAR sqlszState[10], sqlszMsgText[SQL_ERR_MSG_LEN];
  SQLINTEGER sqliNativeErrorPtr;
  SQLSMALLINT sqliMsgLen;
  CString cstrErr, cstrMsg;
  short int iRet = FAIL;

  if(iDBConnect() == SUCCESS)
    {
    if(sqlhHandle != NULL)
      {
      sqlrRet = SQLGetDiagRec(s_sqliHandleType, sqlhHandle, 1, sqlszState,
                             &sqliNativeErrorPtr, sqlszMsgText, SQL_ERR_MSG_LEN, &sqliMsgLen);
      if(sqlrRet == SQL_NO_DATA)
        iRet = SUCCESS_NO_DATA;
      else
        {
        if(sqlrRet == SQL_ERROR)
          cstrMsg.LoadString(IDS_SQL_ERROR);
        else if(sqlrRet == SQL_INVALID_HANDLE)
          cstrMsg.LoadString(IDS_SQL_INVALID_HANDLE);
        else
          cstrMsg = sqlszMsgText;
      
        m_cstrErr.Format(IDS_FMT_DATABASE_ERROR, cstrMsg);
        theApp.vLog(m_cstrErr);
          // Error -1508 is column already exists.
        if(sqliNativeErrorPtr != -1508)
          {
          cstrErr.LoadString(IDS_ERROR);
          AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErr, MB_OK);
          }
        else
          {
          m_cstrInfo = cstrMsg;
          iRet = SUCCESS_WITH_INFO;
          }
        }
      }
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// Patient table functions


/********************************************************************
iUpdate_patient_100_101 - Update the patient database table from
                          version 1.00 to version 1.01

  inputs: None.

  output: Creates a new column in the table for the middle initial.

  return: SUCCESS if the table was updated.
          FAIL if there was an error.
********************************************************************/
short CDBAccessUpd::iUpdate_patient_100_101(void)
  {
  CString cstrSQL;
  short int iRet = FAIL;

  cstrSQL = "ALTER TABLE Patient ADD MiddleInitial VARCHAR(50) NULL";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
  return(iRet);
  }

/********************************************************************
iUpdate_patient_202_203 - Update from version 2.02 to version 2.03:
                            - Patient database table
                            - PatientInfo database table.
                          
 Process:
   - Create the UseResrchRefNum and ResearchRefNum fields in the
     Patient table.
   - For each patient in the Patient table, if the research reference
     number is being used, copy the new fields from the PatientInfo table
     to the Patient table.
   - Drop the UseResrchRefNum and ResearchRefNum fields from the
     PatientInfo table. Note that the index on the ResearchRefNum field
     must be dropped before the field is dropped.

  inputs: None.

  output: Creates 2 new columns in the Patient table for the research
            reference fields.
          Drops the 2 research reference columns from the PatientInfo table.

  return: SUCCESS if the operation was successful.
          FAIL if there was an error.
********************************************************************/
short CDBAccessUpd::iUpdate_patient_202_203(void)
  {
  short int iRet = FAIL;
  CString cstrSQL;
  long lPatientID;
  CString cstrLastName;
  CString cstrFirstName;
  CString cstrMI; // Middle initial
  BOOL bUseResrchRefNum;
	CString	cstrResearchRefNum;

    // Add Research reference fields.
  cstrSQL = "ALTER TABLE Patient ADD UseResrchRefNum BIT";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
  if(iRet == SUCCESS)
    {
    cstrSQL = "ALTER TABLE Patient ADD ResearchRefNum VARCHAR(50) NULL";
    if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(s_sqlrSts))
        iRet = SUCCESS;
      else
        iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(s_sqlhStmt);
      }
    }
  if(iRet == SUCCESS)
    { // Copy the research reference data from the patient info table.
      // for each patient, get patient ID, read record from patient info table
      // and write to patient table.
      // Get the first patient record.
    if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL = SQL_GET_PATIENT_ORDER;
      s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(s_sqlrSts))
        iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
      else
        {
        do
          {
          iRet = FAIL;
          s_sqlrSts = SQLFetch(s_sqlhStmt);
          if(!SQL_SUCCEEDED(s_sqlrSts))
            iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
          else
            {
            SQLGetData(s_sqlhStmt, 1, SQL_C_LONG, &lPatientID, 0, 0);
            iGet_string(s_sqlhStmt, 2, cstrLastName);
            iGet_string(s_sqlhStmt, 3, cstrFirstName);
            iGet_string(s_sqlhStmt, 4, cstrMI);
              // Now get the data from the patient info table.
            if((s_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
              {
              cstrSQL.Format(
                "SELECT UseResrchRefNum,ResearchRefNum from PatientInfo where PatientID=%lu",
                lPatientID);
              s_sqlrSts = SQLExecDirect(s_sqlhStmt1,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
              if(!SQL_SUCCEEDED(s_sqlrSts))
                iRet = iReport_err(s_sqlhStmt1, true); // SQLExecDirect() failed.
              else
                {
                s_sqlrSts = SQLFetch(s_sqlhStmt1);
                if(SQL_SUCCEEDED(s_sqlrSts))
                  {
                  iRet = SUCCESS;
                  iGet_bool(s_sqlhStmt1, 1, &bUseResrchRefNum);
                  iGet_string(s_sqlhStmt1, 2, cstrResearchRefNum);
                  }
                }
              sqlrFree_stmt_handle(s_sqlhStmt1);
              }
            }
          if(iRet == SUCCESS)
            { // Got all data, see if it needs to be transferred.
            if(bUseResrchRefNum == TRUE)
              { // Using research reference number.
                // Update the patient table and transfer the data.
              cstrSQL.Format(SQL_UPDATE_PATIENT, cstrLastName, cstrFirstName,
                             cstrMI, bUseResrchRefNum, cstrResearchRefNum,
                             lPatientID);
              if((s_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
                {
                s_sqlrSts = SQLExecDirect(s_sqlhStmt1,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
                if(SQL_SUCCEEDED(s_sqlrSts))
                  iRet = SUCCESS;
                else
                  iRet = iReport_err(s_sqlhStmt1, true); // SQLExecDirect() failed.
                sqlrFree_stmt_handle(s_sqlhStmt1);
                }
              }
            }
          } while(iRet == SUCCESS);
        }
      sqlrFree_stmt_handle(s_sqlhStmt);
      }
    }
  if(iRet == SUCCESS || iRet == SUCCESS_NO_DATA)
    { // Successfully moved the research reference fields.  Now delete the
      // fields from the PatientInfo table.
    cstrSQL = "ALTER TABLE PatientInfo DROP COLUMN UseResrchRefNum";
    if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(s_sqlrSts))
        iRet = SUCCESS;
      else
        iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(s_sqlhStmt);
      }
    if(iRet == SUCCESS)
      { // Drop the index on the column first, otherwise the column can't be dropped.
      cstrSQL = "DROP INDEX ResearchRefNum ON PatientInfo";
      if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(s_sqlrSts))
          iRet = SUCCESS;
        else
          iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
        sqlrFree_stmt_handle(s_sqlhStmt);
        }
      }
    if(iRet == SUCCESS)
      {
      cstrSQL = "ALTER TABLE PatientInfo DROP COLUMN ResearchRefNum";
      if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(s_sqlrSts))
          iRet = SUCCESS;
        else
          iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
        sqlrFree_stmt_handle(s_sqlhStmt);
        }
      }
    }
  return(iRet);
  }

  /********************************************************************
iUpdate_patient_203_204 - Update from version 2.03 to version 2.04:
                            - Add tables:
                              DiagnosisDesc, ICDCode, Recommendations
                            - Add fields to the StudyData table:
                              DiagKeyPhrase, ICDCode, RecommendationKeyPhrase
                          
  inputs: None.

  output: Create 3 new tables.
          Creates 3 new columns in the StudyData table

  return: SUCCESS if the operation was successful.
          FAIL if there was an error.
********************************************************************/
short CDBAccessUpd::iUpdate_patient_203_204(void)
    {
  short int iRet = FAIL;
  CString cstrSQL;

  cstrSQL = "CREATE TABLE DiagnosisDesc (DiagKeyPhrase varchar(50) PRIMARY KEY, DiagDesc MEMO)";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
  if(iRet == SUCCESS)
    {
    cstrSQL = "CREATE TABLE ICDCode (ICDCode varchar(50) PRIMARY KEY)";
    if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(s_sqlrSts))
        iRet = SUCCESS;
      else
        iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(s_sqlhStmt);
      }
    }
  if(iRet == SUCCESS)
    {
    cstrSQL = "CREATE TABLE Recommendations (KeyPhrase varchar(50) PRIMARY KEY, Description MEMO)";
    if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(s_sqlrSts))
        iRet = SUCCESS;
      else
        iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(s_sqlhStmt);
      }
    }
  if(iRet == SUCCESS)
    {
    cstrSQL = "ALTER TABLE StudyData ADD DiagKeyPhrase VARCHAR(50) NULL,\
              ICDCode VARCHAR(50) NULL,RecommendationKeyPhrase VARCHAR(50) NULL,\
              Gastroparesis BIT,DyspepsiaBloatSatiety BIT,Dyspepsia BIT,Anorexia BIT,\
              RefractoryGERD BIT,WeightLoss BIT,PostDilationEval BIT,EvalEffectMed BIT";
    if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(s_sqlrSts))
        iRet = SUCCESS;
      else
        iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(s_sqlhStmt);
      }
    }
  if(iRet == SUCCESS)
    { // Add Diagnosis key phrases and descriptions.
    iRet = iAdd_diag_key_phrase_desc("Pyloric Outlet Obstruction",
      "The findings demonstrate elevated 3 cycle per minute activity that remains elevated or increases after water load. This finding suggests mechanical or functional pyloric outlet obstruction after a standard water load.");
    if(iRet == SUCCESS)
      iRet = iAdd_diag_key_phrase_desc("Bradygastria",
        "The findings demonstrate poor 3 cycle per minute activity and predominant activity in the 1-2.5 cycle per minute range after a standard water load.");
    if(iRet == SUCCESS)
      iRet = iAdd_diag_key_phrase_desc("Tachygastria ",
        "The findings demonstrate poor 3 cycle per minute activity and predominant activity in the 3.5-10 cycle per minute range after a standard water load.");
    if(iRet == SUCCESS)
      iRet = iAdd_diag_key_phrase_desc("Mixed Disorder",
        "The findings demonstrate poor 3 cycle per minute activity and a series of dysrhythmias that include a mix of predominant activity in the 1-2.5 and 3.5-10 cycle per minute range after a standard water load.");
    if(iRet == SUCCESS)
      iRet = iAdd_diag_key_phrase_desc("Normal",
        "The EGG study is normal with the predominant activity in the 2.3-3.5 cycle per minute range after a standard water load.");
    }
  if(iRet == SUCCESS)
    { // Add ICD codes.
    iRet = iAdd_ICD_code("Gastroparesis/536.3");
    if(iRet == SUCCESS)
      iRet = iAdd_ICD_code("Bradygastria/536.0");
    if(iRet == SUCCESS)
      iRet = iAdd_ICD_code("Pyloric outlet obstruction/536.0");
    if(iRet == SUCCESS)
      iRet = iAdd_ICD_code("Tachygastria/536.0");
    if(iRet == SUCCESS)
      iRet = iAdd_ICD_code("Mixed/nonspecific/536.0");
    if(iRet == SUCCESS)
      iRet = iAdd_ICD_code("Bloating and satiety/536.80");
    if(iRet == SUCCESS)
      iRet = iAdd_ICD_code("Dyspepsia/536.80");
    if(iRet == SUCCESS)
      iRet = iAdd_ICD_code("Nausea/787.02");
    if(iRet == SUCCESS)
      iRet = iAdd_ICD_code("Nausea and vomiting/787.01");
    if(iRet == SUCCESS)
      iRet = iAdd_ICD_code("Anorexia/783.0");
    }
  return(iRet);
    }

  /********************************************************************
iAdd_diag_key_phrase_desc - Add a diagnosis key phrase and a description
                            to the DiagnosisDesc table.

                            Version 2.04
                          
  inputs: Pointer to a text buffer containing the key phrase.
          Pointer to a text buffer containing the description.

  output: Add a diagnosis key phrase and a description to the DiagnosisDesc table.

  return: SUCCESS if the operation was successful.
          FAIL if there was an error.
********************************************************************/
short CDBAccessUpd::iAdd_diag_key_phrase_desc(char * pszKeyPhrase, char * pszDesc)
    {
  short int iRet = FAIL;
  CString cstrSQL;

  cstrSQL.Format("INSERT INTO DiagnosisDesc (DiagKeyPhrase,DiagDesc) VALUES ('%s','%s')",
    pszKeyPhrase, pszDesc);
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
  return(iRet);
    }

  /********************************************************************
iAdd_ICD_code - Add an ICD code to the ICDCode table.

                Version 2.04
                          
  inputs: Pointer to a text buffer containing the ICD code.

  output: Add an ICD code to the ICDCode table.

  return: SUCCESS if the operation was successful.
          FAIL if there was an error.
********************************************************************/
short CDBAccessUpd::iAdd_ICD_code(char * pszCode)
    {
  short int iRet = FAIL;
  CString cstrSQL;

  cstrSQL.Format("INSERT INTO ICDCode (ICDCode) VALUES ('%s')", pszCode);
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
  return(iRet);
    }

  /********************************************************************
iUpdate_patient_204_205 - Update from version 2.04 to version 2.05:
                            - Add fields to the Patient table: Hide
                            - Add fields to the PatientInfo table:
                              Male, Female
                          
  inputs: None.

  output: Creates new columns in the Patient and PatientInfo tables.

  return: SUCCESS if the operation was successful.
          FAIL if there was an error.
********************************************************************/
short CDBAccessUpd::iUpdate_patient_204_205(void)
  {
  short int iRet = FAIL;
  CString cstrSQL;

    // Add Research reference fields.
  cstrSQL = "ALTER TABLE Patient ADD Hide BIT";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
  if(iRet == SUCCESS)
    {
    cstrSQL = "ALTER TABLE PatientInfo ADD Male BIT,Female BIT";
    if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(s_sqlrSts))
        iRet = SUCCESS;
      else
        iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(s_sqlhStmt);
      }
    }

  return(iRet);
  }

/********************************************************************
iUpdate_patient_205_206 - Update from version 2.05 to version 2.06:
                            - Add fields to the StudyData table: Frequency
                            - Add fields to the WhatIf table: Frequency
                          
  inputs: None.

  output: Creates new columns in the StudyData and WhatIf tables.

  return: SUCCESS if the operation was successful.
          FAIL if there was an error.
********************************************************************/
short CDBAccessUpd::iUpdate_patient_205_206(void)
  {
  short int iRet = FAIL;
  CString cstrSQL;

    // Add Research reference fields.
  cstrSQL = "ALTER TABLE StudyData ADD Frequency NUMBER";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
  if(iRet == SUCCESS)
    {
    cstrSQL = "ALTER TABLE WhatIf ADD Frequency NUMBER";
    if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(s_sqlrSts))
        iRet = SUCCESS;
      else
        iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(s_sqlhStmt);
      }
    }

  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// PatientInfo table functions


/////////////////////////////////////////////////////////////////////
//// StudyData table functions


/********************************************************************
iUpdate_study_100_101 - Update the StudyData database table from
                          version 1.00 to version 1.01

  inputs: None.

  output: Creates a new column in the table for the delay gastric
            emptying check box in pre-study information.

  return: SUCCESS if the table was updated.
          FAIL if there was an errorl
********************************************************************/
int CDBAccessUpd::iUpdate_study_100_101(void)
  {
  CString cstrSQL;
  short int iRet = FAIL;

  cstrSQL = "ALTER TABLE StudyData ADD DelayedGastricEmptying BIT";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
  return(iRet);
  }

/********************************************************************
iUpdate_study_101_200 - Update the StudyData database table from
                          version 1.01 to version 1.00

                        Change Version Number To 2.00
                        New database fields:
                          BLDataPoints - Number, long integer
                          Pause - Yes/No
                          PostStimDataPoints - Number, long integer
                          StimulationMedium - memo
                        Changed database fields:
                          PeriodStart - length = 255
                          PeriodLen - length = 255
                          StartMinutes - length = 255
                          EndMinutes - length = 255

   inputs: None.

  output: Creates a new column in the table for the delay gastric
            emptying check box in pre-study information.

  return: SUCCESS if the table was updated.
          FAIL if there was an errorl
********************************************************************/
int CDBAccessUpd::iUpdate_study_101_200(void)
  {
  CString cstrSQL;
  short int iRet = FAIL;

    // Change PeriodLen column to be 255 characters.
  cstrSQL = "ALTER TABLE StudyData ALTER COLUMN PeriodLen VARCHAR(255)";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
    // Change PeriodStart column to be 255 characters.
  cstrSQL = "ALTER TABLE StudyData ALTER COLUMN PeriodStart VARCHAR(255)";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
    // Change StartMinutes column to be 255 characters.
  cstrSQL = "ALTER TABLE StudyData ALTER COLUMN StartMinutes VARCHAR(255)";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
    // Change EndMinutes column to be 255 characters.
  cstrSQL = "ALTER TABLE StudyData ALTER COLUMN EndMinutes VARCHAR(255)";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
    // Add BLDataPoints column, number, long integer
  cstrSQL = "ALTER TABLE StudyData ADD BLDataPoints INTEGER";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
    // Add Pause column, Yes/No
  cstrSQL = "ALTER TABLE StudyData ADD Pause BIT";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
    // Add PostStimDataPoints column, number, long integer
  cstrSQL = "ALTER TABLE StudyData ADD PostStimDataPoints INTEGER";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
    // Add StimulationMedium column, memo
  cstrSQL = "ALTER TABLE StudyData ADD StimulationMedium MEMO";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
  return(iRet);
  }

/********************************************************************
iUpdate_study_201_202 - Update the StudyData database table from
                          version 2.01 to version 2.02

                        Changed database fields:
                          RespirationRate - length = 255

   inputs: None.

  output: Changes the field width for RespirationRate.

  return: SUCCESS if the table was updated.
          FAIL if there was an errorl
********************************************************************/
int CDBAccessUpd::iUpdate_study_201_202(void)
  {
  CString cstrSQL;
  short int iRet = FAIL;

    // Change RespirationRate column to be 255 characters.
  cstrSQL = "ALTER TABLE StudyData ALTER COLUMN RespirationRate VARCHAR(255)";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// Events table functions


/********************************************************************
iAdd_event_table_201 - Add the Events table.  This makes it version 2.01.

                       Add Events table with following fields:
                         EventID: Key, AutoNumber
                         StudyID: Long Integer
                         DataPoint: Long Integer
                         Seconds: Long Integer
                         Type: Text, 50
                         PeriodType: Integer
                         Description: Memo
   inputs: None.

  output: Creates a new table

  return: SUCCESS if the table was created.
          FAIL if there was an errorl
********************************************************************/
short CDBAccessUpd::iAdd_event_table_201(void)
  {
  CString cstrSQL;
  short int iRet = FAIL;

  cstrSQL = "CREATE TABLE Events (EventID AutoIncrement PRIMARY KEY, \
StudyID INTEGER, DataPoint INTEGER, Seconds INTEGER, Type varchar(50) NULL, \
PeriodType INTEGER, Description MEMO)";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
  return(iRet);
  }

/********************************************************************
iAdd_whatif_table_202 - Add the WhatIf table.  This makes it version 2.02.

                       Add WhatIf table with following fields:
                         WhatIf: Key, AutoNumber
                         StudyID: Long Integer
                         Description: Text, 50
                         DateTimeSaved: Text, 50
                         DateTimeSaved: Text, 50
                         NumPeriods: Long Integer
                         PeriodStart: Text, 255
                         PeriodLen: Text, 255
                         StartMinutes: Text, 255
                         EndMinutes: Text, 255
                         RespirationRate: Text, 255

   inputs: None.

  output: Creates a new table.

  return: SUCCESS if the table was createde.
          FAIL if there was an error.
********************************************************************/
short CDBAccessUpd::iAdd_whatif_table_202(void)
  {
  CString cstrSQL;
  short int iRet = FAIL;

  cstrSQL = "CREATE TABLE WhatIf (WhatIfID AutoIncrement PRIMARY KEY, \
StudyID INTEGER, Description varchar(50) NULL, DateTimeSaved varchar(50) NULL,\
NumPeriods INTEGER, PeriodStart varchar(255) NULL, PeriodLen varchar(255) NULL, \
StartMinutes varchar(255) NULL, EndMinutes varchar(255) NULL, \
RespirationRate varchar(255) NULL)";
  if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
    {
    s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
    if(SQL_SUCCEEDED(s_sqlrSts))
      iRet = SUCCESS;
    else
      iRet = iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
    sqlrFree_stmt_handle(s_sqlhStmt);
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// Facility table functions

/////////////////////////////////////////////////////////////////////
//// Version table functions


/********************************************************************
iGet_version - Get the version from the Version table.
               Version number composition:
                 Major.minor

  inputs: Reference to a CString object that gets the version.

  output: None.

  return: SUCCESS if the table was updated.
          FAIL if there was an errorl
********************************************************************/
short CDBAccessUpd::iGet_version(CString & cstrVersion)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL = SQL_GET_VERSION;
      s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(s_sqlrSts))
        { // Now get all the values.
        s_sqlrSts = SQLFetch(s_sqlhStmt);
        if(SQL_SUCCEEDED(s_sqlrSts))
          {
          iRet = SUCCESS;
          iGet_string(s_sqlhStmt, 1, cstrVersion);
          }
        else if(s_sqlrSts != SQL_NO_DATA)
          iReport_err(s_sqlhStmt, true);
        }
      else
        iReport_err(s_sqlhStmt, true); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(s_sqlhStmt);
      }
    else
      iReport_err(s_sqlhStmt, true); // sqlhGet_stmt_handle() failed.
    }
  return(iRet);
  }

/********************************************************************
iGet_major_version - Gets the major version number from the full version.

  inputs: CString containing the full version.

  output: None.

  return: Major version number
********************************************************************/
short CDBAccessUpd::iGet_major_version(CString cstrVersion)
  {
  short int iVersion;

  iVersion = atoi(cstrVersion);
  return(iVersion);
  }

/********************************************************************
iGet_minor_version - Gets the minor version number from the full version.

  inputs: CString containing the full version.

  output: None.

  return: Minor version number
********************************************************************/
short CDBAccessUpd::iGet_minor_version(CString cstrVersion)
  {
  CString cstrMinor;
  short int iVersion, iIndex;

  if((iIndex = cstrVersion.Find(".")) >= 0)
    cstrMinor = cstrVersion.Right(cstrVersion.GetLength() - (iIndex + 1));
  iVersion = atoi(cstrMinor);
  return(iVersion);
  }

/********************************************************************
iUpdate_version - Write the new version into the Version table.

  inputs: None.

  output: Changes the version table.

  return: SUCCESS if the table was updated.
          FAIL if there was an errorl
********************************************************************/
int CDBAccessUpd::iUpdate_version(void)
  {
  CString cstrSQL, cstrVersion;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    cstrVersion.Format("%d.%02d", DB_VERSION_MAJOR, DB_VERSION_MINOR);
    cstrSQL.Format(SQL_UPDATE_VERSION, cstrVersion);
    if((s_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      s_sqlrSts = SQLExecDirect(s_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(s_sqlrSts))
        iReport_err(s_sqlhStmt, true);
      else
        iRet = SUCCESS;
      sqlrFree_stmt_handle(s_sqlhStmt);
      }
    else
      iReport_err(s_sqlhStmt, true);
    }
  return(iRet);
  }

