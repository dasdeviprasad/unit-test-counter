/********************************************************************
StimulationMedium.cpp

Copyright (C) 2007,2008,2009,2012,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CStimulationMedium class.

  This class provides for entering the amount of stimulation medium ingested.

HISTORY:
10-MAY-07  RET  New.
03-JUL-08  RET  Version 2.05
                  Fix problem where units number and units string were
                    not being handled properly.
                    Change methods: OnInitDialog(), OnBnClickedOk().
                  Change OnBnClickedOk() to not update the INI file with the
                    units if viewing.
14-JAN-09  RET  Version CDDRdr.2.08c
                  Display only if view mode.  Change Constructor.
                  Change vGet_stim_med_amount() to return the original data
                    from the CDoc class in View mode.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedWaterloadHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "StimulationMedium.h"
#include "util.h"
#include ".\stimulationmedium.h"

// CStimulationMedium dialog

IMPLEMENT_DYNAMIC(CStimulationMedium, CDialog)
CStimulationMedium::CStimulationMedium(CEGGSASDoc *pDoc, unsigned uType,
                                       CWnd* pParent /*=NULL*/)
	: CDialog(CStimulationMedium::IDD, pParent)
  , m_bMilliliters(FALSE)
  , m_bOunces(FALSE)
  , m_cstrDescription(_T(""))
  , m_bGrams(FALSE)
  , m_uQuantity(0)
  {

  m_pDoc = pDoc;
#if EGGSAS_READER == READER_DEPOT
  m_uType = SM_VIEW;
#else
  m_uType = uType;
#endif
  }

CStimulationMedium::~CStimulationMedium()
{
}

void CStimulationMedium::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Check(pDX, IDC_MILLILITERS, m_bMilliliters);
DDX_Check(pDX, IDC_OUNCES, m_bOunces);
DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_cstrDescription);
DDX_Check(pDX, IDC_GRAMS, m_bGrams);
DDX_Text(pDX, IDC_EDIT_QUANTITY, m_uQuantity);
}


