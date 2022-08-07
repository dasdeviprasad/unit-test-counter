/********************************************************************
SelStudyTypeFreq.h

Copyright (C) 2013 3CPM Company, Inc.  All rights reserved.

  Header file for the CSelStudyTypeFreq class.

HISTORY:
13-MAR-13  RET  New.
********************************************************************/

#pragma once


// CSelStudyTypeFreq dialog

class CSelStudyTypeFreq : public CDialog
{
	DECLARE_DYNAMIC(CSelStudyTypeFreq)

public:
	CSelStudyTypeFreq(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelStudyTypeFreq();

// Implementation
public: // Data

public: // Methods
  unsigned short uGet_freq_range();
  unsigned short uGet_freq_range_id();
  ePgmType ptGet_study_type();
  void vSet_text(void);

// Dialog Data
	enum { IDD = IDD_SEL_STUDY_TYPE_FREQ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  BOOL m_chkStandard15;
  BOOL m_chkResearch15;
  BOOL m_chkResearch60;
  BOOL m_chkResearch200;
  afx_msg void OnBnClickedOk();
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedChkStandard15();
  afx_msg void OnBnClickedChkResearch15();
  afx_msg void OnBnClickedChkResearch60();
  afx_msg void OnBnClickedChkResearch200();
};
