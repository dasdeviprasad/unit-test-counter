/********************************************************************
Dbaccess.cpp

Copyright (C) 2003,2004,2005,2007,2008,2009,2012,2013,2016,2018 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CDbaccess class.

  This class provides access to the Microsoft Access database via
  SQL and an ODBC driver.


HISTORY:
08-NOV-03  RET  New.
20-FEB-04  RET  Add method iGet_bool() to get BOOLEAN values from the database.
                Changed all methods that get boolean values from the database
                  to use this new method.
10-FEB-05  RET  Change SQL statements for Patient name table to include
                  middle initial:
                    SQL_INSERT_PATIENT, SQL_UPDATE_PATIENT,
                    SQL_INSERT_STUDY_DATA, SQL_UPDATE_STUDY_DATA
                  Change the following methods:
                    iSave_study_data(), iGet_study_data(),
                    iGet_patient(), iSave_patient(), iFind_next_patient()
                Add new defines for SQL statements:
                  SQL_GET_PATIENT_ORDER, SQL_FIND_PATIENT
                Add new method iGet_bit().
12-MAY-07  RET  Research Version
                  Add new database fields: number of points in baseline,
                    pause indicator, number of post-stimulation data points,
                    Stimulation Medium
                      Change SQL_INSERT_STUDY_DATA and SQL_UPDATE_STUDY_DATA
                      Change iGet_study_data(), iSave_study_data()
                   Change iFind_next_study() to add a Pause parameter.
                   Add overload of lFind_study_data().
                   Add Event table access methods.
05-FEB-08  RET  Version 2.03
                  Add WhatIf table and the following methods to access it.
                    iFind_next_whatif(), iSave_whatif_data(), iFind_whatif(),
                    iGet_whatif_data(), iDelete_whatif()
30-JUN-08 RET  Version 2.05
                 Add static variable m_sqlhStmt2.
                 Change iFind_next_whatif() to use m_sqlhStmt2.
                 Change iFind_next_study() to get the study ID.
28-AUG-08  RET  Version 2.05c
                  Move UseResrchRefNum and ResearchRefNum fields from
                    PatientInfo table to Patient table so that research
                    reference numbers are displayed in alphabetical order.
                    Change SQL statements:
                      SQL_INSERT_PATIENT, SQL_UPDATE_PATIENT, SQL_GET_PATIENT_ORDER,
                      SQL_INSERT_PATIENT_INFO, SQL_UPDATE_PATIENT_INFO,
                      SQL_FIND_PATIENT
                    Change methods:
                      iGet_patient(), iSave_patient(), iFind_next_patient(),
                      iSave_patient_info(), iGet_patient_info(),
                      iFind_patient_name()

                 
//// REMOTE DB
                 Implement using databases from other places.
                   Add method cstrGet_db_file_path().
                   Change method sqlrDB_connect() to add the path and file
                     name to the connection string.
                   Change method iGet_study_data() to put in the correct path
                     for the data files based on the path used to connect to
                     the database.
//// END OF REMOTE DB
07-N0V-08  RET  Version 2.08a
                  Modifications for the Reader program version.
                    Move all static data to class object.
                    Modify sqlrDB_connect() to connect with either the local
                      database or the reader database.
                    Add overloaded method: iGet_patient_info(), iSave_patient_info(),
                      iGet_study_data(), iSave_study_data(), iFind_next_whatif(),
                      iSave_whatif_data(), iGet_event(), iSave_event()
                    Add new method: iFind_study(), iDelete_whatif_for_study(),
                    Add SQL statement define SQL_WHATIF_DELETE_FOR_STUDY
                    Change iFind_patient_name(), iFind_next_study()
                      to return SUCCESS_NO_DATA if a patient is not found
                      and there were no database errors.
12-DEC-08  RET  Version R.2.08b
                  Change sqlrDB_connect() to make sure the database file exists
                    before connecting to it.
                  Add method bDBFileExists().
16-JAN-09  RET  Version CDDRdr.2.08e
                  Change iGet_num_study_records() to return SUCCESS after
                    counting records.
13-FEB-09  RET  Version 2.08i
                  Add method prototypes: iGet_version(), iValidate_version().
22-MAR-09  RET  Version 2.08k
                  Add methods: iGet_next_data_file(), iGet_study_count().
15-JUL-09  RET  Version 2.08m
                  Add method cstrFormat() to reformat all strings being put in
                    the database.  Initially it is to add an apostrophe be an
                    existing apostrophe.
18-FEB-12  RET  Version 
                  Changes for foreign languages.
27-AUG-12  RET
             Implement new database table DiagnosisDesc
               Add defines: SQL_INSERT_DIAGNOSIS_DESC, SQL_UPDATE_DIAGNOSIS_DESC,
                 SQL_GET_DIAGNOSIS_DESC_ORDER, SQL_FIND_DIAGNOSIS_DESC,
                 SQL_DELETE_DIAGNOSIS_DESC
               Add methods: iSave_diagnosis_desc(), iGet_diagnosis_desc(),
                 iFind_diagnosis_desc(), iDelete_diagnosis_desc(),
                 iUpdate_diagnosis_desc()
               Change tables: StudyData
               Change defines: SQL_UPDATE_STUDY_DATA, SQL_UPDATE_STUDY_DATA_1,
                 SQL_INSERT_STUDY_DATA_1, SQL_INSERT_STUDY_DATA
31-AUG-12  RET
             Implement new database table for ICDCodes.
               Add defines: SQL_INSERT_ICD_CODE, SQL_GET_ICD_CODE_ORDER,
                 SQL_FIND_ICD_CODE, SQL_DELETE_ICD_CODE, SQL_UPDATE_ICD_CODE
               Add methods: iSave_ICDCode(), iUpdate_ICDCode(), iGet_ICDCode(),
                 iFind_ICDCode(), iDelete_ICDCode()
               Change tables: StudyData
               Change defines: SQL_UPDATE_STUDY_DATA, SQL_UPDATE_STUDY_DATA_1,
                 SQL_INSERT_STUDY_DATA_1, SQL_INSERT_STUDY_DATA
02-SEP-12  RET
             Implement new database table for Recommendations.
               Add defines: SQL_INSERT_RECOMMENDATIONS, SQL_UPDATE_RECOMMENDATIONS,
                 SQL_GET_RECOMMENDATIONS_ORDER, SQL_FIND_RECOMMENDATIONS,
                 SQL_DELETE_RECOMMENDATIONS
               Add methods: iSave_recomm_desc(), iUpdate_recommendation(),
                 iGet_recomm_desc(), iFind_recommendation(), iDelete_recommendation()
               Change tables: StudyData
               Change defines: SQL_UPDATE_STUDY_DATA, SQL_UPDATE_STUDY_DATA_1,
                 SQL_INSERT_STUDY_DATA_1, SQL_INSERT_STUDY_DATA
13-SEP-12  RET
             Add new indications to the StudyData table.
               Change defines: SQL_UPDATE_STUDY_DATA, SQL_UPDATE_STUDY_DATA_1,
                 SQL_INSERT_STUDY_DATA_1, SQL_INSERT_STUDY_DATA
               Change methods: iGet_study_data(), iSave_study_data()
25-FEB-13  RET
             Add Demographics checkboxes for Male and Female.
               Add to PatientInfo database table.
                 Change defines: SQL_INSERT_PATIENT_INFO, SQL_UPDATE_PATIENT_INFO
                 Change methods: iSave_patient_info(), iGet_patient_info()
             Add Hide patient.
               Add to Patient databae table.
                 Change defines: SQL_INSERT_PATIENT, SQL_UPDATE_PATIENT, SQL_GET_PATIENT_ORDER
                 Add defines: SQL_GET_PATIENT_ORDER_HIDE
                 Change methods: iSave_patient(), iFind_next_patient()
13-MAR-13  RET
             Add Frequency field to the Study table and the WhatIF table.
               Change defines: SQL_INSERT_STUDY_DATA, SQL_INSERT_STUDY_DATA_1,
                 SQL_UPDATE_STUDY_DATA, SQL_UPDATE_STUDY_DATA_1,
                 SQL_WHATIF_INSERT_RECORD, SQL_WHATIF_UPDATE_RECORD
               Change methods: iGet_study_data(), iSave_study_data(),
                 iFind_next_whatif(), iSave_whatif_data(), iGet_whatif_data()
02-JUN-16  RET
             Add feature to hide study dates.
               Add method: iEncode_study_dates()
20-JAN-18  RET
             Provide search criteria for listing patients.
               Change methods: iFind_next_patient(), iFind_next_study()
               Add methods: bLook_for_study_date(), ctMake(), vParse_date()
               Add variables: m_cstrSearchSQL, m_iDateSrchType, m_ctSrchDate,
                 m_cstrSrchDate
               Add defines: DB_SRCH_NONE, DB_SRCH_GREATER_THAN, DB_SRCH_LESS_THAN,
                 DB_SRCH_EQUAL_TO


select * from smbprotocol where Description like 'S%' or Description like 'T%'
select * from LoadingSchedule where StartDate > '2015-06-30 13:50:00'
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"

#include "util.h"
#include "Dbaccess.h"
#include ".\dbaccess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SQL_LEN 100 // Max length of SQL statement.
#define SQL_ERR_MSG_LEN 512 // Max length of SQL error message.

// SQL statements for Version table.
#define SQL_GET_VERSION "SELECT * from Version"

// SQL statements for Patient table.
#define SQL_INSERT_PATIENT \
"INSERT INTO Patient (LastName,FirstName,MiddleInitial,UseResrchRefNum,ResearchRefNum,Hide) \
VALUES ('%s','%s','%s',%d,'%s',%d)"

#define SQL_UPDATE_PATIENT \
"UPDATE Patient set LastName='%s',FirstName='%s',MiddleInitial='%s',\
UseResrchRefNum=%d,ResearchRefNum='%s',Hide=%d where PatientID=%lu"

#define SQL_GET_PATIENT \
"SELECT * from Patient where PatientID=%lu"

#define SQL_GET_PATIENT_ORDER \
"Select PatientID,LastName,FirstName,MiddleInitial,UseResrchRefNum,ResearchRefNum,Hide \
from Patient ORDER by LastName,FirstName,MiddleInitial,ResearchRefNum ASC"

#define SQL_GET_PATIENT_ORDER_HIDE \
"Select PatientID,LastName,FirstName,MiddleInitial,UseResrchRefNum,ResearchRefNum,Hide \
from Patient where Hide=%d ORDER by LastName,FirstName,MiddleInitial,ResearchRefNum ASC"

#define SQL_FIND_PATIENT \
"Select PatientID from Patient where LastName='%s' AND FirstName='%s' \
AND MiddleInitial='%s' AND ResearchRefNum='%s'"

// SQL statements for PatientInfo table.
#define SQL_INSERT_PATIENT_INFO \
"INSERT INTO PatientInfo (PatientID,Address1,Address2,City,State,ZipCode,Phone,DateOfBirth,\
HospitalNumber,SocialSecurityNum,Male,Female) \
VALUES (%lu,'%s','%s','%s','%s','%s','%s','%s','%s','%s',%d,%d)"

#define SQL_UPDATE_PATIENT_INFO \
"UPDATE PatientInfo set Address1='%s',Address2='%s',City='%s',\
State='%s',ZipCode='%s',Phone='%s',DateOfBirth='%s',\
HospitalNumber='%s',SocialSecurityNum='%s',Male=%d,Female=%d where PatientID=%lu"

#define SQL_GET_PATIENT_INFO \
"SELECT * from PatientInfo where PatientID=%lu"

// SQL statements for StudyData table.
#define SQL_INSERT_STUDY_DATA \
"INSERT INTO StudyData (PatientID,DataFile,DateOfStudy,MealTime,WaterAmount,\
WaterAmountUnits,PeriodStart,PeriodLen,StartMinutes,EndMinutes,TestMaxSignal,\
TestMinSignal,Extra10Min,SuggestedDiagnosis,Bloating,EarlySateity,Eructus,Gerd,\
Nausea,PostPrandialFullness,Vomiting,AttendingPhysician,ReferingPhysician,\
Technician,ManualMode,RespirationRate,UserDiagnosis,IFPComments,Diagnosis,\
Recommendations,Medications,DelayedGastricEmptying,BLDataPoints,Pause,\
PostStimDataPoints,StimulationMedium,DiagKeyPhrase,ICDCode,RecommendationKeyPhrase,\
Gastroparesis,DyspepsiaBloatSatiety,Dyspepsia,Anorexia,\
RefractoryGERD,WeightLoss,PostDilationEval,EvalEffectMed,Frequency) \
VALUES (%lu,'%s','%s','%s',%d,'%s','%s','%s','%s','%s',%f,%f,%d,'%s',%d,%d,%d,%d,%d,%d,%d,\
'%s','%s','%s',%d,'%s',%d,'%s','%s','%s','%s',%d,%lu,%d,%lu,'%s','%s','%s','%s',\
%d,%d,%d,%d,%d,%d,%d,%d,%d)"

#define SQL_INSERT_STUDY_DATA_1 \
"INSERT INTO StudyData (PatientID,DataFile,DateOfStudy,MealTime,WaterAmount,\
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
%d,%d,%d,%d,%d,%d,%d,%d,%d)"

#define SQL_UPDATE_STUDY_DATA \
"UPDATE StudyData set DataFile='%s',DateOfStudy='%s',MealTime='%s',WaterAmount=%u,\
WaterAmountUnits='%s',PeriodStart='%s',PeriodLen='%s',StartMinutes='%s',EndMinutes='%s',\
TestMaxSignal=%f,TestMinSignal=%f,Extra10Min=%d,SuggestedDiagnosis='%s',\
Bloating=%d,EarlySateity=%d,Eructus=%d,Gerd=%d,Nausea=%d,PostPrandialFullness=%d,\
Vomiting=%d,AttendingPhysician='%s',ReferingPhysician='%s',\
Technician='%s',ManualMode=%d,RespirationRate='%s',UserDiagnosis=%d,IFPComments='%s',\
Diagnosis='%s',Recommendations='%s',Medications='%s',DelayedGastricEmptying=%d,\
BLDataPoints=%lu,Pause=%d,PostStimDataPoints=%lu,StimulationMedium='%s',\
DiagKeyPhrase='%s',ICDCode='%s',RecommendationKeyPhrase='%s',\
Gastroparesis=%d,DyspepsiaBloatSatiety=%d,Dyspepsia=%d,Anorexia=%d,\
RefractoryGERD=%d,WeightLoss=%d,PostDilationEval=%d,EvalEffectMed=%d,Frequency=%d where StudyID=%lu"

#define SQL_UPDATE_STUDY_DATA_1 \
"UPDATE StudyData set DataFile='%s',DateOfStudy='%s',MealTime='%s',WaterAmount=%u,\
WaterAmountUnits='%s',PeriodStart='%s',PeriodLen='%s',StartMinutes='%s',EndMinutes='%s',\
TestMaxSignal=%d,TestMinSignal=%d,Extra10Min=%d,SuggestedDiagnosis='%s',\
Bloating=%d,EarlySateity=%d,Eructus=%d,Gerd=%d,Nausea=%d,PostPrandialFullness=%d,\
Vomiting=%d,AttendingPhysician='%s',ReferingPhysician='%s',\
Technician='%s',ManualMode=%d,RespirationRate='%s',UserDiagnosis=%d,IFPComments='%s',\
Diagnosis='%s',Recommendations='%s',Medications='%s',DelayedGastricEmptying=%d,\
BLDataPoints=%lu,Pause=%d,PostStimDataPoints=%lu,StimulationMedium='%s',\
DiagKeyPhrase='%s',ICDCode='%s',RecommendationKeyPhrase='%s',\
Gastroparesis=%d,DyspepsiaBloatSatiety=%d,Dyspepsia=%d,Anorexia=%d,\
RefractoryGERD=%d,WeightLoss=%d,PostDilationEval=%d,EvalEffectMed=%d,Frequency=%d where StudyID=%lu"

#define SQL_GET_STUDY_DATA \
"SELECT * from StudyData where PatientID=%lu and DateOfStudy='%s'"

#define SQL_GET_STUDY_DATA_FOR_ID \
"SELECT * from StudyData where StudyID=%lu"

#define SQL_GET_NEXT_DIAGNOSIS \
"Select StudyID,DateOfStudy,UserDiagnosis,Diagnosis,SuggestedDiagnosis from StudyData \
where PatientID=%lu ORDER by DateOfStudy ASC"

#define SQL_GET_NEXT_MEDICATION \
"Select StudyID,DateOfStudy,Medications from StudyData \
where PatientID=%lu ORDER by DateOfStudy ASC"

#define SQL_UPDATE_MEDICATION \
"UPDATE StudyData set Medications='%s' where StudyID=%lu"

// SQL statements for Facility table.
#define SQL_GET_FACILITY_INFO \
"SELECT * from Facility"

#define SQL_GET_FACILITY_INFO_ID \
"SELECT * from Facility where FacilitID=%lu"

#define SQL_INSERT_FACILITY_INFO \
"INSERT INTO Facility (FacilityName,Address1,Address2,City,State,ZipCode,Phone) \
VALUES ('%s','%s','%s','%s','%s','%s','%s')"

#define SQL_UPDATE_FACILITY_INFO \
"UPDATE Facility set FacilityName='%s',Address1='%s',Address2='%s',\
City='%s',State='%s',ZipCode='%s',Phone='%s' where FacilityID=%lu"

// SQL statements for Events table.
#define SQL_INSERT_EVENTS \
"INSERT INTO Events (StudyID,DataPoint,Seconds,Type,PeriodType,Description) \
VALUES (%lu,%lu,%lu,'%s',%d,'%s')"

#define SQL_UPDATE_EVENTS \
"UPDATE Events set Seconds=%lu,Type='%s',PeriodType=%d,Description='%s' \
where StudyID=%lu and DataPoint=%lu"

#define SQL_FIND_EVENT \
"SELECT Seconds from Events where StudyID=%lu and DataPoint=%d and PeriodType = %d"

#define SQL_GET_EVENTS \
"SELECT * from Events where StudyID=%lu ORDER by Seconds ASC"

#define SQL_DELETE_EVENTS \
"DELETE FROM Events where StudyID=%lu"

#define SQL_DELETE_EVENT \
"DELETE FROM Events where StudyID=%lu and DataPoint=%d and PeriodType = %d"

// SQL statements for WhatIf table.
#define SQL_WHATIF_GET_DESC_ID \
"SELECT WhatIfID,DateTimeSaved,Description from WhatIf where StudyID=%lu \
ORDER by DateTimeSaved ASC"

#define SQL_WHATIF_GET_STUDY_ID \
"SELECT * from WhatIf where StudyID=%lu"

#define SQL_WHATIF_GET_RECORD \
"SELECT * from WhatIf where WhatIfID=%lu"

#define SQL_WHATIF_INSERT_RECORD \
"INSERT INTO WhatIf (StudyID,Description,DateTimeSaved,NumPeriods,PeriodStart,PeriodLen,\
StartMinutes,EndMinutes,RespirationRate,Frequency) \
VALUES (%lu,'%s','%s',%d,'%s','%s','%s','%s','%s',%d)"

#define SQL_WHATIF_UPDATE_RECORD \
"UPDATE WhatIf set StudyID=%lu,Description='%s',DateTimeSaved='%s',NumPeriods=%d,\
PeriodStart='%s',PeriodLen='%s',StartMinutes='%s',EndMinutes='%s',\
RespirationRate='%s',Frequency=%d where WhatIfID=%lu"

#define SQL_WHATIF_FIND \
"SELECT WhatIfID from WhatIf where WhatIfID=%lu"

#define SQL_WHATIF_DELETE \
"DELETE FROM WhatIf where WhatIfID=%lu"

#define SQL_WHATIF_DELETE_FOR_STUDY \
"DELETE FROM WhatIf where StudyID=%lu"

/////////////////////////////////////////////////////////////////////
//// SQL for accessing the DiagnosisDesc table.
#define SQL_INSERT_DIAGNOSIS_DESC \
"INSERT INTO DiagnosisDesc (DiagKeyPhrase,DiagDesc) VALUES ('%s','%s')"

#define SQL_UPDATE_DIAGNOSIS_DESC \
"UPDATE DiagnosisDesc set DiagDesc='%s',DiagKeyPhrase='%s' where DiagKeyPhrase='%s'"

#define SQL_GET_DIAGNOSIS_DESC_ORDER \
"Select DiagKeyPhrase,DiagDesc from DiagnosisDesc ORDER by DiagKeyPhrase";

#define SQL_FIND_DIAGNOSIS_DESC "SELECT * from DiagnosisDesc where DiagKeyPhrase='%s'"

#define SQL_DELETE_DIAGNOSIS_DESC "DELETE FROM DiagnosisDesc where DiagKeyPhrase='%s'"

//// End of SQL for accessing the DiagnosisDesc table.
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//// SQL for accessing the ICDCode table.
#define SQL_INSERT_ICD_CODE "INSERT INTO ICDCode (ICDCode) VALUES ('%s')"

//#define SQL_UPDATE_ICD_CODE \
//"UPDATE ICDCode set ICDCodeDesc='%s' where ICDCode='%s'"

#define SQL_GET_ICD_CODE_ORDER "Select ICDCode from ICDCode ORDER by ICDCode";

#define SQL_FIND_ICD_CODE "SELECT * from ICDCode where ICDCode='%s'"

#define SQL_DELETE_ICD_CODE "DELETE FROM ICDCode where ICDCode='%s'"

#define SQL_UPDATE_ICD_CODE "UPDATE ICDCode set ICDCode='%s' where ICDCode='%s'"

//// End of SQL for accessing the ICDCode table.
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//// SQL for accessing the Recommendations table.
#define SQL_INSERT_RECOMMENDATIONS \
"INSERT INTO Recommendations (KeyPhrase,Description) VALUES ('%s','%s')"

#define SQL_UPDATE_RECOMMENDATIONS \
"UPDATE Recommendations set Description='%s',KeyPhrase='%s' where KeyPhrase='%s'"

#define SQL_GET_RECOMMENDATIONS_ORDER \
"Select KeyPhrase,Description from Recommendations ORDER by KeyPhrase";

#define SQL_FIND_RECOMMENDATIONS "SELECT * from Recommendations where KeyPhrase='%s'"

#define SQL_DELETE_RECOMMENDATIONS "DELETE FROM Recommendations where KeyPhrase='%s'"

//// End of SQL for accessing the Recommendations table.
/////////////////////////////////////////////////////////////////////

/********************************************************************
Constructor

  Initialize class data.

********************************************************************/
CDbaccess::CDbaccess(LPCTSTR strDSN)
: m_cstrSearchSQL(_T(""))
, m_iDateSrchType(0)
, m_ctSrchDate(0)
, m_cstrSrchDate(_T(""))
  {

  m_cstrDSN = strDSN; //DSN_DEFAULT;
  m_bDBConnected = false;
  m_iConnType = DB_CONN_NONE;

  m_sqlhEnv = NULL; // Database environment handle.
  m_sqlhDbc = NULL; // Database connection handle.
  m_sqlhStmt = NULL; // Database statement handle.
  m_sqlhStmt1 = NULL; // Database statement handle.
  m_sqlhStmt2 = NULL; // Database statement handle.
  m_sqlhStmtPatient = NULL;
  m_cstrSearchSQL.Empty();

//// REMOTE DB
/*****************************/
  //m_cstrAltDBPath = "";
  //m_cstrLocalDBPath = "";
/******************************/
  }

