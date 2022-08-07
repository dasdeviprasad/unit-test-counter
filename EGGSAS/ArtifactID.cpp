/********************************************************************
ArtifactID.cpp

Copyright (C) 2003,2004, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CArtifactID class.

  Artifact Idendification
    - Identifies artifact both while recording and from the data file
    - Uses the CArtifact class to maintain a list of artifacts.  There
      is a separate list for the baseline and each 10 minute period.


HISTORY:
03-OCT-03  RET  New.
********************************************************************/

#include "stdafx.h"
#include "float.h"

#include "EGGSAS.h"
#include "ArtifactID.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////
// CArtifact - This class holds one artifact.

/********************************************************************
CArtifact Constructor
********************************************************************/
CArtifact::CArtifact()
  {
  }

/********************************************************************
CArtifact Destructor
********************************************************************/
CArtifact::~CArtifact()
  {
  }

/////////////////////////////////////////////////////////////////////
// CArtifactID - This class holds an array of CArtifact type for each period.

/********************************************************************
constructor

  Initialize class variables.
    Read the artifact window around the baseline used for analyzing the
    3 ten minute periods for artifact from the INI file.

********************************************************************/
CArtifactID::CArtifactID()
  {

    // Get the artifact window.
  //m_fArtWindow = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER, INI_ITEM_ARTIFACTWINDOW, 1000);
  m_fArtWindow = g_pConfig->fGet_item(CFG_ARTIFACTWINDOW);
  m_uResetArtLists = 0;
  }

/********************************************************************
destructor

  Empty out each artifact list and destroy the objects.

********************************************************************/
CArtifactID::~CArtifactID()
  {
  int iIndex;

    // Baseline artifacts.
  for(iIndex = 0; iIndex < m_ArtifactsBL.GetSize(); ++iIndex)
    delete m_ArtifactsBL[iIndex];
  m_ArtifactsBL.RemoveAll();

    // First 10 minute period artifacts.
  for(iIndex = 0; iIndex < m_Artifacts10.GetSize(); ++iIndex)
    delete m_Artifacts10[iIndex];
  m_Artifacts10.RemoveAll();

    // Second 10 minute period artifacts.
  for(iIndex = 0; iIndex < m_Artifacts20.GetSize(); ++iIndex)
    delete m_Artifacts20[iIndex];
  m_Artifacts20.RemoveAll();

    // Second 10 minute period artifacts.
  for(iIndex = 0; iIndex < m_Artifacts30.GetSize(); ++iIndex)
    delete m_Artifacts30[iIndex];
  m_Artifacts30.RemoveAll();
  }

/********************************************************************
palGet_art_list - Get a pointer to a the array of artifact minutes for
                  the specified period.

  inputs: Period for which the get the artifact list.

  return: Pointer to the start of the array of artifact for the period.
********************************************************************/
CArtifactList *CArtifactID::palGet_art_list(unsigned uPeriod)
  {
  CArtifactList *palArtLst;

  switch(uPeriod)
    {
    case PERIOD_BL:
      palArtLst = &m_ArtifactsBL;
      break;
    case PERIOD_10:
      palArtLst = &m_Artifacts10;
      break;
    case PERIOD_20:
      palArtLst = &m_Artifacts20;
      break;
    case PERIOD_30:
      palArtLst = &m_Artifacts30;
      break;
    default:
      palArtLst = NULL;
      break;
    }
  return(palArtLst);
  }

