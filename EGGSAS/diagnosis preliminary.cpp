
#define DIAG_NONE                    -1 // No diagonsis.
#define DIAG_NORMAL                  0 // Normal
#define DIAG_PROB_NORM               1 // Probable Normal
#define DIAG_BRADY                   2 // Bradygastria
#define DIAG_POS_BRADY_CC            3 // Possible Bradygastria Clinical Correlation Required
#define DIAG_TACHY                   4 // Tachygastria
#define DIAG_POS_TACHY_CC            5 // Possible Tachygastria Clinical Correlation Required
#define DIAG_PROB_TACHY              6 // Probable Tachygastria
#define DIAG_DUOD                    7 // Duodenal Hyperactivity
#define DIAG_RESP_EFFECT             8 // Respiratory Effect
#define DIAG_GAS_OUT_OBS             9 // Gastric Outlet Obstruction 
#define DIAG_POS_GAS_OUT_OBS        10 // Possible Gastric Outlet Obstruction 
#define DIAG_PROB_NORM_GAS_OUT_OBS  11 // Probable Normal Consider Gastric Outlet Obstruction
#define DIAG_MIX_DYS                12 // Mixed Dysrythmia


#define IDS_DIAG_PP_NORM "Post-prandial EGG is suggestive of Normal gastric rhythm."
#define IDS_DIAG_PP_TACHY "Post-prandial EGG is suggestive of Tachygastria."
#define IDS_DIAG_PP_BRADY "Post-prandial EGG is suggestive of Bradygastria."
#define IDS_DIAG_PP_MXNS "Post-prandial EGG is suggestive of Mixed/Nonspecific Dysrhythmia."
#define IDS_DIAG_PP_GOO "Sustained 3 cpm activity in the post-prandial EGG is suggestive of Gastric Outlet Obstruction."
#define IDS_DIAG_PP_RPD "Sustained duodenal/respiratory activity in the post-prandial EGG is suggestive of Respiration Pattern Dysrhythmia."
#define IDS_DIAG_PP_UGR "Abnormal gastric rhythms in the post-prandial EGG are suggestive on an Unspecified Gastric Rhythm."

// Control data
//  First dimension: Baseline(high,low), min10(high,low), 
//                   min20(high,low), min30(high,low)
//  Second dimension: Frequency band 1 through 4
//  A value of -1 indicates there is no control value.  So control
//    values will neither be displayed nor graphed.
// These values should go into a file.
static float s_fControl[8][4] =
  {
  {25.0, 47.0, 27.0, 23.0}, // Bradygastria (frequency band 1) low
  {61.0, 77.0, 65.0, 45.0}, //    high

  {18.0, 10.0, 23.0, 36.0}, // Normal (frequency band 2) low
  {54.0, 36.0, 57.0, 68.0}, //    high

  {7.0, 8.0, 7.0, 7.0},     // Tachygastria (frequency band 3) low
  {25.0, 18.0, 17.0, 17.0}, //    high

  {2.0, 0.0, 1.0, 0.0},     // Duodenal (frequency band 4) low
  {10.0, 6.0, 5.0, 8.0},    //    high
  };

#define NORMAL     0  // below high normal range
#define ABOVE_NORM 1  // above high normal range
  // Indexes into arrays that indicate value in or out of normal range.
enum eRangeIndex
  {
  RI_BRADY = 0,
  RI_3CPM,
  RI_TACHY,
  RI_DUOD,
  };
  // Holds indicators of whether frequency band is normal or above normal.
  //   0 = Normal
  //   1 = above normal.
  //   Frequency bands: 
  //     element 0 = bradygastria
  //     element 1 = normal
  //     element 2 = tachygastria
  //     element 2 = duodenal/respiratory
static unsigned short s_uPreWater[4];
static unsigned short s_uPostWater10[4];
static unsigned short s_uPostWater20[4];
static unsigned short s_uPostWater30[4];

/////////////////////////////////////////////////////////////////////
// The diagnosis routines are based on the document: Diagnostic Aid.doc

