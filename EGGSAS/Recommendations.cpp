/********************************************************************
Recommendations.cpp

Copyright (C) 2003,3004,2009,2012,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CRecommendations class.

  This class provides the user a means of entering recommendations
  for a study.


HISTORY:
08-NOV-03  RET  New.
16-MAR-04  RET  Add help button and message handler.
14-JAN-09  RET  Version CDDRdr.2.08c
                  Display only if view mode.  Change Constructor.
02-SEP-12  RET
             Implement new database table Recommendations
               Add fields: IDC_BTN_ADD_TO_REPORT, IDC_CBX_RECOMM_KEY_PHRASE,
                 IDC_BTN_SAVE_RECOMM_TO_LIST, IDC_BTN_REMOVE_FROM_LIST
               Change methods: OnInitDialog(), vGet_recommendations(),
                 vSet_recommendations(), bChanged()
               Add methods: vFill_key_phrase(), vSet_text()
               Add event handlers: OnCbnSelchangeCbxRecommKeyPhrase(),
                 OnBnClickedBtnRemoveFromList(), OnBnClickedBtnSaveRecommToList(),
                 OnBnClickedBtnAddToReport()
20-SEP-12  RET
             Redo how saving recommendations work based on bug found during test.
               Change methods: OnBnClickedBtnSaveRecommToList()
             For listing recommendation key phrases, instead of an empty line,
             use "Add new key phrase".
               Change methods: vFill_key_phrase()
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedRecommendationsHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "Dbaccess.h"
#include "Recommendations.h"
#include "util.h"
#include ".\recommendations.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecommendations dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the currently active document.
          Indicates why this object is being used (new study, edit, view).
          Pointer to parent window (default is NULL).
********************************************************************/
CRecommendations::CRecommendations(CEGGSASDoc *pDoc, unsigned uFrom, CWnd* pParent /*=NULL*/)
	: CDialog(CRecommendations::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CRecommendations)
	m_cstrRecommendations = _T("");
	//}}AFX_DATA_INIT

  m_pDoc = pDoc;
#if EGGSAS_READER == READER_DEPOT
  m_uFrom = SM_VIEW;
#else
  m_uFrom = uFrom;
#endif
  }


void CRecommendations::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
//{{AFX_DATA_MAP(CRecommendations)
DDX_Text(pDX, IDC_RECOMMENDATIONS, m_cstrRecommendations);
//}}AFX_DATA_MAP
DDX_Control(pDX, IDC_CBX_RECOMM_KEY_PHRASE, m_cRecommList);
}


