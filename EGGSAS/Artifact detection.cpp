/********************************************************************
ArtifactId.cpp

  class CArtifactId

Artifact Idendification
  - Identifies artifact both while recording and from the data file
  - Uses the CArtifact class to maintain a list of artifacts.  There
    is a separate list for the baseline and each 10 minute period.

********************************************************************/

#define ART_NONE      // No artifact.
#define ART_PGM_SEL 1 // Program selected artifact
#define ART_OP_SEL  2 // Operator selected artifact
#define ART_OP_REJ  3 // Operator rejected artifact
#define ART_OFFSCALE 0x8000 // Artifact is off scale
                            //  If bit not set then artifact is flat line.

#define INV_FLOAT FLT_MAX

#define DATA_POINTS_PER_SEC (DATA_POINTS_PER_MIN_1 / 60)
#define DATA_POINTS_PER_ARTIFACT_DURATION (DATA_POINTS_PER_SEC * 5)
#define DATA_POINTS_PER_HALF_MIN (DATA_POINTS_PER_MIN_1 / 2)

#define MAX_BASELINE  5000F  // 5000 microvolts
#define MAX_ZERO 1 // Maximum value that is considered no signal
#define MIN_ZERO -1 // Minimum value that is considered no signal

#define IS_ZERO_SIG(f) (f >= MIN_ZERO && f <= MAX_ZERO)

#define PERIOD_NONE 0
#define PERIOD_BL   1 // Baseline period
#define PERIOD_10   2 // First ten minute period
#define PERIOD_20   3 // Second ten minute period
#define PERIOD_30   4 // Third ten minute period

/////////////////////////////////////////////////////////////////////
// This class holds one artifact.
class CArtifact : public CObject
{
public:
	CArtifact();
	DECLARE_SERIAL(CArtifact)

// Data
public:
	short unsigned m_uStartMin; // Rounded to previous 1/2 minute mark
	short unsigned m_uEndMin;   // Rounded to next 1/2 minute mark.
  short unsigned m_uType; // Type of artifact:
                          //   Program selected
                          //   Operator rejected
                          //   Operator added
                          //   Artifact type: flat line or offscale.
                          //   No artifact.  This marks the end of the
                          //     artifact list.  m_uEndMin is the last
                          //     data point in the period.

// Operations
public:

// Implementation
public:
	virtual ~CArtifact();
	virtual void Serialize(CArchive& ar);
};

  // List of artifacts that apply to a specific period.
  //   The last item in the list is not an artifact, but a
  //   marker to remember the total number of data points in the period.
typedef CTypedPtrArray<CObArray,CArtifact*> CArtifactList;


/////////////////////////////////////////////////////////////////////
class CArtifactId
{
public:
  CArtifactId();  // Constructor
  ~CArtifactId();  // Destructor

public: // Data
  float m_fMaxBL; // Max baseline signal
  unsigned short m_uLenBL;   // Length of baseline period.
  unsigned short m_uLenBadBL; // Length of bad baseline signal.
  unsigned short m_uArtType; // Artifact type, flat line or offscale.
  float m_fArtLimit; // Above this limit is an artifact.

    // List of artifacts, one list for each period in test.
    // The last item in each list is not an artifact. It is
    // there to mark the end minute of the data.
    // Artifacts with overlapping minutes are valid.
  CArtifactList m_ArtifactsBL;
  CArtifactList m_Artifacts10;
  CArtifactList m_Artifacts20;
  CArtifactList m_Artifacts30;

public: // Methods
  CArtifactList *palGet_art_list(unsigned uPeriod);
  bool bIns_artifact(unsigned uPeriod, unsigned short uStart, unsigned short uEnd);
  void vArtifact_id(unsigned uPeriod, float *pfData, unsigned uNumVals);
  void vPeriod_id(float *pfData, unsigned uNumVals, CArtifactList *pArtLst);
  void vBaseLine_id(float fData);
  void vSet_artifact_limit();
  bool bFind_art_free_min(unsigned uPeriod, unsigned short *puStart,
                          unsigned short *puEnd);

};

/********************************************************************
constructor

  Initialize class variables.

  inputs: None.

********************************************************************/
CArtifactId::CArtifactId()
  {
  }

/********************************************************************
destructor

  Empty out each artifact list and destroy the objects.

********************************************************************/
CArtifactId::~CArtifactId()
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

CArtifactList *CArtifactId::palGet_art_list(unsigned uPeriod)
  {
  CArtifactList *palArtLst;

  switch(uPeriod)
    {
    case PERIOD_BL:
      palArtLst = m_ArtifactsBL;
      break;
    case PERIOD_10:
      palArtLst = m_Artifacts10;
      break;
    case PERIOD_20:
      palArtLst = m_Artifacts20;
      break;
    case PERIOD_30:
      palArtLst = m_Artifacts30;
      break;
    default:
      palArtLst = NULL;
      break;
    }
  return(palArtLst);
  }

