/********************************************************************
WhatIf.cpp

Copyright (C) 2008,2012,2013,2016,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CWhatIf class.

  This class provides for saving, recalling, deleting and updating
  What If Scenarios.

HISTORY:
25-JAN-08  RET  New.
29-JUN-08 RET  Version 2.05
                 Change OnBnClickedBtnSave() to reset all periods and good
                   minutes if creating a new WhatIF.
28-AUG-08  RET  Version 2.05c
                  Add link to Research help file.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
13-MAR-13  RET
             Changes for frequency range seleclion.
               Add Frequency to studies.
                 Change methods: vGet_whatif_record()
16-MAR-13  RET
             Changes for frequency range seleclion.
               Change methods: vGet_whatif_record()
09-JUN-16  RET
             Add feature to hide study dates.
               Change methods: vFill_listbox(), vAdd_item_to_list()
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedBtnWhatIfHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "Dbaccess.h"
#include "WhatIf.h"
#include "util.h"


IMPLEMENT_DYNAMIC(CWhatIf, CDialog)

/////////////////////////////////////////////////////////////////////////////
// CWhatIf dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the active document.
          WhatIf ID.
********************************************************************/
CWhatIf::CWhatIf(CEGGSASDoc *pDoc, long lWhatIfID, CWnd* pParent /*=NULL*/)
	: CDialog(CWhatIf::IDD, pParent)
  , m_cstrWhatIfDesc(_T(""))
  {

  m_pDoc = pDoc;
  m_lWhatIfID = lWhatIfID;
  }

CWhatIf::~CWhatIf()
{
}

void CWhatIf::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_EDIT_WHAT_IF_DESCRIPTION, m_cstrWhatIfDesc);
DDV_MaxChars(pDX, m_cstrWhatIfDesc, 50);
DDX_Control(pDX, IDC_LIST_WHAT_IF, m_cWhatIfList);
}


BEGIN_MESSAGE_MAP(CWhatIf, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDC_BTN_RECALL, OnBnClickedBtnRecall)
  ON_BN_CLICKED(IDC_BTN_DELETE, OnBnClickedBtnDelete)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
  ON_BN_CLICKED(IDC_BTN_WHAT_IF_HELP, OnBnClickedBtnWhatIfHelp)
  ON_BN_CLICKED(IDC_BTN_SAVE, OnBnClickedBtnSave)
  ON_LBN_DBLCLK(IDC_LIST_WHAT_IF, OnLbnDblclkListWhatIf)
  ON_LBN_SELCHANGE(IDC_LIST_WHAT_IF, OnLbnSelchangeListWhatIf)
  ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWhatIf message handlers