BEGIN_MESSAGE_MAP(CRecommendations, CDialog)
	//{{AFX_MSG_MAP(CRecommendations)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_RECOMMENDATIONS_HELP, OnBnClickedRecommendationsHelp)
  ON_BN_CLICKED(IDC_BTN_ADD_TO_REPORT, OnBnClickedBtnAddToReport)
  ON_BN_CLICKED(IDC_BTN_REMOVE_FROM_LIST, OnBnClickedBtnRemoveFromList)
  ON_CBN_SELCHANGE(IDC_CBX_RECOMM_KEY_PHRASE, OnCbnSelchangeCbxRecommKeyPhrase)
  ON_BN_CLICKED(IDC_BTN_SAVE_RECOMM_TO_LIST, OnBnClickedBtnSaveRecommToList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecommendations message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Modify the fields for editing/viewing.
********************************************************************/
BOOL CRecommendations::OnInitDialog() 
  {
  RECOMMENDATIONS_RECORD rrRecord;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();
    // Set up the dialog box fields for view/edit/new study
  if(m_uFrom == SM_VIEW)
    {
	  GetDlgItem(IDC_RECOMMENDATIONS)->EnableWindow(FALSE);
	  GetDlgItem(IDC_BTN_ADD_TO_REPORT)->EnableWindow(FALSE);
	  GetDlgItem(IDC_CBX_RECOMM_KEY_PHRASE)->EnableWindow(FALSE);
	  GetDlgItem(IDC_BTN_SAVE_RECOMM_TO_LIST)->EnableWindow(FALSE);
	  GetDlgItem(IDC_BTN_REMOVE_FROM_LIST)->EnableWindow(FALSE);
    GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
      // Center the OK button.
    vCenter_button_horiz(this, GetDlgItem(IDOK));
    }
  else if(m_uFrom == SM_EDIT)
    {
    }

  vFill_key_phrase();
  // Select the Key Phrase from the Study.
  // Get the recommendation key phrase from the study table.  If none (older studies) or
  // if can't find it, select the first entry in the combbox (empty line).
  rrRecord.cstrKeyPhrase = m_pDoc->m_pdPatient.cstrRecommendationKeyPhrase;
  if(m_pDoc->m_pDB->iFind_recommendation(&rrRecord) == SUCCESS)
    m_iRecommSelIndex = m_cRecommList.SelectString(0, m_pDoc->m_pdPatient.cstrRecommendationKeyPhrase);
  else
    m_iRecommSelIndex = 0;
  if(m_iRecommSelIndex >= 0)
    m_cRecommList.SetCurSel(m_iRecommSelIndex);
  m_cstrOrigRecommKeyPhrase = rrRecord.cstrKeyPhrase;


  UpdateData(FALSE);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.
********************************************************************/
void CRecommendations::OnCancel() 
  {
	
	CDialog::OnCancel();
  }

/********************************************************************
OnOK

  Message handler for the OK button.
    Get the contents of all the fields.
********************************************************************/
void CRecommendations::OnOK() 
  {

  UpdateData(TRUE);
	CDialog::OnOK();
  }

/********************************************************************
OnBnClickedRecommendationsHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CRecommendations::OnBnClickedRecommendationsHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_RECOMMENDATIONS_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_RECOMMENDATIONS_HELP);
  }

/////////////////////////////////////////////////////////////////////
//// Support Methods

/********************************************************************
vGet_recommendations - Returns the recommendations to the user.

  inputs: Reference to a CString object that gets the recommendation key phrase.
          Reference to a CString object that gets the recommendation.

  return: None.
********************************************************************/
void CRecommendations::vGet_recommendations(CString &cstrKeyPhrase, CString &cstrRecommendations)
  {

  cstrKeyPhrase = m_cstrRecommKeyPhrase;
  cstrRecommendations = m_cstrRecommendations;
  }

/********************************************************************
vSet_recommendations - Tell this dialog box what the initial recommendations
                       are.

  inputs: Reference to a CString object containing the recommendation key phrase.
          Reference to a CString object containing the recommendation.

  return: None.
********************************************************************/
void CRecommendations::vSet_recommendations(CString &cstrKeyPhrase, CString &cstrRecommendations)
  {

  m_cstrRecommKeyPhrase = cstrKeyPhrase;
  m_cstrOrigRecommKeyPhrase = cstrKeyPhrase;
  m_cstrRecommendations = cstrRecommendations;
  m_cstrOrigRecommendations = m_cstrRecommendations;
  }

/********************************************************************
bChanged - Determines if the recommendations have changed.

  inputs: None.

  return: true if the recommendation has changed.
          false if there is no change in the recommendation.
********************************************************************/
bool CRecommendations::bChanged()
  {
  bool bRet;

  if(m_cstrOrigRecommendations != m_cstrRecommendations
  || m_cstrOrigRecommKeyPhrase != m_cstrRecommKeyPhrase)
    bRet = true;
  else
    bRet = false;

  return(bRet);
  }

/********************************************************************
OnBnClickedBtnAddToReport

  Message handler for the "Add to report" button.
    Save the recommendation corresponding to the key phrase so it can
      be added to the report.
********************************************************************/
void CRecommendations::OnBnClickedBtnAddToReport()
  {
  RECOMMENDATIONS_RECORD rrRecord;
  
  UpdateData(TRUE);
  rrRecord.cstrKeyPhrase = m_cstrRecommKeyPhrase;
  if(m_pDoc->m_pDB->iFind_recommendation(&rrRecord) == SUCCESS)
    {
    // If recommendation is already in the report, don't add it again.
    if(m_cstrRecommendations.Find(rrRecord.cstrDescription) < 0)
      {
      if(m_cstrRecommendations != "")
        m_cstrRecommendations += "\r\n";
      m_cstrRecommendations += rrRecord.cstrDescription;
      }
    }
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedBtnSaveRecommToList

  Message handler for the "Save recommendation to list" button.
    Save the key phrase and description to the Recommendations database
    table.
********************************************************************/
void CRecommendations::OnBnClickedBtnSaveRecommToList()
  {
  RECOMMENDATIONS_RECORD rrRecord;
  CString cstrWinTxtKeyPhrase;
  
  UpdateData(TRUE);
  m_cRecommList.GetWindowText(cstrWinTxtKeyPhrase);

  if(cstrWinTxtKeyPhrase == "")
    {
    g_pLang->vStart_section(SEC_RECOMMENDATIONS);
    MessageBox(g_pLang->cstrGet_text(ITEM_REC_ENTER_RECOMM_KEY_PHRASE), //"Please enter a key phrase for the recommendation.", 
      g_pLang->cstrLoad_string(ITEM_G_INFORMATION));
    }
  else if(m_cstrRecommendations == "")
    {
    g_pLang->vStart_section(SEC_RECOMMENDATIONS);
    MessageBox(g_pLang->cstrGet_text(ITEM_REC_ADD_RECOMM_DESC), //"Please add a description of the recommendation.", 
      g_pLang->cstrLoad_string(ITEM_G_INFORMATION));
    }
  else
    {
    // If the selected item is 0, it is a new key phrase.  Otherwise it is an
    // existing key phrase being updated.
    if(m_iRecommSelIndex == 0)
      {
      rrRecord.cstrKeyPhrase = cstrWinTxtKeyPhrase;
      rrRecord.cstrDescription = m_cstrRecommendations;
      // Save the record.  If the ID already exists, update it.
      m_pDoc->m_pDB->iSave_recomm_desc(&rrRecord);
      }
    else if(m_iRecommSelIndex > 0)
      {
      rrRecord.cstrKeyPhrase = cstrWinTxtKeyPhrase;
      rrRecord.cstrDescription = m_cstrRecommendations;
      m_pDoc->m_pDB->iUpdate_recommendation(m_cstrRecommKeyPhrase, &rrRecord);
      }


    //// If window text is not empty and the selected key phrase is empty, user
    //// is adding a new key phrase. New key phrase is window text.
    //// If selected key phrase is "Add new key phrase" and window text is not empty,
    //// user is adding a new key phrase.  New key phrase is window text.
    //// If selected key phrase is not "Add new key phrase" and window text is not empty,
    //// user is updating the key phrase with a new one.
    //// if selected key phrase and window text are the same, user is update the description.
    //if((cstrWinTxtKeyPhrase != "" && m_cstrRecommKeyPhrase == "")
    //  || (m_cRecommList.GetCurSel() == 0 && cstrWinTxtKeyPhrase != ""))
    //  { // adding a new key phrase. New key phrase is window text.
    //  rrRecord.cstrKeyPhrase = cstrWinTxtKeyPhrase;
    //  rrRecord.cstrDescription = m_cstrRecommendations;
    //  // Save the record.  If the ID already exists, update it.
    //  m_pDoc->m_pDB->iSave_recomm_desc(&rrRecord);
    //  }
    //else if((m_cRecommList.GetCurSel() != 0 && cstrWinTxtKeyPhrase != "")
    //  || (m_cstrRecommKeyPhrase == cstrWinTxtKeyPhrase))
    //  { // updating the key phrase with a new one or update the description.
    //  rrRecord.cstrKeyPhrase = cstrWinTxtKeyPhrase;
    //  rrRecord.cstrDescription = m_cstrRecommendations;
    //  m_pDoc->m_pDB->iUpdate_recommendation(m_cstrRecommKeyPhrase, &rrRecord);
    //  }
 
    
    // Rebuild the combobox and select the empty line.
    vFill_key_phrase();
    // Select the empty entry.
    m_cRecommList.SetCurSel(0);
    m_iRecommSelIndex = 0;
    m_cstrRecommendations = "";
    UpdateData(FALSE);
    }
  }

/********************************************************************
OnBnClickedBtnRemoveFromList

  Message handler for the "Remove from list" button.
    Remove the key phrase and description from the Recommendations database
    table.
********************************************************************/
void CRecommendations::OnBnClickedBtnRemoveFromList()
  {

  UpdateData(TRUE);
  if(m_cstrRecommendations != "")
    {
    g_pLang->vStart_section(SEC_RECOMMENDATIONS);
    MessageBox(g_pLang->cstrGet_text(ITEM_REC_ERASE_RECOMM_DESC), //"Please erase the recommendation.", 
      g_pLang->cstrLoad_string(ITEM_G_INFORMATION));
    }
  else
  {
    m_pDoc->m_pDB->iDelete_recommendation(m_cstrRecommKeyPhrase);
    m_cstrRecommKeyPhrase = "";
    // Rebuild the combobox and select the empty line.
    vFill_key_phrase();
    m_cRecommList.SetCurSel(0);
    UpdateData(FALSE);
  }
  }

/********************************************************************
OnCbnSelchangeCbxRecommKeyPhrase

  Message handler for the Recommendations dropdown list box.
    Save the currently selected key phrase.
********************************************************************/
void CRecommendations::OnCbnSelchangeCbxRecommKeyPhrase()
  {

  UpdateData(TRUE);
  m_iRecommSelIndex = m_cRecommList.GetCurSel();
  m_cRecommList.GetLBText(m_iRecommSelIndex, m_cstrRecommKeyPhrase);
  if(m_iRecommSelIndex <= 0)
    m_cstrRecommendations = "";
  UpdateData(FALSE);
  }

/********************************************************************
vFill_key_phrase - Fill the recommendation key phrse dropdown list box from the
                     Recommendations database table.

  inputs: None.

  return: None.
********************************************************************/
void CRecommendations::vFill_key_phrase()
{
  RECOMMENDATIONS_RECORD rrRecord;

  m_cRecommList.ResetContent();
  m_cRecommList.AddString(g_pLang->cstrLoad_string(ITEM_G_ADD_NEW_KEY_PHRASE)); // Add an empty line.
  if(m_pDoc->m_pDB->iGet_recomm_desc(FIRST_RECORD, &rrRecord) == SUCCESS)
    {
    m_cRecommList.AddString(rrRecord.cstrKeyPhrase);
    while(m_pDoc->m_pDB->iGet_recomm_desc(NEXT_RECORD, &rrRecord) == SUCCESS)
      m_cRecommList.AddString(rrRecord.cstrKeyPhrase);
    }
}

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CRecommendations::vSet_text()
  {
    if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_RECOMMENDATIONS);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_REC_PATIENT_RECOMMENDATIONS));

    GetDlgItem(IDC_BTN_ADD_TO_REPORT)->SetWindowText(g_pLang->cstrGet_text(ITEM_REC_ADD_TO_REPORT));
    GetDlgItem(IDC_BTN_REMOVE_FROM_LIST)->SetWindowText(g_pLang->cstrGet_text(ITEM_REC_REMOVE_FROM_LIST));
    GetDlgItem(IDC_BTN_SAVE_RECOMM_TO_LIST)->SetWindowText(g_pLang->cstrGet_text(ITEM_REC_SAVE_RECOMMENDATION_TO_LIST));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_RECOMMENDATIONS_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    }

  }