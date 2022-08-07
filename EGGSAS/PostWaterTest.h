/********************************************************************
PostWaterTest.h

Copyright (C) 2003,2004,2005,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CPostWaterTest class.

HISTORY:
03-OCT-03  RET  New.
04-MAR-05  RET  Add class variable m_ZeroPntsTmr.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#if !defined(AFX_POSTWATERTEST_H__EBB02700_4770_44D9_9F91_C232D4A4E8EA__INCLUDED_)
#define AFX_POSTWATERTEST_H__EBB02700_4770_44D9_9F91_C232D4A4E8EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define PW_MAX_TIME 1800 // Standard study Post-water time period (30 minutes) in seconds.
   // Research study Post-water time period (180 minutes) in seconds.
#define PW_MAX_TIME_R   POST_STIMULATION_MAX_TIME_SEC
#define PW_X_MAX_SCALE 10; // Max X axis of 10 minutes. // (10*60*4) // Maximum scale of the X axis. 10 min * 4 data pts/sec
#define PW_TOTAL_PNTS_IN_WINDOW (10.0F * 60.0F * 4.267F)
//#define PW_X_MAX_SCALE 1; // Max X axis of 10 minutes. // (10*60*4) // Maximum scale of the X axis. 10 min * 4 data pts/sec
//#define PW_TOTAL_PNTS_IN_WINDOW (1.0F * 60.0F * 4.267F)


/////////////////////////////////////////////////////////////////////////////
// CPostWaterTest dialog

class CPostWaterTest : public CDialog
{
// Construction
public:
	CPostWaterTest(CEGGSASDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
  ~CPostWaterTest();

// Dialog Data
	//{{AFX_DATA(CPostWaterTest)
	enum { IDD = IDD_POST_WATER_TEST };
	CString	m_cstrClock;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPostWaterTest)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  CRespRate *m_pdlgRespRate; // Pointer to the dialog box that requests the
                             // respiration rate for the period.
  unsigned m_uTimer; // Windows timer number.
  short int m_iClockTime; // Holds the time for the clock.
  short int m_iTimerCnt; // Number of time the timer has executed.
                         // Reset on the second so the clock can be updated
                         // on a second basis.

  HWND m_hPEData; // Window handle for the realtime graph window.
  float m_fXPos; // X axis position.
  unsigned short m_uElapsedSeconds; // Number of elapsed quarter seconds for
                                    // continuous good values.
  double m_dMinXScale; // Minimum X value that is on the screen.
  double m_dMaxXScale; // Maximum X value that is on the screen.
  unsigned m_uTotalTime; // Number of quarter seconds test has been running.
  unsigned m_uTotalPts; // Total number of points graphed.
  short int m_iRespClock; // Used to count seconds so the timer knows
                          // when to display the respiration rate dialog box.
  unsigned short m_uPeriod; // Keeps track of the 10 minute period for respiration.
  int m_iGraphBottom; // bottom of graph window.
  short int m_ZeroPntsTmr; // Timer to measure how long we have been getting
                           // zero points.

public: // Methods
  void vSetup_graph();
  void vSet_text(void);

protected:

	// Generated message map functions
	//{{AFX_MSG(CPostWaterTest)
	virtual BOOL OnInitDialog();
	afx_msg void OnStart();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnExtendTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnDeltaposAdjustAmp(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaposAdjustFreq(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnBnClickedPostwaterHelp();
protected:
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSTWATERTEST_H__EBB02700_4770_44D9_9F91_C232D4A4E8EA__INCLUDED_)