/********************************************************************
bIns_artifact - Insert a new artifact into the array of artifact.

  inputs: Period for the array for which the artifact gets inserted into.
          Starting minute of the artifact.
          Ending minute of the artifact.

  return: true if the artifact is successfully inserted.
          false if the artifact is not inserted.
********************************************************************/
bool CArtifactID::bIns_artifact(unsigned uPeriod, unsigned short uStart, unsigned short uEnd)
  {
  CArtifactList *palArtLst;
  CArtifact *pArtifact, *pNewArtifact;
  int iIndex;
  bool bRet;

  if((palArtLst = palGet_art_list(uPeriod)) != NULL)
    { // Find out where to insert it.
    bRet = true;
    for(iIndex = 0; iIndex < palArtLst->GetSize(); ++iIndex)
      {
      pArtifact = (CArtifact *)palArtLst->GetAt(iIndex);
      if(uStart > pArtifact->m_uStartMin)
        { // Went one past where we want to insert.
        if(iIndex > 0)
          {
          --iIndex;
          break;
          }
        }
      }
    pNewArtifact = new CArtifact();
    pNewArtifact->m_uStartMin = uStart;
    pNewArtifact->m_uEndMin = uEnd;
    pNewArtifact->m_uType = ART_OP_SEL; // Operator selected this artifact.
    if(palArtLst->GetSize() == 0 || iIndex == palArtLst->GetSize())
      palArtLst->Add((CArtifact *)pNewArtifact);
    else
      palArtLst->InsertAt(iIndex, (CArtifact *)pNewArtifact);
    }
  else
    bRet = false;
  return(bRet);
  }

/********************************************************************
vDel_artifact - Delete an artifact from the array of artifact.

  inputs: Period for the array for which the artifact gets inserted into.
          Index into the artifact array for the artifact that gets deleted.

  return: None.
********************************************************************/
void CArtifactID::vDel_artifact(unsigned uPeriod, int iIndex)
  {
  CArtifactList *palArtLst;
  CArtifact *pArtifact;

  if((palArtLst = palGet_art_list(uPeriod)) != NULL)
    {
    pArtifact = (CArtifact *)palArtLst->GetAt(iIndex);
      // Remove the CArtifact ptr from the CTypedPtrArray.
    palArtLst->RemoveAt(iIndex);
      // Delete the CArtifact object. (The CTypedPtrArray only holds the ptr.)
    delete pArtifact;
    }
  }

/********************************************************************
vReset_artifact_list - Mark the artifact list to be reset (emptied) for
                       the specified period.

  inputs: Period (0, 10, 20 or 30) for the artifact list that is marked
            to be emptied.

  return: None.
********************************************************************/
void CArtifactID::vReset_artifact_list(unsigned uPeriod)
  {

  m_uResetArtLists |= (0x1 << uPeriod);
  }

/********************************************************************
vUndo_reset_artifact_list - Unmark the artifact list to be reset (emptied)
                            for the specified period.

  inputs: Period (0, 10, 20 or 30) for the artifact list that to be unmarked.

  return: None.
********************************************************************/
void CArtifactID::vUndo_reset_artifact_list(unsigned uPeriod)
  {

  m_uResetArtLists &= ~(0x1 << uPeriod);
  }

/********************************************************************
Artifact_id - Identify artifact.  Only do this if we haven't already done it.

  inputs: Period (0, 10, 20 or 30) which this data is for.
          Pointer to an array of floating point values for which artifact
            is going to be identified and remembered.
          Number of values.

  return: None.
********************************************************************/
void CArtifactID::vArtifact_id(unsigned uPeriod, float *pfData, unsigned uNumVals)
  {
  CArtifactList *palArtLst;
  short int iIndex;

  if((palArtLst = palGet_art_list(uPeriod)) != NULL)
    {
    if((m_uResetArtLists & (0x1 << uPeriod)) != 0)
      { // Reset the artifact list.
      for(iIndex = 0; iIndex < palArtLst->GetSize(); ++iIndex)
       delete palArtLst->GetAt(iIndex);
      palArtLst->RemoveAll();
        // Reset the empty list bit, so we don't do it again.
      m_uResetArtLists &= ~(0x1 << uPeriod);
      }
    if(palArtLst->GetSize() == 0)
      vPeriod_id(uPeriod, pfData, uNumVals, palArtLst);
    }
  }

