/********************************************************************
MakeSecureDataFileDlg.h

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Header file for the CMakeSecureDataFileDlg class, the main dialog box.

HISTORY:
17-SEP-10  RET  New.
********************************************************************/

#pragma once
#include "afxwin.h"


// CMakeSecureDataFileDlg dialog
class CMakeSecureDataFileDlg : public CDialog
{
// Construction
public:
	CMakeSecureDataFileDlg(CWnd* pParent = NULL);	// standard constructor
  ~CMakeSecureDataFileDlg(void);

// Dialog Data
	enum { IDD = IDD_MAKESECUREDATAFILE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

public:
  CSecureInstall *m_psi;
  bool m_bRestoreFile;

  void vCreate_secure_file(void);
  void vAdd_to_result_list(CString cstrMsg);
  void vRestore_file(bool bAddToList);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
  afx_msg void OnBnClickedBtnCreateBackup();
  afx_msg void OnBnClickedBtnTest();
  afx_msg void OnBnClickedBtnRestoreBackup();
  CListBox m_cResultsList;
  afx_msg void OnBnClickedCancel();
  afx_msg void OnBnClickedBtnCreateDataFile();
  CString m_cstrPassword;
};
