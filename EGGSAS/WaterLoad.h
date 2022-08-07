/********************************************************************
WaterLoad.h

Copyright (C) 2003,2004,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CWaterLoad class.

HISTORY:
03-OCT-03  RET  New.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
24-AUG-12  RET
             Add prototype for method bChanged().
             Add variables: m_uOrigWaterAmount, m_bOrigMilliliters,
               m_bOrigOunces
********************************************************************/

#if !defined(AFX_WATERLOAD_H__EB2F8F7A_20AC_4CB4_A99D_EA32665A44C4__INCLUDED_)
#define AFX_WATERLOAD_H__EB2F8F7A_20AC_4CB4_A99D_EA32665A44C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// These definitions define the type of text that go into various fields
// for displaying what the dialog box is being used for.
#define TYPE_INGEST_WATER 1 // Default dialog box, after Baseline.
#define TYPE_WATER_AMOUNT 2 // Edit
#define TYPE_AMT_OF_WATER 3 // View

/////////////////////////////////////////////////////////////////////////////
// CWaterLoad dialog

class CWaterLoad : public CDialog
{
// Construction
public:
	CWaterLoad(CEGGSASDoc *pDoc, unsigned uFrom, unsigned uType, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaterLoad)
	enum { IDD = IDD_WATER_LOAD };
	UINT	m_uWaterAmt;
	BOOL	m_bMilliliters;
	BOOL	m_bOunces;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaterLoad)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  unsigned m_uType; // Type of dialog box (i.e. the text that goes into fields).
  unsigned m_uFrom; // Place where caller is from.
  CString m_cstrWaterUnits; // Holds the text representation of the water units.
  unsigned short m_uOrigWaterAmount;
	BOOL	m_bOrigMilliliters;
	BOOL	m_bOrigOunces;

public: // Methods
  void vGet_water_amount(PATIENT_DATA *pdWater);
  void vSet_text(void);
  bool bChanged();

protected:

	// Generated message map functions
	//{{AFX_MSG(CWaterLoad)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnMilliliters();
	afx_msg void OnOunces();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedWaterloadHelp();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATERLOAD_H__EB2F8F7A_20AC_4CB4_A99D_EA32665A44C4__INCLUDED_)
