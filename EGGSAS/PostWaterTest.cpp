/********************************************************************
PostWaterTest.cpp

Copyright (C) 2003,2004,2005,2007,2009,2012,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CPostWaterTest class.

  This class performs the measurements after the water load.

  At the start of each 10 minute period, display a dialog box
  asking for the respirations per minute.

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
03-MAR-05  RET  Change OnTimer() to read data again if the first return
                  is a low battery warning.
04-MAR-05  RET  Change OnTimer() so that if 0 data points are read for
                  a predeterined amount of time, the user can't continue test.
26-MAY-07  RET  Research Version
                  Change OnInitDialog() to:
                    - Hide the "Extend Test ..." button.
                    - Enable the "OK" (Finish) button.
                  Change OnOK() to ask user to finish study and don't
                    ask for water load.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
28-FEB-12  RET  Version 
             Add data storage location.
               Change methods: OnInitDialog()
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedPostwaterHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "RespRate.h"
#include "PostWaterTest.h"
#include "WaterLoad.h"
#include "FinishStudy.h"
#include "EventRecorder.h"
#include "util.h"
  // For graphing
#include "Pegrpapi.h"
#include "Pemessag.h"
//#include ".\postwatertest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPostWaterTest dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the currently active document.
          Pointer to parent window (default is NULL).
********************************************************************/
CPostWaterTest::CPostWaterTest(CEGGSASDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CPostWaterTest::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CPostWaterTest)
	m_cstrClock = _T("");
	//}}AFX_DATA_INIT

  m_pDoc = pDoc;
  m_uTimer = 0;
  m_hPEData = NULL;
  m_pdlgRespRate = NULL;
  }

/********************************************************************
Destructor

  Clean up class data.
    - Close the data file and force the data to disk.
    - Stop the hardware driver.
    - Make sure the timer is stopped.
    - Delete the graph.

********************************************************************/
CPostWaterTest::~CPostWaterTest()
  {

    // Close the data file and force the data to disk.
    // Currently have an open data file.  Close it.
  m_pDoc->vClose_data_file(true);
	
  if(m_pDoc->m_pDriver != NULL)
    m_pDoc->m_pDriver->vStop(); // Stop the driver.

  if(m_uTimer > 0)
    KillTimer(m_uTimer); // Stop the timer.

  if(m_hPEData != NULL)
    PEdestroy(m_hPEData); // Get rid of the graph.
  }


void CPostWaterTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPostWaterTest)
	DDX_Text(pDX, IDC_CLOCK, m_cstrClock);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPostWaterTest, CDialog)
	//{{AFX_MSG_MAP(CPostWaterTest)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_EXTEND_TEST, OnExtendTest)
	//}}AFX_MSG_MAP
  ON_NOTIFY(UDN_DELTAPOS, IDC_ADJUST_AMP, OnDeltaposAdjustAmp)
  ON_NOTIFY(UDN_DELTAPOS, IDC_ADJUST_FREQ, OnDeltaposAdjustFreq)
  ON_BN_CLICKED(IDC_POSTWATER_HELP, OnBnClickedPostwaterHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPostWaterTest message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
********************************************************************/
BOOL CPostWaterTest::OnInitDialog() 
  {
  CFont cfBoldFont;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();

    // Get the font to use in dialog box fields.
  vSet_font(this, NULL, &cfBoldFont, FW_BOLD);
    // Set the font in the dialog box fields.
  GetDlgItem(IDC_START)->SetFont(&cfBoldFont);
  GetDlgItem(IDC_EXTEND_TEST)->SetFont(&cfBoldFont);

  vSetup_graph(); // Set up the graph.

  /****************************/
    // If in fake data mode, show dialog box fields and initialize data.
  //if(AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER, INI_ITEM_USE_FAKE_DATA, 0) == 1)
  if(g_pConfig->iGet_item(CFG_USEFAKEDATA) == 1)
    {
    GetDlgItem(IDC_INC_AMP_TITLE)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_DEC_AMP_TITLE)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_ADJUST_AMP)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_INC_FREQ_TITLE)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_DEC_FREQ_TITLE)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_ADJUST_FREQ)->ShowWindow(SW_SHOW);
    ((CSpinButtonCtrl *)GetDlgItem(IDC_ADJUST_AMP))->SetRange(0, 1000); //(short int)m_pDoc->m_pDriver->m_fMaxEGG);
    ((CSpinButtonCtrl *)GetDlgItem(IDC_ADJUST_FREQ))->SetRange(0, 4);
    }
    /***************/

  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Research study
    GetDlgItem(IDC_EXTEND_TEST)->ShowWindow(SW_HIDE);
    GetDlgItem(IDOK)->EnableWindow(TRUE); // "Finished" button.
      // Set up the timing clock.
    m_cstrClock.Format("%02d:%02d", PW_MAX_TIME_R / 60, PW_MAX_TIME_R % 60);
    }
  else
    { // Standard study
      // Set up the timing clock.
    m_cstrClock.Format("%02d:%02d", PW_MAX_TIME / 60, PW_MAX_TIME % 60);
    }

  // Re-open the data file.
  m_pDoc->iReOpen_data_file();

  UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnStart

  Message handler for the Start button.
    This starts the recording.