BOOL CWhatIf::OnInitDialog()
  {
  CString cstrItem; //1, cstrItem2;

  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  vSet_text();

  m_bForceOriginalStudy = false;
  m_bSavedWhatIf = false;

    // insert two columns (REPORT mode) and modify the new header items
  //cstrItem.LoadString(IDS_DATE_TIME);
  //GetDlgItem(IDC_STATIC_DATE_TIME)->SetWindowText(cstrItem);
  //cstrItem.LoadString(IDS_DESCRIPTION);
  //GetDlgItem(IDC_STATIC_DESCRITPION)->SetWindowText(cstrItem);

    // Fill the listbox with dates/times and descriptions from the database.
  if(m_pDoc->m_pDB != NULL)
    {
    vFill_listbox();
    vSet_selected_item(-1);
    vEnable_buttons();
    m_cWhatIfList.SetFocus();
    }

  UpdateData(FALSE);

  return FALSE;  // return TRUE unless you set the focus to a control
//  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnBnClickedOk

  Message handler for "Save" button.
    Save the What If Scenario in the What If database table.
    Also save any other data that has changed in its appropriate table.
********************************************************************/
void CWhatIf::OnBnClickedOk()
  {

  OnOK();
  }

/********************************************************************
OnBnClickedBtnSave

  Message handler for "Save" button.
    Save the What If Scenario in the What If database table.
    Also save any other data that has changed in its appropriate table.
********************************************************************/
void CWhatIf::OnBnClickedBtnSave()
  {
  CString cstrMsg, cstrTitle, cstrItem;
  WHAT_IF wi;
  int iOverwrite, iIndex;

  UpdateData(TRUE);
  if(m_cstrWhatIfDesc != "")
    { // Need a description before saving.
    //cstrItem.LoadString(IDS_WHATIF_NEW);
    cstrItem = g_pLang->cstrLoad_string(ITEM_G_WHATIF_NEW);
    if(m_cstrWhatIfDesc == cstrItem)
      { // Naming a What If scenario "NEW" is not allowed.
      //cstrMsg.Format(IDS_CANT_NAME_WHATIF, cstrItem);
      //cstrTitle.LoadString(IDS_ERROR1);
      cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_CANT_NAME_WHATIF), cstrItem);
      cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
      MessageBox(cstrMsg, cstrTitle, MB_ICONERROR);
      }
    else
      {
      iOverwrite = IDYES;
      if(m_iListSel != 0)
        { // Saving to an existing What If.  Ask if sure.
        //cstrMsg.LoadString(IDS_SURE_OVERWRITE_WHATIF);
        //cstrTitle.LoadString(IDS_WARNING);
        cstrMsg = g_pLang->cstrLoad_string(ITEM_G_SURE_OVERWRITE_WHATIF);
        cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
        iOverwrite = MessageBox(cstrMsg, cstrTitle, MB_YESNO | MB_ICONQUESTION);
        }
      if(iOverwrite  == IDYES)
        { // Get the data that goes in the WHAT_IF database table.
        //cstrItem.LoadString(IDS_WHATIF_NEW);
        cstrItem = g_pLang->cstrLoad_string(ITEM_G_WHATIF_NEW);
        if(m_cstrLstBoxDesc == cstrItem)
          wi.lWhatIfID = INV_LONG;
        else
          wi.lWhatIfID = (long)m_cWhatIfList.GetItemData(m_iListSel);
        wi.lStudyID = m_pDoc->m_pdPatient.lStudyID;
        wi.cstrDescription = m_cstrWhatIfDesc;
          // Date/time: year/month/day-of-month 24-hour:minute
        wi.cstrDateTime = cstrFmt_current_date_time();
        wi.uNumPeriods = m_pDoc->m_pdPatient.uNumPeriods;
        wi.uFrequency = m_pDoc->m_pdPatient.uFrequency;
    
        if(wi.lWhatIfID == INV_LONG)
          { // This is a NEW WhatIF.  Reset all periods and good minutes.
          for(iIndex = 0; iIndex < MAX_TOTAL_PERIODS; ++iIndex)
            {
            wi.fPeriodLen[iIndex] = (float)INV_PERIOD_LEN;
            wi.fPeriodStart[iIndex] = (float)INV_PERIOD_START;
            wi.fGoodMinStart[iIndex] = INV_MINUTE;
            wi.fGoodMinEnd[iIndex] = INV_MINUTE;
            }
          m_pDoc->vReset_periods_analyzed();
          // Baseline
          wi.fPeriodStart[PERIOD_BL] = (float)0.0;
          wi.fPeriodLen[PERIOD_BL] = fRound_down_to_half_min(
              (float)m_pDoc->m_pdPatient.uDataPtsInBaseline / (float)DATA_POINTS_PER_MIN_1);
          // Study
          wi.uNumPeriods = 1;
          wi.fPeriodStart[1] = (float)0.0;
          wi.fPeriodLen[1] = 
                     fRound_down_to_half_min(
                     (float)m_pDoc->m_pdPatient.uPostStimDataPoints
                     / (float)(DATA_POINTS_PER_MIN_1 * 2));
          }
        else
          { // Existing study being renamed.
          memcpy(wi.fPeriodStart, m_pDoc->m_pdPatient.fPeriodStart, sizeof(wi.fPeriodStart));
          memcpy(wi.fPeriodLen, m_pDoc->m_pdPatient.fPeriodLen, sizeof(wi.fPeriodLen));
          memcpy(wi.fGoodMinStart, m_pDoc->m_pdPatient.fGoodMinStart, sizeof(wi.fGoodMinStart));
          memcpy(wi.fGoodMinEnd, m_pDoc->m_pdPatient.fGoodMinEnd, sizeof(wi.fGoodMinEnd));
          memcpy(wi.uRespRate, m_pDoc->m_pdPatient.uRespRate, sizeof(wi.uRespRate));
          }
        if(m_pDoc->m_pDB != NULL)
          {
          if(m_pDoc->m_pDB->iSave_whatif_data(&wi) == FAIL)
            {
            //cstrMsg.LoadString(IDS_FAILED_TO_SAVE_WHATIF);
            cstrMsg = g_pLang->cstrLoad_string(ITEM_G_FAILED_TO_SAVE_WHATIF);
            //cstrTitle.LoadString(IDS_ERROR1);
            cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
            MessageBox(cstrMsg, cstrTitle, MB_ICONERROR);
            }
          else
            {
            m_bSavedWhatIf = true;
            m_lWhatIfID = wi.lWhatIfID;
            vFill_listbox();
            vSet_selected_item(-1);
            vSet_state(m_iListSel);
            vEnable_buttons();
            }
          }
        }
      }
    }
  else
    {
    //cstrMsg.LoadString(IDS_ENTER_DESCRIPTION);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_ENTER_DESCRIPTION);
    //cstrTitle.LoadString(IDS_ERROR1);
    cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    MessageBox(cstrMsg, cstrTitle, MB_ICONERROR);
    }
  }