short int iDiagnose()
  {
  float *pfStart;
  short int iRet;

    // Determine pre-water values.
    // Set all values to NORMAL or 0.
  memset(s_uPreWater, 0, sizeof(s_uPreWater));
  if(*m_adData.pfPctGrp1 > s_fControl[1][0])
    s_uPreWater[RI_BRADY] = ABOVE_NORM;  // bradygastria
  if(*(m_adData.pfPctGrp1 + MAX_FREQ_BANDS) > s_fControl[3][0])
    s_uPreWater[RI_3CPM] = ABOVE_NORM; // normal
  if(*(m_adData.pfPctGrp1 + (2 * MAX_FREQ_BANDS)) > s_fControl[5][0])
    s_uPreWater[RI_TACHY] = ABOVE_NORM; // tachygastria
  if(*(m_adData.pfPctGrp1 + (3 * MAX_FREQ_BANDS)) > s_fControl[7][0])
    s_uPreWater[RI_DUOD] = ABOVE_NORM; // duodenal/respiratory
  if((iRet = iDiagnose_prewater()) == DIAG_NORMAL || iRet == DIAG_POS_GAS_OUT_OBS)
    { // Pre-water is normal or possible gastric outlet obstruction, check post-water
      // Determine post-water values.
      // Set all values to NORMAL or 0.
    memset(s_uPostWater10, 0, sizeof(s_uPostWater10));
    memset(s_uPostWater20, 0, sizeof(s_uPostWater20));
    memset(s_uPostWater30, 0, sizeof(s_uPostWater30));
      // 10 minute
    pfStart = m_adData.pfPctGrp1 + 1;
    if(*pfStart > s_fControl[1][1])
      s_uPostWater10[RI_BRADY] = ABOVE_NORM;  // bradygastria
    if(*(pfStart + MAX_FREQ_BANDS) > s_fControl[3][1])
      s_uPostWater10[RI_3CPM] = ABOVE_NORM;  // bradygastria
    if(*(pfStart + (2 * MAX_FREQ_BANDS)) > s_fControl[5][1])
      s_uPostWater10[RI_TACHY] = ABOVE_NORM; // tachygastria
    if(*(pfStart + (3 * MAX_FREQ_BANDS)) > s_fControl[7][1])
      s_uPostWater10[RI_DUOD] = ABOVE_NORM; // duodenal/respiratory
      // 20 minute
    pfStart = m_adData.pfPctGrp1 + 2;
    if(*pfStart > s_fControl[1][2])
      s_uPostWater20[RI_BRADY] = ABOVE_NORM;  // bradygastria
    if(*(pfStart + MAX_FREQ_BANDS) > s_fControl[3][2])
      s_uPostWater20[RI_3CPM] = ABOVE_NORM;  // bradygastria
    if(*(pfStart + (2 * MAX_FREQ_BANDS)) > s_fControl[5][2])
      s_uPostWater20[RI_TACHY] = ABOVE_NORM; // tachygastria
    if(*(pfStart + (3 * MAX_FREQ_BANDS)) > s_fControl[7][2])
      s_uPostWater20[RI_DUOD] = ABOVE_NORM; // duodenal/respiratory
      // 30 minute
    pfStart = m_adData.pfPctGrp1 + 3;
    if(*pfStart > s_fControl[1][3])
      s_uPostWater30[RI_BRADY] = ABOVE_NORM;  // bradygastria
    if(*(pfStart + MAX_FREQ_BANDS) > s_fControl[3][3])
      s_uPostWater30[RI_3CPM] = ABOVE_NORM;  // bradygastria
    if(*(pfStart + (2 * MAX_FREQ_BANDS)) > s_fControl[5][3])
      s_uPostWater30[RI_TACHY] = ABOVE_NORM; // tachygastria
    if(*(pfStart + (3 * MAX_FREQ_BANDS)) > s_fControl[7][3])
      s_uPostWater30[RI_DUOD] = ABOVE_NORM; // duodenal/respiratory

    iRet = iDiagnose_postwater();
    }


  return(iRet);
  }