bool CArtifactId::bIns_artifact(unsigned uPeriod, unsigned short uStart, unsigned short uEnd)
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

void CArtifactId::vDel_artifact(unsigned uPeriod, int iIndex)
  {
  CArtifactList *palArtLst;
  CArtifact *pArtifact;

  if((palArtLst = palGet_art_list(uPeriod)) != NULL)
    {
    pArtifact = palArtLst[iIndex];
      // Remove the CArtifact ptr from the CTypedPtrArray.
    palArtLst.RemoveAt(iIndex);
      // Delete the CArtifact object. (The CTypedPtrArray only holds the ptr.)
    delete pArtifact;
    }
  }

/********************************************************************
Artifact_id - Identify artifacts and good minutes.

  inputs: Period (10, 20 or 30) which this data is for.
          Pointer to an array of floating point values for which artifact
            is going to be identified and remembered.
          Number of values.

********************************************************************/
void CArtifactId::vArtifact_id(unsigned uPeriod, float *pfData, unsigned uNumVals)
  {
  CArtifactList *palArtLst;

  if((palArtLst = palGet_art_list(uPeriod)) != NULL)
    vPeriod_id(pfData, uNumVals, pArtLst);
  }

void CArtifactId::vPeriod_id(float *pfData, unsigned uNumVals, CArtifactList *pArtLst)
  {
  CArtifact *pArtifact;
  unsigned uCnt;
  unsigned short uNumArtPts, uStart, uEnd;

  uNumArtPts = 0;
  for(uCnt = 0; uCnt < uNumVals; ++uCnt, ++pfData)
    {
    if(IS_ZERO_SIG(*pfData) || *pfData > m_fArtLimit)
      {
      if(uNumArtPts == 0)
        uStart = uCnt; // Start time.
      ++uNumArtPts;
      }
    else
      {
      if(uNumArtPts != 0)
        { // Record this artifact.
        pArtifact = new CArtifact();
          // Calculate the start on the nearest previous 1/2 minute.
        pArtifact->m_uStartMin = uStart / DATA_POINTS_PER_HALF_MIN;
          // Calculate the end of the nearest next 1/2 minute.
        pArtifact->m_uEndMin = ((uCnt - 1) + DATA_POINTS_PER_HALF_MIN) / DATA_POINTS_PER_HALF_MIN;
        pArtifact->m_uType = ART_PGM_SEL;
        if(*(pfData - 1) > m_fArtLimit)
          pArtifact->m_uType |= ART_OFFSCALE;
        pArtLst->Add((CArtifact *)pArtifact);
        }
      uNumArtPts = 0;
      }
    }
    // Add the last data point to the artifact list.
    // This is to remember the number of sample in the period.
  pArtifact = new CArtifact();
  pArtifact->m_uStartMin = uNumVals;
  pArtifact->m_uEndMin = uNumVals;
  pArtifact->m_uType = ART_NONE;
  pArtLst->Add((CArtifact *)pArtifact);
  }

// Gets every 80th data point.
// Called during recording baseline
void CArtifactId::vBaseLine_id(float fData)
  {

  if(m_fMaxBL == INV_FLOAT)
    { // Initialize.
    m_uLenBL = 0;
    m_uLenBadBL = 0;
    m_fMaxBL = fData;
    }
  ++m_uLenBL;
  if(IS_ZERO_SIG(fData))
    { // Flat line
    ++m_uLenBadBL;
    m_uArtType = 0;
    }
  else if(fData > MAX_BASELINE)
    { // Signal off scale
    ++m_uLenBadBL;
    m_uArtType = ART_OFFSCALE;
    }
  else
    { // Good signal.
    if(m_uLenBadBL > 0)
      { // Record an artifact.
      pArtifact = new CArtifact();
        // Calculate the start on the nearest previous 1/2 minute.
      pArtifact->m_uStartMin = m_iStartBL / DATA_POINTS_PER_HALF_MIN;
        // Calculate the end of the nearest next 1/2 minute.
      pArtifact->m_uEndMin = (m_uLenBL + DATA_POINTS_PER_HALF_MIN) / DATA_POINTS_PER_HALF_MIN;
      pArtifact->m_uType = ART_PGM_SEL;
      pArtifact->m_uType |= m_uArtType;
      m_ArtifactsBL.Add((CArtifact *)pArtifact);
      m_uLenBadBL = 0;
      }
    if(fData > m_fMaxBL)
      m_fMaxBL = fData;
    }
  }

