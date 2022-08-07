/********************************************************************
Dbaccess.h

Copyright (C) 2003,2004,2005,2007,2008,2009, 3CPM Company, Inc.  All rights reserved.

  Header file for the CDbaccess class.

HISTORY:
08-NOV-03  RET  New.
10-FEB-05  RET  Change FIND_PATIENT_INFO structure to add a middle initial.
02-JUL-07  RET  Research Version
                  Add FIND_NEXT_STUDY structure.
30-JUN-08  RET  Version 2.05
                  Add study ID to FIND_NEXT_STUDY structure.
28-AUG-08  RET  Version 2.05c
                  Add research reference number to FIND_PATIENT_INFO structure.
//// REMOTE DB
                 Implement using databases from other places.
                   Add class variable: m_cstrAltDBPath.
//// END OF REMOTE DB
07-N0V-08  RET  Version 2.08a
                  Modifications for the Reader program version.
                    Add define EXP_DSN_DEFAULT, DB_CONN_NONE, DB_CONN_LOCAL,
                      DB_CONN_REMOTE
                    Add class variables: m_iConnType, m_sqlhStmtPatient
                    Add structures: PATIENT_INFO, STUDY_DATA_RECORD, WHAT_IF_RECORD
12-DEC-08  RET  Version R.2.08b
                  Add define DB_FILE_EXT.
13-FEB-09  RET  Version 2.08i
                  Add method prototypes: iGet_version(), iValidate_version().
********************************************************************/

#if !defined(AFX_DBACCESS_H__FE294E81_6E1A_4F67_B73E_992EB6AF9BEE__INCLUDED_)
#define AFX_DBACCESS_H__FE294E81_6E1A_4F67_B73E_992EB6AF9BEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SQL.h"
#include "SQLExt.h"
#include <odbcss.h>

// Current database version
#define DB_VERSION "2.03"

// Default database DSN
#define DSN_DEFAULT "3CPM EGGSAS"
// Exported database name.
#define EXP_DSN_DEFAULT "3CPM EGGSAS EXP"

#define DB_FILE_EXT "mdb"

// Database connection types
#define DB_CONN_NONE   0
#define DB_CONN_LOCAL  1
#define DB_CONN_REMOTE 2

// Defines for what record to get from the database.
#define FIRST_RECORD 1
#define NEXT_RECORD  2

  // Data returned by find patient routines
typedef struct
  {
  long lPatientID;
  CString cstrLastName;
  CString cstrFirstName;
  CString cstrMI; // Middle initial
  BOOL bUseResrchRefNum;
	CString	cstrResearchRefNum;
  } FIND_PATIENT_INFO;

typedef struct
  {
  long lFacilityID;
	CString	cstrName;
	CString	cstrAddr1;
	CString	cstrAddr2;
	CString	cstrCity;
	CString	cstrState;
	CString	cstrZipcode;
	CString	cstrPhone;
  } FACILITY_INFO;

typedef struct
  {
  long lPatientID;
  CString	cstrAddress1;
  CString	cstrAddress2;
  CString	cstrCity;
  CString	cstrState;
  CString	cstrZipCode;
  CString	cstrPhone;
  CString	cstrDateOfBirth;
  CString	cstrHospitalNum;
  CString	cstrSocialSecurityNum;
  } PATIENT_INFO;

typedef struct
  {
  CString cstrDate; // Date of the study
  BOOL bPause; // true indicate the study has been paused
  unsigned uDataPtsInBaseline; // Number of data points in the Baseline.
  long lPatientID; // The patient ID to get the study for.
  long lStudyID; // Study ID for this study.
  } FIND_NEXT_STUDY;

