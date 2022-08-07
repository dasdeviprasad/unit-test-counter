/********************************************************************
FinishStusy.h

Copyright (C) 2007,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CFinishStudy class.

HISTORY:
26-MAY-07  RET  New.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/
#pragma once

class CFinishStudy : public CDialog
{
	DECLARE_DYNAMIC(CFinishStudy)

public:
	CFinishStudy(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFinishStudy();

  BOOL bGet_finish_sts();
  void vSet_text(void);

// Dialog Data
	enum { IDD = IDD_FINISH_STUDY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
  BOOL m_bFinishSts;
  virtual BOOL OnInitDialog();
};