BEGIN_MESSAGE_MAP(CStimulationMedium, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
  ON_BN_CLICKED(IDC_WATERLOAD_HELP, OnBnClickedWaterloadHelp)
  ON_BN_CLICKED(IDC_OUNCES, OnBnClickedOunces)
  ON_BN_CLICKED(IDC_MILLILITERS, OnBnClickedMilliliters)
  ON_BN_CLICKED(IDC_GRAMS, OnBnClickedGrams)
END_MESSAGE_MAP()


// CStimulationMedium message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Set the title line
    Modify the fields for editing/viewing.
********************************************************************/
BOOL CStimulationMedium::OnInitDialog() 
  {
  CString cstrTemp;
  CFont cfBoldFont;
  int iDefaultUnits;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();
    // Modify the dialog box for new study/edit/view
    // Change the title line.
  if(m_uType == SM_EDIT)
    { // Only asking for the amount of water ingested.
    //cstrTemp.LoadString(IDS_ENTER_TYPE_MED_QTY);
    cstrTemp = g_pLang->cstrLoad_string(ITEM_G_ENTER_TYPE_MED_QTY);
    GetDlgItem(IDC_BEGIN_STIM_MEDIUM)->SetWindowText((LPCTSTR)cstrTemp);
    //cstrTemp.LoadString(IDS_OK);
    cstrTemp = g_pLang->cstrLoad_string(ITEM_G_OK);
    GetDlgItem(IDOK)->SetWindowText((LPCTSTR)cstrTemp);
    // Enable/disable fields and change field text.
    //cstrTemp.LoadString(IDS_SAVE_CLOSE); // Change OK button text.
    cstrTemp = g_pLang->cstrLoad_string(ITEM_G_SAVE_CLOSE);
    GetDlgItem(IDOK)->SetWindowText((LPCTSTR)cstrTemp);
    }
  else if(m_uType == SM_VIEW)
    { // Displaying the amount of water ingested.
    //cstrTemp.LoadString(IDS_TYPE_MED_QTY);
    cstrTemp = g_pLang->cstrLoad_string(ITEM_G_TYPE_MED_QTY);
    GetDlgItem(IDC_BEGIN_STIM_MEDIUM)->SetWindowText((LPCTSTR)cstrTemp);
    //cstrTemp.LoadString(IDS_OK);
    cstrTemp = g_pLang->cstrLoad_string(ITEM_G_OK);
    GetDlgItem(IDOK)->SetWindowText((LPCTSTR)cstrTemp);
    // Enable/disable fields and change field text.
		GetDlgItem(IDC_EDIT_DESCRIPTION)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_QUANTITY)->EnableWindow(FALSE);
		GetDlgItem(IDC_OUNCES)->EnableWindow(FALSE);
		GetDlgItem(IDC_MILLILITERS)->EnableWindow(FALSE);
		GetDlgItem(IDC_GRAMS)->EnableWindow(FALSE);
    GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
      // Center the OK button.
    vCenter_button_horiz(this, GetDlgItem(IDOK));
    //cstrTemp.LoadString(IDS_OK);
    cstrTemp = g_pLang->cstrLoad_string(ITEM_G_OK);
    GetDlgItem(IDOK)->SetWindowText((LPCTSTR)cstrTemp);
    }

  // Get the font to use in dialog box fields.
  vSet_font(this, NULL, &cfBoldFont, FW_BOLD);
    // Set the font in the dialog box fields.
  GetDlgItem(IDC_BEGIN_STIM_MEDIUM)->SetFont(&cfBoldFont);
	
    // Fill in the dialog box fields.
  if(m_pDoc->m_pdPatient.uWaterAmount != 0)
    {
    m_uQuantity = m_pDoc->m_pdPatient.uWaterAmount;
    m_cstrUnits = m_pDoc->m_pdPatient.cstrWaterUnits;
    GetDlgItem(IDC_MILLILITERS)->GetWindowText(cstrTemp);
    if(cstrTemp == m_cstrUnits)
      { // milliliters
      m_bMilliliters = TRUE;
      m_bOunces = FALSE;
      m_bGrams = FALSE;
      }
    else
      {
      GetDlgItem(IDC_OUNCES)->GetWindowText(cstrTemp);
      if(cstrTemp == m_cstrUnits)
        { // Ounces
        m_bMilliliters = FALSE;
        m_bGrams = FALSE;
        m_bOunces = TRUE;
        }
      else
        { // Grams
        m_bMilliliters = FALSE;
        m_bGrams = TRUE;
        m_bOunces = FALSE;
        }
      }
    }
  else
    { // Water amount not entered yet, fill in the default units.
    //if((iDefaultUnits = AfxGetApp()->GetProfileInt(INI_SEC_GENERAL,
    //INI_ITEM_WATERLOAD_UNITS, OUNCES)) == OUNCES)
    if((iDefaultUnits = g_pConfig->iGet_item(CFG_WATERLOADUNITS)) == OUNCES)
      { // Ounces
      m_bMilliliters = FALSE;
      m_bOunces = TRUE;
      m_bGrams = FALSE;
      GetDlgItem(IDC_OUNCES)->GetWindowText(m_cstrUnits);
      }
    else if(iDefaultUnits == MILLILITERS)
      { // milliliters
      m_bMilliliters = TRUE;
      m_bOunces = FALSE;
      m_bGrams = FALSE;
      GetDlgItem(IDC_MILLILITERS)->GetWindowText(m_cstrUnits);
      }
    else if(iDefaultUnits == GRAMS)
      { // grams
      m_bMilliliters = FALSE;
      m_bOunces = FALSE;
      m_bGrams = TRUE;
      GetDlgItem(IDC_GRAMS)->GetWindowText(m_cstrUnits);
      }
    else
      { // Not a valid number, default to ounces.
      m_bMilliliters = FALSE;
      m_bOunces = TRUE;
      m_bGrams = FALSE;
      }
    }

  m_cstrDescription = m_pDoc->m_pdPatient.cstrStimMedDesc;

  Invalidate();
  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }


