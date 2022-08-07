/********************************************************************
HwInitExpDateGenDlg.h

Copyright (C) 2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the implementation file that displays the main dialog box.

HISTORY:
07-JAN-12  RET  New.
********************************************************************/

#pragma once
#include "afxdtctl.h"

// defines for the GUIDs of the Expiration Date File.
#define LINE1 "C2A83660-158E-4aa4-86CA-B3298D768FA1\r\n"
#define LINE2 "BE803A39-4091-43ce-858E-1E60209AC8E3\r\n"
#define LINE3 "70643ECE-D801-4c61-8EBA-BA6387D3F34C\r\n"
#define LINE4 "21521C01-5FF5-4fd3-94E5-7E7EB409FEB5\r\n"

/////////////////////////////////////////////////////////////////////
// Indexes for storing the expiration date in the expiration date file.
// Not these indexes they are Dashes: 8, 13, 18, 23
// Last index: 35
// Line number for the item.
#define MONTH1_LINE 3
#define MONTH2_LINE 1
#define DAY1_LINE 4
#define DAY2_LINE 2
#define YEAR1_LINE 1
#define YEAR2_LINE 3
#define YEAR3_LINE 2
#define YEAR4_LINE 4
#define NUM_TIMES_TO_RUN1_LINE 2
#define NUM_TIMES_TO_RUN2_LINE 1
#define NUM_TIMES_TO_RUN3_LINE 4
// Indexes for the line the item goes on.
#define MONTH1_IDX 17
#define MONTH2_IDX 33
#define DAY1_IDX 4
#define DAY2_IDX 11
#define YEAR1_IDX 3
#define YEAR2_IDX 27
#define YEAR3_IDX 29
#define YEAR4_IDX 14
#define NUM_TIMES_TO_RUN1_IDX 15
#define NUM_TIMES_TO_RUN2_IDX 20
#define NUM_TIMES_TO_RUN3_IDX 31

// Indexes used:
// Line 1: 3, 33, 20
// Line 2: 11, 29, 15
// Line 3: 17, 27
// Line 4: 4, 14, 31
/////////////////////////////////////////////////////////////////////

// Value to indicate that the number of times to run is not used.
#define NUM_TIMES_TO_RUN_NOT_USED 0xFFF

// Expiration date file name.
#define EXP_DATE_FILE_NAME "HWINIT.TXT"


// CHwInitExpDateGenDlg dialog
class CHwInitExpDateGenDlg : public CDialog
{
// Construction
public:
	CHwInitExpDateGenDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_HWINITEXPDATEGEN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
  // Data
  CString m_cstrLine1; // Line 1 of the output file
  CString m_cstrLine2; // Line 2 of the output file
  CString m_cstrLine3; // Line 3 of the output file
  CString m_cstrLine4; // Line 4 of the output file

  // Methods
  void vSet_font(CWnd *pwnd, CDC *pDC, CFont *pcfFont, long lFontWeight, long lFontSize = 0);
  long lCalc_font_height(int iPointSize, CDC *pcdc);
  void vCreate_file(void);
  void vEncode_item(int iLine, int iLineIndex, char cItem);
  //bool bValidate_date(CString cstrDate);
  int iDays_in_month(int iMonth, int iYear);

public:
  afx_msg void OnBnClickedOk();

  int m_iNumTimesToRun; // Filled in by the IDC_EDIT_TIMES_TO_RUN field.
  CDateTimeCtrl c_dtcExpDate;
};
