/********************************************************************
SetupRemoteDBDlg.h

Copyright (C) 2020 3CPM Company, Inc.  All rights reserved.

 header file for the main dialog box

HISTORY:
31-OCT-20  RET  New.  Version 1.0.0.1
22-NOV-20  RET  Version 1.0.0.2
            Change the remote database path written to eggsas.ini to include
						the database file name.
						  Add class variable: m_cstrRemoteDBPathFile
********************************************************************/

#pragma once
#include "afxcmn.h"

//// INI file definitions
#define INI_FILE _T("C:\\3CPM\\Program\\eggsas.ini")
  // General section
#define INI_SEC_GENERAL _T("GENERAL")
#define INI_ITEM_REMOTE_DB_PATH _T("RemoteDBPath") // Path of the remote database.
#define INI_ITEM_REMOTE_DB_PASSWORD _T("RemoteDBPasswd") // Password for changing the remote database.
#define INI_ITEM_REMOTE_DB_DEF_PASSWORD _T("RemoteDBDefPasswd") // Default password for changing the remote database.
#define INI_ITEM_DATAPATH _T("datapath") // Path of the study data from the INI file.


// CSetupRemoteDBDlg dialog
class CSetupRemoteDBDlg : public CDialog
{
// Construction
public:
	CSetupRemoteDBDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SETUPREMOTEDB_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_cstrRemoteDBPath; // Remote database path.
	CString m_cstrRemoteDBFile; // Remote database file name.
	CString m_cstrRemoteDBPathFile; // Remote database Path and  file name.
	CString m_cstrPassword; // Associated with the first password edit box.
	CString m_cstrPassword2; // Associated with the second password edit box.

	afx_msg void OnBnClickedBtnSetupRemoteDb();
	afx_msg void OnBnClickedBtnSelRemoteDb2();

  void vEncode_char_in_string(wchar_t c, CString &cstrDest, int iIndex, int iIndex1);
  CString cstrCreate_guid(void);
  char cHex_to_ascii(unsigned char ucHex);


	CProgressCtrl m_ctrlProgress;
	void vUpdateProgress();
};