/********************************************************************
Destructor

  Clean up class data.
  Disconnect from the database.
********************************************************************/

CDbaccess::~CDbaccess()
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
short CDbaccess::iDBConnect(void)
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
vSet_DSN - Set the DSN string used to connect to the database.

  inputs: CString object containing the DSN string.

  return: None.
********************************************************************/
void CDbaccess::vSet_DSN(CString cstrDSN)
  {
  m_cstrDSN = cstrDSN;
  }

//// REMOTE DB
/********************************************************************
vSet_alt_db_path - Set the alternate Database path.  The path is not
                   alternate if the local path is empty or the local
                   path is the same as the path in the input.

  inputs: CString object containing the database path.

  return: None.
********************************************************************/
//void CDbaccess::vSet_alt_db_path(CString cstrPath)
//  {
//
//  if(m_cstrLocalDBPath.IsEmpty() == false && m_cstrLocalDBPath != cstrPath)
//    m_cstrAltDBPath = cstrPath;
//  else
//    m_cstrAltDBPath.Empty(); // Not an alternate database.
//  }

/////////////////////////////////////////////////////////////////////
//// ODBC functions

/********************************************************************
sqlrDB_connect - Connect to the database.
                 Use SQLDriverConnect to attempt a connection to the 
                 indicated datasource.

  inputs: None.

  return: Return from ODBC functions.
********************************************************************/
SQLRETURN CDbaccess::sqlrDB_connect(void)
  {
  CString cstrConnStr;
  short int iOutConnLen;

  if(m_bDBConnected == false)
    {
      // Before we do anything, verify that the database file actually exists.
    if(bDBFileExists(m_cstrDSN) == true)
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
            {
            if(m_cstrDSN == DSN_DEFAULT) // && m_cstrAltDBPath == "")//// REMOTE DB
              {
              m_iConnType = DB_CONN_LOCAL;
              cstrConnStr.Format("DSN=%s", m_cstrDSN);
              }
            else
              {
              m_iConnType = DB_CONN_REMOTE;
              cstrConnStr.Format(
                "Driver={Microsoft Access Driver (*.mdb)};DBQ=%s", m_cstrDSN);
              }
//// REMOTE DB
/**************************************/
            //if(m_cstrAltDBPath.IsEmpty() == true)
            //  cstrConnStr.Format("DSN=%s", m_cstrDSN);
            //else
            //  cstrConnStr.Format("DSN=%s;DBQ=%s\\%s.mdb", m_cstrDSN, m_cstrAltDBPath, m_cstrDSN);
/******************************************/
            m_sqlrSts = SQLDriverConnect(m_sqlhDbc, NULL, (SQLTCHAR *)(const char *)cstrConnStr,
                                         SQL_NTS, m_sqlOutConnStr, 1024, &iOutConnLen, SQL_DRIVER_COMPLETE);
////                                      SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
            m_sqliHandleType = SQL_HANDLE_DBC;
            if(!SQL_SUCCEEDED(m_sqlrSts))
              { // Got an error.
              vReport_err("sqlrDB_connect", m_sqlhDbc);
              vFree_env_handle(); // Free the environment handle.
              vFree_db_conn_handle(); // Free the database connection handle.
              m_iConnType = DB_CONN_NONE;
              }
            else
              {
              m_bDBConnected = true; // Connected to the database.
//// REMOTE DB
/********************************************/
              //if(m_cstrLocalDBPath.IsEmpty() == true)
              //  { // First connection to the local database.  Save the path name.
              //  m_cstrLocalDBPath = cstrGet_db_file_path();
              //  }
/*****************************************/
              }
            }
          else
            {
            vReport_err("sqlrDB_connect", m_sqlhDbc);  // Got an error.
            vFree_env_handle(); // Free the environment handle.
            }
          }
        else
          {
          vReport_err("sqlrDB_connect", m_sqlhEnv);  // Got an error.
          vFree_env_handle(); // Free the environment handle.
          }
        }
      else //if(!SQL_SUCCEEDED(m_sqlrSts))
        vReport_err("sqlrDB_connect", m_sqlhEnv);  // Got an error.
      }
    else
      m_sqlrSts = SQL_ERROR; // Database wasn't found.
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
SQLRETURN CDbaccess::sqlrDB_disconnect(void)
  {

  if(m_sqlhDbc != NULL)
    {
    m_sqliHandleType = SQL_HANDLE_DBC;
    m_sqlrSts = SQLDisconnect(m_sqlhDbc);
    if(!SQL_SUCCEEDED(m_sqlrSts))
      vReport_err("sqlrDB_disconnect", m_sqlhDbc);  // Got an error.
    vFree_db_conn_handle(); // Free the database connection handle.
    }
  vFree_env_handle(); // Free the environment handle.
  m_bDBConnected = false;
  return(m_sqlrSts);
  }

/********************************************************************
vFree_env_handle - Free the environment handle.

  inputs: None.

  return: None.
********************************************************************/
void CDbaccess::vFree_env_handle(void)
  {

  if(m_sqlhEnv != NULL)
    {
    m_sqliHandleType = SQL_HANDLE_ENV;
    m_sqlrSts = SQLFreeHandle(SQL_HANDLE_ENV, m_sqlhEnv);
    if(SQL_SUCCEEDED(m_sqlrSts))
      m_sqlhEnv = NULL;
    else
      vReport_err("vFree_env_handle", m_sqlhEnv);  // Got an error.
    }
  }

/********************************************************************
vFree_db_conn_handle - Free the database connection handle.

  inputs: None.

  return: None.
********************************************************************/
void CDbaccess::vFree_db_conn_handle(void)
  {

  if(m_sqlhDbc != NULL)
    {
    m_sqliHandleType = SQL_HANDLE_DBC;
    m_sqlrSts = SQLFreeHandle(SQL_HANDLE_DBC, m_sqlhDbc);
    if(SQL_SUCCEEDED(m_sqlrSts))
      m_sqlhDbc = NULL;
    else
      vReport_err("vFree_db_conn_handle", m_sqlhDbc);  // Got an error.
    }
  }

/********************************************************************
sqlhGet_stmt_handle - Get a database statement handle.

  inputs: None.

  return: Handle to statement.
********************************************************************/
SQLHANDLE CDbaccess::sqlhGet_stmt_handle(void)
  {
  SQLHANDLE sqlhStmt;

  sqlhStmt = NULL;
  if(iDBConnect() == SUCCESS)
    {
    m_sqliHandleType = SQL_HANDLE_STMT;
    m_sqlrSts = SQLAllocHandle(SQL_HANDLE_STMT, m_sqlhDbc, &sqlhStmt); //&sqlhStmt);
    if(!SQL_SUCCEEDED(m_sqlrSts))
      { // Got an error, make sure the handle has been freed.
      vReport_err("sqlhGet_stmt_handle", sqlhStmt); //sqlhStmt);
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
SQLRETURN CDbaccess::sqlrFree_stmt_handle(SQLHANDLE sqlhStmt)
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
CString CDbaccess::cstrGet_db_name(void)
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
CString CDbaccess::cstrGet_db_path(void)
  {
  CString cstrPath;
  int iIndex;

  cstrPath.Empty();
  if(m_bDBConnected == true) //if(iDBConnect() == SUCCESS)
    { // Already connected, so we have a path to the database.
      // Get the path out of the connection string that resulted from 
      // connecting to the database.
    cstrPath = m_sqlOutConnStr;
    if((iIndex = cstrPath.Find("DBQ=")) >= 0)
      {
      cstrPath.Delete(0, iIndex + strlen("DBQ="));
      if((iIndex = cstrPath.Find(';')) >= 0)
        cstrPath.Delete(iIndex, cstrPath.GetLength() - iIndex);
      if((iIndex = cstrPath.ReverseFind('\\')) >= 0)
        cstrPath.Delete(iIndex, cstrPath.GetLength() - iIndex);
      }
    }
  if(cstrPath.IsEmpty() == TRUE)
    { // Not connected or didn't get a path to the database,
      // use the data path from the INI file.
    //cstrPath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_DATAPATH, "");
    cstrPath = g_pConfig->cstrGet_item(CFG_DATAPATH);
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
short int CDbaccess::iGet_string(SQLHSTMT sqlhStmt, short int iColumn, CString &cstrData)
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
      vReport_err("iGet_string", sqlhStmt);
    }
  return(iRet);
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
short int CDbaccess::iGet_long_string(SQLHSTMT sqlhStmt, short int iColumn, CString &cstrData)
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
      cstrData += szData;
      iRet = SUCCESS;
      }
    else if(m_sqlrSts == SQL_NO_DATA)
      {
      cstrData.Empty();
      iRet = SUCCESS;
      }
    else
      vReport_err("iGet_long_string", sqlhStmt);
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
short int CDbaccess::iGet_bool(SQLHSTMT sqlhStmt, short int iColumn, BOOL *pbValue)
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
iGet_bit - Get a boolean (TRUE/FALSE) value for the specified column from the
            current record retrieved in an SQL statement.

  inputs: Handle to the SQL statement used to get the record.
          Column in the database table to get the value for.
          Pointer to a BOOL that gets the value read from the database record.

  return: SUCCESS.
********************************************************************/
short int CDbaccess::iGet_bit(SQLHSTMT sqlhStmt, short int iColumn, BOOL *pbValue)
  {
  unsigned char ucValue;

  m_sqlrSts = SQLGetData(sqlhStmt, iColumn, SQL_C_BIT, &ucValue, 0, 0);
  if(ucValue != 0)
    *pbValue = TRUE;
  else
    *pbValue = FALSE;
  return(SUCCESS);
  }

/********************************************************************
vReport_err - Report a database error.  The message is obtained from data
              kept by the ODBC driver.

  inputs: Handle for the function that got the error.

  return: None.
********************************************************************/
void CDbaccess::vReport_err(char *pszMethod, SQLHANDLE sqlhHandle)
  {
  SQLRETURN sqlrRet;
  SQLCHAR sqlszState[10], sqlszMsgText[SQL_ERR_MSG_LEN];
  SQLINTEGER sqliNativeErrorPtr;
  SQLSMALLINT sqliMsgLen;
  CString cstrErr, cstrMsg, cstrLogMsg;


    //CString cstrMsg;
    //cstrMsg.Format("vReport_err, Method: %s", pszMethod);
    //AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, "DEBUG", MB_OK);

  if(iDBConnect() == SUCCESS)
    {
    if(sqlhHandle != NULL)
      {
      sqlrRet = SQLGetDiagRec(m_sqliHandleType, sqlhHandle, 1, sqlszState,
                             &sqliNativeErrorPtr, sqlszMsgText, SQL_ERR_MSG_LEN, &sqliMsgLen);
      if(sqlrRet == SQL_ERROR)
        cstrMsg = g_pLang->cstrLoad_string(ITEM_G_SQL_ERROR);
        //cstrMsg.LoadString(IDS_SQL_ERROR);
      else if(sqlrRet == SQL_INVALID_HANDLE)
        cstrMsg = g_pLang->cstrLoad_string(ITEM_G_SQL_INVALID_HANDLE);
        //cstrMsg.LoadString(IDS_SQL_INVALID_HANDLE);
      else if(sqlrRet == SQL_NO_DATA)
        cstrMsg = g_pLang->cstrLoad_string(ITEM_G_SQL_ERR_UNKNOWN);
        //cstrMsg.LoadString(IDS_SQL_ERR_UNKNOWN);
      else
        cstrMsg = sqlszMsgText;
  
      //cstrErr.Format(IDS_FMT_DATABASE_ERROR, cstrMsg);
      cstrErr = g_pLang->cstrLoad_string(ITEM_G_FMT_DATABASE_ERROR);
      cstrLogMsg.Format("%s: %s", pszMethod, cstrErr);
      ((CEGGSASApp *)AfxGetApp())->vLog(cstrLogMsg);
      //cstrErr.LoadString(IDS_ERROR1);
      cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERROR);
      (CEGGSASApp *)AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErr, MB_OK);
      }
    }
  }

/////////////////////////////////////////////////////////////////////
//// Patient table functions

/********************************************************************
iSave_patient - Save the required data to the "Patient" database table.
                If the patient is not yet in the table, add the record,
                otherwise, update the record.

  inputs: Pointer to PATIENT_DATA that contains the data to save to
            the table.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iSave_patient(PATIENT_DATA *pdInfo)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(pdInfo->lPatientID == INV_LONG)
      { // Add a new record to the database.
      cstrSQL.Format(SQL_INSERT_PATIENT, cstrFormat(pdInfo->cstrLastName), 
                     cstrFormat(pdInfo->cstrFirstName), cstrFormat(pdInfo->cstrMI),
                     pdInfo->bUseResrchRefNum, cstrFormat(pdInfo->cstrResearchRefNum),
                     pdInfo->bHide);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err("iSave_patient", m_sqlhStmt);
        sqlrFree_stmt_handle(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          { // Now find the PatientID that was just assigned.
          cstrSQL.Format("SELECT TOP 1 PatientID from Patient ORDER by PatientID DESC");
          if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
            {
            m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
            if(SQL_SUCCEEDED(m_sqlrSts))
              {
              m_sqlrSts = SQLFetch(m_sqlhStmt);
              if(SQL_SUCCEEDED(m_sqlrSts))
                {
                m_sqlrSts = SQLGetData(m_sqlhStmt, 1, SQL_C_LONG, &pdInfo->lPatientID, 0, 0);
                if(!SQL_SUCCEEDED(m_sqlrSts))
                  vReport_err("iSave_patient", m_sqlhStmt);
                else
                  iRet = SUCCESS;
                }
              else
                vReport_err("iSave_patient", m_sqlhStmt); // SQLFetch() failed.
              }
            else
              vReport_err("iSave_patient", m_sqlhStmt); // SQLExecDirect() failed.
            sqlrFree_stmt_handle(m_sqlhStmt);
            }
          }
        }
      else
        vReport_err("iSave_patient", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    else
      { // Update record in the database.
      cstrSQL.Format(SQL_UPDATE_PATIENT, cstrFormat(pdInfo->cstrLastName),
                     cstrFormat(pdInfo->cstrFirstName), cstrFormat(pdInfo->cstrMI),
                     pdInfo->bUseResrchRefNum, cstrFormat(pdInfo->cstrResearchRefNum),
                     pdInfo->bHide, pdInfo->lPatientID);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err("iSave_patient", m_sqlhStmt);
        else
          iRet = SUCCESS;
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      else
        vReport_err("iSave_patient", m_sqlhStmt);  // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/********************************************************************
iGet_patient - Get the  data from the "Patient" database table for
               the patient ID specified in the input.

  inputs: Pointer to PATIENT_DATA that contains the patient ID for
            the record to be retieved.  The structure also gets the
            data retrieved from the table.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int  CDbaccess::iGet_patient(PATIENT_DATA *pdInfo)
  {
  CString cstrSQL;
  int iField;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format(SQL_GET_PATIENT, pdInfo->lPatientID);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        { // Now get all the values.
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          {
          iRet = SUCCESS;
          iField = 1;
          m_sqlrSts = SQLGetData(m_sqlhStmt, iField, SQL_C_LONG, &pdInfo->lPatientID, 0, 0);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrLastName);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrFirstName);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrMI);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bUseResrchRefNum);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrResearchRefNum);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bHide);
          }
        else
          vReport_err("iGet_patient", m_sqlhStmt); // SQLFetch() failed.
        }
      else
        vReport_err("iGet_patient", m_sqlhStmt); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err("iGet_patient", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
    }
  return(iRet);
  }

/********************************************************************
iFind_patient - Determine if a patient is in the "Patient" database table
                for the patient ID specified in the input.

  inputs: Pointer to PATIENT_DATA that contains the patient ID.

  return: SUCCESS if the patient is in the table.
          FAIL if there is an error or the patient is not in the table.
********************************************************************/
short int CDbaccess::iFind_patient(PATIENT_DATA *pdInfo)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format("Select PatientID from Patient where PatientID=%lu", pdInfo->lPatientID);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err("iFind_patient", m_sqlhStmt);
    }
  return(iRet);
  }

