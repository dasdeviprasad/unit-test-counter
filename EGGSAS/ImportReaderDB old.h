/********************************************************************
ImportReaderDB.h

Copyright (C) 2008, 3CPM Company, Inc.  All rights reserved.

  Header file for the CImportReaderDB class.

HISTORY:
12-NOV-08  RET  New.
********************************************************************/

#pragma once

// Returns for the iCopy_file() method
#define FILE_COPY_FAIL 0
#define FILE_COPY_NEW 1
#define FILE_COPY_OVERWRITTEN 2
#define FILE_COPY_NOT 3

class CImportReaderDB
  {
  public:
    CImportReaderDB(CEGGSASDoc *pDoc);
    virtual ~CImportReaderDB(void);

  public: // Data
    CEGGSASDoc *m_pDoc;

  public: // Methods
    void vImport_reader_db();
    int iImport_db(CDbaccess *pDBLocal, CDbaccess *pDBRemote);
    short int iCopy_file(LPCTSTR strSrc, LPCTSTR strDest, CString cstrOverwriteInfo);
    short int iImport_whatif(long lStudyIDLocal, long lStudyIDRemote,
                             CDbaccess *pDBLocal, CDbaccess *pDBRemote,
                             short int iStudyType);
    short int iImport_events(long lStudyIDLocal, long lStudyIDRemote,
                             CDbaccess *pDBLocal, CDbaccess *pDBRemote, 
                             short int iStudyType);
  };
