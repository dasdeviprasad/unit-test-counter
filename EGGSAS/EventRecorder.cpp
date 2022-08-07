/********************************************************************
EventRecorder.cpp

Copyright (C) 2007,2012, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CEventRecorder class.

  Display a dialog box allowing the user to specify an event.

HISTORY:
06-AUG-07  RET  New.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EventRecorder.h"
#include ".\eventrecorder.h"


// CEventRecorder dialog

IMPLEMENT_DYNAMIC(CEventRecorder, CDialog)

/********************************************************************
Constructor

  Initialize class data.
  Calculate minutes:seconds from the input and set the time field.

  inputs: Seconds from the start of either the baseline or the
            post-stimulation study.
********************************************************************/
CEventRecorder::CEventRecorder(int iSeconds, CWnd* pParent /*=NULL*/)
	: CDialog(CEventRecorder::IDD, pParent)
  , m_cstrDescription(_T(""))
  , m_bCough(FALSE)
  , m_bIngestion(FALSE)
  , m_bMovement(FALSE)
  , m_bNausea(FALSE)
  , m_bPain(FALSE)
  , m_bTalking(FALSE)
  , m_bVomiting(FALSE)
  , m_bOther(FALSE)
  , m_cstrTime(_T(""))
  {

  int iMinute = iSeconds / 60;
  m_cstrTime.Format("%d:%02d", iMinute, iSeconds - (iMinute * 60));
  }

/********************************************************************
Destructor

********************************************************************/
CEventRecorder::~CEventRecorder()
{
}

void CEventRecorder::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_cstrDescription);
DDX_Check(pDX, IDC_CHK_COUGH, m_bCough);
DDX_Check(pDX, IDC_CHK_INGESTION, m_bIngestion);
DDX_Check(pDX, IDC_CHK_MOVEMENT, m_bMovement);
DDX_Check(pDX, IDC_CHK_NAUSEA, m_bNausea);
DDX_Check(pDX, IDC_CHK_PAIN, m_bPain);
DDX_Check(pDX, IDC_CHK_TALKING, m_bTalking);
DDX_Check(pDX, IDC_CHK_VOMITING, m_bVomiting);
DDX_Check(pDX, IDC_CHK_OTHER, m_bOther);
DDX_Text(pDX, IDC_EDIT_TIME, m_cstrTime);
}


BEGIN_MESSAGE_MAP(CEventRecorder, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////
//// CEventRecorder message handlers

BOOL CEventRecorder::OnInitDialog()
  {
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  vSet_text();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnBnClickedOk

  Update the class variables associated with dialog box fields.
********************************************************************/
void CEventRecorder::OnBnClickedOk()
  {

  UpdateData(TRUE);

  OnOK();
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
cstrGet_description - Get the Event description.

inputs: None.

return: CString object containing the description.  May be empty.
********************************************************************/
CString CEventRecorder::cstrGet_description(void)
  {
  return (m_cstrDescription);
  }

/********************************************************************
cstrGet_indications - Get the Event indications.  These are based on the
                      check states of the checkboxes.
                      The return string consists of one letter per checkbox
                      that is checked.
                      If no checkboxes are checked, the default indication
                      is Other.

inputs: None.

return: CString object containing the indications.
********************************************************************/
CString CEventRecorder::cstrGet_indications(void)
  {
  CString cstrIndications;

  cstrIndications.Empty();
  if(m_bCough == TRUE)
    cstrIndications += g_pLang->cstrLoad_string(ITEM_G_INDICATION_COUGH); //"C";
  if(m_bIngestion == TRUE)
    cstrIndications += g_pLang->cstrLoad_string(ITEM_G_INDICATION_INGESTION); //"I";
  if(m_bMovement == TRUE)
    cstrIndications += g_pLang->cstrLoad_string(ITEM_G_INDICATION_MOVEMENT); //"M";
  if(m_bNausea == TRUE)
    cstrIndications += g_pLang->cstrLoad_string(ITEM_G_INDICATION_NAUSEA); //"N";
  if(m_bPain == TRUE)
    cstrIndications += g_pLang->cstrLoad_string(ITEM_G_INDICATION_PAIN); //"P";
  if(m_bTalking == TRUE)
    cstrIndications += g_pLang->cstrLoad_string(ITEM_G_INDICATION_TALKING); //"T";
  if(m_bVomiting == TRUE)
    cstrIndications += g_pLang->cstrLoad_string(ITEM_G_INDICATION_VOMITING); //"V";
  if(m_bOther == TRUE)
    cstrIndications += g_pLang->cstrLoad_string(ITEM_G_INDICATION_OTHER); //"O";
    // If nothing is checked, force Other.
  if(cstrIndications.IsEmpty() == TRUE)
    cstrIndications = g_pLang->cstrLoad_string(ITEM_G_INDICATION_OTHER); //"O";
  return (cstrIndications);
  }

/********************************************************************
vSet_data - Set the changeable data for the event.
            The changeable data is:
              - Description
              - Indications

inputs: CString object containing the indications.
        CString object containing the description.

return: None.
********************************************************************/
void CEventRecorder::vSet_data(CString cstrIndications, CString cstrDescription)
  {

  m_cstrDescription = cstrDescription;
  if(cstrIndications.Find('C') >= 0)
    m_bCough = TRUE;
  if(cstrIndications.Find('I') >= 0)
    m_bIngestion = TRUE;
  if(cstrIndications.Find('M') >= 0)
    m_bMovement = TRUE;
  if(cstrIndications.Find('N') >= 0)
    m_bNausea = TRUE;
  if(cstrIndications.Find('P') >= 0)
    m_bPain = TRUE;
  if(cstrIndications.Find('T') >= 0)
    m_bTalking = TRUE;
  if(cstrIndications.Find('V') >= 0)
    m_bVomiting = TRUE;
  if(cstrIndications.Find('O') >= 0)
    m_bOther = TRUE;
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CEventRecorder::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_EVENT_RECORDER);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_RE_RECORD_EVENT));

    GetDlgItem(IDC_STATIC_TIME)->SetWindowText(g_pLang->cstrGet_text(ITEM_RE_TIME));
    GetDlgItem(IDC_CHK_COUGH)->SetWindowText(g_pLang->cstrGet_text(ITEM_RE_COUGH));
    GetDlgItem(IDC_CHK_INGESTION)->SetWindowText(g_pLang->cstrGet_text(ITEM_RE_INGESTION));
    GetDlgItem(IDC_CHK_MOVEMENT)->SetWindowText(g_pLang->cstrGet_text(ITEM_RE_MOVEMENT));
    GetDlgItem(IDC_CHK_NAUSEA)->SetWindowText(g_pLang->cstrGet_text(ITEM_RE_NAUSEA));
    GetDlgItem(IDC_CHK_PAIN)->SetWindowText(g_pLang->cstrGet_text(ITEM_RE_PAIN));
    GetDlgItem(IDC_CHK_TALKING)->SetWindowText(g_pLang->cstrGet_text(ITEM_RE_TALKING));
    GetDlgItem(IDC_CHK_VOMITING)->SetWindowText(g_pLang->cstrGet_text(ITEM_RE_VOMITING));
    GetDlgItem(IDC_CHK_OTHER)->SetWindowText(g_pLang->cstrGet_text(ITEM_RE_OTHER));
    GetDlgItem(IDC_STATIC_DESCRIPTION)->SetWindowText(g_pLang->cstrGet_text(ITEM_RE_DESCRIPTION));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    }
  }