/********************************************************************
OnBnClickedBtnRecall

  Message handler for "Recall" button.
    Get the What If Scenario from the What If database table corresponding
      to the item selected in the listbox.
    Exit the dialog box.
********************************************************************/
void CWhatIf::OnBnClickedBtnRecall()
  {
  CString cstrMsg, cstrTitle;

  if(m_iListSel >= 0)
    vGet_whatif_record();
  else
    {
    //cstrMsg.LoadString(IDS_SELECT_WHATIF);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_SELECT_WHATIF);
    //cstrTitle.LoadString(IDS_ERROR1);
    cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    MessageBox(cstrMsg, cstrTitle, MB_ICONERROR);
    }
  }

/********************************************************************
OnBnClickedBtnDelete

  Message handler for "Delete" button.
    Delete the What If record from the What If database table corresponding
      to the item selected in the listbox.
********************************************************************/
void CWhatIf::OnBnClickedBtnDelete()
  {
  CString cstrItem, cstrTitle;
  long lWhatIfID;

  UpdateData(TRUE);
  m_iListSel = iGet_selected_item(m_cstrDateTime, m_cstrWhatIfDesc);
  if(m_iListSel > 0)
    { // Ask if user really wants to delete this item.
    //cstrItem.Format(IDS_ASK_DELETE_WHATIF, m_cstrDateTime, m_cstrWhatIfDesc);
    cstrItem = g_pLang->cstrLoad_string(ITEM_G_ASK_DELETE_WHATIF) + "\n\n" + m_cstrDateTime
      + ": " + m_cstrWhatIfDesc;
    //cstrTitle.LoadString(IDS_WARNING);
    cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
    if(MessageBox(cstrItem, cstrTitle, MB_YESNO | MB_ICONQUESTION) == IDYES)
      { // Delete the What If record.
      lWhatIfID = (long)m_cWhatIfList.GetItemData(m_iListSel);
      m_pDoc->m_pDB->iDelete_whatif(lWhatIfID);
      vFill_listbox();
      vSet_selected_item(0);
      vSet_state(m_iListSel);
      vEnable_buttons();
        // If the What If ID is the What If ID current displayed, set a flag
        // so that when closing (i.e. cancel), the state is set to Original
        // study and cancel is changed to OK.
      if(lWhatIfID == m_pDoc->m_wifOrigData.lWhatIfID)
        m_bForceOriginalStudy = true;
      }
    }
  else if(m_iListSel == 0)
    {
    //cstrItem.LoadString(IDS_CANT_DELETE_ORIG_STUDY);
    cstrItem = g_pLang->cstrLoad_string(ITEM_G_CANT_DELETE_ORIG_STUDY);
    //cstrTitle.LoadString(IDS_ERROR1);
    cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    MessageBox(cstrItem, cstrTitle, MB_ICONERROR);
    }
  else
    {
    //cstrItem.LoadString(IDS_SELECT_WHATIF);
    cstrItem = g_pLang->cstrLoad_string(ITEM_G_SELECT_WHATIF);
    //cstrTitle.LoadString(IDS_ERROR1);
    cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    MessageBox(cstrItem, cstrTitle, MB_ICONERROR);
    }
  }

