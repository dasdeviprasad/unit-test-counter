/********************************************************************
RespEditResearch.cpp

Copyright (C) 2007,2009,2012,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CRespEditResearch class.

  This class provides the user a means of changing/viewing the
  respiration rate for a Research a study.


HISTORY:
01-JUL-07  RET  New.
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
#include "RespEditResearch.h"
#include "util.h"
#include ".\respeditresearch.h"


// CRespEditResearch dialog

IMPLEMENT_DYNAMIC(CRespEditResearch, CDialog)

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the active document that started this dialog box.
          Where the diaglog box is being called from (edit, view, etc).
          Pointer to the window that started this dialog box.

********************************************************************/
CRespEditResearch::CRespEditResearch(CEGGSASDoc *pDoc, unsigned uFrom, CWnd* pParent /*=NULL*/)
	: CDialog(CRespEditResearch::IDD, pParent)
  , m_cstrRespiration(_T(""))
  {

  m_pDoc = pDoc;
  m_uFrom = uFrom;
#if EGGSAS_READER == READER_DEPOT
    // If in What IF, allowed to change respiration rates.
  if(m_pDoc->m_bUsingWhatIf == false)
    m_uFrom = SM_VIEW;
#endif
  }

/********************************************************************
Destructor

********************************************************************/
CRespEditResearch::~CRespEditResearch()
{
}

void CRespEditResearch::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_RESPIRATION, m_cRespirationList);
  DDX_Text(pDX, IDC_EDIT_RESPIRATION, m_cstrRespiration);
  }


