/********************************************************************
Settings.h

Copyright (C) 2012, 2020, 3CPM Company, Inc.  All rights reserved.

  Header file for the CSettings class.


HISTORY:
23-FEB-12  RET  New
14-NOV-20  RET
             Add ability to work with a remote database.
             Changed settings to allow the user to specify the remote database.
             Add class variables: m_cstrRemoteDBPasswd, m_cstrRemoteDBPath, m_bSetRemoteDBPath
             Add method prototype: OnBnClickedRemoteDbLocBrowse()
********************************************************************/

#pragma once
#include "afxwin.h"

// CSettings dialog

class CSettings : public CDialog
{
	DECLARE_DYNAMIC(CSettings)

public:
	CSettings(CEGGSASDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettings();

  // Data
  CString m_cstrDataFilePath; 
  bool m_bSetDataFilePath;
  CEGGSASDoc *m_pDoc;
  bool m_bSetRemoteDBPath;

  // Methods
  void vSet_text(void);
  //CString cstrGet_data_file_path(void);
	//static int __stdcall BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

// Dialog Data
	enum { IDD = IDD_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedDataFileLocBrowse();
  afx_msg void OnBnClickedOk();
  virtual BOOL OnInitDialog();
  CComboBox m_cLanguageList;
  CString m_cstrDataFileLoc;
  afx_msg void OnBnClickedSettingsHelp();
  CString m_cstrRemoteDBPasswd;
  CString m_cstrRemoteDBPath;
  afx_msg void OnBnClickedRemoteDbLocBrowse();
};
