/********************************************************************
Settings.h

Copyright (C) 2008, 3CPM Company, Inc.  All rights reserved.

  Header file for the CSettings class.

HISTORY:
03-NOV-08  RET  New.
Version 1.02
  07-DEC-08  RET  Change the constructor parameter to CString.
********************************************************************/

#pragma once

class CSettings
{
public:
  CSettings(CString cstrFile);
  ~CSettings(void);

public: // Data
  CString m_cstrFile; // Name and path of settings file
  char m_szFile[1024]; // Name and path of settings file
  CStringArray m_cstraData; // CString array for setting values
  CStringArray m_cstraDataID; //CString array for setting IDs.
  CString m_cstrSectionName; // Section name.

public: // Methods
  int iRead(void);
  int iWrite(void);
  CString cstrGet_item(LPCTSTR pstrItem);
  void vWrite_string_item(LPCTSTR pstrItem, LPCTSTR pstrData);

};
