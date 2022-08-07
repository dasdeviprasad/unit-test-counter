/********************************************************************
WaterLoadInfo.h

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Header file for the WaterLoad information dialog box.

HISTORY:
28-JUN-10  RET  New.
********************************************************************/

#pragma once

// CWaterLoadInfo dialog

class CWaterLoadInfo : public CDialog
{
	DECLARE_DYNAMIC(CWaterLoadInfo)

public:
	CWaterLoadInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWaterLoadInfo();

  UINT m_uTotalMin; // Total minutes in the study, set by parent.

// Dialog Data
	enum { IDD = IDD_WATERLOAD_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedCancel();
  UINT m_uBaselineMin;
  UINT m_uPostWaterMin;
  virtual BOOL OnInitDialog();
  UINT m_uTotalMinutes;
};
