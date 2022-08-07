/********************************************************************
DBAccess.h

Copyright (C) 2009,2016 3CPM Company, Inc.  All rights reserved.

  Header file for the CDBAccess class.

HISTORY:
03-NOV-08  RET  New.
Version 1.03
  05-FEB-09  RET  Add data file name to structure FIND_NEXT_STUDY.
09-JUN-16 RET Versin 1.06
            Update database access methods for the most current database version, 2.06.
              Add members to structures:
                PATIENT_TABLE: bHide
                STUDY_DATA: cstrDiagnosisKeyPhrase, cstrICDCode, cstrRecommendationKeyPhrase,
                  uFrequency, bGastroparesis, bDyspepsiaBloatSatiety, bDyspepsia,
                  bAnorexia, bRefractoryGERD, bWeightLoss, bPostDilationEval, bEvalEffectMed
                  WHAT_IF: uFrequency
                  PATIENT_INFO: bMale, bFemale
20-JAN-18  RET
             Provide search criteria for listing patients.
               Add method prototypes: bLook_for_study_date(), ctMake(), vParse_date()
               Add variables: m_cstrSearchSQL, m_iDateSrchType, m_ctSrchDate,
                 m_cstrSrchDate
               Add defines: DB_SRCH_NONE, DB_SRCH_GREATER_THAN, DB_SRCH_LESS_THAN,
                 DB_SRCH_EQUAL_TO
19-MAY-18  RET Version 1.09
             Add database search button to main screen.  This allows the user
						 to select studies to export based on names/research reference numbers
						 and/or dates.  Both allows use of wild cards.
						   Add defines: DB_SRCH_NONE, DB_SRCH_GREATER_THAN, DB_SRCH_LESS_THAN,
							   DB_SRCH_EQUAL_TO
						   Add class variables: m_cstrSearchSQL, m_iDateSrchType, m_ctSrchDate,
							   m_cstrSrchDate
********************************************************************/

#pragma once

#include "SQL.h"
#include "SQLExt.h"
#include <odbcss.h>

// Local database DSN
#define LOCAL_DSN_DEFAULT _T("3CPM EGGSAS")
// Exported database name.
#define EXP_DSN_DEFAULT _T("3CPM EGGSAS EXP")
// Name of an empty database.
#define EMPTY_DB _T("3CPM EGGSAS EMPTY")
// Database extension.
#define DB_EXTENSION _T(".mdb")

#define SQL_ERR_MSG_LEN 512 // Max length of SQL error message.

// Defines for what record to get from the database.
#define FIRST_RECORD 1
#define NEXT_RECORD  2

#define DB_SRCH_NONE 0
#define DB_SRCH_GREATER_THAN 1
#define DB_SRCH_LESS_THAN 2
#define DB_SRCH_EQUAL_TO 3

  // Maximum periods.
  // Post-stimulation max hours * 60 = max minutes.
  // max minutes / 4 minutes per period plus 1 for the baseline.
#define MAX_TOTAL_PERIODS (((POST_STIMULATION_MAX_TIME_HRS * 60) / 4) + 1)

  // Data returned by find patient routines
typedef struct
  {
  long lPatientID;
  CString cstrLastName;
  CString cstrFirstName;
  CString cstrMI; // Middle initial
  BOOL bUseResrchRefNum;
	CString	cstrResearchRefNum;
  BOOL bHide; // true if patient is hidden.
  } PATIENT_TABLE;

  // Data structure returned when finding the next study.
typedef struct
  {
  CString cstrDate; // Date of the study
  BOOL bPause; // true indicate the study has been paused
  unsigned uDataPtsInBaseline; // Number of data points in the Baseline.
  long lPatientID; // The patient ID to get the study for.
  long lStudyID; // Study ID for this study.
  CString cstrFileName; // Name of data file.
  } FIND_NEXT_STUDY;

// Data structure that holds all data from a StudyData record.
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
     // Key phrase for the diagnosis description, link to DiagnosisDesc table
  CString cstrDiagnosisKeyPhrase;
  CString cstrICDCode; // ICD Code
     // Key phrase for the recommendation description, link to Recommendations table
  CString cstrRecommendationKeyPhrase;
  unsigned uFrequency; // Frequency study was recorded at.
	BOOL bGastroparesis; // gastroparesis - 536.3  substitutes for "delayed emptying"
	BOOL bDyspepsiaBloatSatiety; // dyspepsia - bloating and/or satiety - 536.80 substitutes for bloating and satiety
	BOOL bDyspepsia; // dyspepsia - 536.80
	BOOL bAnorexia; // anorexia - 783.0
	BOOL bRefractoryGERD; // Refractory GERD – 530.11
	BOOL bWeightLoss; // Weight loss
	BOOL bPostDilationEval; // Post dilation evaluation
  BOOL bEvalEffectMed; // Evaluate effect of medication
} STUDY_DATA;

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
  unsigned uFrequency; // Frequency for viewing the study.
  } WHAT_IF;

