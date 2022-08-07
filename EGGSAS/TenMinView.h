/********************************************************************
TenMinView.h

Copyright (C) 2003,2004,2007, 3CPM Company, Inc.  All rights reserved.

  Header file for the CTenMinView class.

HISTORY:
03-OCT-03  RET  New.
12-MAY-07  RET  Research Version
                  Remove class variables: m_uOrigPeriodsAnalyzed
                  Add class variables: m_bOrigPeriodsAnalyzed[],
                    m_iNumMinOnGraph, m_iFirstMinOnGraph, m_uMenuTypeForGraph,
                    m_fSpinPeriodStart, m_fSpinPeriodEnd, m_fSpinPeriodStartLoLim,
                    m_fSpinPeriodStartHiLim, m_fSpinPeriodEndLoLim,
                    m_fSpinPeriodEndHiLim, m_bGoodMinChanged, m_bPeriodChanged
                  Add methods: vSet_periods_analyzed_from_orig(),
                    bValidate_good_minutes()
********************************************************************/

#include "afxwin.h"
#if !defined(AFX_TENMINVIEW_H__B530AF71_D957_4A17_A1E1_F0FFCAA1EA48__INCLUDED_)
#define AFX_TENMINVIEW_H__B530AF71_D957_4A17_A1E1_F0FFCAA1EA48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTenMinView dialog

class CTenMinView : public CFormView
{
// Construction
public:
	CTenMinView();   // standard constructor
	DECLARE_DYNCREATE(CTenMinView)
  ~CTenMinView();  // Destructor.

// Dialog Data
	//{{AFX_DATA(CTenMinView)
	enum { IDD = IDD_10MIN_VIEW };
	CListBox	m_cArtifactList;
	CComboBox	m_c10MinPeriodLst;
	CString	m_cstrPatientName;
	CString	m_cstrStartMin;
	CString	m_cstrEndMin;
	int		m_iAccept;
	CString	m_cstrGoodMin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTenMinView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the active document.
  bool m_bInitialUpdate; // true if initial update has been completed.
  HWND m_hPEData1; // Window handle for the raw data graph window 1.
  HWND m_hPEData2; // Window handle for the raw data graph window 2.
  HWND m_hPEData3; // Window handle for the raw data graph window 3.
  bool m_bAnalyzedFieldsMoved; // true if the analyzed fields have moved to
                               // the proper position on the screen.
  bool m_bAnalyzedFieldsVisible; // true if the analyzed fields are visible.
  float m_fSpinStart; // Current setting for the start minute spin button.
  float m_fSpinEnd; // Current setting for the end minute spin button.

  float m_fSpinStartLoLim; // Low limit for the start minute spin button.
  float m_fSpinEndLoLim; // Low limit for the end minute spin button.
  float m_fSpinStartHiLim; // High limit for the start minute spin button.
  float m_fSpinEndHiLim; // High limit for the end minute spin button.

