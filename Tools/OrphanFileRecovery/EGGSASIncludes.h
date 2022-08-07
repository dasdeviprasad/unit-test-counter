/********************************************************************
EGGSASIncludes.h

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Contains definitions, structures for EGGSAS.


HISTORY:
25-JUN-10  RET  New.
********************************************************************/

#include "English.h"

// Define to include events.
//#define EGG_EVENTS

// Define to include WhatIf
//#define EGG_WHAT_IF

  // The following enumerated list indicates the type of program and/or study.
enum ePgmType
  {
  PT_NONE,       // No program/study type.
  PT_STANDARD,   // Standard program/study type.
  PT_RESEARCH,   // Research program/study type.
  PT_STD_RSCH,   // Both Standard and Research program types.
  };

#define INV_MINUTE        -1.0F // Represents an invalid designation of a minute.
#define INV_PERIOD_LEN    -1.0F // Invalid period length.
#define INV_PERIOD_START  -1.0F // Invalid period start.

//// Defines for items in the INI file.
#define WATER_LOAD_UNITS_OUNCES 0 // ounces
#define WATER_LOAD_UNITS_MILLILITERS 1 // milliliters

#define DEFAULT_MIN_PER_GRAPH 4 // Default number of minutes per graph.
//// End of INI file definitions

// Patient types
#define PAT_NONE  0
#define PAT_DOG   1
#define PAT_HUMAN 2

#define PATIENT_TYPE_DOG "_DOG_"

/////////////////////////////////////////////////////////////////////
//// Defines for the data collected during an EGG.
#define MAX_CHANNELS 2
#define MIN_PER_PERIOD 10 // Minutes per period.
#define DATA_POINT_SIZE sizeof(float) //sizeof(short int)
#define DATA_POINTS_PER_SECOND 4.267F
  // Data points per minute for one channel.
#define DATA_POINTS_PER_MIN_1 256 //260 //
#define DATA_POINTS_PER_HALF_MIN (DATA_POINTS_PER_MIN_1 / 2)
  // Data points per minute for all channels.
#define DATA_POINTS_PER_MIN_ALL (DATA_POINTS_PER_MIN_1 * MAX_CHANNELS)
#define MINUTES_PER_EPOCH 4
#define HALF_MINUTES_PER_EPOCH (MINUTES_PER_EPOCH * 2)
  // Bytes of data per minute for one channel.
#define BYTES_PER_MINUTE_1 (DATA_POINTS_PER_MIN_1 * DATA_POINT_SIZE)
  // Bytes of data per minute for all channels.
#define BYTES_PER_MINUTE_ALL (DATA_POINTS_PER_MIN_ALL * DATA_POINT_SIZE)
  // Bytes per epoch for one channel.
#define BYTES_PER_EPOCH_1 (BYTES_PER_MINUTE_1 * MINUTES_PER_EPOCH)
  // Bytes per epoch for all channels.
#define BYTES_PER_EPOCH_ALL (BYTES_PER_MINUTE_ALL * MINUTES_PER_EPOCH)
  // Data points per epoch for one channel.
#define DATA_POINTS_PER_EPOCH_1 (DATA_POINTS_PER_MIN_1 * MINUTES_PER_EPOCH)
  // Data points per epoch for all channels.
#define DATA_POINTS_PER_EPOCH_ALL (DATA_POINTS_PER_MIN_ALL * MINUTES_PER_EPOCH)
  // Number of bytes in a period
#define BYTES_PER_PERIOD (BYTES_PER_MINUTE_1 * MIN_PER_PERIOD)

  // Research version
  // Post-stimulation time period in hours
#define POST_STIMULATION_MAX_TIME_HRS 3
  // Post-stimulation time period in minutes
#define POST_STIMULATION_MAX_TIME_MIN (POST_STIMULATION_MAX_TIME_HRS * 60)
  // Post-stimulation time period in seconds.
#define POST_STIMULATION_MAX_TIME_SEC (POST_STIMULATION_MAX_TIME_MIN * 60)
/////////////////////////////////////////////////////////////////////

// Defines for the period.  The values for these defines are based on the
// period.  For example PERIOD_10 is the second 10 minute period (including
// the baseline).
#define PERIOD_NONE INV_SHORT
#define PERIOD_BL   0 // Baseline period
#define PERIOD_10   1 // First ten minute period
#define PERIOD_20   2 // Second ten minute period
#define PERIOD_30   3 // Third ten minute period
#define PERIOD_40   4 // Fourth ten minute period
#define MAX_PERIODS  (PERIOD_30 + 1)
#define MAX_MANUAL_MODE_PERIODS (PERIOD_40 + 1)
#define TOTAL_PERIODS  (PERIOD_40 + 1)
  // Maximum periods.
  // Post-stimulation max hours * 60 = max minutes.
  // max minutes / 4 minutes per period plus 1 for the baseline.
#define MAX_TOTAL_PERIODS (((POST_STIMULATION_MAX_TIME_HRS * 60) / 4) + 1)
//  // For research version
//#define PERIOD_CEILING 100 // Maximum number of periods.

//// Bits to define the periods.  These are ORed together so 1 byte can hold
//// the information needed to determine what periods have been analyzed.
//#define PERIOD_BL_BIT  (0x1 << PERIOD_BL)
//#define PERIOD_10_BIT  (0x1 << PERIOD_10)
//#define PERIOD_20_BIT  (0x1 << PERIOD_20)
//#define PERIOD_30_BIT  (0x1 << PERIOD_30)
//#define PERIOD_40_BIT  (0x1 << PERIOD_40)

