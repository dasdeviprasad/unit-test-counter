/********************************************************************
analyze.h

Copyright (C) 2003,2004,2012, 3CPM Company, Inc.  All rights reserved.

This file defines the CAnalyze class for implementing data analysis. 

HISTORY:

03-OCT-99  RET  New.
30-JUN-00  RET Add Dominant frequency. 
                 Add class methods:
                   - fGet_dom_freq_se()
                   - iGet_dom_freq_point()
                   - fArray_index_to_freq()
                   - vCalc_mean_std_dev()
                  Add class members:
                   - m_fDomFreq4MinSE
                   - m_iDomFreq4MinPt
                  Add parameter to bAnalyze_data().
07-DEC-00  RET  Move ANALYZED_DATA structure to eggsap.h.
16-JAN-01  RET  Remove class variables: m_fDomFreq4MinSE, m_iDomFreq4MinPt.
                Remove prototypes for fGet_dom_freq_se(), iGet_dom_freq_point().
03-JUL-01  RET  Modifications to add 15-50 cpm.
                  - Add defines:
                      FREQ_BAND7, FREQ_BAND8
                      FREQ_BAND7_LOW_CPM, FREQ_BAND7_HIGH_CPM
                      FREQ_BAND8_LOW_CPM, FREQ_BAND8_HIGH_CPM
05-JUL-01  RET  Modifications to add 15-50 cpm.
                  - Add defines:
                      H_FREQ_BAND7_LOW_INDEX, H_FREQ_BAND7_HIGH_INDEX
                      H_FREQ_BAND8_LOW_INDEX, H_FREQ_BAND8_HIGH_INDEX
                      NUM_FREQ_BANDS_50, MAX_FREQ_POINTS_50,
                      MAX_FREQ_BANDS
                  - Add class variables:
                      m_uiFreqBand5LowIndex, m_uiFreqBand5HighIndex
                      m_uiFreqBand6LowIndex, m_uiFreqBand6HighIndex
                      m_pApp, m_iNumFreqBands, m_iMaxFreqPoints
14-OCT-08  RET  Version 2.07a
                  Add defines for dog frequencies: D_FREQ_BAND1_LOW_INDEX,
                    D_FREQ_BAND1_HIGH_INDEX, D_FREQ_BAND2_LOW_INDEX,
                    D_FREQ_BAND2_HIGH_INDEX, D_FREQ_BAND3_LOW_INDEX,
                    D_FREQ_BAND3_HIGH_INDEX, D_FREQ_BAND4_LOW_INDEX,
                    D_FREQ_BAND4_HIGH_INDEX
                  Change FREQ_BAND1_LOW_CPM, FREQ_BAND1_HIGH_CPM,
                    FREQ_BAND2_LOW_CPM, FREQ_BAND2_HIGH_CPM,
                    FREQ_BAND3_LOW_CPM, FREQ_BAND3_HIGH_CPM,
                    FREQ_BAND4_LOW_CPM, FREQ_BAND4_HIGH_CPM,
                    FREQ_BAND5_LOW_CPM, FREQ_BAND5_HIGH_CPM,
                    FREQ_BAND6_LOW_CPM, FREQ_BAND6_HIGH_CPM
                    to have a leading H_ for human.
                  Add frequency band defines for dog:
                    D_FREQ_BAND1_LOW_CPM, D_FREQ_BAND1_HIGH_CPM,
                    D_FREQ_BAND2_LOW_CPM, D_FREQ_BAND2_HIGH_CPM,
                    D_FREQ_BAND3_LOW_CPM, D_FREQ_BAND3_HIGH_CPM,
                    D_FREQ_BAND4_LOW_CPM, D_FREQ_BAND4_HIGH_CPM
22-SEP-12  RET
             Change for 60/200 CPM
               Add defines: NUM_FREQ_BANDS_60, NUM_FREQ_BANDS_200,
                 MAX_FREQ_POINTS_60, MAX_FREQ_POINTS_200

                 FREQ_BAND9, FREQ_BAND10, FREQ_BAND11, FREQ_BAND12, FREQ_BAND13,
                 FREQ_BAND14, FREQ_BAND15, FREQ_BAND16, FREQ_BAND17, FREQ_BAND18,
                 FREQ_BAND19,
                 MAX_FREQ_BANDS_15_CPM, MAX_FREQ_BANDS_15_CPM_IDX,
                 NUM_FREQ_BANDS_60, NUM_FREQ_BANDS_200,
                 MAX_FREQ_BANDS_60_CPM, MAX_FREQ_BANDS_60_CPM_IDX,
                 MAX_FREQ_BANDS_200_CPM, MAX_FREQ_BANDS_200_CPM_IDX,
                 H_FREQ_BAND7_LOW_CPM, H_FREQ_BAND7_HIGH_CPM,
                 H_FREQ_BAND8_LOW_CPM, H_FREQ_BAND8_HIGH_CPM,
                 H_FREQ_BAND9_LOW_CPM, H_FREQ_BAND9_HIGH_CPM,
                 H_FREQ_BAND10_LOW_CPM, H_FREQ_BAND10_HIGH_CPM,
                 H_FREQ_BAND11_LOW_CPM, H_FREQ_BAND11_HIGH_CPM,
                 H_FREQ_BAND12_LOW_CPM, H_FREQ_BAND12_HIGH_CPM,
                 H_FREQ_BAND13_LOW_CPM, H_FREQ_BAND13_HIGH_CPM,
                 H_FREQ_BAND14_LOW_CPM, H_FREQ_BAND14_HIGH_CPM,
                 H_FREQ_BAND15_LOW_CPM, H_FREQ_BAND15_HIGH_CPM,
                 H_FREQ_BAND16_LOW_CPM, H_FREQ_BAND16_HIGH_CPM,
                 H_FREQ_BAND17_LOW_CPM, H_FREQ_BAND17_HIGH_CPM,
                 H_FREQ_BAND18_LOW_CPM, H_FREQ_BAND18_HIGH_CPM,
                 H_FREQ_BAND7_LOW_INDEX, H_FREQ_BAND7_HIGH_INDEX,
                 H_FREQ_BAND8_LOW_INDEX, H_FREQ_BAND8_HIGH_INDEX,
                 H_FREQ_BAND9_LOW_INDEX, H_FREQ_BAND9_HIGH_INDEX,
                 H_FREQ_BAND10_LOW_INDEX, H_FREQ_BAND10_HIGH_INDEX,
                 H_FREQ_BAND11_LOW_INDEX, H_FREQ_BAND11_HIGH_INDEX,
                 H_FREQ_BAND12_LOW_INDEX, H_FREQ_BAND12_HIGH_INDEX,
                 H_FREQ_BAND13_LOW_INDEX, H_FREQ_BAND13_HIGH_INDEX,
                 H_FREQ_BAND14_LOW_INDEX, H_FREQ_BAND14_HIGH_INDEX,
                 H_FREQ_BAND15_LOW_INDEX, H_FREQ_BAND15_HIGH_INDEX,
                 H_FREQ_BAND16_LOW_INDEX, H_FREQ_BAND16_HIGH_INDEX,
                 H_FREQ_BAND17_LOW_INDEX, H_FREQ_BAND17_HIGH_INDEX,
                 H_FREQ_BAND18_LOW_INDEX, H_FREQ_BAND18_HIGH_INDEX
                     
               Change MAX_FREQ_BANDS to MAX_FREQ_BANDS_DIAG.
               Change MAX_FREQ_BANDS, MAX_FREQ_BANDS_IDX to be set properly
                 for 60/200 cpm
               Add arrays: m_uiaFreqBandLowIndex[], m_uiaFreqBandHighIndex[]
               Add variables: m_uiFreqMaxBandForAnalysis, m_uSummBand1, m_uSummBand2
               Change arrays in structure ANALYZED_DATA to size arrays to
                 the maximum frequency bands (for 200 cpm).
********************************************************************/

