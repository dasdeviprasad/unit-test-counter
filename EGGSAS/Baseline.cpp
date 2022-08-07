/********************************************************************
Baseline.cpp

Copyright (C) 2003,2004,2005,2007,2012,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CBaseline class.

  This class runs the baseline measurement.

  The hardware is read from the Timer routine which is set to
  come in on the quarter second.  Since there are 4.267 data values
  every second, the timer periodically will get 2 values from the
  hardware.


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
17-MAY-07  RET  Research Version
                  Change Extend Baseline button to Stop Baseline button.
                    Change message handler.
                  Change Water Load button to Stimulation Medium button.
                  Add Pause Study and Skip Stimulation Medium buttons and
                    corresponding message handlers.
                  Change OnInitDialog() to display additional buttons and
                    move buttons around so they all fit.
                  Change OnOK() to indicate next action.
                  Add methods: vGet_action(), uGet_points_in_baseline()
                  Change OnTimer() so that after a predefined time (4 minutes)
                    the Stop button is enabled.  At least 4 minutes is needed
                    to perform an analysis.
25-AUG-07  RET  Version 2.02
                  Add second message box to pausing a study to tell user
                    how to resume study.
18-FEB-12  RET  Version 
                  Changes for foreign languages.
28-FEB-12  RET  Version 
             Add data storage location.
               Change methods: Destructor
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedBaselineHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "Baseline.h"
#include "EventRecorder.h"
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
// CBaseline dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the active document that started this dialog box.
          Pointer to the window that started this dialog box.

********************************************************************/
CBaseline::CBaseline(CEGGSASDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CBaseline::IDD, pParent)
  {

	//{{AFX_DATA_INIT(CBaseline)
	m_cstrClock = _T("");
	//}}AFX_DATA_INIT

  m_pDoc = pDoc;
  m_uTimer = 0;
  m_hPEData = NULL;
  m_bExtend = false;
  m_uAction = BL_ACTION_NONE;
  }

/********************************************************************
Destructor

  Clean up class data.
    Make sure the timer is stopped.
    Destroy the window and data associated with the graph.

********************************************************************/
CBaseline::~CBaseline()
  {

    // Close the data file and force the data to disk.
    // Currently have an open data file.  Close it.
  m_pDoc->vClose_data_file(true);

  if(m_uTimer > 0)
    KillTimer(m_uTimer);

  if(m_hPEData != NULL)
    PEdestroy(m_hPEData);
  }

void CBaseline::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
//{{AFX_DATA_MAP(CBaseline)
DDX_Text(pDX, IDC_CLOCK, m_cstrClock);
//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBaseline, CDialog)
	//{{AFX_MSG_MAP(CBaseline)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_START_BL, OnStartBl)
	ON_BN_CLICKED(IDC_EXTEND, OnExtend)
	//}}AFX_MSG_MAP
  ON_NOTIFY(UDN_DELTAPOS, IDC_ADJUST_AMP, OnDeltaposAdjustAmp)
  ON_NOTIFY(UDN_DELTAPOS, IDC_ADJUST_FREQ, OnDeltaposAdjustFreq)
  ON_BN_CLICKED(IDC_BASELINE_HELP, OnBnClickedBaselineHelp)
  ON_BN_CLICKED(IDC_PAUSE_STUDY, OnBnClickedPauseStudy)
  ON_BN_CLICKED(IDC_SKIP_STIM_MED, OnBnClickedSkipStimMed)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseline message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Set up initial values for various fields in the dialog box.
    Create and setup the graph.