/********************************************************************

OUT FOR VERSION II

  inputs: None.
            
********************************************************************/
/********************************************************************
#define IDS_DIAG_BL_NONE "Inadequate EGG for Diagnosis"
#define IDS_DIAG_BL_NORM "Baseline EGG is suggestive of Normal gastric rhythm."
#define IDS_DIAG_BL_TACHY "Baseline EGG is suggestive of Tachygastria."
#define IDS_DIAG_BL_BRADY "Baseline EGG is suggestive of Bradygastria."
#define IDS_DIAG_BL_DUOD "Baseline EGG is suggestive of Duodenal/Respiratory dysrhythmia."

short int iDiagnose_prewater()
  {
  short int iRet;

  if(s_uPreWater[RI_TACHY] == ABOVE_NORM)
    { // Tachy is above normal
    if(s_uPreWater[RI_BRADY] == NORMAL)
      iRet = DIAG_TACHY; // Brady is normal
    else
      iRet = DIAG_MIX_DYS; // Brady is also above normal
    }
  else
    { // Tachy is normal
    if(s_uPreWater[RI_BRADY] == ABOVE_NORM)
      iRet = DIAG_BRADY; // Brady is above normal
    else
      { // Brady is normal
      if(s_uPreWater[RI_3CPM] == NORMAL)
        iRet = DIAG_NORMAL; // Normal
      else
        iRet = DIAG_POS_GAS_OUT_OBS; // Normal is above normal
      }
    }
  return(iRet);
  }
********************************************************************/