#include "math.h"

#ifndef _200_CPM_HARDWARE
// Frequency band parameters for computing powers/sums of powers.
#define NUM_FREQ_BANDS    6 // For 0 to 15 cpm
//#define NUM_FREQ_BANDS_50 8  // For 0 to 50 cpm.

#define FREQ_BAND1  0  // Human: 0 to 2.25 cpm, Dog: 0.00 to 0.00 cpm
#define FREQ_BAND2  1  // Human: 2.5 to 3.5 cpm, Dog: 0.00 to 3.00 cpm (DOG Brady)
#define FREQ_BAND3  2  // Human: 3.75 to 9.75 cpm, Dog: 3.25 to 6.00 cpm (DOG Normal)
#define FREQ_BAND4  3  // Human: 10.0 to 15.0 cpm, Dog: 6.25 to 15.0 cpm (DOG Tachy)
                                                   // Dog: No duodenal
#define FREQ_BAND5  4  // Human: 15.25 to 30 cpm
#define FREQ_BAND6  5  // Human: 30.25 to 50 cpm
#define FREQ_BAND7  6  // Frequency bands: 15cpm: band 1 thru 4
                       //                  50cpm:  1 thru 6
                       //                  15-50cpm: 5 thru 6
#define FREQ_BAND8  7  // Frequency bands: 15cpm: band 2 thru 4
                       //                  50cpm:  2 thru 6
                       //                  15-50cpm: 5 thru 6