********************************************************************/
BOOL CBaseline::OnInitDialog() 
  {
  CFont cfBoldFont;
  CString cstrText;
  CWnd *pcwnd;
  RECT rField;
  CDC *pDC;
  WINDOWPLACEMENT wp, wp1;
  int iDistMoved, iWidth;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();

    // Get the font to use in dialog box fields.
  vSet_font(this, NULL, &cfBoldFont, FW_BOLD);
    // Set the font in the dialog box fields.
  GetDlgItem(IDC_START_BL)->SetFont(&cfBoldFont);
  GetDlgItem(IDC_EXTEND)->SetFont(&cfBoldFont);

  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Research version
    m_iMaxTime = MAX_BL_TIME_RESEARCH;
    //cstrText.LoadString(IDS_STOP_BASELINE);
    pcwnd = GetDlgItem(IDC_EXTEND);
//    pcwnd->EnableWindow(TRUE);
    pcwnd->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_STOP_BASELINE));
      // Set the button to the same dimensions as the start button.
    GetDlgItem(IDC_START_BL)->GetWindowRect(&rField); // Dimension of the button.
    pcwnd->SetWindowPos(&CWnd::wndBottom, rField.left, rField.top,
                        rField.right - rField.left, rField.bottom - rField.top,
                        SWP_NOMOVE|SWP_NOACTIVATE);
    pcwnd->EnableWindow(FALSE);
    GetDlgItem(IDC_PAUSE_STUDY)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_PAUSE_STUDY)->EnableWindow(FALSE);
      // Change the "Water Load" button text.
    pcwnd = GetDlgItem(IDOK);
    pDC = pcwnd->GetDC();
    pcwnd->GetWindowRect(&rField); // Dimension of the button.
    pcwnd->GetWindowText(cstrText);
    CSize csTextExtent = pDC->GetTextExtent(cstrText);
    //cstrText.LoadString(IDS_STIMULATION_MEDIUM);
    cstrText = g_pLang->cstrLoad_string(ITEM_G_STIMULATION_MEDIUM);
    pcwnd->SetWindowText(cstrText);
    CSize csNewTextExtent = pDC->GetTextExtent(cstrText);
    iWidth = (rField.right - rField.left) + (csNewTextExtent.cx - csTextExtent.cx) / 2;
    pcwnd->SetWindowPos(&CWnd::wndBottom, rField.left, rField.top,
                        iWidth, rField.bottom - rField.top,
                        SWP_NOMOVE|SWP_NOACTIVATE);
      // Now position the "Skip Stimulation Medium" button and show it.
      // Move the Stimulation Medium button to the right.
    GetWindowRect(&rField); // Rectangle of dialog box.
    pcwnd = GetDlgItem(IDOK);
    pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
    iWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
    wp.rcNormalPosition.right = rField.right - 10;
    wp.rcNormalPosition.left = wp.rcNormalPosition.right - iWidth;
    pcwnd->SetWindowPlacement(&wp);
      // Move the "Pause Study" button.
    GetDlgItem(IDC_EXTEND)->GetWindowRect(&rField); // Dimension of the button.
    pcwnd = GetDlgItem(IDC_PAUSE_STUDY);
    pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
    iDistMoved = rField.right;
    pcwnd->GetWindowRect(&rField); // Dimension of the button.
    iDistMoved = rField.left - iDistMoved - 20;
    wp.rcNormalPosition.right -= iDistMoved;
    wp.rcNormalPosition.left -= iDistMoved;
    pcwnd->SetWindowPlacement(&wp);
      // Now move the "Skip Stimulation Medium" button.
    pcwnd = GetDlgItem(IDC_SKIP_STIM_MED);
    pcwnd->GetWindowPlacement(&wp1); // Dimensions and position of field.
    iWidth = wp1.rcNormalPosition.right - wp1.rcNormalPosition.left;
    int iHeight = wp1.rcNormalPosition.bottom - wp1.rcNormalPosition.top;
    wp1.rcNormalPosition.left = wp.rcNormalPosition.right + 10;
    wp1.rcNormalPosition.right = wp1.rcNormalPosition.left + iWidth;
    wp1.rcNormalPosition.top = wp.rcNormalPosition.top;
    wp1.rcNormalPosition.bottom = wp1.rcNormalPosition.top + iHeight;
    pcwnd->SetWindowPlacement(&wp1);
    pcwnd->ShowWindow(SW_SHOW);
    pcwnd->EnableWindow(FALSE);
    }
  else
    { // Standard version
    m_iMaxTime = MAX_BL_TIME;
    }

  m_cstrClock.Format("%02d:%02d", m_iMaxTime / 60, m_iMaxTime % 60);

  vSetup_graph();

  /*****************************/
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
    ((CSpinButtonCtrl *)GetDlgItem(IDC_ADJUST_AMP))->SetRange(0, (short int)m_pDoc->m_pDriver->m_fMaxEGG);
    ((CSpinButtonCtrl *)GetDlgItem(IDC_ADJUST_FREQ))->SetRange(0, 4);
    }
    /******************************/

  UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnTimer

  Message handler for the Timer.
    Gets the data from the EGG hardware.
    Graph the data and write it to the data file.
    If there is an error, stop the test.
    Update the countdown clock
