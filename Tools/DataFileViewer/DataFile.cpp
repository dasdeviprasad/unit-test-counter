/********************************************************************
DataFile.cpp

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CDataFile class.

  This class performs file I/O for the data file.
  It also parses the file name for the patient name and date of study.

HISTORY:
25-JUN-10  RET  New.
10-AUG-10  RET  Version 1.1
                  Add methods: uiGet_data_file_version(), cstrGet_data_file_type()
********************************************************************/

#include "StdAfx.h"
#include "DataFileViewer.h"
#include "DataFile.h"

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

/********************************************************************
iRead - Read a data file.
        Saves the header and baseline trailer information and the total
          number of data points in the file.

  inputs: None.

  return: SUCCESS if the file is read with no errors.
          FAIL if there was an error, the class variable contains
            the error message.
********************************************************************/
int CDataFile::iRead(void)
  {
  int iRet;
  long lPosition, lData;
  bool bGotOne = false;
  unsigned int uNumBytesRead;
  CFileStatus cfStatus;

  iRet = SUCCESS;
  m_cstrErrMsg = "";
  m_lTotalDataPoints = 0;
  m_lPostStimDataPoints = 0;
  m_btTrailer.lDataSamples = 0;
  // Data starts here.
  lPosition = sizeof(m_dhfHdr) + m_dhfHdr.uiPatientDataSize;
  try
    {
    m_cfDataFile.Seek(lPosition, CFile::begin);
    }
  catch(CFileException *pfe)
    { // Got a file read error.
    m_cstrErrMsg.Format(_T(IDS_ERR_FILE_READ), m_cstrDataPathFile,
      strerror(pfe->m_lOsError));
    iRet = FAIL;
    pfe->Delete();
    }
  // Read until we get 4 words = 0xFFFF, or 2 longs.
  while(iRet == SUCCESS)
    {
    try
      {
      uNumBytesRead = m_cfDataFile.Read(&lData, sizeof(long));
      if(uNumBytesRead == 0)
        break; // end of file
      else if(uNumBytesRead < sizeof(long))
        {
        m_cstrErrMsg.Format(_T(IDS_ERR_UNEXPECTED_EOF), m_cstrDataPathFile);
        iRet = FAIL;
        }
      else
        { // Got a data point or start of Baseline trailer.
        if(lData == 0xFFFFFFFF)
          { // Baseline trailer.
          if(bGotOne == true)
            { // Got a second 0xFFFF.
            // Back up to start of BASELINE_TRAILER structure.
            lPosition = (long)m_cfDataFile.GetPosition();
            m_cfDataFile.Seek(lPosition - 8, CFile::begin);
            // Read the number of point in the baseline.
            m_cfDataFile.Read(&m_btTrailer, sizeof(BASELINE_TRAILER));
            // Get the post-stimulation data points.
            m_cfDataFile.GetStatus(cfStatus);
            lPosition = (long)m_cfDataFile.GetPosition();
            m_lPostStimDataPoints = ((long)cfStatus.m_size - lPosition) / 4;
            break;
            }
          else
            bGotOne = true; // First 0xFFFF.
          }
        else
          { // Data point.
          ++m_lTotalDataPoints;
          bGotOne = false;
          }
        }
      }
    catch(CFileException *pfe)
      { // Got a file read error.
      if(pfe->m_cause != CFileException::endOfFile)
        { // Not an end of file.
        m_cstrErrMsg.Format(_T(IDS_ERR_FILE_READ), m_cstrDataPathFile,
          strerror(pfe->m_lOsError));
        }
      else
        m_cstrErrMsg.Format(_T(IDS_ERR_UNEXPECTED_EOF), m_cstrDataPathFile);
      iRet = FAIL;
      pfe->Delete();
      }
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
        m_cstrErrMsg.Format(_T(IDS_ERR_FILE_OPEN), m_cstrDataPathFile,
          strerror(FileEx.m_lOsError));
        }
    }
  catch(CFileException *pfe)
    { // Got a file open error.
    m_cstrErrMsg.Format(_T(IDS_ERR_FILE_OPEN), m_cstrDataPathFile,
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
        m_cstrErrMsg.Format(_T(IDS_ERR_NO_HEADER), m_cstrDataPathFile);
        iRet = FAIL;
        }
      }
    catch(CFileException *pfe)
      { // Got a file read error.
      if(pfe->m_cause != CFileException::endOfFile)
        { // Not an end of file.
        m_cstrErrMsg.Format(_T(IDS_ERR_FILE_READ), m_cstrDataPathFile,
          strerror(pfe->m_lOsError));
        }
      else
        m_cstrErrMsg.Format(_T(IDS_ERR_UNEXPECTED_EOF), m_cstrDataPathFile);
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
    m_cstrErrMsg.Format(_T(IDS_ERR_INV_FILE_HDR), m_cstrDataPathFile);
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
  }

