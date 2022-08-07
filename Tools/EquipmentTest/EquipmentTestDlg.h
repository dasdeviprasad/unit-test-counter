/********************************************************************
EquipmentTestDlg.cpp

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Header file for the CEquipmentTestDlg class, the main dialog box.

HISTORY:
17-AUG-10  RET  New. Version 1.0
03-SEP-10  RET  Version 1.1
                  Add define: MAX_RESPIRATION_POINTS
                  Add class variables: m_fRespMaxUnconnSignal,
                    m_fRespMinConnectedSignal, m_fRespMinPeakToPeak,
                    m_fAllRespData[], m_iAllRespDataIndex, m_bDataWrappedAround
********************************************************************/

#pragma once

#define MAX_ET_TIME 300 // Base line time period (5 minutes) in seconds.
#define X_MAX_ET_SCALE (5*60*4) // Maximum scale of the X axis. 5 min * 4 data pts/sec

// Timer actions.
#define ACTION_BATTERY_VOLT 1 // Check the battery voltage.
#define ACTION_CHECK_POS_IMPEDANCE 2 // Check the positive impedance.
#define ACTION_CHECK_NEG_IMPEDANCE 3 // Check the negative impedance.
#define ACTION_ACQUIRE_SIGNAL 4 // Acquire and graph the signal.

// Maximum number of respiration points to maintain during test.
#define MAX_RESPIRATION_POINTS 3000

// CEquipmentTestDlg dialog
class CEquipmentTestDlg : public CDialog
{
// Construction
public:
	CEquipmentTestDlg(CWnd* pParent = NULL);	// standard constructor
  ~CEquipmentTestDlg();

// Dialog Data
	enum { IDD = IDD_EQUIPMENTTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
public:
  // Data
  CDriver *m_pDriver;
  unsigned m_uTimer; // Windows timer number.
  short int m_iTimerAction; // Action the timer should take each time it
                            // is started.
  // Data for the graph.
  HWND m_hPEData; // Window handle for the realtime graph window.
  double m_dMinXScale;
  double m_dMaxXScale;
  float m_fTestMaxEGG;
  float m_fTestMinEGG;
  float m_fMaxResp; // Maximum respiration signal.
  float m_fMinResp; // Minimum respiration signal.
  float m_fXPos; // X axis position.
    // Number of elapsed quarter seconds for continuous good values for EGG signal.
  unsigned short m_uElapsedEGGSeconds;
    // Number of elapsed quarter seconds for continuous good values for Respiration signal.
  unsigned short m_uElapsedRespSeconds;
  unsigned m_uTotalTime; // Number of quarter seconds test has been running.
  unsigned m_uTotalPts; // Total number of points graphed.
  unsigned m_uStableDataPoints; // Number of data points that signal has to be within the
                                // stable window to declare the signal stable.
  short int m_ZeroPntsTmr; // Timer to measure how long we have been getting
                           // zero points.
  // End Data for the graph.

  // Data from the INI file 
    // Window (plus and minus) EGG signal must stay in to be stable.
  float m_fStableEGGWindow;
    // Window (plus and minus) Respiration signal must stay in to be stable.
  float m_fStableRespWindow;
    // Number of seconds that signal has to be within the stable window
    // to declare the signal stable.
  unsigned m_uStableTime;
    // Maximum respiration signal (MicroVolts) below which the belt may not be connected.
  float m_fRespMaxUnconnSignal;
    // Minimum respiration signal (MicroVolts) for the belt to be operating properly.
  float m_fRespMinConnectedSignal;
    // Minimum peak-to-peak respiration signal (MicroVolts)
  float m_fRespMinPeakToPeak;
  // End of Data from the INI file 

  float m_fAllRespData[MAX_RESPIRATION_POINTS];
  int m_iAllRespDataIndex;
  bool m_bDataWrappedAround;



  bool m_bEGGStable;
  bool m_bRespStable;

  // Methods
  void vSetup_graph();
  void vInit_strip_chart();
  void vSet_strip_chart_attrib(HWND hPE, STRIP_CHART_ATTRIB *pscaGraph);
  int iEquipment_checks(void);
  void vAcquire_signal_setup(void);
  void vClear_graph(void);
  long lCalc_font_height(int iPointSize, CDC *pcdc);
  void vSet_font(CFont *pcfFont, long lFontWeight, long lFontSize = 0);
  void vDisplay_status(LPCTSTR strMsg, UINT uID, bool bErr);
  bool bAnalyze_data();
  //int iCheck_resp(void);

protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnTimer(UINT nIDEvent);
  CString m_cstrStatus;
  afx_msg void OnBnClickedBtnStartEquipmentTest();
  afx_msg void OnBnClickedBtnStop();
};
