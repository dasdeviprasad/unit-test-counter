/********************************************************************
DBAccessUpd.h

Copyright (C) 2019 3CPM Company, Inc.  All rights reserved.

  Header file for the CDBAccessUpd class.

HISTORY:
09-FEB-19  RET
             New
********************************************************************/
#pragma once

#include "SQL.h"
#include "SQLExt.h"
#include <odbcss.h>
#include "atltime.h"

#define DB_VERSION_MAJOR 2
#define DB_VERSION_MINOR 6 //3

class CDBAccessUpd
  {
  public:
    CDBAccessUpd(CString cstrUpdDB);
    ~CDBAccessUpd(void);
    CString m_cstrDSN;
    bool m_bDBConnected;
    CString m_cstrErr;
    CString m_cstrInfo;
    short iDBConnect(void);
    SQLRETURN sqlrDB_connect(void);
    SQLRETURN sqlrDB_disconnect(void);
    void vFree_env_handle(void);
    void vFree_db_conn_handle(void);
    SQLHANDLE sqlhGet_stmt_handle(void);
    SQLRETURN sqlrFree_stmt_handle(SQLHANDLE sqlhStmt);
    CString cstrGet_db_name(void);
    CString cstrGet_db_path(void);
    short int iGet_string(SQLHSTMT sqlhStmt, short int iColumn, CString &cstrData);
    short int iGet_bool(SQLHSTMT sqlhStmt, short iColumn, BOOL * pbValue);
    short iReport_err(SQLHANDLE sqlhHandle, bool bDisplayMsgBox);
    short iUpdate_patient_100_101(void);
    short iUpdate_patient_202_203(void);
    short iUpdate_patient_203_204(void);
    short iAdd_diag_key_phrase_desc(char * pszKeyPhrase, char * pszDesc);
    short iAdd_ICD_code(char * pszCode);
    short iUpdate_patient_204_205(void);
    short iUpdate_patient_205_206(void);
    int iUpdate_study_100_101(void);
    int iUpdate_study_101_200(void);
    int iUpdate_study_201_202(void);
    short iAdd_event_table_201(void);
    short iAdd_whatif_table_202(void);
    short iGet_version(CString & cstrVersion);
    short iGet_major_version(CString cstrVersion);
    short iGet_minor_version(CString cstrVersion);
    int iUpdate_version(void);
  };