/********************************************************************
OnBnClickedCancel

  Message handler for "Cancel" button.
    Exit the dialog box without doing anything.
********************************************************************/
void CWhatIf::OnBnClickedCancel()
  {

  if(m_bForceOriginalStudy == false)
    {
    if(m_bSavedWhatIf == true)
      vGet_whatif_record();
    else
      OnCancel();
    }
  else
    {
    m_iState = WIFS_ORIGINAL;
  	CDialog::OnOK();
    }
  }

/********************************************************************
OnClose

  Message handler for Closing the dialog box.
    If original study is not being loaded, do normal close.
    If original study is being loaded, set the state to original and
      close with OK.
********************************************************************/
void CWhatIf::OnClose()
  {

  if(m_bForceOriginalStudy == false)
    CDialog::OnClose();
  else
    {
    m_iState = WIFS_ORIGINAL;
  	CDialog::OnOK();
    }
  }

/********************************************************************
OnBnClickedBtnWhatIfHelp

  Message handler for "Help" button.
    Display help for this dialog box.
********************************************************************/
void CWhatIf::OnBnClickedBtnWhatIfHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_BTN_WHAT_IF_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_BTN_WHAT_IF_HELP);
  }

/********************************************************************
OnLbnDblclkListWhatIf

  Message handler for double clicking on an item in the listbox.
    Get the What If Scenario from the What If database table corresponding
      to the item selected in the listbox.
    Exit the dialog box.
********************************************************************/
void CWhatIf::OnLbnDblclkListWhatIf()
  {

  vGet_whatif_record();
  }

/********************************************************************
OnLbnSelchangeListWhatIf

  Message handler for changing the selected item in the listbox.
    Display the description in the Description field.
********************************************************************/
void CWhatIf::OnLbnSelchangeListWhatIf()
  {

  UpdateData(TRUE);
  m_iListSel = iGet_selected_item(m_cstrDateTime, m_cstrWhatIfDesc);
  if(m_iListSel >= 0)
    {
    UpdateData(FALSE);
    vEnable_buttons();
    }
  }

/////////////////////////////////////////////////////////////////////
//// Private Support methods

/********************************************************************
vFill_listbox - Fill in the listbox with the date/time and description
                of the What If records matching the study ID.

  inputs: true to add a "new" item to the listbox, otherwise false.

  return: None.
********************************************************************/
void CWhatIf::vFill_listbox() //bool bNewItem)
  {
  WHAT_IF wi;
  CString cstrDate, cstrItem;
  short int iPos;
  int iIndex, iCnt;

    // Display the hourglass cursor.
  SetCursor(LoadCursor(NULL, IDC_WAIT));
  ShowCursor(TRUE);

  m_cWhatIfList.ResetContent(); //  .DeleteAllItems();
  iCnt = 0;

//  if(bNewItem == true)
//    {
    //cstrItem.LoadString(IDS_WHATIF_NEW);
    cstrItem = g_pLang->cstrLoad_string(ITEM_G_WHATIF_NEW);
    vAdd_item_to_list(cstrFmt_current_date_time(), cstrItem, INV_LONG);
    ++iCnt;
//    }
    // Index in the listbox of the original study.
  m_iOriginalStudyIndex = iCnt;

//#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
  //if(g_pConfig->m_bHideStudyDates == false)
  if(m_pDoc->m_pdPatient.cstrDate.GetLength() <= 10)
    {
    //#else
    // Reformat the original study date so its in the same format as the
    // What If dates.
    if((iIndex = m_pDoc->m_pdPatient.cstrDate.ReverseFind('/')) > 0)
      {
      cstrDate.Format("%s/%s",
        m_pDoc->m_pdPatient.cstrDate.Right(m_pDoc->m_pdPatient.cstrDate.GetLength() - iIndex - 1),
        m_pDoc->m_pdPatient.cstrDate.Left(iIndex));
      }
    else
      cstrDate = m_pDoc->m_pdPatient.cstrDate;
    }
  else
    cstrDate = "";
//#endif
  //cstrItem.LoadString(IDS_WHATIF_ORIGINAL_STUDY);
  cstrItem = g_pLang->cstrLoad_string(ITEM_G_WHATIF_ORIGINAL_STUDY);
  vAdd_item_to_list(cstrDate, cstrItem, INV_LONG);
  ++iCnt;

  wi.lStudyID = m_pDoc->m_pdPatient.lStudyID;
  iPos = FIRST_RECORD;
  while(m_pDoc->m_pDB->iFind_next_whatif(iPos, &wi) == SUCCESS)
    { // Have a record, display it.
    vAdd_item_to_list(wi.cstrDateTime, wi.cstrDescription, wi.lWhatIfID);
    ++iCnt;
    iPos = NEXT_RECORD;
    }

    // Restore the arrow cursor.
  SetCursor(LoadCursor(NULL, IDC_ARROW));
  ShowCursor(TRUE);
  }

