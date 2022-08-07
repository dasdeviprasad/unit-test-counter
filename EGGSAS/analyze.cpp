/********************************************************************
analyze.cpp

Copyright (C) 2003,2004,2012, 3CPM Company, Inc.  All rights reserved.

  Implements all analysis functions for an EGG raw data file.


HISTORY:

11-OCT-03  RET  New.
14-OCT-08  RET  Version 2.07a
                  Add method vSet_freq_bands().
                  Change Constructor to call vSet_freq_bands().
                  Change bAnalyze_data():
                    - To call vSet_freq_bands().
                    - To skip calculating power summary and average power
                      for band 1 for Dogs.
                  Change vCalc_diagnosis() to skip frequency band 1 for dogs.
                  Change vCalc_datasheet():
                    - To skip frequency band 1 for dogs.
                    - To stop at freq band 4 for dogs.
22-SEP-12  RET
03-OCT-12  RET
             Change for 60/200 CPM
               Change constructor to set the number of frequency bands and the
                 maximum number of frequency points.
               Change bAnalyze_data() to create the data points for the RSA and datasheet
                 by using a 2 level loop rather than in line.
               Change vSet_freq_bands() to set up the frequency bands in arrays for
                 use by bAnalyze_data().
               Add method: vSet_freq_range()
19-MAR-13  RET
             Change bAnalyze_data() to set the high frequency band appropriately
             if reading a study that has a recording frequency of 60 or 200cpm.

********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "driver.h"

#include "analyze.h"

#include "fftw3.h"

#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/********************************************************************
constructor

  Initialize class variables.

  inputs: None.

********************************************************************/
CAnalyze::CAnalyze(CEGGSASDoc *pDoc)
  {
//  CString cstrLowFreqCutOff;

  m_pDoc = pDoc; // Pointer to the document.

  m_fScaleFactor = 1.0F;
  vSet_freq_bands(PAT_HUMAN);
/*********************************
  m_uiFreqBand1LowIndex = H_FREQ_BAND1_LOW_INDEX;
  m_uiFreqBand1HighIndex = H_FREQ_BAND1_HIGH_INDEX;
  m_uiFreqBand2LowIndex = H_FREQ_BAND2_LOW_INDEX;
  m_uiFreqBand2HighIndex = H_FREQ_BAND2_HIGH_INDEX;
  m_uiFreqBand3LowIndex = H_FREQ_BAND3_LOW_INDEX;
  m_uiFreqBand3HighIndex = H_FREQ_BAND3_HIGH_INDEX;
  m_uiFreqBand4LowIndex = H_FREQ_BAND4_LOW_INDEX;
  m_uiFreqBand4HighIndex = H_FREQ_BAND4_HIGH_INDEX;
  m_uiFreqBand5LowIndex = H_FREQ_BAND5_LOW_INDEX;
  m_uiFreqBand5HighIndex = H_FREQ_BAND5_HIGH_INDEX;
  m_uiFreqBand6LowIndex = H_FREQ_BAND6_LOW_INDEX;
  m_uiFreqBand6HighIndex = H_FREQ_BAND6_HIGH_INDEX;
********************************/

  m_cstrErrMsg.Empty();

//    // Get the low freqency cut-off for the high pass filter from the
//    // Registry.
//  cstrLowFreqCutOff = AfxGetApp()->GetProfileString(REG_SEC_PROGRAM, 
//                                             REG_VALNAM_LOW_FREQ_CUT_OFF, "");
  m_fLowFreqCutOff = (float)0.010; //atof(cstrLowFreqCutOff);

  vSet_freq_range(theApp.m_iFreqLimit);
//#ifdef _200_CPM_HARDWARE
//  if(theApp.m_iFreqLimit == FREQ_LIMIT_15_CPM)
//    {
//    m_iNumFreqBands = NUM_FREQ_BANDS; 
//    m_iMaxFreqPoints = (int)MAX_FREQ_POINTS;
//    m_uSummBand1 = FREQ_BAND5;
//    m_uSummBand2 = FREQ_BAND6;
//    }
//  else if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
//    {
//    m_iNumFreqBands = NUM_FREQ_BANDS_60; 
//    m_iMaxFreqPoints = (int)MAX_FREQ_POINTS_60;
//    m_uSummBand1 = FREQ_BAND10;
//    m_uSummBand2 = FREQ_BAND11;
//    }
//  else // if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
//    {
//    m_iNumFreqBands = NUM_FREQ_BANDS_200; 
//    m_iMaxFreqPoints = (int)MAX_FREQ_POINTS_200;
//    m_uSummBand1 = FREQ_BAND19;
//    m_uSummBand2 = FREQ_BAND20;
//    }
//#else
//  m_iNumFreqBands = NUM_FREQ_BANDS; 
//  m_iMaxFreqPoints = (int)MAX_FREQ_POINTS;
//  m_uSummBand1 = FREQ_BAND5;
//  m_uSummBand2 = FREQ_BAND6;
//  //m_uSummBand1 = FREQ_BAND7;
//  //m_uSummBand2 = FREQ_BAND8;
//#endif

	m_bSubResp = FALSE;
	m_iRespChannel = 1;

  }

/********************************************************************
destructor

********************************************************************/
CAnalyze::~CAnalyze()
  {

  }

/********************************************************************
cstrGetErrMsg - Get the error message for the last error to occur.

  inputs: None.

  return: CString object containing the error message.
********************************************************************/
CString CAnalyze::cstrGetErrMsg()
  {

  return(m_cstrErrMsg);
  }