********************************************************************/
void CPostWaterTest::OnStart() 
  {
  short int iRet;

  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Research version
    m_iClockTime = PW_MAX_TIME_R;
    }
  else
    { // Standard version
    m_iClockTime = PW_MAX_TIME; // 30 minutes in seconds.
    }
      // Initialize drivers.
  if(m_pDoc->m_pDriver == NULL)
    { // We don't have the driver initialized yet, so initialize it.
    m_pDoc->m_pDriver = new CDriver();
    iRet = m_pDoc->m_pDriver->iInit();
    }
  else
      // Driver has been initialized, but make sure it is reset.
    iRet = m_pDoc->m_pDriver->iReStart();
  if(iRet == SUCCESS)
    {
    m_fXPos = 0; // X-axis position.
    m_uTotalPts = 0;
    m_ZeroPntsTmr = 0; // Init how long we have been reading 0 points.
      // Disable the start button.
    GetDlgItem(IDC_START)->EnableWindow(FALSE);
      // Set up the timer to come in for reading the EGG device.
    m_iTimerCnt = 0;
    m_iRespClock = 599; // Set respiration clock to display dialog box
                        // on the first second.
      // Set the period to 1 less that where we are starting because it is
      // incremented before it is used.
    m_uPeriod = PERIOD_BL;
      // Set up the timer to come in every quarter second for reading the EGG device.
    m_uTimer = SetTimer(1, 250, NULL);
//    ((CEGGSASApp *)AfxGetApp())->vLog("**** Start Post water");
    }
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.
    Makes sure the user really wants to cancel the study.
********************************************************************/
void CPostWaterTest::OnCancel() 
  {
  CString cstrMsg, cstrCaption;

  //cstrMsg.LoadString(IDS_END_STUDY_ARE_YOU_SURE);
  cstrMsg = g_pLang->cstrLoad_string(ITEM_G_END_STUDY_SURE);
  //cstrCaption.LoadString(IDS_WARNING);
  cstrCaption = g_pLang->cstrLoad_string(ITEM_G_WARNING);
  if(MessageBox(cstrMsg, cstrCaption, MB_YESNO) == IDYES)
    {
    if(m_uTimer > 0)
      {
      KillTimer(m_uTimer); // Stop the timer.
      m_uTimer = 0;
      }
    CDialog::OnCancel();
    }
  }

