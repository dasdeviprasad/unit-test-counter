/********************************************************************
FilterRSA.cpp

Copyright (C) 2012, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CFilterRSA class.

  This class allows the user to set RSA filtering so it is persistant.

HISTORY:
14-DEC-12  RET  New.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "FilterRSA.h"
#include ".\filterrsa.h"


// CFilterRSA dialog

IMPLEMENT_DYNAMIC(CFilterRSA, CDialog)

/********************************************************************
Constructor

  Initialize class data.

********************************************************************/
CFilterRSA::CFilterRSA(CWnd* pParent /*=NULL*/)
	: CDialog(CFilterRSA::IDD, pParent)
  , m_bFilter(FALSE)
  {
}

CFilterRSA::~CFilterRSA()
{
}

void CFilterRSA::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Check(pDX, IDC_CHK_FILTER, m_bFilter);
}


BEGIN_MESSAGE_MAP(CFilterRSA, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CFilterRSA message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Set the Filter RSA checkbox according to the global flag (not according
      to the value in the INI file).
********************************************************************/
BOOL CFilterRSA::OnInitDialog()
  {
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  if(g_pConfig->m_bFilterRSA == true)
    m_bFilter = TRUE;
  else
    m_bFilter = FALSE;

  vSet_text();

  UpdateData(FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnOK

  Message handler for the OK button.

  Write the filter flag to the INI file.
  Update the global flag.
********************************************************************/
void CFilterRSA::OnBnClickedOk()
  {
  UpdateData(TRUE);
  // Update the global flag for filtering RSA.
  if(m_bFilter == TRUE)
    g_bFilterRSA = true;
  else
    g_bFilterRSA = false;
  // Save the flag to the INI file.
  g_pConfig->vWrite_item(CFG_FILTER_RSA, g_bFilterRSA);
  OnOK();
  }

/********************************************************************
OnBnClickedCancel

  Message handler for the Cancel button.

  No change to filter flag.
********************************************************************/
void CFilterRSA::OnBnClickedCancel()
  {
  // TODO: Add your control notification handler code here
  OnCancel();
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CFilterRSA::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_FILTERRSA);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_FRSA_TITLE));

    GetDlgItem(IDC_CHK_FILTER)->SetWindowText(g_pLang->cstrGet_text(ITEM_FRSA_FILTER_RSA_CHKBOX));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    }
  }


