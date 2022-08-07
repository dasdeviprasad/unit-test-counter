/********************************************************************
EquipTest.h

Copyright (C) 2003,2004,2005,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CEquipTest class.

HISTORY:
03-OCT-03  RET  New.
03-MAR-05  RET  Add class variable m_ZeroPntsTmr.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#if !defined(AFX_EQUIPTEST_H__2BFF8DAC_1CB9_4F16_A780_71C818B41411__INCLUDED_)
#define AFX_EQUIPTEST_H__2BFF8DAC_1CB9_4F16_A780_71C818B41411__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define X_MAX_ET_SCALE (10*4) // Maximum scale of the X axis. 10 sec * 4 data pts/sec
#define MAX_ET_TIME 300 // Base line time period (5 minutes) in seconds.
#define X_MAX_ET_SCALE (5*60*4) // Maximum scale of the X axis. 5 min * 4 data pts/sec

// Timer actions.
#define ACTION_BATTERY_VOLT 1 // Check the battery voltage.
#define ACTION_CHECK_POS_IMPEDANCE 2 // Check the positive impedance.
#define ACTION_CHECK_NEG_IMPEDANCE 3 // Check the negative impedance.
#define ACTION_ACQUIRE_SIGNAL 4 // Acquire and graph the signal.

/////////////////////////////////////////////////////////////////////////////
// CEquipTest dialog

class CEquipTest : public CDialog
{
// Construction
public:
	CEquipTest(CEGGSASDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
  ~CEquipTest();

// Dialog Data
	//{{AFX_DATA(CEquipTest)
	enum { IDD = IDD_EQUIP_TEST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEquipTest)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  unsigned m_uTimer; // Windows timer number.
  HWND m_hPEData; // Window handle for the realtime graph window.
  float m_fXPos; // X axis position.
    // Number of elapsed quarter seconds for continuous good values for EGG signal.
  unsigned short m_uElapsedEGGSeconds;
    // Number of elapsed quarter seconds for continuous good values for Respiration signal.
  unsigned short m_uElapsedRespSeconds;
  double m_dMinXScale;
  double m_dMaxXScale;
  unsigned m_uTotalTime; // Number of quarter seconds test has been running.
  unsigned m_uTotalPts; // Total number of points graphed.
    // Window (plus and minus) EGG signal must stay in to be stable.
  float m_fStableEGGWindow;
    // Window (plus and minus) Respiration signal must stay in to be stable.
  float m_fStableRespWindow;
  unsigned m_uStableTime; // Number of seconds that signal has to be within the
                          // stable window to declare the signal stable. Read from INI
  unsigned m_uStableDataPoints; // Number of data points that signal has to be within the
                                // stable window to declare the signal stable.
  float m_fMaxResp; // Maximum respiration signal.
  float m_fMinResp; // Minimum respiration signal.
  short int m_iNumDots; // Number of dots being displayed to show user system
                        // is working.
  short int m_iTimerAction; // Action the timer should take each time it
                            // is started.
  short int m_ZeroPntsTmr; // Timer to measure how long we have been getting
                           // zero points.

public: // Methods
  void vSetup_graph();
  void vShowStatus(void);
  void vEquipment_checks(void);
  void vAcquire_signal_setup(void);
  void vSet_text(void);

protected:

	// Generated message map functions
	//{{AFX_MSG(CEquipTest)
	virtual BOOL OnInitDialog();
	afx_msg void OnStartTest();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  CString m_cstrStatus;
  afx_msg void OnDeltaposAdjustAmp(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaposAdjustFreq(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnBnClickedEquipTestHelp();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EQUIPTEST_H__2BFF8DAC_1CB9_4F16_A780_71C818B41411__INCLUDED_)
