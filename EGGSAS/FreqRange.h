/********************************************************************
FreqRange.h

Copyright (C) 2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CFreqRange class.

HISTORY:
03-OCT-12  RET  New.
********************************************************************/
#pragma once


// CFreqRange dialog

class CFreqRange : public CDialog
{
	DECLARE_DYNAMIC(CFreqRange)

public:
	CFreqRange(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFreqRange();

// Implementation
public: // Data
  int m_iFreqRange;

public: // Methods
  void vSet_data(int iFreqRange);
  int iGet_freq_range();
  void vSet_text(void);

// Dialog Data
	enum { IDD = IDD_FREQ_RANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedRadio15Cpm();
  afx_msg void OnBnClickedRadio60Cpm();
  afx_msg void OnBnClickedRadio200Cpm();
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedFreqRangeHelp();
};