/********************************************************************
bAnalyze_data - Perform the data analysis based on the contents of the input.
                This function performs the analysis on one epoch 
                (4-minute period) at a time.

                This computes the spectrum for a 4-minute epoch of data 
                (1024 points).  The resulting spectra are overlapped by 75%.

                If user selected to have the respiration signal subtracted,
                the raw respiration signal is subtracted from the raw channel
                signal before any calculations are performed.  Then all
                calculations are based on the difference.
                
                Analysis always done:
                  - Correct the raw data for the scaling factor. This is only
                    used for Version I data files. For Version II data files, this
                    is set to 1.0.
                  - Calculate a mean and correct the data for the mean.
                  - Filter the data through a high pass filter using a
                    hamming window.
                  - Perform a complex FFT on the filtered data.

                Analysis for reports only:
                  - Sum the data into arrays according to frequency bands.

                If the allocated data buffer
                   REQ_ANALYZE_DATA.ANALYZED_DATA.pfData
                is NULL or the number of data points is less than the
                number of points in a 4-minute epoch, then no analysis
                is performed and just the raw data buffer is returned.

  inputs: Pointer to a ANALYZED_DATA structure that contains all the information
            needed to perform the analysis.

  return: TRUE if the data was successfully read from the file.
          FALSE if the data couldn't be read from the file.
********************************************************************/
bool CAnalyze::bAnalyze_data(REQ_ANALYZE_DATA *pReq)
  {
  bool bRet = true;
  CString cstrErrMsg, cstrErrTitle;
  unsigned int uiNumPoints, uiIndex, uiDataIndex, uiIndexFreqBand; // uiNumBytes, 
  unsigned int uiCnt, uiTotalPointsRead, uiDestIndex, uiFreqBandHighIndex;
  unsigned int uiFreqBandLowIndex;
  float fFFTData[DATA_POINTS_PER_EPOCH_1 * 2];
  float fMean, fSpecEst, fDomFreq;
  float fRespSpecEst, fFFTRespData[DATA_POINTS_PER_EPOCH_1 * 2];
  float *pfRespData = NULL, *pfEpoch;
  bool bDoRespSub;
  long lDataOffset;

  theApp.vLog("bAnalyze_data(): timeband %d, epoch %f", pReq->uTimeBand, pReq->fEpochNum);

  vSet_freq_bands(m_pDoc->m_pdPatient.uPatientType);
  m_uiFreqMaxBandForAnalysis = theApp.m_iMaxFreqBands;

  if(m_bSubResp == TRUE  // && m_iRespChannel != CHANNEL_NOT_USED_ID
  && m_iRespChannel != pReq->iChannel)
    bDoRespSub = true;
  else
    bDoRespSub = false;

  m_cstrErrMsg.Empty();
    // Get a pointer to the proper epoch.

    // Calculate the offset into the data buffer.
    // If the number of channels is even, then the offset must be even.
  lDataOffset = (long)(pReq->fEpochNum * (float)(DATA_POINTS_PER_MIN_1 * pReq->iNumChannelsInFile));
  theApp.vLog("lDataOffset = %ld", lDataOffset);
  if((pReq->iNumChannelsInFile % 2) == 0)
    {
    if((lDataOffset % 2) != 0)
      --lDataOffset; // Data offset is odd, EGG points start on even offsets.
    }
  pfEpoch = m_pDoc->m_fData + lDataOffset;
//            (long)(pReq->fEpochNum * (float)(DATA_POINTS_PER_MIN_1 * pReq->iNumChannelsInFile));

  uiTotalPointsRead = DATA_POINTS_PER_EPOCH_1 * MAX_CHANNELS; 
  uiNumPoints = DATA_POINTS_PER_EPOCH_1;

  pReq->padData->iNumDataPoints = uiNumPoints;
  if(pReq->padData->pfRawData != NULL) //pReq->iType == EPOCH_SPECTRA_TYPE && )
    {
      // Put the channel we are interested in in an array by itself.
    vCopy_data_points(pReq->padData->pfRawData, pfEpoch, uiNumPoints,
                      pReq->iChannel, pReq->iNumChannelsInFile);
    if(bDoRespSub == true)
      { // The respiration channel is already in the data array.
        // Step through the raw data array and subtract the respiration
        // from the data. This is only for graphing the raw data.
      for(uiDestIndex = 0, uiIndex = m_iRespChannel; uiIndex < uiTotalPointsRead;
      uiIndex += pReq->iNumChannelsInFile, ++uiDestIndex)
        {
        *(pReq->padData->pfRawData + uiDestIndex) -= *(pfEpoch + uiIndex);
        }
      }
    }
    // Correct for the scale factor. Calculate the mean.
  for(uiIndex = pReq->iChannel, uiDestIndex = 0, fMean = 0.0F;
  uiIndex < uiTotalPointsRead;
  uiIndex += pReq->iNumChannelsInFile, ++uiDestIndex)
    {
    pReq->padData->fData[uiDestIndex] = *(pfEpoch + uiIndex) * m_fScaleFactor;
    fMean += pReq->padData->fData[uiDestIndex];
    }
  fMean /= (float)uiNumPoints; // Calculate the mean.
  for(uiIndex = 0; uiIndex < uiNumPoints; ++uiIndex)
    pReq->padData->fData[uiIndex] -= fMean; // Subtract the mean from the data
    // Filter the data through a hamming window and a high pass filter.
  vHigh_pass_filter(pReq->padData->fData, uiNumPoints);
    // Calculate a complex FFT.
  vCalc_fft(fFFTData, pReq->padData->fData, uiNumPoints);

    //  Now subract the processed respiration signal from the processed EGG signal.
  if(bDoRespSub == true)
    { // We are subtracting the respiration.  Do all calculations including
      // the FFT.
    if((pfRespData = (float *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
    DATA_POINTS_PER_EPOCH_1 * sizeof(float))) != NULL)
      {
      for(uiIndex = m_iRespChannel, uiDestIndex = 0, fMean = 0.0F;
      uiIndex < uiTotalPointsRead;
      uiIndex += pReq->iNumChannelsInFile, ++uiDestIndex)
        {
        *(pfRespData + uiDestIndex) = *(pfEpoch + uiIndex) * m_fScaleFactor;
        fMean += *(pfRespData + uiDestIndex);
        }
      fMean /= (float)uiNumPoints; // Calculate the mean.
      for(uiIndex = 0; uiIndex < uiNumPoints; ++uiIndex)
        *(pfRespData + uiIndex) -= fMean; // Subtract the mean from the data
        // Filter the data through a hamming window and a high pass filter.
      vHigh_pass_filter(pfRespData, uiNumPoints);
        // Calculate a complex FFT.
      vCalc_fft(fFFTRespData, pfRespData, uiNumPoints);
      }
    else
      bDoRespSub = false; // Failed to allocate array for respiration data.
    }

    // calculate the spectral estimates of the data in one 4-minute epoch.
    // The FFT changes the data points from a time domain to a frequency
    // domain.  We are only interested in up to 15 CPM.
  //theApp.vLog("Spectral estimates for epoch %f", pReq->fEpochNum);
  //theApp.vLog("    Pnt#\tSE\t\t\tFFT1\t\t\tFFT2");
  pReq->padData->fDomFreq4MinSE = 0.0f;
  for(uiCnt = 0, uiIndex = 0; uiIndex < (unsigned)m_iMaxFreqPoints; ++uiIndex, uiCnt += 2)
    {
    fSpecEst = (fFFTData[uiCnt] * fFFTData[uiCnt])
                + (fFFTData[uiCnt + 1] * fFFTData[uiCnt + 1]);
    if(bDoRespSub == true)
      { // We are subtracting the respiration.  Subtract the spectral
        // estimates, but the difference should never be negative.
      fRespSpecEst = (fFFTRespData[uiCnt] * fFFTRespData[uiCnt])
                  + (fFFTRespData[uiCnt + 1] * fFFTRespData[uiCnt + 1]);
      fSpecEst -= fRespSpecEst;
        // If negative, don't set it to zero as it won't graph properly.
      if(fSpecEst < 0)
        fSpecEst = (float)0.0001;
      }
    pReq->padData->fData[uiIndex] = fSpecEst;

    //theApp.vLog("    %d:\t%f\t\t%f\t%f", uiIndex, fSpecEst, fFFTData[uiCnt], fFFTData[uiCnt + 1]);

      // Update the dominant frequency.  Make sure its in the range we
      // are working with: 0-15 or 50, 15-50
    if(fSpecEst > pReq->padData->fDomFreq4MinSE)
      {
      pReq->padData->iDomFreq4MinPt = (int)uiIndex;
      pReq->padData->fDomFreq4MinSE = fSpecEst;
      }
    }
  if(pReq->iType == DIAGNOSIS_TYPE || pReq->iType == DATA_SHEET_TYPE)
    { // Sum the powers so that later we can compute the average powers 
      // and power summary
      // Frequency band 1
    if(bDoRespSub == true)
      { // We are subtracting the respiration.
      for(uiIndex = 0; uiIndex < uiNumPoints; ++uiIndex)
        pReq->padData->fData[uiIndex] -= *(pfRespData + uiIndex);
      }
    //uiDataIndex = (pReq->uTimeBand * MAX_FREQ_BANDS) + FREQ_BAND1;
    //if(pReq->padData->pfPwrSum != NULL)
    //  *(pReq->padData->pfPwrSum + uiDataIndex) = 0.0F;

    // Outer loop: Frequency bands
    // Inner loop: power summary, average power
    theApp.vLog("power summary, average power");
    for(uiIndexFreqBand = 0; uiIndexFreqBand < m_uiFreqMaxBandForAnalysis - 2; ++uiIndexFreqBand)
      {
      if(m_uiaFreqBandLowIndex[uiIndexFreqBand] != INV_LONG)
        {
        uiDataIndex = (pReq->uTimeBand * m_uiFreqMaxBandForAnalysis) + uiIndexFreqBand;
        if(pReq->padData->pfPwrSum != NULL)
          *(pReq->padData->pfPwrSum + uiDataIndex) = 0.0F;
        if(uiIndexFreqBand == 0)
          { // Test for human or dog.
          if(m_pDoc->m_pdPatient.uPatientType == PAT_HUMAN)
            { // Human
            for(uiIndex = m_uiaFreqBandLowIndex[uiIndexFreqBand];
              uiIndex <= m_uiaFreqBandHighIndex[uiIndexFreqBand]; ++uiIndex)
              {
              if(pReq->padData->pfPwrSum != NULL)
                *(pReq->padData->pfPwrSum + uiDataIndex) += pReq->padData->fData[uiIndex];
              pReq->padData->fAvgPwr[uiDataIndex] += pReq->padData->fData[uiIndex];
              //theApp.vLog("fData[%d] = %f, uiDataIndex[%d], pfPwrSum = %f, fAvgPwr = %f",
              //  uiIndex, pReq->padData->fData[uiIndex],
              //  uiDataIndex, *(pReq->padData->pfPwrSum + uiDataIndex),
              //  pReq->padData->fAvgPwr[uiDataIndex]);
              }
            }
          else
            { // Dog
              pReq->padData->fAvgPwr[uiDataIndex] = 0.0F;
            }
          }
        else
          {
          for(uiIndex = m_uiaFreqBandLowIndex[uiIndexFreqBand];
            uiIndex <= m_uiaFreqBandHighIndex[uiIndexFreqBand]; ++uiIndex)
            {
            if(pReq->padData->pfPwrSum != NULL)
              *(pReq->padData->pfPwrSum + uiDataIndex) += pReq->padData->fData[uiIndex];
            pReq->padData->fAvgPwr[uiDataIndex] += pReq->padData->fData[uiIndex];
              //theApp.vLog("fData[%d] = %f, uiDataIndex[%d], pfPwrSum = %f, fAvgPwr = %f",
              //  uiIndex, pReq->padData->fData[uiIndex],
              //  uiDataIndex, *(pReq->padData->pfPwrSum + uiDataIndex),
              //  pReq->padData->fAvgPwr[uiDataIndex]);
            }
          }
        }
      }

    //if(m_pDoc->m_pdPatient.uPatientType == PAT_HUMAN)
    //  { // Human
    //  for(uiIndex = m_uiFreqBand1LowIndex; uiIndex <= m_uiFreqBand1HighIndex; ++uiIndex)
    //    {
    //    if(pReq->padData->pfPwrSum != NULL)
    //      *(pReq->padData->pfPwrSum + uiDataIndex) += pReq->padData->fData[uiIndex];
    //    pReq->padData->fAvgPwr[uiDataIndex] += pReq->padData->fData[uiIndex];
    //    }
    //  }
    //else
    //  { // Dog
    //    pReq->padData->fAvgPwr[uiDataIndex] = 0.0F;
    //  }
    //  // Frequency band 2
    //uiDataIndex = (pReq->uTimeBand * MAX_FREQ_BANDS) + FREQ_BAND2;
    //if(pReq->padData->pfPwrSum != NULL)
    //  *(pReq->padData->pfPwrSum + uiDataIndex) = 0.0;
    //for(uiIndex = m_uiFreqBand2LowIndex; uiIndex <= m_uiFreqBand2HighIndex; ++uiIndex)
    //  {
    //  if(pReq->padData->pfPwrSum != NULL)
    //    *(pReq->padData->pfPwrSum + uiDataIndex) += pReq->padData->fData[uiIndex];
    //  pReq->padData->fAvgPwr[uiDataIndex] += pReq->padData->fData[uiIndex];
    //  }
    //  // Frequency band 3
    //uiDataIndex = (pReq->uTimeBand * MAX_FREQ_BANDS) + FREQ_BAND3;
    //if(pReq->padData->pfPwrSum != NULL)
    //  *(pReq->padData->pfPwrSum + uiDataIndex) = 0.0;
    //for(uiIndex = m_uiFreqBand3LowIndex; uiIndex <= m_uiFreqBand3HighIndex; ++uiIndex)
    //  {
    //  if(pReq->padData->pfPwrSum != NULL)
    //    *(pReq->padData->pfPwrSum + uiDataIndex) += pReq->padData->fData[uiIndex];
    //  pReq->padData->fAvgPwr[uiDataIndex] += pReq->padData->fData[uiIndex];
    //  }
    //  // Frequency band 4
    //uiDataIndex = (pReq->uTimeBand * MAX_FREQ_BANDS) + FREQ_BAND4;
    //if(pReq->padData->pfPwrSum != NULL)
    //  *(pReq->padData->pfPwrSum + uiDataIndex) = 0.0;
    //for(uiIndex = m_uiFreqBand4LowIndex; uiIndex <= m_uiFreqBand4HighIndex; ++uiIndex)
    //  {
    //  if(pReq->padData->pfPwrSum != NULL)
    //    *(pReq->padData->pfPwrSum + uiDataIndex) += pReq->padData->fData[uiIndex];
    //  pReq->padData->fAvgPwr[uiDataIndex] += pReq->padData->fData[uiIndex];
    //  }
    if(m_pDoc->m_pdPatient.uPatientType == PAT_HUMAN)
      {
        // Frequency band 7 (15cpm: band 1 thru 4, 50cpm:  1 thru 6, 15-50cpm: 5 thru 6)
      uiDataIndex = (pReq->uTimeBand * theApp.m_iMaxFreqBands) + m_uSummBand1; //FREQ_BAND7;
#ifdef _200_CPM_HARDWARE
      if(theApp.m_iFreqLimit == FREQ_LIMIT_15_CPM)
        uiFreqBandHighIndex = m_uiaFreqBandHighIndex[5]; // 15 cpm //m_uiFreqBand4HighIndex;
      else if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
        uiFreqBandHighIndex = m_uiaFreqBandHighIndex[12]; // 60 cpm
      else if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
        uiFreqBandHighIndex = m_uiaFreqBandHighIndex[19]; // 200 cpm 
#else
        uiFreqBandHighIndex = m_uiaFreqBandHighIndex[5]; // 15 cpm //m_uiFreqBand4HighIndex;
#endif
      //uiFreqBandHighIndex = m_uiaFreqBandHighIndex[12]; // 60cpm //m_uiFreqBand4HighIndex;
      uiFreqBandLowIndex = m_uiaFreqBandLowIndex[0]; //m_uiFreqBand1LowIndex;
    
      if(pReq->padData->pfPwrSum != NULL)
        *(pReq->padData->pfPwrSum + uiDataIndex) = 0.0;
      for(uiIndex = uiFreqBandLowIndex; uiIndex <= uiFreqBandHighIndex; ++uiIndex)
        { 
        //if(m_uiaFreqBandLowIndex[uiIndex] != INV_LONG)
          {
          if(pReq->padData->pfPwrSum != NULL)
            *(pReq->padData->pfPwrSum + uiDataIndex) += pReq->padData->fData[uiIndex];
          pReq->padData->fAvgPwr[uiDataIndex] += pReq->padData->fData[uiIndex];
          }
        }
        // Frequency band 8 (15cpm: band 2 thru 4, 50cpm:  2 thru 6, 15-50cpm: 5 thru 6)
      uiFreqBandLowIndex = m_uiaFreqBandLowIndex[1]; //m_uiFreqBand2LowIndex;
      uiDataIndex = (pReq->uTimeBand * theApp.m_iMaxFreqBands) + m_uSummBand2; //FREQ_BAND8;
      if(pReq->padData->pfPwrSum != NULL)
        *(pReq->padData->pfPwrSum + uiDataIndex) = 0.0;
      for(uiIndex = uiFreqBandLowIndex; uiIndex <= uiFreqBandHighIndex; ++uiIndex)
        { 
        if(m_uiaFreqBandLowIndex[uiIndex] != INV_LONG)
          {
          pReq->padData->fAvgPwr[uiDataIndex] += pReq->padData->fData[uiIndex];
          }
        }
      }
    if(pReq->iType == DATA_SHEET_TYPE)
      { // Data sheet report only.
        // Dominant frequency for the 4-minute epoch.
      fDomFreq = fArray_index_to_freq(pReq->padData->iDomFreq4MinPt);
      if(pReq->padData->pfDomFreq != NULL)
        *(pReq->padData->pfDomFreq + pReq->uRunningEpochCnt) = fDomFreq;
        // Record which frequency band the dominant frequency
        // is in.
      if(pReq->padData->pfPctDomFreq != NULL)
        {
        uiDataIndex = pReq->uTimeBand * theApp.m_iMaxFreqBands;
        if(m_pDoc->m_pdPatient.uPatientType == PAT_HUMAN)
          { // Human
          if(fDomFreq < H_FREQ_BAND2_LOW_CPM) // m_pApp->m_iCpm != CPM_15_50 && 
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND1));
          else if(fDomFreq < H_FREQ_BAND3_LOW_CPM) // m_pApp->m_iCpm != CPM_15_50 && 
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND2));
          else if(fDomFreq < H_FREQ_BAND4_LOW_CPM) // m_pApp->m_iCpm != CPM_15_50 && 
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND3));
#ifndef _200_CPM_HARDWARE
          else
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND4));
#else
          else if(fDomFreq < H_FREQ_BAND5_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND4));
          ////else if(fDomFreq < H_FREQ_BAND6_LOW_CPM)
          ////  ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND5));
          ////else if(fDomFreq < H_FREQ_BAND7_LOW_CPM)
          ////  ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND6));
          else if(fDomFreq < H_FREQ_BAND6_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND5));
          else if(fDomFreq < H_FREQ_BAND7_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND6));
          else if(fDomFreq < H_FREQ_BAND8_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND7));
          else if(fDomFreq < H_FREQ_BAND9_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND8));
          else if(fDomFreq < H_FREQ_BAND10_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND9));
          else if(fDomFreq < H_FREQ_BAND11_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND10));
          else if(fDomFreq < H_FREQ_BAND12_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND11));
          else if(fDomFreq < H_FREQ_BAND13_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND12));
          else if(fDomFreq < H_FREQ_BAND14_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND13));
          else if(fDomFreq < H_FREQ_BAND15_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND14));
          else if(fDomFreq < H_FREQ_BAND16_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND15));
          else
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND16));
#endif
          }
        else
          { // Dog
          if(fDomFreq < D_FREQ_BAND2_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND1));
          else if(fDomFreq < D_FREQ_BAND3_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND2));
          else if(fDomFreq < D_FREQ_BAND4_LOW_CPM)
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND3));
          else
            ++(*(pReq->padData->pfPctDomFreq + uiDataIndex + FREQ_BAND4));
          }
        }
      }
    }
  if(pfRespData != NULL)
    HeapFree(GetProcessHeap(), 0, (LPVOID)pfRespData);

  return(bRet);
  }

