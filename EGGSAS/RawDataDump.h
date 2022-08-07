/********************************************************************
RawDataDump.h

Copyright (C) 2011, 3CPM Company, Inc.  All rights reserved.

  Header file for the CRawDataDump class.

HISTORY:
27-SEP-11  RET  New.
********************************************************************/

#pragma once

class CRawDataDump
  {
  public:
    CRawDataDump(CEGGSASDoc *pDoc);
    virtual ~CRawDataDump(void);

  public: // Data
    CEGGSASDoc *m_pDoc; // Pointer to the active document.

  public: // Methods
    void vSave_ascii_data_file();
    short int iWrite_to_ascii_data_file(CFile *pcfFile, CString cstrLine);
    short int iWrite_to_ascii_data(CFile *pcfFile, float *pfData,
                                   unsigned uPoints, bool bEGG);
    CString cstrSelect_raw_data_file_location(CString cstrFile);
  bool bShow_save_as_dlgbox(CString cstrFile, CString &cstrSelFile);

  };