/********************************************************************
OnOK

  Message handler for the "Start Study" button.
    Write the selected units to the INI file so they will be the default
      units the next time.
********************************************************************/
void CStimulationMedium::OnBnClickedOk()
  {
  int iUnits;

  UpdateData(TRUE);

  if(m_uType == SM_EDIT)
    { // Editing only, not viewing.
      // Update the INI file with the selected water load units.
    if(m_bOunces == TRUE)
      iUnits = OUNCES; // ounces
    else if(m_bMilliliters == TRUE)
      iUnits = MILLILITERS; //  milliliters
    else
      iUnits = GRAMS; // grams
    //AfxGetApp()->WriteProfileInt(INI_SEC_GENERAL, INI_ITEM_WATERLOAD_UNITS, iUnits);
    g_pConfig->vWrite_item(CFG_WATERLOADUNITS, iUnits);
    }

  OnOK();
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.

  If this is a study, the cancel button will end the study.  So ask
    the user for confirmation to end the study.
********************************************************************/
void CStimulationMedium::OnBnClickedCancel()
  {
  CString cstrMsg, cstrCaption;

  if(m_uType == SM_NEW_STUDY)
    {
    //cstrMsg.LoadString(IDS_END_STUDY_ARE_YOU_SURE);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_END_STUDY_SURE);
    cstrCaption = g_pLang->cstrLoad_string(ITEM_G_WARNING);
    //cstrCaption.LoadString(IDS_WARNING);
    if(MessageBox(cstrMsg, cstrCaption, MB_YESNO) == IDYES)
      OnCancel(); // User wants to end the study.
    }
  else
    OnCancel();
  }

/********************************************************************
OnBnClickedWaterloadHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CStimulationMedium::OnBnClickedWaterloadHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_WATERLOAD_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_WATERLOAD_HELP);
  }

/********************************************************************
OnBnClickedOunces

  Message handler for the Ounces check box.
********************************************************************/
void CStimulationMedium::OnBnClickedOunces()
  {
	
  UpdateData(TRUE);
	m_bMilliliters = FALSE; // Make sure milliliters check box is unchecked.
  m_bGrams = FALSE; // Make sure grams check box is unchecked.
  GetDlgItem(IDC_OUNCES)->GetWindowText(m_cstrUnits);
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedMilliliters

  Message handler for the Milliliters check box.
********************************************************************/
void CStimulationMedium::OnBnClickedMilliliters()
  {
	
  UpdateData(TRUE);
	m_bOunces = FALSE; // Make sure ounces check box is unchecked.
  m_bGrams = FALSE; // Make sure grams check box is unchecked.
  GetDlgItem(IDC_MILLILITERS)->GetWindowText(m_cstrUnits);
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedGrams

  Message handler for the Grams check box.
********************************************************************/
void CStimulationMedium::OnBnClickedGrams()
  {
	
  UpdateData(TRUE);
	m_bOunces = FALSE; // Make sure ounces check box is unchecked.
	m_bMilliliters = FALSE; // Make sure milliliters check box is unchecked.
  GetDlgItem(IDC_GRAMS)->GetWindowText(m_cstrUnits);
  UpdateData(FALSE);
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
vGet_stim_med_amount - Get the amount of stimulation medium, the units,
                       and the description.

  inputs: Pointer to a PATIENT_DATA structure that gets the stimulation
            medium, the units, and the description.

  return: None.
********************************************************************/
void CStimulationMedium::vGet_stim_med_amount(PATIENT_DATA *pdWater)
  {

  if(m_uType == SM_VIEW)
    { // Nothing changes in view mode, so just copy the patient data to
      // the input.
    pdWater->uWaterAmount = m_pDoc->m_pdPatient.uWaterAmount;
    pdWater->cstrWaterUnits = m_pDoc->m_pdPatient.cstrWaterUnits;
    pdWater->cstrStimMedDesc = m_pDoc->m_pdPatient.cstrStimMedDesc;
    pdWater->uWaterUnits = m_pDoc->m_pdPatient.uWaterUnits;
    }
  else
    {
    pdWater->uWaterAmount = (unsigned short)m_uQuantity;
    pdWater->cstrWaterUnits = m_cstrUnits;
    pdWater->cstrStimMedDesc = m_cstrDescription;
  
		if(m_bMilliliters == TRUE)
      pdWater->uWaterUnits = MILLILITERS;
		else if(m_bOunces == TRUE)
      pdWater->uWaterUnits = OUNCES;
    else
      pdWater->uWaterUnits = GRAMS;
    }
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CStimulationMedium::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_STIMULATION_MEDIUM);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_SM_STIM_MEDIUM));

    GetDlgItem(IDC_BEGIN_STIM_MEDIUM)->SetWindowText(g_pLang->cstrGet_text(ITEM_SM_INGEST_STIM_MEDIUM));
    GetDlgItem(IDC_STATIC_DESCRIPTION)->SetWindowText(g_pLang->cstrGet_text(ITEM_SM_DESCRIPTION));
    GetDlgItem(IDC_STATIC_QUANTITY)->SetWindowText(g_pLang->cstrGet_text(ITEM_SM_QUANTITY));
    GetDlgItem(IDC_OUNCES)->SetWindowText(g_pLang->cstrGet_text(ITEM_SM_OZ));
    GetDlgItem(IDC_MILLILITERS)->SetWindowText(g_pLang->cstrGet_text(ITEM_SM_ML));
    GetDlgItem(IDC_GRAMS)->SetWindowText(g_pLang->cstrGet_text(ITEM_SM_GRAMS));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrGet_text(ITEM_SM_START_STUDY));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_WATERLOAD_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    }
  }
