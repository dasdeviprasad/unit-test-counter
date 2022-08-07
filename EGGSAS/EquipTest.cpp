/********************************************************************
EquipTest.cpp

Copyright (C) 2003,2004,2005,2008,2012,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CEquipTest class.

  This class performs the initial testing to make sure the connections
  are OK before the study begins.

HISTORY:
03-OCT-03  RET  New.
03-MAR-04  RET  Added fields to dialog box for varying the amplitude and
                  frequency of artificially generated data. These fields
                  are normally not visible and only made visible if using
                  artificially generated data.  Added 2 event handlers to
                  modify the amplitude and frequency:
                    OnDeltaposAdjustFreq(), OnDeltaposAdjustAmp().
                Change OnInitDialog() to show the fields used to modify the
                  artifically generated data if it is being used.
16-MAR-04  RET  Add help button and message handler.
03-MAR-05  RET  Change OnTimer() so that if get a low battery warning,
                  user can't continue test.
04-MAR-05  RET  Change OnTimer() so that if 0 data points are read for
                  a predeterined amount of time, the user can't continue test.
11-APR-05  RET  Change OnCancel() so if EGG driver has already been unloaded,
                  the message box for cancelling study is not displayed.
                Change OnInitDialog() so that if the EGG driver doesn't
                  re-initialize, the Equipment test screen is not displayed
                  and the test is terminated.
29-JUN-08  RET  Version 2.05
                  Change "Begin Baseline" button to show "Begin Study" for
                    a study being resumed.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedEquipTestHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "EquipTest.h"
#include "util.h"
  // For graphing
#include "Pegrpapi.h"
#include "Pemessag.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEquipTest dialog

/********************************************************************
Constructor

  Initialize class data.

********************************************************************/
CEquipTest::CEquipTest(CEGGSASDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CEquipTest::IDD, pParent)
  , m_cstrStatus(_T(""))
  {
	//{{AFX_DATA_INIT(CEquipTest)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  m_pDoc = pDoc;
  m_uTimer = 0;
  m_hPEData = NULL;
  }

/********************************************************************
Destructor

  Clean up class data.

********************************************************************/
CEquipTest::~CEquipTest()
  {

  if(m_uTimer > 0)
    KillTimer(m_uTimer);

  if(m_hPEData != NULL)
    PEdestroy(m_hPEData);
  }

void CEquipTest::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
//{{AFX_DATA_MAP(CEquipTest)
// NOTE: the ClassWizard will add DDX and DDV calls here
//}}AFX_DATA_MAP
DDX_Text(pDX, IDC_STATUS, m_cstrStatus);
}