/********************************************************************
vPeriod_id - Identify artifact for the specified period.

  inputs: Period for the artifact identification.
          Pointer to an array of floating point values for which artifact
            is going to be identified and remembered.
          Number of values.

  return: None.
********************************************************************/
void CArtifactID::vPeriod_id(unsigned uPeriod, float *pfData, unsigned uNumVals,
CArtifactList *pArtLst)
  {
  CArtifact *pArtifact;
  unsigned uCnt;
  unsigned short uNumArtPts, uStart;
  float fLowLimit, fHiLimit;

  if(uPeriod == PERIOD_BL)
    { // Baseline period.  This has different artifact limits from the
      // 30 minute study.
    fLowLimit = m_fBaseLineLowLimit;
    fHiLimit = m_fBaseLineHiLimit;
    }
  else
    { // One of the periods in the 30 minute study.
    fLowLimit = m_fArtLowLimit;
    fHiLimit = m_fArtHiLimit;
    }
  if(fLowLimit != INV_FLOAT && fHiLimit != INV_FLOAT)
    { // There are valid limits, so look for the artifact.
    uNumArtPts = 0;
      // This is a multiplexed array of EGG and Respiration signals.
      // Only want to look at every other data point (EGG).
    for(uCnt = 0; uCnt <= uNumVals; uCnt += 2, pfData += 2)
      {
      if(uCnt < uNumVals && (*pfData  < fLowLimit || *pfData > fHiLimit))
        {
        if(uNumArtPts == 0)
          uStart = uCnt / 2; // Start time.
        ++uNumArtPts;
        }
      else
        {
        if(uNumArtPts != 0)
          { // Record this artifact.
          pArtifact = new CArtifact();
            // Calculate the start on the nearest previous 1/2 minute.
          pArtifact->m_uStartMin = uStart / DATA_POINTS_PER_HALF_MIN;
          if((uStart % DATA_POINTS_PER_HALF_MIN) == 0 && pArtifact->m_uStartMin > 0)
            --pArtifact->m_uStartMin; // Artifact starts on exactly half minute, round down.
            // Calculate the end of the nearest next 1/2 minute.
          if(uCnt == uNumVals)
            pArtifact->m_uEndMin = (uCnt / 2) / DATA_POINTS_PER_HALF_MIN;
          else
              // Round up if its on exactly the half minute.
            pArtifact->m_uEndMin = ((uCnt / 2) + DATA_POINTS_PER_HALF_MIN) / DATA_POINTS_PER_HALF_MIN;
          pArtifact->m_uType = ART_PGM_SEL;
          pArtLst->Add((CArtifact *)pArtifact);
          }
        uNumArtPts = 0;
        }
      }
      // Add the last data point to the artifact list.
      // This is to remember the number of 1/2 minutes in the period.
    pArtifact = new CArtifact();
      // Number of values per minute is double since both EGG and Respiration
      // are in the buffer.
    pArtifact->m_uStartMin = uNumVals / DATA_POINTS_PER_MIN_1;
    pArtifact->m_uEndMin = uNumVals / DATA_POINTS_PER_MIN_1;
    pArtifact->m_uType = ART_NONE;
    pArtLst->Add((CArtifact *)pArtifact);
    }
  }

/********************************************************************
vSet_baseline_limit - Set the limits for the analyzing baseline for
                      artifact.

  inputs: Low limit.
          High limit.

  return: None.
********************************************************************/
void CArtifactID::vSet_baseline_limit(float fLowLimit, float fHiLimit)
  {

  m_fBaseLineLowLimit = fLowLimit;
  m_fBaseLineHiLimit = fHiLimit;
  }