#define MAX_FREQ_BANDS_DIAG   (FREQ_BAND8 + 1) // For use with diagnosis.
//#define MAX_FREQ_BANDS_IDX   FREQ_BAND8
#define MAX_FREQ_BANDS_15_CPM   (FREQ_BAND8 + 1)
#define MAX_FREQ_BANDS_15_CPM_IDX   FREQ_BAND8

// Humans
// 1-15 CPM Hardware
#define H_FREQ_BAND1_LOW_CPM    0.0  
#define H_FREQ_BAND1_HIGH_CPM   2.25
#define H_FREQ_BAND2_LOW_CPM    2.5
#define H_FREQ_BAND2_HIGH_CPM   3.5
#define H_FREQ_BAND3_LOW_CPM    3.75
#define H_FREQ_BAND3_HIGH_CPM   9.75
#define H_FREQ_BAND4_LOW_CPM    10.0
#define H_FREQ_BAND4_HIGH_CPM   15.0
#define H_FREQ_BAND5_LOW_CPM    15.25
#define H_FREQ_BAND5_HIGH_CPM   30.0
#define H_FREQ_BAND6_LOW_CPM    30.25
#define H_FREQ_BAND6_HIGH_CPM   50.0

// Humans
#define H_FREQ_BAND1_LOW_INDEX    0
#define H_FREQ_BAND1_HIGH_INDEX   9
#define H_FREQ_BAND2_LOW_INDEX    10
#define H_FREQ_BAND2_HIGH_INDEX   14
#define H_FREQ_BAND3_LOW_INDEX    15
#define H_FREQ_BAND3_HIGH_INDEX   39
#define H_FREQ_BAND4_LOW_INDEX    40
#define H_FREQ_BAND4_HIGH_INDEX   60
#define H_FREQ_BAND5_LOW_INDEX    61
#define H_FREQ_BAND5_HIGH_INDEX   120
#define H_FREQ_BAND6_LOW_INDEX    121
#define H_FREQ_BAND6_HIGH_INDEX   200

#else
//// 200 CPM Hardware

// Frequency band parameters for computing powers/sums of powers.
#define NUM_FREQ_BANDS    6 // For 0 to 15 cpm
//#define NUM_FREQ_BANDS_50 8  // For 0 to 50 cpm.
#define NUM_FREQ_BANDS_60    9 // For 0 to 60 cpm
#define NUM_FREQ_BANDS_200    18 // For 0 to 200 cpm

#define FREQ_BAND1  0  // Human: 0 to 2.25 cpm, Dog: 0.00 to 0.00 cpm
#define FREQ_BAND2  1  // Human: 2.5 to 3.5 cpm, Dog: 0.00 to 3.00 cpm (DOG Brady)
#define FREQ_BAND3  2  // Human: 3.75 to 9.75 cpm, Dog: 3.25 to 6.00 cpm (DOG Normal)
#define FREQ_BAND4  3  // Human: 10.0 to 15.0 cpm, Dog: 6.25 to 15.0 cpm (DOG Tachy)
                                                   // Dog: No duodenal
