/********************************************************************
DataFile.cpp

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CDataFile class.

  This class performs file I/O for the data file.
  It also parses the file name for the patient name and date of study.

HISTORY:
25-JUN-10  RET  New.
********************************************************************/

#include "StdAfx.h"
#include "ConvertDataFile.h"
#include "DataFile.h"
#include ".\datafile.h"

/********************************************************************
Constructor

********************************************************************/
CDataFile::CDataFile(void)
  {

  }

/********************************************************************
Destructor

Close the data file if its not already closed.
********************************************************************/
CDataFile::~CDataFile(void)
  {

  vClose_file();
  }

int CDataFile::iNum_data_pnts()
  {
  long lStart, lEnd;
  int iNumPnts;

  m_cstrErrMsg = "";
  lStart = sizeof(m_dhfHdr) + m_dhfHdr.uiPatientDataSize;
  //lStart += m_dhfHdr.uiPatientDataSize;
  try
    {
    lEnd = (long)m_cfDataFile.Seek(0, CFile::end);
    m_uiFileLength = (unsigned int)(lEnd - lStart);
    iNumPnts = m_uiFileLength / sizeof(short int);
    }
  catch(CFileException *pfe)
    { // Got a file read error.
    m_cstrErrMsg.Format(_T("Error reading file %s, %s"), m_cstrDataPathFile,
      strerror(pfe->m_lOsError));
    iNumPnts = 0;
    pfe->Delete();
    }
  return(iNumPnts);
  }

/********************************************************************
iRead_EGGSAS4 - Read a data file.
        Saves the header and baseline trailer information and the total
          number of data points in the file.

  inputs: None.

  return: SUCCESS if the file is read with no errors.
          FAIL if there was an error, the class variable contains
            the error message.
********************************************************************/
int CDataFile::iRead_EGGSAS4(void)
  {
  int iRet;
  long lPosition;
  bool bGotOne = false;
  unsigned int uNumBytesRead;
  CFileStatus cfStatus;

  iRet = SUCCESS;
  m_cstrErrMsg = "";
  m_lBLDataPoints = 0;
  m_lPostStimDataPoints = 0;
  m_btTrailer.lDataSamples = 0;
  // allocate a buffer big enough to hold the entire data file.
  if((m_piData = (short int *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_uiFileLength))
  != NULL)
    {
    // Data starts here.
    lPosition = sizeof(m_dhfHdr) + m_dhfHdr.uiPatientDataSize;
    try
      {
      m_cfDataFile.Seek(lPosition, CFile::begin);
      }
    catch(CFileException *pfe)
      { // Got a file read error.
      m_cstrErrMsg.Format(_T("Error reading file %s, %s"), m_cstrDataPathFile,
        strerror(pfe->m_lOsError));
      iRet = FAIL;
      pfe->Delete();
      }
    try
      {
      uNumBytesRead = m_cfDataFile.Read(m_piData, m_uiFileLength);
      if(uNumBytesRead < m_uiFileLength)
        {
        m_cstrErrMsg.Format(_T("Unexpected EOF in %s"), m_cstrDataPathFile);
        iRet = FAIL;
        }
      }
    catch(CFileException *pfe)
      { // Got a file read error.
      if(pfe->m_cause != CFileException::endOfFile)
        { // Not an end of file.
        m_cstrErrMsg.Format(_T("Error reading file %s, %s"), m_cstrDataPathFile,
          strerror(pfe->m_lOsError));
        }
      else
        m_cstrErrMsg.Format(_T("Unexpected EOF in %s"), m_cstrDataPathFile);
      iRet = FAIL;
      pfe->Delete();
      }
    }
  else
    {
    m_cstrErrMsg = "Can't allocation enough memory to read data file.";
    iRet = FAIL;
    }

  return(iRet);
  }