/********************************************************************
vHigh_pass_filter - Create and apply a hamming window.  Create and apply
                    a high pass filter.

                    NOTE:  These functions are not well understood as they
                           are copied from the SPEC9 program and the associated
                           files obtained from Michael Vasey.  The ComputerBoards
                           filter and hamming functions were tried, but there was
                           not good coorelation with the SPEC9 data.

  inputs: Pointer to a buffer of data to run through the filter.
          Number of points in the buffer.

  return: None.
********************************************************************/
void CAnalyze::vHigh_pass_filter(float *pfData, unsigned int uiNumPoints)
  {
  float *pfHamWin, fTwoPi, fPi, fWcp, fCs;
  float fA[HPF_COEFF + 1], fB[HPF_COEFF + 1], fC[HPF_COEFF + 1];
  float fF[10][10], fTemp;
  int i, j;
  unsigned int uiIndex, uiNumCoeff;
 
    // First allocate a data buffer for the hamming window. 
  if((pfHamWin = (float *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
  uiNumPoints * sizeof(float))) != NULL)
    { // Generate the hamming window.
    fPi = (float)3.1415926536;
    fTwoPi = (float)2 * (float)3.1415926536; // (float)8.0 * (float)atan((double)1.0);
    for(uiIndex = 0; uiIndex < uiNumPoints; ++uiIndex)
      *(pfHamWin + uiIndex) = (float)0.54 - (float)0.46
                              * (float)cos((double)(fTwoPi * (float)uiIndex 
                              / (float)(uiNumPoints - 1)));
      // Generate the high pass filter.
    uiNumCoeff = HPF_COEFF;
    fWcp = (float)sin(m_fLowFreqCutOff * fPi * (float)HPF_TIME) 
           / (float)cos(m_fLowFreqCutOff * fPi * (float)HPF_TIME);
    for(uiIndex = 1; uiIndex <= uiNumCoeff; ++uiIndex)
      {
      fCs = (float)cos((float)(2 * (uiIndex + uiNumCoeff) - 1) * fPi / (float)(4 * uiNumCoeff));
      fA[uiIndex] = (float)1 / ((float)1 + fWcp * fWcp - (float)2 * fWcp * fCs);
      fB[uiIndex] = (float)2 * (fWcp * fWcp - (float)1) * fA[uiIndex];
      fC[uiIndex] = ((float)1 + fWcp * fWcp + (float)2 * fWcp * fCs) * fA[uiIndex];
      }
      // Apply the hamming window.
    for(uiIndex = 0; uiIndex < uiNumPoints; ++uiIndex)
      *(pfData + uiIndex) *= *(pfHamWin + uiIndex);

      // Apply the filter.
    for(i = 1; i <= 4; ++i)
      {
      for(j = 1; j <= 3; ++j)
        fF[i][j] = 0;
      }
    for(uiIndex = 0; uiIndex < uiNumPoints; ++uiIndex) 
      {
      fF[1][3] = *(pfData + uiIndex);
      for(j = 1; j <= 3; ++j)
        {
        fTemp = fA[j]*(fF[j][3]-2*fF[j][2]+fF[j][1]);
        fF[j+1][3] = fTemp-fB[j]*fF[j+1][2]-fC[j]*fF[j+1][1];
        }
      for(i = 1;  i <= 4; ++i)
        {
        for(j = 1; j <= 2; ++j)
          fF[i][j] = fF[i][j+1];
        }
      *(pfData + uiIndex) = fF[4][3];
      }
    HeapFree(GetProcessHeap(), 0, (LPVOID)pfHamWin);
    }
  }

