/********************************************************************
StudyExportDlg.h

Copyright (C) 2008 - 2020, 3CPM Company, Inc.  All rights reserved.

  Header file for the CStudyExportDlg class, the main window for
    the application.

HISTORY:
03-NOV-08  RET  New.
Version 1.05
  27-JAN-11  RET  Add prototypes: lCalc_font_height(), vSet_font()
25-NOV-17  RET Version 1.07
             Users don't seem to know how to make multiple list box selections
						 (i.e don't know how to use Ctrl Left Click).
						 Changed the Left Click to behave as a Ctrl Left Click.
						   Add variable: m_bAllowCtrlKeyUp
30-JAN-18  RET Version 1.08
             Reworked multiple selection without using the control key.
						   Remove methods prototyes: vCtrl_key_down(), vCtrl_key_up()
						   Remove variable: m_bAllowCtrlKeyUp
27-NOV-20  RET
            Changes to allow using a remote database.
							Add method prototype: pdbNewDB()
********************************************************************/

#pragma once
#include "afxcmn.h"

#include "DBAccess.h"
#include "Settings.h"


// CStudyExportDlg dialog
class CStudyExportDlg : public CDialog
{
// Construction
public:
	CStudyExportDlg(CWnd* pParent = NULL);	// standard constructor
  virtual ~CStudyExportDlg(void);

// Dialog Data
	enum { IDD = IDD_STUDYEXPORT_DIALOG };

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
  CDBAccess *m_pDBA; // Pointer to CDBAccess object for the local database.
  CSettings *m_pSett; // Pointer to CSettings object.
  CListCtrl m_cPatientList; // Patient listbox.
  CString m_cstrDestination; // Destination path.
  _TCHAR m_szCurDir[1024];

  int iSelectDestination(void);
  void vFill_patient_listbox(void);
  void vExport(void);
  short int iCreate_exp_db(CDBAccess *pDBA);
  short int iCopy_study_data(CDBAccess *pDBA, long lStudyID, long lNewPatientID);
  BOOL bCopy_file(LPCTSTR strSrc, LPCTSTR strDest);
  bool bFileExists(CString cstrFile);
  void vSet_font(CWnd *pwnd, CDC *pDC, CFont *pcfFont, long lFontWeight, long lFontSize = 0);
  long lCalc_font_height(int iPointSize, CDC *pcdc);

  afx_msg void OnBnClickedBtnSelectDest();
  afx_msg void OnBnClickedBtnExport();
  CProgressCtrl m_cExportProgressBar;
  CString m_cstrProgress;
  int m_iPatientsInList;
  int m_iStudiesInList;
  int m_iSelectedPatients;
  int m_iSelectedStudies;
  afx_msg void OnBnClickedBtnHelp();
  afx_msg void OnNMClickListPatient(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnBnClickedOk();
  afx_msg void OnClose();
	afx_msg void OnLvnItemActivateListPatient(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListPatient(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangingListPatient(NMHDR *pNMHDR, LRESULT *pResult);
	bool m_bSelected;
	afx_msg void OnBnClickedBtnSearchDb();
  CDBAccess *pdbNewDB();
};
