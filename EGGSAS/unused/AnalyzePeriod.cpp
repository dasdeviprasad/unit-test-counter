/********************************************************************
AnalyzePeriod.cpp

Copyright (C) 2003, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CAnalyzePeriod class.

  This class determines the good minutes in each period and displays
  them to the user for acceptance/modification.


HISTORY:
03-OCT-03  RET  New.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "analyze.h"
#include "TenMinView.h"
#include "AnalyzePeriod.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnalyzePeriod dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Indicates the period to analyze.
          Pointer to the Document using this class.
          Pointer to parent window (default is NULL).
********************************************************************/
CAnalyzePeriod::CAnalyzePeriod(unsigned uPeriod, CEGGSASDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CAnalyzePeriod::IDD, pParent)
  {

	//{{AFX_DATA_INIT(CAnalyzePeriod)
	m_iAccept = -1;
	m_cstrAnalyzeTitle = _T("");
	m_cstrEndMinute = _T("");
	m_cstrStartMinute = _T("");
	m_cstrGoodMinutes = _T("");
	//}}AFX_DATA_INIT

  m_uPeriod = uPeriod;
  m_pDoc = pDoc;
  m_iAccept = 0; // Default to accepting the good minutes.
  }


void CAnalyzePeriod::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnalyzePeriod)
	DDX_Control(pDX, IDC_ARTIFACT_LIST, m_cArtifact);
	DDX_Radio(pDX, IDC_ACCEPT, m_iAccept);
	DDX_Text(pDX, IDC_ANALYZE_TITLE, m_cstrAnalyzeTitle);
	DDX_Text(pDX, IDC_END_MIN, m_cstrEndMinute);
	DDV_MaxChars(pDX, m_cstrEndMinute, 4);
	DDX_Text(pDX, IDC_START_MIN, m_cstrStartMinute);
	DDV_MaxChars(pDX, m_cstrStartMinute, 4);
	DDX_Text(pDX, IDC_GOOD_MIN, m_cstrGoodMinutes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnalyzePeriod, CDialog)
	//{{AFX_MSG_MAP(CAnalyzePeriod)
	ON_BN_CLICKED(IDC_ACCEPT, OnAccept)
	ON_BN_CLICKED(IDC_REJECT, OnReject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalyzePeriod message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Move the dialog box to the right of the graphs so the graphs are 
      fully visible.
    Set the title line
    Find the artifact free minutes.
    Display the artifact minutes in the listbox.
    Display the longest artifact free minutes.

  If in manual mode, don't search for artifact and don't display the
    artifact minutes and artifact free minutes. Display the fields
    allowing for entry of good minutes.
********************************************************************/
BOOL CAnalyzePeriod::OnInitDialog() 
  {
  CString cstrTemp;
  short int iPos;
  float fStart, fEnd;
  BOOL bRet;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    // Set the secondary title for this dialog box to indicate which period
    // is being analyzed.
  switch(m_uPeriod)
    {
    case PERIOD_BL:
      m_cstrAnalyzeTitle.LoadString(IDS_TITLE_BASELINE);
      break;
    case PERIOD_10:
      m_cstrAnalyzeTitle.LoadString(IDS_TITLE_10_MIN_PERIOD);
      break;
    case PERIOD_20:
      m_cstrAnalyzeTitle.LoadString(IDS_TITLE_20_MIN_PERIOD);
      break;
    case PERIOD_30:
      m_cstrAnalyzeTitle.LoadString(IDS_TITLE_30_MIN_PERIOD);
      break;
    default:
      m_cstrAnalyzeTitle.Empty(); // Default title, baseline period
      break;
    }

  if(m_pDoc->m_pdPatient.bManualMode == FALSE)
    {
    bRet = TRUE;
    if(m_pDoc->m_pArtifact != NULL)
      { // Fill in the artifact list box.
      iPos = FIRST_ART;
      while(m_pDoc->m_pArtifact->bGet_next_artifact(m_uPeriod, iPos, &fStart, &fEnd) == true)
        {
        cstrTemp.Format("%.1f - %.1f", fStart + (float)(m_uPeriod * 10),
                        fEnd + (float)(m_uPeriod * 10));
        m_cArtifact.AddString(cstrTemp);
        iPos = NEXT_ART;
        }
        // Get the good minutes for this period.
      if(m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod] == INV_MINUTE)
        {
        if(m_pDoc->m_pArtifact->bFind_art_free_min(m_uPeriod, &m_fStart, &m_fEnd) == false)
          {
          m_fStart = INV_MINUTE;
          m_fEnd = INV_MINUTE;
          }
        }
      else
        {
        m_fStart = m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod];
        m_fEnd = m_pDoc->m_pdPatient.fGoodMinEnd[m_uPeriod];
        }
      if(m_fStart != INV_MINUTE)
        { // Found artifact free minutes in half minute increments.
          // The minutes are 0 to 10.  Convert them to minutes within the period.
        m_cstrGoodMinutes.Format("%.1f - %.1f", m_fStart + (float)(m_uPeriod * 10), 
                                 m_fEnd + (float)(m_uPeriod * 10));
        }
      }
    }
  else
    { // In manual mode, hide accept/reject buttons, default to full period
      // for good minutes.
	  GetDlgItem(IDC_ACCEPT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_REJECT)->ShowWindow(SW_HIDE);
      // Show the additional dialog box fields.
    GetDlgItem(IDC_SELECT_NEW_MINUTES)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_START_MIN_TITLE)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_START_MIN)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_END_MIN_TITLE)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_END_MIN)->ShowWindow(SW_SHOW);
    m_fStart = 0.0F;
    m_fEnd = (float)MIN_PER_PERIOD;
    m_cstrGoodMinutes.Empty(); //.Format("%.1f - %.1f", m_fStart + (float)(m_uPeriod * 10), 
                               // m_fEnd + (float)(m_uPeriod * 10));
    GetDlgItem(IDC_START_MIN)->SetFocus();
    bRet = FALSE;
    m_iAccept = -1;
    }
  UpdateData(FALSE);

    // Move the dialog box to the right of the graphs.
  RECT rViewWin, rWin;
  int iWidth, iHeight;

  GetWindowRect(&rWin); // This dialog box's window dimensions.
  iWidth = rWin.right - rWin.left;
  iHeight = rWin.bottom - rWin.top;
  m_pDoc->m_p10MinView->GetWindowRect(&rViewWin);

  rWin.right = rViewWin.right;
  rWin.left = rWin.right - iWidth;
  rWin.top = rViewWin.top;
  rWin.bottom = rWin.top + iHeight;
  MoveWindow(&rWin, TRUE);

	return(bRet);  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/********************************************************************
OnAccept

  Message handler for the Accept button.
********************************************************************/
void CAnalyzePeriod::OnAccept() 
  {

  UpdateData(TRUE);
    // Hide the additional dialog box fields.
  GetDlgItem(IDC_SELECT_NEW_MINUTES)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_START_MIN_TITLE)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_START_MIN)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_END_MIN_TITLE)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_END_MIN)->ShowWindow(SW_HIDE);
  UpdateData(FALSE);
  }