BEGIN_MESSAGE_MAP(CEquipTest, CDialog)
	//{{AFX_MSG_MAP(CEquipTest)
	ON_BN_CLICKED(IDC_START_TEST, OnStartTest)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
  ON_NOTIFY(UDN_DELTAPOS, IDC_ADJUST_AMP, OnDeltaposAdjustAmp)
  ON_NOTIFY(UDN_DELTAPOS, IDC_ADJUST_FREQ, OnDeltaposAdjustFreq)
  ON_BN_CLICKED(IDC_EQUIP_TEST_HELP, OnBnClickedEquipTestHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEquipTest message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
********************************************************************/
BOOL CEquipTest::OnInitDialog() 
  {
  CFont cfBoldFont;
  short int iRet;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();
    // Get the font to use in dialog box fields.
  vSet_font(this, NULL, &cfBoldFont, FW_BOLD);
    // Set the font in the dialog box fields.
  GetDlgItem(IDC_START_TEST)->SetFont(&cfBoldFont);

  GetDlgItem(IDC_RESP_GREEN)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_ELECTRODE_GREEN)->ShowWindow(SW_HIDE);

    // Initialize drivers.
  if(m_pDoc->m_pDriver == NULL)
    { // We don't have the driver initialized yet, so initialize it.
    m_pDoc->m_pDriver = new CDriver();
    iRet = m_pDoc->m_pDriver->iInit();
    }
  else
      // Driver has been initialized, but make sure it is reset.
    iRet = m_pDoc->m_pDriver->iReInitEGG();
  if(iRet != SUCCESS)
    { // Failed to initialize the driver.
    delete m_pDoc->m_pDriver;
    m_pDoc->m_pDriver = NULL;
    OnCancel();
    }
  else
    {
      // Set up the strip chart for recording the signal.
    vSetup_graph();

      // Get the window of signal values within which we must stay for
      // a stable signal, both EGG and respiration signals.
    //m_fStableEGGWindow = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
    //                              INI_ITEM_STABLEWINDOW_EGG, 1000);
    m_fStableEGGWindow = g_pConfig->fGet_item(CFG_STABLEEGGWINDOW);
    //m_fStableRespWindow = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
    //                              INI_ITEM_STABLEWINDOW_RESP, 500);
    m_fStableRespWindow = g_pConfig->fGet_item(CFG_STABLERESPWINDOW);
      // Get the length of time the signal has to be stable before the equipment test is over.
    //m_uStableTime = (unsigned)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER, INI_ITEM_STABLETIME, 30);
    m_uStableTime = (unsigned)g_pConfig->iGet_item(CFG_STABLETIME);

      // If in fake data mode, show dialog box fields and initialize data.
    //if(AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER, INI_ITEM_USE_FAKE_DATA, 0) == 1)
    if(g_pConfig->iGet_item(CFG_USEFAKEDATA) == 1)
      {
      m_uStableTime = 10;
      GetDlgItem(IDC_INC_AMP_TITLE)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_DEC_AMP_TITLE)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_ADJUST_AMP)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_INC_FREQ_TITLE)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_DEC_FREQ_TITLE)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_ADJUST_FREQ)->ShowWindow(SW_SHOW);
      ((CSpinButtonCtrl *)GetDlgItem(IDC_ADJUST_AMP))->SetRange(0, (short int)m_pDoc->m_pDriver->m_fMaxEGG);
      ((CSpinButtonCtrl *)GetDlgItem(IDC_ADJUST_FREQ))->SetRange(0, 4);
      }
    }

  if(m_pDoc->m_iStudyType == STUDY_RESUME)
    GetDlgItem(IDOK)->SetWindowText("Begin Study");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnStartTest

  Message handler for the Start test button.
    Initialize the hardware driver.
    Check the battery voltage and electrode impedance.
    Start a timer for reading the EGG Hardware.
********************************************************************/
void CEquipTest::OnStartTest() 
  {
  CString cstrMsg, cstrTemp, cstrErr;

  if(m_pDoc->m_pDriver == NULL)
    { // We don't have the driver initialized yet, so initialize it.
    m_pDoc->m_pDriver = new CDriver();
    if(m_pDoc->m_pDriver->iInit() != SUCCESS)
      {
      delete m_pDoc->m_pDriver;
      m_pDoc->m_pDriver = NULL;
      }
    }
  else
    {  // Driver has been initialized, but make sure it is reset.
//    ((CEGGSASApp *)AfxGetApp())->vLog("equip Test: restart egg driver");
    m_pDoc->m_pDriver->iReStart();
//    ((CEGGSASApp *)AfxGetApp())->vLog("equip Test: Done restart egg driver");
    }
  if(m_pDoc->m_pDriver != NULL)
    { // Disable the start button.
    GetDlgItem(IDC_START_TEST)->EnableWindow(FALSE);
      // Check the battery.
      // Set up data so user has visual feedback the system is
      // alive while checking the battery voltage.
    if(m_pDoc->m_pDriver->m_iDoBatteryCheck == 1)
      { // Do the battery check.
      m_iNumDots = 0;
      m_cstrStatus = g_pLang->cstrGet_text(ITEM_ET_CHK_BATT_VOLT);
      UpdateData(FALSE);
        // Set a flag so timer knows to check the battery voltage.
      m_iTimerAction = ACTION_BATTERY_VOLT;
      }
    else
      vAcquire_signal_setup(); // Acquire the signal.
      // Start the timer to check the battery voltage.
    m_uTimer = SetTimer(1, 250, NULL);
    UpdateData(FALSE);
    }
  }

