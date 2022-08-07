/********************************************************************
WaterLoad.cpp

Copyright (C) 2003,2004,2009,2012,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CWaterLoad class.

  This class provides for entering the amount of water ingested.

HISTORY:
03-OCT-03  RET  New.
16-MAR-04  RET  Add help button and message handler.
16-JAN-09  RET  Version CDDRdr.2.08e
                  Display only if view mode.  Change Constructor.
                  Change vGet_water_amount() to return the original data
                    from the CDoc class in View mode.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
24-AUG-12  RET
             Add method bChanged() so caller can determine if something
               changed (to redraw the report).
                 Add variables: m_uOrigWaterAmount, m_bOrigMilliliters,
                   m_bOrigOunces
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedWaterloadHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "WaterLoad.h"
#include "util.h"
#include ".\waterload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaterLoad dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the active document that started this dialog box.
          Indicates why this object is being used (new study, edit, view).
          Indicates what the title line should be.
          Pointer to parent window (default is NULL).
********************************************************************/
CWaterLoad::CWaterLoad(CEGGSASDoc *pDoc, unsigned uFrom, unsigned uType, CWnd* pParent /*=NULL*/)
	: CDialog(CWaterLoad::IDD, pParent)
  {

	//{{AFX_DATA_INIT(CWaterLoad)
	m_uWaterAmt = 0;
	m_bMilliliters = FALSE;
	m_bOunces = FALSE;
	//}}AFX_DATA_INIT

  m_pDoc = pDoc;
  m_uType = uType;
#if EGGSAS_READER == READER_DEPOT
  m_uFrom = SM_VIEW;
  m_uType = TYPE_AMT_OF_WATER;
#else
  m_uFrom = uFrom;
  m_uType = uType;
#endif
  }


void CWaterLoad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaterLoad)
	DDX_Text(pDX, IDC_WATER_AMOUNT, m_uWaterAmt);
	DDX_Check(pDX, IDC_MILLILITERS, m_bMilliliters);
	DDX_Check(pDX, IDC_OUNCES, m_bOunces);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWaterLoad, CDialog)
	//{{AFX_MSG_MAP(CWaterLoad)
	ON_BN_CLICKED(IDC_MILLILITERS, OnMilliliters)
	ON_BN_CLICKED(IDC_OUNCES, OnOunces)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_WATERLOAD_HELP, OnBnClickedWaterloadHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaterLoad message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Set the title line
    Modify the fields for editing/viewing.