#define FREQ_BAND5  4   // 15.25 - 20 cpm
#define FREQ_BAND6  5   // 20.25 - 30 cpm
#define FREQ_BAND7  6  // 30.25 - 40 cpm
#define FREQ_BAND8  7  // 40.25 - 50 cpm
#define FREQ_BAND9  8  // 50.25 - 60 cpm
// For 60 cpm, done programtically
//#define FREQ_BAND10  9  // Frequency bands: 15cpm: band 1 thru 9
//#define FREQ_BAND11  10  //  Frequency bands: 15cpm: band 2 thru 9
#define FREQ_BAND10  9  // 60.25 - 70 cpm
#define FREQ_BAND11  10  // 70.25 - 80 cpm
#define FREQ_BAND12  11  // 80.25 - 90 cpm
#define FREQ_BAND13  12  // 90.25 - 100 cpm
#define FREQ_BAND14  13  // 100.25 - 120 cpm
#define FREQ_BAND15  14  // 120.25 - 140 cpm
#define FREQ_BAND16  15  // 140.25 - 160 cpm
#define FREQ_BAND17  16  // 160.25 - 180 cpm
#define FREQ_BAND18  17  // 180.25 - 200 cpm
#define FREQ_BAND19  18  // Frequency bands: 15cpm: band 1 thru 17
#define FREQ_BAND20  19  //  Frequency bands: 15cpm: band 2 thru 17

////#define FREQ_BAND5  4  // Human: 15.25 to 30 cpm  NOT USED
////#define FREQ_BAND6  5  // Human: 30.25 to 50 cpm NOT USED
//#define FREQ_BAND7  6  // Frequency bands: 15cpm: band 1 thru 4
//                       //                  50cpm:  1 thru 6
//                       //                  15-50cpm: 5 thru 6
//#define FREQ_BAND8  7  // Frequency bands: 15cpm: band 2 thru 4
//                       //                  50cpm:  2 thru 6
//                       //                  15-50cpm: 5 thru 6
//#ifndef _200_CPM_HARDWARE
#define MAX_FREQ_BANDS_DIAG   (FREQ_BAND8 + 1) // For use with diagnosis.
//#define MAX_FREQ_BANDS_IDX   FREQ_BAND8
#define MAX_FREQ_BANDS_15_CPM   (FREQ_BAND8 + 1)
#define MAX_FREQ_BANDS_15_CPM_IDX   FREQ_BAND8

#define MAX_FREQ_BANDS_60_CPM   (FREQ_BAND11 + 1)
#define MAX_FREQ_BANDS_60_CPM_IDX   FREQ_BAND11
#define MAX_FREQ_BANDS_200_CPM   (FREQ_BAND20 + 1)
#define MAX_FREQ_BANDS_200_CPM_IDX   FREQ_BAND20

  // Humans
#define H_FREQ_BAND1_LOW_CPM     0.0  
#define H_FREQ_BAND1_HIGH_CPM    2.25
#define H_FREQ_BAND2_LOW_CPM     2.5
#define H_FREQ_BAND2_HIGH_CPM    3.5
#define H_FREQ_BAND3_LOW_CPM     3.75
#define H_FREQ_BAND3_HIGH_CPM    9.75
#define H_FREQ_BAND4_LOW_CPM     10.0
#define H_FREQ_BAND4_HIGH_CPM    15.0
#define H_FREQ_BAND5_LOW_CPM     15.25
#define H_FREQ_BAND5_HIGH_CPM    20.0
#define H_FREQ_BAND6_LOW_CPM     20.25
#define H_FREQ_BAND6_HIGH_CPM    30.0
#define H_FREQ_BAND7_LOW_CPM     30.25
#define H_FREQ_BAND7_HIGH_CPM    40.0
#define H_FREQ_BAND8_LOW_CPM     40.25
#define H_FREQ_BAND8_HIGH_CPM    50.0
#define H_FREQ_BAND9_LOW_CPM     50.25
#define H_FREQ_BAND9_HIGH_CPM    60.0
#define H_FREQ_BAND10_LOW_CPM    60.25
#define H_FREQ_BAND10_HIGH_CPM   70.0
#define H_FREQ_BAND11_LOW_CPM    70.25
#define H_FREQ_BAND11_HIGH_CPM   80.0
#define H_FREQ_BAND12_LOW_CPM    80.25
#define H_FREQ_BAND12_HIGH_CPM   90.0
#define H_FREQ_BAND13_LOW_CPM    90.25
#define H_FREQ_BAND13_HIGH_CPM   100.0
#define H_FREQ_BAND14_LOW_CPM    100.25
#define H_FREQ_BAND14_HIGH_CPM   120.0
#define H_FREQ_BAND15_LOW_CPM    120.25
#define H_FREQ_BAND15_HIGH_CPM   140.0
#define H_FREQ_BAND16_LOW_CPM    140.25
#define H_FREQ_BAND16_HIGH_CPM   160.0
#define H_FREQ_BAND17_LOW_CPM    160.25
#define H_FREQ_BAND17_HIGH_CPM   180.0
#define H_FREQ_BAND18_LOW_CPM    180.25
#define H_FREQ_BAND18_HIGH_CPM   200.0

  // Humans