/********************************************************************
OnReject

  Message handler for the Reject button.
********************************************************************/
void CAnalyzePeriod::OnReject() 
  {

  UpdateData(TRUE);
    // Show the additional dialog box fields.
  GetDlgItem(IDC_SELECT_NEW_MINUTES)->ShowWindow(SW_SHOW);
  GetDlgItem(IDC_START_MIN_TITLE)->ShowWindow(SW_SHOW);
  GetDlgItem(IDC_START_MIN)->ShowWindow(SW_SHOW);
  GetDlgItem(IDC_END_MIN_TITLE)->ShowWindow(SW_SHOW);
  GetDlgItem(IDC_END_MIN)->ShowWindow(SW_SHOW);
  UpdateData(FALSE);
  }

/********************************************************************
OnOK

  Message handler for the OK button.
********************************************************************/
void CAnalyzePeriod::OnOK() 
  {
  CString cstrMsg, cstrTitle;
  float fStartMinute, fEndMinute;
  bool bOKToClose = true;

  UpdateData(TRUE);
	
  if(m_iAccept == 0)
    { // User accept program defined good minutes
    m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod] = m_fStart;
    m_pDoc->m_pdPatient.fGoodMinEnd[m_uPeriod] = m_fEnd;
    }
  else
    { // User defined own good minutes.
      // Convert the minutes to floating point values.
    fStartMinute = (float)atof(m_cstrStartMinute);
    fEndMinute = (float)atof(m_cstrEndMinute);
      // Make sure that the minutes entered are valid.
      // Verify that minutes must start on the minute or half minute.
    if(bValid_minute_format(m_cstrStartMinute) == false)
      { // Start minute doesn't start on the minute or half minute.
      cstrMsg.LoadString(IDS_START_MINUTE_FORMAT_ERR);
      bOKToClose = false;
      }
    else if(bValid_minute_format(m_cstrEndMinute) == false)
      { // End minute doesn't start on the minute or half minute.
      cstrMsg.LoadString(IDS_END_MINUTE_FORMAT_ERR);
      bOKToClose = false;
      }
      // Check to make sure that the minutes actually fall within the period.
    else if(fStartMinute < (float)(m_uPeriod * 10) || fStartMinute > (float)((m_uPeriod + 1) * 10))
      { // Start minutes is out of range.
      cstrMsg.LoadString(IDS_START_MINUTE_OUT_OF_RANGE);
      bOKToClose = false;
      }
    else if(fEndMinute < (float)(m_uPeriod * 10) || fEndMinute > (float)((m_uPeriod + 1) * 10))
      { // Start minutes is out of range.
      cstrMsg.LoadString(IDS_END_MINUTE_OUT_OF_RANGE);
      bOKToClose = false;
      }
    else if((fEndMinute - fStartMinute) < 4)
      { // Need 4 minutes.
      cstrMsg.LoadString(IDS_NEED_4_MINUTES);
      bOKToClose = false;
      }

    if(bOKToClose == false)
      { // Error, display the error message.
      cstrTitle.LoadString(IDS_ERROR1);
      MessageBox(cstrMsg, cstrTitle);
      }
    else
      { // Check if user define minutes include an artifact and if so, display
        // a warning message.
      fStartMinute -= (float)(m_uPeriod * 10);
      fEndMinute -= (float)(m_uPeriod * 10);
      if(m_pDoc->m_pArtifact->bArtifact_in_minutes(m_uPeriod, fStartMinute, fEndMinute)
      == true)
        {
        cstrTitle.LoadString(IDS_WARNING);
        cstrMsg.LoadString(IDS_MIN_INCL_ART);
        if(MessageBox(cstrMsg, cstrTitle, MB_YESNO) == IDNO)
          bOKToClose = false;
        }

      m_pDoc->m_pdPatient.fGoodMinStart[m_uPeriod] = fStartMinute;
      m_pDoc->m_pdPatient.fGoodMinEnd[m_uPeriod] = fEndMinute;
      m_fStart = fStartMinute;
      m_fEnd = fEndMinute;
      }
    }

  if(bOKToClose == true)
    CDialog::OnOK();
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.
********************************************************************/
void CAnalyzePeriod::OnCancel() 
  {
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
bValid_minute_format - Make sure the minute format is correct.
                         - Minute must start on the minute or the half minute.
                         - Minutes have been limited to 4 characters.

  inputs: CString object containing a user entered minute.

  return: true if the minute meets the above criteria.
          false if the minute is not valid.
********************************************************************/
bool CAnalyzePeriod::bValid_minute_format(CString cstrMinute)
  {
  int iIndex;
  bool bRet = false;

  if((iIndex = cstrMinute.Find('.')) >= 0)
    { // Have a decimal point, only a 5 or 0 is allowed to the right.
    ++iIndex; // Index of character after the decimal point.
    if(cstrMinute.GetLength() > iIndex)
      { // Have characters after the decimal point. There can be 2 at most.
      if(cstrMinute.GetAt(iIndex) == '0' || cstrMinute.GetAt(iIndex) == '5')
        { // So far OK.  There may be one more character so we need to check it.
        ++iIndex; // Index of next character.
        if(cstrMinute.GetLength() > iIndex)
          { // Last character. It must be a 0.
          if(cstrMinute.GetAt(iIndex) == '0')
            bRet = true;
          }
        else
          bRet = true; // No more characters.
        }
      }
    else
      bRet = true; // No characters after decimal point, on the minute.
    }
  else
    bRet = true; // This is on the minute.
  return(bRet);
  }

