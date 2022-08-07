/********************************************************************
WhatIf.h

Copyright (C) 2008,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the WhatIf class.

HISTORY:
25-JAN-08  RET  New.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

typedef enum
  {
  WIFS_NONE = 0,
  WIFS_ORIGINAL, // The original study item is selected in the listbox.
  WIFS_NEW,      // The "New" item is selected in the listbox.
  WIFS_OTHER,    // A What If item is selected.
  WIFS_LAST,
  } eWhatIfState;

/////////////////////////////////////////////////////////////////////////////
// CWhatIf dialog

class CWhatIf : public CDialog
{
	DECLARE_DYNAMIC(CWhatIf)

public:
	CWhatIf(CEGGSASDoc *pDoc, long lWhatIfID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CWhatIf();

// Dialog Data
	enum { IDD = IDD_WHAT_IF };

public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  long m_lWhatIfID; // What If ID.
  int m_iListSel; // list selection.
  CString m_cstrDateTime; // Date/time.
  CString m_cstrLstBoxDesc; // Description in the listbox of selected item.
  int m_iOriginalStudyIndex; // Index of the "Original" study in the listbox.
  int m_iState; // State of dialog box. See enumerated list eWhatIfState
  bool m_bForceOriginalStudy; // true, the original study state is set
                              // when user selects the close (cancel) button.
  bool m_bSavedWhatIf; // true if a What If is saved.

public: // Methods
  CString cstrGet_whatif_desc(void);
  int iGet_state(void);

private: // Methods
  void vFill_listbox(); //bool bNewItem);
  void vGet_whatif_record(void);
  int iGet_selected_item(CString &cstrDateTime, CString &cstrDesc);
  CString cstrFmt_current_date_time(void);
  void vSet_state(int iSel);
  void vSet_selected_item(int iSel);
  void vEnable_buttons(void);
  void vAdd_item_to_list(CString cstrDateTime, CString cstrDescription, long lID);
  void vParse_list_item(CString &cstrDateTime, CString &cstrDesc);
  int iFind_whatif_id_in_list(long lWhatIfID);
  void vSet_text(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedBtnRecall();
  afx_msg void OnBnClickedBtnDelete();
  afx_msg void OnBnClickedCancel();
  afx_msg void OnBnClickedBtnWhatIfHelp();
  virtual BOOL OnInitDialog();
  CString m_cstrWhatIfDesc;
  afx_msg void OnBnClickedBtnSave();
  CListBox m_cWhatIfList;
  afx_msg void OnLbnDblclkListWhatIf();
  afx_msg void OnLbnSelchangeListWhatIf();
  afx_msg void OnClose();
};