/********************************************************************
iFind_patient_name - Determine if a patient is in the "Patient" database
                     table for the patient name specified in the input.

  inputs: Pointer to FIND_PATIENT_INFO that contains the patient name.

  output: The lPatientID member of FIND_PATIENT_INFO gets set to the patient
          ID from the database.

  return: SUCCESS if the patient is in the table.
          FAIL if there is an error or the patient is not in the table.
********************************************************************/
short int CDbaccess::iFind_patient_name(FIND_PATIENT_INFO *pfpiData)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
//      cstrSQL.Format("Select PatientID from Patient where LastName='%s' AND FirstName='%s'",
      cstrSQL.Format(SQL_FIND_PATIENT, cstrFormat(pfpiData->cstrLastName),
                     cstrFormat(pfpiData->cstrFirstName), cstrFormat(pfpiData->cstrMI),
                     cstrFormat(pfpiData->cstrResearchRefNum));
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          {
          SQLGetData(m_sqlhStmt, 1, SQL_C_LONG, &pfpiData->lPatientID, 0, 0);
          iRet = SUCCESS;
          }
        else if(m_sqlrSts == SQL_NO_DATA)
          iRet = SUCCESS_NO_DATA;
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err("iFind_patient_name", m_sqlhStmt);
    }
  return(iRet);
  }

/********************************************************************
iFind_next_patient - Get the patient information in the "Patient" database
                     table for the first or next patient ordered by
                     last name, then first name.
                     Getting the first record causes an SQL Select statement
                     to get all the patient records ordered by last name
                     then first name.

  inputs: Position in the record set from the table to get
            (first record or next record).
          Pointer to FIND_PATIENT_INFO that contains the patient name.

  return: SUCCESS if the patient is in the table.
          FAIL if there is an error or the patient is not in the table.
********************************************************************/
short int CDbaccess::iFind_next_patient(short int iPos, FIND_PATIENT_INFO *pfpiData)
  {
  CString cstrSQL;
  int iField;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(iPos == FIRST_RECORD)
      { // Wants first record, so select all patients and return only first record.
      if((m_sqlhStmtPatient = sqlhGet_stmt_handle()) != NULL)
        {
        if(m_cstrSearchSQL.IsEmpty() == true)
          {
          if(pfpiData->bAll == TRUE)
            cstrSQL = SQL_GET_PATIENT_ORDER;
          else
            cstrSQL.Format(SQL_GET_PATIENT_ORDER_HIDE, pfpiData->bHide);
          }
        else
            cstrSQL = m_cstrSearchSQL;

        m_sqlrSts = SQLExecDirect(m_sqlhStmtPatient,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      else
        vReport_err("iFind_next_patient", m_sqlhStmtPatient);
      }
    else
      iRet = SUCCESS; // Get records after the first
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmtPatient);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        iField = 1;
        SQLGetData(m_sqlhStmtPatient, iField, SQL_C_LONG, &pfpiData->lPatientID, 0, 0);
        iGet_string(m_sqlhStmtPatient, ++iField, pfpiData->cstrLastName);
        iGet_string(m_sqlhStmtPatient, ++iField, pfpiData->cstrFirstName);
        iGet_string(m_sqlhStmtPatient, ++iField, pfpiData->cstrMI);
        iGet_bool(m_sqlhStmtPatient, ++iField, &pfpiData->bUseResrchRefNum);
        iGet_string(m_sqlhStmtPatient, ++iField, pfpiData->cstrResearchRefNum);
        iGet_bool(m_sqlhStmtPatient, ++iField, &pfpiData->bHide);
        }
      else
        iRet = FAIL;
      }
    if(iRet == FAIL)
      sqlrFree_stmt_handle(m_sqlhStmtPatient); // Done getting records.
    }
  return(iRet);
  }


/////////////////////////////////////////////////////////////////////
//// PatientInfo table functions

/********************************************************************
iSave_patient_info - Save the required data to the "PatientInfo" database
                     table. If the patient is not yet in the table, add the
                     record, otherwise, update the record.

  inputs: Pointer to PATIENT_DATA that contains the data to save to
            the table.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iSave_patient_info(PATIENT_DATA *pdInfo)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    if(pdInfo->lPatientID != INV_LONG)
      { // Have a patient ID, see if we can find a record in this table.
      iRet = iFind_patient_info(pdInfo->lPatientID);
      }
    else
      iRet = FAIL;
    if(iRet == FAIL)
      { // Add a new record to the database.

      //int iIndex;
      //if((iIndex = pdInfo->cstrAddress1.Find("'")) >= 0)
      //  pdInfo->cstrAddress1.Insert(iIndex, "'");

      cstrSQL.Format(SQL_INSERT_PATIENT_INFO, pdInfo->lPatientID,
                     cstrFormat(pdInfo->cstrAddress1),
                     cstrFormat(pdInfo->cstrAddress2), cstrFormat(pdInfo->cstrCity),
                     cstrFormat(pdInfo->cstrState), cstrFormat(pdInfo->cstrZipCode),
                     cstrFormat(pdInfo->cstrPhone), cstrFormat(pdInfo->cstrDateOfBirth),
                     cstrFormat(pdInfo->cstrHospitalNum),
                     cstrFormat(pdInfo->cstrSocialSecurityNum), pdInfo->bMale,
                     pdInfo->bFemale);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err("iSave_patient_info", m_sqlhStmt);
        else
          iRet = SUCCESS;
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      else
        vReport_err("iSave_patient_info", m_sqlhStmt);  // sqlhGet_stmt_handle() failed.
      }
    else
      { // Update record in the database.
      iRet = FAIL;
      cstrSQL.Format(SQL_UPDATE_PATIENT_INFO, cstrFormat(pdInfo->cstrAddress1),
                     cstrFormat(pdInfo->cstrAddress2), cstrFormat(pdInfo->cstrCity),
                     cstrFormat(pdInfo->cstrState), cstrFormat(pdInfo->cstrZipCode), 
                     cstrFormat(pdInfo->cstrPhone), cstrFormat(pdInfo->cstrDateOfBirth),
                     cstrFormat(pdInfo->cstrHospitalNum),
                     cstrFormat(pdInfo->cstrSocialSecurityNum), pdInfo->bMale,
                     pdInfo->bFemale, pdInfo->lPatientID);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err("iSave_patient_info", m_sqlhStmt);
        else
          iRet = SUCCESS;
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      else
        vReport_err("iSave_patient_info", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/********************************************************************
iSave_patient_info - Save the required data to the "PatientInfo" database
                     table. If the patient is not yet in the table, add the
                     record, otherwise, update the record.

  inputs: Pointer to PATIENT_DATA that contains the data to save to
            the table.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iSave_patient_info(PATIENT_INFO *ppiInfo)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    if(ppiInfo->lPatientID != INV_LONG)
      { // Have a patient ID, see if we can find a record in this table.
      iRet = iFind_patient_info(ppiInfo->lPatientID);
      }
    else
      iRet = FAIL;
    if(iRet == FAIL)
      { // Add a new record to the database.
      cstrSQL.Format(SQL_INSERT_PATIENT_INFO, ppiInfo->lPatientID,
                     cstrFormat(ppiInfo->cstrAddress1), cstrFormat(ppiInfo->cstrAddress2),
                     cstrFormat(ppiInfo->cstrCity), cstrFormat(ppiInfo->cstrState),
                     cstrFormat(ppiInfo->cstrZipCode), cstrFormat(ppiInfo->cstrPhone),
                     cstrFormat(ppiInfo->cstrDateOfBirth), cstrFormat(ppiInfo->cstrHospitalNum),
                     cstrFormat(ppiInfo->cstrSocialSecurityNum), ppiInfo->bMale,
                     ppiInfo->bFemale);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err("iSave_patient_info", m_sqlhStmt);
        else
          iRet = SUCCESS;
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      else
        vReport_err("iSave_patient_info", m_sqlhStmt);  // sqlhGet_stmt_handle() failed.
      }
    else
      { // Update record in the database.
      iRet = FAIL;
      cstrSQL.Format(SQL_UPDATE_PATIENT_INFO, cstrFormat(ppiInfo->cstrAddress1),
                     cstrFormat(ppiInfo->cstrAddress2), cstrFormat(ppiInfo->cstrCity),
                     cstrFormat(ppiInfo->cstrState), cstrFormat(ppiInfo->cstrZipCode),
                     cstrFormat(ppiInfo->cstrPhone), cstrFormat(ppiInfo->cstrDateOfBirth),
                     cstrFormat(ppiInfo->cstrHospitalNum),
                     cstrFormat(ppiInfo->cstrSocialSecurityNum), ppiInfo->bMale,
                     ppiInfo->bFemale, ppiInfo->lPatientID); // ppiInfo->bHide, 
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err("iSave_patient_info", m_sqlhStmt);
        else
          iRet = SUCCESS;
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      else
        vReport_err("iSave_patient_info", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/********************************************************************
iGet_patient_info - Get the  data from the "PatientInfo" database table for
                    the patient ID specified in the input.

  inputs: Pointer to PATIENT_DATA that contains the patient ID for
            the record to be retieved.  The structure also gets the
            data retrieved from the table.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iGet_patient_info(PATIENT_DATA *pdInfo)
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
      cstrSQL.Format(SQL_GET_PATIENT_INFO, pdInfo->lPatientID);
      m_sqlrSts = SQLExecDirect(sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        { // Now get all the values.
        m_sqlrSts = SQLFetch(sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          {
          iRet = SUCCESS;
          iField = 1;
          m_sqlrSts = SQLGetData(sqlhStmt, iField, SQL_C_LONG, &pdInfo->lPatientID, 0, 0);
          iGet_string(sqlhStmt, ++iField, pdInfo->cstrAddress1);
          iGet_string(sqlhStmt, ++iField, pdInfo->cstrAddress2);
          iGet_string(sqlhStmt, ++iField, pdInfo->cstrCity);
          iGet_string(sqlhStmt, ++iField, pdInfo->cstrState);
          iGet_string(sqlhStmt, ++iField, pdInfo->cstrZipCode);
          iGet_string(sqlhStmt, ++iField, pdInfo->cstrPhone);
          iGet_string(sqlhStmt, ++iField, pdInfo->cstrDateOfBirth);
          iGet_string(sqlhStmt, ++iField, pdInfo->cstrHospitalNum);
          iGet_string(sqlhStmt, ++iField, pdInfo->cstrSocialSecurityNum);
          iGet_bool(sqlhStmt, ++iField, &pdInfo->bMale);
          iGet_bool(sqlhStmt, ++iField, &pdInfo->bFemale);
          }
        else
          vReport_err("iGet_patient_info", sqlhStmt); // SQLFetch() failed.
        }
      else
        vReport_err("iGet_patient_info", sqlhStmt); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(sqlhStmt);
      }
    else
      vReport_err("iGet_patient_info", sqlhStmt); // sqlhGet_stmt_handle() failed.
    }
  return(iRet);
  }

/********************************************************************
iGet_patient_info - Overloaded method.
                    Get the  data from the "PatientInfo" database table for
                    the patient ID specified in the input.

  inputs: Pointer to PATIENT_INFO that contains the patient ID for
            the record to be retieved.  The structure also gets the
            data retrieved from the table.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iGet_patient_info(PATIENT_INFO *ppiInfo)
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
      cstrSQL.Format(SQL_GET_PATIENT_INFO, ppiInfo->lPatientID);
      m_sqlrSts = SQLExecDirect(sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        { // Now get all the values.
        m_sqlrSts = SQLFetch(sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          {
          iRet = SUCCESS;
          iField = 1;
          m_sqlrSts = SQLGetData(sqlhStmt, iField, SQL_C_LONG, &ppiInfo->lPatientID, 0, 0);
          iGet_string(sqlhStmt, ++iField, ppiInfo->cstrAddress1);
          iGet_string(sqlhStmt, ++iField, ppiInfo->cstrAddress2);
          iGet_string(sqlhStmt, ++iField, ppiInfo->cstrCity);
          iGet_string(sqlhStmt, ++iField, ppiInfo->cstrState);
          iGet_string(sqlhStmt, ++iField, ppiInfo->cstrZipCode);
          iGet_string(sqlhStmt, ++iField, ppiInfo->cstrPhone);
          iGet_string(sqlhStmt, ++iField, ppiInfo->cstrDateOfBirth);
          iGet_string(sqlhStmt, ++iField, ppiInfo->cstrHospitalNum);
          iGet_string(sqlhStmt, ++iField, ppiInfo->cstrSocialSecurityNum);
          iGet_bool(sqlhStmt, ++iField, &ppiInfo->bMale);
          iGet_bool(sqlhStmt, ++iField, &ppiInfo->bFemale);
          }
        else
          vReport_err("iGet_patient_info", sqlhStmt); // SQLFetch() failed.
        }
      else
        vReport_err("iGet_patient_info", sqlhStmt); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(sqlhStmt);
      }
    else
      vReport_err("iGet_patient_info", sqlhStmt); // sqlhGet_stmt_handle() failed.
    }
  return(iRet);
  }

/********************************************************************
iFind_patient_info - Determine if a patient is in the "PatientInfo" database
                     table for the patient ID specified in the input.

  inputs: Pointer to PATIENT_DATA that contains the patient ID.

  return: SUCCESS if the patient is in the table.
          FAIL if there is an error or the patient is not in the table.
********************************************************************/
short int CDbaccess::iFind_patient_info(long lPatientID)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format("Select PatientID from PatientInfo where PatientID=%lu", lPatientID);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err("iFind_patient_info", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// StudyData table functions

/********************************************************************
lFind_study_data - Determine if there is at least one study for the
                   specified patient is in the "StudyData" database
                   table.  The patient ID is specified in the input.

  inputs: Pointer to PATIENT_DATA that contains the patient ID.

  return: Study ID if one is found.
          INV_LONG if a study is not found.
********************************************************************/
long CDbaccess::lFind_study_data(long lPatientID)
  {
  CString cstrSQL;
  long lStudyID = INV_LONG;

  if(iDBConnect() == SUCCESS)
    {
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format("Select StudyID from StudyData where PatientID=%lu", lPatientID);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          m_sqlrSts = SQLGetData(m_sqlhStmt, 1, SQL_C_LONG, &lStudyID, 0, 0);
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err("lFind_study_data", m_sqlhStmt);
    }
  return(lStudyID);
  }

/********************************************************************
lFind_study_data - Determine if there is at least one study for the
                   specified patient is in the "StudyData" database
                   table.  The patient ID is specified in the input.

  inputs: Pointer to PATIENT_DATA that contains the patient ID.
          String containing the date of this study "mm/dd/yyyy".

  return: Study ID if one is found.
          INV_LONG if a study is not found.
********************************************************************/
long CDbaccess::lFind_study_data(long lPatientID, CString cstrDate)
  {
  CString cstrSQL;
  long lStudyID = INV_LONG;

  if(iDBConnect() == SUCCESS)
    {
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format(
          "Select StudyID from StudyData where PatientID=%lu and DateOfStudy='%s'",
          lPatientID, cstrFormat(cstrDate));
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          m_sqlrSts = SQLGetData(m_sqlhStmt, 1, SQL_C_LONG, &lStudyID, 0, 0);
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err("lFind_study_data", m_sqlhStmt);
    }
  return(lStudyID);
  }


/********************************************************************
iGet_study_data - Get all the record values for the study specified
                  by the study ID from the "StudyData" database
                  table.  The study ID is specified in the input.

  inputs: Pointer to PATIENT_DATA that contains the study ID.

  return: SUCCESS if the study data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iGet_study_data(PATIENT_DATA *pdInfo)
  {
  CString cstrSQL, cstrTemp;
  int iField;
  unsigned short uCnt;
  short int iIndex, iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      if(pdInfo->cstrDate.IsEmpty() == FALSE)
        cstrSQL.Format(SQL_GET_STUDY_DATA, pdInfo->lPatientID, cstrFormat(pdInfo->cstrDate));
      else
        cstrSQL.Format(SQL_GET_STUDY_DATA_FOR_ID, pdInfo->lStudyID);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        { // Now get all the values.
        pdInfo->uFrequency = 0;
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          {
          iRet = SUCCESS;
          iField = 1;
          SQLGetData(m_sqlhStmt, iField, SQL_C_LONG, &pdInfo->lStudyID, 0, 0);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &pdInfo->lPatientID, 0, 0);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrFile);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrDate);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrMealTime);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_SHORT, &pdInfo->uWaterAmount, 0, 0);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrWaterUnits);
          iGet_string(m_sqlhStmt, ++iField, cstrTemp); // Start period
          for(uCnt = 0; uCnt < MAX_TOTAL_PERIODS; ++uCnt)
            {
            if(cstrTemp.IsEmpty() == FALSE)
              {
              iIndex = cstrTemp.Find(",", 0);
              if(iIndex < 0)
                iIndex = cstrTemp.GetLength(); // Last one.
              pdInfo->fPeriodStart[uCnt] = (float)atof(cstrTemp.Left(iIndex));
              cstrTemp.Delete(0, iIndex + 1);
              }
            else
              pdInfo->fPeriodStart[uCnt] = INV_PERIOD_START;
            }
            // First period is baseline, don't count it.
          pdInfo->uNumPeriods = 0;
          iGet_string(m_sqlhStmt, ++iField, cstrTemp); // Length of period
          for(uCnt = 0; uCnt < MAX_TOTAL_PERIODS; ++uCnt)
            {
            if(cstrTemp.IsEmpty() == FALSE)
              {
              iIndex = cstrTemp.Find(",", 0);
              if(iIndex < 0)
                iIndex = cstrTemp.GetLength(); // Last one.
              pdInfo->fPeriodLen[uCnt] = (float)atof(cstrTemp.Left(iIndex));
              if(pdInfo->fPeriodLen[uCnt] > 0)
                ++pdInfo->uNumPeriods;
              cstrTemp.Delete(0, iIndex + 1);
              }
            else
              pdInfo->fPeriodLen[uCnt] = INV_PERIOD_LEN;
            }
          iGet_string(m_sqlhStmt, ++iField, cstrTemp); // Start minutes
          for(uCnt = 0; uCnt < MAX_TOTAL_PERIODS; ++uCnt)
            {
            if(cstrTemp.IsEmpty() == FALSE)
              {
              iIndex = cstrTemp.Find(",", 0);
              if(iIndex < 0)
                iIndex = cstrTemp.GetLength(); // Last one.
              pdInfo->fGoodMinStart[uCnt] = (float)atof(cstrTemp.Left(iIndex));
              cstrTemp.Delete(0, iIndex + 1);
              }
            else
              pdInfo->fGoodMinStart[uCnt] = INV_MINUTE;
            }
          iGet_string(m_sqlhStmt, ++iField, cstrTemp); // End minutes
          for(uCnt = 0; uCnt < MAX_TOTAL_PERIODS; ++uCnt)
            {
            if(cstrTemp.IsEmpty() == FALSE)
              {
              iIndex = cstrTemp.Find(",", 0);
              if(iIndex < 0)
                iIndex = cstrTemp.GetLength(); // Last one.
              pdInfo->fGoodMinEnd[uCnt] = (float)atof(cstrTemp.Left(iIndex));
              cstrTemp.Delete(0, iIndex + 1);
              }
            else
              pdInfo->fGoodMinEnd[uCnt] = INV_MINUTE;
            }
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_FLOAT, &pdInfo->fTestMaxEGG, 0, 0);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_FLOAT, &pdInfo->fTestMinEGG, 0, 0);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bExtra10Min);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrSuggestedDiagnosis);
            // BOOL requires a short.
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bBloating);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bEarlySateity);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bEructus);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bGerd);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bNausea);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bPPFullness);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bVomiting);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrAttendPhys);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrReferPhys);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrTechnician);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bManualMode);
          iGet_string(m_sqlhStmt, ++iField, cstrTemp); // Respiration rate
          for(uCnt = 0; uCnt < MAX_TOTAL_PERIODS; ++uCnt)
            {
            if(cstrTemp.IsEmpty() == FALSE)
              {
              iIndex = cstrTemp.Find(",", 0);
              if(iIndex < 0)
                iIndex = cstrTemp.GetLength();
              pdInfo->uRespRate[uCnt] = (unsigned short)atoi(cstrTemp.Left(iIndex));
              cstrTemp.Delete(0, iIndex + 1);
              }
            else
              pdInfo->uRespRate[uCnt] = 0;
            }
//          SQLGetData(m_sqlhStmt, ++iField, SQL_C_SHORT, &pdInfo->bUserDiagnosis, 0, 0);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bUserDiagnosis);
          iGet_long_string(m_sqlhStmt, ++iField, pdInfo->cstrIFPComments);
          iGet_long_string(m_sqlhStmt, ++iField, pdInfo->cstrDiagnosis);
          iGet_long_string(m_sqlhStmt, ++iField, pdInfo->cstrRecommendations);
          iGet_long_string(m_sqlhStmt, ++iField, pdInfo->cstrMedications);
          iGet_bit(m_sqlhStmt, ++iField, &pdInfo->bDelayedGastricEmptying);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &pdInfo->uDataPtsInBaseline, 0, 0);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bPause);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &pdInfo->uPostStimDataPoints, 0, 0);
          iGet_long_string(m_sqlhStmt, ++iField, pdInfo->cstrStimMedDesc);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrDiagnosisKeyPhrase);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrICDCode);
          iGet_string(m_sqlhStmt, ++iField, pdInfo->cstrRecommendationKeyPhrase);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bGastroparesis);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bDyspepsiaBloatSatiety);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bDyspepsia);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bAnorexia);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bRefractoryGERD);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bWeightLoss);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bPostDilationEval);
          iGet_bool(m_sqlhStmt, ++iField, &pdInfo->bEvalEffectMed);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &pdInfo->uFrequency, 0, 0);

          // Frequency is a new field and may not have ever been set to a valid value.
          // If frequency is not a valid value, default it to 15cpm.
          if(pdInfo->uFrequency == 0 || pdInfo->uFrequency > 20000)
            pdInfo->uFrequency = 15;
            // If one the old values of m_bBloating or m_bEarlySateity are
            // TRUE then set the new value bDyspepsiaBloatSatiety to TRUE
          if(pdInfo->bBloating == TRUE || pdInfo->bEarlySateity == TRUE)
            pdInfo->bDyspepsiaBloatSatiety = TRUE;
          // If the old value of m_bDelayedGastricEmptying is TRUE
          // then set the new bGastroparesis to TRUE.
          if(pdInfo->bDelayedGastricEmptying == TRUE)
            pdInfo->bGastroparesis = TRUE;

          if(pdInfo->uDataPtsInBaseline == 0)
            { // Standard version.  Even though uNumPeriods is used only in the
              // research version, set it for consistency.
            if(pdInfo->bExtra10Min == TRUE)
              pdInfo->uNumPeriods = 4;
            else
              pdInfo->uNumPeriods = 3;
            }
          else
            {
            if(pdInfo->bPause ==  TRUE)
              pdInfo->uNumPeriods = 0;
            else if(pdInfo->uNumPeriods == 0 && pdInfo->bPause == false)
              pdInfo->uNumPeriods = 1; // If haven't paused, there must always be one period.
            else if(pdInfo->uNumPeriods > 1)
              --pdInfo->uNumPeriods;  // Remove one for the baseline.
            }
//// REMOTE DB
/*******************************************/
          //// Make sure the patient data file has the correct path.  The path may
          //// change from what's saved in the database if the user selected another
          //// database in another location.
          //if(m_cstrAltDBPath.IsEmpty() == false)
          //  { // Database exists in a different place so correct the data file path and name.
          //  if((iIndex = pdInfo->cstrFile.ReverseFind('\\')) > 0)
          //    {
          //    m_cstrOrigEGGPath = pdInfo->cstrFile;
          //    cstrTemp = pdInfo->cstrFile.Right(pdInfo->cstrFile.GetLength() - iIndex);
          //    pdInfo->cstrFile = m_cstrAltDBPath + cstrTemp;
          //    }
          //  }
/*************************************/
          }
        }
      else
        vReport_err("iGet_study_data", m_sqlhStmt); // SQLExecDirect failed
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err("iGet_study_data", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
    }
  return(iRet);
  }

