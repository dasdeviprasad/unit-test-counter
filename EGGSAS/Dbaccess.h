/********************************************************************
Dbaccess.h

Copyright (C) 2003,2004,2005,2007,2008,2009,2012,2013,2016,2018 3CPM Company, Inc.  All rights reserved.

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
27-AUG-12  RET
             Implement new database table DiagnosisDesc
               Modify structures: STUDY_DATA_RECORD
               Add structure: DIAGNOSIS_DESC_RECORD
               Add prototypes for methods to access the DiagnosisDesc table
31-AUG-12  RET
             Implement new database table for ICDCodes.
               Modify structures: STUDY_DATA_RECORD
               Add structure: ICD_CODE_RECORD
               Add prototypes for methods to access the ICD Code table
02-SEP-12  RET
             Implement new database table for Recommendations.
               Modify structures: STUDY_DATA_RECORD
               Add structure: RECOMMENDATIONS_RECORD
               Add prototypes for methods to access the Recommendations table
13-SEP-12  RET
             Add new indications to the StudyData table.
               Add to structure STUDY_DATA_RECORD:
                 bGastroparesis, bDyspepsiaBloatSatiety, bDyspepsia,
                 bAnorexia, bRefractoryGERD, bWeightLoss, bPostDilationEval,
                 bEvalEffectMed
25-FEB-13  RET
               Change database version to 2.05
               Add to structure PATIENT_INFO:
                 bMale, bFemale, bHide
               Add to structure FIND_PATIENT_INFO:
                 bHide, bAll
13-MAR-13  RET
             Change database version to 2.06
             Add Frequency field to the Study table and the WhatIF table.
             Add to structures: STUDY_DATA_RECORD, WHAT_IF_RECORD: uFrequency
02-JUN-16  RET
             Add feature to hide study dates.
               Add prototype for method: iEncode_study_dates()
               Add defines: STUDY_DATE_DAY_1, STUDY_DATE_DAY_2, STUDY_DATE_MONTH,
                 STUDY_DATE_YEAR_1, STUDY_DATE_YEAR_2, STUDY_DATE_YEAR_3,
                 STUDY_DATE_YEAR_4
20-JAN-18  RET
             Provide search criteria for listing patients.
               Add method prototypes: bLook_for_study_date(), ctMake(), vParse_date()
               Add variables: m_cstrSearchSQL, m_iDateSrchType, m_ctSrchDate,
                 m_cstrSrchDate
               Add defines: DB_SRCH_NONE, DB_SRCH_GREATER_THAN, DB_SRCH_LESS_THAN,
                 DB_SRCH_EQUAL_TO
********************************************************************/

#if !defined(AFX_DBACCESS_H__FE294E81_6E1A_4F67_B73E_992EB6AF9BEE__INCLUDED_)
#define AFX_DBACCESS_H__FE294E81_6E1A_4F67_B73E_992EB6AF9BEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SQL.h"
#include "SQLExt.h"
#include <odbcss.h>
#include "atltime.h"

// Current database version
#define DB_VERSION "2.06" //"2.03"

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

//// Indexes used to encode study date in the study date field.
#define STUDY_DATE_DAY_1 14
#define STUDY_DATE_DAY_2 3
#define STUDY_DATE_MONTH 7
//#define STUDY_DATE_MONTH_2 5
#define STUDY_DATE_YEAR_1 10
#define STUDY_DATE_YEAR_2 17
#define STUDY_DATE_YEAR_3 19
#define STUDY_DATE_YEAR_4 11
//// end of Indexes used to encode study date in the study date field.

#define DB_SRCH_NONE 0
#define DB_SRCH_GREATER_THAN 1
#define DB_SRCH_LESS_THAN 2
#define DB_SRCH_EQUAL_TO 3

  // Data returned by find patient routines
