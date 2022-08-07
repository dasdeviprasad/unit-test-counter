/********************************************************************
SelectStudyType.h

Copyright (C) 2007,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CSelectStudyType class.

HISTORY:
28-JUL-07  RET  New.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#pragma once


// CSelectStudyType dialog

class CSelectStudyType : public CDialog
{
	DECLARE_DYNAMIC(CSelectStudyType)

public:
	CSelectStudyType(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectStudyType();

  ePgmType ptGet_study_type();
  void vSet_text(void);

// Dialog Data
	enum { IDD = IDD_SELECT_STUDY_TYPE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  BOOL m_bStandard;
  BOOL m_bResearch;
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedChkStandard();
  afx_msg void OnBnClickedChkResearch();
};