/********************************************************************
vSet_artifact_limit - Set the limits for the analyzing one of the 3 ten
                      minute periods for artifact.

                      The high and low limits get set to a window around
                      the mean of the baseline. The window is specified
                      in the INI file.

  inputs: Mean of the baseline signal.
          High limit.

  return: None.
********************************************************************/
void CArtifactID::vSet_artifact_limit(float fMeanBL)
  {

  if(fMeanBL != INV_FLOAT)
    {
    m_fArtHiLimit = fMeanBL + m_fArtWindow;
    m_fArtLowLimit = fMeanBL - m_fArtWindow;
    }
  else
    {
    m_fArtHiLimit = INV_FLOAT;
    m_fArtLowLimit = INV_FLOAT;
    }
  }

/********************************************************************
bFind_art_free_min - Search the artifact list for the specified period for
                     the longest artifact free period.

  inputs: Period for which to find the artifact free minutes.
          Pointer to a variable that gets the start minute of the artifact
            free period. This is on the half minute.
          Pointer to a variable that gets the end minute of the artifact
            free period. This is on the half minute.

  return: true if a period could be found, even if the period contains
            no minutes.
          false if the period is invalid or there is no artifact list.
********************************************************************/
bool CArtifactID::bFind_art_free_min(unsigned uPeriod, float *pfStart, float *pfEnd)
  {
  CArtifactList *palArtLst;
  CArtifact *pArtifact, *pPrevArtifact;
  unsigned short uCnt, uSize, uArtFreeLen, uEnd, uStart, uPrevEnd;
  bool bRet = false;

  *pfStart = (float)0.0;
  *pfEnd = (float)0.0;
  uStart = 0;
  uEnd = 0;
  if((palArtLst = palGet_art_list(uPeriod)) != NULL)
    {
    if((uSize = palArtLst->GetSize()) == 0)
      bRet = false; // No good minutes.
    else if(uSize == 1)
      { // Only one object in list, this is the terminating object.
        // This indicates there are no artifacts in this period.
      bRet = true;
      pArtifact = (CArtifact *)palArtLst->GetAt(0);
      uEnd = pArtifact->m_uEndMin;
      }
    else
      { // Search for the longest artifact free period.
      pPrevArtifact = NULL;
      uArtFreeLen = 0;
      uPrevEnd = 0; // End of previous artifact.
      for(uCnt = 0; uCnt < uSize; ++uCnt)
        {
        pArtifact = (CArtifact *)palArtLst->GetAt(uCnt);
        uArtFreeLen = pArtifact->m_uStartMin - uEnd;
        if(uArtFreeLen > HALF_MINUTES_PER_EPOCH)
          { // Found at least 4 minutes that are artifact free.
          bRet = true;
          if(uStart == 0 && uPrevEnd == 0)
            { // First interval.
            if(pPrevArtifact != NULL)
              uStart = pPrevArtifact->m_uEndMin;
            uEnd = pArtifact->m_uStartMin;
            uPrevEnd = uEnd;
            }
          else
            { // Already have one interval.  If this interval is larger than
              // the one we have, use this one.
            if(uArtFreeLen > (uPrevEnd - uStart))
              {
              uStart = uPrevEnd;
              uEnd = pArtifact->m_uStartMin;
              uPrevEnd = uEnd;
              }
            }
          }
        pPrevArtifact = pArtifact;
        uPrevEnd = pArtifact->m_uEndMin; // End of previous artifact.
        }
      }
    *pfStart = (float)uStart / (float)2.0;
    *pfEnd = (float)uEnd / (float)2.0;
    }
//  else
//    bRet = false; // Unrecognized artifact period.
  return(bRet);
  }