/********************************************************************
OnOK

  Message handler for the OK button.
    Used after test is completed.
    If water load has not been entered, request it.
      It must be entered.
********************************************************************/
void CPostWaterTest::OnOK() 
  {
  CWaterLoad *pWaterLoadDlg;
  CFinishStudy *pFinishDlg;
  BOOL bClose;

	// TODO: Add extra validation here
  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Research version
      // Ask user if really finishing the study.
    pFinishDlg = new CFinishStudy();
    if(pFinishDlg->DoModal() == IDOK)
      { // Get the finish status.
      bClose = pFinishDlg->bGet_finish_sts();
      if(bClose == TRUE)
        { // User is finished with post-stimulation study.
/*******************************
          // Save the minutes.
        m_pDoc->m_pdPatient.fPeriodStart[1] = (float)0;
        //// TODO Allow for half minutes.
        m_pDoc->m_pdPatient.fPeriodLen[1] =
                    (float)((POST_STIMULATION_MAX_TIME_SEC - m_iClockTime) / 60);
          // Set the good minutes in the first period.
        m_pDoc->m_pdPatient.fGoodMinStart[1] = (float)0.0;
        m_pDoc->m_pdPatient.fGoodMinEnd[1] = m_pDoc->m_pdPatient.fPeriodLen[1];
****************************************/
          // The study has been completed from the previously paused state.
        m_pDoc->m_pdPatient.bPause = FALSE;
        m_pDoc->m_pdPatient.bManualMode = TRUE;
        m_pDoc->m_pdPatient.uPostStimDataPoints = m_uTotalPts * 2;
        m_pDoc->m_pdPatient.uNumPeriods = 1;
        m_pDoc->m_pdPatient.fPeriodStart[1] = (float)0.0;
        m_pDoc->m_pdPatient.fPeriodLen[1] = 
           fRound_down_to_half_min(
           (float)m_pDoc->m_pdPatient.uPostStimDataPoints
           / (float)(DATA_POINTS_PER_MIN_1 * 2));
        }
      }
    else
      bClose = FALSE;
    }
  else
    bClose = TRUE; // Standard version.
  if(bClose == TRUE)
    {
    if(m_uTimer > 0)
      {
      KillTimer(m_uTimer); // Stop the timer.
      m_uTimer = 0;
      }
    if(theApp.m_ptStudyType == PT_STANDARD)
      {
      if(m_pDoc->m_pdPatient.uWaterAmount == 0)
        { // Amount of water not entered.  We have to have it.
        pWaterLoadDlg = new CWaterLoad(m_pDoc, SM_NEW_STUDY, TYPE_WATER_AMOUNT);
        if(pWaterLoadDlg->DoModal() == IDOK)
          pWaterLoadDlg->vGet_water_amount(&m_pDoc->m_pdPatient);
        delete pWaterLoadDlg;
        }
      if(m_pDoc->m_pdPatient.uWaterAmount != 0)
        bClose = TRUE; // Got the water amount.  Ok to close.
      else
        bClose = FALSE;
      }
    }
  if(bClose == TRUE)
  	CDialog::OnOK();
  }

