/********************************************************************
dbupdateDlg.h

Copyright (C) 2005, 3CPM Company, Inc.  All rights reserved.

  Header file for the CdbupdateDlg class.

HISTORY:
28-FEB-05  RET  New.
********************************************************************/
// dbupdateDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CdbupdateDlg dialog
class CdbupdateDlg : public CDialog
{
// Construction
public:
	CdbupdateDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DBUPDATE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
public: // Data
  CDbaccess *m_pDB;

protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  CListBox m_clbUpdateStatus;
  int iUpdate_database(void);
  void vUpdate_status_list(LPCTSTR pstrMsg, bool bLog);
};