typedef struct
  {
  long lPatientID;
  CString cstrLastName;
  CString cstrFirstName;
  CString cstrMI; // Middle initial
  BOOL bUseResrchRefNum;
	CString	cstrResearchRefNum;
  BOOL bHide; // true to get hidden patients, false to get active patients.
  BOOL bAll; // true to get all patients, false get patients according to bHide.
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
  BOOL bMale;
  BOOL bFemale;
  BOOL bHide;
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
  CString cstrDiagnosisKeyPhrase;
  CString cstrICDCode;
     // Key phrase for the recommendation description, link to Recommendations table
  CString cstrRecommendationKeyPhrase;
	BOOL bGastroparesis; // gastroparesis - 536.3  substitutes for "delayed emptying"
	BOOL bDyspepsiaBloatSatiety; // dyspepsia - bloating and/or satiety - 536.80 substitutes for bloating and satiety
	BOOL bDyspepsia; // dyspepsia - 536.80
	BOOL bAnorexia; // anorexia - 783.0
	BOOL bRefractoryGERD; // Refractory GERD – 530.11
	BOOL bWeightLoss; // Weight loss
	BOOL bPostDilationEval; // Post dilation evaluation
  BOOL bEvalEffectMed; // Evaluate effect of medication
  unsigned uFrequency; // Frequency study was recorded at.
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
  unsigned uFrequency; // Frequency study was recorded at.
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

// Data contained in the DiagnosisDesc table.
typedef struct
  {
  CString cstrDiagnosisKeyPhrase;
  CString cstrDiagDesc;
  } DIAGNOSIS_DESC_RECORD;
  
// Data contained in the ICDCode table.
typedef struct
  {
  CString cstrICDCode;
  } ICD_CODE_RECORD;

// Data contained in the Recommendations table.
typedef struct
  {
  CString cstrKeyPhrase;
  CString cstrDescription;
  } RECOMMENDATIONS_RECORD;
  
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
/********************************************/
  //CString m_cstrAltDBPath; // Contains the path for an alternate database.
  //CString m_cstrOrigEGGPath; // Contains the original path and file name for
  //                           // the EGG data file is an alternate database is
  //                           // being used.
  //CString m_cstrLocalDBPath; // Contains the path for the local database, the
  //                           // database created during installation.
/*************************************************/

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
/********************************************/
  //void vSet_alt_db_path(CString cstrPath);
/***********************************/
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
  short int iSave_event(long lStudyID, CEvnt *pEvent);
  short int iSave_event(EVENT_RECORD *pEvent);
  short int iGet_event(long lStudyID, CEvnt *pEvent);
  short int iGet_event(EVENT_RECORD *pEvent);
  short int iDelete_event(long lStudyID, int iDataPoint, int iPeriodType);
  short int iDelete_events(long lStudyID);
  short int iFind_event(long lStudyID, int iDataPoint, int iPeriodType);

    // WhatIf table.
  short int iFind_next_whatif(short int iPos, WHAT_IF *pwi);
  short int iFind_next_whatif(short int iPos, WHAT_IF_RECORD *pwi);
  short int iSave_whatif_data(WHAT_IF *pwi);
  short int iSave_whatif_data(WHAT_IF_RECORD *pwi);
  short int iFind_whatif(long lWhatIfID);
  short int iGet_whatif_data(WHAT_IF *pwi);
  short int iDelete_whatif(long lWhatIfID);
  short int iDelete_whatif_for_study(long lStudyID);
  
  // DiagnosisDesc table.
  short int iSave_diagnosis_desc(DIAGNOSIS_DESC_RECORD *pddr);
  short int iGet_diagnosis_desc(short int iPos, DIAGNOSIS_DESC_RECORD *pddr);
  short int iFind_diagnosis_desc(DIAGNOSIS_DESC_RECORD *pddr);
  short int iDelete_diagnosis_desc(CString cstrKeyPhrase);
  short int iUpdate_diagnosis_desc(CString cstrKeyPhrase, DIAGNOSIS_DESC_RECORD *pddr);

  // ICDCode table
  short int iSave_ICDCode(ICD_CODE_RECORD *pICD);
  short int iGet_ICDCode(short int iPos, ICD_CODE_RECORD *pICD);
  short int iFind_ICDCode(ICD_CODE_RECORD *pICD);
  short int iDelete_ICDCode(CString cstrID);
  short int iUpdate_ICDCode(ICD_CODE_RECORD *pICDExisting, ICD_CODE_RECORD *pICDNew);

  // Recommendations table
  short int iSave_recomm_desc(RECOMMENDATIONS_RECORD *prr);
  short int iUpdate_recommendation(CString cstrKeyPhrase, RECOMMENDATIONS_RECORD *prr);
  short int iGet_recomm_desc(short int iPos, RECOMMENDATIONS_RECORD *prr);
  short int iFind_recommendation(RECOMMENDATIONS_RECORD *prr);
  short int iDelete_recommendation(CString cstrKeyPhrase);


  CString cstrGet_db_name(void);
  short iDBConnect(void);
  void vFree_env_handle(void);
  void vFree_db_conn_handle(void);
  CString cstrFormat(CString cstrItem);
  int iEncode_study_dates(bool bEncode);
  // Added for use by database for search criteria when listing patients.
  CString m_cstrSearchSQL;
  int m_iDateSrchType;
  CTime m_ctSrchDate;
  CString m_cstrSrchDate;
  bool bLook_for_study_date(CString cstrDate);
  CTime ctMake(CString cstrDate);
  void vParse_date(CString cstrDate, CString cstraDate[], int iLen);
};

#endif // !defined(AFX_DBACCESS_H__FE294E81_6E1A_4F67_B73E_992EB6AF9BEE__INCLUDED_)
