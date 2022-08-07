/********************************************************************
DataFile.h

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Header file for the CDataFile class.

  This class contains definitions and structures for reading the data file.

HISTORY:
25-JUN-10  RET  New.
********************************************************************/

#pragma once

/////////////////////////////////////////////////////////////////////
//// Data file definitions.
#define DATA_FILE_VER    5  // Current file version for Standard version.
#define DATA_FILE_VER_R  6  // Current file version for Research version.
#define PROGRAM_NAME_1  "EGGSAP" // Program name for data file version 1.
#define PROGRAM_NAME_2  "EGGSAS" // Program name for data file version 2.
#define PROGRAM_NAME_3  "EGGSS4" // Program name for data file version 3 and 4.
#define PROGRAM_NAME    "EGGSV2" // Program name for data file version 5.
#define PROGRAM_NAME_R1 "EGGRV1" // Program name for Research Version, data file version 6
#define DATA_FILE_HDR_START -1 // Delimiter for file header.  

// Structure for writing patient information to the data file.  This is also 
// used to determine the file version.  For an old DOS file, the first integer
// should never be more that 4096 since a 12-bit A/D card is used.
//// THIS IS USED TO BE BACKWARD COMPATIBLE WITH OLD DATA FILES.
typedef struct
  {
  short int iHdrStart; // All bits are set.
  char szProgName[7];  // "EGGSAP".  A SPEC9 data file will have 7 counts from 
                       // the A/D board here.
  short unsigned int uiDataFileVer;  // The file version for an EGGSAP data file.
                                     // This starts a 1.  A SPEC9  data file
                                     // will have a count from the A/D board here.
  short unsigned int uiPatientDataSize; // Number of characters of patient
                                        //  information entered in the patient 
                                        // information window.
  } DATA_FILE_HEADER;

// Structure used to mark the end of the baseline for the Research version.
// Since the baseline can be a variable length, the end must be marked in the
// data file, it case the study doesn't get recorded and it needs to be
// restored.
typedef struct
  {
  unsigned short uStartWord1; // All bits set.
  unsigned short uStartWord2; // All bits set.
  unsigned short uStartWord3; // All bits set.
  unsigned short uStartWord4; // All bits set.
  long lDataSamples; // Number of data samples.
  long lSpare; // Spare
  unsigned short uEndWord1; // All bits set.
  unsigned short uEndWord2; // All bits set.
  unsigned short uEndWord3; // All bits set.
  unsigned short uEndWord4; // All bits set.
  } BASELINE_TRAILER;


class CDataFile
{
public:
  CDataFile(void);
  virtual ~CDataFile(void);

public: // Data
  CFile m_cfDataFile; // Pointer to the opened data file.
  DATA_FILE_HEADER m_dhfHdr; // File header
  BASELINE_TRAILER m_btTrailer; // Baseline trailer
  CString m_cstrDataPathFile; // Data file path and name.
  CString m_cstrErrMsg; // Error message, empty if no error.
  long m_lBLDataPoints; // Total number of baseline data points in file.
  long m_lPostStimDataPoints; // Post-stimulation data points for a research study.
  //CString m_cstrMI; // Patient middle initial
  //CString m_cstrFirstName; // Patient first name
  //CString m_cstrLastName; // Patient last name
  unsigned int m_uiFileLength;
  //unsigned char *m_pucData;
  short int *m_piData;

public: // Methods
  int iRead_EGGSAS4(void);
  int iOpen_file_get_hdr(void);
  bool bIsValid_hdr(void);
  void vClose_file(void);
  int iNum_data_pnts(void);
  short int iWrite_data(short int *piData, int iNumDataPnts);

  bool bIsResearch_study(void);
  CString cstrDate_of_study(void);
  CString cstrGet_name(void);
  int iAdd_baseline_trailer(void);
  int iWrite_research_data_file(int iBaselineMinutes);
};