/********************************************************************
OnTimer

  Message handler for the Timer.
    Gets the data from the EGG hardware.
********************************************************************/
void CPostWaterTest::OnTimer(UINT nIDEvent) 
  {
  int iPtsToGraph, iCnt;
  float fXInc, fData[2], fScroll;
  float fEGGData[MAX_PTS_PER_READ], fRespData[MAX_PTS_PER_READ];
  short int iFileSts = SUCCESS;

	// TODO: Add your message handler code here and/or call default
  if(m_uTimer == nIDEvent)
    { // Update the clock.
    if(++m_iTimerCnt == 4)
      { // One second, update the clock.
      m_iTimerCnt = 0;
      if(--m_iClockTime <= 0)
        { // Postwater test period is up.
        if(theApp.m_ptStudyType == PT_RESEARCH)
          { // Make sure we have the correct number of data points.
          if(m_uTotalPts < ((PW_MAX_TIME_R / 60) * DATA_POINTS_PER_MIN_1))
            m_iClockTime = 1; // Need more data points.
          }
        if(m_iClockTime <= 0)
          {
          KillTimer(m_uTimer); // Stop the timer.
          m_uTimer = 0;
          GetDlgItem(IDC_START)->EnableWindow(FALSE);
          GetDlgItem(IDOK)->EnableWindow(TRUE); // "Finished" button.
          GetDlgItem(IDOK)->SetFocus();
          GetDlgItem(IDC_EXTEND_TEST)->EnableWindow(TRUE);
          }
        }
      m_cstrClock.Format("%02d:%02d", m_iClockTime / 60, m_iClockTime % 60);
      }
    if(m_iClockTime > 0)
      { // Get the data and graph the points.
        // Get the new values.
      iPtsToGraph = m_pDoc->m_pDriver->iGet_data_all(fEGGData, fRespData);
      if(iPtsToGraph == FAIL_EGG_DRIVER_LOW_BATT)
        { // On a low battery warning, try reading one more time.  This should
          // actually read the hardware.
        iPtsToGraph = m_pDoc->m_pDriver->iGet_data_all(fEGGData, fRespData);
        }
      if(iPtsToGraph > 0)
        {
        m_ZeroPntsTmr = 0; // Init how long we have been reading 0 points.
          // Append new values and examine the signal
        m_uTotalPts += (unsigned)iPtsToGraph;
        fXInc = 1.0F / (float)(DATA_POINTS_PER_MIN_1); //(float)X_MAX_BL_SCALE;
        for(iCnt = 0; iCnt < iPtsToGraph; ++iCnt, m_fXPos += fXInc)
          {
          fData[0] = fEGGData[iCnt];
          fData[1] = fRespData[iCnt];
          PEvset(m_hPEData, PEP_faAPPENDYDATA, fData, 1); // New Y value.

            // Write the data points to the patient data file.
          if((iFileSts = m_pDoc->iWrite_data_file(fData, 2 * sizeof(float))) != SUCCESS)
            break;

          fData[0] = m_fXPos;
          fData[1] = m_fXPos;
          PEvset(m_hPEData, PEP_faAPPENDXDATA, fData, 1);// New X value. 
          }
//          // Switch to AutoScaling x axis after receiving X_MAX_SCALE data points
//        if(m_fXPos == X_MAX_BL_SCALE)
//          PEnset(m_hPEData, PEP_nMANUALSCALECONTROLX, PEMSC_NONE);
        if(m_uTotalPts > (unsigned)PW_TOTAL_PNTS_IN_WINDOW) //(600 * 4.267))
          { // Scroll the X-axis left by the number of points just received.
          fScroll = (float)iPtsToGraph * (10.0F / PW_TOTAL_PNTS_IN_WINDOW);
          m_dMinXScale += (double)fScroll;
          PEvset(m_hPEData, PEP_fMANUALMINX, &m_dMinXScale, 1);
          m_dMaxXScale += (double)fScroll;
          PEvset(m_hPEData, PEP_fMANUALMAXX, &m_dMaxXScale, 1);
          }
        PEreinitialize(m_hPEData);
        PEresetimage(m_hPEData, 0, 0);
        ::InvalidateRect(m_hPEData, NULL, FALSE);
        }
      if(iPtsToGraph <= 0 || iFileSts != SUCCESS)
        { // Possible error reading the device or file error.
        CString cstrMsg, cstrErr;
        if(iPtsToGraph == 0)
          { // If get 0 points for a pre-determined amount of time, then
            // something is wrong with the hardware.
          if(++m_ZeroPntsTmr >= ZERO_POINTS_MAX_COUNT)
            { // getting zero points for too long a period of time.
            //cstrErr.LoadString(IDS_READING_ZERO_POINTS);
            cstrErr = g_pLang->cstrLoad_string(ITEM_G_NOT_READING_DATA);
            //cstrMsg.Format(IDS_ERR_EGG_READ, cstrErr);
            cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_EGG_READ), cstrErr);
            iPtsToGraph = FAIL_EGG_DRIVER;
            }
          }
        else // if(iPtsToGraph < 0)
          { // Error from driver.
          //cstrMsg.Format(IDS_ERR_EGG_READ, m_pDoc->m_pDriver->m_cstrErr);
          cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_EGG_READ), m_pDoc->m_pDriver->m_cstrErr);
          }
        if(iPtsToGraph < 0)
          { // Get an error, stop the test.
          KillTimer(m_uTimer); // Stop the timer.
          m_uTimer = 0;
            // Make sure the OK button is disabled.
          if(iFileSts != SUCCESS)
            cstrMsg = m_pDoc->m_cstrErrMsg; // Got a file error, get the error message.
          //cstrErr.LoadString(IDS_ERROR1);
          cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERROR);
          theApp.vLog(cstrMsg);
          MessageBox(cstrMsg, cstrErr, MB_OK);
          }
        }
      }
    if(theApp.m_ptStudyType == PT_STANDARD)
      { // Standard version
      if(m_iTimerCnt == 0 && ++m_iRespClock == 600)
        { // Start of a 10 minute period.
        m_iRespClock = 0;
        ++m_uPeriod;
          // Ask for the respiration rate.
        if(m_pdlgRespRate != NULL)
          { // Dialog box is already there, change the period title.
          m_pdlgRespRate->vSet_period(m_uPeriod);
          }
        else
          {
          m_pdlgRespRate = new CRespRate(m_iGraphBottom, m_uPeriod, this);
          m_pdlgRespRate->DoModal();
          m_pDoc->m_pdPatient.uRespRate[m_uPeriod] = m_pdlgRespRate->uGet_resp_rate();
          delete m_pdlgRespRate;
          m_pdlgRespRate = NULL;

            //{
            //CString cstrMsg;
            //cstrMsg.Format("@@@ DEBUG: vFileNewPatient(respiration): Period 0 len: %.1f, Period 0 start: %.1f.",
            //   m_pDoc->m_pdPatient.fPeriodLen[0], m_pDoc->m_pdPatient.fPeriodStart[0]);
            //theApp.vLog(cstrMsg);
            //}

          }
        }
      }
    UpdateData(FALSE);
    }
	
	CDialog::OnTimer(nIDEvent);
  }