void CArtifactId::vSet_artifact_limit()
  {
  m_fArtLimit = m_fMaxBL * (float)10.0;
  }

bool CArtifactId::bFind_art_free_min(unsigned uPeriod, unsigned short *puStart,
unsigned short *puEnd)
  {
  CArtifactList *palArtLst;
  CArtifact *pArtifact;
  unsigned short uCnt, uSize;
  short int iArtFreeLen;
  unsigned short uEnd;
  bool bRet;

  *puStart = 0;
  *puEnd = 0;
  if((palArtLst = palGet_art_list(uPeriod)) != NULL)
    {
    if((uSize = palArtLst->GetSize()) == 0)
      bRet = false; // No good minutes.
    else if(uSize == 1)
      { // Only one object in list, this is the terminating object.
        // This indicates there are no artifacts in this period.
      bRet = true;
      pArtifact = (CArtifact *)palArtLst->GetAt(0);
      *puEnd = pArtifact->m_uEndMin;
      }
    else
      { // Search for the longest artifact free period.
      iArtFreeLen = 0;
      uEnd = 0; // End of previous artifact.
      for(uCnt = 0; uCnt < uSize; ++uCnt)
        {
        pArtifact = (CArtifact *)palArtLst->GetAt(uCnt);
        if((iArtFreeLen = pArtifact->m_uStartMin - uEnd) > DATA_POINTS_PER_EPOCH_1)
          { // Found at least 4 minutes that are artifact free.
          if(*puStart == 0 && *puEnd == 0)
            { // First interval.
            *puStart = pArtifact->m_uStartMin;
            *puEnd = pArtifact->m_uEndMin;
            bRet = true;
            }
          else
            { // Already have one interval.  If this interval is larger than
              // the one we have, use this one.
            if(iArtFreeLen > (*puEnd - *puStart))
              {
              *puStart = pArtifact->m_uStartMin;
              *puEnd = pArtifact->m_uEndMin;
              }
            }
          }
        uEnd = pArtifact->m_uEndMin; // End of previous artifact.
        }
      }
    }
  else
    bRet = false; // Unrecognized artifact period.
  return(bRet);
  }


/***
//// Algorithms

//// For database
signal eval high - used for baseline analysis

//// Definitions
  // Recording mode
#define REC_INACTIVE   0
#define REC_TEST       1
#define REC_SIG_EVAL   2
// ?? do I need a baseline and water load mode??
#define REC_RUN        3

//// Data
int iRecordMode;
int iRecordTime; // Milliseconds
int iSigTooHiTime;
int iSigTooLoTIme;

//// Test mode
// Called from timer used to read the signal.
if recording time < 10 seconds
  if signal is too low
    if too low for > 1 sec
      Set display items
      reset recording time
  else if signal is too high
    if too high for > 5 sec
      Set display items
      reset recording time
else // Signal OK and recording time is > 10 seconds
  set record mode to signal evaluation
  reset recording time
  set signal eval recording time to 0
  set signal eval low and high to invalid floats.

//// Signal evaluation mode
// Called from timer used to read the signal.
if signal eval recording time < 30 seconds
  if(signal is > too low and < too high
    if(signal eval low is invalid float)
      save this signal as signal eval low.
    if(signal eval high is invalid float)
      save this signal as signal eval high.
    if(signal is < signal eval low)
      save this signal as signal eval low.
    else if(signal is > signal eval high)
      save this signal as signal eval high
    update signal eval recording time
  else
    set signal eval low and high to invalid floats.
else
  if(signal eval low and high == invalid floats)
    set baseline artifact value to invalid float
  else
    set baseline artifact value to signal eval high
  baseline high = invalid float
  set mode to baseline

//// Baseline recording mode
// Called from timer used to read the signal.
if(set baseline artifact value == invalid float)
  call signal eval function
else
  if(baseline high == invalid float)
    baseline high = signal
  else if(signal > baseline high)
    baseline high = signal
if(end of baseline)
  artifact limit = baseline high * 10
  save baseline end minute
  
  

//// Analyze mode
Open file and go to data section
if(artifact limit == invalid float)
  look through baseline section to get artifact limit.
determine number of minutes of good baseline signal
  0 < signal < 5000
if( > 4 minutes)
  go to start of first 10 minute period in file
  Look through 10 minute period for 4 good minutes.


Sample rate is 341.36s/s that is 4.267Hz*80.
The only two posible sample rates are 170.68 and 85.34s/s.

  Save every 80th data point.
    - This should be equivalent to EGGSAS-4 saving every 1000th data point.
    - This is whats gets put on the strip chart.
    - This is whats gets saved.

********************************************************************/
