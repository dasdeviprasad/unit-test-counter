/********************************************************************
ArtifactID.h

Copyright (C) 2003,2004, 3CPM Company, Inc.  All rights reserved.

  Header file for the CArtifactID class.

HISTORY:
06-OCT-03  RET  New.
********************************************************************/

#if !defined(AFX_ARTIFACTID_H__E38A2EF9_1D58_4393_8561_72285A4E8F15__INCLUDED_)
#define AFX_ARTIFACTID_H__E38A2EF9_1D58_4393_8561_72285A4E8F15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ART_NONE    0  // No artifact.
#define ART_PGM_SEL 1 // Program selected artifact
#define ART_OP_SEL  2 // Operator selected artifact
#define ART_OP_REJ  3 // Operator rejected artifact
//#define ART_LAST    4 // Last item in the artifact list.  This is not an
//                      // artifact but a place holder to remember the total
//                      // number of half minute intervals in the period.
#define ART_OFFSCALE 0x8000 // Artifact is off scale
                            //  If bit not set then artifact is flat line.

#define INV_FLOAT FLT_MAX

#define DATA_POINTS_PER_SEC (DATA_POINTS_PER_MIN_1 / 60)
#define DATA_POINTS_PER_ARTIFACT_DURATION (DATA_POINTS_PER_SEC * 5)
#define DATA_POINTS_PER_HALF_MIN (DATA_POINTS_PER_MIN_1 / 2)

#define MAX_BASELINE  5000.0F  // 5000 microvolts
#define MAX_ZERO 1 // Maximum value that is considered no signal
#define MIN_ZERO -1 // Minimum value that is considered no signal

#define IS_ZERO_SIG(f) ((f) >= MIN_ZERO && (f) <= MAX_ZERO)

// Definitions for finding an artifact
#define ARTIFACT_NONE  0
#define ARTIFACT_START 1
#define ARTIFACT_END   2

// Definitions for getting the next artifact.
#define FIRST_ART 0  // first artifact in the list
#define NEXT_ART  1  // next artifact in the list.

/////////////////////////////////////////////////////////////////////
// This class holds one artifact.
class CArtifact : public CObject
{
public:
	CArtifact();
//	DECLARE_SERIAL(CArtifact)

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
//public:

// Implementation
public:
	virtual ~CArtifact();
//	virtual void Serialize(CArchive& ar);
};

  // List of artifacts that apply to a specific period.
  //   The last item in the list is not an artifact, but a
  //   marker to remember the total number of data points in the period.
typedef CTypedPtrArray<CObArray,CArtifact*> CArtifactList;


/////////////////////////////////////////////////////////////////////
class CArtifactID  
{
public:
	CArtifactID();
	virtual ~CArtifactID();

public: // Data
  float m_fBaseLineLowLimit; // Signal low limit when analyzing baseline.
  float m_fBaseLineHiLimit;  // Signal high limit when analyzing baseline.
  unsigned short m_uLenBL;   // Length of baseline period.
  unsigned short m_uLenBadBL; // Length of bad baseline signal.
  unsigned short m_uArtType; // Artifact type, flat line or offscale.
  float m_fArtLowLimit; // Below this limit is an artifact.
  float m_fArtHiLimit; // Above this limit is an artifact.
  unsigned short m_uArtPos; // Used for getting the next artifact.
                            // This is the index number for the current artifact
  float m_fArtWindow; // Window outside of which defines an artifact.
  unsigned short m_uResetArtLists; // Contains 1 bit per artifact list (period).
                                   // If bit is set, delete the artifact list
                                   // before rebuilding it.
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
  void vPeriod_id(unsigned uPeriod, float *pfData, unsigned uNumVals, CArtifactList *pArtLst);
  void vBaseLine_id(float fData);
  void vSet_artifact_limit(float fMeanBL);
  bool bFind_art_free_min(unsigned uPeriod, float *pfStart, float *pfEnd);
  void vDel_artifact(unsigned uPeriod, int iIndex);
  void vSet_baseline_limit(float fLowLimit, float fHiLimit);
  short int iFind_artifact(unsigned uPeriod, unsigned uTime);
  bool bGet_next_artifact(unsigned uPeriod, short int iPos, float *pfStart, float *pfEnd);
  bool bArtifact_in_minutes(unsigned uPeriod, float fStart, float fEnd);
  void vReset_artifact_list(unsigned uPeriod);
  void vUndo_reset_artifact_list(unsigned uPeriod);

};

#endif // !defined(AFX_ARTIFACTID_H__E38A2EF9_1D58_4393_8561_72285A4E8F15__INCLUDED_)