/********************************************************************
OnOK

  Message handler for the OK button.
********************************************************************/
void CEquipTest::OnOK() 
  {
	
	CDialog::OnOK();
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.
    Make sure the user really wants to stop the test.
********************************************************************/
void CEquipTest::OnCancel() 
  {
  CString cstrMsg, cstrCaption;

  if(m_pDoc->m_pDriver != NULL)
    {
    //cstrMsg.LoadString(IDS_END_STUDY_ARE_YOU_SURE);
    //cstrCaption.LoadString(IDS_WARNING);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_END_STUDY_SURE);
    cstrCaption = g_pLang->cstrLoad_string(ITEM_G_WARNING);
    if(MessageBox(cstrMsg, cstrCaption, MB_YESNO) == IDYES)
      {
      m_pDoc->m_pDriver->vStop(); // Stop the driver.
      CDialog::OnCancel();
      }
    }
  else
    CDialog::OnCancel();
  }

/********************************************************************
DestroyWindow - Called by the framework when the dialog box is removed.
                Stop the timer.

********************************************************************/
BOOL CEquipTest::DestroyWindow() 
  {

	// TODO: Add your specialized code here and/or call the base class
  if(m_uTimer > 0)
    {
    KillTimer(m_uTimer);
    m_uTimer = 0;
    }
	
	return CDialog::DestroyWindow();
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
void CEquipTest::OnTimer(UINT nIDEvent) 
  {
  int iPtsToGraph, iCnt;
  float fData[2], fEGG, fResp, fXInc; //, fScroll;
  float fEGGData[MAX_PTS_PER_READ], fRespData[MAX_PTS_PER_READ];

	// TODO: Add your message handler code here and/or call default
  if(nIDEvent == m_uTimer)
    {
    if(m_iTimerAction != ACTION_ACQUIRE_SIGNAL)
      { // Do equipment checks.
      vEquipment_checks();
      }
    else // if(m_iTimerAction == ACTION_ACQUIRE_SIGNAL)
      {
      ++m_uTotalTime;
        // Get the new values.
//      ((CEGGSASApp *)AfxGetApp())->vLog("equip Test: Get data points");
      iPtsToGraph = m_pDoc->m_pDriver->iGet_data_all(fEGGData, fRespData);
      if(iPtsToGraph > 0)
        {
        m_ZeroPntsTmr = 0;  // Reset zero points timer.
          // Append new values and examine the signal
        m_uTotalPts += (unsigned)iPtsToGraph;
        fXInc = 1.0F / (float)(DATA_POINTS_PER_MIN_1); //(float)X_MAX_BL_SCALE;
        for(iCnt = 0; iCnt < iPtsToGraph; ++iCnt, m_fXPos += fXInc) //0.25F)
          {
          fEGG = fEGGData[iCnt];
          fResp = fRespData[iCnt];
          fData[0] = fEGG;
          fData[1] = fResp;
          PEvset(m_hPEData, PEP_faAPPENDYDATA, fData, 1); // New Y value.
          fData[0] = m_fXPos;
          fData[1] = m_fXPos;
          PEvset(m_hPEData, PEP_faAPPENDXDATA, fData, 1);// New X value. 
            // Now evaluate the EGG signal.
          if(m_pDoc->m_pdPatient.fTestMaxEGG == INV_FLOAT)
            m_pDoc->m_pdPatient.fTestMaxEGG = fEGG;
          if(m_pDoc->m_pdPatient.fTestMinEGG == INV_FLOAT)
            m_pDoc->m_pdPatient.fTestMinEGG = fEGG;
            // Look for max and mins.
          if(fEGG > m_pDoc->m_pdPatient.fTestMaxEGG)
            m_pDoc->m_pdPatient.fTestMaxEGG = fEGG;
          else if(fEGG < m_pDoc->m_pdPatient.fTestMinEGG)
            m_pDoc->m_pdPatient.fTestMinEGG = fEGG;
          if(m_pDoc->m_pdPatient.fTestMaxEGG
          > m_pDoc->m_pdPatient.fTestMinEGG + (m_fStableEGGWindow * 2))
            {
            m_pDoc->m_pdPatient.fTestMaxEGG = INV_FLOAT;
            m_pDoc->m_pdPatient.fTestMinEGG = INV_FLOAT;
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
          }
        else
          {
          GetDlgItem(IDC_ELECTRODE_RED)->ShowWindow(SW_SHOW);
          GetDlgItem(IDC_ELECTRODE_GREEN)->ShowWindow(SW_HIDE);
          }
        if(m_uElapsedRespSeconds > m_uStableDataPoints)
          { // There is a stable respiration signal for the specified period.
            // Set Green respiration light on, enable the OK button.
          GetDlgItem(IDC_RESP_GREEN)->ShowWindow(SW_SHOW);
          GetDlgItem(IDC_RESP_RED)->ShowWindow(SW_HIDE);
          }
        else
          {
          GetDlgItem(IDC_RESP_GREEN)->ShowWindow(SW_HIDE);
          GetDlgItem(IDC_RESP_RED)->ShowWindow(SW_SHOW);
          }
        if(m_uElapsedEGGSeconds > m_uStableDataPoints
        && m_uElapsedRespSeconds > m_uStableDataPoints)
          { // Both signals are stable.  Stop the test period.
          GetDlgItem(IDOK)->EnableWindow(TRUE);
            // Stop reading data.
          KillTimer(m_uTimer);
          m_uTimer = 0;
          }
        if(m_uTotalPts > (unsigned)((float)MAX_ET_TIME * DATA_POINTS_PER_SECOND))
          { // Signal hasn't settled yet, ask to enter manual mode or cancel.
          CString cstrMsg, cstrTitle;
    
          KillTimer(m_uTimer);
          m_uTimer = 0;
          //cstrTitle.LoadString(IDS_WARNING);
          cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
          cstrMsg = g_pLang->cstrLoad_string(ITEM_G_SIGNAL_NOT_STABLE1)
            + "\n\n" + g_pLang->cstrLoad_string(ITEM_G_SIGNAL_NOT_STABLE2)
            + "\n" + g_pLang->cstrLoad_string(ITEM_G_SIGNAL_NOT_STABLE3);
          //cstrMsg.LoadString(IDS_SIGNAL_NOT_STABLE);
          if(MessageBox(cstrMsg, cstrTitle, MB_OKCANCEL) == IDOK)
            { // Continuing in manual mode.
            m_pDoc->m_pdPatient.bManualMode = TRUE;
            GetDlgItem(IDOK)->EnableWindow(TRUE);
            }
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
            cstrErr = g_pLang->cstrLoad_string(ITEM_G_NOT_READING_DATA);
            //cstrMsg.Format(IDS_ERR_EGG_READ, cstrErr);
            cstrMsg.Format(g_pLang->cstrLoad_string_fmt(ITEM_G_ERR_EGG_READ), cstrErr);
            iPtsToGraph = FAIL_EGG_DRIVER;
            }
          }
        else // if(iPtsToGraph < 0)
          { // Error from driver.
          //cstrMsg.Format(IDS_ERR_EGG_READ, m_pDoc->m_pDriver->m_cstrErr);
          cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_EGG_READ),
                         m_pDoc->m_pDriver->m_cstrErr);
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
          GetDlgItem(IDOK)->EnableWindow(FALSE);
//            // If low battery, don't allow user to continue.
//          if(iPtsToGraph == FAIL_EGG_DRIVER_LOW_BATT)
            GetDlgItem(IDC_START_TEST)->EnableWindow(FALSE);
//          else
//            GetDlgItem(IDC_START_TEST)->EnableWindow(TRUE);
         //cstrErr.LoadString(IDS_ERROR1);
         cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERROR);
         MessageBox(cstrMsg, cstrErr, MB_OK);
          }
        }
      }
    }
	
	CDialog::OnTimer(nIDEvent);
  }