short int iDiagnose_postwater()
  {
  short unsigned uNumTachyAboveNorm, uNumBradyAboveNorm, uNumDuodAboveNorm;
  short unsigned uNum3cpmAboveNorm;
  short int iRet = DIAG_NONE;

  uNumTachyAboveNorm = 0;
  if(s_uPostWater10[RI_TACHY] == ABOVE_NORM)
    ++uNumTachyAboveNorm;
  if(s_uPostWater20[RI_TACHY] == ABOVE_NORM)
    ++uNumTachyAboveNorm;
  if(s_uPostWater30[RI_TACHY] == ABOVE_NORM)
    ++uNumTachyAboveNorm;
  uNumBradyAboveNorm = 0;
  if(s_uPostWater10[RI_BRADY] == ABOVE_NORM)
    ++uNumBradyAboveNorm;
  if(s_uPostWater20[RI_BRADY] == ABOVE_NORM)
    ++uNumBradyAboveNorm;
  if(s_uPostWater30[RI_BRADY] == ABOVE_NORM)
    ++uNumBradyAboveNorm;
  uNum3cpmAboveNorm = 0;
  if(s_uPostWater10[RI_3CPM] == ABOVE_NORM)
    ++uNum3cpmAboveNorm;
  if(s_uPostWater20[RI_3CPM] == ABOVE_NORM)
    ++uNum3cpmAboveNorm;
  if(s_uPostWater30[RI_3CPM] == ABOVE_NORM)
    ++uNum3cpmAboveNorm;
  uNumDuodAboveNorm = 0;
  if(s_uPostWater10[RI_DUOD] == ABOVE_NORM)
    ++uNumDuodAboveNorm;
  if(s_uPostWater20[RI_DUOD] == ABOVE_NORM)
    ++uNumDuodAboveNorm;
  if(s_uPostWater30[RI_DUOD] == ABOVE_NORM)
    ++uNumDuodAboveNorm;

  if(uNumDuodAboveNorm == 0)
    { // First look for normal conditions:
    if(uNumBradyAboveNorm == 0 && uNumTachyAboveNorm == 0)
      {
      if(uNum3cpmAboveNorm >= 2)
        iRet = DIAG_NORMAL;
      else if(uNum3cpmAboveNorm == 1)
        iRet = sub-normal
      }
    if(iRet == DIAG_NONE)
      {
      if(uNumBradyAboveNorm == 0 && uNumTachyAboveNorm > 0)
        { // Look for tachygastria
        if(uNumTachyAboveNorm >= 2)
          {
          if(uNum3cpmAboveNorm < 2)
            iRet = DIAG_TACHY;
          else
            iRet = DIAG_PROB_TACHY;
          }
        else // if(uNumTachyAboveNorm == 1)
          {
          if(uNum3cpmAboveNorm >= 2)
            iRet = DIAG_PROB_TACHY;
          else
            iRet = DIAG_POS_TACHY_CC;
          }
        }
      else if(uNumTachyAboveNorm == 0 && uNumBradyAboveNorm > 0)
        { // Look for bradygastria.
        if(uNumBradyAboveNorm == 3)
          iRet = DIAG_BRADY;
        else if(uNumBradyAboveNorm == 2)
          {
          if(uNum3cpmAboveNorm < 2)
            iRet = DIAG_BRADY;
          else
            iRet = DIAG_POS_BRADY_CC;
          }
        else // uNumBradyAboveNorm == 1
          {
          if(uNum3cpmAboveNorm >= 2)
            iRet = DIAG_POS_BRADY_CC;
          else
            iRet = DIAG_PROB_NORM;
          }
        }
      else if(uNumBradyAboveNorm == 0 && uNumTachyAboveNorm == 0)
        { // All values are in normal range, check for obstruction.
        if(uNum3cpmAboveNorm <= 1)
          iRet = DIAG_NORMAL;
        else if(uNum3cpmAboveNorm == 2)
          {
          iRet = DIAG_PROB_NORM;
/**** Version III
//          if(s_uPostWater20[RI_3CPM] == ABOVE_NORM && s_uPostWater30[RI_3CPM] == ABOVE_NORM)
//            iRet = DIAG_PROB_NORM_GAS_OUT_OBS;
//          else
//            iRet = DIAG_NORMAL;
******/
          }
        else // uNum3cpmAboveNorm == 3
          iRet = DIAG_GAS_OUT_OBS;
        }
      }
    }
  else
    { // At least 1 duodenal point is above normal.
      // Need to look at respiration rate.
    }

  if(iRet == DIAG_NONE)
    iRet = DIAG_MIX_DYS;
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
////  THE FOLLOWING IS NOT USED ANYMORE.

#define WITHIN_10PCT 0x1

short int iGet_diag_range(float fValue, short int iCtrl1, short int iCtrl2);

#define DIAG_BL_NONE  0
#define DIAG_BL_NORM  1
#define DIAG_BL_TACHY 2
#define DIAG_BL_BRADY 3
#define DIAG_BL_DUOD  4

#define DIAG_PP_NONE  0
#define DIAG_PP_NORM  1
#define DIAG_PP_TACHY 2
#define DIAG_PP_BRADY 3
#define DIAG_PP_MXNS  4
#define DIAG_PP_GOO   5
#define DIAG_PP_RPD   6
#define DIAG_PP_UGR   7
/*********************************************
3.10.1.  The following diagnostic categories will be evaluated for the BASELINE Recording Period:
3.10.1.1.  Normal;
3.10.1.1.1.  VALUE for the BRADY range is within the normal range, and
3.10.1.1.2.  VALUE for the NORM range is within the normal range, and
3.10.1.1.3.  VALUE for the TACHY range is within the normal range, and
3.10.1.1.4.  VALUE for the DUOD/RESP range is within the normal range.
3.10.1.1.5.  Suggested diagnostic interpretation shall read, "Baseline EGG is suggestive of Normal gastric rhythm."
3.10.1.2.  Tachygastria;
3.10.1.2.1.  VALUE for the BRADY range is with the normal range, and
3.10.1.2.2.  VALUE for the NORM range is less than the lower limit of the normal range, and
3.10.1.2.3.  VALUE for the TACHY range is greater than the upper limit of the normal range, and
3.10.1.2.4.  VALUE for the DUOD/RESP range is within the normal range.
3.10.1.2.5.  Suggested diagnostic interpretation shall read, "Baseline EGG is suggestive of Tachygastria."
3.10.1.3.  Bradygastria;
3.10.1.3.1.  VALUE for the BRADY range is greater than the upper limit of the normal range, and
3.10.1.3.2.  VALUE for the NORM range is less than the lower limit of the normal range, and
3.10.1.3.3.  VALUE for the TACHY range is within the normal range or is less than the lower limit of the normal range.
3.10.1.3.4.  VALUE for the DUOD/RESP range is within the normal range or is less than the lower limit of the normal range.
3.10.1.3.5.  Suggested diagnostic interpretation shall read, "Baseline EGG is suggestive of Bradygastria."
3.10.1.4.  Duodenal/Respiratory Dysrhythmia.
3.10.1.4.1.  VALUE for the BRADY range is within the normal range.
3.10.1.4.2.  VALUE for the NORM range is less than the lower limit of the normal range.
3.10.1.4.3.  VALUE for the TACHY range is within the normal range.
3.10.1.4.4.  VALUE for the DUOD/RESP range is above the upper limit of the normal range.
3.10.1.4.5.  Suggested diagnostic interpretation shall read, "Baseline EGG is suggestive of Duodenal/Respiratory dysrhythmia."
*********************************************/

/********************************************************************

  Range indicators:
    0 = VALUE is within the normal range.
    1 = VALUE is above the normal range.
    -1 = VALUE is below the normal range.

  inputs: Array of floating point patient values:
            [0] = Brady
            [1] = Normal
            [2] = Tachy
            [3] = Duod/Resp

********************************************************************/
short int iEval_baseline(float *pfValues)
  {
  float fVal;
  short int iBrady, iNorm, iTachy, iDuod, iRet;

    // First determine which values are within the normal range.
  iBrady = iGet_diag_range(0, *pfValues, s_fControl[0][0], s_fControl[1][0]);
  ++pfValues;
  iNorm = iGet_diag_range(0, *pfValues, s_fControl[2][0], s_fControl[3][0]);
  ++pfValues;
  iTachy = iGet_diag_range(0, *pfValues, s_fControl[4][0], s_fControl[5][0]);
  ++pfValues;
  iDuod = iGet_diag_range(0, *pfValues, s_fControl[6][0], s_fControl[7][0]);

    // Check for Normal
    //   VALUE for the BRADY range is within the normal range, and
    //   VALUE for the NORM range is within the normal range, and
    //   VALUE for the TACHY range is within the normal range, and
    //   VALUE for the DUOD/RESP range is within the normal range.
  if(iBrady == 0 && iNorm == 0 && iTachy == 0 && iDuod == 0)
    iRet = DIAG_BL_NORM;

    // Check for Tachygastria
    //   VALUE for the BRADY range is with the normal range, and
    //   VALUE for the NORM range is less than the lower limit of the normal range, and
    //   VALUE for the TACHY range is greater than the upper limit of the normal range, and
    //   VALUE for the DUOD/RESP range is within the normal range.
  else if(iBrady == 0 && iNorm == -1 && iTachy == 1 && iDuod == 0)
    iRet = DIAG_BL_TACHY;

    // Check for Bradygastria
    //   VALUE for the BRADY range is greater than the upper limit of the normal range, and
    //   VALUE for the NORM range is less than the lower limit of the normal range, and
    //   VALUE for the TACHY range is within the normal range 
    //      or is less than the lower limit of the normal range.
    //   VALUE for the DUOD/RESP range is within the normal range
    //      or is less than the lower limit of the normal range.
  else if(iBrady == 1 && iNorm == -1 && (iTachy == 0 || iTachy == -1)
  && (iDuod == 0 || iDuod == -1))
    iRet = DIAG_BL_BRADY;

    // Check for Duodenal/Respiratory Dysrhythmia
    //   VALUE for the BRADY range is within the normal range.
    //   VALUE for the NORM range is less than the lower limit of the normal range.
    //   VALUE for the TACHY range is within the normal range.
    //   VALUE for the DUOD/RESP range is above the upper limit of the normal range.
  else if(iBrady == 0 && iNorm == -1 && iTachy == 0 && iDuod == 1)
    iRet = DIAG_BL_DUOD;

  else
    iRet = DIAG_BL_NONE;
  return(iRet);
  }

/********************************************************************
The following diagnostic categories will be evaluated for the 
Post-water-load (post prandial) Recording Period:

  inputs: Array of floating point patient values:
            [0] - [2] =  Brady (10MIN, 20MIN and 30MIN periods)
            [3] - [5] =  Normal (10MIN, 20MIN and 30MIN periods)
            [6] - [8] =  Tachy (10MIN, 20MIN and 30MIN periods)
            [9] - [11] = Duod/Resp (10MIN, 20MIN and 30MIN periods)

********************************************************************/
short int iEval_post_prandial(float *pfValues)
  {
  short int iRet = DIAG_PP_NONE;
  short int iBrady10, iBrady20, iBrady30;
  short int iNorm10, iNorm20, iNorm30;
  short int iTachy10, iTachy20, iTachy30;
  short int iDuod10, iDuod20, iDuod30;

    // First determine which values are within the normal range.
  iBrady10 = iGet_diag_range(0, *pfValues, s_fControl[0][1], s_fControl[1][1]);
  ++pfValues;
  iBrady20 = iGet_diag_range(0, *pfValues, s_fControl[0][2], s_fControl[1][2]);
  ++pfValues;
  iBrady30 = iGet_diag_range(0, *pfValues, s_fControl[0][3], s_fControl[1][3]);
  ++pfValues;

  iNorm10 = iGet_diag_range(0, *pfValues, s_fControl[2][1], s_fControl[3][1]);
  ++pfValues;
  iNorm20 = iGet_diag_range(0, *pfValues, s_fControl[2][2], s_fControl[3][2]);
  ++pfValues;
  iNorm30 = iGet_diag_range(0, *pfValues, s_fControl[2][3], s_fControl[3][3]);
  ++pfValues;

  iTachy10 = iGet_diag_range(WITHIN_10PCT, *pfValues, s_fControl[4][1], s_fControl[5][1]);
  ++pfValues;
  iTachy20 = iGet_diag_range(WITHIN_10PCT, *pfValues, s_fControl[4][2], s_fControl[5][2]);
  ++pfValues;
  iTachy30 = iGet_diag_range(WITHIN_10PCT *pfValues, s_fControl[4][3], s_fControl[5][3]);
  ++pfValues;

  iDuod10 = iGet_diag_range(0, *pfValues, s_fControl[6][1], s_fControl[7][1]);
  ++pfValues;
  iDuod20 = iGet_diag_range(0, *pfValues, s_fControl[6][2], s_fControl[7][2]);
  ++pfValues;
  iDuod30 = iGet_diag_range(0, *pfValues, s_fControl[6][3], s_fControl[7][3]);
  ++pfValues;

  // Check for Normal
  //   VALUE for each of the BRADY, NORM, TACHY and DUOD/RESP ranges are 
  //      within the normal range of values for each of the 10MIN, 20MIN
  //      and 30MIN periods.
  if(iBrady10 == 0 && iBrady20 == 0 && iBrady30 == 0
  && iNorm10 == 0 && iNorm20 == 0 && iNorm30 == 0
  && iTachy10 == 0 && iTachy20 == 0 && iTachy30 == 0
  && iDuod10 == 0 && iDuod20 == 0 && iDuod30 == 0)
    iRet = DIAG_PP_NORM;

  if(iRet == DIAG_PP_NONE)
    { // Check for Tachygastria
      //   VALUE for the BRADY range is within the normal range or less than the 
      //     lower limit of the normal range for each of the 10MIN, 20MIN and 30MIN periods
      //   AND
      //   VALUE for the NORM range is less than the lower limit of the normal range 
      //     for at least two of the 10MIN, 20MIN and 30MIN periods
      //   AND
      //   VALUE for the TACHY range
      //     is greater than upper limit of the normal range for each of the 
      //        10MIN, 20MIN and 30MIN periods, or
      //     is greater than the upper limit of the normal range for the 
      //        10MIN and 20MIN periods and is within ten percent of the upper limit 
      //         for the 30MIN range, or
      //     is greater than the upper limit of the normal range for the 
      //        10MIN and 20MIN periods and is within the normal range for the 30MIN period, or
      //     is greater than the upper limit of the normal range for the 
      //        10MIN period and is within ten percent of the upper limit 
      //        for both the 20MIN and 30MIN periods, or
      //     is greater than the upper limit of the normal range for the 10MIN period,
      //        is within ten percent of the upper limit for the 20MIN 
      //        and is within the normal range for the 30MIN period, or
      //     is greater than the upper limit of the normal range for the 10MIN period, 
      //        is within ten percent of the upper limit for the 30MIN 
      //        and is within the normal range for the 20MIN period, or
      //     is greater than the upper limit of the normal range for the 10MIN period,
      //        is within the normal range for both the 20MIN and 30MIN periods
      //   AND
      //   VALUE for the DUOD/RESP range is within the normal range 
      //      or less than the lower limit of the normal range for each of the 
      //        10MIN, 20MIN and 30MIN periods.
    if((iBrady10 == 0 && iBrady20 == 0 && iBrady30 == 0)
    || (iBrady10 == -1 && iBrady20 == -1 && iBrady30 == -1)
      iRet = DIAG_PP_TACHY;
    }

  if(iRet == DIAG_PP_NONE)
    { // Check for Bradygastria
      //   VALUE for the BRADY range is over the upper limit of the normal range 
      //     for at least one of the 10MIN, 20MIN or 30MIN periods
      //   AND
      //   VALUE for the NORM range is below the lower limit of the normal range 
      //     for at least one of the 10MIN, 20MIN or 30MIN periods
      //   AND
      //   VALUE for the TACHY range is DON'T CARE for each of the 
      //     10MIN, 20MIN and 30MIN periods
      //   AND
      //   VALUE for the DUOD/RESP range is DON'T CARE for each of the 
      //     10MIN, 20MIN and 30MIN periods.
      iRet = DIAG_PP_BRADY;
    }

  if(iRet == DIAG_PP_NONE)
    { // Check for Mixed/Nonspecific Dysrhythmia
      //   VALUE for the BRADY range is greater than the upper limit of the normal range 
      //     and VALUE for the NORM range is within the normal range 
      //     and VALUE for the TACHY range is less than the lower limit of the normal range 
      //     and VALUE for the DUOD/RESP range is DON'T CARE for at least one 
      //       of the 10MIN, 20MIN or 30MIN periods
      //   OR
      //   VALUE for the BRADY range is greater than the upper limit of the normal range
      //     and VALUE for the NORM range is less than the upper limit of the normal range 
      //     and VALUE for the TACHY range is less than the lower limit of the normal range 
      //     and VALUE for the DUOD/RESP range is DON'T CARE for at least one 
      //       of the 10MIN, 20MIN or 30MIN periods
      //   OR
      //   VALUE for the BRADY range is less than the lower limit of the normal range 
      //     and VALUE for the NORM range is within the normal range 
      //     and VALUE for the TACHY range is greater than the upper limit of the normal range 
      //     and VALUE for the DUOD/RESP range is DON'T CARE for at least one 
      //       of the 10MIN, 20MIN or 30MIN periods
      //   OR
      //   VALUE for the BRADY range is less than the lower limit of the normal range 
      //     and VALUE for the NORM range is less than the upper limit of the normal range 
      //     and VALUE for the TACHY range is greater than the upper limit of the normal range 
      //     and VALUE for the DUOD/RESP range is DON'T CARE for at least one 
      //       of the 10MIN, 20MIN or 30MIN periods
      //   OR
      //   VALUE for the BRADY range is greater than the upper limit of the normal range 
      //       for the 10MIN period 
      //     and VALUE for the TACHY range is less than the lower limit of the normal range 
      //       for the 20MIN period 
      //     and VALUE for the NORM range is DON'T CARE
      //     and VALUE for the DUOD/RESP range is DON'T CARE
      //   OR
      //   VALUE for the BRADY range is less than the lower limit of the normal range 
      //       for the 10MIN period 
      //     and VALUE for the TACHY range is greater than the upper limit of the normal range 
      //       for the 20MIN period 
      //     and VALUE for the NORM range is DON'T CARE and VALUE for 
      //       the DUOD/RESP range is DON'T CARE
      //   OR
      //   VALUE for the BRADY range is greater than the upper limit of the normal range 
      //       for the 10MIN period 
      //     and VALUE for the TACHY range is less than the lower limit of the normal range 
      //       for the 30MIN period 
      //     and VALUE for the NORM range is DON'T CARE 
      //     and VALUE for the DUOD/RESP range is DON'T CARE
      //   OR
      //   VALUE for the BRADY range is less than the lower limit of the normal range 
      //       for the 10MIN period 
      //     and VALUE for the TACHY range is greater than the upper limit of the normal range 
      //     for the 30MIN period 
      //     and VALUE for the NORM range is DON'T CARE 
      //     and VALUE for the DUOD/RESP range is DON'T CARE
      //   OR
      //   VALUE for the BRADY range is greater than the upper limit of the normal range 
      //       for the 20MIN period 
      //     and VALUE for the TACHY range is less than the lower limit of the normal range 
      //     for the 30MIN period 
      //     and VALUE for the NORM range is DON'T CARE 
      //     and VALUE for the DUOD/RESP range is DON'T CARE
      //   OR
      //   VALUE for the BRADY range is less than the lower limit of the normal range 
      //     for the 20MIN period 
      //     and VALUE for the TACHY range is greater than the upper limit of the normal range 
      //       for the 30MIN period 
      //     and VALUE for the NORM range is DON'T CARE 
      //     and VALUE for the DUOD/RESP range is DON'T CARE.
      iRet = DIAG_PP_MXNS;
    }

  if(iRet == DIAG_PP_NONE)
    { // Check for Gastric Outlet Obstruction
      //   VALUE for the BRADY range is DON'T CARE
      //   AND
      //   VALUE for the NORM range is greater than the upper limit of the normal range 
      //     for each of the 10MIN, 20MIN and 30MIN periods
      //   AND
      //   VALUE for the TACHY range is DON'T CARE
      //   AND
      //   VALUE for the DUOD/RESP range is DON'T CARE.
    iRet = DIAG_PP_GOO;
    }

  if(iRet == DIAG_PP_NONE)
    { // Check for Respiration Pattern Dysrhythmia
      //   VALUE for the BRADY range is DON'T CARE
      //   AND
      //   VALUE for the NORM range
      //     is within the normal range for each of the 10MIN, 20MIN and 30MIN periods, or
      //     is less than the lower limit of the normal range for at 
      //       least one of the 10MIN, 20MIN or 30MIN periods
      //   AND
      //   VALUE for the TACHY range is DON'T CARE
      //   AND
      //   VALUE for the DUOD/RESP range is greater than the upper limit 
      //     of the normal range for each of the 10MIN, 20MIN and 30MIN periods.
    iRet = DIAG_PP_RPD;
    }
  if(iRet == DIAG_PP_NONE)
    { // Check for Unspecified Gastric Rhythm
      //   VALUE for the BRADY, NORM, TACHY and DUOD/RESP ranges does not match 
      //     any of the diagnostic categories above
      //   AND
      //   VALUE for the BRADY range is not within the normal range
      //   OR
      //   VALUE for the NORM range is not within the normal range
      //   OR
      //   VALUE for the TACHY range is not within the normal range
      //   OR
      //   VALUE for the DUOD/RESP range is not within the normal range.
    iRet = DIAG_PP_UGR;
    }

  return(iRet);
  }

/********************************************************************

  inputs: Patient value.
          Index into first dimension of control array to indicate the
            CPM range or frequency band. This is the index for the low
            value and incremented by 1 is the index for the high value.
          Index into the second dimension of the control array to indicate
            the 10MIN,20MIN,30MIN high and low values.

  return: 0 = VALUE is within the normal range.
          1 = VALUE is above the normal range.
          -1 = VALUE is below the normal range.
********************************************************************/
short int iGet_diag_range(unsigned short uFlags, float fValue, float fMin, float fMax)
  {
  short int iRet;

  if(fValue >= fMax && fValue <= fMin)
    iRet = 0; // VALUE is within the normal range.
  else
    {
    if(fValue >= fMax)
      {
      if(uFlags == WITHIN_10PCT)
        {
        if(fValue <= (fMax * (float)1.10))
          iRet = 10; // Value is within 10% of the max.
        }
      else
        iRet = 1;  // VALUE is above the normal range.
      }
    else // fValue <= fMin
      {
      if(uFlags == WITHIN_10PCT)
        {
        if(fValue <= (fMax * (float)1.10))
          iRet = -10; // Value is within 10% of the min.
        }
      else
        iRet = -1;  // VALUE is below the normal range.
      }
    }
  return(iRet);
  }
