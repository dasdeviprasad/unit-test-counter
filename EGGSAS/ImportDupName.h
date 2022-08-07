/********************************************************************
ImportDupName.h

Copyright (C) 2018 3CPM Company, Inc.  All rights reserved.

  Header file for the CImportDupName class.

  This class provides for importing a patient that already exists in the database under
  a different name or research reference number.


HISTORY:
28-MAY-18  RET  New.
********************************************************************/

#pragma once


// CImportDupName dialog

class CImportDupName : public CDialog
{
	DECLARE_DYNAMIC(CImportDupName)

public:
	CImportDupName(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImportDupName();

// Dialog Data
	enum { IDD = IDD_IMPORT_DUP_NAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  //CString m_cstrName;
  CString m_cstrFullName;
  afx_msg void OnBnClickedRadioSkip();
  afx_msg void OnBnClickedRadioOverwrite();
  afx_msg void OnBnClickedRadioChangeName();
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedCancel();
  virtual BOOL OnInitDialog();
  // true if using research reference number, false if using name.
  BOOL m_bUseResrchRefNum;
  void vSet_text(void);
  CString m_cstrName;
  int m_iDupAction;
};