/********************************************************************
fArray_index_to_freq - Convert the index into the frequency array into
                       a frequency.

  inputs: Array index.

  return: Frequency at the index.
********************************************************************/
float CAnalyze::fArray_index_to_freq(int iArrayIndex)
  {
  return((float)iArrayIndex / (float)4.0);
  }

/********************************************************************
vCopy_data_points - Copy the data points for a channel from a buffer
                    containing data points for all channels to a buffer
                    that will contain data points for only the
                    specified channel.

  inputs: Pointer to the destination buffer that will contain data points
            for only the specified channel.
          Pointer to the souce buffer containing data points for all channels.
          Number of data points to copy.
          The channel whose data points get copied.
          Number of channels in the source buffer.

  return: None.
********************************************************************/
void CAnalyze::vCopy_data_points(float *pfDest, float *pfSrc,
int iPointsToCopy, int iChannel, int iNumChannels)
  {
  int iNumPoints;

  pfSrc += (iChannel - 1);
  for(iNumPoints = 0; iNumPoints < iPointsToCopy; ++iNumPoints)
    {
    *pfDest = *pfSrc;
    ++pfDest;
    pfSrc += iNumChannels;
    }
  }

/********************************************************************
vSet_resp_channel - Set the respiration channel number.  This method is
                    called by the ANALYZE_EGG dialog box.

  inputs: Channel number (1 based) of the respiration channel.

  return: None.
********************************************************************/
void CAnalyze::vSet_resp_channel(int iRespChannel)
  {
  m_iRespChannel = iRespChannel;
  }