/********************************************************************
iOpen_file_get_hdr - Opens a data file, reads and saves the header information.

  inputs: None.

  return: SUCCESS if the file is read with no errors.
          FAIL if there was an error and the class variable contains
            the error message.
********************************************************************/
int CDataFile::iOpen_file_get_hdr(void)
  {
  int iRet;
  int iNumBytes, iNumBytesToRead;
  CFileException FileEx;

  m_cstrErrMsg = "";
  iRet = SUCCESS;
  try
    { // Open the file.
      if(m_cfDataFile.Open(m_cstrDataPathFile, CFile::modeRead | CFile::shareDenyWrite, &FileEx)
        == FALSE)
        {
        iRet = FAIL;
        m_cstrErrMsg.Format(_T("Error opening file %s, %s"), m_cstrDataPathFile,
          strerror(FileEx.m_lOsError));
        }
    }
  catch(CFileException *pfe)
    { // Got a file open error.
    m_cstrErrMsg.Format(_T("Error opening file %s, %s"), m_cstrDataPathFile,
      strerror(pfe->m_lOsError));
    pfe->Delete();
    iRet = FAIL;
    }
  if(iRet == SUCCESS)
    { // Check to see if this is a EGGSAP data file or SPEC9 data file.
    try
      { // Read the file header.
      iNumBytesToRead = sizeof(DATA_FILE_HEADER);
      //if((iNumBytes = m_cfDataFile.Read(&m_dhfHdr, iNumBytesToRead))
      //  < iNumBytesToRead)
      iNumBytes = m_cfDataFile.Read(&m_dhfHdr, iNumBytesToRead);
      if(iNumBytes  < iNumBytesToRead)
        { // Data file is too small.
        m_cstrErrMsg.Format(_T("No header found in: %s"), m_cstrDataPathFile);
        iRet = FAIL;
        }
      }
    catch(CFileException *pfe)
      { // Got a file read error.
      if(pfe->m_cause != CFileException::endOfFile)
        { // Not an end of file.
        m_cstrErrMsg.Format(_T("Error reading file %s, %s"), m_cstrDataPathFile,
          strerror(pfe->m_lOsError));
        }
      else
        m_cstrErrMsg.Format(_T("Unexpected EOF in %s"), m_cstrDataPathFile);
      iRet = FAIL;
      pfe->Delete();
      }
    }
  return(iRet);
  }

/********************************************************************
bIsValid_hdr - Validates the header read from the file for the
               file version number and the program version that created
               it.

  inputs: None.

  return: true if the header is OK, false if the file version or the
          program version is wrong.
********************************************************************/
bool CDataFile::bIsValid_hdr(void)
  {
  bool bRet;

  m_cstrErrMsg = "";
  if(m_dhfHdr.iHdrStart == DATA_FILE_HDR_START
  && ((m_dhfHdr.uiDataFileVer == 1 && strcmp(m_dhfHdr.szProgName, PROGRAM_NAME_1) == 0)
  || (m_dhfHdr.uiDataFileVer == 2 && strcmp(m_dhfHdr.szProgName, PROGRAM_NAME_2) == 0)
  || (m_dhfHdr.uiDataFileVer == 3 && strcmp(m_dhfHdr.szProgName, PROGRAM_NAME_3) == 0)
  || (m_dhfHdr.uiDataFileVer == 4 && strcmp(m_dhfHdr.szProgName, PROGRAM_NAME_3) == 0)
  || (m_dhfHdr.uiDataFileVer == 5 && strcmp(m_dhfHdr.szProgName, PROGRAM_NAME) == 0)
  || (m_dhfHdr.uiDataFileVer == 6 && strcmp(m_dhfHdr.szProgName, PROGRAM_NAME_R1) == 0)))
    bRet = true;
  else
  {
    m_cstrErrMsg.Format(_T("File header is invalid for %s"), m_cstrDataPathFile);
    bRet = false;
  }
  return(bRet);
  }

/********************************************************************
vClose_file - Close the data file if it is open.

  inputs: None.

  return: None.
********************************************************************/
void CDataFile::vClose_file(void)
  {
  if(m_cfDataFile.m_hFile != CFile::hFileNull)
    m_cfDataFile.Close();
  if(m_piData != NULL)
    {
    HeapFree(GetProcessHeap(), 0, (LPVOID)m_piData);
    m_piData = NULL;
    }
  }


/********************************************************************
iAdd_baseline_trailer - Add a baseline trailer to the data file.  This
                        makes a cancelled research study a paused study.

  inputs: None.

  return: SUCCESS if the baseline trailer was added.
          FAIL if there was an error.
********************************************************************/
int CDataFile::iAdd_baseline_trailer(void)
  {
  int iRet;
  CFileException FileEx;
  UINT uiSize;

  m_btTrailer.uStartWord1 = 0xFFFF;
  m_btTrailer.uStartWord2 = 0xFFFF;
  m_btTrailer.uStartWord3 = 0xFFFF;
  m_btTrailer.uStartWord4 = 0xFFFF;
  m_btTrailer.uEndWord1 = 0xFFFF;
  m_btTrailer.uEndWord2 = 0xFFFF;
  m_btTrailer.uEndWord3 = 0xFFFF;
  m_btTrailer.uEndWord3 = 0xFFFF;
  // m_lBLDataPoints is the number of data points in the original file.
  // This must be divided by 2 as the original file has 4 channels and this
  // file has 2 channels.
  m_btTrailer.lDataSamples = m_lBLDataPoints / 2;
  m_btTrailer.lSpare = 0;
  // Open the file for appending and write the baseline trailer.
  //m_cstrErrMsg = "";
  iRet = SUCCESS;
  try
    {
      // Append the baseline trailer.
    uiSize = sizeof(BASELINE_TRAILER);
    m_cfDataFile.Write(&m_btTrailer, uiSize);
    }
  catch(CFileException *pfe)
    { // Got a file open error.
    m_cstrErrMsg.Format(_T("Error opening file %s, %s"), m_cstrDataPathFile,
      strerror(pfe->m_lOsError));
    pfe->Delete();
    iRet = FAIL;
    }
  return(iRet);
  }