/********************************************************************
iGet_study_data - Get all the record values for the study specified
                  by the study ID from the "StudyData" database
                  table.  The study ID is specified in the input.

  inputs: Pointer to STUDY_DATA_RECORD that contains the study ID and gets
            the data for the study.

  return: SUCCESS if the study data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iGet_study_data(STUDY_DATA_RECORD *psd)
  {
  CString cstrSQL, cstrTemp;
  int iField;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      if(psd->cstrDateOfStudy.IsEmpty() == FALSE)
        cstrSQL.Format(SQL_GET_STUDY_DATA, psd->lPatientID, cstrFormat(psd->cstrDateOfStudy));
      else
        cstrSQL.Format(SQL_GET_STUDY_DATA_FOR_ID, psd->lStudyID);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        { // Now get all the values.
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          {
          iRet = SUCCESS;
          iField = 1;
          SQLGetData(m_sqlhStmt, iField, SQL_C_LONG, &psd->lStudyID, 0, 0);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &psd->lPatientID, 0, 0);
          iGet_string(m_sqlhStmt, ++iField, psd->cstrDataFile);
          iGet_string(m_sqlhStmt, ++iField, psd->cstrDateOfStudy);
          iGet_string(m_sqlhStmt, ++iField, psd->cstrMealTime);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_SHORT, &psd->uWaterAmount, 0, 0);
          iGet_string(m_sqlhStmt, ++iField, psd->cstrWaterAmountUnits);
          iGet_string(m_sqlhStmt, ++iField, psd->cstrPeriodStart); // Start period
          iGet_string(m_sqlhStmt, ++iField, psd->cstrPeriodLen); // Length of period
          iGet_string(m_sqlhStmt, ++iField, psd->cstrStartMinutes); // Start minutes
          iGet_string(m_sqlhStmt, ++iField, psd->cstrEndMinutes); // End minutes
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &psd->lTestMaxSignal, 0, 0);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &psd->lTestMinSignal, 0, 0);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bExtra10Min);
          iGet_string(m_sqlhStmt, ++iField, psd->cstrSuggestedDiagnosis);
            // BOOL requires a short.
          iGet_bool(m_sqlhStmt, ++iField, &psd->bBloating);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bEarlySateity);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bEructus);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bGerd);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bNausea);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bPostPrandialFullness);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bVomiting);
          iGet_string(m_sqlhStmt, ++iField, psd->cstrAttendingPhysician);
          iGet_string(m_sqlhStmt, ++iField, psd->cstrReferingPhysician);
          iGet_string(m_sqlhStmt, ++iField, psd->cstrTechnician);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bManualMode);
          iGet_string(m_sqlhStmt, ++iField, psd->cstrRespirationRate); // Respiration rate
          iGet_bool(m_sqlhStmt, ++iField, &psd->bUserDiagnosis);
          iGet_long_string(m_sqlhStmt, ++iField, psd->cstrIFPComments);
          iGet_long_string(m_sqlhStmt, ++iField, psd->cstrDiagnosis);
          iGet_long_string(m_sqlhStmt, ++iField, psd->cstrRecommendations);
          iGet_long_string(m_sqlhStmt, ++iField, psd->cstrMedications);
          iGet_bit(m_sqlhStmt, ++iField, &psd->bDelayedGastricEmptying);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &psd->uiBLDataPoints, 0, 0);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bPause);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &psd->uiPostStimDataPoints, 0, 0);
          iGet_long_string(m_sqlhStmt, ++iField, psd->cstrStimulationMedium);
          iGet_string(m_sqlhStmt, ++iField, psd->cstrDiagnosisKeyPhrase);
          iGet_string(m_sqlhStmt, ++iField, psd->cstrICDCode);
          iGet_string(m_sqlhStmt, ++iField, psd->cstrRecommendationKeyPhrase);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bGastroparesis);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bDyspepsiaBloatSatiety);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bDyspepsia);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bAnorexia);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bRefractoryGERD);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bWeightLoss);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bPostDilationEval);
          iGet_bool(m_sqlhStmt, ++iField, &psd->bEvalEffectMed);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &psd->uFrequency, 0, 0);

          // Frequency is a new field and may not have ever been set to a valid value.
          // If frequency is not a valid value, default it to 15cpm.
          if(psd->uFrequency == 0 || psd->uFrequency > 20000)
            psd->uFrequency = 15;
          }
        }
      else
        vReport_err("iGet_study_data", m_sqlhStmt); // SQLExecDirect failed
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err("iGet_study_data", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
    }
  return(iRet);
  }

/********************************************************************
iSave_study_data - Save the study data to the "StudyData" database
                   table.  If the study is not yet in the table, add the
                   record, otherwise, update the record.

  inputs: Pointer to PATIENT_DATA that contains the study ID.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iSave_study_data(PATIENT_DATA *pdInfo)
  {
  CString cstrSQL, cstrStartMinutes, cstrEndMinutes, cstrPeriodStart, cstrPeriodLen;
  CString cstrRespRate, cstrTemp;
  long lStudy;
  short int iRet, iIndex;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
      //// TODO Put in loops for array limit.
      // These data items are arrays, but saved as comma delimited strings in the
      // database.
    cstrStartMinutes.Empty();
    cstrEndMinutes.Empty();
    cstrPeriodStart.Empty();
    cstrPeriodLen.Empty();
    cstrRespRate.Empty();
    for(iIndex = 0; iIndex < (MAX_TOTAL_PERIODS - 1); ++iIndex)
      {
      cstrTemp.Format("%.1f,", pdInfo->fGoodMinStart[iIndex]);
      cstrStartMinutes += cstrTemp;
      cstrTemp.Format("%.1f,", pdInfo->fGoodMinEnd[iIndex]);
      cstrEndMinutes += cstrTemp;
      cstrTemp.Format("%.1f,", pdInfo->fPeriodStart[iIndex]);
      cstrPeriodStart += cstrTemp;
      cstrTemp.Format("%.1f,", pdInfo->fPeriodLen[iIndex]);
      cstrPeriodLen += cstrTemp;
      cstrTemp.Format("%d,", pdInfo->uRespRate[iIndex]);
      cstrRespRate += cstrTemp;
      }
      // Now do last one without trailing comma.
      cstrTemp.Format("%.1f", pdInfo->fGoodMinStart[iIndex]);
      cstrStartMinutes += cstrTemp;
      cstrTemp.Format("%.1f", pdInfo->fGoodMinEnd[iIndex]);
      cstrEndMinutes += cstrTemp;
      cstrTemp.Format("%.1f", pdInfo->fPeriodStart[iIndex]);
      cstrPeriodStart += cstrTemp;
      cstrTemp.Format("%.1f", pdInfo->fPeriodLen[iIndex]);
      cstrPeriodLen += cstrTemp;
      cstrTemp.Format("%d", pdInfo->uRespRate[iIndex]);
      cstrRespRate += cstrTemp;
//// REMOTE DB
/*************************************************/
      //  // Make sure the path on the file name field is correct.
      //if(m_cstrAltDBPath.IsEmpty() == false)
      //  { // Using an alternate database. Restore the original path.
      //  pdInfo->cstrFile = m_cstrOrigEGGPath;
      //  }
/********************************************/
    if(pdInfo->lStudyID == INV_LONG
    || (lStudy = lFind_study_data(pdInfo->lPatientID)) == INV_LONG)
      { // Insert new study.
      cstrSQL.Format(SQL_INSERT_STUDY_DATA, pdInfo->lPatientID, cstrFormat(pdInfo->cstrFile),
                     cstrFormat(pdInfo->cstrDate), cstrFormat(pdInfo->cstrMealTime),
                     pdInfo->uWaterAmount, cstrFormat(pdInfo->cstrWaterUnits),
                     cstrPeriodStart, cstrPeriodLen, cstrStartMinutes, cstrEndMinutes,
                     pdInfo->fTestMaxEGG, pdInfo->fTestMinEGG, pdInfo->bExtra10Min,
                     cstrFormat(pdInfo->cstrSuggestedDiagnosis), pdInfo->bBloating,
                     pdInfo->bEarlySateity, pdInfo->bEructus, pdInfo->bGerd, pdInfo->bNausea,
                     pdInfo->bPPFullness, pdInfo->bVomiting, cstrFormat(pdInfo->cstrAttendPhys), 
                     cstrFormat(pdInfo->cstrReferPhys), cstrFormat(pdInfo->cstrTechnician),
                     pdInfo->bManualMode, cstrRespRate, pdInfo->bUserDiagnosis,
                     cstrFormat(pdInfo->cstrIFPComments), cstrFormat(pdInfo->cstrDiagnosis),
                     cstrFormat(pdInfo->cstrRecommendations), cstrFormat(pdInfo->cstrMedications),
                     pdInfo->bDelayedGastricEmptying, pdInfo->uDataPtsInBaseline,
                     pdInfo->bPause, pdInfo->uPostStimDataPoints,
                     cstrFormat(pdInfo->cstrStimMedDesc),
                     cstrFormat(pdInfo->cstrDiagnosisKeyPhrase),
                     cstrFormat(pdInfo->cstrICDCode),
                     cstrFormat(pdInfo->cstrRecommendationKeyPhrase),
                     pdInfo->bGastroparesis, pdInfo->bDyspepsiaBloatSatiety,
                     pdInfo->bDyspepsia, pdInfo->bAnorexia, pdInfo->bRefractoryGERD,
                     pdInfo->bWeightLoss, pdInfo->bPostDilationEval,
                     pdInfo->bEvalEffectMed, pdInfo->uFrequency);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err("iSave_study_data", m_sqlhStmt);
        sqlrFree_stmt_handle(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          { // Now find the StudyID that was just assigned.
          cstrSQL.Format("SELECT TOP 1 StudyID from StudyData where PatientID = %lu ORDER by StudyID DESC",
                         pdInfo->lPatientID);
          if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
            {
            m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
            if(SQL_SUCCEEDED(m_sqlrSts))
              {
              m_sqlrSts = SQLFetch(m_sqlhStmt);
              if(SQL_SUCCEEDED(m_sqlrSts))
                {
                m_sqlrSts = SQLGetData(m_sqlhStmt, 1, SQL_C_LONG, &pdInfo->lStudyID, 0, 0);
                if(SQL_SUCCEEDED(m_sqlrSts))
                  iRet = SUCCESS;
                else
                  vReport_err("iSave_study_data", m_sqlhStmt);
                }
              else
                vReport_err("iSave_study_data", m_sqlhStmt); // SQLFetch() failed.
              }
            else
              vReport_err("iSave_study_data", m_sqlhStmt); // SQLExecDirect() failed.
            sqlrFree_stmt_handle(m_sqlhStmt);
            }
          }
        }
      else
        vReport_err("iSave_study_data", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    else
      { // Update existing study
      cstrSQL.Format(SQL_UPDATE_STUDY_DATA, cstrFormat(pdInfo->cstrFile), 
                     cstrFormat(pdInfo->cstrDate), cstrFormat(pdInfo->cstrMealTime),
                     pdInfo->uWaterAmount, cstrFormat(pdInfo->cstrWaterUnits),
                     cstrPeriodStart, cstrPeriodLen, cstrStartMinutes, cstrEndMinutes,
                     pdInfo->fTestMaxEGG, pdInfo->fTestMinEGG, pdInfo->bExtra10Min,
                     cstrFormat(pdInfo->cstrSuggestedDiagnosis), pdInfo->bBloating,
                     pdInfo->bEarlySateity, pdInfo->bEructus, pdInfo->bGerd, pdInfo->bNausea,
                     pdInfo->bPPFullness, pdInfo->bVomiting, cstrFormat(pdInfo->cstrAttendPhys),
                     cstrFormat(pdInfo->cstrReferPhys), cstrFormat(pdInfo->cstrTechnician),
                     pdInfo->bManualMode, cstrRespRate, pdInfo->bUserDiagnosis,
                     cstrFormat(pdInfo->cstrIFPComments), cstrFormat(pdInfo->cstrDiagnosis),
                     cstrFormat(pdInfo->cstrRecommendations), cstrFormat(pdInfo->cstrMedications),
                     pdInfo->bDelayedGastricEmptying, pdInfo->uDataPtsInBaseline,
                     pdInfo->bPause, pdInfo->uPostStimDataPoints,
                     cstrFormat(pdInfo->cstrStimMedDesc),
                     cstrFormat(pdInfo->cstrDiagnosisKeyPhrase), cstrFormat(pdInfo->cstrICDCode),
                     cstrFormat(pdInfo->cstrRecommendationKeyPhrase), 
                     pdInfo->bGastroparesis, pdInfo->bDyspepsiaBloatSatiety,
                     pdInfo->bDyspepsia, pdInfo->bAnorexia, pdInfo->bRefractoryGERD,
                     pdInfo->bWeightLoss, pdInfo->bPostDilationEval,
                     pdInfo->bEvalEffectMed, pdInfo->uFrequency, pdInfo->lStudyID);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        else
          vReport_err("iSave_study_data", m_sqlhStmt);
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      }
    }
  return(iRet);
  }