/********************************************************************
cstrFmt_current_date_time - Format the current date and time.

  inputs: None.

  return: CString containing the current date and time.
********************************************************************/
CString CWhatIf::cstrFmt_current_date_time(void)
  {
  CString cstrDateTime;
  CTime ctDate;

  ctDate = CTime::GetCurrentTime();
  cstrDateTime = ctDate.Format("%Y/%m/%d %H:%M");
  return (cstrDateTime);
  }

/********************************************************************
vAdd_item_to_list - Format a line and add it to the listbox.

  inputs: CString containing the date and time.
          CString containing the description.
          The What If database ID.

  return: None.
********************************************************************/
void CWhatIf::vAdd_item_to_list(CString cstrDateTime, CString cstrDescription, long lID)
  {
  CString cstrItem;
  int iIndex;

  if(cstrDateTime.IsEmpty() == false)
    cstrItem.Format("%s\t    %s", cstrReformat_date(cstrDateTime), cstrDescription);
  else
    cstrItem.Format("\t\t    %s", cstrDescription);
  iIndex = m_cWhatIfList.AddString(cstrItem);
  m_cWhatIfList.SetItemData(iIndex, (DWORD)lID); // What If ID
  }

/********************************************************************
vGet_whatif_record - Get the What If record matching the What If ID
                     from the selected item in the listbox.  Fill in
                     the patient data from the record and close the
                     dialog box.
                     Also save the What If record in the Document class.

  inputs: None.

  return: None.
********************************************************************/
void CWhatIf::vGet_whatif_record(void)
  {
  WHAT_IF wi;

  UpdateData(TRUE);
  m_iListSel = iGet_selected_item(m_cstrDateTime, m_cstrWhatIfDesc);
  if(m_iListSel > m_iOriginalStudyIndex)
    { // Patient name has been selected, get the name and date.
    wi.lWhatIfID = (long)m_cWhatIfList.GetItemData(m_iListSel);
    m_pDoc->m_wifOrigData.lWhatIfID = wi.lWhatIfID;
    if(m_pDoc->m_pDB->iGet_whatif_data(&wi) == SUCCESS)
      { // Got the data.  Update the patient data.
      m_pDoc->m_wifOrigData.lStudyID = wi.lStudyID;
      m_pDoc->m_pdPatient.uNumPeriods = wi.uNumPeriods;
      m_pDoc->m_wifOrigData.uNumPeriods = wi.uNumPeriods;
      m_pDoc->m_wifOrigData.cstrDateTime = wi.cstrDateTime;
      m_pDoc->m_wifOrigData.cstrDescription = wi.cstrDescription;
      m_pDoc->m_pdPatient.uFrequency = wi.uFrequency;
      m_pDoc->m_wifOrigData.uFrequency = wi.uFrequency;
        // Copy the what if data to the patient data.
      memcpy(m_pDoc->m_pdPatient.fPeriodStart, wi.fPeriodStart, sizeof(wi.fPeriodStart));
      memcpy(m_pDoc->m_pdPatient.fPeriodLen, wi.fPeriodLen, sizeof(wi.fPeriodLen));
      memcpy(m_pDoc->m_pdPatient.fGoodMinStart, wi.fGoodMinStart, sizeof(wi.fGoodMinStart));
      memcpy(m_pDoc->m_pdPatient.fGoodMinEnd, wi.fGoodMinEnd, sizeof(wi.fGoodMinEnd));
      memcpy(m_pDoc->m_pdPatient.uRespRate, wi.uRespRate, sizeof(wi.uRespRate));
        // Save the original what if data.
      memcpy(m_pDoc->m_wifOrigData.fPeriodStart, wi.fPeriodStart, sizeof(wi.fPeriodStart));
      memcpy(m_pDoc->m_wifOrigData.fPeriodLen, wi.fPeriodLen, sizeof(wi.fPeriodLen));
      memcpy(m_pDoc->m_wifOrigData.fGoodMinStart, wi.fGoodMinStart, sizeof(wi.fGoodMinStart));
      memcpy(m_pDoc->m_wifOrigData.fGoodMinEnd, wi.fGoodMinEnd, sizeof(wi.fGoodMinEnd));
      memcpy(m_pDoc->m_wifOrigData.uRespRate, wi.uRespRate, sizeof(wi.uRespRate));

      m_pDoc->vSet_freq_range(wi.uFrequency);
      //int iFreq;
      //// Set the frequency range.
      //if(wi.uFrequency == 15)
      //  iFreq = FREQ_LIMIT_15_CPM;
      //else if(wi.uFrequency == 60)
      //  iFreq = FREQ_LIMIT_60_CPM;
      //else
      //  iFreq = FREQ_LIMIT_200_CPM;
      //theApp.vSet_freq_range(iFreq);

        // Set the periods analyzed or not analyzed.
      m_pDoc->vSet_rsrch_pds_analyzed();
      vSet_state(m_iListSel);
    	CDialog::OnOK();
      }
    }
  else if(m_iListSel == m_iOriginalStudyIndex)
    { // Original.
    vSet_state(m_iListSel);
   	CDialog::OnOK();
    }
  else
    { // Nothing has been selected.
    CString cstrMsg, cstrTitle;
    //cstrMsg.LoadString(IDS_SELECT_WHATIF_LIST);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_SELECT_WHATIF_LIST);
    //cstrTitle.LoadString(IDS_ERROR1);
    cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    MessageBox(cstrMsg, cstrTitle, MB_ICONERROR);
    }
  }