// Data contained in the PatientInfo table.
typedef struct
{
  CString cstrAddress1;
  CString cstrAddress2;
  CString cstrCity;
  CString cstrState;
  CString cstrZipCode;
  CString cstrPhone;
  CString cstrDateOfBirth;
  CString cstrHospitalNumber;
  CString cstrSocialSecurityNum;
  BOOL bMale; // true if patient is a male.
  BOOL bFemale; // true if patient is a female.
} PATIENT_INFO;

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
} EVENTS_DATA;

class CDBAccess
{
public:
  CDBAccess(const wchar_t *pszDSN);
  virtual ~CDBAccess(void);

public: // Data
  CString m_cstrDSN; // DSN for the database.
  bool m_bDBConnected; // true - database is connected.


  SQLRETURN m_sqlrSts; // Returns status from ODBC functions.
  SQLHENV m_sqlhEnv; // Database environment handle.
  SQLHDBC m_sqlhDbc; // Database connection handle.
  SQLSMALLINT m_sqliHandleType; // Handle type used by error handler.
  SQLHSTMT m_sqlhStmt; // Database statement handle.
  SQLHSTMT m_sqlhStmt1; // Database statement handle.
  SQLHSTMT m_sqlhStmt2; // Database statement handle.
  SQLHSTMT m_sqlhStmt3; // Database statement handle.
  SQLHSTMT m_sqlhStmt4; // Database statement handle.
  SQLWCHAR m_sqlOutConnStr[1024];

public: // Methods
  short iDBConnect(void);
    // SQL methods
  SQLRETURN sqlrDB_connect(void);
  SQLRETURN sqlrDB_disconnect(void);
  void vReport_err(SQLHANDLE sqlhHandle);
  void vFree_env_handle(void);
  void vFree_db_conn_handle(void);
  SQLHANDLE sqlhGet_stmt_handle(void);
  SQLRETURN sqlrFree_stmt_handle(SQLHANDLE sqlhStmt);

  short int iFind_patient(PATIENT_TABLE *pPatient);
  short int iGet_next_patient(short int iPos, PATIENT_TABLE *pPatientTable);
  short int iGet_patient(PATIENT_TABLE *pPatient);
  short int iSave_patient(PATIENT_TABLE *pPatient);

  short int iFind_next_study(short int iPos, FIND_NEXT_STUDY *pfns);
  short int iGet_patient_id_from_study(long lStudyID, long *plPatientID);
  short int iGet_study_data(STUDY_DATA *pStudy);
  short int iSave_study_data(STUDY_DATA *pStudy);

  short int iGet_patient_info(long lPatientID, PATIENT_INFO *pPInfo);
  short int iSave_patient_info(long lPatientID, PATIENT_INFO *pPInfo);
  short int iFind_patient_info(long lPatientID);

  short int iFind_next_whatif(short int iPos, WHAT_IF *pwi);
  short int iSave_whatif_data(WHAT_IF *pwi);
  short int iGet_whatif_data(WHAT_IF *pwi);

  short int iGet_event(EVENTS_DATA *pEvent);
  short int iSave_event(EVENTS_DATA *pEvent);

  short int iGet_string(SQLHSTMT sqlhStmt, short int iColumn, CString &cstrData);
  short int iGet_bool(SQLHSTMT sqlhStmt, short int iColumn, BOOL *pbValue);
  short int iGet_long_string(SQLHSTMT sqlhStmt, short int iColumn, CString &cstrData);
  short int iGet_bit(SQLHSTMT sqlhStmt, short int iColumn, BOOL *pbValue);
  CString cstrFormat(CString cstrItem);

  // Added for use by database for search criteria when listing patients.
  CString m_cstrSearchSQL;
  int m_iDateSrchType;
  CTime m_ctSrchDate;
  CString m_cstrSrchDate;
	bool bLook_for_study_date(CString cstrDate);
	CTime ctMake(CString cstrDate);
	void vParse_date(CString cstrDate, CString cstraDate[], int iLen);
	CString cstrGet_db_path(void);
  int iGet_patient_count(int *piCount);
	CString cstrGet_db_file_path();

};