/********************************************************************
iSave_study_data - Save the study data to the "StudyData" database
                   table.  If the study is not yet in the table, add the
                   record, otherwise, do nothing.

  inputs: Pointer to a STUDY_DATA_RECORD structure that contains the study data
             and an invalid study ID.  On return, the Study ID is the new
             study ID obtained when the record was added to the table.

  return: SUCCESS if the data is saved to the table or nothing is done.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iSave_study_data(STUDY_DATA_RECORD *pStudy)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(pStudy->lStudyID == INV_LONG)
      { // Insert new study.
        cstrSQL.Format(SQL_INSERT_STUDY_DATA_1, pStudy->lPatientID, 
                     cstrFormat(pStudy->cstrDataFile), cstrFormat(pStudy->cstrDateOfStudy),
                     cstrFormat(pStudy->cstrMealTime), pStudy->uWaterAmount,
                     cstrFormat(pStudy->cstrWaterAmountUnits),
                     pStudy->cstrPeriodStart, pStudy->cstrPeriodLen, 
                     pStudy->cstrStartMinutes, pStudy->cstrEndMinutes,
                     pStudy->lTestMaxSignal, pStudy->lTestMinSignal, pStudy->bExtra10Min,
                     cstrFormat(pStudy->cstrSuggestedDiagnosis), pStudy->bBloating,
                     pStudy->bEarlySateity, pStudy->bEructus, pStudy->bGerd, pStudy->bNausea, 
                     pStudy->bPostPrandialFullness, pStudy->bVomiting, 
                     cstrFormat(pStudy->cstrAttendingPhysician), 
                     cstrFormat(pStudy->cstrReferingPhysician), cstrFormat(pStudy->cstrTechnician),
                     pStudy->bManualMode, pStudy->cstrRespirationRate, pStudy->bUserDiagnosis,
                     cstrFormat(pStudy->cstrIFPComments), cstrFormat(pStudy->cstrDiagnosis),
                     cstrFormat(pStudy->cstrRecommendations), cstrFormat(pStudy->cstrMedications),
                     pStudy->bDelayedGastricEmptying, pStudy->uiBLDataPoints,
                     pStudy->bPause, pStudy->uiPostStimDataPoints,
                     cstrFormat(pStudy->cstrStimulationMedium),
                     cstrFormat(pStudy->cstrDiagnosisKeyPhrase),
                     cstrFormat(pStudy->cstrICDCode),
                     cstrFormat(pStudy->cstrRecommendationKeyPhrase),
                     pStudy->bGastroparesis, pStudy->bDyspepsiaBloatSatiety,
                     pStudy->bDyspepsia, pStudy->bAnorexia, pStudy->bRefractoryGERD,
                     pStudy->bWeightLoss, pStudy->bPostDilationEval,
                     pStudy->bEvalEffectMed, pStudy->uFrequency);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err("iSave_study_data", m_sqlhStmt);
        sqlrFree_stmt_handle(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          { // Now find the StudyID that was just assigned.
          cstrSQL.Format("SELECT TOP 1 StudyID from StudyData where PatientID = %lu ORDER by StudyID DESC",
                         pStudy->lPatientID);
          if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
            {
            m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
            if(SQL_SUCCEEDED(m_sqlrSts))
              {
              m_sqlrSts = SQLFetch(m_sqlhStmt);
              if(SQL_SUCCEEDED(m_sqlrSts))
                {
                m_sqlrSts = SQLGetData(m_sqlhStmt, 1, SQL_C_LONG, &pStudy->lStudyID, 0, 0);
                if(SQL_SUCCEEDED(m_sqlrSts))
                  iRet = SUCCESS;
                else
                  vReport_err("iSave_study_data", m_sqlhStmt);
                }
              else
                vReport_err("iSave_study_data", m_sqlhStmt); // SQLFetch() failed.
              }
            else
              vReport_err("iSave_study_data", m_sqlhStmt); // SQLExecDirect() failed.
            sqlrFree_stmt_handle(m_sqlhStmt);
            }
          }
        }
      else
        vReport_err("iSave_study_data", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    else
      { // Update existing study
      cstrSQL.Format(SQL_UPDATE_STUDY_DATA_1, cstrFormat(pStudy->cstrDataFile),
                     cstrFormat(pStudy->cstrDateOfStudy), cstrFormat(pStudy->cstrMealTime),
                     pStudy->uWaterAmount, cstrFormat(pStudy->cstrWaterAmountUnits),
                     pStudy->cstrPeriodStart, pStudy->cstrPeriodLen,
                     pStudy->cstrStartMinutes, pStudy->cstrEndMinutes,
                     pStudy->lTestMaxSignal, pStudy->lTestMinSignal, pStudy->bExtra10Min,
                     cstrFormat(pStudy->cstrSuggestedDiagnosis), pStudy->bBloating,
                     pStudy->bEarlySateity, pStudy->bEructus, pStudy->bGerd, pStudy->bNausea,
                     pStudy->bPostPrandialFullness,
                     pStudy->bVomiting, cstrFormat(pStudy->cstrAttendingPhysician),
                     cstrFormat(pStudy->cstrReferingPhysician), cstrFormat(pStudy->cstrTechnician),
                     pStudy->bManualMode, pStudy->cstrRespirationRate,
                     pStudy->bUserDiagnosis, cstrFormat(pStudy->cstrIFPComments),
                     cstrFormat(pStudy->cstrDiagnosis),
                     cstrFormat(pStudy->cstrRecommendations), cstrFormat(pStudy->cstrMedications),
                     pStudy->bDelayedGastricEmptying, pStudy->uiBLDataPoints,
                     pStudy->bPause, pStudy->uiPostStimDataPoints,
                     cstrFormat(pStudy->cstrStimulationMedium),
                     cstrFormat(pStudy->cstrDiagnosisKeyPhrase),
                     cstrFormat(pStudy->cstrICDCode),
                     cstrFormat(pStudy->cstrRecommendationKeyPhrase),
                     pStudy->bGastroparesis, pStudy->bDyspepsiaBloatSatiety,
                     pStudy->bDyspepsia, pStudy->bAnorexia, pStudy->bRefractoryGERD,
                     pStudy->bWeightLoss, pStudy->bPostDilationEval,
                     pStudy->bEvalEffectMed, pStudy->uFrequency, pStudy->lStudyID);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        else
          vReport_err("iSave_study_data", m_sqlhStmt);
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      }
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
          Reference to a CString object that gets the date of the study.
          Pointer to a boolean to indicate if the study has been paused.
          The patient ID to get the study for.

  return: SUCCESS if there is a next study.
          FAIL if there is an error or the there are no more studies.
********************************************************************/
short int CDbaccess::iFind_next_study(short int iPos, FIND_NEXT_STUDY *pfns)
//CString &cstrDate, BOOL *pbPause, long lPatientID)
  {
  CString cstrSQL, cstrToken;
  short int iRet;
  //int iCurPos; //, iDay, iMonth, iYear;
  CTime ctSrchDate;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(iPos == FIRST_RECORD)
      { // Wants first record, so select all patients and return only first record.
      if((m_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
        {
        cstrSQL.Format("Select StudyID,DateOfStudy,Pause,BLDataPoints from StudyData \
                       where PatientID=%lu ORDER by DateOfStudy ASC", pfns->lPatientID);
        m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      else
        vReport_err("iFind_next_study", m_sqlhStmt1);
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
          // If searching by study date, check here.
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
          {
          vReport_err("iFind_next_study", m_sqlhStmt1);
          iRet = FAIL;
          }
        }
      }
    if(iRet == FAIL || iRet == SUCCESS_NO_DATA)
      sqlrFree_stmt_handle(m_sqlhStmt1); // Done getting records.
    }
  return(iRet);
  }

/********************************************************************
bLook_for_study_date - Look to see if this study date matches the
                       search study date.

  inputs: Reference to a CString object containing the date of the study.

  return: true if the study date match the search study date, otherwise false.
********************************************************************/
bool CDbaccess::bLook_for_study_date(CString cstrDate)
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
      if(cstraSrchDate[0].Find("*") >= 0)
        bRet = true;
      else
        {
        if(atoi(cstraSrchDate[0]) == atoi(cstraDate[0]))
          bRet = true;
        else
          bRet = false;
        }
      if(bRet == true)
        { // Month matches, look at day.
        if(cstraSrchDate[1].Find("*") >= 0)
          bRet = true;
        else
          {
          if(atoi(cstraSrchDate[1]) == atoi(cstraDate[1]))
            bRet = true;
          else
            bRet = false;
          }
        if(bRet == true)
          { // Month and day match, look at year.
          if((iIndex = cstraSrchDate[2].Find("*")) >= 0)
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
            if(atoi(cstraSrchDate[2]) == atoi(cstraDate[2]))
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
CTime CDbaccess::ctMake(CString cstrDate)
  {
  CString cstraDate[3]; // Month, day, year

  vParse_date(cstrDate, cstraDate, 3);
  return(CTime(atoi(cstraDate[2]), atoi(cstraDate[0]), atoi(cstraDate[1]), 0, 0, 0));
  }

/********************************************************************
vParse_date - Separate the month, day, year from a date string in the format: mm/dd/yyyy

  inputs: Reference to a CString object containing a date.
          CString array that will get the month, day, year respectively.

  return: None.
********************************************************************/
void CDbaccess::vParse_date(CString cstrDate, CString cstraDate[], int iLen)
  {
  int iCurPos;
  CString cstrToken;

  iCurPos = 0;
  cstrToken = cstrDate.Tokenize("/", iCurPos);
  for(int i = 0; cstrToken != "" && i < iLen; ++i)
    {
    cstraDate[i] = cstrToken;
    cstrToken = cstrDate.Tokenize("/", iCurPos);
    }
  }


/********************************************************************
iGet_next_data_file - Get the data file for the first or next patient
                      study in the "StudyInfo" database table.
                      Getting the first record causes an SQL Select
                      statement to get all the study records.

  inputs: Position in the record set from the table to get
            (first record or next record).
          Reference to a CString object that gets the data file for the study.

  return: SUCCESS if there is a next study.
          FAIL if there is an error or the there are no more studies.
********************************************************************/
short int CDbaccess::iGet_next_data_file(short int iPos, CString &cstrFile)
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
        cstrSQL.Format("Select DataFile from StudyData");
        m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        else
          vReport_err("iGet_next_data_file", m_sqlhStmt1);
        }
      else
        vReport_err("iGet_next_data_file", m_sqlhStmt1);
      }
    else
      iRet = SUCCESS; // Get records after the first
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt1);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        iGet_string(m_sqlhStmt1, 1, cstrFile);
        }
      else
        {
        if(m_sqlrSts == SQL_NO_DATA)
          iRet = SUCCESS_NO_DATA;
        else
          {
          vReport_err("iGet_next_data_file", m_sqlhStmt1);
          iRet = FAIL;
          }
        }
      }
    if(iRet == FAIL || iRet == SUCCESS_NO_DATA)
      sqlrFree_stmt_handle(m_sqlhStmt1); // Done getting records.
    }
  return(iRet);
  }

/********************************************************************
iGet_study_count - Count the number of records  in the "StudyInfo" database
                   table.  This executes a SQL statement to get all records
                   and then counts them.

  inputs: Pointer to an integer that gets the number of records.

  return: SUCCESS if the number of records could be counted.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iGet_study_count(int *piCount)
  {
  CString cstrSQL;
  short int iRet;
  int iCount;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format("Select StudyID from StudyData");
      m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {  // Fetch each record and count it.
        iCount = 0;
        while(true)
          {
          m_sqlrSts = SQLFetch(m_sqlhStmt1);
          if(!SQL_SUCCEEDED(m_sqlrSts) || m_sqlrSts == SQL_NO_DATA)
            break;
          ++iCount;
          }
        *piCount = iCount;
        iRet = SUCCESS;
        }
      else
        vReport_err("iGet_study_count", m_sqlhStmt1);
      sqlrFree_stmt_handle(m_sqlhStmt1); // Done getting records.
      }
    else
      vReport_err("iGet_study_count", m_sqlhStmt1);
    }
  return(iRet);
  }

///********************************************************************
//iGet_study_count_1 - Count the number of records  in the "StudyInfo" database
//                     table.  This executes a SQL statement to count the number
//                     of records.
//
//  inputs: Pointer to an integer that gets the number of records.
//
//  return: SUCCESS if the number of records could be counted.
//          FAIL if there is an error.
//********************************************************************/
//int CDbaccess::iGet_study_count_1(int *piCount)
//  {
//  CString cstrSQL;
//  short int iRet;
//
//  if((iRet = iDBConnect()) == SUCCESS)
//    {
//    iRet = FAIL;
//    if((m_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
//      {
//      cstrSQL.Format("Select Count(StudyID) from StudyData");
//      m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
//      if(SQL_SUCCEEDED(m_sqlrSts))
//        {  // Fetch each record and count it.
//        m_sqlrSts = SQLFetch(m_sqlhStmt1);
//        if(SQL_SUCCEEDED(m_sqlrSts))
//          {
//          SQLGetData(m_sqlhStmt1, 1, SQL_C_LONG, piCount, 0, 0);
//          iRet = SUCCESS;
//          }
//        }
//      sqlrFree_stmt_handle(m_sqlhStmt1); // Done getting records.
//      }
//    if(!SQL_SUCCEEDED(m_sqlrSts))
//      vReport_err("iGet_study_count", m_sqlhStmt1);
//    }
//  return(iRet);
//  }
//
///********************************************************************
//iGet_study_date - Get a study date gived the study ID.
//
//  inputs: Study ID.
//          Pointer to a CString object that gets the study date.
//
//  return: SUCCESS if the number of records could be counted.
//          FAIL if there is an error.
//********************************************************************/
//int CDbaccess::iGet_study_date(long lStudyID, CString &cstrStudyDate)
//  {
//  CString cstrSQL;
//  short int iRet;
//
//  if((iRet = iDBConnect()) == SUCCESS)
//    {
//    iRet = FAIL;
//    if((m_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
//      {
//      cstrSQL.Format("SELECT DateOfStudy from StudyData where StudyID=%lu", lStudyID);
//      m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
//      if(SQL_SUCCEEDED(m_sqlrSts))
//        {  // Fetch each record and count it.
//        m_sqlrSts = SQLFetch(m_sqlhStmt1);
//        if(SQL_SUCCEEDED(m_sqlrSts))
//          {
//          iGet_string(m_sqlhStmt1, 1, cstrStudyDate);
//          iRet = SUCCESS;
//          }
//        }
//      sqlrFree_stmt_handle(m_sqlhStmt1); // Done getting records.
//      }
//    if(!SQL_SUCCEEDED(m_sqlrSts))
//      vReport_err("iGet_study_count", m_sqlhStmt1);
//    }
//  return(iRet);
//  }

/********************************************************************
iFind_study - Find a study based on the name of the data file.

  inputs: CString object containing the data file name.

  return: Study ID if one is found, otherwise INV_LONG.
********************************************************************/
long CDbaccess::lFind_study(CString cstrDataFile)
  {
  CString cstrSQL;
  long lStudyID = INV_LONG;

  if(iDBConnect() == SUCCESS)
    {
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format("Select StudyID from StudyData where DataFile='%s'",
                     cstrFormat(cstrDataFile));
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          SQLGetData(m_sqlhStmt, 1, SQL_C_LONG, &lStudyID, 0, 0);
        }
      sqlrFree_stmt_handle(m_sqlhStmt); // Done getting records.
      }
    else
      vReport_err("lFind_study", m_sqlhStmt);
    }
  return(lStudyID);
  }