/********************************************************************
vSet_sub_resp - Indicates to this object whether the channel data should
                have the respiration subtracted from it.

  inputs: TRUE - Subtract respiration data, FALSE - Don't subtract.

  return: None.
********************************************************************/
void CAnalyze::vSet_sub_resp(BOOL bSubResp)
  {
  m_bSubResp = bSubResp;
  }

/********************************************************************
uCalc_freq_points - Calculate the number of data points between the
                    2 specified frequencies.

  inputs: Low CPM.
          High CPM.

  return: Number of data points..
********************************************************************/
unsigned CAnalyze::uCalc_freq_points(unsigned uLowFreq, unsigned uHighFreq)
  {

  return(((uHighFreq - uLowFreq) * 4) + 1);
  }

/********************************************************************
vCalc_fft - Calculate the FFT.
              For complex data, each data point consists of two
              floating point numbers.  The first number is the real portion.
              The second number is the imaginary part.

  inputs: Pointer to the data where the results are stored.
          Pointer to the data from which the FFT is calculated.
          Number of data values.

  return: None.
********************************************************************/
void CAnalyze::vCalc_fft(float *pfDest, float *pfSrc, unsigned uNumPoints)
  {
  unsigned short uCnt;

//  fftw_complex *pfFFTWData;
//  fftw_complex *pfFFTWDataOut;
  fftw_complex fFFTWData[DATA_POINTS_PER_EPOCH_1 * sizeof(fftwf_complex)];
  fftw_complex fFFTWDataOut[DATA_POINTS_PER_EPOCH_1 * sizeof(fftwf_complex)];
  fftw_plan p;

//  pfFFTWData = (fftw_complex *)fftw_malloc(DATA_POINTS_PER_EPOCH_1 * sizeof(fftw_complex));
//  pfFFTWDataOut = (fftw_complex *)fftw_malloc(DATA_POINTS_PER_EPOCH_1 * sizeof(fftw_complex));

  for(uCnt = 0; uCnt < uNumPoints; ++uCnt)
    {
    fFFTWData[uCnt][0] = *(pfSrc + uCnt);
    fFFTWData[uCnt][1] = 0.0F;
//    pfFFTWData[uCnt][0] = *(pfSrc + uCnt);
//    pfFFTWData[uCnt][1] = 0.0F;
    }
  p = fftw_plan_dft_1d(DATA_POINTS_PER_EPOCH_1, fFFTWData, fFFTWDataOut,
                       FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(p);

  fftw_destroy_plan(p);

    // Put the FFT data back in a floating point real/imaginary array.
  for(uCnt = 0; uCnt < uNumPoints; ++uCnt)
    {
    *pfDest = (float)fFFTWDataOut[uCnt][0];
    ++pfDest;
    *pfDest = (float)fFFTWDataOut[uCnt][1];
    ++pfDest;
    }
//  fftw_free(pfFFTWData);
//  fftw_free(pfFFTWDataOut);
  }

/********************************************************************
vCalc_diagnosis - Perform calculations needed to make a suggested
                  diagnosis.

  inputs: Pointer to a ANALYZED_DATA structure that contains all the
            information needed to perform the calculations.

  return: None.
********************************************************************/
void CAnalyze::vCalc_diagnosis(REQ_ANALYZE_DATA *pReq)
  {
  unsigned short uDataIndex, uIndex, uStartIndex;
  float fDivisor;

  uDataIndex = pReq->uTimeBand * theApp.m_iMaxFreqBands; //MAX_FREQ_BANDS_DIAG;
  if(m_pDoc->m_pdPatient.uPatientType == PAT_HUMAN)
    uStartIndex = FREQ_BAND1; // Human
  else
    uStartIndex = FREQ_BAND2; // Dog
    // Calculate average power
  //for(uIndex = uStartIndex; uIndex < MAX_FREQ_BANDS_DIAG; ++uIndex)
  for(uIndex = uStartIndex; uIndex < theApp.m_iMaxFreqBands; ++uIndex)
    pReq->padData->fAvgPwr[uDataIndex + uIndex] /= (float)pReq->uRunningEpochCnt;
    // Calculate percentage in group 1.
  //for(uIndex = uStartIndex; uIndex <= FREQ_BAND7; ++uIndex)
  for(uIndex = uStartIndex; uIndex <= m_uSummBand1; ++uIndex)
    {
    //if(pReq->padData->fAvgPwr[uDataIndex + FREQ_BAND7] <= 0.0F)
    if(pReq->padData->fAvgPwr[uDataIndex + m_uSummBand1] <= 0.0F)
      fDivisor = 1.0F;
    else
      fDivisor = pReq->padData->fAvgPwr[uDataIndex + m_uSummBand1]; //FREQ_BAND7];
    pReq->padData->fPctGrp1[uDataIndex + uIndex] = 
              (pReq->padData->fAvgPwr[uDataIndex + uIndex] * 100.0F) / fDivisor;
    }
  }

/********************************************************************
vCalc_datasheet - Perform calculations needed for the datasheet report.

                  IMPORTANT
                    This requires vCalc_diagnosis() first calculate
                    the average power.

  inputs: Pointer to a ANALYZED_DATA structure that contains all the
            information needed to perform the calculations.

  return: None.
********************************************************************/
void CAnalyze::vCalc_datasheet(REQ_ANALYZE_DATA *pReq)
  {
  unsigned short uDataIndex, uIndex, uMaxIndex, uStartIndex;
  float fDivisor;


  uDataIndex = pReq->uTimeBand * theApp.m_iMaxFreqBands;
    // Calculate percentage in group 2.
  ////if(m_pDoc->m_pdPatient.uPatientType == PAT_HUMAN)
  ////  uMaxIndex = FREQ_BAND6; // Human
  ////else
    uMaxIndex = FREQ_BAND4; // Dog
  for(uIndex = FREQ_BAND2; uIndex <= uMaxIndex; ++uIndex)
    {
    //if(pReq->padData->fAvgPwr[uDataIndex + FREQ_BAND8] <= 0.0F)
    if(pReq->padData->fAvgPwr[uDataIndex + m_uSummBand2] <= 0.0F)
      fDivisor = 1.0F;
    else
      fDivisor = pReq->padData->fAvgPwr[uDataIndex + m_uSummBand2]; //FREQ_BAND8];
    pReq->padData->fPctGrp2[uDataIndex + uIndex] = 
              (pReq->padData->fAvgPwr[uDataIndex + uIndex] * 100.0F) / fDivisor;
    }

  if(m_pDoc->m_pdPatient.uPatientType == PAT_HUMAN)
    uStartIndex = FREQ_BAND1; // Human
  else
    uStartIndex = FREQ_BAND2; // Dog
  for(uIndex = uStartIndex; uIndex < theApp.m_iMaxFreqBands; ++uIndex)
    { // Calculate ratios only for POSTprandial timebands.
    if(pReq->uTimeBand != 0)
      {
      if(pReq->padData->fAvgPwr[uIndex] <= 0.0F)
        fDivisor = 1.0F; // No PREprandial time band or its average power is 0.
      else
        fDivisor = pReq->padData->fAvgPwr[uIndex];
      *(pReq->padData->pfRatioAvgPwr + uDataIndex + uIndex) = 
                     pReq->padData->fAvgPwr[uDataIndex + uIndex] / fDivisor;
      }
    }
  }

/********************************************************************
iCalc_respiration_rate - Calculation the respiration rate.

  Find the respiration rate between the selected minutes for the 
  specified period.

  Algorithm:
    Calculate the mean of the points in the selected minutes.
    Determine number of mean crossings.
    There are 2 mean crossing for each breath.

    Respiration rate = Number of mean crossings / 2 / number of minutes.

  inputs: Pointer to a ANALYZED_DATA structure that contains all the
            information needed to perform the calculations.

  return: Number of breaths per minute.
          -1 if error reading file.
********************************************************************/
short int CAnalyze::iCalc_respiration_rate(unsigned short uPeriod,
float fStart, float fEnd)
  {
  short int iRespRate, iMeanCrossing;
  unsigned uNumPoints, uIndex;
  float *pfData, *pfDataStart, fMean;
  bool bLookAbove;

  if(m_pDoc->iRead_period(uPeriod, m_pDoc->m_fData) == SUCCESS)
    {
      // Get a pointer to the proper epoch.
    pfDataStart = m_pDoc->m_fData + 
            ((long)fStart * (long)(DATA_POINTS_PER_MIN_1 * MAX_CHANNELS));
    uNumPoints = (unsigned)(fEnd - fStart) * DATA_POINTS_PER_MIN_1  * MAX_CHANNELS;
    fMean = 0.0F;
    pfData = pfDataStart + CHANNEL_RESP;
    for(uIndex = CHANNEL_RESP; uIndex < uNumPoints; uIndex += MAX_CHANNELS, pfData += MAX_CHANNELS)
      fMean += *pfData;
    fMean /= (float)(uNumPoints / MAX_CHANNELS); // Calculate the mean.
      // Now look for mean crossings.
    pfData = pfDataStart + CHANNEL_RESP;
    if(*pfData < fMean)
      bLookAbove = true;
    else
      bLookAbove = false;
    pfData += MAX_CHANNELS;
    iMeanCrossing = 0;
    for(uIndex = CHANNEL_RESP; uIndex < uNumPoints; uIndex += MAX_CHANNELS, pfData += MAX_CHANNELS)
      {
      if(bLookAbove == true)
        {
        if(*pfData > fMean)
          {
          ++iMeanCrossing;
          bLookAbove = false;
          }
        }
      else // bLookAbove == false
        {
        if(*pfData < fMean)
          {
          ++iMeanCrossing;
          bLookAbove = true;
          }
        }
      }
    }
  iRespRate = (short int)uFround((float)iMeanCrossing / (float)2.0 / (fEnd - fStart));
  return(iRespRate);
  }

/********************************************************************
fFind_mean_signal - Calculate the mean of the EGG signal between the
                    specified start and end minutes.

  inputs: Pointer to the raw data for a period.
          Start minute for the mean calculation.
          End minute for the mean calculation.

  return: Mean.
********************************************************************/
float CAnalyze::fFind_mean_signal(float *pfData, float fMinStart, float fMinEnd)
  {
  int iIndex, iDataPts;
  float fMean;

    // Convert minutes to data samples.
  fMinStart *= (DATA_POINTS_PER_MIN_1 * 2);
  fMinEnd *= (DATA_POINTS_PER_MIN_1 * 2);
    // Now look through the data for the max signal.
  pfData += (unsigned)fMinStart;
  fMean = 0.0F;
  iDataPts = 0;
  for(iIndex = (int)fMinStart; iIndex < (int)fMinEnd; iIndex += 2)
    {
    fMean += *pfData;
    pfData += 2;
    ++iDataPts;
    }
  fMean /= (float)iDataPts;
  return(fMean);
  }

/********************************************************************
vSet_freq_bands - Set the indexes into the data array for the frequency bands.  
                  Used by the parent.

  inputs: Patient type.

  return: None.
********************************************************************/
void CAnalyze::vSet_freq_bands(unsigned int uiType)
  {

  if(uiType == PAT_HUMAN)
    { // Human

    //// Put in an array and then can be used easily with 200 CPM hardware.
    m_uiaFreqBandLowIndex[0] = H_FREQ_BAND1_LOW_INDEX;
    m_uiaFreqBandHighIndex[0] = H_FREQ_BAND1_HIGH_INDEX;
    m_uiaFreqBandLowIndex[1] = H_FREQ_BAND2_LOW_INDEX;
    m_uiaFreqBandHighIndex[1] = H_FREQ_BAND2_HIGH_INDEX;
    m_uiaFreqBandLowIndex[2] = H_FREQ_BAND3_LOW_INDEX;
    m_uiaFreqBandHighIndex[2] = H_FREQ_BAND3_HIGH_INDEX;
    m_uiaFreqBandLowIndex[3] = H_FREQ_BAND4_LOW_INDEX;
    m_uiaFreqBandHighIndex[3] = H_FREQ_BAND4_HIGH_INDEX;
    m_uiaFreqBandLowIndex[4] =  H_FREQ_BAND5_LOW_INDEX;
    m_uiaFreqBandHighIndex[4] = H_FREQ_BAND5_HIGH_INDEX;
    m_uiaFreqBandLowIndex[5] =   H_FREQ_BAND6_LOW_INDEX;
    m_uiaFreqBandHighIndex[5] =  H_FREQ_BAND6_HIGH_INDEX;
#ifdef _200_CPM_HARDWARE
    m_uiaFreqBandLowIndex[6] =  H_FREQ_BAND7_LOW_INDEX;
    m_uiaFreqBandHighIndex[6] = H_FREQ_BAND7_HIGH_INDEX;
    m_uiaFreqBandLowIndex[7] =  H_FREQ_BAND8_LOW_INDEX;
    m_uiaFreqBandHighIndex[7] = H_FREQ_BAND8_HIGH_INDEX;
    m_uiaFreqBandLowIndex[8] =  H_FREQ_BAND9_LOW_INDEX;
    m_uiaFreqBandHighIndex[8] = H_FREQ_BAND9_HIGH_INDEX;
    m_uiaFreqBandLowIndex[9] =  H_FREQ_BAND10_LOW_INDEX;
    m_uiaFreqBandHighIndex[9] = H_FREQ_BAND10_HIGH_INDEX;
    m_uiaFreqBandLowIndex[10] =  H_FREQ_BAND11_LOW_INDEX;
    m_uiaFreqBandHighIndex[10] = H_FREQ_BAND11_HIGH_INDEX;
    m_uiaFreqBandLowIndex[11] =  H_FREQ_BAND12_LOW_INDEX;
    m_uiaFreqBandHighIndex[11] = H_FREQ_BAND12_HIGH_INDEX;
    m_uiaFreqBandLowIndex[12] =  H_FREQ_BAND13_LOW_INDEX;
    m_uiaFreqBandHighIndex[12] = H_FREQ_BAND13_HIGH_INDEX;
    m_uiaFreqBandLowIndex[13] =  H_FREQ_BAND14_LOW_INDEX;
    m_uiaFreqBandHighIndex[13] = H_FREQ_BAND14_HIGH_INDEX;
    m_uiaFreqBandLowIndex[14] =  H_FREQ_BAND15_LOW_INDEX;
    m_uiaFreqBandHighIndex[14] = H_FREQ_BAND15_HIGH_INDEX;
    m_uiaFreqBandLowIndex[15] =  H_FREQ_BAND16_LOW_INDEX;
    m_uiaFreqBandHighIndex[15] = H_FREQ_BAND16_HIGH_INDEX;
    m_uiaFreqBandLowIndex[16] =  H_FREQ_BAND17_LOW_INDEX;
    m_uiaFreqBandHighIndex[16] = H_FREQ_BAND17_HIGH_INDEX;
    m_uiaFreqBandLowIndex[17] =  H_FREQ_BAND18_LOW_INDEX;
    m_uiaFreqBandHighIndex[17] = H_FREQ_BAND18_HIGH_INDEX;

    m_uiaFreqBandLowIndex[18] =  H_FREQ_BAND17_LOW_INDEX;
    m_uiaFreqBandHighIndex[18] = H_FREQ_BAND17_HIGH_INDEX;
    m_uiaFreqBandLowIndex[19] =  H_FREQ_BAND18_LOW_INDEX;
    m_uiaFreqBandHighIndex[19] = H_FREQ_BAND18_HIGH_INDEX;

#endif


    //m_uiFreqBand1LowIndex = H_FREQ_BAND1_LOW_INDEX;
    //m_uiFreqBand1HighIndex = H_FREQ_BAND1_HIGH_INDEX;
    //m_uiFreqBand2LowIndex = H_FREQ_BAND2_LOW_INDEX;
    //m_uiFreqBand2HighIndex = H_FREQ_BAND2_HIGH_INDEX;
    //m_uiFreqBand3LowIndex = H_FREQ_BAND3_LOW_INDEX;
    //m_uiFreqBand3HighIndex = H_FREQ_BAND3_HIGH_INDEX;
    //m_uiFreqBand4LowIndex = H_FREQ_BAND4_LOW_INDEX;
    //m_uiFreqBand4HighIndex = H_FREQ_BAND4_HIGH_INDEX;
    //// Not used
    //m_uiFreqBand5LowIndex = H_FREQ_BAND5_LOW_INDEX;
    //m_uiFreqBand5HighIndex = H_FREQ_BAND5_HIGH_INDEX;
    //m_uiFreqBand6LowIndex = H_FREQ_BAND6_LOW_INDEX;
    //m_uiFreqBand6HighIndex = H_FREQ_BAND6_HIGH_INDEX;
    }
  else
    { // Dog
    m_uiFreqBand1LowIndex = D_FREQ_BAND1_LOW_INDEX;
    m_uiFreqBand1HighIndex = D_FREQ_BAND1_HIGH_INDEX;
    m_uiFreqBand2LowIndex = D_FREQ_BAND2_LOW_INDEX;
    m_uiFreqBand2HighIndex = D_FREQ_BAND2_HIGH_INDEX;
    m_uiFreqBand3LowIndex = D_FREQ_BAND3_LOW_INDEX;
    m_uiFreqBand3HighIndex = D_FREQ_BAND3_HIGH_INDEX;
    m_uiFreqBand4LowIndex = D_FREQ_BAND4_LOW_INDEX;
    m_uiFreqBand4HighIndex = D_FREQ_BAND4_HIGH_INDEX;
    }
  }

/********************************************************************
vSet_freq_range - Set the frequency range and other variables affecting
                  the analysis that are based on the frequency range.

  inputs: Frequency range from the eFrequencyRange in EGGSAS.h.

  return: None.
********************************************************************/
void CAnalyze::vSet_freq_range(int iFreqRange)
  {

  theApp.m_iFreqLimit = iFreqRange;
#ifdef _200_CPM_HARDWARE
  if(theApp.m_iFreqLimit == FREQ_LIMIT_15_CPM)
    {
    m_iNumFreqBands = NUM_FREQ_BANDS; 
    m_iMaxFreqPoints = (int)MAX_FREQ_POINTS;
    m_uSummBand1 = FREQ_BAND5;
    m_uSummBand2 = FREQ_BAND6;
    }
  else if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
    {
    m_iNumFreqBands = NUM_FREQ_BANDS_60; 
    m_iMaxFreqPoints = (int)MAX_FREQ_POINTS_60;
    m_uSummBand1 = FREQ_BAND10;
    m_uSummBand2 = FREQ_BAND11;
    }
  else // if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
    {
    m_iNumFreqBands = NUM_FREQ_BANDS_200; 
    m_iMaxFreqPoints = (int)MAX_FREQ_POINTS_200;
    m_uSummBand1 = FREQ_BAND19;
    m_uSummBand2 = FREQ_BAND20;
    }
#else
  m_iNumFreqBands = NUM_FREQ_BANDS; 
  m_iMaxFreqPoints = (int)MAX_FREQ_POINTS;
  m_uSummBand1 = FREQ_BAND5;
  m_uSummBand2 = FREQ_BAND6;
  //m_uSummBand1 = FREQ_BAND7;
  //m_uSummBand2 = FREQ_BAND8;
#endif
  }