/********************************************************************
OnExtendTest

  Message handler for the Extend Test button.
    Start the timer for an additional 10 minutes.
********************************************************************/
void CPostWaterTest::OnExtendTest() 
  {
	
  m_iClockTime = 600;
    // Clear out the driver data buffers in case they have gotten
    // full because the user didn't select this button soon enough.
  m_pDoc->m_pDriver->vClear_data_buffers();
    // Indicate the study has an extra 10 minutes.
  m_pDoc->m_pdPatient.bExtra10Min = TRUE;
    // Set the timer for 250 msec.
  m_uTimer = SetTimer(1, 250, NULL);
  GetDlgItem(IDOK)->EnableWindow(FALSE);
  GetDlgItem(IDC_EXTEND_TEST)->EnableWindow(FALSE);
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
void CPostWaterTest::OnDeltaposAdjustAmp(NMHDR *pNMHDR, LRESULT *pResult)
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
void CPostWaterTest::OnDeltaposAdjustFreq(NMHDR *pNMHDR, LRESULT *pResult)
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
OnBnClickedPostwaterHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CPostWaterTest::OnBnClickedPostwaterHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_POSTWATER_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_POSTWATER_HELP);
  }

/********************************************************************
OnCommand

  Event handler for processing the WM_COMMAND message
  For Research Version.
    Process the left mouse click on one of the graph window.
       Displays the EventRecorder dialog box.
********************************************************************/
BOOL CPostWaterTest::OnCommand(WPARAM wParam, LPARAM lParam)
  {

  switch (HIWORD(wParam))
    {
    case PEWN_CLICKED:  // Mouse left button click in the graph window.
      HOTSPOTDATA HSData;
      CEventRecorder *pdlg;
      CEvnt *pEvent;
      int iDist, iDPIndex;

        // HSData.w1 = subset index, HSData.w2 = point index
      PEvget(m_hPEData, PEP_structHOTSPOTDATA, &HSData);
      if (HSData.nHotSpotType == PEHS_DATAPOINT)
        {
        pEvent = new CEvnt();
          // The data point at the cursor (HSData.w2) is actually the
          // last data point on the graph window minus the difference
          // between the last data point on the graph window and the
          // actual index of the data point at the cursor.
          // To find out the real index of the data point at the cursor:
        iDist = (int)PW_TOTAL_PNTS_IN_WINDOW - HSData.w2;
        iDPIndex = m_uTotalPts - iDist;
        pEvent->m_iDataPointIndex = iDPIndex;
        pEvent->m_iSeconds = (iDPIndex  * 60) / 256;
        pdlg = new CEventRecorder(pEvent->m_iSeconds);
        if(pdlg->DoModal() == IDOK)
          {
          pEvent->m_cstrDescription = pdlg->cstrGet_description();
          pEvent->m_cstrIndications = pdlg->cstrGet_indications();
          pEvent->m_iPeriodType = EVENT_POST_STIM;
          m_pDoc->m_pdPatient.Events.m_PSEvents.Add(pEvent);
          }
        else
          delete pEvent; // Canceled, not added to event list.
        delete pdlg;
        } 
      return TRUE;
    }

  return CDialog::OnCommand(wParam, lParam);
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
void CPostWaterTest::vSetup_graph()
  {    
  RECT rButton, rGraph, rClock, rWin;
  POINT ptGraphBottom;
  STRIP_CHART_ATTRIB scaGraph;

  GetClientRect(&rGraph); // Dialog box client window rectangle
    // Get the coodinates of the Clock field and the Start Test button 
    // in terms of this window and adjust the graphic window fit between
    // the bottom of the clock field and the top of the button.
  GetDlgItem(IDC_CLOCK)->GetClientRect(&rClock);
  GetDlgItem(IDC_CLOCK)->MapWindowPoints(this, &rClock);
  GetDlgItem(IDC_START)->GetClientRect(&rButton);
  GetDlgItem(IDC_START)->MapWindowPoints(this, &rButton);
    // Leave 10 pixels between the bottom of the clock field and the
    // top of the graph window.
  rGraph.top = rClock.bottom + 5;
    // Leave 10 pixels between the bottom of the graph window and the
    // top of the Start Test button.
  rGraph.bottom = rButton.top - 5;
    // Calculate the bottom of the graph in screen coordinates in order to display the
    // dialog box requesting the respiration rate.
  GetWindowRect(&rWin); // screen coordinates of dialog box.
  ptGraphBottom.x = 0; //rWin.left;
  ptGraphBottom.y = rButton.bottom;
  ClientToScreen(&ptGraphBottom);
  m_iGraphBottom = ptGraphBottom.y + rWin.top;

     // Construct the graph object.
  m_hPEData = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rGraph, m_hWnd, IDD_EQUIP_TEST);

    // Set the strip chart attributes;
  scaGraph.dMaxXScale = (float)PW_X_MAX_SCALE;
//  scaGraph.iMaxPoints = (int)PW_TOTAL_PNTS_IN_WINDOW + 1;
  scaGraph.iTotalPoints = (int)PW_TOTAL_PNTS_IN_WINDOW + 1; //(int)((PW_TOTAL_PNTS_IN_WINDOW + 1) * TOTAL_PERIODS);
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
  m_dMaxXScale = (float)PW_X_MAX_SCALE; // Numbers are displayed every half minute.

  PEreinitialize(m_hPEData);
  PEresetimage(m_hPEData, 0, 0);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CPostWaterTest::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_POST_WATER_TEST);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_PW_STUDYPHASE));

    GetDlgItem(IDC_START)->SetWindowText(g_pLang->cstrGet_text(ITEM_PW_BEGINSTUDY));
    GetDlgItem(IDC_EXTEND_TEST)->SetWindowText(g_pLang->cstrGet_text(ITEM_PW_EXTENDTEST));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrGet_text(ITEM_PW_FINISHED));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_POSTWATER_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
   }
  }