/********************************************************************
iGet_num_study_records - Get the number of records in the StudyData table.

  inputs: Pointer to the number of record.  Gets the number of records
            in the StudyData table.

  return: SUCCESS if the number of records. is retrieved.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iGet_num_study_records(long *plNumRecords)
  {
  short int iRet;
  CString cstrSQL;

  *plNumRecords = 0;
  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format("Select * from StudyData");
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        iRet = SUCCESS;
        while(true)
          {
          m_sqlrSts = SQLFetch(m_sqlhStmt);
          if(SQL_SUCCEEDED(m_sqlrSts))
            ++(*plNumRecords); // Get number of records.
          else
            break;
          }
        }
      sqlrFree_stmt_handle(m_sqlhStmt); // Done getting records.
      }
    else
      vReport_err("iGet_num_study_records", m_sqlhStmt);
    }
  return(iRet);
  }

/********************************************************************
iFind_next_diagnosis - Get the first or next patient diagnosis in the
                       "StudyInfo" database table for the first or next
                       diagnosis ordered by the date of the study.
                       Getting the first record causes an SQL Select statement
                       to get all the study records ordered by date of
                       study.

  inputs: Position in the record set from the table to get
            (first record or next record).
          Reference to a CString object that gets the date of the study.
          Reference to a CString object that gets the diagnosis.
          Pointer to a data location that gets the study ID.
          The patient ID to get the study for.

  return: SUCCESS if a diagnosis is retrieved.
          FAIL if there is an error or the there are no more diagnoses.
********************************************************************/
short int CDbaccess::iFind_next_diagnosis(short int iPos, CString &cstrDate,
CString &cstrDiagnosis, long *plStudyID, long lPatientID)
  {
  CString cstrSQL;
  short int iUserDiagnosis;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(iPos == FIRST_RECORD)
      { // Wants first record, so select all patients and return only first record.
      if((m_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
        {
        cstrSQL.Format(SQL_GET_NEXT_DIAGNOSIS, lPatientID);
        m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      else
        vReport_err("iFind_next_diagnosis", m_sqlhStmt1);
      }
    else
      iRet = SUCCESS; // Get records after the first
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt1);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        SQLGetData(m_sqlhStmt1, 1, SQL_C_LONG, plStudyID, 0, 0);
        iGet_string(m_sqlhStmt1, 2, cstrDate);
        SQLGetData(m_sqlhStmt1, 3, SQL_C_SHORT, &iUserDiagnosis, 0, 0);
          // If there is an alternate diagnosis, return it otherwise return the
          // suggested diagnosis.
        if(iUserDiagnosis != 0)
          iGet_long_string(m_sqlhStmt1, 4, cstrDiagnosis); // User diagnosis
        else
          iGet_string(m_sqlhStmt1, 5, cstrDiagnosis); // Suggested diagnosis
        }
      else
        iRet = FAIL;
      }
    if(iRet == FAIL)
      sqlrFree_stmt_handle(m_sqlhStmt1); // Done getting records.
    }
  return(iRet);
  }

/********************************************************************
iFind_next_medication - Get the first or next patient medication in the
                        "StudyInfo" database table for the first or next
                        medication ordered by the date of the study.
                        Getting the first record causes an SQL Select
                        statement to get all the study records ordered
                        by date of study.

  inputs: Position in the record set from the table to get
            (first record or next record).
          Reference to a CString object that gets the date of the study.
          Reference to a CString object that gets the medication.
          Pointer to a data location that gets the study ID.
          The patient ID to get the study for.

  return: SUCCESS if a medication is retrieved.
          FAIL if there is an error or the there are no more medications.
********************************************************************/
short int CDbaccess::iFind_next_medication(short int iPos, CString &cstrDate,
CString &cstrMedication, long *plStudyID, long lPatientID)
  {
  CString cstrSQL, cstrDiag;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(iPos == FIRST_RECORD)
      { // Wants first record, so select all patients and return only first record.
      if((m_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
        {
        cstrSQL.Format(SQL_GET_NEXT_MEDICATION, lPatientID);
        m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      else
        vReport_err("iFind_next_medication", m_sqlhStmt1);
      }
    else
      iRet = SUCCESS; // Get records after the first
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt1);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        SQLGetData(m_sqlhStmt1, 1, SQL_C_LONG, plStudyID, 0, 0);
        iGet_string(m_sqlhStmt1, 2, cstrDate);
        iGet_long_string(m_sqlhStmt1, 3, cstrMedication);
        }
      else
        iRet = FAIL;
      }
    if(iRet == FAIL)
      sqlrFree_stmt_handle(m_sqlhStmt1); // Done getting records.
    }
  return(iRet);
  }

/********************************************************************
iUpdate_medication - Update the medication in the "StudyInfo" table
                     for the specified study ID.

  inputs: Study ID
          CString object containing the medication.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iUpdate_medication(long lStudyID, CString cstrMedication)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    cstrSQL.Format(SQL_UPDATE_MEDICATION, cstrFormat(cstrMedication), lStudyID);
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(m_sqlrSts))
        vReport_err("iUpdate_medication", m_sqlhStmt);
      else
        iRet = SUCCESS;
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err("iUpdate_medication", m_sqlhStmt);
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// Facility table functions

/********************************************************************
iSave_facility_info - Save the facility information to the "Facility" database
                 `    table.  There is only one facility record in the table.

  inputs: Pointer to FACILITY_INFO that contains the facility ID and the
            information to write to the table.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iSave_facility_info(FACILITY_INFO *pfi)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(pfi->lFacilityID == INV_LONG)
      { // Add a new record to the database.
      cstrSQL.Format(SQL_INSERT_FACILITY_INFO, cstrFormat(pfi->cstrName),
                     cstrFormat(pfi->cstrAddr1), cstrFormat(pfi->cstrAddr2),
                     cstrFormat(pfi->cstrCity), cstrFormat(pfi->cstrState),
                     cstrFormat(pfi->cstrZipcode), cstrFormat(pfi->cstrPhone));
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err("iSave_facility_info", m_sqlhStmt);
        sqlrFree_stmt_handle(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          { // Now find the FacilityID that was just assigned.
          cstrSQL.Format("SELECT TOP 1 FacilityID from Facility ORDER by FacilityID DESC");
          if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
            {
            m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
            if(SQL_SUCCEEDED(m_sqlrSts))
              {
              m_sqlrSts = SQLFetch(m_sqlhStmt);
              if(SQL_SUCCEEDED(m_sqlrSts))
                {
                m_sqlrSts = SQLGetData(m_sqlhStmt, 1, SQL_C_LONG, &pfi->lFacilityID, 0, 0);
                if(!SQL_SUCCEEDED(m_sqlrSts))
                  vReport_err("iSave_facility_info", m_sqlhStmt);
                else
                  iRet = SUCCESS;
                }
              else
                vReport_err("iSave_facility_info", m_sqlhStmt); // SQLExecDirect() failed.
              }
            else
              vReport_err("iSave_facility_info", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
            sqlrFree_stmt_handle(m_sqlhStmt);
            }
          }
        }
      else
        vReport_err("iSave_facility_info", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    else
      { // Update record in the database.
      cstrSQL.Format(SQL_UPDATE_FACILITY_INFO, cstrFormat(pfi->cstrName),
                     cstrFormat(pfi->cstrAddr1), cstrFormat(pfi->cstrAddr2),
                     cstrFormat(pfi->cstrCity), cstrFormat(pfi->cstrState),
                     cstrFormat(pfi->cstrZipcode), cstrFormat(pfi->cstrPhone),
                     pfi->lFacilityID);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err("iSave_facility_info", m_sqlhStmt);
        else
          iRet = SUCCESS;
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      else
        vReport_err("iSave_facility_info", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/********************************************************************
iGet_facility_info - Get the  facility data from the "Facility" database
                     table for the facility ID specified in the input.

  inputs: Pointer to FACILITY_INFO that contains the facility ID for
            the record to be retieved.  The structure also gets the
            data retrieved from the table.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iGet_facility_info(FACILITY_INFO *pfi)
  {
  CString cstrSQL;
  int iField;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      if(pfi->lFacilityID == INV_LONG)
        cstrSQL.Format(SQL_GET_FACILITY_INFO);
      else
        cstrSQL.Format(SQL_GET_FACILITY_INFO_ID, pfi->lFacilityID);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        { // Now get all the values.
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          {
          iRet = SUCCESS;
          iField = 1;
          m_sqlrSts = SQLGetData(m_sqlhStmt, iField, SQL_C_LONG, &pfi->lFacilityID, 0, 0);
          iGet_string(m_sqlhStmt, ++iField, pfi->cstrName);
          iGet_string(m_sqlhStmt, ++iField, pfi->cstrAddr1);
          iGet_string(m_sqlhStmt, ++iField, pfi->cstrAddr2);
          iGet_string(m_sqlhStmt, ++iField, pfi->cstrCity);
          iGet_string(m_sqlhStmt, ++iField, pfi->cstrState);
          iGet_string(m_sqlhStmt, ++iField, pfi->cstrZipcode);
          iGet_string(m_sqlhStmt, ++iField, pfi->cstrPhone);
          }
        else if(m_sqlrSts != SQL_NO_DATA)
          vReport_err("iGet_facility_info", m_sqlhStmt);
        }
      else
        vReport_err("iGet_facility_info", m_sqlhStmt); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err("iGet_facility_info", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
    }
  return(iRet);
  }

/********************************************************************
iSave_event - Save an event to the "Events" database table.

  inputs: Study ID.
          Pointer to a CEvnt object.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iSave_event(long lStudyID, CEvnt *pEvent)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    if(iFind_event(lStudyID, pEvent->m_iDataPointIndex, pEvent->m_iPeriodType) == FAIL)
      { // Event doesn't exist, add it.
      cstrSQL.Format(SQL_INSERT_EVENTS, lStudyID, pEvent->m_iDataPointIndex,
                     pEvent->m_iSeconds, cstrFormat(pEvent->m_cstrIndications),
                     pEvent->m_iPeriodType, cstrFormat(pEvent->m_cstrDescription));
      }
    else
      { // Event already in database, update it.
      cstrSQL.Format(SQL_UPDATE_EVENTS, pEvent->m_iSeconds, cstrFormat(pEvent->m_cstrIndications),
                     pEvent->m_iPeriodType, cstrFormat(pEvent->m_cstrDescription),
                     lStudyID, pEvent->m_iDataPointIndex);
      }
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(m_sqlrSts))
        {
        iRet = FAIL;
        vReport_err("iSave_event", m_sqlhStmt);
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      {
      iRet = FAIL;
      vReport_err("iSave_event", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/********************************************************************
iSave_event - Save an event to the "Events" database table.

  inputs: Pointer to a EVENT_RECORD structure.  It contains the Study ID.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iSave_event(EVENT_RECORD *pEvent)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
  {
    cstrSQL.Format(SQL_INSERT_EVENTS, pEvent->lStudyID, pEvent->lDataPoint, pEvent->lSeconds,
                   cstrFormat(pEvent->cstrType), pEvent->iPeriodType,
                   cstrFormat(pEvent->cstrDescription));
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(m_sqlrSts))
        {
        iRet = FAIL;
        vReport_err("iSave_event", m_sqlhStmt);
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      {
      iRet = FAIL;
      vReport_err("iSave_event", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/********************************************************************
iGet_event - Get an Event from the Events table.
             If the StudyID is specified, the first event for the study
             ID is retrieved.  If the Study ID is invalid, the next
             event is retrieved for the original study ID.

  inputs: Study ID.
          Pointer to a CEvnt object which gets the event.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iGet_event(long lStudyID, CEvnt *pEvent)
  {
  CString cstrSQL;
  int iField;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(lStudyID != INV_LONG)
      { // Get first event for the study.
      if((m_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
        {
        cstrSQL.Format(SQL_GET_EVENTS, lStudyID);
        m_sqlrSts = SQLExecDirect(m_sqlhStmt1, (SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
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
        m_sqlrSts = SQLGetData(m_sqlhStmt1, iField, SQL_C_LONG,
                               &pEvent->m_iDataPointIndex, 0, 0);
        m_sqlrSts = SQLGetData(m_sqlhStmt1, ++iField, SQL_C_LONG,
                               &pEvent->m_iSeconds, 0, 0);
        iGet_string(m_sqlhStmt1, ++iField, pEvent->m_cstrIndications);
        SQLGetData(m_sqlhStmt1, ++iField, SQL_C_SHORT, &pEvent->m_iPeriodType, 0, 0);
        iGet_long_string(m_sqlhStmt1, ++iField, pEvent->m_cstrDescription);
        }
      else
        iRet = FAIL;
      }
    if(iRet == FAIL)
      { // Either error or no more records.
      if(m_sqlrSts != SQL_NO_DATA)
        vReport_err("iGet_event", m_sqlhStmt1); // Error.
      sqlrFree_stmt_handle(m_sqlhStmt1);
      }
    }
  return(iRet);
  }

/********************************************************************
iGet_event - Get an Event from the Events table.
             If the StudyID is specified, the first event for the study
             ID is retrieved.  If the Study ID is invalid, the next
             event is retrieved for the original study ID.

  inputs: Pointer to a EVENT_RECORD structure which gets the event.
              It contains the Study ID.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iGet_event(EVENT_RECORD *pEvent)
  {
  CString cstrSQL;
  int iField;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(pEvent->lStudyID != INV_LONG)
      { // Get first event for the study.
      if((m_sqlhStmt2 = sqlhGet_stmt_handle()) != NULL)
        {
        cstrSQL.Format(SQL_GET_EVENTS, pEvent->lStudyID);
        m_sqlrSts = SQLExecDirect(m_sqlhStmt2, (SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      }
    else
      iRet = SUCCESS; // Get next event for the study.
    if(iRet == SUCCESS)
      { // Now get all the values.
      m_sqlrSts = SQLFetch(m_sqlhStmt2);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        iRet = SUCCESS;
        iField = 3; // Skip first field (Event ID) and second field (study ID).
        m_sqlrSts = SQLGetData(m_sqlhStmt2, iField, SQL_C_LONG, &pEvent->lDataPoint, 0, 0);
        m_sqlrSts = SQLGetData(m_sqlhStmt2, ++iField, SQL_C_LONG, &pEvent->lSeconds, 0, 0);
        iGet_string(m_sqlhStmt2, ++iField, pEvent->cstrType);
        SQLGetData(m_sqlhStmt2, ++iField, SQL_C_SHORT, &pEvent->iPeriodType, 0, 0);
        iGet_long_string(m_sqlhStmt2, ++iField, pEvent->cstrDescription);
        }
      else
        iRet = FAIL;
      }
    if(iRet == FAIL)
      { // Either error or no more records.
      if(m_sqlrSts != SQL_NO_DATA)
        vReport_err("iGet_event", m_sqlhStmt2); // Error.
      sqlrFree_stmt_handle(m_sqlhStmt2);
      }
    }
  return(iRet);
  }

/********************************************************************
iFind_event - Determine if there is an event for the given study and
              data point.

  inputs: Study ID.
          Data point index.
          Period type (baseline, post-stimulation)

  return: SUCCESS if found.
          FAIL if not found.
********************************************************************/
short int CDbaccess::iFind_event(long lStudyID, int iDataPoint, int iPeriodType)
  {
  CString cstrSQL;
  short int iRet = FAIL;

  if(iDBConnect() == SUCCESS)
    {
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format(SQL_FIND_EVENT, lStudyID, iDataPoint, iPeriodType);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err("iFind_event", m_sqlhStmt);
    }
  return(iRet);
  }

/********************************************************************
iDelete_event - Delete one event from the "Events" database table for
                the specified study ID, data point and period type.

  inputs: Study ID.
          Data point index.
          Period type (baseline, post_stimulation

  return: SUCCESS if the record is deleted from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iDelete_event(long lStudyID, int iDataPoint, int iPeriodType)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    cstrSQL.Format(SQL_DELETE_EVENT, lStudyID, iDataPoint, iPeriodType);
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(m_sqlrSts))
        {
        iRet = FAIL;
        vReport_err("iDelete_event", m_sqlhStmt);
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      {
      iRet = FAIL;
      vReport_err("iDelete_event", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/********************************************************************
iDelete_events - Delete all events from the "Events" database table for
                 the specified study ID.

  inputs: Study ID, may be invalid (INV_LONG).

  return: SUCCESS if the records are delete2 from the table or if
            the study ID is invalid.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iDelete_events(long lStudyID)
  {
  CString cstrSQL;
  short int iRet;

  if(lStudyID == INV_LONG)
    iRet = SUCCESS;
  else
    {
    if((iRet = iDBConnect()) == SUCCESS)
      {
      cstrSQL.Format(SQL_DELETE_EVENTS, lStudyID);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts) && m_sqlrSts != SQL_NO_DATA)
          {
          iRet = FAIL;
          vReport_err("iDelete_events", m_sqlhStmt);
          }
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      else
        {
        iRet = FAIL;
        vReport_err("iDelete_events", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
        }
      }
    }
  return(iRet);
  }

/********************************************************************
iFind_next_whatif - Get the first or next date/time and description in
                    the "WhatIf" database table for the first or next
                    What If Scenario ordered by the date/time of the
                    What If.
                    Getting the first record causes an SQL Select
                    statement to get all the What If records ordered
                    by date/time.

  inputs: Position in the record set from the table to get
            (first record or next record).
            The StudyID member must contain the WhatIf StudyID on
              entry.
          Pointer to a WHAT_IF structure that gets the date/time and
            description.

  return: SUCCESS if there is a next WhatIf record for the Study ID.
          FAIL if there is an error or the there are no more studies.
********************************************************************/
short int CDbaccess::iFind_next_whatif(short int iPos, WHAT_IF *pwi)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(iPos == FIRST_RECORD)
      { // Wants first record, so select all patients and return only first record.
      if((m_sqlhStmt2 = sqlhGet_stmt_handle()) != NULL)
        {
        cstrSQL.Format(SQL_WHATIF_GET_DESC_ID, pwi->lStudyID);
        m_sqlrSts = SQLExecDirect(m_sqlhStmt2,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        else
          vReport_err("iFind_next_whatif", m_sqlhStmt2);
        }
      else
        vReport_err("iFind_next_whatif", m_sqlhStmt2);
      }
    else
      iRet = SUCCESS; // Get records after the first
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt2);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        SQLGetData(m_sqlhStmt2, 1, SQL_C_LONG, &pwi->lWhatIfID, 0, 0);
        iGet_string(m_sqlhStmt2, 2, pwi->cstrDateTime);
        iGet_string(m_sqlhStmt2, 3, pwi->cstrDescription);
        }
      else
        iRet = FAIL;
      }
    if(iRet == FAIL)
      sqlrFree_stmt_handle(m_sqlhStmt2); // Done getting records.
    }
  return(iRet);
  }

/********************************************************************
iFind_whatif - Find the What If Scenario specified by the ID.

  inputs: What If ID for getting the scenario.

  return: SUCCESS if the scenario is found.
          FAIL if one isn't found.
********************************************************************/
short int CDbaccess::iFind_whatif(long lWhatIfID)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format(SQL_WHATIF_FIND, lWhatIfID);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        iRet = SUCCESS;
      }
    else
      vReport_err("iFind_whatif", m_sqlhStmt1);
    sqlrFree_stmt_handle(m_sqlhStmt1); // Done getting records.
    }
  return(iRet);
  }