********************************************************************/
BOOL CWaterLoad::OnInitDialog() 
  {
  CString cstrTemp;
  CFont cfBoldFont;
  int iDefaultUnits;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();
    // Modify the dialog box for new study/edit/view
    // Change the title line.
  if(m_uType == TYPE_WATER_AMOUNT)
    { // Only asking for the amount of water ingested, Edit.
    //cstrTemp.LoadString(IDS_ENTER_WATER_AMOUNT);
    cstrTemp = g_pLang->cstrLoad_string(ITEM_G_ENTER_WATER_AMOUNT);
    GetDlgItem(IDC_BEGIN_WATER)->SetWindowText((LPCTSTR)cstrTemp);
    //cstrTemp.LoadString(IDS_OK);
    cstrTemp = g_pLang->cstrLoad_string(ITEM_G_OK);
    GetDlgItem(IDOK)->SetWindowText((LPCTSTR)cstrTemp);
    }
  else if(m_uType == TYPE_AMT_OF_WATER)
    { // Displaying the amount of water ingested, View.
    //cstrTemp.LoadString(IDS_WATER_INGESTED);
    cstrTemp = g_pLang->cstrLoad_string(ITEM_G_WATER_INGESTED);
    GetDlgItem(IDC_BEGIN_WATER)->SetWindowText((LPCTSTR)cstrTemp);
    //cstrTemp.LoadString(IDS_OK);
    cstrTemp = g_pLang->cstrLoad_string(ITEM_G_OK);
    GetDlgItem(IDOK)->SetWindowText((LPCTSTR)cstrTemp);
    }
    // Enable/disable fields and change field text.
  if(m_uFrom == SM_EDIT || m_uFrom == SM_VIEW)
    {
    if(m_uFrom == SM_VIEW)
      {
		  GetDlgItem(IDC_WATER_AMOUNT)->EnableWindow(FALSE);
		  GetDlgItem(IDC_OUNCES)->EnableWindow(FALSE);
		  GetDlgItem(IDC_MILLILITERS)->EnableWindow(FALSE);
      GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
        // Center the OK button.
      vCenter_button_horiz(this, GetDlgItem(IDOK));
      //cstrTemp.LoadString(IDS_OK);
      cstrTemp = g_pLang->cstrLoad_string(ITEM_G_OK);
      }
    else
      cstrTemp = g_pLang->cstrLoad_string(ITEM_G_SAVE_CLOSE);
      //cstrTemp.LoadString(IDS_SAVE_CLOSE); // Change OK button text.
    GetDlgItem(IDOK)->SetWindowText((LPCTSTR)cstrTemp);
    }

    // Get the font to use in dialog box fields.
  vSet_font(this, NULL, &cfBoldFont, FW_BOLD);
    // Set the font in the dialog box fields.
  GetDlgItem(IDC_BEGIN_WATER)->SetFont(&cfBoldFont);
	
    // Fill in the dialog box fields.
  if(m_pDoc->m_pdPatient.uWaterAmount != 0)
    {
    m_uWaterAmt = m_pDoc->m_pdPatient.uWaterAmount;
    m_uOrigWaterAmount = m_uWaterAmt;
    m_cstrWaterUnits = m_pDoc->m_pdPatient.cstrWaterUnits;
    GetDlgItem(IDC_MILLILITERS)->GetWindowText(cstrTemp);
    if(cstrTemp == m_cstrWaterUnits)
      { // milliliters
      m_bMilliliters = TRUE;
      m_bOunces = FALSE;
      }
    else
      { // Ounces
      m_bMilliliters = FALSE;
      m_bOunces = TRUE;
      }
    }
  else
    { // Water amount not entered yet, fill in the default units.
    //if((iDefaultUnits = AfxGetApp()->GetProfileInt(INI_SEC_GENERAL,
    //INI_ITEM_WATERLOAD_UNITS, 0)) == 0)
    if((iDefaultUnits = g_pConfig->iGet_item(CFG_WATERLOADUNITS)) == 0)
      { // Ounces
      m_bMilliliters = FALSE;
      m_bOunces = TRUE;
      }
    else if(iDefaultUnits == 1)
      { // milliliters
      m_bMilliliters = TRUE;
      m_bOunces = FALSE;
      }
    else
      { // Not a valid number, default to ounces.
      m_bMilliliters = FALSE;
      m_bOunces = TRUE;
      }
    }
  m_bOrigMilliliters = m_bMilliliters;
  m_bOrigOunces = m_bOunces;

  Invalidate();
  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnOK

  Message handler for the OK button.
    Write the selected units to the INI file so they will be the default
      units the next time.
********************************************************************/
void CWaterLoad::OnOK() 
  {
  int iUnits;

  UpdateData(TRUE);

    // Update the INI file with the selected water load units.
  if(m_bOunces == TRUE)
    iUnits = 0; // ounces
  else
    iUnits = 1; //  milliliters
  //AfxGetApp()->WriteProfileInt(INI_SEC_GENERAL, INI_ITEM_WATERLOAD_UNITS, iUnits);
  g_pConfig->vWrite_item(CFG_WATERLOADUNITS, iUnits);

	CDialog::OnOK();
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.

  If this is a study, the cancel button will end the study.  So ask
    the user for confirmation to end the study.
********************************************************************/
void CWaterLoad::OnCancel() 
  {
  CString cstrMsg, cstrCaption;

  if(m_uFrom == SM_NEW_STUDY && m_uType != TYPE_WATER_AMOUNT)
    {
    //cstrMsg.LoadString(IDS_END_STUDY_ARE_YOU_SURE);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_END_STUDY_SURE);
    //cstrCaption.LoadString(IDS_WARNING);
    cstrCaption = g_pLang->cstrLoad_string(ITEM_G_WARNING);
    if(MessageBox(cstrMsg, cstrCaption, MB_YESNO) == IDYES)
      CDialog::OnCancel(); // User wants to end the study.
    }
  else
    CDialog::OnCancel();
  }

/********************************************************************
OnMilliliters

  Message handler for the Milliliters check box.
********************************************************************/
void CWaterLoad::OnMilliliters() 
  {
	
  UpdateData(TRUE);
	m_bOunces = FALSE; // Make sure ounces check box is unchecked.
  GetDlgItem(IDC_MILLILITERS)->GetWindowText(m_cstrWaterUnits);
  UpdateData(FALSE);
  }

/********************************************************************
OnOunces

  Message handler for the Ounces check box.
********************************************************************/
void CWaterLoad::OnOunces() 
  {
	
  UpdateData(TRUE);
	m_bMilliliters = FALSE; // Make sure milliliters check box is unchecked.
  GetDlgItem(IDC_OUNCES)->GetWindowText(m_cstrWaterUnits);
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedWaterloadHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CWaterLoad::OnBnClickedWaterloadHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_WATERLOAD_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_WATERLOAD_HELP);
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
vGet_water_amount - Get the amount of water ingested and the units.

  inputs: Pointer to a PATIENT_DATA structure that gets the amount of
            water ingested and the units.

  return: None.
********************************************************************/
void CWaterLoad::vGet_water_amount(PATIENT_DATA *pdWater)
  {

  if(m_uFrom == SM_VIEW)
    { // Nothing changes in view mode, so just copy the patient data to
      // the input.
    pdWater->uWaterAmount = m_pDoc->m_pdPatient.uWaterAmount;
    pdWater->cstrWaterUnits = m_pDoc->m_pdPatient.cstrWaterUnits;
    pdWater->uWaterUnits = m_pDoc->m_pdPatient.uWaterUnits;
    }
  else
    {
    pdWater->uWaterAmount = (unsigned short)m_uWaterAmt;
    pdWater->cstrWaterUnits = m_cstrWaterUnits;
  
		if(m_bMilliliters == TRUE)
      pdWater->uWaterUnits = MILLILITERS;
		else // if(m_bOunces == TRUE)
      pdWater->uWaterUnits = OUNCES;
    }
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CWaterLoad::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_STIMULATION_MEDIUM);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_SM_WATERLOAD));

    GetDlgItem(IDC_BEGIN_WATER)->SetWindowText(g_pLang->cstrGet_text(ITEM_SM_AMTWATER));
    GetDlgItem(IDC_STATIC_AMT_WATER)->SetWindowText(g_pLang->cstrGet_text(ITEM_SM_INGESTWATER));
    GetDlgItem(IDC_OUNCES)->SetWindowText(g_pLang->cstrGet_text(ITEM_SM_OZ));
    GetDlgItem(IDC_MILLILITERS)->SetWindowText(g_pLang->cstrGet_text(ITEM_SM_ML));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrGet_text(ITEM_SM_START_STUDY));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_WATERLOAD_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    }
  }

/********************************************************************
bChanged - Determines if any of the facility information has changed.

  inputs: None.

  return: true if the facility information has changed.
          false if there is no change in the facility information.
********************************************************************/
bool CWaterLoad::bChanged()
  {
  bool bRet;

  if(m_uOrigWaterAmount != m_uWaterAmt
  || m_bOrigMilliliters != m_bMilliliters
  || m_bOrigOunces != m_bOunces)
    bRet = true;
  else
    bRet = false;
  return(bRet);
  }

