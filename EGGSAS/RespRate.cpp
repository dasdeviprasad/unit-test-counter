/********************************************************************
RespRate.cpp

Copyright (C) 2003,2004,2012, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CRespRate class.

  This class provides the user a means of entering the respiration rate
  during a study.


HISTORY:
11-DEC-03  RET  New.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "RespRate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRespRate dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Y Pixel where top of dialog box should go.
          Period that respiration rate gets entered for.
          Parent window.  Must not be NULL.
=********************************************************************/
CRespRate::CRespRate(int iWinTop, unsigned short uPeriod, CWnd* pParent)
	: CDialog(CRespRate::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CRespRate)
	m_uRespRate = 0;
	m_cstrPeriod = _T("");
	//}}AFX_DATA_INIT

  m_pcwndParent = pParent;
  m_iWinTop = iWinTop;
  m_uPeriod = uPeriod;
  }

void CRespRate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRespRate)
	DDX_Text(pDX, IDC_RESP_RATE, m_uRespRate);
	DDV_MinMaxUInt(pDX, m_uRespRate, 0, 100);
	DDX_Text(pDX, IDC_PERIOD, m_cstrPeriod);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRespRate, CDialog)
	//{{AFX_MSG_MAP(CRespRate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRespRate message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Move the dialog box so that its at the specified top Y coordinate
      and its centered withc repsect to the parent window.
********************************************************************/
BOOL CRespRate::OnInitDialog() 
  {

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();
    // Move the dialog box to the bottom of the parent window
    // and center it under the parent window.
  RECT rParentWin, rWin;
  int iWidth, iHeight, iParentWidth;

  GetWindowRect(&rWin); // This dialog box's window dimensions.
  iWidth = rWin.right - rWin.left;
  iHeight = rWin.bottom - rWin.top;
  m_pcwndParent->GetWindowRect(&rParentWin);
  iParentWidth = rParentWin.right - rParentWin.left;

  rWin.left = rParentWin.left + ((iParentWidth - iWidth) / 2);
  rWin.right = rWin.left + iWidth;
  rWin.top = m_iWinTop; //rParentWin.bottom;
  rWin.bottom = rWin.top + iHeight;
  MoveWindow(&rWin, TRUE);
	
  vDisplay_period();
  UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnOK

  Message handler for the OK button.
    Get the values from the dialog box fields.
********************************************************************/
void CRespRate::OnOK() 
  {

  UpdateData(TRUE);
	CDialog::OnOK();
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
uGet_resp_rate - Get the respiration rate the user entered for the period.

  inputs: None.

  return: Respiration rate.
********************************************************************/
short unsigned CRespRate::uGet_resp_rate()
  {
  return((short unsigned)m_uRespRate);
  }

/********************************************************************
vDisplay_period - Display the period the user wants the respiration rate
                  for.

  inputs: None.

  return: None.
********************************************************************/
void CRespRate::vDisplay_period()
  {

  switch(m_uPeriod)
    {
    case PERIOD_10:
      //m_cstrPeriod.LoadString(IDS_FIRST_10_MIN_PD);
      m_cstrPeriod = g_pLang->cstrLoad_string(ITEM_G_FIRST_10_MIN_PD);
      break;
    case PERIOD_20:
      //m_cstrPeriod.LoadString(IDS_SECOND_10_MIN_PD);
      m_cstrPeriod = g_pLang->cstrLoad_string(ITEM_G_SECOND_10_MIN_PD);
     break;
    case PERIOD_30:
      //m_cstrPeriod.LoadString(IDS_THIRD_10_MIN_PD);
      m_cstrPeriod = g_pLang->cstrLoad_string(ITEM_G_THIRD_10_MIN_PD);
      break;
    case PERIOD_40:
      //m_cstrPeriod.LoadString(IDS_FOURTH_10_MIN_PD);
      m_cstrPeriod = g_pLang->cstrLoad_string(ITEM_G_FOURTH_10_MIN_PD);
      break;
    }
  }

/********************************************************************
vSet_period - Tell this dialog box what period to get the respiration rate
              for.  Display the period.

  inputs: Period.

  return: None.
********************************************************************/
void CRespRate::vSet_period(unsigned short uPeriod)
  {

  m_uPeriod = uPeriod;
  vDisplay_period();
  UpdateData(FALSE);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CRespRate::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_RESPIRATION);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_R_RESPRATE));

    GetDlgItem(IDC_STATIC_INSTRUCTIONS)->SetWindowText(g_pLang->cstrGet_text(ITEM_R_INSTRUCTIONS));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    }
  }
