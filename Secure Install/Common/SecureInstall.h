/********************************************************************
SecureInstall.h

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Header file for the CSecureInstall class.

HISTORY:
10-AUG-10  RET  New.
17-SEP-10  RET
                Add class variable m_cstrDataFile.
********************************************************************/

#pragma once

// File version.  Goes in line 0 positions 0 and 1. Can go up to 255.
#define SECURE_FILE_VERSION 1

// Secure data file name.
#define SECURE_INSTALL_FILE "ValidateInstall.txt"
// Name of secure data file backup.
#define SECURE_INSTALL_FILE_BACKUP "ValidateInstall.bak"
// Password.
#define INSTALL_PASSWORD "3cpm"

// Size of the class array to hold the contents of the data file.
#define DATA_ARRAY_SIZE_1 6   // size of the first dimension of the data array.
#define DATA_ARRAY_SIZE_2 50  // size of the second dimension of the data array.

#include "..\common\Encrypt.h"

class CSecureInstall
  {
  friend class CEncrypt;

  public:
    CSecureInstall(void);
    virtual ~CSecureInstall(void);

  public: // Methods
    bool bRead_file();
    bool bWrite_file();
    bool bValidate_password(CString cstrPassword);
    bool bCreate_secure_file();
    bool bCreate_guid(CString &cstrGUID);
    bool bChange_password();
    void vSet_file_path(CString cstrPath);

  public: // Data
    CString m_cstrErr; // Contains any error message.  Empty if no errors.
    // Array that holds the contents of the secure data file.
    char m_szaGUID[DATA_ARRAY_SIZE_1][DATA_ARRAY_SIZE_2];
    CEncrypt *m_pce; // Pointer to the encrypt/decrypt class.
    CString m_cstrDataFile; // Path and file name to the secure data file.

  };
