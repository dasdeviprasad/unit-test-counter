/********************************************************************
Dbaccess.h

Copyright (C) 2005,2007,2008,2012,2019, 3CPM Company, Inc.  All rights reserved.

  Header file for the CDbaccess class.

HISTORY:
28-FEB-05  RET  New.
07-FEB-08  RET  Version 2.02 of the database.
                  Add prototypes for new methods.
29-AUG-08  RET  Version 2.03 of the database.
                  Add prototypes for new methods.
02-SEP-12  RET  Version 2.04 of the database.
                  Add prototypes for new methods.
13-SEP-12  RET  Version 2.04 of the database.
                  Add prototypes for new methods.
27-FEB-13  RET  Version 2.05 of the database.
                  Add prototypes for new methods.
16-MAR-13  RET  Version 2.06 of the database.
                  Add prototypes for new methods.
22-JAN-19  RET  DB Ver 2.06, product ver 1.6
                  Add ability to specify a different database on the  command line.
                    Add class variable: m_bStdDB
********************************************************************/

#if !defined(AFX_DBACCESS_H__FE294E81_6E1A_4F67_B73E_992EB6AF9BEE__INCLUDED_)
#define AFX_DBACCESS_H__FE294E81_6E1A_4F67_B73E_992EB6AF9BEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SQL.h"
#include "SQLExt.h"
#include <odbcss.h>

// Default database DSN
#define DSN_DEFAULT "3CPM EGGSAS"

class CDbaccess  
{
public:
	CDbaccess();
	virtual ~CDbaccess();

public: // Data
  CString m_cstrDSN; // DSN for the database.
  bool m_bDBConnected; // true - database is connected.
  CString m_cstrErr; // Holds the error message.
  CString m_cstrInfo; // Additional information about the database action.
  bool m_bStdDB; // true to update the standard database.
                       // false to update a database from the command line.

public: // Methods
  void vSet_DSN(CString cstrDSN);
    // SQL methods
  SQLRETURN sqlrDB_connect(void);
  SQLRETURN sqlrDB_disconnect(void);
  short int iReport_err(SQLHANDLE sqlhHandle, bool bDisplayMsgBox);
  SQLHANDLE sqlhGet_stmt_handle(void);
  SQLRETURN sqlrFree_stmt_handle(SQLHANDLE sqlhStmt);
  CString cstrGet_db_path(void);
  short int iGet_string(SQLHSTMT sqlhStmt, short int iColumn, CString &cstrData);
  short int iGet_bool(SQLHSTMT sqlhStmt, short int iColumn, BOOL *pbValue);

  CString cstrGet_db_name(void);
  short iDBConnect(void);
  void vFree_env_handle(void);
  void vFree_db_conn_handle(void);

    // Patient table mathods
  short int CDbaccess::iUpdate_patient_100_101();
  short int iUpdate_patient_202_203();

    // PatientInfo table mathods

    // StudyData table mathods
  int iUpdate_study_101_200(void);
  int iUpdate_study_201_202(void);

    // Facility table methods

    // Events table functions
  short int iAdd_event_table_201(void);

    // WhatIf table functions
  short int iAdd_whatif_table_202(void);

    // Version table functions
  short int CDbaccess::iGet_version(CString &cstrVersion);
  short int iGet_major_version(CString cstrVersion);
  short int iGet_minor_version(CString cstrVersion);

  int iUpdate_study_100_101(void);
  int iUpdate_version(void);
  short int iUpdate_patient_203_204();
  short int iAdd_diag_key_phrase_desc(char *pszKeyPhrase, char *pszDesc);
  short int iAdd_ICD_code(char *pszCode);
  short int iUpdate_patient_204_205();
  short int iUpdate_patient_205_206();

};

#endif // !defined(AFX_DBACCESS_H__FE294E81_6E1A_4F67_B73E_992EB6AF9BEE__INCLUDED_)