int CDataFile::iWrite_research_data_file(int iBaselineMinutes)
  {
  int iRet, iBaselineLen, iPostwaterLen;
  CFileException FileEx;
  UINT uiSize;

  m_cstrErrMsg = "";
  iRet = SUCCESS;
  try
    {
    // Open the file for writing.
    if(m_cfDataFile.m_hFile != CFile::hFileNull)
      m_cfDataFile.Close();
    if(m_cfDataFile.Open(m_cstrDataPathFile, CFile::modeCreate | CFile::modeWrite, &FileEx)
        == FALSE)
        {
        iRet = FAIL;
        m_cstrErrMsg.Format(_T("Error opening file %s, %s"), m_cstrDataPathFile,
          strerror(FileEx.m_lOsError));
        }
    }
  catch(CFileException *pfe)
    { // Got a file open error.
    m_cstrErrMsg.Format(_T("Error opening file %s, %s"), m_cstrDataPathFile,
      strerror(pfe->m_lOsError));
    pfe->Delete();
    iRet = FAIL;
    }
  if(iRet == SUCCESS)
    {
    try
      {
      // Change the data file header to reflect that this is now a research data file.
      strcpy(m_dhfHdr.szProgName, "EGGRV1");
      m_dhfHdr.uiDataFileVer = DATA_FILE_VER_R;
      m_dhfHdr.uiPatientDataSize = 0;
      // Write the header
      m_cfDataFile.Seek(0, CFile::begin);
      uiSize = sizeof(DATA_FILE_HEADER);
      m_cfDataFile.Write(&m_dhfHdr, uiSize);
      // Write the baseline data
      // 4 channels of data for each point.
      m_lBLDataPoints = iBaselineMinutes * DATA_POINTS_PER_MIN_1 * 4;
      iBaselineLen = m_lBLDataPoints * sizeof(short int); //+ m_dhfHdr.uiPatientDataSize;
      if((iRet = iWrite_data(m_piData, (int)m_lBLDataPoints)) == SUCCESS)
        {
        // Write the baseline trailer
        iAdd_baseline_trailer();
        // Write the postwater data
        iPostwaterLen = (int)m_uiFileLength - iBaselineLen;
        int iNumPostWaterDataPnts = iPostwaterLen / sizeof(short int);
        iRet = iWrite_data(m_piData + m_lBLDataPoints, iNumPostWaterDataPnts);
        }
      //iRet = SUCCESS;
      }
    catch(CFileException *pfe)
      { // Got a file open error.
      m_cstrErrMsg.Format(_T("Error writing file %s, %s"), m_cstrDataPathFile,
        strerror(pfe->m_lOsError));
      pfe->Delete();
      iRet = FAIL;
      }
    }

  return(iRet);
  }

short int CDataFile::iWrite_data(short int *piData, int iNumDataPnts)
  {
  short int iRet = SUCCESS;
  float fScaleFactor, fData;
  int iIndex;

  // File consists if short ints, 4 data points per sample, 3 EGG and 1 respiration.
  fScaleFactor = 1.72660F;
  try
    {
    for(iIndex = 0; iIndex < iNumDataPnts; iIndex += 4)
      {
      fData = (float)*piData * fScaleFactor;
      m_cfDataFile.Write(&fData, sizeof(float)); // EGG
      ++piData;
      fData = (float)*piData * fScaleFactor;
      m_cfDataFile.Write(&fData, sizeof(float)); // Respiration
      piData += 3; // Skip other 2 EGG channels.
      }
    }
    catch(CFileException *pfe)
      { // Got a file open error.
      m_cstrErrMsg.Format(_T("Error writing file %s, %s"), m_cstrDataPathFile,
        strerror(pfe->m_lOsError));
      pfe->Delete();
      iRet = FAIL;
      }
  return(iRet);
  }