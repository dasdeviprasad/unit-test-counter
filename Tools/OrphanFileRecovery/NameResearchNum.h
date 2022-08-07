/********************************************************************
NameResearchNum.h

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Header file for the Patient Name/Research Reference Number dialog box.

HISTORY:
01-JUL-10  RET  New.
********************************************************************/

#pragma once

// CNameResearchNum dialog

class CNameResearchNum : public CDialog
{
	DECLARE_DYNAMIC(CNameResearchNum)

public:
	CNameResearchNum(CString cstrStudyName, CWnd* pParent = NULL);   // standard constructor
	virtual ~CNameResearchNum();

// Dialog Data
	enum { IDD = IDD_NAME_RESEARCH_NUM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  CString m_cstrStudyName;
  BOOL m_bPatientName;
  BOOL m_bResearchRefNum;
  afx_msg void OnBnClickedChkPatientName();
  afx_msg void OnBnClickedChkResearchNum();
  afx_msg void OnBnClickedOk();
};