#define H_FREQ_BAND1_LOW_INDEX     0  // 0.0 
#define H_FREQ_BAND1_HIGH_INDEX    9  // 2.25
#define H_FREQ_BAND2_LOW_INDEX     10 // 2.5
#define H_FREQ_BAND2_HIGH_INDEX    14 // 3.5
#define H_FREQ_BAND3_LOW_INDEX     15 // 3.75
#define H_FREQ_BAND3_HIGH_INDEX    39 // 9.75 
#define H_FREQ_BAND4_LOW_INDEX     40 // 10.0
#define H_FREQ_BAND4_HIGH_INDEX    60 // 15.0
#define H_FREQ_BAND5_LOW_INDEX     61  //  15.25
#define H_FREQ_BAND5_HIGH_INDEX    80  //  20.0
#define H_FREQ_BAND6_LOW_INDEX     81  //  20.25
#define H_FREQ_BAND6_HIGH_INDEX    120 //  30.0
#define H_FREQ_BAND7_LOW_INDEX     121 //  30.25
#define H_FREQ_BAND7_HIGH_INDEX    160 //  40.0
#define H_FREQ_BAND8_LOW_INDEX     161 //  40.25
#define H_FREQ_BAND8_HIGH_INDEX    200 //  50.0
#define H_FREQ_BAND9_LOW_INDEX     201 //  50.25
#define H_FREQ_BAND9_HIGH_INDEX    240 //  60.0
#define H_FREQ_BAND10_LOW_INDEX    241 //  60.25
#define H_FREQ_BAND10_HIGH_INDEX   280 //  70.0
#define H_FREQ_BAND11_LOW_INDEX    281 //  70.25
#define H_FREQ_BAND11_HIGH_INDEX   320 //  80.0
#define H_FREQ_BAND12_LOW_INDEX    321 //  80.25
#define H_FREQ_BAND12_HIGH_INDEX   360 //  90.0
#define H_FREQ_BAND13_LOW_INDEX    361 //  90.25
#define H_FREQ_BAND13_HIGH_INDEX   400 //  100.0
#define H_FREQ_BAND14_LOW_INDEX    401 //  100.25
#define H_FREQ_BAND14_HIGH_INDEX   480 //  120.0
#define H_FREQ_BAND15_LOW_INDEX    481 //  120.25
#define H_FREQ_BAND15_HIGH_INDEX   560 //  140.0
#define H_FREQ_BAND16_LOW_INDEX    561 //  140.25
#define H_FREQ_BAND16_HIGH_INDEX   640 //  160.0
#define H_FREQ_BAND17_LOW_INDEX    641 //  160.25
#define H_FREQ_BAND17_HIGH_INDEX   720 //  180.0
#define H_FREQ_BAND18_LOW_INDEX    721 //  180.25
#define H_FREQ_BAND18_HIGH_INDEX   800 //  200.0

#endif

//#define H_15_CPM_FREQ_MAX_BAND_FOR_ANALYSIS 4
//#define H_200_CPM_FREQ_MAX_BAND_FOR_ANALYSIS 18

// Dog
#define D_FREQ_BAND1_LOW_CPM    0.0  
#define D_FREQ_BAND1_HIGH_CPM   0.0
#define D_FREQ_BAND2_LOW_CPM    0.0
#define D_FREQ_BAND2_HIGH_CPM   3.0
#define D_FREQ_BAND3_LOW_CPM    3.25
#define D_FREQ_BAND3_HIGH_CPM   6.0
#define D_FREQ_BAND4_LOW_CPM    6.25
#define D_FREQ_BAND4_HIGH_CPM   15.0

  // Maximum number of data points to get to 15 CPM in increments of .25 CPM.
  // This is inclusive from 0 to 15 so we have to add 1 data point.
#define MAX_FREQ_POINTS ((H_FREQ_BAND4_HIGH_CPM * 4) + 1) // For 0 to 15 cpm
#define MAX_FREQ_POINTS_50 ((H_FREQ_BAND6_HIGH_CPM * 4) + 1) // For 0 to 50 cpm