/********************************************************************
iGet_selected_item - Get the date/time and description from the 
                     selected item in the list box.

  inputs: Reference to a CString that gets the date and time.
          Reference to a CString that gets the description.

  return: Index of the selected item in the listbox.
********************************************************************/
int CWhatIf::iGet_selected_item(CString &cstrDateTime, CString &cstrDesc)
  {
  int iSel = 0;

  iSel = m_cWhatIfList.GetCurSel();
  if(iSel >= 0)
    {
    vParse_list_item(cstrDateTime, cstrDesc);
    }
  else
    {
    cstrDateTime = "";
    cstrDesc = "";
    }
  return(iSel);
  }

/********************************************************************
vSet_state - Set the state of the dialog box.
             See the enumerated list eWhatIfState.

  inputs: Index of selected item in the listbox.

  return: None.
********************************************************************/
void CWhatIf::vSet_state(int iSel)
  {
  CString cstrItem;

  //cstrItem.LoadString(IDS_WHATIF_ORIGINAL_STUDY);
  cstrItem = g_pLang->cstrLoad_string(ITEM_G_WHATIF_ORIGINAL_STUDY);
  if(m_cstrLstBoxDesc == cstrItem)
    m_iState = WIFS_ORIGINAL;
  else
    {
    cstrItem = g_pLang->cstrLoad_string(ITEM_G_WHATIF_NEW);
    //cstrItem.LoadString(IDS_WHATIF_NEW);
    if(m_cstrLstBoxDesc == cstrItem)
      m_iState = WIFS_NEW;
    else
      m_iState = WIFS_OTHER;
    }
  }

/********************************************************************
vSet_selected_item - Select the specified item in the listbox.

  inputs: Index of item to select in the listbox.  -1 if the current
            What If ID is to be used to select the item in the listbox.

  return: None.
********************************************************************/
void CWhatIf::vSet_selected_item(int iSel)
  {
  CString cstrDate;

  if(iSel == -1)
    iSel = iFind_whatif_id_in_list(m_lWhatIfID);
  m_iListSel = iSel;
  m_cWhatIfList.SetCurSel(iSel);
  vParse_list_item(cstrDate, m_cstrWhatIfDesc);
  m_iState = WIFS_NEW;

  vSet_state(iSel);
  }

