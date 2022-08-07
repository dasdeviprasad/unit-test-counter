/********************************************************************
FreqRange.cpp

Copyright (C) 2012,2013,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CFreqRange class.

  This class displays a dialog box that allows the user to select
  the frequency range.  It is only used if the program is built for
  thej 60/200 CPM hardware.


HISTORY:
03-OCT-12  RET  New.
15-JAN-13  RET
             Add help button.
               Add event handler: OnBnClickedFreqRangeHelp()
17-MAR-13  RET
              Add cancel button.
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedFreqRangeHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "FreqRange.h"
#include ".\freqrange.h"


// CFreqRange dialog

IMPLEMENT_DYNAMIC(CFreqRange, CDialog)

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the currently active document.

********************************************************************/
CFreqRange::CFreqRange(CWnd* pParent /*=NULL*/)
	: CDialog(CFreqRange::IDD, pParent)
{
}

CFreqRange::~CFreqRange()
{
}

void CFreqRange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFreqRange, CDialog)
  ON_BN_CLICKED(IDC_RADIO_15_CPM, OnBnClickedRadio15Cpm)
  ON_BN_CLICKED(IDC_RADIO_60_CPM, OnBnClickedRadio60Cpm)
  ON_BN_CLICKED(IDC_RADIO_200_CPM, OnBnClickedRadio200Cpm)
  ON_BN_CLICKED(IDC_FREQ_RANGE_HELP, OnBnClickedFreqRangeHelp)
END_MESSAGE_MAP()


// CFreqRange message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Select the radio button based on the frequency range from the config file.
********************************************************************/
BOOL CFreqRange::OnInitDialog()
  {
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  vSet_text();
  if(m_iFreqRange == FREQ_LIMIT_200_CPM)
  	((CButton *)GetDlgItem(IDC_RADIO_200_CPM))->SetCheck(BST_CHECKED);
  else if(m_iFreqRange == FREQ_LIMIT_60_CPM)
  	((CButton *)GetDlgItem(IDC_RADIO_60_CPM))->SetCheck(BST_CHECKED);
  else // if(m_iFreqRange == FREQ_LIMIT_15_CPM)
  	((CButton *)GetDlgItem(IDC_RADIO_15_CPM))->SetCheck(BST_CHECKED);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnBnClickedRadio15Cpm

  Message handler for the "15 CPM" radio button.
    Update the frequency range variable.
********************************************************************/
void CFreqRange::OnBnClickedRadio15Cpm()
  {
  m_iFreqRange = FREQ_LIMIT_15_CPM;
  }

/********************************************************************
OnBnClickedRadio60Cpm

  Message handler for the "60 CPM" radio button.
    Update the frequency range variable.
********************************************************************/
void CFreqRange::OnBnClickedRadio60Cpm()
  {
  m_iFreqRange = FREQ_LIMIT_60_CPM;
  }

/********************************************************************
OnBnClickedRadio200Cpm

  Message handler for the "200 CPM" radio button.
    Update the frequency range variable.
********************************************************************/
void CFreqRange::OnBnClickedRadio200Cpm()
  {
  m_iFreqRange = FREQ_LIMIT_200_CPM;
  }

/////////////////////////////////////////////////////////////////////
//// Support Methods

/********************************************************************
vSet_data - Set the frequency range selection.  Used by the parent.

  inputs: Frequency range from the eFrequencyRange in EGGSAS.h.

  return: None.
********************************************************************/
void CFreqRange::vSet_data(int iFreqRange)
  {
  m_iFreqRange = iFreqRange;
  }

/********************************************************************
iGet_freq_range - Get the frequency range selection.  Used by the parent.

  inputs: None.

  return: Selected frequency range.  Values are from the eFrequencyRange in EGGSAS.h.
********************************************************************/
int CFreqRange::iGet_freq_range()
  {
  return(m_iFreqRange);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CFreqRange::vSet_text(void)
  {
  CString cstrTemp;

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_FREQUENCYRANGE);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_FR_TITLE));

    GetDlgItem(IDC_STATIC_SEL_FREQ_RANGE)->SetWindowText(g_pLang->cstrGet_text(ITEM_FR_SELECT));
    // Radio buttons
    cstrTemp.Format("15 %s", g_pLang->cstrLoad_string(ITEM_G_CPM1));
    GetDlgItem(IDC_RADIO_15_CPM)->SetWindowText(cstrTemp);

    cstrTemp.Format("60 %s", g_pLang->cstrLoad_string(ITEM_G_CPM1));
    GetDlgItem(IDC_RADIO_60_CPM)->SetWindowText(cstrTemp);

    cstrTemp.Format("200 %s", g_pLang->cstrLoad_string(ITEM_G_CPM1));
    GetDlgItem(IDC_RADIO_200_CPM)->SetWindowText(cstrTemp);
    
    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    }
  }

/********************************************************************
OnBnClickedFreqRangeHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CFreqRange::OnBnClickedFreqRangeHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_FREQ_RANGE_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_FREQ_RANGE_HELP);
  }
