/********************************************************************
RespEditResearch.h

Copyright (C) 2007,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CRespEditResearch class.

HISTORY:
01-JUL-07  RET  New.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#pragma once
#include "afxwin.h"


// CRespEditResearch dialog

class CRespEditResearch : public CDialog
{
	DECLARE_DYNAMIC(CRespEditResearch)

public:
	CRespEditResearch(CEGGSASDoc *pDoc, unsigned uFrom, CWnd* pParent = NULL);   // standard constructor
	virtual ~CRespEditResearch();

public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the document that started this dialog box.
  unsigned m_uFrom; // Place where caller is from (new study, edit, view).
  CString m_cstrBaseText; // Base text for a listbox line.
  bool m_bNewLstBoxSel; // true indicates user just click on the listbox.
  short unsigned m_uRespRate[MAX_TOTAL_PERIODS]; // Holds the respiration rate for each period.
  bool m_bRespChanged; // True if a respiration rate has been changed in the edit box.
  int m_iLstBoxSel; // Currently selected list box item.

public: // Methods
  void vGet_respiration(short unsigned *puRespRate);
  CString cstrFmt_lstbox_line(CString cstrRespRate, int iPeriod);
  void vSet_respiration(int iLstBoxSel);
  void vSave_respiration();
  void vCheck_resp_edit_field();
  CString cstrGet_respiration(unsigned short uRespRate);
  void vSet_text(void);

// Dialog Data
	enum { IDD = IDD_EDIT_VIEW_RESP_RESEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  CListBox m_cRespirationList;
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedCancel();
  afx_msg void OnBnClickedEditviewrespHelp();
  afx_msg void OnLbnSelchangeListRespiration();
  afx_msg void OnBnClickedBtnSetResp();
  afx_msg void OnEnChangeEditRespiration();
  CString m_cstrRespiration;
};