typedef struct
  {
  long lStudyID;
  long lPatientID;
  CString cstrDataFile;
  CString cstrDateOfStudy;
  CString cstrMealTime;
  unsigned short uWaterAmount;
  CString cstrWaterAmountUnits;
  CString cstrPeriodStart;
  CString cstrPeriodLen;
  CString cstrStartMinutes;
  CString cstrEndMinutes;
  long lTestMaxSignal;
  long lTestMinSignal;
  BOOL bExtra10Min;
  CString cstrSuggestedDiagnosis;
  BOOL bBloating;
  BOOL bEarlySateity;
  BOOL bEructus;
  BOOL bGerd;
  BOOL bNausea;
  BOOL bPostPrandialFullness;
  BOOL bVomiting;
  CString cstrAttendingPhysician;
  CString cstrReferingPhysician;
  CString cstrTechnician;
  BOOL bManualMode;
  CString cstrRespirationRate;
  BOOL bUserDiagnosis;
  CString cstrIFPComments;
  CString cstrDiagnosis;
  CString cstrRecommendations;
  CString cstrMedications;
  BOOL bDelayedGastricEmptying;
  unsigned int uiBLDataPoints;
  BOOL bPause;
  unsigned int uiPostStimDataPoints;
  CString cstrStimulationMedium;
  } STUDY_DATA_RECORD;

// Structure for holding the data for a What If scenario.
typedef struct
  {
  long lWhatIfID;
  long lStudyID;
  CString cstrDescription;
  CString cstrDateTimeSaved;
  short unsigned uNumPeriods; // Number of periods (minus baseline) user has defined.
  CString cstrPeriodStart;
  CString cstrPeriodLen;
  CString cstrStartMinutes;
  CString cstrEndMinutes;
  CString cstrRespirationRate; // Holds the respiration rate for each period.
  } WHAT_IF_RECORD;

// Data contained in the Events table.
typedef struct
  {
  long lEventID;
  long lStudyID;
  long lDataPoint;
  long lSeconds;
  CString cstrType; // Indications
  short int iPeriodType;
  CString cstrDescription;
  } EVENT_RECORD;

class CDbaccess  
{
public:
	CDbaccess(LPCTSTR strDSN);
	virtual ~CDbaccess();

public: // Data
  CString m_cstrDSN; // DSN for the database.
  bool m_bDBConnected; // true - database is connected.
  int m_iConnType; // Database connection type.

    // Database data.
  SQLRETURN m_sqlrSts; // Returns status from ODBC functions.
  SQLHENV m_sqlhEnv; // Database environment handle.
  SQLHDBC m_sqlhDbc; // Database connection handle.
  SQLSMALLINT m_sqliHandleType; // Handle type used by error handler.
  SQLHSTMT m_sqlhStmt; // Database statement handle.
  SQLHSTMT m_sqlhStmt1; // Database statement handle.
  SQLHSTMT m_sqlhStmt2; // Database statement handle.
  SQLHSTMT m_sqlhStmtPatient; // Database statement handle for the Patient table.
//unsigned m_uQueryTimeout = 5; // Query time out is 5 seconds.

  SQLCHAR m_sqlOutConnStr[1024];


//// REMOTE DB
/********************************************
  CString m_cstrAltDBPath; // Contains the path for an alternate database.
  CString m_cstrOrigEGGPath; // Contains the original path and file name for
                             // the EGG data file is an alternate database is
                             // being used.
  CString m_cstrLocalDBPath; // Contains the path for the local database, the
                             // database created during installation.
*************************************************/

public: // Methods
  void vSet_DSN(CString cstrDSN);
  CString cstrGet_db_file_path();
    // SQL methods
  SQLRETURN sqlrDB_connect(void);
  SQLRETURN sqlrDB_disconnect(void);
  void vReport_err(char *pszMethod, SQLHANDLE sqlhHandle);
  SQLHANDLE sqlhGet_stmt_handle(void);
  SQLRETURN sqlrFree_stmt_handle(SQLHANDLE sqlhStmt);
  CString cstrGet_db_path(void);
  CString cstrGet_conn_path();
  bool bDBFileExists(CString cstrDB);
//// REMOTE DB
/********************************************
  void vSet_alt_db_path(CString cstrPath);
***********************************/
  short int iGet_string(SQLHSTMT sqlhStmt, short int iColumn, CString &cstrData);
  short int iGet_long_string(SQLHSTMT sqlhStmt, short int iColumn, CString &cstrData);
  short int iGet_bool(SQLHSTMT sqlhStmt, short int iColumn, BOOL *pbValue);
  short int iGet_bit(SQLHSTMT sqlhStmt, short int iColumn, BOOL *pbValue);