#define MAX_FREQ_POINTS_60 ((H_FREQ_BAND9_HIGH_CPM * 4) + 1) // For 0 to 60 cpm
#define MAX_FREQ_POINTS_200 ((H_FREQ_BAND18_HIGH_CPM * 4) + 1) // For 0 to 200 cpm

  // Dogs
#define D_FREQ_BAND1_LOW_INDEX    0
#define D_FREQ_BAND1_HIGH_INDEX   0
#define D_FREQ_BAND2_LOW_INDEX    0
#define D_FREQ_BAND2_HIGH_INDEX   12
#define D_FREQ_BAND3_LOW_INDEX    13
#define D_FREQ_BAND3_HIGH_INDEX   24
#define D_FREQ_BAND4_LOW_INDEX    25
#define D_FREQ_BAND4_HIGH_INDEX   60

// Data for the high pass filter.
//#define HPF_LOW_FREQ_CUTOFF  0.010 // in HERTZ - so 0.6 cycles per MINUTE
#define HPF_TIME 0.2344 // this is 1 sec / sampling rate - so 1 / 4.267 = 0.2344
#define HPF_COEFF 3 // Number of coefficients or the number of filter sections.

// Structure for holding calculated data.
// The members are all pointers.  The members that are used for a particular
// function have to be allocated at run time, the other members must be NULL.
// All arrays have to be large enough to hold data for all specified
// time bands for all frequency bands.
typedef struct
  {
  float fData[DATA_POINTS_PER_EPOCH_1];  // Analyzed data up to and including the FFT (All).
  float *pfPwrSum; // Power summary (Reports).
  float *pfRatioAvgPwr; // Ratio average power (POSTprandial/PREprandial) (Reports).
#ifdef _200_CPM_HARDWARE
  float fAvgPwr[MAX_TOTAL_PERIODS * MAX_FREQ_BANDS_200_CPM]; // Average power (Reports).
//  float fAvgPwr[TOTAL_PERIODS * MAX_FREQ_BANDS]; // Average power (Reports).
  float fPctGrp1[MAX_TOTAL_PERIODS * MAX_FREQ_BANDS_200_CPM]; // Group 1 percentages (Reports).
  float fPctGrp2[MAX_TOTAL_PERIODS * MAX_FREQ_BANDS_200_CPM]; // Group 2 percentages (Reports).
#else
  float fAvgPwr[MAX_TOTAL_PERIODS * MAX_FREQ_BANDS_15_CPM]; // Average power (Reports).
//  float fAvgPwr[TOTAL_PERIODS * MAX_FREQ_BANDS]; // Average power (Reports).
  float fPctGrp1[MAX_TOTAL_PERIODS * MAX_FREQ_BANDS_15_CPM]; // Group 1 percentages (Reports).
  float fPctGrp2[MAX_TOTAL_PERIODS * MAX_FREQ_BANDS_15_CPM]; // Group 2 percentages (Reports).
#endif
//  float fPctGrp1[TOTAL_PERIODS * MAX_FREQ_BANDS]; // Group 1 percentages (Reports).
//  float fPctGrp2[TOTAL_PERIODS * MAX_FREQ_BANDS]; // Group 2 percentages (Reports).
  float *pfDomFreq; // Dominant frequency.
  float *pfPctDomFreq; // Total minutes that the dominant frequency is in the
                       // various frequency ranges.
  float *pfRawData;  // Raw data (View 4-min epoch).
  float fDomFreq4MinSE;  // Dominant frequency spectral estimate value for 
                         // a 4-minute epoch.
  int iDomFreq4MinPt;  // Point within the 4-minute epoch where the dominant
                       // frequency occurs.
  int iNumDataPoints; // Number of data points read from file.
  } ANALYZED_DATA;

// Structure used to pass parameters to bAnalyze_data().
typedef struct
  {
  int iType;  // Type of function to be performed with the analyzed data.
              // This can be for viewing the spectra or printing reports.
  float fEpochNum; // The epoch number.  This must be a 0 based number and is
                   // used to index into the data file to read the correct data.
  unsigned int uTimeBand; // The timeband number.  This is a 0 based
                           // number and is used to calculate sums for reports.
  short int iChannel; // The channel number being analyzed. Starts at 0.
  short int iNumChannelsInFile; // Number of channels in the data file.
  ANALYZED_DATA *padData; // Pointer to a ANALYZED_DATA structure where the
                          // analyzed data is to be stored.
  unsigned short uRunningEpochCnt; // The running count of epochs.
                                   // Used for the data sheet report.
                                   // For diagnosis, number of epochs in current period.
  bool bUsePartMinAtDataEnd; // true - use any partial minutes at end of file.
  }REQ_ANALYZE_DATA; // Request to analyze data.

