/********************************************************************
InstallEGGSASDlg.h

Copyright (C) 2010,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CInstallEGGSASDlg class, the main dialog box.

HISTORY:
17-SEP-10  RET  New.
18-SEP-12  RET
             Change to allow 3 tries to enter the correct password.
               Add define: MAX_TRIES
               Add class variable: m_iNumTries
********************************************************************/

#pragma once

// Maximum number of tries to get the password correct.
#define MAX_TRIES 3

// CInstallEGGSASDlg dialog
class CInstallEGGSASDlg : public CDialog
{
// Construction
public:
	CInstallEGGSASDlg(CWnd* pParent = NULL);	// standard constructor
  ~CInstallEGGSASDlg(void);

// Dialog Data
	enum { IDD = IDD_INSTALLEGGSAS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
public: // Data
    CSecureInstall *m_psi;
  	CString m_strPath;
    // Number of tries to get the password correct.
    int m_iNumTries;

    // Methods
    //bool bFind_file();
    void vCreate_ok_file(bool bOk);

protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedCancel();
//  afx_msg void OnDestroy();
  CString m_cstrPassword;
};
