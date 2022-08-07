/********************************************************************
BckupRest.h

Copyright (C) 2004,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CBckupRest class.

HISTORY:
20-JAN-04  RET  New.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#pragma once
#include "afxcmn.h"

#define BACKUP 1
#define RESTORE 2

class CBckupRest : public CDialog
{
	DECLARE_DYNAMIC(CBckupRest)

public:
	CBckupRest(CEGGSASDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CBckupRest();

// Dialog Data
	enum { IDD = IDD_BCKUP_REST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedBackup();
  afx_msg void OnBnClickedRestore();
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedStart();
  virtual BOOL OnInitDialog();
    // Field Data
  CProgressCtrl m_cProgress;    // Progress bar control
  CString m_cstrBckupRestPath;  // User selected path for backup or restore
  CString m_cstrToFrom;         // Title for Backup/restore path field.
  CString m_cstrFileCopied;     // File currently being copied.
    // class data
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  CString m_cstrProgress; // Title for the progress bar (backup or restore)
  CString m_cstrDestPath; // Destination path.
  CString m_cstrSrcPath; // Source path.
  int m_iBckupRest; // Indicates if doing backup or restore.
  CStringArray m_cstraErrs;
    // Methods
  int iBackup_restore(void);
  void vReport_sts(int iSts);
  int iCopy_egg_files(void);
  void vSetup_progress_bar(void);
  bool bValidate_file(char *pszName, CString cstrPath);
  bool bCheck_data_files();
  bool bCheck_bckup_data_files();
  void vSet_text(void);

  afx_msg void OnBnClickedBckuprestHelp();
};
