/********************************************************************
Settings.cpp

Copyright (C) 2008 - 2020, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CSettings class.

  This class provides for reading/writing a Settings file containing
  program data to be remembered between executions.  This is similar
  to the INI file.

  Implementation:
    - The file is formatted with one ID/value pair per line as follows:
        ID_ITEM1=value item 1
          Where:
            ID_ITEM1 is the ID for the first item in the file.
            The value is treated a string and is to the right of the '=' sign.
    - When the file is read, the IDs and values are stored in matching string
      arrays.

HISTORY:
03-NOV-08  RET  New.
Version 1.02
  07-DEC-08  RET  Change the constructor parameter to CString.
27-NOV-20  RET
            Changes to allow using a remote database.
						  Change  method: cstrGet_item()
********************************************************************/

#include "StdAfx.h"
#include "StudyExport.h"
#include "Settings.h"

/********************************************************************
Constructor - Initialize ID and data arrays, read in the settings file.

inputs: File name and path of settings file.
********************************************************************/
CSettings::CSettings(CString cstrFile)
{

  m_cstrFile = cstrFile;
    // Copy the CString object into a character array so fopen()
    // can use it.
  CW2A pszA((LPCTSTR)cstrFile);
  strcpy(m_szFile, pszA);
  m_cstraData.SetSize(10, 10); // String array for values
  m_cstraDataID.SetSize(10, 10); // String array for IDs
  iRead();
}

/********************************************************************
Destructor - Do nothing.
********************************************************************/
CSettings::~CSettings(void)
{
}

/********************************************************************
iRead - Read in the settings file and store the IDs/values in class arrays.

inputs: None.

return: 1 if settings file is read.
        0 if the file can't be opened.
********************************************************************/
int CSettings::iRead(void)
{
  int iRet = 0;
  FILE *pfFile;
  char szInBuf[100];
  CString cstrLine;
  int iIndex, iCnt;

  pfFile = fopen((const char *)m_szFile, "r");
  if(pfFile != NULL)
  {
    iCnt = 0;
    while(fgets(szInBuf, 100, pfFile) != NULL)
    {
      if(szInBuf[0] != '[')
      { // Setting item, not a section.
        // Got file line, parse it into ID and data.
        cstrLine = szInBuf;
        if((iIndex = cstrLine.Find('=')) > 0)
        {
          m_cstraDataID[iCnt] = cstrLine.Left(iIndex);
          cstrLine.Delete(0, iIndex + 1);
          cstrLine.Trim(_T("\n\r"));
          m_cstraData[iCnt] = cstrLine; //.Right(cstrLine.GetLength() - iIndex);
          ++iCnt;
        }
      }
      else
      { // Save the section name.
        m_cstrSectionName = szInBuf;
        m_cstrSectionName.Trim(_T("\n\r"));
      }
    }
    m_cstraDataID.SetSize(iCnt, 10);
    m_cstraData.SetSize(iCnt, 10);
    fclose(pfFile);
    iRet = 1;
  }

  return(iRet);
}

/********************************************************************
iWrite - Write the IDs/values to the settings file.

inputs: None.

return: 1 if settings file is written.
        0 if the file can't be opened.
********************************************************************/
int CSettings::iWrite(void)
{
  int iRet = 0;
  FILE *pfFile;
  CString cstrLine;
  int iCnt;

  pfFile = fopen((const char *)m_szFile, "w");
  if(pfFile != NULL)
  {
    // first write the section name.
    cstrLine = m_cstrSectionName;
    CW2A pszA((LPCTSTR)cstrLine);
    fputs(pszA, pfFile);
    fputs("\n", pfFile);
    for(iCnt = 0; iCnt < m_cstraDataID.GetCount(); ++iCnt)
    {
      cstrLine.Format(_T("%s=%s"), m_cstraDataID[iCnt], m_cstraData[iCnt]);
      CW2A pszA((LPCTSTR)cstrLine);
      fputs(pszA, pfFile);
      fputs("\n", pfFile);
    }
    fclose(pfFile);
    iRet = 1;
  }
  return(iRet);
}

/********************************************************************
cstrGet_item - Get the value for the specified ID.

inputs: ID of item to get value for.

return: CString object containing the value.
        Empty CString object if the value can't be found.
********************************************************************/
CString CSettings::cstrGet_item(LPCTSTR pstrItem)
{
  int iCnt, iIndex;
  CString cstrValue, cstrTemp;
	wchar_t sz[2] = _T("\\");

  cstrValue.Empty();
  for(iCnt = 0; iCnt < m_cstraDataID.GetCount(); ++iCnt)
  {
    if(m_cstraDataID[iCnt] == pstrItem)
    {
      cstrValue = m_cstraData[iCnt];
			if(wcscmp(pstrItem, SETT_ITEM_DATA_PATH) == 0)
			{
				if(cstrValue.Find(_T(".mdb")) > 0)
				{ // Have file name in path, remove it.
					cstrTemp = _T("\\");
					if((iIndex = cstrValue.ReverseFind(_T('\\'))) > 0)
					{
						cstrValue = cstrValue.Left(iIndex);
					}
				}
			}
      break;
    }
  }
  return(cstrValue);
}

/********************************************************************
vWrite_string_item - Save the value for the specified ID and write all the
                     settings to the settings file.

                     The ID must alread exist.

inputs: ID of item to get value for.
        Value for the item.

return: None.
********************************************************************/
void CSettings::vWrite_string_item(LPCTSTR pstrItem, LPCTSTR pstrData)
{
  int iCnt;

  // Find the item in the array and update its value.
  for(iCnt = 0; iCnt < m_cstraDataID.GetCount(); ++iCnt)
  {
    if(m_cstraDataID[iCnt] == pstrItem)
    {
      m_cstraData[iCnt] = pstrData;
      break;
    }
  }
  // Write the file.
  iWrite();
}