#define PERIOD_MASK 0xF // Bits representing PERIOD_BL through PERIOD_30.

// Structure to hold the patient data (includes patient information,
// pre-study information and study information).
typedef struct
  {
    // Data base information.
  long lPatientID;
  long lStudyID;
    // Demographics
	CString	cstrLastName;
	CString	cstrFirstName;
  CString cstrMI; // Middle initial
	CString	cstrAddress1;
	CString	cstrAddress2;
	CString	cstrCity;
	CString	cstrDateOfBirth;
	CString	cstrHospitalNum;
	CString	cstrPhone;
  BOOL bUseResrchRefNum;
	CString	cstrResearchRefNum;
	CString	cstrSocialSecurityNum;
	CString	cstrState;
	CString	cstrZipCode;
    // Pre-study information
	CString	cstrMealTime;
	CString	cstrAttendPhys;
	CString	cstrIFPComments; // Indication for procedure comments.
	CString	cstrReferPhys;
	CString	cstrTechnician;
	BOOL bBloating;
	BOOL bEarlySateity;
	BOOL bEructus;
	BOOL bGerd;
	BOOL bNausea;
	BOOL bPPFullness;
	BOOL bVomiting;
  BOOL bDelayedGastricEmptying;
    // Water
  unsigned short uWaterAmount; // Amount of water
  unsigned short uWaterUnits; // Units for the amount of water.
  CString cstrWaterUnits; // Text string for water units.
    // Study data
  CString cstrDate; // Date of study.
  CString cstrFile; // Name of file.
  unsigned uNumDataPts; // Number of data points read from file for period.
    // Length in minutes of each period.
//  unsigned short uPeriodStart[TOTAL_PERIODS];
//  unsigned short uPeriodLen[TOTAL_PERIODS];
  float fPeriodStart[MAX_TOTAL_PERIODS];
  float fPeriodLen[MAX_TOTAL_PERIODS];
    // Start and end of good minutes for each period.
  float fGoodMinStart[MAX_TOTAL_PERIODS]; //TOTAL_PERIODS];
  float fGoodMinEnd[MAX_TOTAL_PERIODS]; //TOTAL_PERIODS];
  float fTestMaxEGG; // Maximum EGG signal for the equipment test period.
  float fTestMinEGG; // Minimum EGG signal for the equipment test period.
  float fMaxEGG; // Maximum EGG signal for the current period.
  float fMinEGG; // Minimum EGG signal for the current period.
  float fMaxResp; // Maximum Respiration signal for the current period.
  float fMinResp; // Minimum Respiration signal for the current period.
  float fMeanBaseline; // Mean of the baseline good minutes. Used for artifact detection.
//  unsigned short uPeriodsAnalyzed; // Bits to indicate which periods have
//                                   // been analyzed.
  bool bPeriodsAnalyzed[MAX_TOTAL_PERIODS]; //PERIOD_CEILING]; // true indicate period has been analyzed.
  CString cstrSuggestedDiagnosis; // Suggested diagnosis.
  CString cstrDiagnosis; // Modified diagnosis (will be empty if suggested diagnosis accepted).
  BOOL bUserDiagnosis; // TRUE if user entered a diagnosis.
  short int iDiagType; // 0 - User accepted suggested diagnosis
                       // Not 0 - user modified suggested diagnosis.
  CString cstrRecommendations; // User entered recommendations.
  CString cstrMedications; // Medications.
  BOOL bExtra10Min; // TRUE if ran study an extra 10 minutes.
  bool bHavePatientInfo;  // true if we have the information from the PatientInfo database table.
  BOOL bManualMode; // TRUE - user is in manual mode for this study.
  short unsigned uRespRate[MAX_TOTAL_PERIODS]; //TOTAL_PERIODS]; // Holds the respiration rate for each period.
    // For research version
  short unsigned uNumPeriods; // Number of periods (minus baseline) user has defined.
  CString cstrStimMedDesc; // Stimulation medium description.
  unsigned uDataPtsInBaseline; // Number of data points in the Baseline.
                               // One data point consists of a EGG value and a respiration
                               // value.
  BOOL bPause; // TRUE - study has been paused.
  unsigned uPostStimDataPoints; // Number of data points in the post-stimulation study.
#ifdef EGG_EVENTS
  CEvents Events;
#endif
  short unsigned uPatientType; // Human or Dog
  } PATIENT_DATA;

#ifdef EGG_WHAT_IF
// Structure for holding the data for a What If scenario.
typedef struct
  {
  long lWhatIfID;
  long lStudyID;
  CString cstrDescription;
  CString cstrDateTime;
  short unsigned uNumPeriods; // Number of periods (minus baseline) user has defined.
  float fPeriodStart[MAX_TOTAL_PERIODS];
  float fPeriodLen[MAX_TOTAL_PERIODS];
    // Start and end of good minutes for each period.
  float fGoodMinStart[MAX_TOTAL_PERIODS];
  float fGoodMinEnd[MAX_TOTAL_PERIODS];
  short unsigned uRespRate[MAX_TOTAL_PERIODS]; // Holds the respiration rate for each period.
  } WHAT_IF;
#endif