/********************************************************************
vEnable_buttons - Enable buttons based on what What If study is selected.

  inputs: None.

  return: None.
********************************************************************/
void CWhatIf::vEnable_buttons(void)
  {
  CString cstrDateTime, cstrItem;

  //cstrItem.LoadString(IDS_WHATIF_ORIGINAL_STUDY);
  cstrItem = g_pLang->cstrLoad_string(ITEM_G_WHATIF_ORIGINAL_STUDY);
  iGet_selected_item(cstrDateTime, m_cstrLstBoxDesc);
  if(m_cstrLstBoxDesc == cstrItem)
    { // Original study selected.  Can't Save or Delete.
    GetDlgItem(IDC_BTN_SAVE)->EnableWindow(FALSE);
    GetDlgItem(IDC_BTN_RECALL)->EnableWindow(TRUE);
    GetDlgItem(IDC_BTN_DELETE)->EnableWindow(FALSE);
    }
  else
    {
    //cstrItem.LoadString(IDS_WHATIF_NEW);
    cstrItem = g_pLang->cstrLoad_string(ITEM_G_WHATIF_NEW);
    if(m_cstrLstBoxDesc == cstrItem)
      { // New is selected. Can't Recall or Delete.
      GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
      GetDlgItem(IDC_BTN_RECALL)->EnableWindow(FALSE);
      GetDlgItem(IDC_BTN_DELETE)->EnableWindow(FALSE);
      }
    else
      { // What If is selected. Can do all button actions.
      GetDlgItem(IDC_BTN_SAVE)->EnableWindow(TRUE);
      GetDlgItem(IDC_BTN_RECALL)->EnableWindow(TRUE);
      GetDlgItem(IDC_BTN_DELETE)->EnableWindow(TRUE);
      }
    }
  }

/********************************************************************
vParse_list_item - Parse the selected listbox item into its date/time
                   and description.

  inputs: Reference to a CString object that gets the date and time.
          Reference to a CString object that gets the description.

  return: None.
********************************************************************/
void CWhatIf::vParse_list_item(CString &cstrDateTime, CString &cstrDesc)
  {
  CString cstrItem;
  int iIndex;

  m_cWhatIfList.GetText(m_cWhatIfList.GetCurSel(), cstrItem);
  if((iIndex = cstrItem.Find('\t')) > 0)
    {
    cstrDateTime = cstrItem.Left(iIndex);
    cstrDesc = cstrItem.Right(cstrItem.GetLength() - (iIndex + 1));
    cstrDesc.Trim();
    }
  else
    {
    cstrDateTime = "";
    cstrDesc = "";
    }
  }

int CWhatIf::iFind_whatif_id_in_list(long lWhatIfID)
  {
  int iIndex;
  long lID;

  for(iIndex = 0; iIndex < m_cWhatIfList.GetCount(); ++iIndex)
    {
    lID = (long)m_cWhatIfList.GetItemData(iIndex);
    if(lID == lWhatIfID)
      break;
    }
  if(iIndex == m_cWhatIfList.GetCount())
    iIndex = 0;
  return(iIndex);
  }

/////////////////////////////////////////////////////////////////////
//// Public Support methods

/********************************************************************
cstrGet_whatif_desc - Get the What If description selected in the
                      listbox.  This is for use by the parent object.
                      The string is formatted as follows:
                        date time: description.

  inputs: None.

  return: CString containing the description.
********************************************************************/
CString CWhatIf::cstrGet_whatif_desc(void)
  {
  CString cstrItem;

  cstrItem.Format("%s: %s", m_cstrDateTime, m_cstrWhatIfDesc);
  return (cstrItem);
  }

/********************************************************************
iGet_state - Get the state of the dialog box.

  inputs: None.

  return: State of the dialog box.  See enumerated list eWhatIfState.
********************************************************************/
int CWhatIf::iGet_state(void)
  {
  return(m_iState);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CWhatIf::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_WHAT_IF);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_WI_WHATIFSCENARIO));

    GetDlgItem(IDC_STATIC_DATE_TIME)->SetWindowText(g_pLang->cstrGet_text(ITEM_WI_DATETIME));
    GetDlgItem(IDC_STATIC_DESCRITPION)->SetWindowText(g_pLang->cstrGet_text(ITEM_WI_DESCRIPTION));
    GetDlgItem(IDC_STATIC_DESCRIPTION_1)->SetWindowText(g_pLang->cstrGet_text(ITEM_WI_DESCRIPTION));
    GetDlgItem(IDC_BTN_SAVE)->SetWindowText(g_pLang->cstrGet_text(ITEM_WI_SAVE));
    GetDlgItem(IDC_BTN_RECALL)->SetWindowText(g_pLang->cstrGet_text(ITEM_WI_RECALL));
    GetDlgItem(IDC_BTN_DELETE)->SetWindowText(g_pLang->cstrGet_text(ITEM_WI_DELETE));

    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_BTN_WHAT_IF_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    }
  }