/********************************************************************
bIsResearch_study - Determine if the data file contains a research or
                    waterload study.

  inputs: None.

  return: true if a research study, false if a waterload study.
********************************************************************/
bool CDataFile::bIsResearch_study(void)
  {
  bool bRet;

  if(m_dhfHdr.uiDataFileVer == 6)
    bRet = true;
  else
    bRet = false;
  return(bRet);
  }

/********************************************************************
cstrDate_of_study - Get the date of the study from the file name.
                    Format: mm/dd/yyyy

  inputs: None.

  return: CString object containing the date of the study.
********************************************************************/
CString CDataFile::cstrDate_of_study(void)
  {
  CString cstrDate;
  int iIndex, iIndex1;

  cstrDate = "";
  if((iIndex = m_cstrDataPathFile.Find('.')) > 0)
  {
    if((iIndex1 = m_cstrDataPathFile.ReverseFind(' ')) > 0)
    { // Extract the date.
      cstrDate = m_cstrDataPathFile.Mid(iIndex1 + 1, iIndex - iIndex1 - 1);
      // Change '-' to '/'
      cstrDate.Replace(_T("-"), _T("/"));
    }
  }
  return (cstrDate);
  }

/********************************************************************
cstrGet_name - Get the patient name from the file name.
               Format: FirstName MI LastName

  inputs: None.

  return: CString object containing the patient name.
********************************************************************/
CString CDataFile::cstrGet_name(void)
  {
  int iIndex, iIndex1;
  CString cstrName;

  if((iIndex = m_cstrDataPathFile.ReverseFind('\\')) > 0)
    {
    if((iIndex1 = m_cstrDataPathFile.Find(' ', iIndex)) > 0)
      {
      cstrName = m_cstrDataPathFile.Mid(iIndex + 1, iIndex1 - iIndex - 1);
      m_cstrFirstName = cstrName;
      iIndex = iIndex1 + 1;
      if((iIndex1 = m_cstrDataPathFile.Find(' ', iIndex)) >= 0)
        {
        if(iIndex1 == iIndex)
          m_cstrMI = "";
        else
          m_cstrMI = m_cstrDataPathFile.Mid(iIndex, iIndex1 - iIndex);
        cstrName += " " + m_cstrMI;
        iIndex = iIndex1 + 1;
        if((iIndex1 = m_cstrDataPathFile.Find(' ', iIndex)) >= 0)
          {
          if(m_cstrMI.IsEmpty() == false)
            cstrName += " ";
          m_cstrLastName = m_cstrDataPathFile.Mid(iIndex, iIndex1 - iIndex);
          cstrName += m_cstrLastName;
          }
        }
      }
    }
  return(cstrName);
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

  m_btTrailer.uStartWord1 = 0xFFFF;
  m_btTrailer.uStartWord2 = 0xFFFF;
  m_btTrailer.uStartWord3 = 0xFFFF;
  m_btTrailer.uStartWord4 = 0xFFFF;
  m_btTrailer.uEndWord1 = 0xFFFF;
  m_btTrailer.uEndWord2 = 0xFFFF;
  m_btTrailer.uEndWord3 = 0xFFFF;
  m_btTrailer.uEndWord3 = 0xFFFF;
  m_btTrailer.lDataSamples = m_lTotalDataPoints;
  m_btTrailer.lSpare = 0;
  // Open the file for appending and write the baseline trailer.
  m_cstrErrMsg = "";
  iRet = SUCCESS;
  try
    { // Open the file.
    if(m_cfDataFile.Open(m_cstrDataPathFile, CFile::modeWrite | CFile::shareDenyWrite, &FileEx)
      == FALSE)
      {
      iRet = FAIL;
      m_cstrErrMsg.Format(_T(IDS_ERR_FILE_OPEN), m_cstrDataPathFile,
        strerror(FileEx.m_lOsError));
      }
    else
      {
      m_cfDataFile.Seek(0, CFile::end); // Go to end of file.
       // Append the baseline trailer.
      m_cfDataFile.Write(&m_btTrailer, sizeof(BASELINE_TRAILER));
      }
    m_cfDataFile.Close();
    }
  catch(CFileException *pfe)
    { // Got a file open error.
    m_cstrErrMsg.Format(_T(IDS_ERR_FILE_OPEN), m_cstrDataPathFile,
      strerror(pfe->m_lOsError));
    pfe->Delete();
    iRet = FAIL;
    }
  return(iRet);
  }

/********************************************************************
cstrGet_data_file_type - Return the file type from the file header.

  inputs: None.

  return: CString containing the file type.
********************************************************************/
CString CDataFile::cstrGet_data_file_type()
  {
  CString cstrType;

  cstrType = m_dhfHdr.szProgName;
  return(cstrType);
  }

/********************************************************************
uiGet_data_file_version - Return the file version from the file header.

  inputs: None.

  return: File version.
********************************************************************/
short unsigned int CDataFile::uiGet_data_file_version()
  {
  return(m_dhfHdr.uiDataFileVer);
  }