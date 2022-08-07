/********************************************************************
Settings.h

Copyright (C) 2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CSettings class.


HISTORY:
23-FEB-12  RET  New
********************************************************************/

#pragma once
#include "afxwin.h"

//UINT_PTR CALLBACK OFNHookProcOldStyle(          HWND hdlg,
//    UINT uiMsg,
//    WPARAM wParam,
//    LPARAM lParam
//);
//

// CSettings dialog

class CSettings : public CDialog
{
	DECLARE_DYNAMIC(CSettings)

public:
	CSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettings();

  // Data
  CString m_cstrDataFilePath; 
  bool m_bSetDataFilePath;

  // Methods
  void vSet_text(void);
  CString cstrGet_data_file_path(void);
	static int __stdcall BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

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
};