    // Version table
  short int iGet_version(CString &cstrVersion);
  short int iValidate_version();

    // Patient table mathods
  short int iSave_patient(PATIENT_DATA *pdInfo);
  short int iGet_patient(PATIENT_DATA *pdInfo);
  short int iFind_patient(PATIENT_DATA *pdInfo);
  short int iFind_next_patient(short int iPos, FIND_PATIENT_INFO *pfpiData);
  short int iFind_patient_name(FIND_PATIENT_INFO *pfpiData);

    // PatientInfo table mathods
  short int iSave_patient_info(PATIENT_DATA *pdInfo);
  short int iSave_patient_info(PATIENT_INFO *ppiInfo);
  short int iGet_patient_info(PATIENT_DATA *pdInfo);
  short int iGet_patient_info(PATIENT_INFO *ppiInfo);
  short int iFind_patient_info(long lPatientID);

    // StudyData table mathods
  short int iGet_study_data(PATIENT_DATA *pdInfo);
  short int iGet_study_data(STUDY_DATA_RECORD *psd);
  short int iGet_study_data(CString cstrDataFile, STUDY_DATA_RECORD *psd);
  short int iFind_study(CString cstrDataFile);
  short int iSave_study_data(PATIENT_DATA *pdInfo);
  short int iSave_study_data(STUDY_DATA_RECORD *pStudy);
  long lFind_study_data(long lPatientID);
  long lFind_study_data(long lPatientID, CString cstrDate);
  long lFind_study(CString cstrDataFile);
  short int iFind_next_study(short int iPos, FIND_NEXT_STUDY *pfns);
//                             CString &cstrDate, BOOL *pbPause, long lPatientID);
  short int iFind_next_diagnosis(short int iPos, CString &cstrDate, CString &cstrDiagnosis,
                                 long *plStudyID, long lPatientID);
  short int iFind_next_medication(short int iPos, CString &cstrDate, CString &cstrMedication,
                                  long *plStudyID, long lPatientID);
  short int iUpdate_medication(long lStudyID, CString cstrMedication);
  short int iGet_num_study_records(long *plNumRecords);
  short int iGet_next_data_file(short int iPos, CString &cstrFile);
  short int iGet_study_count(int *piCount);

    // Facility table methods
  short int iGet_facility_info(FACILITY_INFO *pfi);
  short int iSave_facility_info(FACILITY_INFO *pfi);

    // Events table methods
#ifdef EGG_EVENTS
  short int iSave_event(long lStudyID, CEvnt *pEvent);
  short int iSave_event(EVENT_RECORD *pEvent);
  short int iGet_event(long lStudyID, CEvnt *pEvent);
  short int iGet_event(EVENT_RECORD *pEvent);
  short int iDelete_event(long lStudyID, int iDataPoint, int iPeriodType);
  short int iDelete_events(long lStudyID);
  short int iFind_event(long lStudyID, int iDataPoint, int iPeriodType);
#endif

#ifdef EGG_WHAT_IF
    // WhatIf table.
  short int iFind_next_whatif(short int iPos, WHAT_IF *pwi);
  short int iFind_next_whatif(short int iPos, WHAT_IF_RECORD *pwi);
  short int iSave_whatif_data(WHAT_IF *pwi);
  short int iSave_whatif_data(WHAT_IF_RECORD *pwi);
  short int iFind_whatif(long lWhatIfID);
  short int iGet_whatif_data(WHAT_IF *pwi);
  short int iDelete_whatif(long lWhatIfID);
  short int iDelete_whatif_for_study(long lStudyID);
#endif

  CString cstrGet_db_name(void);
  short iDBConnect(void);
  void vFree_env_handle(void);
  void vFree_db_conn_handle(void);
  CString cstrFormat(CString cstrItem);
};

#endif // !defined(AFX_DBACCESS_H__FE294E81_6E1A_4F67_B73E_992EB6AF9BEE__INCLUDED_)
