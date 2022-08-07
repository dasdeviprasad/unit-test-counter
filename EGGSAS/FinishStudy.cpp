/********************************************************************
FinishStudy.cpp

Copyright (C) 2007,2012, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CFinishStudy class.

  This class performs the makes sure the user wants to finish the study.

HISTORY:
26-MAY-07  RET  New.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "FinishStudy.h"
#include ".\finishstudy.h"


// CFinishStudy dialog

IMPLEMENT_DYNAMIC(CFinishStudy, CDialog)
CFinishStudy::CFinishStudy(CWnd* pParent /*=NULL*/)
	: CDialog(CFinishStudy::IDD, pParent)
  , m_bFinishSts(FALSE)
  {
}

CFinishStudy::~CFinishStudy()
{
}

void CFinishStudy::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Check(pDX, IDC_FINISH_STUDY, m_bFinishSts);
}


BEGIN_MESSAGE_MAP(CFinishStudy, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CFinishStudy message handlers

BOOL CFinishStudy::OnInitDialog()
  {
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  vSet_text();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnOK

  Message handler for the OK button.
    Save the current checkbox state.
********************************************************************/
void CFinishStudy::OnBnClickedOk()
  {

    // Save the current checkbox state.
  UpdateData(TRUE);

  OnOK();
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
bGet_finish_sts - Get the finish status (i.e. if user really wants to
                  finish the study).

  inputs: None.

  return: TRUE - User wants to finish the study.
          FALSE - User want to continue with the study.
********************************************************************/
BOOL CFinishStudy::bGet_finish_sts()
  {
  return(m_bFinishSts);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CFinishStudy::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_FINISH_STUDY);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_FS_FINISH_STUDY));

    GetDlgItem(IDC_FINISH_STUDY)->SetWindowText(g_pLang->cstrGet_text(ITEM_FS_FINISH_STUDY));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    }
  }