/********************************************************************
OnDeltaposAdjustAmp
    Message handler for the spin controls for changing the amplitude of
    artificially generated data.
      The amplitude is incremented/decremented by 25 and not allowed to
        go below zero nor above the maximum EGG signal.

  inputs: Pointer to a structure that indicates if this is the down arrow
            or the up arrow.
          Pointer to a value that gets a result.  Always set to 0.
********************************************************************/
void CEquipTest::OnDeltaposAdjustAmp(NMHDR *pNMHDR, LRESULT *pResult)
  {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  // TODO: Add your control notification handler code here
  if(pNMUpDown->iDelta < 0)
    {
    m_pDoc->m_pDriver->m_fFakeAmp -= 25.0F; // Down arrow
    if(m_pDoc->m_pDriver->m_fFakeAmp < 0)
      m_pDoc->m_pDriver->m_fFakeAmp = 0;
    }
  else
    {
    m_pDoc->m_pDriver->m_fFakeAmp += 25.0F; // Up arrow
    if(m_pDoc->m_pDriver->m_fFakeAmp > m_pDoc->m_pDriver->m_fMaxEGG)
      m_pDoc->m_pDriver->m_fFakeAmp = m_pDoc->m_pDriver->m_fMaxEGG;
    }

  *pResult = 0;
  }