********************************************************************/
void CBaseline::OnTimer(UINT nIDEvent) 
  {
  int iPtsToGraph, iCnt;
  float fXInc, fData[2], fScroll;
  float fEGGData[MAX_PTS_PER_READ], fRespData[MAX_PTS_PER_READ];
  short int iFileSts = SUCCESS;

  if(m_uTimer == nIDEvent)
    { // Update the clock.
    if(++m_iTimerCnt == 4)
      { // One second, update the clock.
      m_iTimerCnt = 0;
      if(theApp.m_ptStudyType == PT_RESEARCH)
        { // Research version
        if(++m_uStopEnableCnt == 240)
          { // 4 minutes has elapsed.
            // Enable the Stop button.
          GetDlgItem(IDC_EXTEND)->EnableWindow(TRUE);
          }
        }
      if(--m_iClockTime <= 0)
        { // Baseline period is up.
        if(theApp.m_ptStudyType == PT_RESEARCH)
          { // Make sure we have the correct number of data points.
          if(m_uTotalPts < ((MAX_BL_TIME_RESEARCH / 60) * DATA_POINTS_PER_MIN_1))
            m_iClockTime = 1; // Need more data points.
          }
        if(m_iClockTime <= 0)
          {
          KillTimer(m_uTimer); // Stop the timer.
          m_uTimer = 0;
          GetDlgItem(IDOK)->EnableWindow(TRUE);
          m_iClockTime = 0;
          if(theApp.m_ptStudyType == PT_STANDARD)
            {
            if(m_bExtend == false)
              GetDlgItem(IDC_EXTEND)->EnableWindow(TRUE);
            }
          else
            { // Research Version
            GetDlgItem(IDC_PAUSE_STUDY)->EnableWindow(TRUE);
            GetDlgItem(IDC_SKIP_STIM_MED)->EnableWindow(TRUE);
            }
          }
        }
        // Only update clock every second.
      m_cstrClock.Format("%02d:%02d", m_iClockTime / 60, m_iClockTime % 60);
      }
    if(m_iClockTime > 0)
      { // Get the data and graph the points.
        // Get the new values.
      iPtsToGraph = m_pDoc->m_pDriver->iGet_data_all(fEGGData, fRespData);
      if(iPtsToGraph > 0)
        {
        m_ZeroPntsTmr = 0;  // Reset zero points timer.
        m_uTotalPts += (unsigned)iPtsToGraph;
          // Append new values and examine the signal
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

        if(theApp.m_ptStudyType == PT_RESEARCH)
          { // Research version
          if(m_uTotalPts > (unsigned)BL_TOTAL_PNTS_IN_WINDOW) //(600 * 4.267))
            { // Scroll the X-axis left by the number of points just received.
            fScroll = (float)iPtsToGraph * (10.0F / BL_TOTAL_PNTS_IN_WINDOW);
            m_dMinXScale += (double)fScroll;
            PEvset(m_hPEData, PEP_fMANUALMINX, &m_dMinXScale, 1);
            m_dMaxXScale += (double)fScroll;
            PEvset(m_hPEData, PEP_fMANUALMAXX, &m_dMaxXScale, 1);
            }
          }

//          // Switch to AutoScaling x axis after receiving X_MAX_SCALE data points
//        if(m_fXPos == X_MAX_BL_SCALE)
//          PEnset(m_hPEData, PEP_nMANUALSCALECONTROLX, PEMSC_NONE);

//        if(m_uTotalPts > (unsigned)PW_TOTAL_PNTS_IN_WINDOW) //(600 * 4.267))
        if(m_bExtend == true)
          { // Scroll the X-axis left by the number of points just received.
          fScroll = (float)iPtsToGraph * (10.0F / BL_TOTAL_PNTS_IN_WINDOW);
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
        { // Error reading the device.  Stop the test.
        CString cstrMsg, cstrErr;
        if(iPtsToGraph == 0)
          { // If get 0 points for a pre-determined amount of time, then
            // something is wrong with the hardware.
          if(++m_ZeroPntsTmr >= ZERO_POINTS_MAX_COUNT)
            { // getting zero points for too long a period of time.
            //cstrErr.LoadString(IDS_READING_ZERO_POINTS);
            cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERROR);
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
        if(iPtsToGraph < 0 || iFileSts != SUCCESS)
          { // Get an error, stop the test.
          KillTimer(m_uTimer); // Stop the timer.
          m_uTimer = 0;
            // Make sure the "OK" button is disabled.
          GetDlgItem(IDOK)->EnableWindow(FALSE);
            // Make sure the "Start Baseline" button is disabled.
          GetDlgItem(IDC_START_BL)->EnableWindow(FALSE);
          if(iFileSts != SUCCESS)
            cstrMsg = m_pDoc->m_cstrErrMsg; // Got a file error, get the error message.
          //cstrErr.LoadString(IDS_ERROR1);
          cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERROR);
          theApp.vLog(cstrMsg);
          MessageBox(cstrMsg, cstrErr, MB_OK);
          }
        }
      }
    UpdateData(FALSE);
    }
	
	CDialog::OnTimer(nIDEvent);
  }

/********************************************************************
OnStartBl

  Message handler for the Start baseline button.
    This start the recording.
      Make sure the driver is running and has no accumulated data.
      Start the timer.
      Disable the start button.
********************************************************************/
void CBaseline::OnStartBl() 
  {
  short int iRet;

  m_iClockTime = m_iMaxTime; // Max run time in seconds.
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
    m_uTotalPts = 0;
    m_uStopEnableCnt = 0;
    m_fXPos = 0; // X-axis position.
    m_ZeroPntsTmr = 0; // Init how long we have been reading 0 points.
      // Disable the start button.
    GetDlgItem(IDC_START_BL)->EnableWindow(FALSE);
      // Set up the timer to come in for reading the EGG device.
    m_iTimerCnt = 0;
      // Set timer for every quarter second.
    m_uTimer = SetTimer(1, 250, NULL);
//    ((CEGGSASApp *)AfxGetApp())->vLog("**** Start baseline");
//    if(theApp.m_ptStudyType == PT_RESEARCH)
//      { // Research version
//      GetDlgItem(IDC_EXTEND)->EnableWindow(TRUE);
//      }
    }
  }

/********************************************************************
OnBnClickedPauseStudy

  Message handler for the Pause Study button.
    Display a message to ask user if user is sure the study should
    be paused.
    If user pauses the study:
      Stop the timer.
      Stop the driver.
********************************************************************/
void CBaseline::OnBnClickedPauseStudy()
  {
  CString cstrCaption, cstrText, cstrText1;

  //cstrCaption.LoadString(IDS_WARNING);
  cstrCaption = g_pLang->cstrLoad_string(ITEM_G_WARNING);
  //cstrText.LoadString(IDS_PAUSE_CONTINUE_BASELINE);
  cstrText = g_pLang->cstrLoad_string(ITEM_G_PAUSE_CONTINUE_BASELINE)
    + "\n" + g_pLang->cstrLoad_string(ITEM_G_PAUSE_CONTINUE_BASELINE1)
    + "\n\n" + g_pLang->cstrLoad_string(ITEM_G_RESUME_TO_CONTINUE);
  //cstrText += "\n\n";
  //cstrText1.LoadString(IDS_RESUME_TO_CONTINUE);
  //cstrText += cstrText1;
  if(MessageBox(cstrText, cstrCaption, MB_YESNO) == IDYES)
    { // Stopping the baseline and pausing the study.
    if(m_pDoc->m_pDriver != NULL)
      m_pDoc->m_pDriver->vStop(); // Stop the driver.
    m_uAction = BL_ACTION_PAUSE;
    }
  OnOK();
  }

/********************************************************************
OnBnClickedSkipStimMed

  Message handler for the Skip Stimulation Medium button.
    Stop the timer.
********************************************************************/
void CBaseline::OnBnClickedSkipStimMed()
  {

  m_uAction = BL_ACTION_SKIP_STIM_MED;
  OnOK();
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.
    Display a message that this will cancel the test and allow the
      user to cancel the Cancel button.
    If user cancels the test:
      Stop the timer.
      Stop the driver.
********************************************************************/
void CBaseline::OnCancel() 
  {
  CString cstrMsg, cstrCaption;

  //cstrMsg.LoadString(IDS_END_STUDY_ARE_YOU_SURE);
  cstrMsg = g_pLang->cstrLoad_string(ITEM_G_END_STUDY_SURE);
  //cstrCaption.LoadString(IDS_WARNING);
  cstrCaption = g_pLang->cstrLoad_string(ITEM_G_WARNING);
  if(MessageBox(cstrMsg, cstrCaption, MB_YESNO) == IDYES)
    {
    if(m_uTimer != 0)
      {
      KillTimer(m_uTimer); // Stop the timer.
      m_uTimer = 0;
      }
    if(m_pDoc->m_pDriver != NULL)
      m_pDoc->m_pDriver->vStop(); // Stop the driver.
    CDialog::OnCancel();
    }
  }

/********************************************************************
OnOK

  Message handler for the OK button.
    Stop the timer.
********************************************************************/
void CBaseline::OnOK() 
  {

	// TODO: Add extra validation here
  if(m_uTimer != 0)
    {
    KillTimer(m_uTimer); // Stop the timer.
    m_uTimer = 0;
    }
  // Add the baseline trailer to the file.
  vAdd_baseline_trailer();
    // Don't set the next action if its already been set by
    // another button.
  if(m_uAction == BL_ACTION_NONE)
    m_uAction = BL_ACTION_STIM_MED;
	
	CDialog::OnOK();
  }

/********************************************************************
OnExtend

  Standard version
    Message handler for the "Extend Baseline for 10 minutes" button.
      Update the baseline period length and the start period minutes
        to reflect the extra 10 minutes.
      Start the timer.
  Research version
    Message handler for the "Stop Baseline" button.
********************************************************************/
void CBaseline::OnExtend() 
  {
  short int iIndex;
  CString cstrCaption, cstrText;

  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Research version. This is "Stop Baseline" button.
    //cstrCaption.LoadString(IDS_WARNING);
    cstrCaption = g_pLang->cstrLoad_string(ITEM_G_WARNING);
    //cstrText.LoadString(IDS_STOP_CONTINUE_BASELINE);
    cstrText = g_pLang->cstrLoad_string(ITEM_G_STOP_CONTINUE_BASELINE);
    if(MessageBox(cstrText, cstrCaption, MB_YESNO) == IDYES)
      { // Stopping the baseline.
      GetDlgItem(IDC_PAUSE_STUDY)->EnableWindow(TRUE);
      GetDlgItem(IDOK)->EnableWindow(TRUE);
      GetDlgItem(IDC_SKIP_STIM_MED)->EnableWindow(TRUE);
      GetDlgItem(IDC_EXTEND)->EnableWindow(FALSE);
      if(m_uTimer != 0)
        {
        KillTimer(m_uTimer); // Stop the timer.
        m_uTimer = 0;
        }
      }
    }
  else
    { // Standard version
    m_iClockTime = 600;
    GetDlgItem(IDOK)->EnableWindow(FALSE);
    GetDlgItem(IDC_EXTEND)->EnableWindow(FALSE);
      // Adjust period start times and period length in the document.
    m_pDoc->m_pdPatient.fPeriodLen[PERIOD_BL] += (float)10;
    for(iIndex = PERIOD_10; iIndex < TOTAL_PERIODS; ++iIndex)
      m_pDoc->m_pdPatient.fPeriodStart[iIndex] += (float)10.0;
    m_bExtend = true;
      // Clear out the driver data buffers in case they have gotten
      // full because the user didn't select this button soon enough.
    m_pDoc->m_pDriver->vClear_data_buffers();
      // Start the timer for 150 msec intervals.
    m_uTimer = SetTimer(1, 250, NULL);
    }
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
void CBaseline::OnDeltaposAdjustAmp(NMHDR *pNMHDR, LRESULT *pResult)
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
void CBaseline::OnDeltaposAdjustFreq(NMHDR *pNMHDR, LRESULT *pResult)
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
OnBnClickedBaselineHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CBaseline::OnBnClickedBaselineHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_BASELINE_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_BASELINE_HELP);
  }

