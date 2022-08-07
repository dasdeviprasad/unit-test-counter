/********************************************************************
EquipmentTestDlg.cpp

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the main dialog box.

  Operation:
    - This program uses the eggsas.ini file.  There for EGGSAS must be installed in the
      standard place on the computer.
    - Select the Start Equipment Test button to start the test.
    - Select the Stop Equipment Test button to stop the test.
    - For the test to pass, the signal for both channels must be stable for
      2 minutes.  The time is specified in the eggsas.ini file "stabletime=120  ; seconds".
        - EGG stability:  Max signal must be within 1000 microvolts of the min signal.
             Specified in eggsas.ini file "stableEGGwindow=500 ; microvolts"
        - Respiration stability: Max signal must be within 400 microvolts of the min signal.
             Specified in eggsas.ini file "stableRespwindow=200 ; microvolts"
    - Errors:
        Hardware not connected at start of test.
        Driver (or part of) is missing.
        Hardware disconnected during test.
          Error is no signal.
        Respiration and/or EGG signals do not settle.
        Respiration signal is too low (belt may not be connected).
          The value for this can be adjusted in the INI file by
            setting "respmaxunconnsignal" to a different value.
        Respiration signal is low (belt may need tightening).
          The value for this can be adjusted in the INI file by
            setting "respminconnectedsignal" to a different value.
        The peak-to-peak signal is too low.
          The value for this can be adjusted in the INI file by
            setting "respminpeaktopeak" to a different value.
    - Status window:
        Shows any errors.
        Shows if test is stopped by the stop button.
        Shows if both signals have settled (test passed)
    - Does not detect:
        If a lead (or all leads) is disconnected.
        If one channel (EGG or Respiration) is working but not the other.
          It will show an error of no signal.

HISTORY:
17-AUG-10  RET  New. Version 1.0
03-SEP-10  RET  Version 1.1
                  Add verification that the Respiration belt is operating OK.
                    Add items to INI file for respiration testing:
                      Signal too low, signal low, peak-to-peak to low.
                    Add an array to maintain the respiration signals locally.
                    Add method bAnalyze_data()
                  Remove reference to EGGSAS INI file and put the need items
                    from that INI file into this programs INI file.
                      Stable EGG window, stable Respiration window,
                      length of test, respiration multiplier.
********************************************************************/

#include "stdafx.h"
#include "float.h"
#include "EquipmentTest.h"
#include "EquipmentTestDlg.h"
  // For graphing
#include "Pegrpapi.h"
#include "Pemessag.h"
#include ".\equipmenttestdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CEquipmentTestDlg dialog



CEquipmentTestDlg::CEquipmentTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEquipmentTestDlg::IDD, pParent)
  , m_cstrStatus(_T(""))
  {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CEquipmentTestDlg::~CEquipmentTestDlg()
  {

  delete m_pDriver;
  }

void CEquipmentTestDlg::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_STATUS, m_cstrStatus);
}