/********************************************************************
iFind_next_whatif - Get the first or next record in the "WhatIf" database table
                    for the study ID specified in the input structure on
                    the first call.
                    Getting the first record causes an SQL Select
                    statement to get all the What If records ordered
                    by date/time.

  inputs: Position in the record set from the table to get
            (first record or next record).
          Pointer to a WHAT_IF_RECORD structure that contains the Study ID
            on the first call and gets the record data.

  return: SUCCESS if there is a next WhatIf record for the Study ID.
          FAIL if there is an error or the there are no more studies.
********************************************************************/
short int CDbaccess::iFind_next_whatif(short int iPos, WHAT_IF_RECORD *pwi)
  {
  CString cstrSQL;
  short int iRet;
  int iField; //, iCurPos;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(iPos == FIRST_RECORD)
      { // Wants first record, so select all patients and return only first record.
      if((m_sqlhStmt2 = sqlhGet_stmt_handle()) != NULL)
        {
        cstrSQL.Format(SQL_WHATIF_GET_STUDY_ID, pwi->lStudyID);
        m_sqlrSts = SQLExecDirect(m_sqlhStmt2,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        else
          vReport_err("iFind_next_whatif", m_sqlhStmt2);
        }
      else
        vReport_err("iFind_next_whatif", m_sqlhStmt2);
      }
    else
      iRet = SUCCESS; // Get records after the first
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt2);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        //iCurPos = 0;
        //while(true)
        //  {
          iField = 1;
          SQLGetData(m_sqlhStmt2, iField, SQL_C_LONG, &pwi->lWhatIfID, 0, 0);
          SQLGetData(m_sqlhStmt2, ++iField, SQL_C_LONG, &pwi->lStudyID, 0, 0);
          iGet_string(m_sqlhStmt2, ++iField, pwi->cstrDescription);
          iGet_string(m_sqlhStmt2, ++iField, pwi->cstrDateTimeSaved);
          SQLGetData(m_sqlhStmt2, ++iField, SQL_C_SHORT, &pwi->uNumPeriods, 0, 0);
          iGet_string(m_sqlhStmt2, ++iField, pwi->cstrPeriodStart); // Start period
          iGet_string(m_sqlhStmt2, ++iField, pwi->cstrPeriodLen); // Length of period
          iGet_string(m_sqlhStmt2, ++iField, pwi->cstrStartMinutes); // Start minutes
          iGet_string(m_sqlhStmt2, ++iField, pwi->cstrEndMinutes); // End minutes
          iGet_string(m_sqlhStmt2, ++iField, pwi->cstrRespirationRate); // Respiration rate
          SQLGetData(m_sqlhStmt2, ++iField, SQL_C_LONG, &pwi->uFrequency, 0, 0);

          // Frequency is a new field and may not have ever been set to a valid value.
          // If frequency is not a valid value, default it to 15cpm.
          if(pwi->uFrequency == 0 || pwi->uFrequency > 20000)
            pwi->uFrequency = 15;

          //// If searching by study date, check here.
          //if(bLook_for_study_date(pwi->cstrDateTimeSaved, m_sqlhStmt2, m_sqlrSts, iCurPos) == true)
          //  break;
          //else
          //  {
          //  if(!SQL_SUCCEEDED(m_sqlrSts))
          //    {
          //    iRet = FAIL;
          //    break;
          //    }
          //  }
          //}
        }
      else
        iRet = FAIL;
      }
    if(iRet == FAIL)
      sqlrFree_stmt_handle(m_sqlhStmt2); // Done getting records.
    }
  return(iRet);
  }

/********************************************************************
iSave_whatif_data - Save the What If data to the "WhatIf" database
                    table.  If the What If is not yet in the table, add the
                    record, otherwise, update the record.

  inputs: Pointer to WHAT_IF that contains the study ID plus the data
            to save.  If this is a new What If, the WhatIf ID member
            of this structure gets the new WhatIf ID.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iSave_whatif_data(WHAT_IF *pwi)
  {
  CString cstrSQL, cstrStartMinutes, cstrEndMinutes, cstrPeriodStart, cstrPeriodLen;
  CString cstrRespRate, cstrTemp;
  short int iRet, iIndex;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
      //// TODO Put in loops for array limit.
      // These data items are arrays, but saved as comma delimited strings in the
      // database.
    cstrStartMinutes.Empty();
    cstrEndMinutes.Empty();
    cstrPeriodStart.Empty();
    cstrPeriodLen.Empty();
    for(iIndex = 0; iIndex < (MAX_TOTAL_PERIODS - 1); ++iIndex)
      {
      cstrTemp.Format("%.1f,", pwi->fGoodMinStart[iIndex]);
      cstrStartMinutes += cstrTemp;
      cstrTemp.Format("%.1f,", pwi->fGoodMinEnd[iIndex]);
      cstrEndMinutes += cstrTemp;
      cstrTemp.Format("%.1f,", pwi->fPeriodStart[iIndex]);
      cstrPeriodStart += cstrTemp;
      cstrTemp.Format("%.1f,", pwi->fPeriodLen[iIndex]);
      cstrPeriodLen += cstrTemp;
      cstrTemp.Format("%d,", pwi->uRespRate[iIndex]);
      cstrRespRate += cstrTemp;
      }
      // Now do last one without trailing comma.
    cstrTemp.Format("%.1f", pwi->fGoodMinStart[iIndex]);
    cstrStartMinutes += cstrTemp;
    cstrTemp.Format("%.1f", pwi->fGoodMinEnd[iIndex]);
    cstrEndMinutes += cstrTemp;
    cstrTemp.Format("%.1f", pwi->fPeriodStart[iIndex]);
    cstrPeriodStart += cstrTemp;
    cstrTemp.Format("%.1f", pwi->fPeriodLen[iIndex]);
    cstrPeriodLen += cstrTemp;
    cstrTemp.Format("%d", pwi->uRespRate[iIndex]);
    cstrRespRate += cstrTemp;
    if(pwi->lWhatIfID == INV_LONG || iFind_whatif(pwi->lWhatIfID) == FAIL)
      { // Insert new study.
      cstrSQL.Format(SQL_WHATIF_INSERT_RECORD, pwi->lStudyID, cstrFormat(pwi->cstrDescription),
                     cstrFormat(pwi->cstrDateTime), pwi->uNumPeriods, cstrPeriodStart,
                     cstrPeriodLen, cstrStartMinutes, cstrEndMinutes, cstrRespRate, pwi->uFrequency);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err("iSave_whatif_data", m_sqlhStmt);
        sqlrFree_stmt_handle(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          { // Now find the WhatIf ID that was just assigned.
          cstrSQL.Format("SELECT TOP 1 WhatIfID from WhatIf where StudyID = %lu ORDER by DateTimeSaved DESC",
                         pwi->lStudyID);
          if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
            {
            m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
            if(SQL_SUCCEEDED(m_sqlrSts))
              {
              m_sqlrSts = SQLFetch(m_sqlhStmt);
              if(SQL_SUCCEEDED(m_sqlrSts))
                {
                m_sqlrSts = SQLGetData(m_sqlhStmt, 1, SQL_C_LONG, &pwi->lWhatIfID, 0, 0);
                if(SQL_SUCCEEDED(m_sqlrSts))
                  iRet = SUCCESS;
                else
                  vReport_err("iSave_whatif_data", m_sqlhStmt);
                }
              else
                vReport_err("iSave_whatif_data", m_sqlhStmt); // SQLFetch() failed.
              }
            else
              vReport_err("iSave_whatif_data", m_sqlhStmt); // SQLExecDirect() failed.
            sqlrFree_stmt_handle(m_sqlhStmt);
            }
          }
        }
      else
        vReport_err("iSave_whatif_data", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    else
      { // Update existing study
      cstrSQL.Format(SQL_WHATIF_UPDATE_RECORD, pwi->lStudyID, cstrFormat(pwi->cstrDescription),
                     cstrFormat(pwi->cstrDateTime), pwi->uNumPeriods, cstrPeriodStart,
                     cstrPeriodLen, cstrStartMinutes, cstrEndMinutes, cstrRespRate,
                     pwi->uFrequency, pwi->lWhatIfID);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        else
          vReport_err("iSave_whatif_data", m_sqlhStmt);
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      }
    }
  return(iRet);
  }

/********************************************************************
iSave_whatif_data - Save the What If data to the "WhatIf" database
                    table.  If the What If is not yet in the table, add the
                    record, otherwise, do nothing.

  inputs: Pointer to a WHAT_IF_RECORD structure that contains an invalid
            WhatIF ID plus the data to save.

  return: SUCCESS if the data is saved to the table or nothing is done.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iSave_whatif_data(WHAT_IF_RECORD *pwi)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if(pwi->lWhatIfID == INV_LONG)
      { // Insert new study.
      cstrSQL.Format(SQL_WHATIF_INSERT_RECORD, pwi->lStudyID, cstrFormat(pwi->cstrDescription),
                     cstrFormat(pwi->cstrDateTimeSaved), pwi->uNumPeriods,
                     pwi->cstrPeriodStart, pwi->cstrPeriodLen,
                     pwi->cstrStartMinutes, pwi->cstrEndMinutes, pwi->cstrRespirationRate,
                     pwi->uFrequency);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          vReport_err("iSave_whatif_data", m_sqlhStmt);
        else
          iRet = SUCCESS;
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      else
        vReport_err("iSave_whatif_data", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/********************************************************************
iGet_whatif_data - Get all the record values for the What If record
                   specified by the What If ID from the "WhatIf" database
                   table.  The What If ID is specified in the input.

  inputs: Pointer to WHAT_IF that contains the What If ID.

  return: SUCCESS if the WhatIf data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iGet_whatif_data(WHAT_IF *pwi)
  {
  CString cstrSQL, cstrTemp;
  int iField;
  unsigned short uCnt;
  short int iIndex, iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format(SQL_WHATIF_GET_RECORD, pwi->lWhatIfID);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        { // Now get all the values.
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          {
          iRet = SUCCESS;
          iField = 1;
          SQLGetData(m_sqlhStmt, iField, SQL_C_LONG, &pwi->lWhatIfID, 0, 0);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &pwi->lStudyID, 0, 0);
          iGet_string(m_sqlhStmt, ++iField, pwi->cstrDescription);
          iGet_string(m_sqlhStmt, ++iField, pwi->cstrDateTime);
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_SHORT, &pwi->uNumPeriods, 0, 0);
          iGet_string(m_sqlhStmt, ++iField, cstrTemp); // Start period
          for(uCnt = 0; uCnt < MAX_TOTAL_PERIODS; ++uCnt)
            {
            if(cstrTemp.IsEmpty() == FALSE)
              {
              iIndex = cstrTemp.Find(",", 0);
              if(iIndex < 0)
                iIndex = cstrTemp.GetLength(); // Last one.
              pwi->fPeriodStart[uCnt] = (float)atof(cstrTemp.Left(iIndex));
              cstrTemp.Delete(0, iIndex + 1);
              }
            else
              pwi->fPeriodStart[uCnt] = INV_PERIOD_START;
            }
          iGet_string(m_sqlhStmt, ++iField, cstrTemp); // Length of period
          for(uCnt = 0; uCnt < MAX_TOTAL_PERIODS; ++uCnt)
            {
            if(cstrTemp.IsEmpty() == FALSE)
              {
              iIndex = cstrTemp.Find(",", 0);
              if(iIndex < 0)
                iIndex = cstrTemp.GetLength(); // Last one.
              pwi->fPeriodLen[uCnt] = (float)atof(cstrTemp.Left(iIndex));
              cstrTemp.Delete(0, iIndex + 1);
              }
            else
              pwi->fPeriodLen[uCnt] = INV_PERIOD_LEN;
            }
          iGet_string(m_sqlhStmt, ++iField, cstrTemp); // Start minutes
          for(uCnt = 0; uCnt < MAX_TOTAL_PERIODS; ++uCnt)
            {
            if(cstrTemp.IsEmpty() == FALSE)
              {
              iIndex = cstrTemp.Find(",", 0);
              if(iIndex < 0)
                iIndex = cstrTemp.GetLength(); // Last one.
              pwi->fGoodMinStart[uCnt] = (float)atof(cstrTemp.Left(iIndex));
              cstrTemp.Delete(0, iIndex + 1);
              }
            else
              pwi->fGoodMinStart[uCnt] = INV_MINUTE;
            }
          iGet_string(m_sqlhStmt, ++iField, cstrTemp); // End minutes
          for(uCnt = 0; uCnt < MAX_TOTAL_PERIODS; ++uCnt)
            {
            if(cstrTemp.IsEmpty() == FALSE)
              {
              iIndex = cstrTemp.Find(",", 0);
              if(iIndex < 0)
                iIndex = cstrTemp.GetLength(); // Last one.
              pwi->fGoodMinEnd[uCnt] = (float)atof(cstrTemp.Left(iIndex));
              cstrTemp.Delete(0, iIndex + 1);
              }
            else
              pwi->fGoodMinEnd[uCnt] = INV_MINUTE;
            }
          iGet_string(m_sqlhStmt, ++iField, cstrTemp); // Respiration rate
          for(uCnt = 0; uCnt < MAX_TOTAL_PERIODS; ++uCnt)
            {
            if(cstrTemp.IsEmpty() == FALSE)
              {
              iIndex = cstrTemp.Find(",", 0);
              if(iIndex < 0)
                iIndex = cstrTemp.GetLength();
              pwi->uRespRate[uCnt] = (unsigned short)atoi(cstrTemp.Left(iIndex));
              cstrTemp.Delete(0, iIndex + 1);
              }
            else
              pwi->uRespRate[uCnt] = 0;
            }
          SQLGetData(m_sqlhStmt, ++iField, SQL_C_LONG, &pwi->uFrequency, 0, 0);
                    // Frequency is a new field and may not have ever been set to a valid value.
          // If frequency is not a valid value, default it to 15cpm.
          if(pwi->uFrequency == 0 || pwi->uFrequency > 20000)
            pwi->uFrequency = 15;
          }
        }
      else
        vReport_err("iGet_whatif_data", m_sqlhStmt); // SQLExecDirect failed
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err("iGet_whatif_data", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
    }
  return(iRet);
  }

/********************************************************************
iDelete_whatif - Delete one record from the "WhatIf" database table for
                 the specified What If ID.

  inputs: What If ID.

  return: SUCCESS if the record is deleted from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iDelete_whatif(long lWhatIfID)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    cstrSQL.Format(SQL_WHATIF_DELETE, lWhatIfID);
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(m_sqlrSts))
        {
        iRet = FAIL;
        vReport_err("iDelete_whatif", m_sqlhStmt);
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      {
      iRet = FAIL;
      vReport_err("iDelete_whatif", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/********************************************************************
iDelete_whatif_for_study - Delete "WhatIf" records for the specified
                           Study ID.

  inputs: Study ID.

  return: SUCCESS if the records are deleted from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iDelete_whatif_for_study(long lStudyID)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    cstrSQL.Format(SQL_WHATIF_DELETE_FOR_STUDY, lStudyID);
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(m_sqlrSts) && m_sqlrSts != SQL_NO_DATA)
        {
        iRet = FAIL;
        vReport_err("iDelete_whatif_for_study", m_sqlhStmt);
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      {
      iRet = FAIL;
      vReport_err("iDelete_whatif_for_study", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// Methods for accessing the DiagnosisDesc table

/********************************************************************
iSave_diagnosis_desc - Save a diagnosis key phrase and description to the
                       "DiagnosisDesc" database table.
                       If the diagnosis ID already exists, it is updated with
                       the description.

  inputs: Pointer to a DIAGNOSIS_DESC_RECORD structure with data to be saved.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iSave_diagnosis_desc(DIAGNOSIS_DESC_RECORD *pddr)
  {
  CString cstrSQL;
  short int iRet;
  DIAGNOSIS_DESC_RECORD ddr;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    ddr.cstrDiagnosisKeyPhrase = pddr->cstrDiagnosisKeyPhrase;
    if(iFind_diagnosis_desc(&ddr) == FAIL)
      { // Diagnosis doesn't exist, add it.
      cstrSQL.Format(SQL_INSERT_DIAGNOSIS_DESC, pddr->cstrDiagnosisKeyPhrase,
        cstrFormat(pddr->cstrDiagDesc));
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          {
          iRet = FAIL;
          vReport_err("iSave_diagnosis_desc", m_sqlhStmt);
          }
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      else
        {
        iRet = FAIL;
        vReport_err("iSave_diagnosis_desc", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
        }
      }
    else
      { // Diagnosis already in database, update it.
      iRet = iUpdate_diagnosis_desc(pddr->cstrDiagnosisKeyPhrase, pddr);
      }
    }
  return(iRet);
  }

/********************************************************************
iUpdate_diagnosis_desc - Update a DiagnosisDesc record from the existing
                         values to the new values in the "DiagnosisDesc"
                         database table.

  inputs: CString object containing the diagnosis key phrase
          Pointer to a new DIAGNOSIS_DESC_RECORD structure.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iUpdate_diagnosis_desc(CString cstrKeyPhrase, DIAGNOSIS_DESC_RECORD *pddr)
  {
  CString cstrSQL;
  short int iRet;
  DIAGNOSIS_DESC_RECORD ddr;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    cstrSQL.Format(SQL_UPDATE_DIAGNOSIS_DESC, cstrFormat(pddr->cstrDiagDesc),
      pddr->cstrDiagnosisKeyPhrase, cstrKeyPhrase);
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(m_sqlrSts))
        {
        iRet = FAIL;
        vReport_err("iUpdate_diagnosis_desc", m_sqlhStmt);
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      {
      iRet = FAIL;
      vReport_err("iUpdate_diagnosis_desc", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }


/********************************************************************
iGet_diagnosis_desc - Get a diagnosis and its key phrase from the DiagnosisDesc
                      table.
             
  inputs: Position in the record set from the table to get
            (first record or next record).
          Pointer to a DIAGNOSIS_DESC_RECORD structure which gets the
            description and key phrase.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iGet_diagnosis_desc(short int iPos, DIAGNOSIS_DESC_RECORD *pddr)
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
        cstrSQL = SQL_GET_DIAGNOSIS_DESC_ORDER;
        m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      else
        vReport_err("iGet_diagnosis_desc", m_sqlhStmt1);
      }
    else
      iRet = SUCCESS; // Get records after the first
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt1);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        iGet_string(m_sqlhStmt1, 1, pddr->cstrDiagnosisKeyPhrase);
        iGet_long_string(m_sqlhStmt1, 2, pddr->cstrDiagDesc);
        }
      else
        iRet = FAIL;
      }
    if(iRet == FAIL)
      sqlrFree_stmt_handle(m_sqlhStmt1); // Done getting records.
    }
  return(iRet);
  }

/********************************************************************
iFind_diagnosis_desc - Get a diagnosis from the DiagnosisDesc table for
                       key phrase specified in the input structure.
             
  inputs: Pointer to a DIAGNOSIS_DESC_RECORD structure which contains the
            diagnosis key phrase to search for and gets the description.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iFind_diagnosis_desc(DIAGNOSIS_DESC_RECORD *pddr)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format(SQL_FIND_DIAGNOSIS_DESC, pddr->cstrDiagnosisKeyPhrase);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        iRet = SUCCESS;
      }
    else
      vReport_err("iFind_diagnosis_desc", m_sqlhStmt1);
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        iGet_string(m_sqlhStmt, 1, pddr->cstrDiagnosisKeyPhrase);
        iGet_long_string(m_sqlhStmt, 2, pddr->cstrDiagDesc);
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
iDelete_diagnosis_desc - Delete the record from the "DiagnosisDesc"
                         database table for the specified diagnosis ID.

  inputs: diagnosis key phrase.

  return: SUCCESS if the record is deleted from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iDelete_diagnosis_desc(CString cstrKeyPhrase)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    cstrSQL.Format(SQL_DELETE_DIAGNOSIS_DESC, cstrKeyPhrase);
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(m_sqlrSts))
        {
        iRet = FAIL;
        vReport_err("iDelete_diagnosis_desc", m_sqlhStmt);
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      {
      iRet = FAIL;
      vReport_err("iDelete_diagnosis_desc", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// End of Methods for accessing the DiagnosisDesc table

/////////////////////////////////////////////////////////////////////
//// Methods for accessing the ICDCode table

/********************************************************************
iSave_ICDCode - Save an ICD code to the "ICDCode" database table.

  inputs: Pointer to a ICD_CODE_RECORD structure with data to be saved.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iSave_ICDCode(ICD_CODE_RECORD *pICD)
  {
  CString cstrSQL;
  short int iRet;
  ICD_CODE_RECORD icdRecord;

  if((iRet = iDBConnect()) == SUCCESS)
  {
    icdRecord.cstrICDCode = pICD->cstrICDCode;
    if(iFind_ICDCode(&icdRecord) == FAIL)
      { // ICD Code doesn't exist, add it.
      cstrSQL.Format(SQL_INSERT_ICD_CODE, pICD->cstrICDCode);
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          {
          iRet = FAIL;
          vReport_err("iSave_ICDCode", m_sqlhStmt);
          }
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      else
        {
        iRet = FAIL;
        vReport_err("iSave_ICDCode", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
        }
      }
    }
  return(iRet);
  }

/********************************************************************
iUpdate_ICDCode - Update an ICD code from the existing to the new ICD Code
                  in the "ICDCode" database table.

  inputs: Pointer to an existing ICD_CODE_RECORD structure.
          Pointer to a new ICD_CODE_RECORD structure.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iUpdate_ICDCode(ICD_CODE_RECORD *pICDExisting, ICD_CODE_RECORD *pICDNew)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    cstrSQL.Format(SQL_UPDATE_ICD_CODE, pICDNew->cstrICDCode, pICDExisting->cstrICDCode);
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(m_sqlrSts))
        {
        iRet = FAIL;
        vReport_err("iUpdate_ICDCode", m_sqlhStmt);
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      {
      iRet = FAIL;
      vReport_err("iUpdate_ICDCode", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/********************************************************************
iGet_ICDCode - Get an ICD code from the "ICDCode" database table.
             
  inputs: Position in the record set from the table to get
            (first record or next record).
          Pointer to a ICD_CODE_RECORD structure which gets the ID.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iGet_ICDCode(short int iPos, ICD_CODE_RECORD *pICD)
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
        cstrSQL = SQL_GET_ICD_CODE_ORDER;
        m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      else
        vReport_err("iGet_ICDCode", m_sqlhStmt1);
      }
    else
      iRet = SUCCESS; // Get records after the first
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt1);
      if(SQL_SUCCEEDED(m_sqlrSts))
      {
        iGet_string(m_sqlhStmt1, 1, pICD->cstrICDCode);
      }
      else
        iRet = FAIL;
      }
    if(iRet == FAIL)
      sqlrFree_stmt_handle(m_sqlhStmt1); // Done getting records.
    }
  return(iRet);
  }

/********************************************************************
iFind_ICDCode - Find the specified ICD code in the "ICDCode" database table.
             
  inputs: Pointer to a ICD_CODE_RECORD structure which contains the
            ICD code.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iFind_ICDCode(ICD_CODE_RECORD *pICD)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format(SQL_FIND_ICD_CODE, pICD->cstrICDCode);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        iRet = SUCCESS;
      }
    else
      vReport_err("iFind_ICDCode", m_sqlhStmt1);
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        iGet_string(m_sqlhStmt, 1, pICD->cstrICDCode);
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
iDelete_ICDCode - Delete the record from the "ICDCode" database table
                  for the specified ICD code.

  inputs: ICD code.

  return: SUCCESS if the record is deleted from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iDelete_ICDCode(CString cstrID)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    cstrSQL.Format(SQL_DELETE_ICD_CODE, cstrID);
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(m_sqlrSts))
        {
        iRet = FAIL;
        vReport_err("iDelete_ICDCode", m_sqlhStmt);
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      {
      iRet = FAIL;
      vReport_err("iDelete_ICDCode", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// End of Methods for accessing the DiagnosisDesc table

/////////////////////////////////////////////////////////////////////
//// Methods for accessing the Recommendations table

/********************************************************************
iSave_recomm_desc - Save a recommendation key phrase description to the
                    "Recommendations" database table.
                    If the recommendation key phrase already exists,
                    it is updated with the description.

  inputs: Pointer to a RECOMMENDATIONS_RECORD structure with data to be saved.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iSave_recomm_desc(RECOMMENDATIONS_RECORD *prr)
  {
  CString cstrSQL;
  short int iRet;
  RECOMMENDATIONS_RECORD rr;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    rr.cstrKeyPhrase = prr->cstrKeyPhrase;
    if(iFind_recommendation(&rr) == FAIL)
      { // Event doesn't exist, add it.
      cstrSQL.Format(SQL_INSERT_RECOMMENDATIONS, prr->cstrKeyPhrase,
        cstrFormat(prr->cstrDescription));
      if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
        {
        m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(!SQL_SUCCEEDED(m_sqlrSts))
          {
          iRet = FAIL;
          vReport_err("iSave_recomm_desc", m_sqlhStmt);
          }
        sqlrFree_stmt_handle(m_sqlhStmt);
        }
      else
        {
        iRet = FAIL;
        vReport_err("iSave_recomm_desc", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
        }
      }
    else
      { // Recommendation already in database, update it.
      iRet = iUpdate_recommendation(prr->cstrKeyPhrase, prr);
      }
    }
  return(iRet);
  }

/********************************************************************
iUpdate_recommendation - Update a Recommendations record from the existing
                         values to the new values in the "Recommendations"
                         database table.

  inputs: CString object containing the key phrase
          Pointer to a new RECOMMENDATIONS_RECORD structure.

  return: SUCCESS if the data is saved to the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iUpdate_recommendation(CString cstrKeyPhrase, RECOMMENDATIONS_RECORD *prr)
  {
  CString cstrSQL;
  short int iRet;
  DIAGNOSIS_DESC_RECORD ddr;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    cstrSQL.Format(SQL_UPDATE_RECOMMENDATIONS, cstrFormat(prr->cstrDescription),
      prr->cstrKeyPhrase, cstrKeyPhrase);
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(m_sqlrSts))
        {
        iRet = FAIL;
        vReport_err("iUpdate_recommendation", m_sqlhStmt);
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      {
      iRet = FAIL;
      vReport_err("iUpdate_recommendation", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }


/********************************************************************
iGet_recomm_desc - Get a diagnosis and its ID from the Recommendations table.
             
  inputs: Position in the record set from the table to get
            (first record or next record).
          Pointer to a RECOMMENDATIONS_RECORD structure which gets the
            description and ID.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iGet_recomm_desc(short int iPos, RECOMMENDATIONS_RECORD *prr)
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
        cstrSQL = SQL_GET_RECOMMENDATIONS_ORDER;
        m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
        if(SQL_SUCCEEDED(m_sqlrSts))
          iRet = SUCCESS;
        }
      else
        vReport_err("iGet_recomm_desc", m_sqlhStmt1);
      }
    else
      iRet = SUCCESS; // Get records after the first
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt1);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        iGet_string(m_sqlhStmt1, 1, prr->cstrKeyPhrase);
        iGet_long_string(m_sqlhStmt1, 2, prr->cstrDescription);
        }
      else
        iRet = FAIL;
      }
    if(iRet == FAIL)
      sqlrFree_stmt_handle(m_sqlhStmt1); // Done getting records.
    }
  return(iRet);
  }

/********************************************************************
iFind_recommendation - Get a diagnosis from the Recommendations table for
                       ID specified in the input structure.
             
  inputs: Pointer to a RECOMMENDATIONS_RECORD structure which contains the
            key phrase to search for and gets the description.

  return: SUCCESS if the data is retrieved from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iFind_recommendation(RECOMMENDATIONS_RECORD *prr)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL.Format(SQL_FIND_RECOMMENDATIONS, prr->cstrKeyPhrase);
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        iRet = SUCCESS;
      }
    else
      vReport_err("iFind_recommendation", m_sqlhStmt1);
    if(iRet == SUCCESS)
      { // Get the data for the record.
      m_sqlrSts = SQLFetch(m_sqlhStmt);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        iGet_string(m_sqlhStmt, 1, prr->cstrKeyPhrase);
        iGet_long_string(m_sqlhStmt, 2, prr->cstrDescription);
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
iDelete_recommendation - Delete the record from the "Recommendations"
                         database table for the specified key phrase.

  inputs: diagnosis key phrase.

  return: SUCCESS if the record is deleted from the table.
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iDelete_recommendation(CString cstrKeyPhrase)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    cstrSQL.Format(SQL_DELETE_RECOMMENDATIONS, cstrKeyPhrase);
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(!SQL_SUCCEEDED(m_sqlrSts))
        {
        iRet = FAIL;
        vReport_err("iDelete_recommendation", m_sqlhStmt);
        }
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      {
      iRet = FAIL;
      vReport_err("iDelete_recommendation", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
      }
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
//// End of Methods for accessing the DiagnosisDesc table



/********************************************************************
cstrGet_db_file_path - Get the complete path and file name of the database
                       from the SQL connection string returned after
                       connecting to the database.

  inputs: None.

  return: If not the local database, the complete path and file name
            of the database.
          If the local database, DSN_DEFAULT.
          Empty string if not connected yet.
********************************************************************/
CString CDbaccess::cstrGet_db_file_path()
  {
  CString cstrFilePath, cstrTemp;
  int iIndex;

  if(m_bDBConnected == true)
    { // Connected.
      // Parse m_sqlOutConnStr[] for the path and file name.
    cstrTemp = m_sqlOutConnStr;
    if((iIndex = cstrTemp.Find("DBQ=")) >= 0)
      {
      cstrTemp = cstrTemp.Right(cstrTemp.GetLength() - (iIndex + 3) - 1);
      if((iIndex = cstrTemp.Find(";")) > 0)
        cstrTemp = cstrTemp.Left(iIndex);
      cstrFilePath = cstrTemp;
      }
    else
      { // Local database connection.
      cstrFilePath = DSN_DEFAULT;
      }
    }
  else
    cstrFilePath = "";
  return(cstrFilePath);
  }

