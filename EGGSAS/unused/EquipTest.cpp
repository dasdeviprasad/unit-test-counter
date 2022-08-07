/********************************************************************
EquipTest.cpp

Copyright (C) 2003,2004, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CEquipTest class.

  This class performs the initial testing to make sure the connections
  are OK before the study begins.

HISTORY:
03-OCT-03  RET  New.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "EquipTest.h"
#include "util.h"
  // For graphing
#include "Pegrpapi.h"
#include "Pemessag.h"
#include ".\equiptest.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CEGGSASApp theApp;

static CEquipTest *s_pDlgBox;

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
    }

    // Set up the strip chart for recording the signal.
  vSetup_graph();

    // Get the window of signal values within which we must stay for
    // a stable signal, both EGG and respiration signals.
  m_fStableEGGWindow = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
                                INI_ITEM_STABLEWINDOW_EGG, 1000);
  m_fStableRespWindow = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
                                INI_ITEM_STABLEWINDOW_RESP, 500);
    // Get the length of time the signal has to be stable before the equipment test is over.
  m_uStableTime = (unsigned)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER, INI_ITEM_STABLETIME, 30);

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
  short int iRet;

  if(m_pDoc->m_pDriver == NULL)
    { // We don't have the driver initialized yet, so initialize it.
    m_pDoc->m_pDriver = new CDriver();
    if(m_pDoc->m_pDriver->iInit() != SUCCESS)
      {
      delete m_pDoc->m_pDriver;
      m_pDoc->m_pDriver = NULL;
      }
    }
  if(m_pDoc->m_pDriver != NULL)
    { // Check the battery.
    s_pDlgBox = this;
    m_iNumDots = 0;
    m_cstrStatus = "Checking battery voltage";
    UpdateData(FALSE);
    if((iRet = m_pDoc->m_pDriver->iCheck_battery_impedance(CHK_BATTERY, &vShowStatus)) != SUCCESS)
      { // Display an appropriate error message.
      if(iRet == SUCCESS_LOW_BATTERY)
        {
        cstrTemp.LoadString(IDS_CONTINUE);
        cstrMsg.Format("%s  %s", m_pDoc->m_pDriver->m_cstrErr, cstrTemp);
        cstrErr.LoadString(IDS_WARNING);
        if(MessageBox(cstrMsg, cstrErr, MB_YESNO) == IDYES)
          iRet = SUCCESS;
        else
          iRet = FAIL;
        }
      else
        {
        cstrErr.LoadString(IDS_ERROR1);
        MessageBox(m_pDoc->m_pDriver->m_cstrErr, cstrErr, MB_OK);
        iRet = FAIL;
        }
      }
    if(iRet == SUCCESS)
      { // Check the impedance.
      m_cstrStatus = "Checking electrode connections";
      UpdateData(FALSE);
      if((iRet = m_pDoc->m_pDriver->iCheck_battery_impedance(CHK_ALL_IMPEDANCE, &vShowStatus))
      != SUCCESS)
        {
        cstrErr.LoadString(IDS_ERROR1);
        MessageBox(m_pDoc->m_pDriver->m_cstrErr, cstrErr, MB_OK);
        iRet = FAIL;
        }
      }
    if(iRet == SUCCESS)
      { // Disable the start button.
      GetDlgItem(IDC_START_TEST)->EnableWindow(FALSE);
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
        // Start the timer to generate an interrupt every 250 milliseconds.
      m_uTimer = SetTimer(1, 250, NULL);
//      ((CEGGSASApp *)AfxGetApp())->vLog("**** Start equipment test");
      }
    }
  m_cstrStatus.Empty();
  UpdateData(FALSE);
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

  cstrMsg.LoadString(IDS_END_STUDY_ARE_YOU_SURE);
  cstrCaption.LoadString(IDS_WARNING);
  if(MessageBox(cstrMsg, cstrCaption, MB_YESNO) == IDYES)
    {
    if(m_pDoc->m_pDriver != NULL)
      m_pDoc->m_pDriver->vStop(); // Stop the driver.
    CDialog::OnCancel();
    }
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
    ++m_uTotalTime;
      // Get the new values.
    iPtsToGraph = m_pDoc->m_pDriver->iGet_data_all(fEGGData, fRespData);
    if(iPtsToGraph > 0)
      { // Append new values and examine the signal
      m_uTotalPts += (unsigned)iPtsToGraph;
      fXInc = 1.0F / (float)(DATA_POINTS_PER_MIN_1); //(float)X_MAX_BL_SCALE;
      for(iCnt = 0; iCnt < iPtsToGraph; ++iCnt, m_fXPos += fXInc) //0.25F)
        {
        fEGG = fEGGData[iCnt];
        fResp = fRespData[iCnt];
        fData[0] = fEGG;
        fData[1] = fResp;
        PEvset(m_hPEData, PEP_faAPPENDYDATA, fData, 1); // New Y value.

//        ((CEGGSASApp *)AfxGetApp())->vLog("EGG equip Test (%d): %.4f    Respiration: %.4f",
//                                          m_uTotalTime, fEGG, fResp);

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
//      if(m_fXPos == X_MAX_SCALE)
//        PEnset(m_hPEData, PEP_nMANUALSCALECONTROLX, PEMSC_NONE);
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
        cstrTitle.LoadString(IDS_WARNING);
        cstrMsg.LoadString(IDS_SIGNAL_NOT_STABLE);
        if(MessageBox(cstrMsg, cstrTitle, MB_OKCANCEL) == IDOK)
          { // Continuing in manual mode.
          m_pDoc->m_pdPatient.bManualMode = TRUE;
          GetDlgItem(IDOK)->EnableWindow(TRUE);
          }
        }
/***************** Used for scrolling the strip chart and not used.
      if(m_uTotalPts > (unsigned)((float)MAX_ET_TIME * DATA_POINTS_PER_SECOND))
        { // Scroll the X-axis left by the number of points just received.
        fScroll = (float)iPtsToGraph * (5.0F / ((float)MAX_ET_TIME * DATA_POINTS_PER_SECOND));
        m_dMinXScale += fScroll; //((float)iPtsToGraph * 0.25F);
        PEvset(m_hPEData, PEP_fMANUALMINX, &m_dMinXScale, 1);
        m_dMaxXScale += fScroll; //((float)iPtsToGraph * 0.25F);
        PEvset(m_hPEData, PEP_fMANUALMAXX, &m_dMaxXScale, 1);
        }
*****************/
        // Update image and force paint
      PEreinitialize(m_hPEData);
      PEresetimage(m_hPEData, 0, 0);
      ::InvalidateRect(m_hPEData, NULL, FALSE);
      }
    else if(iPtsToGraph < 0)
      { // Error reading the device.  Stop the test.
      CString cstrMsg, cstrErr;

      KillTimer(m_uTimer);
      m_uTimer = 0;
        // Make sure the red light on, the OK button is disabled.
      GetDlgItem(IDC_RESP_GREEN)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_RESP_RED)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_ELECTRODE_RED)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_ELECTRODE_GREEN)->ShowWindow(SW_HIDE);
      GetDlgItem(IDOK)->EnableWindow(FALSE);

      cstrMsg.Format(IDS_ERR_EGG_READ, m_pDoc->m_pDriver->m_cstrErr);
      cstrErr.LoadString(IDS_ERROR1);
      MessageBox(cstrMsg, cstrErr, MB_OK);
      }
    }
	
	CDialog::OnTimer(nIDEvent);
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
vShowStatus - Callback function for the driver while it is reading the
              battery voltage and impedance.  It takes a while for the
              driver to read this data, so the driver keeps calling
              this function which displays a status followed by a display
              of increasing number of dots to show user system is still
              alive.

  inputs: None.
  
  return: None.
********************************************************************/
void CEquipTest::vShowStatus(void)
  {
  int iIndex;

  if(s_pDlgBox->m_iNumDots == 10)
    {
    if((iIndex = s_pDlgBox->m_cstrStatus.Find('.')) >= 0)
      s_pDlgBox->m_cstrStatus.Delete(iIndex, s_pDlgBox->m_cstrStatus.GetLength() - iIndex);
    }
  else
    {
    s_pDlgBox->m_cstrStatus += ".";
    ++s_pDlgBox->m_iNumDots;
    }

  s_pDlgBox->UpdateData(FALSE);
  s_pDlgBox->Invalidate();
  }

