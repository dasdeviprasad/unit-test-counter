/********************************************************************
FilterRSA.h

Copyright (C) 2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CFilterRSA class.

HISTORY:
14-DEC-12  RET  New.

********************************************************************/

#pragma once


// CFilterRSA dialog

class CFilterRSA : public CDialog
{
	DECLARE_DYNAMIC(CFilterRSA)

public:
	CFilterRSA(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFilterRSA();

// Dialog Data
	enum { IDD = IDD_FILTER_RSA };

// Implementation
public: // Data
  BOOL m_bFilter;

public: // Methods
  void vSet_text(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedCancel();
};