BEGIN_MESSAGE_MAP(CEquipmentTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_WM_TIMER()
  ON_BN_CLICKED(IDC_BTN_START_EQUIPMENT_TEST, OnBnClickedBtnStartEquipmentTest)
  ON_BN_CLICKED(IDC_BTN_STOP, OnBnClickedBtnStop)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////
// CEquipmentTestDlg message handlers

/********************************************************************
OnInitDialog

  Create the graph window.
  Initialize class data.
********************************************************************/
BOOL CEquipmentTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
  m_iAllRespDataIndex = 0;
  m_bDataWrappedAround = false;

  m_pDriver = NULL;
  vSetup_graph();

  GetDlgItem(IDC_RESP_GREEN)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_ELECTRODE_GREEN)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);

  // Item from the INI file.
    // Get the window of signal values within which we must stay for
    // a stable signal, both EGG and respiration signals.
  m_fStableEGGWindow = (float)AfxGetApp()->GetProfileInt(INI_SEC_TEST,
                                INI_ITEM_STABLEWINDOW_EGG, 1000);
  m_fStableRespWindow = (float)AfxGetApp()->GetProfileInt(INI_SEC_TEST,
                                INI_ITEM_STABLEWINDOW_RESP, 500);
    // Get the length of time the signal has to be stable before the equipment test is over.
  m_uStableTime = (unsigned)AfxGetApp()->GetProfileInt(INI_SEC_TEST, INI_ITEM_STABLETIME, 30);
    // Maximum signal below which the respiration belt may be disconnected
  m_fRespMaxUnconnSignal = (float)AfxGetApp()->GetProfileInt(INI_SEC_TEST,
                           INI_ITEM_RESP_MAX_UNCONNSIGNAL, 0);
    // Minimum signal for the respiration belt (may need to be tightened.
  m_fRespMinConnectedSignal = (float)AfxGetApp()->GetProfileInt(INI_SEC_TEST,
                               INI_ITEM_RESP_MIN_CONNECTEDSIGNAL, 0);
    // Minimum peak-to-peak signal for the respiration belt.
  m_fRespMinPeakToPeak = (float)AfxGetApp()->GetProfileInt(INI_SEC_TEST,
                         INI_ITEM_RESP_MIN_PEAKTOPEAK, 0);

  // Set a larger font for the status window.
  CFont cfBoldFont;
  vSet_font(&cfBoldFont, FW_BOLD);
  GetDlgItem(IDC_STATUS)->SetFont(&cfBoldFont);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEquipmentTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEquipmentTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
  }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEquipmentTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/********************************************************************
OnTimer
    Message handler for the Timer.
      Whenever the timer comes in, read the EGG device driver for both EGG
        and Respiration.
      Add the new points to the graph.
      If necessary, scroll the graph to the left to simulate a strip chart.
      When there are valid data values for the specified time, stop reading the
        data points and display green leds so the operator can continue.

  inputs: ID of the timer.
********************************************************************/
  void CEquipmentTestDlg::OnTimer(UINT nIDEvent)
    {
  int iPtsToGraph, iCnt;
  float fData[2], fEGG, fResp, fXInc; //, fScroll;
  float fEGGData[MAX_PTS_PER_READ], fRespData[MAX_PTS_PER_READ];

	// TODO: Add your message handler code here and/or call default
  if(nIDEvent == m_uTimer)
    {
    //if(m_iTimerAction != ACTION_ACQUIRE_SIGNAL)
    //  { // Do equipment checks.
    //  if(iEquipment_checks() != SUCCESS)
    //    {
    //    GetDlgItem(IDC_BTN_START_EQUIPMENT_TEST)->EnableWindow(TRUE);
    //    GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
    //      // Stop reading data.
    //    KillTimer(m_uTimer);
    //    m_uTimer = 0;
    //    }
    //  }
    //else // if(m_iTimerAction == ACTION_ACQUIRE_SIGNAL)
      {
      ++m_uTotalTime;
        // Get the new values.
//      ((CEGGSASApp *)AfxGetApp())->vLog("equip Test: Get data points");
      iPtsToGraph = m_pDriver->iGet_data_all(fEGGData, fRespData);
      if(iPtsToGraph > 0)
        {
        m_ZeroPntsTmr = 0;  // Reset zero points timer.
          // Append new values and examine the signal
        m_uTotalPts += (unsigned)iPtsToGraph;
        fXInc = 1.0F / (float)(DATA_POINTS_PER_MIN_1); //(float)X_MAX_BL_SCALE;
        for(iCnt = 0; iCnt < iPtsToGraph; ++iCnt, m_fXPos += fXInc) //0.25F)
          {

          if(m_iAllRespDataIndex >= MAX_RESPIRATION_POINTS)
            {
            m_bDataWrappedAround = true;
            m_iAllRespDataIndex = 0; // Reached the end of the array, wrap around.
            }
          m_fAllRespData[m_iAllRespDataIndex++] = fRespData[iCnt];

          fEGG = fEGGData[iCnt];
          fResp = fRespData[iCnt];
          fData[0] = fEGG;
          fData[1] = fResp;
          PEvset(m_hPEData, PEP_faAPPENDYDATA, fData, 1); // New Y value.
          fData[0] = m_fXPos;
          fData[1] = m_fXPos;
          PEvset(m_hPEData, PEP_faAPPENDXDATA, fData, 1);// New X value. 
            // Now evaluate the EGG signal.
          if(m_fTestMaxEGG == INV_FLOAT)
            m_fTestMaxEGG = fEGG;
          if(m_fTestMinEGG == INV_FLOAT)
            m_fTestMinEGG = fEGG;
            // Look for max and mins.
          if(fEGG > m_fTestMaxEGG)
            m_fTestMaxEGG = fEGG;
          else if(fEGG < m_fTestMinEGG)
            m_fTestMinEGG = fEGG;
          if(m_fTestMaxEGG
          > m_fTestMinEGG + (m_fStableEGGWindow * 2))
            {
            m_fTestMaxEGG = INV_FLOAT;
            m_fTestMinEGG = INV_FLOAT;
            m_uElapsedEGGSeconds = 0;
            }
          else
            ++m_uElapsedEGGSeconds;
            // Now evaluate the Respiration signal.
          if(m_fMaxResp == INV_FLOAT)
            m_fMaxResp = fResp;
          if(m_fMinResp == INV_FLOAT)
            m_fMinResp = fResp;
            // Look for max and mins.
          if(fResp > m_fMaxResp)
            m_fMaxResp = fResp;
          else if(fResp < m_fMinResp)
            m_fMinResp = fResp;
          if(m_fMaxResp > m_fMinResp + (m_fStableRespWindow * 2))
            {
            m_fMaxResp = INV_FLOAT;
            m_fMinResp = INV_FLOAT;
            m_uElapsedRespSeconds = 0;
            }
          else
            ++m_uElapsedRespSeconds;
          }
          // Switch to AutoScaling x axis after receiving X_MAX_SCALE data points
//        if(m_fXPos == X_MAX_SCALE)
//          PEnset(m_hPEData, PEP_nMANUALSCALECONTROLX, PEMSC_NONE);
        if(m_uElapsedEGGSeconds > m_uStableDataPoints)
          { // There is a stable EGG signal for the specified period.
            // Set Green EGG light on, enable the OK button.
          GetDlgItem(IDC_ELECTRODE_RED)->ShowWindow(SW_HIDE);
          GetDlgItem(IDC_ELECTRODE_GREEN)->ShowWindow(SW_SHOW);
          m_bEGGStable = true;
          }
        else
          {
          GetDlgItem(IDC_ELECTRODE_RED)->ShowWindow(SW_SHOW);
          GetDlgItem(IDC_ELECTRODE_GREEN)->ShowWindow(SW_HIDE);
          }
        if(m_uElapsedRespSeconds > m_uStableDataPoints)
          { // There is a stable respiration signal for the specified period.
            // Analyze the data and compare to the test parameters from the INI file.
          if(bAnalyze_data() == true)
            { // Set Green respiration light on, enable the OK button.
            GetDlgItem(IDC_RESP_GREEN)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_RESP_RED)->ShowWindow(SW_HIDE);
            m_bRespStable = true;
            }
          }
        else
          {
          GetDlgItem(IDC_RESP_GREEN)->ShowWindow(SW_HIDE);
          GetDlgItem(IDC_RESP_RED)->ShowWindow(SW_SHOW);
          }
        if(m_uElapsedEGGSeconds > m_uStableDataPoints
        && m_uElapsedRespSeconds > m_uStableDataPoints)
          { // Both signals are stable.  Stop the test period.
          GetDlgItem(IDC_BTN_START_EQUIPMENT_TEST)->EnableWindow(TRUE);
          GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
          if(m_bEGGStable == true && m_bRespStable == true)
            vDisplay_status(NULL, IDS_BOTH_SIGNALS_STABLE, false);
          UpdateData(FALSE);
            // Stop reading data.
          KillTimer(m_uTimer);
          m_uTimer = 0;
          }
        //if(m_uTotalPts > (unsigned)((float)30 * DATA_POINTS_PER_SECOND))
        if(m_uTotalPts > (unsigned)((float)MAX_ET_TIME * DATA_POINTS_PER_SECOND))
          { // Signal hasn't settled yet, ask to enter manual mode or cancel.
          CString cstrMsg, cstrTitle;
    
          KillTimer(m_uTimer);
          m_uTimer = 0;
          if(m_bEGGStable == false && m_bRespStable == false)
            vDisplay_status(NULL, IDS_ALL_SIGNALS_NOT_STABLE, true);
            //m_cstrStatus.LoadString(IDS_ALL_SIGNALS_NOT_STABLE);
          else
            {
            if(m_bEGGStable == false)
              vDisplay_status(NULL, IDS_EGG_SIGNAL_NOT_STABLE, true);
              //m_cstrStatus.LoadString(IDS_EGG_SIGNAL_NOT_STABLE);
            else if(m_bRespStable == false)
              {
              if(m_cstrStatus.IsEmpty() == true)
                vDisplay_status(NULL, IDS_RESP_SIGNAL_NOT_STABLE, true);
                //m_cstrStatus.LoadString(IDS_RESP_SIGNAL_NOT_STABLE);
              }
            }
          GetDlgItem(IDC_BTN_START_EQUIPMENT_TEST)->EnableWindow(TRUE);
          GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
          UpdateData(FALSE);
          }
/******************* Used for scrolling the strip chart and not used.
        if(m_uTotalPts > (unsigned)((float)MAX_ET_TIME * DATA_POINTS_PER_SECOND))
          { // Scroll the X-axis left by the number of points just received.
          fScroll = (float)iPtsToGraph * (5.0F / ((float)MAX_ET_TIME * DATA_POINTS_PER_SECOND));
          m_dMinXScale += fScroll; //((float)iPtsToGraph * 0.25F);
          PEvset(m_hPEData, PEP_fMANUALMINX, &m_dMinXScale, 1);
          m_dMaxXScale += fScroll; //((float)iPtsToGraph * 0.25F);
          PEvset(m_hPEData, PEP_fMANUALMAXX, &m_dMaxXScale, 1);
          }
*******************/
          // Update image and force paint
        PEreinitialize(m_hPEData);
        PEresetimage(m_hPEData, 0, 0);
        ::InvalidateRect(m_hPEData, NULL, FALSE);
        }
      else if(iPtsToGraph <= 0)
        { // Possible error reading the device.
        CString cstrMsg, cstrErr;
        if(iPtsToGraph == 0)
          { // If get 0 points for a pre-determined amount of time, then
            // something is wrong with the hardware.
          if(++m_ZeroPntsTmr >= ZERO_POINTS_MAX_COUNT)
            { // getting zero points for too long a period of time.
            //cstrErr.LoadString(IDS_READING_ZERO_POINTS);
            //cstrMsg.Format(IDS_ERR_EGG_READ, cstrErr);
            cstrMsg.LoadString(IDS_READING_ZERO_POINTS);
            if(m_pDriver->m_cstrErr.IsEmpty() == false)
              {
              cstrMsg += "\r\n";
              cstrMsg += m_pDriver->m_cstrErr;
              }
            iPtsToGraph = FAIL_EGG_DRIVER;
            }
          }
        else // if(iPtsToGraph < 0)
          { // Error from driver.
          cstrMsg.Format(IDS_ERR_EGG_READ, m_pDriver->m_cstrErr);
          }
        if(iPtsToGraph < 0)
          { // Get an error, stop the test.
          KillTimer(m_uTimer); // Stop the timer.
          m_uTimer = 0;
            // Make sure the red light on, the OK button is disabled.
          GetDlgItem(IDC_RESP_GREEN)->ShowWindow(SW_HIDE);
          GetDlgItem(IDC_RESP_RED)->ShowWindow(SW_SHOW);
          GetDlgItem(IDC_ELECTRODE_RED)->ShowWindow(SW_SHOW);
          GetDlgItem(IDC_ELECTRODE_GREEN)->ShowWindow(SW_HIDE);
          GetDlgItem(IDC_BTN_START_EQUIPMENT_TEST)->EnableWindow(TRUE);
          GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
          vDisplay_status(cstrMsg, 0, true);
          //m_cstrStatus = cstrMsg;
          UpdateData(FALSE);
          }
        }
      }
    }
	
    CDialog::OnTimer(nIDEvent);
    }