/********************************************************************
OnCommand

  Event handler for processing the WM_COMMAND message
  For Research Version.
    Process the left mouse click on one of the graph window.
       Displays the EventRecorder dialog box.
********************************************************************/
BOOL CBaseline::OnCommand(WPARAM wParam, LPARAM lParam)
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
        iDist = (int)BL_TOTAL_PNTS_IN_WINDOW - HSData.w2;
        iDPIndex = m_uTotalPts - iDist;
        pEvent->m_iDataPointIndex = iDPIndex;
        pEvent->m_iSeconds = (iDPIndex  * 60) / 256;
        pdlg = new CEventRecorder(pEvent->m_iSeconds);
        if(pdlg->DoModal() == IDOK)
          {
          pEvent->m_cstrDescription = pdlg->cstrGet_description();
          pEvent->m_cstrIndications = pdlg->cstrGet_indications();
          pEvent->m_iPeriodType = EVENT_BASELINE;
          m_pDoc->m_pdPatient.Events.m_BLEvents.Add(pEvent);
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
void CBaseline::vSetup_graph()
  {    
  RECT rButton, rGraph, rClock;
  STRIP_CHART_ATTRIB scaGraph;

  GetClientRect(&rGraph); // Dialog box client window rectangle
    // Get the coodinates of the Clock field and the Start Test button 
    // in terms of this window and adjust the graphic window fit between
    // the bottom of the clock field and the top of the button.
  GetDlgItem(IDC_CLOCK)->GetClientRect(&rClock);
  GetDlgItem(IDC_CLOCK)->MapWindowPoints(this, &rClock);
  GetDlgItem(IDC_START_BL)->GetClientRect(&rButton);
  GetDlgItem(IDC_START_BL)->MapWindowPoints(this, &rButton);
    // Leave 10 pixels between the bottom of the clock field and the
    // top of the graph window.
  rGraph.top = rClock.bottom + 5;
    // Leave 10 pixels between the bottom of the graph window and the
    // top of the Start Test button.
  rGraph.bottom = rButton.top - 5;

     // Construct the graph object.
  m_hPEData = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rGraph, m_hWnd, IDD_BASELINE); //IDD_EQUIP_TEST);

    // Set the strip chart attributes;
  scaGraph.dMaxXScale = (float)(X_MAX_BL_SCALE / 60 / 4);
//  scaGraph.iMaxPoints = (int)((float)MAX_BL_TIME * DATA_POINTS_PER_SECOND) + 1;
//    // Allow for 10 min baseline plus an extra 10 minutes.
//  scaGraph.iTotalPoints = (int)((float)MAX_BL_TIME * DATA_POINTS_PER_SECOND * 2) + 1;
//  scaGraph.iTotalPoints = (int)((float)m_iMaxTime * DATA_POINTS_PER_SECOND) + 1;
  scaGraph.iTotalPoints = (int)BL_TOTAL_PNTS_IN_WINDOW + 1;
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
  m_dMaxXScale = (float)(X_MAX_BL_SCALE / 60 / 4); // Numbers are displayed every half minute.

  PEreinitialize(m_hPEData);
  PEresetimage(m_hPEData, 0, 0);
  }

