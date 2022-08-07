/********************************************************************
WaterLoadInfo.cpp

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the WaterLoad information dialog box.

  Operation:
    - Used only for a waterload study.
    - Display the total minutes in the study.
    - The user must enter the baseline and post-waterload minutes in multiples
      of 10 minutes.
    - If the baseline minutes and the post-waterload minutes don't add up to
      the total minuteS, an error message is displayed.

HISTORY:
28-JUN-10  RET  New.
********************************************************************/

#include "stdafx.h"
#include "OrphanFileRecovery.h"
#include "English.h"
#include "WaterLoadInfo.h"

// CWaterLoadInfo dialog

IMPLEMENT_DYNAMIC(CWaterLoadInfo, CDialog)

/********************************************************************
Constructor

********************************************************************/
CWaterLoadInfo::CWaterLoadInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CWaterLoadInfo::IDD, pParent)
  , m_uBaselineMin(0)
  , m_uPostWaterMin(0)
  , m_uTotalMinutes(0)
  {
}

/********************************************************************
Destructor

********************************************************************/
CWaterLoadInfo::~CWaterLoadInfo()
{
}

void CWaterLoadInfo::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_EDIT_BASELINE_MIN, m_uBaselineMin);
DDV_MinMaxUInt(pDX, m_uBaselineMin, 10, 20);
DDX_Text(pDX, IDC_EDIT_POSTWATER_MIN, m_uPostWaterMin);
DDX_Text(pDX, IDC_EDIT_TOTAL_MIN, m_uTotalMinutes);
}


BEGIN_MESSAGE_MAP(CWaterLoadInfo, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CWaterLoadInfo message handlers
/********************************************************************
OnInitDialog

  Initialize class data.
********************************************************************/
BOOL CWaterLoadInfo::OnInitDialog()
  {
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  m_uTotalMinutes = m_uTotalMin;

  UpdateData(FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnBnClickedOk - Handler for the "OK" button.

- Make sure the baseline and post-waterload study minutes add up to the
  total minutes.
- If the minutes don't add up, display an error message and don't exit
  the dialog box.
- If the minutes add up, exit the dialog box.
********************************************************************/
void CWaterLoadInfo::OnBnClickedOk()
  {

  UpdateData(TRUE);
  // Make sure the baseline and post-waterload study minutes add up to the
  // total minutes.
  if(m_uTotalMinutes != (m_uBaselineMin + m_uPostWaterMin))
    MessageBox(IDS_MINUTES_DONT_ADDUP, IDS_ERROR1);
  else
    OnOK();
  }

/********************************************************************
OnBnClickedCancel - Handler for the "Cancel" button.

********************************************************************/
void CWaterLoadInfo::OnBnClickedCancel()
  {
  // TODO: Add your control notification handler code here
  OnCancel();
  }