/********************************************************************
cstrGet_conn_path - Get the connection path of the database from the
                    SQL connection string returned after connecting
                    to the database.

  inputs: None.

  return: If not the local database, the complete path of the connected
            database.
          Empty string if the local database or not connected.
********************************************************************/
CString CDbaccess::cstrGet_conn_path()
  {
  CString cstrPath, cstrTemp;
  int iIndex;

  cstrPath.Empty();
  if(m_bDBConnected == true)
    { // Connected.
      // Parse m_sqlOutConnStr[] for the path and file name.
    cstrTemp = m_sqlOutConnStr;
    if((iIndex = cstrTemp.Find("DBQ=")) >= 0)
      {
      cstrTemp = cstrTemp.Right(cstrTemp.GetLength() - (iIndex + 3) - 1);
      if((iIndex = cstrTemp.Find(";")) > 0)
        cstrTemp = cstrTemp.Left(iIndex);
      cstrPath = cstrTemp;
        // Strip out the file name.
      if((iIndex = cstrPath.ReverseFind('\\')) >= 0)
        cstrPath = cstrPath.Right(cstrPath.GetLength() - iIndex - 1);
      }
    }
  return(cstrPath);
  }

/********************************************************************
bDBFileExists - Determine if the database exists.

  inputs: Name of the database and path (if not the default database).

  return: true if the database exists, otherwise false.
********************************************************************/
bool CDbaccess::bDBFileExists(CString cstrDB)
  {
  bool bRet;
  CString cstrFile, cstrPath;
  FILE *pf;

    //CString cstrMsg;
    //cstrMsg.Format("bDBFileExists, cstrDB: %s", cstrDB);
    //AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, "DEBUG", MB_OK);


  if(cstrDB.Find(DB_FILE_EXT) < 0)
    { // This is the default database, add the path and extension.
    cstrPath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_DATAPATH, "");


    //cstrMsg.Format("default database, cstrDB: %s, Path= %s", cstrDB, cstrPath);
    //AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, "DEBUG", MB_OK);


//// REMOTE DB
    //if(m_cstrAltDBPath == "")
    //  cstrPath = g_pConfig->cstrGet_item(CFG_DATAPATH);
//// REMOTE DB
    //else
    //  cstrPath = m_cstrAltDBPath;
    cstrFile.Format("%s\\%s.%s", cstrPath, cstrDB, DB_FILE_EXT);
    }
  else
    cstrFile = cstrDB;
  if((pf = fopen(cstrFile, "r")) != NULL)
    { // Database exists.
    //AfxGetApp()->m_pActiveWnd->MessageBox("DB exists", "DEBUG", MB_OK);

    bRet = true;
    fclose(pf);
    }
  else
    { // Report the error.
    CString cstrErr, cstrMsg;

    bRet = false;
    //cstrMsg.Format(IDS_DB_NOT_FOUND, cstrFile);
    cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_DB_NOT_FOUND), cstrFile);
    //cstrErr.LoadString(IDS_ERROR1);
    cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    (CEGGSASApp *)AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErr, MB_OK);
    }
  return(bRet);
  }

/********************************************************************
iGet_version - Get the database version from the version table.

  inputs: Reference to a CString object that gets the database version.

  return: SUCCESS if the version is read from the table
          FAIL if there is an error.
********************************************************************/
short int CDbaccess::iGet_version(CString &cstrVersion)
  {
  CString cstrSQL;
  short int iRet;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = FAIL;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      cstrSQL = SQL_GET_VERSION;
      m_sqlrSts = SQLExecDirect(m_sqlhStmt,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        { // Now get all the values.
        m_sqlrSts = SQLFetch(m_sqlhStmt);
        if(SQL_SUCCEEDED(m_sqlrSts))
          {
          iRet = SUCCESS;
          iGet_string(m_sqlhStmt, 1, cstrVersion);
          }
        else if(m_sqlrSts != SQL_NO_DATA)
          vReport_err("iGet_version", m_sqlhStmt);
        }
      else
        vReport_err("iGet_version", m_sqlhStmt); // SQLExecDirect() failed.
      sqlrFree_stmt_handle(m_sqlhStmt);
      }
    else
      vReport_err("iGet_version", m_sqlhStmt); // sqlhGet_stmt_handle() failed.
    }
  return(iRet);
  }

/********************************************************************
iValidate_version - Validate the version in the database table against
                    the version this program is expecting, DB_VERSION.

  inputs: None.

  return: SUCCESS if this is the correct version.
          FAIL if this is not the correct version.
********************************************************************/
short int CDbaccess::iValidate_version()
  {
  short iRet = FAIL;
  CString cstrVersion;

  if(iGet_version(cstrVersion) == SUCCESS)
    {
    if(cstrVersion == DB_VERSION)
      iRet = SUCCESS;
    }
  return(iRet);
  }

/********************************************************************
cstrFormat - Reformat a string being put in the database.
               - Add an apostrophe be each existing apostrophe.

  inputs: String being added to the database.

  return: Reformatted string.
********************************************************************/
CString CDbaccess::cstrFormat(CString cstrItem)
  {
  CString cstrRet;
  int iIndex;

  iIndex = 0;
  cstrRet = cstrItem;
  while((iIndex = cstrRet.Find("'", iIndex)) >= 0)
    {
    cstrRet.Insert(iIndex, "'");
    iIndex += 2;
    }
  return (cstrRet);
  }

/********************************************************************
iEncode_study_dates - Encode all study dates in the database into a GUID string
                      and update the database study date.
                      Decode all the GUID study dates in the database to
                      a date string and update the database.

  inputs: true to encode the study dates, false to decode th study dates.

  return: SUCCESS if all dates are correctly processed.
          FAIL if there is an error.
********************************************************************/
int CDbaccess::iEncode_study_dates(bool bEncode)
  {
  CString cstrDate, cstrSQL;
  short int iRet;
  long lStudyID;
  bool bChanged;

  if((iRet = iDBConnect()) == SUCCESS)
    {
    iRet = SUCCESS;
    if((m_sqlhStmt = sqlhGet_stmt_handle()) != NULL)
      {
      m_sqlrSts = SQLExecDirect(m_sqlhStmt, (SQLCHAR *)"Select StudyID,DateOfStudy from StudyData", SQL_NTS);
      if(SQL_SUCCEEDED(m_sqlrSts))
        {
        while(true)
          {
          m_sqlrSts = SQLFetch(m_sqlhStmt);
          if(!SQL_SUCCEEDED(m_sqlrSts) || m_sqlrSts == SQL_NO_DATA)
            {
            if(m_sqlrSts != SQL_NO_DATA)
              iRet = FAIL;
            break;
            }
          else
            {
            bChanged = false;
            SQLGetData(m_sqlhStmt, 1, SQL_C_LONG, &lStudyID, 0, 0);
            iGet_string(m_sqlhStmt, 2, cstrDate);
            if(bEncode == true)
              {// Encode the date.
              if(cstrDate.GetLength() <= 10)
                {
                cstrDate = cstrEncode_study_date(cstrDate);
                bChanged = true;
                }
              }
            else
              { // Decode the date.
              if(cstrDate.GetLength() > 10)
                {
                cstrDate = cstrDecode_study_date(cstrDate);
                bChanged = true;
                }
              }
            if(bChanged == true)
              {
              // Update the record with the encoded date.
              if((m_sqlhStmt1 = sqlhGet_stmt_handle()) != NULL)
                {
                cstrSQL.Format("UPDATE StudyData set DateOfStudy='%s' where StudyID=%lu", cstrDate, lStudyID);
                m_sqlrSts = SQLExecDirect(m_sqlhStmt1,(SQLCHAR *)(LPCTSTR)cstrSQL, SQL_NTS);
                if(!SQL_SUCCEEDED(m_sqlrSts))
                  {
                  vReport_err("iEncode_study_dates", m_sqlhStmt1);
                  iRet = FAIL;
                  }
                sqlrFree_stmt_handle(m_sqlhStmt1);
                }
              }
            }
          }
        }
      }
    }
  return (iRet);
  }






