/********************************************************************
ImportRemoteDB.h

Copyright (C) 2008,2009,2012,2018, 3CPM Company, Inc.  All rights reserved.

  Header file for the CImportRemoteDB class.

HISTORY:
12-NOV-08  RET  New.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
28-MAY-18  RET
             Add ability to rename an imported patient that already exists in the database.
               Add defines: IMPORT_DUP_SKIP, IMPORT_DUP_OVERWRITE, IMPORT_DUP_CHANGE
********************************************************************/

#pragma once
#include "afxcmn.h"

// Returns for the iCopy_file() method
#define FILE_COPY_FAIL 0
#define FILE_COPY_NEW 1
#define FILE_COPY_OVERWRITTEN 2
#define FILE_COPY_NOT 3

#define IMPORT_DUP_SKIP 0
#define IMPORT_DUP_OVERWRITE 1
#define IMPORT_DUP_CHANGE 2

// CImportRemoteDB dialog

class CImportRemoteDB : public CDialog
{
	DECLARE_DYNAMIC(CImportRemoteDB)

public:
	CImportRemoteDB(CEGGSASDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CImportRemoteDB();

// Dialog Data
	enum { IDD = IDD_IMPORT_REMOTE_DB };

  public: // Data
    CEGGSASDoc *m_pDoc; // Pointer to the document that started this dialog box.
    CString m_cstrCurrentDBPath;
    CDbaccess *m_pImpDB;

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
    CString cstrGet_data_file_name(CString cstrFilePath);
    void vFill_patient_listbox();
    CString cstrFormat_name(FIND_PATIENT_INFO *pfpiData);
    int iSetup_databases();
    void vSet_text(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  CString m_cstrProgressTitle;
  afx_msg void OnBnClickedBtnBrowse();
  CString m_cstrDBLocation;
  virtual BOOL OnInitDialog();
  CProgressCtrl m_cImportProgressBar;
  afx_msg void OnBnClickedImportRemoteDbHelp();
  CListCtrl m_cImportList;
  afx_msg void OnClose();
  afx_msg void OnBnClickedBtnImport();
  afx_msg void OnBnClickedBtnSelectAll();
  afx_msg void OnBnClickedOk();
};