/********************************************************************
OnDeltaposAdjustFreq
    Message handler for the spin controls for changing the frequency of
    artificially generated data.
      The frequency is incremented/decremented by .05 and not allowed to
        go below zero nor above 4.

  inputs: Pointer to a structure that indicates if this is the down arrow
            or the up arrow.
          Pointer to a value that gets a result.  Always set to 0.
********************************************************************/
void CEquipTest::OnDeltaposAdjustFreq(NMHDR *pNMHDR, LRESULT *pResult)
  {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  // TODO: Add your control notification handler code here
  if(pNMUpDown->iDelta < 0)
    {
    m_pDoc->m_pDriver->m_fFakeFreq -= 0.05F; // Down arrow
    if(m_pDoc->m_pDriver->m_fFakeFreq < 0)
      m_pDoc->m_pDriver->m_fFakeFreq = 0;
    }
  else
    {
    m_pDoc->m_pDriver->m_fFakeFreq += 0.05F; // Up arrow
    if(m_pDoc->m_pDriver->m_fFakeFreq > 4)
      m_pDoc->m_pDriver->m_fFakeFreq = 4;
    }

  *pResult = 0;
  }

/********************************************************************
OnBnClickedEquipTestHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CEquipTest::OnBnClickedEquipTestHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_EQUIP_TEST_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_EQUIP_TEST_HELP);
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
void CEquipTest::vSetup_graph()
  {    
  RECT rButton, rGraph;
  STRIP_CHART_ATTRIB scaGraph;

  GetClientRect(&rGraph); // Dialog box client window rectangle
    // Get the coodinates of the Start Test button in terms of this
    // window and adjust the graphic rectangle fit between the bottom
    // of the dialog box title and the top of the button.
  GetDlgItem(IDC_START_TEST)->GetClientRect(&rButton);
  GetDlgItem(IDC_START_TEST)->MapWindowPoints(this, &rButton);
    // Leave 10 pixels between the bottom of the graph window and the
    // top of the Start Test button.
  rGraph.bottom = rButton.top - 10;

     // Construct the graph object.
  m_hPEData = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rGraph, m_hWnd, IDD_EQUIP_TEST);
    // Set the strip chart attributes;
  scaGraph.dMaxXScale = (float)(X_MAX_ET_SCALE / 60 / 4);
//  scaGraph.iMaxPoints = (int)((float)MAX_ET_TIME * DATA_POINTS_PER_SECOND) + 1;
  scaGraph.iTotalPoints = (int)((float)MAX_ET_TIME * DATA_POINTS_PER_SECOND) + 1;
  if(m_pDoc->m_pDriver == NULL)
    { // Driver not initialized, use default values.
    scaGraph.dMinEGG = (double)-1000.0;
    scaGraph.dMaxEGG = (double)1000.0;
    scaGraph.dMinResp = (double)0.0;
    scaGraph.dMaxResp = (double)2.5;
    }
  else
    { // Driver initialized, use values from the driver.
    scaGraph.dMinEGG = (double)m_pDoc->m_pDriver->m_fMinEGG;
    scaGraph.dMaxEGG = (double)m_pDoc->m_pDriver->m_fMaxEGG;
    scaGraph.dMinResp = (double)m_pDoc->m_pDriver->m_fMinResp;
    scaGraph.dMaxResp = (double)m_pDoc->m_pDriver->m_fMaxResp;
    }
  m_pDoc->vSet_strip_chart_attrib(m_hPEData, &scaGraph);

  m_dMinXScale = 0.0F;
  m_dMaxXScale = (float)(X_MAX_ET_SCALE / 60 / 4); // Numbers are displayed every half minute.

  PEreinitialize(m_hPEData);
  PEresetimage(m_hPEData, 0, 0);
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
void CEquipTest::vShowStatus(void)
  {
  int iIndex;

  if(m_iNumDots == 10)
    {
    if((iIndex = m_cstrStatus.Find('.')) >= 0)
      m_cstrStatus.Delete(iIndex, m_cstrStatus.GetLength() - iIndex);
    m_iNumDots = 0;
    }
  else
    {
    m_cstrStatus += ".";
    ++m_iNumDots;
    }

  UpdateData(FALSE);
  }

/********************************************************************
vEquipment_checks - Perform the following equipment checks:
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
void CEquipTest::vEquipment_checks(void)
  {
  CString cstrMsg, cstrTemp, cstrErr;
  short int iSts;
  bool bBatteryOK;

  if(m_iTimerAction == ACTION_BATTERY_VOLT)
    { // Checking the battery voltage.
    bBatteryOK = false;
    if((iSts = m_pDoc->m_pDriver->iCheck_battery()) != SUCCESS)
      { // Display an appropriate error message.
      if(iSts == SUCCESS_LOW_BATTERY)
        {
        //cstrTemp.LoadString(IDS_CONTINUE);
        cstrTemp = g_pLang->cstrLoad_string(ITEM_G_CONTINUE_QUES);
        cstrMsg.Format("%s  %s", m_pDoc->m_pDriver->m_cstrErr, cstrTemp);
        //cstrErr.LoadString(IDS_WARNING);
        cstrErr = g_pLang->cstrLoad_string(ITEM_G_WARNING);
        if(MessageBox(cstrMsg, cstrErr, MB_YESNO) == IDYES)
          bBatteryOK = true;
        }
      }
    if(iSts == SUCCESS || bBatteryOK == true)
      { // Battery voltage is OK.  Now check the impedance.
      m_iTimerAction = ACTION_CHECK_POS_IMPEDANCE;
      m_iNumDots = 0;
      m_cstrStatus = g_pLang->cstrGet_text(ITEM_ET_CHK_POS_IMPED);
      UpdateData(FALSE);
      }
    }
  else if(m_iTimerAction == ACTION_CHECK_POS_IMPEDANCE)
    { // Check the positive impedance.
    iSts = m_pDoc->m_pDriver->iCheck_impedance(CHK_IMPEDANCE_POS);
    if(iSts == SUCCESS)
      { // Negative impedance is OK, check positive impedance.
      m_iTimerAction = ACTION_CHECK_NEG_IMPEDANCE;
      m_iNumDots = 0;
      m_cstrStatus = g_pLang->cstrGet_text(ITEM_ET_CHK_NEG_IMPED);
      UpdateData(FALSE);
      }
    }
  else if(m_iTimerAction == ACTION_CHECK_NEG_IMPEDANCE)
    { // Check the negative impedance.
    iSts = m_pDoc->m_pDriver->iCheck_impedance(CHK_IMPEDANCE_NEG);
    if(iSts == SUCCESS)
      { // Negative impedance is OK, acquire and graph signal.
      vAcquire_signal_setup();
      }
    }
  if(iSts <= FAIL)
    { // Error
    KillTimer(m_uTimer);
    m_uTimer = 0;
    //cstrErr.LoadString(IDS_ERROR1);
    cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    MessageBox(m_pDoc->m_pDriver->m_cstrErr, cstrErr, MB_OK);
      // Enable the start button again.
    GetDlgItem(IDC_START_TEST)->EnableWindow(TRUE);
    m_cstrStatus.Empty();
    }
  else
    vShowStatus();
  }

/********************************************************************
vAcquire_signal_setup - Set up the data for acquiring the signal.

  inputs: None.
  
  return: None.
********************************************************************/
void CEquipTest::vAcquire_signal_setup(void)
  {

  m_iTimerAction = ACTION_ACQUIRE_SIGNAL;
  m_iNumDots = 0;
  m_cstrStatus.Empty();
  UpdateData(FALSE);
  m_fXPos = 0; // X-axis position.
    // Set Maximum and Minimum values to invalid so we can keep track of them.
  m_pDoc->m_pdPatient.fTestMaxEGG = INV_FLOAT;
  m_pDoc->m_pdPatient.fTestMinEGG = INV_FLOAT;
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
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CEquipTest::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_EQUIPMENT_TEST);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_ET_EQUIP_TEST));

    GetDlgItem(IDC_START_TEST)->SetWindowText(g_pLang->cstrGet_text(ITEM_ET_START_EQUIP_TEST));
    GetDlgItem(IDC_STATIC_RESPIRATORY_SENSOR)->SetWindowText(g_pLang->cstrGet_text(ITEM_ET_RESP_SENSOR));
    GetDlgItem(IDC_STATIC_GASTRIC_ELECTRODES)->SetWindowText(g_pLang->cstrGet_text(ITEM_ET_GASTRIC_ELECT));
    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrGet_text(ITEM_ET_BEGIN_BL));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_EQUIP_TEST_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    //g_pLang->vEnd_section();
    }
  }