  unsigned short m_uPeriod; // Period we are current viewing.
  float m_fPeriodStartMin; // Start minute of the current period.
  float m_fStart; // Program defined start of good minutes.
                  //  Always in the range of 0 to max minutes in a period.
  float m_fEnd; // Program defined end of good minutes.
                //  Always in the range of 0 to max minutes in a period.
  float m_fOrigStart; // Start minute when the view first shows the period.
  float m_fOrigEnd; // End minute when the view first shows the period.
//  unsigned short m_uOrigPeriodsAnalyzed; // Periods that have been analyzed when
//                                         // the current period is first displayed.
  bool m_bOrigPeriodsAnalyzed[MAX_TOTAL_PERIODS]; //PERIOD_CEILING];
                                               // true - Period has been analyzed
                                               // set when the current period is
                                               // first displayed.
  float m_fOrigMeanBaseline; // Baseline mean when period first displayed.
  unsigned m_uMenuType; // Menu type that's graphing the data.
  float m_fOrigGoodMinStart[MAX_TOTAL_PERIODS]; // Start minute for each period when
                                                // the period is first displayed.
  float m_fOrigGoodMinEnd[MAX_TOTAL_PERIODS]; // End minute for each period when
                                              // the period is first displayed.
  //// Research version
  int m_iNumMinOnGraph; // Number of minutes per graph.
  int m_iFirstMinOnGraph; // First minute on the first graph of a page.
  unsigned m_uMenuTypeForGraph; // The menu used to get graph the data
                                // (VIEW, EDIT(edit and analyze))
  float m_fSpinPeriodStart; // Current setting for the period start minute spin button.
  float m_fSpinPeriodEnd; // Current setting for the period end minute spin button.
  float m_fSpinPeriodStartLoLim; // Low limit for the period start minute spin button.
  float m_fSpinPeriodStartHiLim; // High limit for the period start minute spin button.
  float m_fSpinPeriodEndLoLim; // Low limit for the period end minute spin button.
  float m_fSpinPeriodEndHiLim; // High limit for the period end minute spin button.
  float m_fPeriodLen; // Number of minutes in the current period.
  bool m_bRsrchAnalyzedFieldsMoved; // true if the research analyzed fields have moved to
                                    // the proper position on the screen.
  bool m_bRsrchAnalyzedFieldsVisible; // true if the research analyzed fields are visible.
  float m_fFirstMinOnGraph[3]; // First minute on each graph.
  bool m_bGraphScrolled; // true to indicate the graph has been scrolled.
    
public: // Methods
  void vSet_patient_info(PATIENT_DATA *pdInfo);
  void vGraph_period(unsigned short uPeriod, float *pfData, PATIENT_DATA *pdInfo,
                     unsigned uMenuType);
  void vEnable_fields(BOOL bEnable);
  void vUpdate_period_combobox(unsigned short uPeriod);
  void vInit();
  void vMove_analyze_fields();
  void vMove_field(unsigned uFieldID, RECT *prWin, int *piDistMoved);
  void vShow_analyze_fields(int iShow);
  void vShow_min_sel_fields(int iShow);
  void vInit_analyze_fields();
  bool bValid_minute_format(CString cstrMinute);
  void vSet_periods_analyzed_from_orig();
  void vMove_research_analyze_fields();
  void vMove_research_sel_min_group(unsigned uFieldID1, unsigned uFieldID2,
                                    unsigned uFieldID3, int iX, int iDistance1,
                                    int iDistance2, int *piY);
  void vCalc_min_per_graph(float fPeriodLen, GRAPH_INFO *pgiData,
                           float *pfNumMin);
  void vInit_research_data();
  bool bValidate_good_minutes(float *pfStartMin, float *pfEndMin,
                              CString cstrStartMin, CString cstrEndMin);
  void vAdd_menu_items_for_period();
  bool bAdd_new_period();
  void vInit_period_good_mins();
  void vBuild_standard_period_cbx();
  void vShow_standard_fields(bool bShow);
  void vShow_research_fields(bool bShow);
  void vEnable_research_period_fields(bool bEnable);
  void vEnable_research_goodmin_fields(bool bEnable);
  void vFill_events_fields(PATIENT_DATA *pdInfo);
  void vEnable_research_buttons(bool bEnable);
  void vEdit_event_from_lstbox();
  void vShow_study_type();
  void vShow_WhatIf(bool bShow, CString cstrDesc);

protected:

	// Generated message map functions
	//{{AFX_MSG(CTenMinView)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelchange10minPeriodLst();
	afx_msg void OnDeltaposStartMinSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAccept();
	afx_msg void OnReject();
	afx_msg void OnDeltaposEndMinSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUndoChanges();
	afx_msg void OnAcceptChanges();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
protected:
  virtual void OnDraw(CDC* /*pDC*/);
public:
  void vShow_logo(bool bShow);
  bool m_bLogoVisible; // true - logos are currently visible
  CString m_cstrPeriodStartMin;
  CString m_cstrPeriodEndMin;
  afx_msg void OnDeltaposStartPeriodMinSpin(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaposEndPeriodMinSpin(NMHDR *pNMHDR, LRESULT *pResult);
  CScrollBar m_cGraphScrollBar;
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnEnChangeStartMin();
  afx_msg void OnEnChangeEndMin();
  afx_msg void OnEnChangeStartPeriodMin();
  afx_msg void OnEnChangeEndPeriodMin();
  afx_msg void OnBnClickedChkSetPeriodLength();
  BOOL m_bSetPeriodLength;
  BOOL m_bSetGoodMinutes;
  afx_msg void OnBnClickedChkSetGoodMinutes();
//  afx_msg void OnBnClickedDeleteThisPeriod();
//  afx_msg void OnBnClickedInsertNewPeriod();
  afx_msg void OnDeltaposStartGoodMinSpin(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaposEndGoodMinSpin(NMHDR *pNMHDR, LRESULT *pResult);
  CString m_cstrStartGoodMin;
  CString m_cstrEndGoodMin;
  afx_msg void OnEnChangeStartGoodMin();
  afx_msg void OnEnChangeEndGoodMin();
  CListBox m_cEventsList;
  CString m_cstrEventDescription;
  afx_msg void OnLbnSelchangeEventList();
protected:
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
  afx_msg void OnLbnDblclkEventList();
  afx_msg void OnBnClickedBtnEventsEdit();
  afx_msg void OnBnClickedBtnEventsDelete();
  CString m_cstrStudyType;
  CString m_cstrWhatIfDesc;
  afx_msg void OnBnClickedResetPeriods();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TENMINVIEW_H__B530AF71_D957_4A17_A1E1_F0FFCAA1EA48__INCLUDED_)
