/********************************************************************
SearchDB.h

Copyright (C) 2018 3CPM Company, Inc.  All rights reserved.

  Header file for the CSearchDB class.

  This class provides for entering search criteria for searching the database.


HISTORY:
19-MAY-18  RET  New.
********************************************************************/

#pragma once

#define MONTH_ENTRY 1
#define DAY_ENTRY 2
#define YEAR_ENTRY 3


// CSearchDB dialog

class CSearchDB : public CDialog
{
	DECLARE_DYNAMIC(CSearchDB)

public:
	CSearchDB(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSearchDB();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedChkLastName();
	afx_msg void OnBnClickedChkRrnum();
	afx_msg void OnBnClickedChkNameAsc();
	afx_msg void OnBnClickedChkNameDesc();
	afx_msg void OnBnClickedChkDateOlder();
	afx_msg void OnBnClickedChkDateNewer();
	afx_msg void OnBnClickedChkDateEqual();
	afx_msg void OnEnChangeEditMonth();
	afx_msg void OnEnChangeEditDay();
	afx_msg void OnEnChangeEditYear();
	//bool m_bLastName;
	//bool m_bResRefNum;
	//bool m_bNameAsc;
	//bool m_bNameDesc;
	//CString m_cstrName;
	//bool m_bDatesOlder;
	//bool m_bDatesNewer;
	//bool m_bDatesEqual;
	//CString m_cstrMonth;
	//CString m_cstrDay;
	//CString m_cstrYear;
  CString m_cstrSQL;
  int m_iDateSrchType;
  CString m_cstrDate;
	void vValidate_date_entry(int iEntryType, CString & cstrItem, int iLen);
	void vEnable_date_fields(void);
	BOOL m_bLastName;
	BOOL m_bResRefNum;
	BOOL m_bNameAsc;
	BOOL m_bNameDesc;
	CString m_cstrName;
	BOOL m_bDatesOlder;
	BOOL m_bDatesNewer;
	BOOL m_bDatesEqual;
	CString m_cstrMonth;
	CString m_cstrDay;
	CString m_cstrYear;
};