// Defines for the type of analysis being performed.
#define RSA_TYPE  1  // Analysis required for displaying the RSA.
#define EPOCH_SPECTRA_TYPE  2 // Analysis required to display one epcoch of 
                              // power and raw signal.
#define DATA_SHEET_TYPE     3 // Analysis required to process the data for 
                              // the data sheet report.
#define EGG_SUMMARY_TYPE  4  // Analysis required to process the data for 
#define DIAGNOSIS_TYPE    4  // Analysis required to make a diagnosis.

/////////////////////////////////////////////////////////////////////
class CAnalyze
{
public:
    CAnalyze(CEGGSASDoc *pDoc);  // Constructor
    ~CAnalyze();  // Destructor

public: // Data
  CEGGSASDoc *m_pDoc;

  float m_fScaleFactor; // Scale factor.
  CString m_cstrErrMsg; // Contains the last error message encountered by
                        // an operation in this class.
  float m_fLowFreqCutOff; // Low frequency cut-off for the high pass filter.
  int m_iNumFreqBands; // Total number of frequency bands.
  int m_iMaxFreqPoints; // Max number of frequency points in .25cpm increments.

	BOOL m_bSubResp;
	int m_iRespChannel;
    // Indexes used to index into frequency bands for calculating average powers,
    // power summaries, ratios and percentages.  These are set according whether
    // the patient is a dog or human.
  unsigned int m_uiFreqBand1LowIndex;
  unsigned int m_uiFreqBand1HighIndex;
  unsigned int m_uiFreqBand2LowIndex;
  unsigned int m_uiFreqBand2HighIndex;
  unsigned int m_uiFreqBand3LowIndex;
  unsigned int m_uiFreqBand3HighIndex;
  unsigned int m_uiFreqBand4LowIndex;
  unsigned int m_uiFreqBand4HighIndex;
  unsigned int m_uiFreqBand5LowIndex;
  unsigned int m_uiFreqBand5HighIndex;
  unsigned int m_uiFreqBand6LowIndex;
  unsigned int m_uiFreqBand6HighIndex;

#ifdef _200_CPM_HARDWARE
  unsigned int m_uiaFreqBandLowIndex[MAX_FREQ_BANDS_200_CPM];
  unsigned int m_uiaFreqBandHighIndex[MAX_FREQ_BANDS_200_CPM];
#else
  unsigned int m_uiaFreqBandLowIndex[MAX_FREQ_BANDS_15_CPM];
  unsigned int m_uiaFreqBandHighIndex[MAX_FREQ_BANDS_15_CPM];
#endif
  // Maximum number of frequency bands used for analysis.
  unsigned int m_uiFreqMaxBandForAnalysis;
  // Used in place of FREQ_BAND7
  unsigned int m_uSummBand1;
  // Used in place of FREQ_BAND8
  unsigned int m_uSummBand2;


public: // Methods
	void vSet_sub_resp(BOOL bSubResp);
	void vSet_resp_channel(int iRespChannel);
  CString cstrGetErrMsg();
  bool bAnalyze_data(REQ_ANALYZE_DATA *pReq);
  void vHigh_pass_filter(float *pfData, unsigned int uiNumPoints);
  float fArray_index_to_freq(int iArrayIndex);
  void vCopy_data_points(float *pfDest, float *pfSrc,
                         int iPointsToCopy, int iChannel, int iNumChannels);
  unsigned uCalc_freq_points(unsigned uLowFreq, unsigned uHighFreq);

  void vCalc_fft(float *pfDest, float *pfSrc, unsigned uNumPoints);
  void vCalc_diagnosis(REQ_ANALYZE_DATA *pReq);
  void vCalc_datasheet(REQ_ANALYZE_DATA *pReq);
  short int iCalc_respiration_rate(unsigned short uPeriod, float fStart, float fEnd);
  float fFind_mean_signal(float *pfData, float fMinStart, float fMinEnd);
  void vSet_freq_bands(unsigned int uiType);
  void vSet_freq_range(int iFreqRange);
};
