/********************************************************************
EquipTest.h

Copyright (C) 2003,2004, 3CPM Company, Inc.  All rights reserved.

  Header file for the CEquipTest class.

HISTORY:
03-OCT-03  RET  New.
********************************************************************/

#if !defined(AFX_EQUIPTEST_H__2BFF8DAC_1CB9_4F16_A780_71C818B41411__INCLUDED_)
#define AFX_EQUIPTEST_H__2BFF8DAC_1CB9_4F16_A780_71C818B41411__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define X_MAX_ET_SCALE (10*4) // Maximum scale of the X axis. 10 sec * 4 data pts/sec
#define MAX_ET_TIME 300 // Base line time period (5 minutes) in seconds.
#define X_MAX_ET_SCALE (5*60*4) // Maximum scale of the X axis. 5 min * 4 data pts/sec

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
  short int m_iNumDots;

public: // Methods
  void vSetup_graph();
  static void vShowStatus(void);

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
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EQUIPTEST_H__2BFF8DAC_1CB9_4F16_A780_71C818B41411__INCLUDED_)