/********************************************************************
vGet_action - Get the next action to take.

  inputs: None.
  
  Output: None.

  return: Next action to take.
********************************************************************/
unsigned short CBaseline::uGet_action()
  {
  unsigned short uAction;

  if(theApp.m_ptStudyType == PT_RESEARCH)
    { // Research version
    uAction = m_uAction;
      // Don't allow no action.
    if(uAction == BL_ACTION_NONE)
      uAction = BL_ACTION_STIM_MED;
    }
  else
    { // Standard version.  Force Water Load.
    uAction = BL_ACTION_STIM_MED;
    }
  return(uAction);
  }

/********************************************************************
uGet_points_in_baseline - Get the number of data points in the baseline.
                          Each point is actually 2 data points, EGG and
                          respiration.

  inputs: None.
  
  Output: None.

  return: Number of data points.
********************************************************************/
unsigned CBaseline::uGet_points_in_baseline()
  {
  return(m_uTotalPts);
  }

/********************************************************************
vAdd_baseline_trailer - Write the Baseline Trailer including the actual
                        number of data samples to the data file.

  inputs: None.
  
  Output: Appends the BASELINE_TRAILER structure to the data file.

  return: None.
********************************************************************/
void CBaseline::vAdd_baseline_trailer()
  {

  if(theApp.m_ptStudyType == PT_RESEARCH)
    {
    BASELINE_TRAILER bt;

    bt.uStartWord1 = 0xFFFF;
    bt.uStartWord2 = 0xFFFF;
    bt.uStartWord3 = 0xFFFF;
    bt.uStartWord4 = 0xFFFF;
    bt.uEndWord1 = 0xFFFF;
    bt.uEndWord2 = 0xFFFF;
    bt.uEndWord3 = 0xFFFF;
    bt.uEndWord4 = 0xFFFF;
    bt.lSpare = 0;
      // Each point is actually 2 data samples (EGG and Respiration).
    bt.lDataSamples = m_uTotalPts * 2;
      // Write the baseline trailer to the data file.
    m_pDoc->iWrite_data_file((const void *)&bt, sizeof(BASELINE_TRAILER));
    }
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CBaseline::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_BASELINE);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_BL_BASELINE));

    GetDlgItem(IDC_START_BL)->SetWindowText(g_pLang->cstrGet_text(ITEM_BL_START_BL));
    GetDlgItem(IDC_EXTEND)->SetWindowText(g_pLang->cstrGet_text(ITEM_BL_EXTEND_BL));
    GetDlgItem(IDC_PAUSE_STUDY)->SetWindowText(g_pLang->cstrGet_text(ITEM_BL_PAUSE_STUDY));
    GetDlgItem(IDC_SKIP_STIM_MED)->SetWindowText(g_pLang->cstrGet_text(ITEM_BL_SKIP_STIM_MEDIUM));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrGet_text(ITEM_BL_WATER_LOAD));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_BASELINE_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    //g_pLang->vEnd_section();
    }
  }
