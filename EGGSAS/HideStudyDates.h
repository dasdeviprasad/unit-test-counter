/********************************************************************
HideStudyDates.h

Copyright (C) 2016 3CPM Company, Inc.  All rights reserved.

  Header file for the CHideStudyDates class.

HISTORY:
06-JUN-16  RET  New.
********************************************************************/

#pragma once


// CHideStudyDates dialog

class CHideStudyDates : public CDialog
{
	DECLARE_DYNAMIC(CHideStudyDates)

public:
	CHideStudyDates(CEGGSASDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CHideStudyDates();

// Dialog Data
	enum { IDD = IDD_HIDE_STUDY_DATE };

  CEGGSASDoc *m_pDoc;
  bool m_bResetPassword;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedOk();
  void vMove_new_password_fields(void);
  afx_msg void OnBnClickedButtonNewPassword();
  afx_msg void OnBnClickedButtonResetPassword();
  afx_msg void OnBnClickedButtonNext();
  bool bValidate_reset_password(void);
  void vSet_text(void);
  CString m_cstrPassword; // Password entered into the password field.
  CString m_cstrNewPassword; // Password entered into the new password field.
  CString m_cstrNewPassword1; // Password entered into the re-enter new password field.
  bool m_bNewPassword; // true indicates this is a new password.
  CString m_cstrHideShowState; // Text displayed in the status box at the top of the form.
  bool m_bFirstPassword;
};