/********************************************************************
OnBnClickedBtnStartEquipmentTest

  Message handler for the "Start Equipment Test" button.
    Initialize the hardware driver.
    Start a timer for reading the EGG Hardware.
********************************************************************/
void CEquipmentTestDlg::OnBnClickedBtnStartEquipmentTest()
  {
  CString cstrMsg, cstrTemp, cstrErr;
  //int iIndex;



  m_iAllRespDataIndex = 0;
  m_bDataWrappedAround = false;


  vDisplay_status("", 0, false);

  if(m_pDriver != NULL)
    {
    delete m_pDriver;
    m_pDriver = NULL;
    }

    // We don't have the driver initialized yet, so initialize it.
    // We don't have the driver initialized yet, so initialize it.
  m_pDriver = new CDriver();
  if(m_pDriver->iInit() == FAIL)
    { // Driver error.
    vDisplay_status(m_pDriver->m_cstrErr, 0, true);
    //m_cstrStatus = m_pDriver->m_cstrErr;
    delete m_pDriver;
    m_pDriver = NULL;
    }
  if(m_pDriver != NULL)
    { // Disable the start button.
    vInit_strip_chart();
    PEreinitialize(m_hPEData);
    PEresetimage(m_hPEData, 0, 0);
    vDisplay_status(NULL, 0, false);
    m_bEGGStable = false;
    m_bRespStable = false;
    // Make sure the graph is cleared.
    vClear_graph();
    m_iTimerAction = ACTION_CHECK_POS_IMPEDANCE;
    //if(iEquipment_checks() == SUCCESS)
      {
      GetDlgItem(IDC_BTN_START_EQUIPMENT_TEST)->EnableWindow(FALSE);
      GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
      vAcquire_signal_setup(); // Acquire the signal.
      // Start the timer to check the battery voltage.
      m_uTimer = (unsigned int)SetTimer(1, 250, NULL);
      }
    }
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedBtnStop

  Message handler for the "Stop Equipment Test" button.
    Display a message indicating the test has been cancelled.
    Stop the current test.
********************************************************************/
void CEquipmentTestDlg::OnBnClickedBtnStop()
  {

  GetDlgItem(IDC_BTN_START_EQUIPMENT_TEST)->EnableWindow(TRUE);
  GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
  vDisplay_status(NULL, IDS_TEST_STOPPED, false);
    // Stop reading data.
  KillTimer(m_uTimer);
  m_uTimer = 0;
  UpdateData(FALSE);
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
vSetup_graph - Set up the graph window and set all the attributes of
               the graph as described in the comments below.
               The graph is set up and manipulated as a real-time strip
               chart.

  inputs: None.
  
  Output: Create the graph window in the dialog box.

  return: None.
********************************************************************/
void CEquipmentTestDlg::vSetup_graph()
  {    
  RECT rButton, rGraph;
  //STRIP_CHART_ATTRIB scaGraph;

  GetClientRect(&rGraph); // Dialog box client window rectangle
    // Get the coodinates of the Start Test button in terms of this
    // window and adjust the graphic rectangle fit between the bottom
    // of the dialog box title and the top of the button.
  GetDlgItem(IDC_BTN_START_EQUIPMENT_TEST)->GetClientRect(&rButton);
  GetDlgItem(IDC_BTN_START_EQUIPMENT_TEST)->MapWindowPoints(this, &rButton);
    // Leave 10 pixels between the bottom of the graph window and the
    // top of the Start Test button.
  rGraph.bottom = rButton.top - 10;
  rGraph.left += 10;
  rGraph.right -= 10;
  rGraph.top += 10;

     // Construct the graph object.
  m_hPEData = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rGraph, m_hWnd, IDD_EQUIPMENTTEST_DIALOG);
    // Set the strip chart attributes;
  vInit_strip_chart();
//  vInit_strip_chart();
//  scaGraph.dMaxXScale = (float)(X_MAX_ET_SCALE / 60 / 4);
////  scaGraph.iMaxPoints = (int)((float)MAX_ET_TIME * DATA_POINTS_PER_SECOND) + 1;
//  scaGraph.iTotalPoints = (int)((float)MAX_ET_TIME * DATA_POINTS_PER_SECOND) + 1;
//  if(m_pDriver == NULL)
//    { // Driver not initialized, use default values.
//    scaGraph.dMinEGG = (double)-1000.0;
//    scaGraph.dMaxEGG = (double)1000.0;
//    scaGraph.dMinResp = (double)0.0;
//    scaGraph.dMaxResp = (double)2.5;
//    }
//  else
//    { // Driver initialized, use values from the driver.
//    scaGraph.dMinEGG = (double)m_pDriver->m_fMinEGG;
//    scaGraph.dMaxEGG = (double)m_pDriver->m_fMaxEGG;
//    scaGraph.dMinResp = (double)m_pDriver->m_fMinResp;
//    scaGraph.dMaxResp = (double)m_pDriver->m_fMaxResp;
//    }
//  vSet_strip_chart_attrib(m_hPEData, &scaGraph);
//
//  m_dMinXScale = 0.0F;
//  m_dMaxXScale = (float)(X_MAX_ET_SCALE / 60 / 4); // Numbers are displayed every half minute.

  PEreinitialize(m_hPEData);
  PEresetimage(m_hPEData, 0, 0);
  }

/********************************************************************
vInit_strip_chart - Initialize the data used by the strip chart.
                         
  inputs: None.

  return: None.
********************************************************************/
void CEquipmentTestDlg::vInit_strip_chart()
  {
  STRIP_CHART_ATTRIB scaGraph;

  scaGraph.dMaxXScale = (float)(X_MAX_ET_SCALE / 60 / 4);
//  scaGraph.iMaxPoints = (int)((float)MAX_ET_TIME * DATA_POINTS_PER_SECOND) + 1;
  scaGraph.iTotalPoints = (int)((float)MAX_ET_TIME * DATA_POINTS_PER_SECOND) + 1;
  if(m_pDriver == NULL)
    { // Driver not initialized, use default values.
    scaGraph.dMinEGG = (double)-1000.0;
    scaGraph.dMaxEGG = (double)1000.0;
    scaGraph.dMinResp = (double)-25.0;
    scaGraph.dMaxResp = (double)250.0;
    }
  else
    { // Driver initialized, use values from the driver.
    scaGraph.dMinEGG = (double)m_pDriver->m_fMinEGG;
    scaGraph.dMaxEGG = (double)m_pDriver->m_fMaxEGG;
    //scaGraph.dMinResp = (double)(m_pDriver->m_fMinResp - (.1F * m_pDriver->m_fMaxResp));
    //scaGraph.dMaxResp = (double)m_pDriver->m_fMaxResp;

    scaGraph.dMinResp = (double)(m_pDriver->m_fMinResp - (.1F * m_fStableRespWindow));
    scaGraph.dMaxResp = (double)m_fStableRespWindow;

    
    }
  vSet_strip_chart_attrib(m_hPEData, &scaGraph);

  m_dMinXScale = 0.0F;
  m_dMaxXScale = (float)(X_MAX_ET_SCALE / 60 / 4); // Numbers are displayed every half minute.
  }

/********************************************************************
vSet_strip_chart_attrib - Set the strip chart attributes for the
                          recording strip charts.

  inputs: Handle to the strip chart.
          Pointer to a STRIP_CHART_ATTRIB structure containing the
            attributes to set.

  return: None.
********************************************************************/
void CEquipmentTestDlg::vSet_strip_chart_attrib(HWND hPE, STRIP_CHART_ATTRIB *pscaGraph)
  {
  CString cstrText;
  double m_dMinXScale, m_dMaxXScale, dAxisCntrl, dValue;
  float fVAlue, fMAPArray[2];
  int iMASArray[2];

  PEnset(hPE, PEP_nSUBSETS, 2); // 2 sets of data, EGG and Respiration.
    // Set the maximum X-axis scale.  It displays quarter seconds.
  PEnset(hPE, PEP_nPOINTS, pscaGraph->iTotalPoints); //iMaxPoints);

    // Set the subset line types for each set of data.
  int nLineTypes[] = {PELT_THINSOLID, PELT_THINSOLID};
  PEvset(hPE, PEP_naSUBSETLINETYPES, nLineTypes, 2);

    // Set Manual X scale
  PEnset(hPE, PEP_nMANUALSCALECONTROLX, PEMSC_MINMAX);
  m_dMinXScale = 0.0F;
  PEvset(hPE, PEP_fMANUALMINX, &m_dMinXScale, 1);
  m_dMaxXScale = pscaGraph->dMaxXScale; // Numbers are displayed every half minute.
  PEvset(hPE, PEP_fMANUALMAXX, &m_dMaxXScale, 1);

    // Set values to be placed every second and tick marks every quarter second.
  dAxisCntrl = 1.0F; // Multiples of grid tick marks where values are placed.
  PEvset(hPE, PEP_fMANUALXAXISLINE, &dAxisCntrl, 1);
  dAxisCntrl = 0.5F; // Where grid tick marks are placed.
  PEvset(hPE, PEP_fMANUALXAXISTICK, &dAxisCntrl, 1);
  PEnset(hPE, PEP_bMANUALXAXISTICKNLINE, TRUE);

    // Set a NULL data value other than 0 so the graph display values of 0. As this
    // may be the case if nothing is connected.
  dValue = -20000.0F;
  PEvset(hPE, PEP_fNULLDATAVALUE, &dValue, 1);

    // Clear out default data //
  fVAlue = 0;
  PEvsetcellEx(hPE, PEP_faXDATA, 0, 0, &fVAlue);
  PEvsetcellEx(hPE, PEP_faXDATA, 0, 1, &fVAlue);
  PEvsetcellEx(hPE, PEP_faXDATA, 0, 2, &fVAlue);
  PEvsetcellEx(hPE, PEP_faXDATA, 0, 3, &fVAlue);

  PEvsetcellEx(hPE, PEP_faYDATA, 0, 0, &fVAlue);
  PEvsetcellEx(hPE, PEP_faYDATA, 0, 1, &fVAlue);
  PEvsetcellEx(hPE, PEP_faYDATA, 0, 2, &fVAlue);
  PEvsetcellEx(hPE, PEP_faYDATA, 0, 3, &fVAlue);

    // Set background and graphs shades.
  PEnset(hPE, PEP_bBITMAPGRADIENTMODE, FALSE);
  PEnset(hPE, PEP_nQUICKSTYLE, PEQS_MEDIUM_INSET);

  PEszset(hPE, PEP_szMAINTITLE, ""); // Blank title.
  PEszset(hPE, PEP_szSUBTITLE, ""); // no subtitle

    // X axis title.
  cstrText.LoadString(IDS_MINUTES);
  PEszset(hPE, PEP_szXAXISLABEL, (char *)(const char *)cstrText);

    // Disable random points to export listbox.
  PEnset(hPE, PEP_bNORANDOMPOINTSTOEXPORT, TRUE);
  PEnset(hPE, PEP_bFOCALRECT, FALSE); // No focal rect.
  PEnset(hPE, PEP_bALLOWBAR, FALSE); // Bar Graph is not available.
  PEnset(hPE, PEP_bALLOWPOPUP, FALSE); // User can not access popup menu.
  PEnset(hPE, PEP_bPREPAREIMAGES, TRUE); // Prepare images in memory.
  PEnset(hPE, PEP_bCACHEBMP, TRUE); // Store prepared image for possible re-use.
  PEnset(hPE, PEP_bFIXEDFONTS, TRUE); // Keep font size fixed.

    // subset colors (EGG, Respiration)
  DWORD dwArray[2] = {RED_RGB, BLACK_RGB};
  PEvsetEx(hPE, PEP_dwaSUBSETCOLORS, 0, 2, dwArray, 0);
    // Set the subset labels that appear at the top of the graph.
  cstrText.LoadString(IDS_EGG);
  PEvsetcell(hPE, PEP_szaSUBSETLABELS, 0, (char *)(const char *)cstrText);
  cstrText.LoadString(IDS_RESPIRATION);
  PEvsetcell(hPE, PEP_szaSUBSETLABELS, 1, (char *)(const char *)cstrText);

    // Set up the double graphs.  EGG on top and Respiration on bottom.
    // Separate the 2 graphs.
  PEnset(hPE, PEP_nMULTIAXISSTYLE, PEMAS_SEPARATE_AXES);
  PEnset(hPE, PEP_nMULTIAXISSEPARATORSIZE, 75);

    // Y axis title.
  cstrText.LoadString(IDS_RAW_WAVFRM_Y_AXIS_LBL);
  PEszset(hPE, PEP_szYAXISLABEL, (char *)(const char *)cstrText);
    // Two graphs are to be drawn in this window (EGG and respiration).
    // Designate which graph uses the top part of the window and which
    // uses the bottom part of the window.
  iMASArray[0] = 1;   // EGG signal is top graph.
  iMASArray[1] = 1;   // Respiration signal is bottom graph.
  PEvset(hPE, PEP_naMULTIAXESSUBSETS, iMASArray, 2);
  
    // Designate axes proportions.  The percentage of the window that
    // each graph uses.
  fMAPArray[0] = (float)0.6; // Percentage of window top graph uses.
  fMAPArray[1] = (float)0.4; // Percentage of window bottom graph uses.
  PEvset(hPE, PEP_faMULTIAXESPROPORTIONS, fMAPArray, 2);

    // Y-axis label for the top graph.
  PEnset(hPE, PEP_nWORKINGAXIS, 0); // Set first y axis parameters
    // Set the minimum and maximum Y-axis scale values for EGG.
  PEnset(hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
  PEvset(hPE, PEP_fMANUALMINY, &pscaGraph->dMinEGG, 1);
  PEvset(hPE, PEP_fMANUALMAXY, &pscaGraph->dMaxEGG, 1);

    // Y-axis label for the bottom graph.
  PEnset(hPE, PEP_nWORKINGAXIS, 1); // Set second y axis parameters
  cstrText.LoadString(IDS_RAW_WAVFRM_Y_AXIS_LBL);
//  cstrText.LoadString(IDS_VOLTS);
  PEszset(hPE, PEP_szYAXISLABEL, (char *)(const char *)cstrText);
    // Set the minimum and maximum Y-axis scale values for Respiration.
  PEnset(hPE, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
//  dValue = (double)0.0; //pscaGraph->dMinEGG / (double)2.0;
//  PEvset(hPE, PEP_fMANUALMINY, &dValue, 1);
//  dValue = pscaGraph->dMaxEGG / (double)2.0;
//  PEvset(hPE, PEP_fMANUALMAXY, &dValue, 1);
  PEvset(hPE, PEP_fMANUALMINY, &pscaGraph->dMinResp, 1);
  PEvset(hPE, PEP_fMANUALMAXY, &pscaGraph->dMaxResp, 1);

  PEnset(hPE, PEP_nWORKINGAXIS, 0); // Always reset working axis to 0.
  }

/********************************************************************
iEquipment_checks - Perform the following equipment checks:
                      - Battery voltage
                      - Positive impedance
                      - Negative impedance

              This is called from the timer each time it executes.  When the
              driver determines enough points have been read, it checks the
              signal to make sure its OK and returns an appropriate status.
              
              If enough points have not been read, the driver returns the status,
              SUCCESS_NO_DATA to indicate it needs more time.

  inputs: None.
  
  return: None.
********************************************************************/
//int CEquipmentTestDlg::iEquipment_checks(void)
//  {
//  CString cstrMsg, cstrTemp, cstrErr;
//  short int iSts = SUCCESS;
//
//  if(m_iTimerAction == ACTION_CHECK_POS_IMPEDANCE)
//    {
//      // Check the positive impedance.
//    if((iSts = m_pDriver->iCheck_impedance(CHK_IMPEDANCE_POS)) == SUCCESS)
//      m_iTimerAction = ACTION_CHECK_NEG_IMPEDANCE;
//    }
//  else if(m_iTimerAction == ACTION_CHECK_NEG_IMPEDANCE)
//    { // Positive impedance is OK, check negative impedance.
//    if((iSts = m_pDriver->iCheck_impedance(CHK_IMPEDANCE_NEG)) == SUCCESS)
//      { // Negative impedance is OK, acquire and graph signal.
//      vAcquire_signal_setup();
//      }
//    }
//  if(iSts <= FAIL)
//    { // Error
//    m_cstrStatus = m_pDriver->m_cstrErr;
//    UpdateData(FALSE);
//    }
//  else
//    iSts = SUCCESS;
//  return(iSts);
//  }

/********************************************************************
vAcquire_signal_setup - Set up the data for acquiring the signal.

  inputs: None.
  
  return: None.
********************************************************************/
void CEquipmentTestDlg::vAcquire_signal_setup(void)
  {

  m_iTimerAction = ACTION_ACQUIRE_SIGNAL;
  //m_iNumDots = 0;
  vDisplay_status(NULL, 0, false);
  //m_cstrStatus.Empty();
  UpdateData(FALSE);
  m_fXPos = 0; // X-axis position.
    // Set Maximum and Minimum values to invalid so we can keep track of them.
  m_fTestMaxEGG = INV_FLOAT;
  m_fTestMinEGG = INV_FLOAT;
  m_fMaxResp = INV_FLOAT;
  m_fMinResp = INV_FLOAT;
     // Initialize the number of elapsed quarter seconds for continuous good values.
  m_uElapsedEGGSeconds = 0;
  m_uElapsedRespSeconds = 0;
  m_uTotalTime = 0; // Total running time in quarter seconds.
  m_uTotalPts = 0;
  m_uStableDataPoints = (unsigned)(DATA_POINTS_PER_SECOND * m_uStableTime);
  m_ZeroPntsTmr = 0; // Init how long we have been reading 0 points.
  }

/********************************************************************
vShowStatus - Update a status field by appending '.' to the text.
              This shows the system is alive and working as it takes a
              while for the driver to read the battery voltage or impedance
              data.
              This is called by the timer each time is executes to read
              the battery voltage or an impedance

  inputs: None.
  
  return: None.
********************************************************************/
//void CEquipmentTestDlg::vShowStatus(void)
//  {
//  int iIndex;
//
//  if(m_iNumDots == 10)
//    {
//    if((iIndex = m_cstrStatus.Find('.')) >= 0)
//      m_cstrStatus.Delete(iIndex, m_cstrStatus.GetLength() - iIndex);
//    m_iNumDots = 0;
//    }
//  else
//    {
//    m_cstrStatus += ".";
//    ++m_iNumDots;
//    }
//
//  UpdateData(FALSE);
//  }

/********************************************************************
vClear_graph - Clear the graph.
                         
  inputs: None.

  return: None.
********************************************************************/
void CEquipmentTestDlg::vClear_graph(void)
  {
  int iPnts, iCnt;
  float fVAlue;

  iPnts = PEnget(m_hPEData, PEP_nPOINTS);
  for(iCnt = 0; iCnt < iPnts; ++iCnt)
    {
    fVAlue = 0;
    PEvsetcellEx(m_hPEData, PEP_faXDATA, 0, iCnt, &fVAlue);
    PEvsetcellEx(m_hPEData, PEP_faXDATA, 1, iCnt, &fVAlue);

    PEvsetcellEx(m_hPEData, PEP_faYDATA, 0, iCnt, &fVAlue);
    PEvsetcellEx(m_hPEData, PEP_faYDATA, 1, iCnt, &fVAlue);
    }

  }

/********************************************************************
vSet_font - Create a font using "Arial" for a specific window or CDC.

  inputs: Pointer to a window. If the CDC pointer is not NULL, this
            pointer is ignored.
          Pointer to a CDC object. If this is NULL, the window pointer
            is used to get the CDC object.
          Pointer to a pointer to a CFont object which gets created font.
          Weight of the Font.
          Font size. Default is 0. If 0, the character height is used
            as the size.

  return: None.
********************************************************************/
void CEquipmentTestDlg::vSet_font(CFont *pcfFont, long lFontWeight,
long lFontSize)
  {
  CDC *pDC;
  TEXTMETRIC tm;
  LOGFONT lfLF;

  pDC = this->GetDC();
  pDC->GetTextMetrics(&tm);
  memset(&lfLF, 0, sizeof(LOGFONT)); // zero out structure
    // Set the size of the font depending on the last input.
  if(lFontSize != 0)
    lfLF.lfHeight = lCalc_font_height(lFontSize, pDC); // Calculate new font size.
  else
    lfLF.lfHeight = tm.tmHeight; // Use the standard font size.
  strcpy(lfLF.lfFaceName, "Arial");  // Font name is "Arial".
  lfLF.lfWeight = lFontWeight; // Set to weight of font.
  pcfFont->CreateFontIndirect(&lfLF); // Create the font.
  }

/********************************************************************
lCalc_font_height - Calculate the font height.
                    Formula: 
                      (Point size * Num Pixels per logical inch along Y) / 72.
                         
  inputs: Point size.
          Pointer to the CDC object for the printer.

  return: Height of the font in pixels.
********************************************************************/
long CEquipmentTestDlg::lCalc_font_height(int iPointSize, CDC *pcdc)
  {
  return(MulDiv(iPointSize, pcdc->GetDeviceCaps(LOGPIXELSY), 72));
  }

/********************************************************************
vDisplay_status - Display the specified message in the status window.
                         
  inputs: Pointer to string containing a status message.  Null if
            the status message is from the string table.
          Resource ID for a status message from the string table.
            0 if the status message is a string.
          true if this in an error message.

  return: None.
********************************************************************/
void CEquipmentTestDlg::vDisplay_status(LPCTSTR strMsg, UINT uID, bool bErr)
  {
  CString cstrMsg;

  if(strMsg != NULL)
    cstrMsg = strMsg; //cstrErr;
  else if(uID != 0)
    cstrMsg.LoadString(uID);
  else
    cstrMsg.Empty();

  if(bErr == true)
    {
    m_cstrStatus = "ERROR: \r\n";
    m_cstrStatus += cstrMsg;
    }
  else
    m_cstrStatus = cstrMsg;
  UpdateData(FALSE);
  }

/********************************************************************
bAnalyze_data - Analyze the respiration data.
                  - Average signal is too low.
                  - Average signal is low.
                  - Average of all peak to peak signals is too low.
                         
  inputs: None.

  return: true if analysis show no problems, otherwise false.
********************************************************************/
bool CEquipmentTestDlg::bAnalyze_data()
  {
  int iMaxIndex, iIndex, iNumPeaks;
  float fAvg, fPeak, fTrough;
  bool bFindPeak, bRet;

  bRet = true;
  if(m_bDataWrappedAround == true)
    iMaxIndex = MAX_RESPIRATION_POINTS - 1;
  else
    iMaxIndex = m_iAllRespDataIndex - 1;
  // Average all the respiration readings.
  for(fAvg = 0.0F, iIndex = 0; iIndex <= iMaxIndex; ++iIndex)
    fAvg += m_fAllRespData[iIndex];
  if(iMaxIndex > 0)
    fAvg /= (float)iMaxIndex;
  else
    fAvg = 0.0F;
  if(fAvg < m_fRespMaxUnconnSignal)
    { // Signal too low, belt may not be connected.
    vDisplay_status(NULL, IDS_RESP_SIGNAL_TOO_LOW, true);
    //vDisplay_status("Signal too low, belt may not be connected.", 0, true);
    bRet = false;
    }
  else if(fAvg < m_fRespMinConnectedSignal)
    { // Signal is low, belt needs to be tightened.
    vDisplay_status(NULL, IDS_RESP_SIGNAL_LOW, true);
    //vDisplay_status("Signal is low, belt may need to be tightened.", 0, true);
    bRet = false;
    }
  else
    { // Look peak-to-peak for all the data.
      // Get the voltage difference for each peak-to-trough and then average them.
    bFindPeak = true; // Find the first peak.
    fPeak = 0;
    iNumPeaks = 0;
    for(fAvg = 0.0F, iIndex = 0; iIndex <= iMaxIndex; ++iIndex)
      {
      if(bFindPeak == true)
        { // Find peak.
        if(m_fAllRespData[iIndex] > fPeak)
          fPeak = m_fAllRespData[iIndex];
        else if(m_fAllRespData[iIndex] < fPeak)
          {
          bFindPeak = false; // Starting to go down, found peak.
          fTrough = fPeak;
          }
        }
      else
        { // Find trough, negative peak
        if(m_fAllRespData[iIndex] < fTrough)
          fTrough = m_fAllRespData[iIndex];
        else if(m_fAllRespData[iIndex] > fTrough)
          {
          bFindPeak = true; // Signal rising, found trough.
          fAvg += (fPeak - fTrough);
          ++iNumPeaks;
          fPeak = 0;
          }
        }
      }
    if(iNumPeaks > 0)
      fAvg /= (float)iNumPeaks;
    else
      fAvg = 0.0F;
    if(fAvg < 1)
      { // Peak-to-peak difference is too small.  Belt may be too loose.
      vDisplay_status(NULL, IDS_RESP_SIGNAL_VARIANCE_LOW, true);
      //vDisplay_status("Signal variance is too small, belt may be too loose.", 0, true);
      bRet = false;
      }
    }
  return(bRet);
  }

//int CEquipmentTestDlg::iCheck_resp(void)
//  {
//  int iBadCnt, iIndex, iRet;
//
//  // Check for 95% of the respiration data to be less than 1.0.
//  iBadCnt = 0;
//  for(iIndex = 0; iIndex < m_iDataIndex; ++iIndex)
//    {
//    if(m_fRespData[iIndex] < 1.0F)
//      ++iBadCnt;
//    }
//  if(iBadCnt > (m_iDataIndex * .95F))
//    iRet = FAIL;
//  else
//    iRet = SUCCESS;
//  return(iRet);
//  }