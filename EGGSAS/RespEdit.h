/********************************************************************
RespEdit.h

Copyright (C) 2003,2004,2008,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CRespEdit class.

HISTORY:
11-DEC-03  RET  New.
08-FEB-08  RET  Version 2.03
                  Remove class variable m_uResp40.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#if !defined(AFX_RESPEDIT_H__C8DDF9AA_B1FD_497E_8048_F6AAAF27A204__INCLUDED_)
#define AFX_RESPEDIT_H__C8DDF9AA_B1FD_497E_8048_F6AAAF27A204__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRespEdit dialog

class CRespEdit : public CDialog
{
// Construction
public:
	CRespEdit(CEGGSASDoc *pDoc, unsigned uFrom, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRespEdit)
	enum { IDD = IDD_EDIT_VIEW_RESPIRATION };
	UINT	m_uResp10;
	UINT	m_uResp20;
	UINT	m_uResp30;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRespEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the document that started this dialog box.
  unsigned m_uFrom; // Place where caller is from (new study, edit, view).

public: // Methods
  void vSet_respiration(short unsigned *puRespRate);
  void vGet_respiration(short unsigned *puRespRate);
  void vSet_text(void);

protected:

	// Generated message map functions
	//{{AFX_MSG(CRespEdit)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedEditviewrespHelp();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESPEDIT_H__C8DDF9AA_B1FD_497E_8048_F6AAAF27A204__INCLUDED_)
