/********************************************************************
SelStudyTypeFreq.cpp

Copyright (C) 2013 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CSelStudyTypeFreq class.

  This class provides for selecting the study type and frequency range for recording
  a study.

HISTORY:
13-MAR-13  RET  New.
17-MAR-13  RET
              Add cancel button.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "SelStudyTypeFreq.h"
#include ".\selstudytypefreq.h"


// CSelStudyTypeFreq dialog

IMPLEMENT_DYNAMIC(CSelStudyTypeFreq, CDialog)

/********************************************************************
Constructor

  Initialize class data.

********************************************************************/
CSelStudyTypeFreq::CSelStudyTypeFreq(CWnd* pParent /*=NULL*/)
	: CDialog(CSelStudyTypeFreq::IDD, pParent)
  , m_chkStandard15(FALSE)
  , m_chkResearch15(FALSE)
  , m_chkResearch60(FALSE)
  , m_chkResearch200(FALSE)
  {
}

CSelStudyTypeFreq::~CSelStudyTypeFreq()
{
}

void CSelStudyTypeFreq::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Check(pDX, IDC_CHK_STANDARD_15, m_chkStandard15);
DDX_Check(pDX, IDC_CHK_RESEARCH_15, m_chkResearch15);
DDX_Check(pDX, IDC_CHK_RESEARCH_60, m_chkResearch60);
DDX_Check(pDX, IDC_CHK_RESEARCH_200, m_chkResearch200);
}


BEGIN_MESSAGE_MAP(CSelStudyTypeFreq, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDC_CHK_STANDARD_15, OnBnClickedChkStandard15)
  ON_BN_CLICKED(IDC_CHK_RESEARCH_15, OnBnClickedChkResearch15)
  ON_BN_CLICKED(IDC_CHK_RESEARCH_60, OnBnClickedChkResearch60)
  ON_BN_CLICKED(IDC_CHK_RESEARCH_200, OnBnClickedChkResearch200)
END_MESSAGE_MAP()

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Set the text for displaying..
********************************************************************/
BOOL CSelStudyTypeFreq::OnInitDialog()
  {
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  vSet_text();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

// CSelStudyTypeFreq message handlers

/********************************************************************
OnBnClickedOk

  Message handler for the OK button.

  Save the check state of the checkboxes.
********************************************************************/
void CSelStudyTypeFreq::OnBnClickedOk()
  {

  UpdateData(TRUE);

  if(m_chkStandard15 == FALSE && m_chkResearch15 == FALSE
    && m_chkResearch60 == FALSE && m_chkResearch200 == FALSE)
    {
    MessageBox(g_pLang->cstrLoad_string(ITEM_G_SEL_STUDY_TYPE_FREQ_RANGE),
      g_pLang->cstrLoad_string(ITEM_G_WARNING), MB_OK);
    }
  else
    OnOK();
  }

/********************************************************************
OnBnClickedChkStandard15 - Handler for the "0 - 15 cpm Standard Study" checkbox.

   Disable the other checkboxes.
********************************************************************/
void CSelStudyTypeFreq::OnBnClickedChkStandard15()
  {

  UpdateData(TRUE);

  if(m_chkStandard15 == TRUE)
    {
    m_chkResearch15 = FALSE;
    m_chkResearch60 = FALSE;
    m_chkResearch200 = FALSE;
    UpdateData(FALSE);
    }
  }

/********************************************************************
OnBnClickedChkResearch15 - Handler for the "0 - 15 cpm Research Study" checkbox.

   Disable the other checkboxes.
********************************************************************/
void CSelStudyTypeFreq::OnBnClickedChkResearch15()
  {

  UpdateData(TRUE);

  if(m_chkResearch15 == TRUE)
    {
    m_chkStandard15 = FALSE;
    m_chkResearch60 = FALSE;
    m_chkResearch200 = FALSE;
    UpdateData(FALSE);
    }
  }

/********************************************************************
OnBnClickedChkResearch60 - Handler for the "0 - 60 cpm Research Study" checkbox.

   Disable the other checkboxes.
********************************************************************/
void CSelStudyTypeFreq::OnBnClickedChkResearch60()
  {

  UpdateData(TRUE);

  if(m_chkResearch60 == TRUE)
    {
    m_chkStandard15 = FALSE;
    m_chkResearch15 = FALSE;
    m_chkResearch200 = FALSE;
    UpdateData(FALSE);
    }
  }

/********************************************************************
OnBnClickedChkResearch200 - Handler for the "0 - 200 cpm Research Study" checkbox.

   Disable the other checkboxes.
********************************************************************/
void CSelStudyTypeFreq::OnBnClickedChkResearch200()
  {

  UpdateData(TRUE);

  if(m_chkResearch200 == TRUE)
    {
    m_chkStandard15 = FALSE;
    m_chkResearch15 = FALSE;
    m_chkResearch60 = FALSE;
    UpdateData(FALSE);
    }
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
ptGet_study_type - Get the study type.

  inputs: None.

  return: Study type from ePgmType enumerated list in EGGSAS.h.
********************************************************************/
ePgmType CSelStudyTypeFreq::ptGet_study_type()
  {
  ePgmType ptRet;

  if(m_chkStandard15 == TRUE)
    ptRet = PT_STANDARD;
  else
    ptRet = PT_RESEARCH;
  return(ptRet);
  }

/********************************************************************
iGet_freq_range - Get the frequency range selection.  Used by the parent.

  inputs: None.

  return: Selected frequency range.
********************************************************************/
unsigned short CSelStudyTypeFreq::uGet_freq_range()
  {
  unsigned short uFreqRange;

  if(m_chkStandard15 == TRUE || m_chkResearch15 == TRUE)
    uFreqRange = 15;
  else if(m_chkResearch60 == TRUE)
    uFreqRange = 60;
  else
    uFreqRange = 200;
  return(uFreqRange);
  }

/********************************************************************
uGet_freq_range_id - Get the frequency range ID selection.  Used by the parent.

  inputs: None.

  return: Selected frequency range ID.  Values are from the eFrequencyRange in EGGSAS.h.
********************************************************************/
unsigned short CSelStudyTypeFreq::uGet_freq_range_id()
  {
  unsigned short uFreqRange;

  if(m_chkStandard15 == TRUE || m_chkResearch15 == TRUE)
    uFreqRange = FREQ_LIMIT_15_CPM;
  else if(m_chkResearch60 == TRUE)
    uFreqRange = FREQ_LIMIT_60_CPM;
  else
    uFreqRange = FREQ_LIMIT_200_CPM;
  return(uFreqRange);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CSelStudyTypeFreq::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_STUDY_TYPE_FREQ_RANGE);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_STFR_TITLE));

    GetDlgItem(IDC_TITLE)->SetWindowText(g_pLang->cstrGet_text(ITEM_STFR_LABEL));
    GetDlgItem(IDC_CHK_STANDARD_15)->SetWindowText(g_pLang->cstrGet_text(ITEM_STFR_STD_0_15_CHKBOX));
    GetDlgItem(IDC_CHK_RESEARCH_15)->SetWindowText(g_pLang->cstrGet_text(ITEM_STFR_RSCH_0_15_CHKBOX));
    GetDlgItem(IDC_CHK_RESEARCH_60)->SetWindowText(g_pLang->cstrGet_text(ITEM_STFR_RSCH_0_60_CHKBOX));
    GetDlgItem(IDC_CHK_RESEARCH_200)->SetWindowText(g_pLang->cstrGet_text(ITEM_STFR_RSCH_0_200_CHKBOX));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    }
  }