BEGIN_MESSAGE_MAP(CRespEditResearch, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
  ON_BN_CLICKED(IDC_EDITVIEWRESP_HELP, OnBnClickedEditviewrespHelp)
  ON_LBN_SELCHANGE(IDC_LIST_RESPIRATION, OnLbnSelchangeListRespiration)
  ON_BN_CLICKED(IDC_BTN_SET_RESP, OnBnClickedBtnSetResp)
  ON_EN_CHANGE(IDC_EDIT_RESPIRATION, OnEnChangeEditRespiration)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRespEditResearch message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Fill in the listbox with the respiration rates.
    Enable/disable fields depending on menu this is called from.
********************************************************************/
BOOL CRespEditResearch::OnInitDialog()
  {
  int iIndex, iLstBoxIndex;
  CString cstrLine, cstrResp;

  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  vSet_text();
    // Set up the dialog box fields for view/edit/new study
  if(m_uFrom == SM_VIEW)
    {
    ((CEdit *)GetDlgItem(IDC_EDIT_RESPIRATION))->SetReadOnly(TRUE);
    GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
      // Center the OK button.
    vCenter_button_horiz(this, GetDlgItem(IDOK));
    }
  GetDlgItem(IDC_BTN_SET_RESP)->EnableWindow(FALSE);
    // Fill in the list box with the respiration rates.
  //m_cstrBaseText.LoadString(IDS_POST_STIM_PERIOD);
  m_cstrBaseText = g_pLang->cstrLoad_string(ITEM_G_POST_STIM_PERIOD);
    // Post-stimulation periods start at period 1 (period 0 is baseline).
    // The Item data for a listbox item is the respiration rate.
  for(iIndex = 1; iIndex <= m_pDoc->m_pdPatient.uNumPeriods; ++iIndex)
    {
      // Save the initial respiration rate.
    m_uRespRate[iIndex] = m_pDoc->m_pdPatient.uRespRate[iIndex];
      // Add a line to the listbox.
    cstrResp = cstrGet_respiration(m_pDoc->m_pdPatient.uRespRate[iIndex]);
    cstrLine = cstrFmt_lstbox_line(cstrResp, iIndex);
    iLstBoxIndex = m_cRespirationList.AddString(cstrLine);
    m_cRespirationList.SetItemData(iLstBoxIndex, m_pDoc->m_pdPatient.uRespRate[iIndex]);
    }
    // Select the first item.
  m_bNewLstBoxSel = true;
  m_iLstBoxSel = 0;
  m_cRespirationList.SetCurSel(0);
  m_cstrRespiration = cstrGet_respiration((unsigned short)m_cRespirationList.GetItemData(0));
  m_bRespChanged = false;

  if(m_uFrom == SM_EDIT)
    { // Editing, go to the edit field.
    GetDlgItem(IDC_EDIT_RESPIRATION)->SetFocus();
    }

  UpdateData(FALSE);
  return FALSE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnOK

  Message handler for the OK button.
    Save the respiration rates from the listbox.
********************************************************************/
void CRespEditResearch::OnBnClickedOk()
  {

  UpdateData(TRUE);

    // First check to see if a respiration has been changed without setting in the
    // listbox.  If so, set it.
  if(m_bRespChanged == true)
    vSet_respiration(m_iLstBoxSel); // Set the respiration.
  vSave_respiration();

  OnOK();
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.
********************************************************************/
void CRespEditResearch::OnBnClickedCancel()
  {
  int iIndex;
  bool bChanged;
  unsigned short uLBRespRate;

  UpdateData(TRUE);
    // First check to see if a respiration has been changed without setting in the
    // listbox.  If so, set it.
  if(m_bRespChanged == true)
    vSet_respiration(m_iLstBoxSel); // Set the respiration.
    // Second, check to see if any respiration has been changed and if so ask
    // user if the respiration rates should be saved.  If yes, save them and
    // exit the dialog box as if the OK button has been selected.
  bChanged = false;
  for(iIndex = 1; iIndex <= m_pDoc->m_pdPatient.uNumPeriods; ++iIndex)
    {
    uLBRespRate = (short unsigned)m_cRespirationList.GetItemData(iIndex - 1);
    if(m_uRespRate[iIndex] != (short unsigned)m_cRespirationList.GetItemData(iIndex - 1))
      { // Respiration rate has changed.
      bChanged = true;
      break;
      }
    }
  if(bChanged == true)
    { // Respiration rates have changed.  Ask user if want to save them.
    CString cstrTitle, cstrMsg;
    //cstrTitle.LoadString(IDS_WARNING);
    cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
    //cstrMsg.LoadString(IDS_RESP_NOT_SAVED);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_RESP_NOT_SAVED);
    if(MessageBox(cstrMsg, cstrTitle, MB_YESNO) == IDYES)
      { // Set the respiration.
      vSave_respiration();
      }
    else
      bChanged = false; // Not saving.
    }

  if(bChanged == false)
    OnCancel();
  else
    OnOK();
  }

/********************************************************************
OnBnClickedEditviewrespHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CRespEditResearch::OnBnClickedEditviewrespHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_EDITVIEWRESP_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_EDITVIEWRESP_HELP);
  }

/********************************************************************
OnLbnSelchangeListRespiration - Handler for selecting a different period
                                in the listbox.

  Display the respiration rate of the selected item in the edit box
    for the user to change.
********************************************************************/
void CRespEditResearch::OnLbnSelchangeListRespiration()
  {
  CString cstrTitle, cstrMsg;

  UpdateData(TRUE);
  if(m_bRespChanged == true)
    { // User changed the respiration and didn't set it.
    //cstrTitle.LoadString(IDS_WARNING);
    cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
    //cstrMsg.LoadString(IDS_CHG_RESP_NOT_SET);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_CHG_RESP_NOT_SET);
    if(MessageBox(cstrMsg, cstrTitle, MB_YESNO) == IDYES)
      vSet_respiration(m_iLstBoxSel); // Set the respiration.
    m_bRespChanged = false;
    }
  m_iLstBoxSel = m_cRespirationList.GetCurSel();
  m_cstrRespiration = cstrGet_respiration((unsigned short)m_cRespirationList.GetItemData(m_iLstBoxSel));
  m_bNewLstBoxSel = true; // Inhibit the "Set Respiration" button from
                          // being enabled.
  if(m_uFrom == SM_EDIT)
    { // Editing, go to the edit field.
    GetDlgItem(IDC_EDIT_RESPIRATION)->SetFocus();
    }
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedBtnSetResp - Handler for "Set Respiration" button.


  Modify the selected line in the listbox with the respiration rate
    the user entered.
********************************************************************/
void CRespEditResearch::OnBnClickedBtnSetResp()
  {

  UpdateData(TRUE);
  vSet_respiration(m_iLstBoxSel);
  m_cRespirationList.SetCurSel(m_iLstBoxSel);
  UpdateData(FALSE);
  }

/********************************************************************
OnEnChangeEditRespiration - Handler for changing the respiration rate
                            edit box.

  Enable the "Set Respiration" button if this is not a new entry as a
    result of the user selecting a new line in the listbox.
********************************************************************/
void CRespEditResearch::OnEnChangeEditRespiration()
  {

  if(m_bNewLstBoxSel == true)
    m_bNewLstBoxSel = false;
  else
    {
    GetDlgItem(IDC_BTN_SET_RESP)->EnableWindow(TRUE);
    m_bRespChanged = true;
    }
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
void CRespEditResearch::vGet_respiration(short unsigned *puRespRate)
  {
  int iIndex;

  for(iIndex = 0; iIndex < m_pDoc->m_pdPatient.uNumPeriods; ++iIndex)
    *(puRespRate + iIndex) = m_uRespRate[iIndex + 1];
  }

/********************************************************************
cstrFmt_lstbox_line - Format a line for the listbox.
                      If the respiration rate hasn't been entered yet
                      (i.e. 0), don't put anything for the rate.

  inputs: CString containing the Respiration rate.
          Period.

  return: CString object containing the line for the listbox.
********************************************************************/
CString CRespEditResearch::cstrFmt_lstbox_line(CString cstrRespRate, int iPeriod)
  {
  CString cstrLine;

  if(cstrRespRate.IsEmpty() == FALSE)
    cstrLine.Format("%s %d: %s", m_cstrBaseText, iPeriod, cstrRespRate);
  else
    cstrLine.Format("%s %d: ", m_cstrBaseText, iPeriod);
  return(cstrLine);
  }

/********************************************************************
vSet_respiration - Set the respiration rate from the edit field into the
                   listbox.

  inputs: Currently selected listbox item.

  return: None.
********************************************************************/
void CRespEditResearch::vSet_respiration(int iLstBoxSel)
  {
  int iLstBoxIndex;
  CString cstrLine;

  cstrLine = cstrFmt_lstbox_line(m_cstrRespiration, iLstBoxSel + 1);
    // Delete the line from the listbox.
  m_cRespirationList.DeleteString(iLstBoxSel);
    // Insert the modified line.
  iLstBoxIndex = m_cRespirationList.InsertString(iLstBoxSel, cstrLine);
  m_cRespirationList.SetItemData(iLstBoxIndex, atoi(m_cstrRespiration));
    // Disable the "Set Respiration" button.
  GetDlgItem(IDC_BTN_SET_RESP)->EnableWindow(FALSE);
  m_bRespChanged = false;
  }

/********************************************************************
vSave_respiration - Save the respiration rates in a local array for the
                    caller to get.

  inputs: None.

  return: None.
********************************************************************/
void CRespEditResearch::vSave_respiration()
  {
  int iIndex, iLstBoxIndex;

//  for(iIndex = 0; iIndex <= m_pDoc->m_pdPatient.uNumPeriods; ++iIndex)
  for(iIndex = 1, iLstBoxIndex = 0; iIndex <= m_pDoc->m_pdPatient.uNumPeriods;
  ++iIndex, ++iLstBoxIndex)
    m_uRespRate[iIndex] = (short unsigned)m_cRespirationList.GetItemData(iLstBoxIndex);
  }

/********************************************************************
cstrGet_respiration - Get the respiration rate as a string.  If the
                      respiration rate is 0, the string is empty.

  inputs: Respiration rate.

  return: CString containing the respiration rate, or empty if the
          respiration rate is 0.
********************************************************************/
CString CRespEditResearch::cstrGet_respiration(unsigned short uRespRate)
  {
  CString cstrResp;

  if(uRespRate != 0)
    cstrResp.Format("%d", uRespRate);
  else
    cstrResp.Empty();
  return(cstrResp);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CRespEditResearch::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_RESPIRATION);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_R_RESPIRATION));

    GetDlgItem(IDC_STATIC_CHANGE_RESP)->SetWindowText(g_pLang->cstrGet_text(ITEM_R_CHANGE_RESP));
    GetDlgItem(IDC_BTN_SET_RESP)->SetWindowText(g_pLang->cstrGet_text(ITEM_R_SET_RESP));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_EDITVIEWRESP_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    //g_pLang->vEnd_section();
    }
  }
