/********************************************************************
OrphanFileRecoveryDlg.h

Copyright (C) 2010 - 2020, 3CPM Company, Inc.  All rights reserved.

  Header file for the main dialog box class.

HISTORY:
28-JUN-10  RET  New.
23-NOV-20  RET Version 1.3
             Add ability to work with a remote database.
             Add method prototype: pdbNewDB()
             Add class variable: m_bRemoteDB
********************************************************************/

#pragma once
#include "afxcmn.h"

#define INI_SEC_GENERAL "GENERAL"
#define INI_ITEM_REMOTE_DB_PATH "RemoteDBPath" // Path of the remote database.
#define INI_FILE "C:\\3CPM\\Program\\eggsas.ini"
#define INI_FILE_CDDR "C:\\3CPM\\Central Data Depot Reader\\eggsas.ini"
#define INI_FILE_RDR "C:\\3CPM\\Reader\\eggsas.ini"


// COrphanFileRecoveryDlg dialog
class COrphanFileRecoveryDlg : public CDialog
{
// Construction
public:
	COrphanFileRecoveryDlg(CWnd* pParent = NULL);	// standard constructor
  virtual ~COrphanFileRecoveryDlg();

// Dialog Data
	enum { IDD = IDD_ORPHANFILERECOVERY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
public:
  CUtils *m_pUtils; // Pointer to the utility class.
  PATIENT_DATA m_pdPatient; // Patient data for adding to the database.
  CString m_cstrPath; // Path for data files from the INI file.
  CStringArray m_cstraFiles; // Array of orphan data files, holds file name and complete path.
  bool m_bRemoteDB; // true if using a remote database.

  void vInit_patient_data(ePgmType ptStudyType);

protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  HRESULT iFind_orphan_files(WPARAM wParam, LPARAM lParam);
  CListCtrl m_cOrphanFileList;
  afx_msg void OnBnClickedBtnAddSelToDb();
  bool bValidate_data_file(CString cstrFile);
  CDbaccess *pdbNewDB();
};
