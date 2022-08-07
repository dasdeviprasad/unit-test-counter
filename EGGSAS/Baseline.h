/********************************************************************
Baseline.h

Copyright (C) 2003,2004,2005,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CBaseline class.

HISTORY:
03-OCT-03  RET  New.
04-MAR-05  RET  Add class variable m_ZeroPntsTmr.
17-MAY-07  RET  Research Version
                  Add defines for sctions when user exits the dialog box using OK.
                  Add class variables: m_uAction, m_iMaxTime, m_uTotalPts,
                    m_uStopEnableCnt.
18-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#if !defined(AFX_BASELINE_H__0525ECB3_2740_4030_9340_D8666377D278__INCLUDED_)
#define AFX_BASELINE_H__0525ECB3_2740_4030_9340_D8666377D278__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_BL_TIME 600 // Base line time period (10 minutes) in seconds.
#define X_MAX_BL_SCALE (10*60*4) // Maximum scale of the X axis. 10 min * 4 data pts/sec
#define BL_TOTAL_PNTS_IN_WINDOW (10.0F * 60.0F * 4.267F)

//// Research Version
#define MAX_BL_TIME_RESEARCH 3600 // 1 hour in seconds.
// Actions when user exits the dialog box using OK.
#define BL_ACTION_NONE          0
#define BL_ACTION_STIM_MED      1
#define BL_ACTION_SKIP_STIM_MED 2
#define BL_ACTION_PAUSE         3
//// End Research Version

/////////////////////////////////////////////////////////////////////////////
// CBaseline dialog

class CBaseline : public CDialog
{
// Construction
public:
	CBaseline(CEGGSASDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
  ~CBaseline(); // Destructor

// Dialog Data
	//{{AFX_DATA(CBaseline)
	enum { IDD = IDD_BASELINE };
	CString	m_cstrClock;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseline)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc;
  unsigned m_uTimer; // Windows timer number.
  short int m_iClockTime; // Holds the time for the clock.
  short int m_iTimerCnt; // Number of time the timer has executed.
                         // Reset on the second so the clock can be updated
                         // on a second basis.

  HWND m_hPEData; // Window handle for the realtime graph window.
  float m_fXPos; // X axis position.
    // Number of elapsed quarter seconds for continuous good values.
  unsigned short m_uElapsedSeconds;
  double m_dMinXScale; // Minimum X value displayed in the window.
  double m_dMaxXScale; // Maximum X value displayed in the window.
  unsigned m_uTotalTime; // Number of quarter seconds test has been running.
  bool m_bExtend; // true - user extended the baseline period.
  short int m_ZeroPntsTmr; // Timer to measure how long we have been getting
                           // zero points.
  short int m_iMaxTime; // Maximum amount of seconds the baseline is allowed to run.

  //// Research Version
  unsigned short m_uAction; // Action user chose when exiting the dialog box
                            // with OK.
  unsigned m_uTotalPts; // Total number of points graphed.
  unsigned short m_uStopEnableCnt; // Counts up by seconds.  When it
                                   // equals a predefined number of seconds
                                   // the Stop button is enabled.  It keeps
                                   // on counting up so the Stop button is
                                   // enable only once.
  //// End Research Version

public: // Methods
  void vSetup_graph();
  unsigned short uGet_action();
  unsigned uGet_points_in_baseline();
  void vAdd_baseline_trailer();
  void vSet_text(void);

protected:

	// Generated message map functions
	//{{AFX_MSG(CBaseline)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnStartBl();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnExtend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnDeltaposAdjustAmp(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaposAdjustFreq(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnBnClickedBaselineHelp();
  afx_msg void OnBnClickedPauseStudy();
  afx_msg void OnBnClickedSkipStimMed();
//  virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
protected:
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASELINE_H__0525ECB3_2740_4030_9340_D8666377D278__INCLUDED_)
