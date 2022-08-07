/********************************************************************
DataFileViewerDlg.h

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Header file for the CDataFileViewerDlg class, the main dialog box.


HISTORY:
25-JUN-10  RET  New.
10-AUG-10  RET  Version 1.1
                  Add class variable m_cstrDataFileTypeVers.
********************************************************************/

#pragma once
#include "afxwin.h"


// CDataFileViewerDlg dialog
class CDataFileViewerDlg : public CDialog
{
// Construction
public:
	CDataFileViewerDlg(CWnd* pParent = NULL);	// standard constructor
  virtual ~CDataFileViewerDlg();

// Dialog Data
	enum { IDD = IDD_DATAFILEVIEWER_DIALOG };

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

public: // Data
  CDataFile *m_pdf; // Pointer to the CDataFile class.
  CUtils *m_pUtils; // Pointer to the utility class.

public:
  afx_msg void OnBnClickedBtnSelDataFile();
  afx_msg void OnBnClickedBtnReadFile();
  CString m_cstrDataFile;
  void vDisplay_data(void);
  CString cstrDate_of_study(void);
  CString cstrGet_name(void);
  CString m_cstrPatientName;
  CString m_cstrDateOfStudy;
  CButton m_chkWaterLoadStudy;
  CButton m_chkResearchStudy;
  void vGet_db_info(unsigned short uNumPeriods, unsigned short *puBaselineMin,
    unsigned short *puPostStimMin);
  CString m_cstrStatus;
  CString m_cstrBaseLineMinutes;
  CString m_cstrPostStimMinutes;
  CString m_cstrDataFileTypeVers;
};
