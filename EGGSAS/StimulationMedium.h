/********************************************************************
StimulationMedium.h

Copyright (C) 2007,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CStimulationMedium class.

HISTORY:
10-MAY-07  RET  New.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#pragma once

// CStimulationMedium dialog

class CStimulationMedium : public CDialog
{
	DECLARE_DYNAMIC(CStimulationMedium)

public:
	CStimulationMedium(CEGGSASDoc *pDoc, unsigned uType,
                     CWnd* pParent = NULL);   // standard constructor
	virtual ~CStimulationMedium();

// Dialog Data
	enum { IDD = IDD_STIMULATION_MEDIUM };

public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  unsigned m_uType; // Type of dialog box (i.e. the text that goes into fields).
  CString m_cstrUnits; // Holds the text representation of the units.

public: // Methods
  void vGet_stim_med_amount(PATIENT_DATA *pdWater);
  void vSet_text(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
  BOOL m_bMilliliters;
  BOOL m_bOunces;
  CString m_cstrDescription;
  afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedCancel();
  afx_msg void OnBnClickedWaterloadHelp();
  BOOL m_bGrams;
  afx_msg void OnBnClickedOunces();
  afx_msg void OnBnClickedMilliliters();
  afx_msg void OnBnClickedGrams();
  UINT m_uQuantity;
};