/********************************************************************
iFind_artifact - Determing if there is an artifact for this period and
                 half minute.

  inputs: Period for which to find the artifact.
          The time to determine if it is in an artifact.

  return: ARTIFACT_START if the time corresponds to the start of an artifact.
          ARTIFACT_END if the time corresponds to the end of an artifact.
          ARTIFACT_NONE if the time doesn't correspond to an artifact.
********************************************************************/
short int CArtifactID::iFind_artifact(unsigned uPeriod, unsigned uTime)
  {
  CArtifactList *palArtLst;
  CArtifact *pArtifact;
  unsigned uCnt, uSize;
  short int iRet = ARTIFACT_NONE;

  if((palArtLst = palGet_art_list(uPeriod)) != NULL
  && (uSize = palArtLst->GetSize()) > 0)
    { // Have an artifact list.
      // Look at each artifact for the specifed half minute.
    for(uCnt = 0; uCnt < uSize; ++uCnt)
      {
      if((pArtifact = (CArtifact *)palArtLst->GetAt(uCnt)) != NULL)
        {
        if(pArtifact->m_uStartMin == uTime)
          {
          iRet = ARTIFACT_START;
          break;
          }
        else if(pArtifact->m_uEndMin == uTime)
          {
          iRet = ARTIFACT_END;
          break;
          }
        }
      }
    if(pArtifact->m_uType == ART_NONE)
      iRet = ART_NONE; // At the end of the list.
    }
  return(iRet);
  }

/********************************************************************
bGet_next_artifact - Get the next artifact in the list.

  inputs: Period to get artifact for.
          Indicates what artifact to get:
            FIRST_ART - first artifact in the list
            NEXT_ART - next artifact in the list.
          Pointer to value that gets the start minute for the artifact.
          Pointer to value that gets the end minute for the artifact.

  return: true if there may be more artifact in the list.
          false if no more artifact.
********************************************************************/
bool CArtifactID::bGet_next_artifact(unsigned uPeriod, short int iPos, float *pfStart, float *pfEnd)
  {
  CArtifactList *palArtLst;
  CArtifact *pArtifact;
  bool bRet = false;

  if((palArtLst = palGet_art_list(uPeriod)) != NULL && palArtLst->GetSize() > 0)
    { // Have an artifact list.
    if(iPos == FIRST_ART)
      m_uArtPos = 0;
    if((pArtifact = (CArtifact *)palArtLst->GetAt(m_uArtPos)) != NULL
    && pArtifact->m_uType != ART_NONE)
      { // There is an artifact and it is not the last one.
      *pfStart = (float)pArtifact->m_uStartMin / (float)2.0;
      *pfEnd = (float)pArtifact->m_uEndMin / (float)2.0;
      ++m_uArtPos;
      bRet = true;
      }
    }
  return(bRet);
  }

/********************************************************************
bArtifact_in_minutes - Determine if the specified start and end minutes
                       contain either a start or end of an artifact for
                       the specified period.

  inputs: Period for the artifact list.
          Start minute.
          End minute.

  return: true if an artifact is contained between the start and end minute.
          false if there is no artifact between the start and end minute.
********************************************************************/
bool CArtifactID::bArtifact_in_minutes(unsigned uPeriod, float fStart, float fEnd)
  {
  CArtifactList *palArtLst;
  CArtifact *pArtifact;
  unsigned uCnt, uSize;
  float fArtStart, fArtEnd;
  bool bRet = false;

  if((palArtLst = palGet_art_list(uPeriod)) != NULL
  && (uSize = palArtLst->GetSize()) > 0)
    { // Have an artifact list.
      // Look at each artifact for the specifed half minute.
    for(uCnt = 0; uCnt < uSize; ++uCnt)
      {
      if((pArtifact = (CArtifact *)palArtLst->GetAt(uCnt)) != NULL
      && pArtifact->m_uType != ART_NONE)
        {
        fArtStart = (float)pArtifact->m_uStartMin / 2.0F;
        fArtEnd = (float)pArtifact->m_uEndMin / 2.0F;
//        if((fStart <= fArtStart && fArtStart < fEnd)
//        || (fStart < fArtEnd && fArtEnd <= fEnd))
        if((fArtStart <= fStart && fStart <= fArtEnd)
        || (fArtStart <= fEnd && fEnd <= fArtEnd))
          {
          bRet = true;
          break;
          }
        }
      }
    }
  return(bRet);
  }

