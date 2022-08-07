/********************************************************************
RespEdit.cpp

Copyright (C) 2003,2004,2008,2009,2012,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CRespEdit class.

  This class provides the user a means of changing/viewing the
  respiration rate recorded during a study.


HISTORY:
11-DEC-03  RET  New.
16-MAR-04  RET  Add help button and message handler.
08-FEB-08  RET  Version 2.03
                  Remove edit field and all processing for the 4th period.
14-JAN-09  RET  Version CDDRdr.2.08c
                  Display only if view mode.  Change Constructor.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedEditviewrespHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "RespEdit.h"
#include "util.h"
#include ".\respedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRespEdit dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the active document that started this dialog box.
          Where the diaglog box is being called from (edit, view, etc).
          Pointer to the window that started this dialog box.

********************************************************************/
CRespEdit::CRespEdit(CEGGSASDoc *pDoc, unsigned uFrom, CWnd* pParent /*=NULL*/)
	: CDialog(CRespEdit::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CRespEdit)
	m_uResp10 = 0;
	m_uResp20 = 0;
	m_uResp30 = 0;
	//}}AFX_DATA_INIT

  m_pDoc = pDoc;
#if EGGSAS_READER == READER_DEPOT
  m_uFrom = SM_VIEW;
#else
  m_uFrom = uFrom;
#endif
  }

void CRespEdit::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
//{{AFX_DATA_MAP(CRespEdit)
DDX_Text(pDX, IDC_RESP_RATE_10, m_uResp10);
DDX_Text(pDX, IDC_RESP_RATE_20, m_uResp20);
DDX_Text(pDX, IDC_RESP_RATE_30, m_uResp30);
//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRespEdit, CDialog)
	//{{AFX_MSG_MAP(CRespEdit)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_EDITVIEWRESP_HELP, OnBnClickedEditviewrespHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRespEdit message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Modify the fields for editing/viewing.
********************************************************************/
BOOL CRespEdit::OnInitDialog() 
  {

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();
//  if(m_pDoc->m_pdPatient.bExtra10Min == TRUE)
//    { // There is an additional ten minutes at end of study.
//    GetDlgItem(IDC_RESP_4_TITLE)->ShowWindow(SW_SHOW);
//    GetDlgItem(IDC_RESP_RATE_40)->ShowWindow(SW_SHOW);
//    }
    // Set up the dialog box fields for view/edit/new study
  if(m_uFrom == SM_VIEW)
    {
	  GetDlgItem(IDC_RESP_RATE_10)->EnableWindow(FALSE);
	  GetDlgItem(IDC_RESP_RATE_20)->EnableWindow(FALSE);
	  GetDlgItem(IDC_RESP_RATE_30)->EnableWindow(FALSE);
//    if(m_pDoc->m_pdPatient.bExtra10Min == TRUE)
//      GetDlgItem(IDC_RESP_RATE_40)->EnableWindow(FALSE);
    GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
      // Center the OK button.
    vCenter_button_horiz(this, GetDlgItem(IDOK));
    }
  UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.
********************************************************************/
void CRespEdit::OnCancel() 
  {
	
	CDialog::OnCancel();
  }

/********************************************************************
OnOK

  Message handler for the OK button.
    Get the values from the dialog box fields.
********************************************************************/
void CRespEdit::OnOK() 
  {

  UpdateData(TRUE);
	CDialog::OnOK();
  }

/********************************************************************
OnBnClickedEditviewrespHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CRespEdit::OnBnClickedEditviewrespHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_EDITVIEWRESP_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_EDITVIEWRESP_HELP);
  }

/////////////////////////////////////////////////////////////////////
//// Support Methods

/********************************************************************
vGet_respiration - Get the respiration rates the user entered.

  inputs: Pointer to an array of respiration rates.  The respiration rates
            from the dialog box fields are put into this array.
              First element: First 10 minute postwater period.
              Second element: Second 10 minute postwater period.
              Third element: Third 10 minute postwater period.

  return: None.
********************************************************************/
void CRespEdit::vGet_respiration(short unsigned *puRespRate)
  {

  *puRespRate = m_uResp10;
  *(puRespRate + 1) = m_uResp20;
  *(puRespRate + 2) = m_uResp30;
//  *(puRespRate + 3) = m_uResp40;
  }

/********************************************************************
vSet_respiration - Tell this dialog box what the respiration rates are.

  inputs: Pointer to an array of respiration rates.
            First element: First 10 minute postwater period.
            Second element: Second 10 minute postwater period.
            Third element: Third 10 minute postwater period.

  return: None.
********************************************************************/
void CRespEdit::vSet_respiration(short unsigned *puRespRate)
  {

  m_uResp10 = *puRespRate;
  m_uResp20 = *(puRespRate + 1);
  m_uResp30 = *(puRespRate + 2);
//  m_uResp40 = *(puRespRate + 3);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CRespEdit::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_RESPIRATION);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_R_RESPIRATION));

    GetDlgItem(IDC_STATIC_FIRST_10_MIN_PD)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_FIRST_10_MIN_PD));
    GetDlgItem(IDC_STATIC_SECOND_10_MIN_PD)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_SECOND_10_MIN_PD));
    GetDlgItem(IDC_STATIC_THIRD_10_MIN_PD)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_THIRD_10_MIN_PD));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_EDITVIEWRESP_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    //g_pLang->vEnd_section();
    }
  }
