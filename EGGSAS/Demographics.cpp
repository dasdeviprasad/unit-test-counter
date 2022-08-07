/********************************************************************
Demographics.cpp

Copyright (C) 2003,2004,2005,2008,2009,2012,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CDemographics class.

  This class provides a data entry screen for entering patient
  demographics.


HISTORY:
03-OCT-03  RET  New.
16-MAR-04  RET  Add help button and message handler.
06-FEB-05  RET  Changes to use the research reference number.  If the
                  reference number is used, the names are blanked.
                  Changed methods:
                    OnResrchRefNumEna(), vSet_demographics(), bChanged(),
                    vGet_demographics()
10-FEB-05  RET  Add middle initial to dialog box.  Change the following
                  methods:
                    OnInitDialog(), bChanged(), vSet_demographics(),
                    vGet_demographics(), OnOK(), OnResrchRefNumEna()
28-AUG-08  RET  Version 2.05c
                    Change OnOK() to also check for duplicate research
                      reference number.
                    Change OnInitDialog() to disable the name fields
                      and enable the research reference number field
                      if the research reference number is being used.
                    Change OnResrchRefNumEna() to enable/disable the
                      name and research reference number fields depending
                      on if research reference number is being used.
14-OCT-08  RET  Version 2.07a
                    Add 2 checkboxes, one for human study, one for dog study.
                      The checkboxes are hidden if research reference numbers
                        are not being used or if the study type is not
                        research.
                      Add message handlers for the 2 checkboxes.
19-OCT-08  RET  Version 2.07b
                  Change message handler for research reference checkbox,
                    OnResrchRefNumEna(), and OnInitDialog() to hide the
                    dog/human checkboxes if dog studies not allowed.
14-JAN-09  RET  Version CDDRdr.2.08c
                  Display only if view mode.  Change Constructor.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
22-AUG-12  RET
             Add checkboxes to each field.  If checkbox is checked, the item
             is included in the report.
               Change methods: OnInitDialog(), OnCancel(), OnResrchRefNumEna(),
                 OnOK(), vSet_text(), bChanged(), vSet_demographics()
               Add event handlers: OnBnClickedChkIncrptRsrchrefnum(),
                 OnBnClickedChkIncrptHospnum(), OnBnClickedChkIncrptPhonenum(),
                 OnBnClickedChkIncrptZipcode(), OnBnClickedChkIncrptState(),
                 OnBnClickedChkIncrptCity(), OnBnClickedChkIncrptAddr2(),
                 OnBnClickedChkIncrptAddr1(), OnBnClickedChkIncrptSsn(),
                 OnBnClickedChkIncrptDateofbirth(), OnBnClickedChkIncrptLastname(),
                 OnBnClickedChkIncrptMi(), OnBnClickedChkIncrptFirstname()
25-FEB-13  RET
            Add checkboxes for Male and Female and a checkbox to include in the report.
              Add event handlers: OnBnClickedChkMale(), OnBnClickedChkFemale(),
                OnBnClickedChkIncrptMaleFemale()
              Add class variables: m_bMale, m_bFemale, m_bIncMaleFemale
              Change methods: OnInitDialog(), vGet_demographics(), vSet_demographics(),
                vSet_text()
            Change "Social Security Number" to "Patient ID or Social Security Number"
            Add checkbox to hide the patient.
              Change methods: OnInitDialog(), vGet_demographics(), vSet_demographics(),
                vSet_text()
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedDemographicsHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "dbaccess.h"
#include "TenMinView.h"
#include "Demographics.h"
#include "util.h"
#include ".\demographics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDemographics dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Where the diaglog box is being called from (edit, view, etc).
          Pointer to the currently active document.
********************************************************************/
CDemographics::CDemographics(unsigned uFrom, CEGGSASDoc *pDoc, CWnd *pParent /*=NULL*/)
	: CDialog(CDemographics::IDD, pParent)
  , m_cstrMiddleInitial(_T(""))
  , m_bHuman(FALSE)
  , m_bDog(FALSE)
  , m_bChkFirstName(FALSE)
  , m_bChkMI(FALSE)
  , m_bChkLastName(FALSE)
  , m_bChkDateOfBirth(FALSE)
  , m_bChkSSN(FALSE)
  , m_bChkAddr1(FALSE)
  , m_bChkAddr2(FALSE)
  , m_bChkCity(FALSE)
  , m_bChkState(FALSE)
  , m_bChkZipCode(FALSE)
  , m_bChkPhoneNum(FALSE)
  , m_bChkHospNum(FALSE)
  , m_bChkRsrchRefNum(FALSE)
  , m_bMale(FALSE)
  , m_bFemale(FALSE)
  , m_bIncMaleFemale(FALSE)
  , m_bHide(FALSE)
  {
	//{{AFX_DATA_INIT(CDemographics)
	m_cstrAddress1 = _T("");
	m_cstrAddress2 = _T("");
	m_cstrCity = _T("");
	m_cstrDateOfBirth = _T("");
	m_cstrHospitalNum = _T("");
	m_cstrPhone = _T("");
	m_cstrResearchRefNum = _T("");
	m_cstrSocialSecurityNum = _T("");
	m_cstrState = _T("");
	m_cstrZipCode = _T("");
	m_bResearchRefEnable = FALSE;
	m_cstrFirstName = _T("");
	m_cstrLastName = _T("");
	//}}AFX_DATA_INIT

  m_pDoc = pDoc;
#if EGGSAS_READER == READER_DEPOT
  if(uFrom == SM_VIEW)
    m_bShowHiddenPatientCheckbox = false;
  else
    m_bShowHiddenPatientCheckbox = true;
  m_uFrom = SM_VIEW;
#else
  m_uFrom = uFrom;
#endif
  m_uExitSts = DLG_CANCEL;
  }


void CDemographics::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
//{{AFX_DATA_MAP(CDemographics)
DDX_Text(pDX, IDC_ADDRESS1, m_cstrAddress1);
DDX_Text(pDX, IDC_ADDRESS2, m_cstrAddress2);
DDX_Text(pDX, IDC_CITY, m_cstrCity);
DDX_Text(pDX, IDC_DATE_OF_BIRTH, m_cstrDateOfBirth);
DDX_Text(pDX, IDC_HOSPITAL_NUMBER, m_cstrHospitalNum);
DDX_Text(pDX, IDC_PHONE_NUMBER, m_cstrPhone);
DDX_Text(pDX, IDC_RESRCH_REF_NUM, m_cstrResearchRefNum);
DDX_Text(pDX, IDC_SOCIAL_SECURITY_NUM, m_cstrSocialSecurityNum);
DDX_Text(pDX, IDC_STATE, m_cstrState);
DDX_Text(pDX, IDC_ZIPCODE, m_cstrZipCode);
DDX_Check(pDX, IDC_RESRCH_REF_NUM_ENA, m_bResearchRefEnable);
DDX_Text(pDX, IDC_PATIENT_FIRST_NAME, m_cstrFirstName);
DDX_Text(pDX, IDC_PATIENT_LAST_NAME, m_cstrLastName);
//}}AFX_DATA_MAP
DDX_Text(pDX, IDC_MIDDLE_INITIAL, m_cstrMiddleInitial);
DDV_MaxChars(pDX, m_cstrMiddleInitial, 1);
DDX_Check(pDX, IDC_CHK_HUMAN, m_bHuman);
DDX_Check(pDX, IDC_CHK_DOG, m_bDog);
DDX_Check(pDX, IDC_CHK_INCRPT_FIRSTNAME, m_bChkFirstName);
DDX_Check(pDX, IDC_CHK_INCRPT_MI, m_bChkMI);
DDX_Check(pDX, IDC_CHK_INCRPT_LASTNAME, m_bChkLastName);
DDX_Check(pDX, IDC_CHK_INCRPT_DATEOFBIRTH, m_bChkDateOfBirth);
DDX_Check(pDX, IDC_CHK_INCRPT_SSN, m_bChkSSN);
DDX_Check(pDX, IDC_CHK_INCRPT_ADDR1, m_bChkAddr1);
DDX_Check(pDX, IDC_CHK_INCRPT_ADDR2, m_bChkAddr2);
DDX_Check(pDX, IDC_CHK_INCRPT_CITY, m_bChkCity);
DDX_Check(pDX, IDC_CHK_INCRPT_STATE, m_bChkState);
DDX_Check(pDX, IDC_CHK_INCRPT_ZIPCODE, m_bChkZipCode);
DDX_Check(pDX, IDC_CHK_INCRPT_PHONENUM, m_bChkPhoneNum);
DDX_Check(pDX, IDC_CHK_INCRPT_HOSPNUM, m_bChkHospNum);
DDX_Check(pDX, IDC_CHK_INCRPT_RSRCHREFNUM, m_bChkRsrchRefNum);
DDX_Check(pDX, IDC_CHK_MALE, m_bMale);
DDX_Check(pDX, IDC_CHK_FEMALE, m_bFemale);
DDX_Check(pDX, IDC_CHK_INCRPT_MALE_FEMALE, m_bIncMaleFemale);
DDX_Check(pDX, IDC_CHK_HIDE, m_bHide);
}


BEGIN_MESSAGE_MAP(CDemographics, CDialog)
	//{{AFX_MSG_MAP(CDemographics)
	ON_BN_CLICKED(IDC_RESRCH_REF_NUM_ENA, OnResrchRefNumEna)
	ON_BN_CLICKED(IDC_SAVE_AND_CLOSE, OnSaveAndClose)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_DEMOGRAPHICS_HELP, OnBnClickedDemographicsHelp)
  ON_BN_CLICKED(IDC_CHK_HUMAN, OnBnClickedChkHuman)
  ON_BN_CLICKED(IDC_CHK_DOG, OnBnClickedChkDog)
  ON_BN_CLICKED(IDC_CHK_INCRPT_FIRSTNAME, OnBnClickedChkIncrptFirstname)
  ON_BN_CLICKED(IDC_CHK_INCRPT_MI, OnBnClickedChkIncrptMi)
  ON_BN_CLICKED(IDC_CHK_INCRPT_LASTNAME, OnBnClickedChkIncrptLastname)
  ON_BN_CLICKED(IDC_CHK_INCRPT_DATEOFBIRTH, OnBnClickedChkIncrptDateofbirth)
  ON_BN_CLICKED(IDC_CHK_INCRPT_SSN, OnBnClickedChkIncrptSsn)
  ON_BN_CLICKED(IDC_CHK_INCRPT_ADDR1, OnBnClickedChkIncrptAddr1)
  ON_BN_CLICKED(IDC_CHK_INCRPT_ADDR2, OnBnClickedChkIncrptAddr2)
  ON_BN_CLICKED(IDC_CHK_INCRPT_CITY, OnBnClickedChkIncrptCity)
  ON_BN_CLICKED(IDC_CHK_INCRPT_STATE, OnBnClickedChkIncrptState)
  ON_BN_CLICKED(IDC_CHK_INCRPT_ZIPCODE, OnBnClickedChkIncrptZipcode)
  ON_BN_CLICKED(IDC_CHK_INCRPT_PHONENUM, OnBnClickedChkIncrptPhonenum)
  ON_BN_CLICKED(IDC_CHK_INCRPT_HOSPNUM, OnBnClickedChkIncrptHospnum)
  ON_BN_CLICKED(IDC_CHK_INCRPT_RSRCHREFNUM, OnBnClickedChkIncrptRsrchrefnum)
  ON_BN_CLICKED(IDC_CHK_MALE, OnBnClickedChkMale)
  ON_BN_CLICKED(IDC_CHK_FEMALE, OnBnClickedChkFemale)
  ON_BN_CLICKED(IDC_CHK_INCRPT_MALE_FEMALE, OnBnClickedChkIncrptMaleFemale)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDemographics message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Modify the fields for editing/viewing.
********************************************************************/
BOOL CDemographics::OnInitDialog() 
  {
  CString cstrText;
  CFont cfBoldFont;  // Object to hold the bold font being used for dialog box.

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();
  if(m_uFrom == SM_EDIT || m_uFrom == SM_VIEW)
    {
    if(m_uFrom == SM_VIEW || g_bHiddenPatients == TRUE)
      { // In View mode, nothing is changeable.
		  GetDlgItem(IDC_PATIENT_FIRST_NAME)->EnableWindow(FALSE);
		  GetDlgItem(IDC_PATIENT_LAST_NAME)->EnableWindow(FALSE);
		  GetDlgItem(IDC_MIDDLE_INITIAL)->EnableWindow(FALSE);
      GetDlgItem(IDC_ADDRESS1)->EnableWindow(FALSE);
		  GetDlgItem(IDC_ADDRESS2)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CITY)->EnableWindow(FALSE);
		  GetDlgItem(IDC_DATE_OF_BIRTH)->EnableWindow(FALSE);
		  GetDlgItem(IDC_HOSPITAL_NUMBER)->EnableWindow(FALSE);
		  GetDlgItem(IDC_PHONE_NUMBER)->EnableWindow(FALSE);
		  GetDlgItem(IDC_RESRCH_REF_NUM)->EnableWindow(FALSE);
		  GetDlgItem(IDC_SOCIAL_SECURITY_NUM)->EnableWindow(FALSE);
		  GetDlgItem(IDC_STATE)->EnableWindow(FALSE);
		  GetDlgItem(IDC_ZIPCODE)->EnableWindow(FALSE);
      GetDlgItem(IDC_RESRCH_REF_NUM_ENA)->EnableWindow(FALSE);
      //cstrText.LoadString(IDS_OK);
      cstrText = g_pLang->cstrLoad_string(ITEM_G_OK);
      GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
      // Disable the report checkboxes.
		  GetDlgItem(IDC_CHK_INCRPT_FIRSTNAME)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHK_INCRPT_MI)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHK_INCRPT_LASTNAME)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHK_INCRPT_DATEOFBIRTH)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHK_INCRPT_SSN)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHK_INCRPT_ADDR1)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHK_INCRPT_ADDR2)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHK_INCRPT_CITY)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHK_INCRPT_STATE)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHK_INCRPT_ZIPCODE)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHK_INCRPT_PHONENUM)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHK_INCRPT_HOSPNUM)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHK_INCRPT_RSRCHREFNUM)->EnableWindow(FALSE);
      GetDlgItem(IDC_CHK_MALE)->EnableWindow(FALSE);
      GetDlgItem(IDC_CHK_FEMALE)->EnableWindow(FALSE);
      GetDlgItem(IDC_CHK_INCRPT_MALE_FEMALE)->EnableWindow(FALSE);
      // Disable if viewing active patients, leave enabled if viewing hidden patients.
      if(m_uFrom == SM_VIEW)
        {
#if EGGSAS_READER == READER_DEPOT
        if(m_bShowHiddenPatientCheckbox == true)
          GetDlgItem(IDC_CHK_HIDE)->EnableWindow(TRUE);
        else
          GetDlgItem(IDC_CHK_HIDE)->EnableWindow(FALSE);
#else
        GetDlgItem(IDC_CHK_HIDE)->EnableWindow(FALSE);
#endif
        }
      else
        GetDlgItem(IDC_CHK_HIDE)->EnableWindow(TRUE);
     }
    else
      { // Edit mode.
        // Enable/disable the research reference number edit window.
      GetDlgItem(IDC_RESRCH_REF_NUM)->EnableWindow(m_bResearchRefEnable);
      }
    //cstrText.LoadString(IDS_OK);
    cstrText = g_pLang->cstrLoad_string(ITEM_G_OK);
    GetDlgItem(IDOK)->SetWindowText((LPCTSTR)cstrText);
      // Hide the save button.
    GetDlgItem(IDC_SAVE_AND_CLOSE)->ShowWindow(SW_HIDE);
    }
  else
    { // New study
      // Get the font to use in dialog box fields.
    vSet_font(this, NULL, &cfBoldFont, FW_BOLD);
      // Set the font in the dialog box fields.
    GetDlgItem(IDOK)->SetFont(&cfBoldFont);
    GetDlgItem(IDC_SAVE_AND_CLOSE)->SetFont(&cfBoldFont);
    GetDlgItem(IDCANCEL)->SetFont(&cfBoldFont);
    GetDlgItem(IDC_CHK_HIDE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CHK_HIDE)->EnableWindow(FALSE);
    }
  if(m_pDoc->m_pdPatient.bUseResrchRefNum == TRUE)
    { // Using research reference numbers.  Gray out the name fields.
		GetDlgItem(IDC_PATIENT_FIRST_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_PATIENT_LAST_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_MIDDLE_INITIAL)->EnableWindow(FALSE);
      // Enable the research reference number fields if not viewing.
    if(m_uFrom != SM_VIEW)
      GetDlgItem(IDC_RESRCH_REF_NUM)->EnableWindow(TRUE);

    if(theApp.m_bAllowDogStudies == true)
      { // Dog studies allowed.
      if(theApp.m_ptStudyType == PT_RESEARCH)
        {
        GetDlgItem(IDC_CHK_HUMAN)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_CHK_DOG)->ShowWindow(SW_SHOW);
        if(m_pDoc->m_pdPatient.uPatientType == PAT_HUMAN)
          {
          m_bHuman = TRUE;
          m_bDog = FALSE;
          }
        else
          {
          m_bHuman = FALSE;
          m_bDog = TRUE;
          }
        if(m_uFrom == SM_EDIT || m_uFrom == SM_VIEW)
          { // Edit or View an existing study.  Can't change patient type.
          GetDlgItem(IDC_CHK_HUMAN)->EnableWindow(FALSE);
          GetDlgItem(IDC_CHK_DOG)->EnableWindow(FALSE);
          }
        }
      else
        { // Standard waterload study.
        GetDlgItem(IDC_CHK_HUMAN)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_CHK_DOG)->ShowWindow(SW_HIDE);
        m_bHuman = TRUE;
        }
      }
    else
      { // Dog studies not allowed.
      GetDlgItem(IDC_CHK_HUMAN)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_CHK_DOG)->ShowWindow(SW_HIDE);
      m_bHuman = TRUE;
      }
    }
  else
    {
    GetDlgItem(IDC_CHK_HUMAN)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CHK_DOG)->ShowWindow(SW_HIDE);
    m_bHuman = TRUE;
    }
	
    // Check the include in report checkboxes.
    // Save the original state of the checkboxes.
    g_pConfig->vCopy_rpt_item(g_pConfig->m_iIncludeRptItems, m_iIncludeRptItems);
    if(m_iIncludeRptItems[INC_RPT_PATIENT_FIRST_NAME] == 1)
      m_bChkFirstName = TRUE;
    else
      m_bChkFirstName = FALSE;
    if(m_iIncludeRptItems[INC_RPT_PATIENT_MI] == 1)
      m_bChkMI = TRUE;
    else
      m_bChkMI = FALSE;
    if(m_iIncludeRptItems[INC_RPT_PATIENT_LAST_NAME] == 1)
      m_bChkLastName = TRUE;
    else
      m_bChkLastName = FALSE;
    if(m_iIncludeRptItems[INC_RPT_DATE_OF_BIRTH] == 1)
      m_bChkDateOfBirth = TRUE;
    else
      m_bChkDateOfBirth = FALSE;
    if(m_iIncludeRptItems[INC_RPT_SSN] == 1)
      m_bChkSSN = TRUE;
    else
      m_bChkSSN = FALSE;
    if(m_iIncludeRptItems[INC_RPT_ADDR1] == 1)
      m_bChkAddr1 = TRUE;
    else
      m_bChkAddr1 = FALSE;
    if(m_iIncludeRptItems[INC_RPT_ADDR2] == 1)
      m_bChkAddr2 = TRUE;
    else
      m_bChkAddr2 = FALSE;
    if(m_iIncludeRptItems[INC_RPT_CITY] == 1)
      m_bChkCity = TRUE;
    else
      m_bChkCity = FALSE;
    if(m_iIncludeRptItems[INC_RPT_STATE] == 1)
      m_bChkState = TRUE;
    else
      m_bChkState = FALSE;
    if(m_iIncludeRptItems[INC_RPT_ZIP_CODE] == 1)
      m_bChkZipCode = TRUE;
    else
      m_bChkZipCode = FALSE;
    if(m_iIncludeRptItems[INC_RPT_PHONE_NUM] == 1)
      m_bChkPhoneNum = TRUE;
    else
      m_bChkPhoneNum = FALSE;
    if(m_iIncludeRptItems[INC_RPT_HOSP_NUM] == 1)
      m_bChkHospNum = TRUE;
    else
      m_bChkHospNum = FALSE;
    if(m_iIncludeRptItems[INC_RPT_RSRCH_REF_NUM] == 1)
     m_bChkRsrchRefNum  = TRUE;
    else
      m_bChkRsrchRefNum = FALSE;
    if(m_iIncludeRptItems[INC_RPT_MALE_FEMALE] == 1)
     m_bIncMaleFemale  = TRUE;
    else
      m_bIncMaleFemale = FALSE;

  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnOK

  Message handler for the Continue button.
    There must be at least a first and last name.
    If there is a date of birth, make sure it looks OK.
********************************************************************/
void CDemographics::OnOK() 
  {
  CString cstrTitle, cstrMsg;
  FIND_PATIENT_INFO fpiData;
  bool bOkToClose = true;
  unsigned uFocusField;

  UpdateData(TRUE);
  if(m_bResearchRefEnable == TRUE)
    { // Must have a reference number.
    if(m_cstrResearchRefNum.IsEmpty() == TRUE)
      { // No reference number.
      bOkToClose = false;
      //cstrMsg.LoadString(IDS_NEED_RSCH_REF_NUM);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_NEED_RSCH_REF_NUM);
      uFocusField = IDC_RESRCH_REF_NUM;
      }
    }
  else
    {
    if(m_cstrFirstName.IsEmpty() == TRUE)
      { // No first name.
      bOkToClose = false;
      //cstrMsg.LoadString(IDS_NEED_FIRST_NAME);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_NEED_FIRST_NAME);
      uFocusField = IDC_PATIENT_FIRST_NAME;
      }
    else if(m_cstrLastName.IsEmpty() == TRUE)
      { // No first name.
      bOkToClose = false;
      //cstrMsg.LoadString(IDS_NEED_LAST_NAME);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_NEED_LAST_NAME);
      uFocusField = IDC_PATIENT_LAST_NAME;
      }
      // Make sure the date of birth is valid.
    else if(m_cstrDateOfBirth.IsEmpty() == FALSE)
      {
      if(iValidate_date(m_cstrDateOfBirth) != SUCCESS)
        { // Date of birth doen't look good.
        bOkToClose = false;
        //cstrMsg.LoadString(IDS_INVALID_DATE);
        cstrMsg = g_pLang->cstrLoad_string(ITEM_G_INV_DATE);
        uFocusField = IDC_DATE_OF_BIRTH;
        }
      }
    }

  if(bOkToClose == false)
    { // ERROR, display the error message.
    //cstrTitle.LoadString(IDS_ERROR1);
    cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    MessageBox(cstrMsg, cstrTitle);
    GetDlgItem(uFocusField)->SetFocus();
    }
  else // if(m_bResearchRefEnable == FALSE)
    { // Look the patient up in the database and if user specified new patient and
      // the patient exists (either name exists or research reference number exists),
      // question the user.
    if(m_uFrom == SM_NEW_STUDY_NEW_PATIENT)
      {
      if(m_pDoc->m_pDB != NULL)
        {
        fpiData.cstrFirstName = m_cstrFirstName;
        fpiData.cstrLastName = m_cstrLastName;
        fpiData.cstrMI = m_cstrMiddleInitial;
        fpiData.bUseResrchRefNum = m_bResearchRefEnable;
        fpiData.cstrResearchRefNum = m_cstrResearchRefNum;
        fpiData.lPatientID = INV_LONG;
        if(m_pDoc->m_pDB->iFind_patient_name(&fpiData) == SUCCESS)
          { // Found a supposedly new patient in the database. Ask if user wants to continue.
          //cstrMsg.LoadString(IDS_PATIENT_IN_DB_CONTINUE);
          cstrMsg = g_pLang->cstrLoad_string(ITEM_G_PATIENT_IN_DB_CONT);
          //cstrTitle.LoadString(IDS_WARNING);
          cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
          if(MessageBox(cstrMsg, cstrTitle, MB_YESNO) == IDNO)
            bOkToClose = false;
          else
            m_pDoc->m_pdPatient.lPatientID = fpiData.lPatientID;
          }
        }
      }
    }
    // Save the items to be included/excluded in the report.
  g_pConfig->vWrite_inc_rpt_item();
  if(bOkToClose == true)
    {
    if(m_uExitSts == DLG_CANCEL)
      m_uExitSts = DLG_OK;
	  CDialog::OnOK();
    }
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.
********************************************************************/
void CDemographics::OnCancel() 
  {
  CString cstrMsg, cstrCaption;
  bool bEndDialog = true;

  UpdateData(TRUE);
  if(m_uFrom <= SM_NEW_STUDY)
    { // New study, ask if user wants to end the study if at least a first
      // name and last name have been entered.
    if(m_cstrLastName.IsEmpty() == FALSE && m_cstrFirstName.IsEmpty() == FALSE)
      {
      //cstrMsg.LoadString(IDS_END_STUDY_ARE_YOU_SURE);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_END_STUDY_SURE);
      //cstrCaption.LoadString(IDS_WARNING);
      cstrCaption = g_pLang->cstrLoad_string(ITEM_G_WARNING);
      if(MessageBox(cstrMsg, cstrCaption, MB_YESNO) == IDNO)
        bEndDialog = false;
      }
    }
    // Restore the items to be included/excluded in the report.
  g_pConfig->vCopy_rpt_item(m_iIncludeRptItems, g_pConfig->m_iIncludeRptItems);
  if(bEndDialog == true)
    CDialog::OnCancel();
  }

/********************************************************************
OnResrchRefNumEna

  Message handler for the research reference number check box.
********************************************************************/
void CDemographics::OnResrchRefNumEna() 
  {
	
  UpdateData(TRUE);
    // Enable the research reference number edit window.
  GetDlgItem(IDC_RESRCH_REF_NUM)->EnableWindow(m_bResearchRefEnable);
  GetDlgItem(IDC_CHK_INCRPT_RSRCHREFNUM)->EnableWindow(m_bResearchRefEnable);
  if(m_bResearchRefEnable == TRUE)
    { // User changed to using the research reference number.
    if(m_cstrLastName.IsEmpty() == FALSE)
      m_cstrSaveLastName = m_cstrLastName;
    m_cstrLastName.Empty();
    m_cstrOrigLastName.Empty();
    if(m_cstrFirstName.IsEmpty() == FALSE)
      m_cstrSaveFirstName = m_cstrFirstName;
    m_cstrFirstName.Empty();
    m_cstrOrigFirstName.Empty();
    if(m_cstrMiddleInitial.IsEmpty() == FALSE)
      m_cstrSaveMiddleInitial = m_cstrMiddleInitial;
    m_cstrMiddleInitial.Empty();
    m_cstrOrigMiddleInitial.Empty();
      // Disable name fields.
		GetDlgItem(IDC_PATIENT_FIRST_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_PATIENT_LAST_NAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_MIDDLE_INITIAL)->EnableWindow(FALSE);
      // Enable the research reference number fields.
    GetDlgItem(IDC_RESRCH_REF_NUM)->EnableWindow(TRUE);
    GetDlgItem(IDC_RESRCH_REF_NUM)->SetFocus();
    if(theApp.m_bAllowDogStudies == true)
      { // Dog studies allowed.
      if(theApp.m_ptStudyType == PT_RESEARCH)
        {
          // Show Patient type fields.
        GetDlgItem(IDC_CHK_HUMAN)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_CHK_DOG)->ShowWindow(SW_SHOW);
          // Make sure appropriate field is checked.
        if(m_pDoc->m_pdPatient.uPatientType == PAT_HUMAN)
          {
          m_bHuman = TRUE;
          m_bDog = FALSE;
          }
        else
          {
          m_bHuman = FALSE;
          m_bDog = TRUE;
          }
        }
      else
        {
        GetDlgItem(IDC_CHK_HUMAN)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_CHK_DOG)->ShowWindow(SW_HIDE);
        m_bHuman = TRUE;
        }
      }
    else
      { // Dog studies NOT allowed.
      GetDlgItem(IDC_CHK_HUMAN)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_CHK_DOG)->ShowWindow(SW_HIDE);
      m_bHuman = TRUE;
      }
    }
  else
    { // User changed to NOT using the research reference number.
      // Enable name fields.
		GetDlgItem(IDC_PATIENT_FIRST_NAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_PATIENT_LAST_NAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_MIDDLE_INITIAL)->EnableWindow(TRUE);
      // Hide Patient type fields.
    GetDlgItem(IDC_CHK_HUMAN)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CHK_DOG)->ShowWindow(SW_HIDE);
      // Disable the research reference number fields.
    GetDlgItem(IDC_RESRCH_REF_NUM)->EnableWindow(FALSE);
    m_cstrLastName = m_cstrSaveLastName;
    m_cstrOrigLastName = m_cstrSaveLastName;
    m_cstrFirstName = m_cstrSaveFirstName;
    m_cstrOrigFirstName = m_cstrSaveFirstName;
    m_cstrMiddleInitial = m_cstrSaveMiddleInitial;
    m_cstrOrigMiddleInitial = m_cstrSaveMiddleInitial;
    }
  UpdateData(FALSE);
  }

/********************************************************************
OnSaveAndClose

  Message handler for the Save and Close button.
    - Close the dialog box with an OK return.
********************************************************************/
void CDemographics::OnSaveAndClose() 
  {
	
  m_uExitSts = DLG_CLOSE_SAVE;
  OnOK();
  }

/********************************************************************
OnBnClickedDemographicsHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CDemographics::OnBnClickedDemographicsHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_DEMOGRAPHICS_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_DEMOGRAPHICS_HELP);
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
vGet_demographics - Get the demographic information.

  inputs: Pointer to a PATIENT_DATA structure that gets the demographics.

  return: None.
********************************************************************/
void CDemographics::vGet_demographics(PATIENT_DATA *pdDemographics)
  {

  if(m_cstrLastName.IsEmpty() == TRUE)
    m_cstrLastName = m_cstrSaveLastName;
  if(m_cstrFirstName.IsEmpty() == TRUE)
    m_cstrFirstName = m_cstrSaveFirstName;
  if(m_cstrMiddleInitial.IsEmpty() == TRUE)
    m_cstrMiddleInitial = m_cstrSaveMiddleInitial;
  pdDemographics->cstrLastName = m_cstrLastName;
  pdDemographics->cstrFirstName = m_cstrFirstName;
  pdDemographics->cstrMI = m_cstrMiddleInitial;
  pdDemographics->cstrAddress1 = m_cstrAddress1;
  pdDemographics->cstrAddress2 = m_cstrAddress2;
  pdDemographics->cstrCity = m_cstrCity;
  pdDemographics->cstrDateOfBirth = m_cstrDateOfBirth;
  pdDemographics->cstrHospitalNum = m_cstrHospitalNum;
  pdDemographics->cstrPhone = m_cstrPhone;
  pdDemographics->bUseResrchRefNum = m_bResearchRefEnable;
  pdDemographics->cstrResearchRefNum = m_cstrResearchRefNum;
  pdDemographics->cstrSocialSecurityNum = m_cstrSocialSecurityNum;
  pdDemographics->cstrState = m_cstrState;
  pdDemographics->cstrZipCode = m_cstrZipCode;
  pdDemographics->bMale = m_bMale;
  pdDemographics->bFemale = m_bFemale;
  pdDemographics->bHide = m_bHide;
  }

/********************************************************************
vSet_demographics - Set the demographic information to be displayed in
                    the dialog box fields.

                    Used by the caller to tell this dialog box what the
                    demographic information is.

  inputs: Pointer to a PATIENT_DATA structure that contains the demographics.

  return: None.
********************************************************************/
void CDemographics::vSet_demographics(PATIENT_DATA *pdDemographics)
  {

  m_cstrLastName = pdDemographics->cstrLastName;
  m_cstrOrigLastName = pdDemographics->cstrLastName;
  m_cstrSaveLastName = pdDemographics->cstrLastName;
  m_cstrFirstName = pdDemographics->cstrFirstName;
  m_cstrOrigFirstName = pdDemographics->cstrFirstName;
  m_cstrSaveFirstName = pdDemographics->cstrFirstName;
  m_cstrMiddleInitial = pdDemographics->cstrMI;
  m_cstrOrigMiddleInitial = pdDemographics->cstrMI;
  m_cstrSaveMiddleInitial = pdDemographics->cstrMI;
  m_cstrAddress1 = pdDemographics->cstrAddress1;
  m_cstrOrigAddress1 = m_cstrAddress1;
  m_cstrAddress2 = pdDemographics->cstrAddress2;
  m_cstrOrigAddress2 = m_cstrAddress2;
  m_cstrCity = pdDemographics->cstrCity;
  m_cstrOrigCity = m_cstrCity;
  m_cstrDateOfBirth = pdDemographics->cstrDateOfBirth;
  m_cstrOrigDateOfBirth = m_cstrDateOfBirth;
  m_cstrHospitalNum = pdDemographics->cstrHospitalNum;
  m_cstrOrigHospitalNum = m_cstrHospitalNum;
  m_cstrPhone = pdDemographics->cstrPhone;
  m_cstrOrigPhone = m_cstrPhone;
  m_bResearchRefEnable = pdDemographics->bUseResrchRefNum;
  m_cstrResearchRefNum = pdDemographics->cstrResearchRefNum;
  m_cstrOrigResearchRefNum = pdDemographics->cstrResearchRefNum;
  m_cstrSocialSecurityNum = pdDemographics->cstrSocialSecurityNum;
  m_cstrOrigSocialSecurityNum = m_cstrSocialSecurityNum;
  m_cstrState = pdDemographics->cstrState;
  m_cstrOrigState = m_cstrState;
  m_cstrZipCode = pdDemographics->cstrZipCode;
  m_cstrOrigZipCode = m_cstrZipCode;
  m_bMale = pdDemographics->bMale;
  m_bOrigMale = m_bMale;
  m_bFemale = pdDemographics->bFemale;
  m_bOrigFemale = m_bFemale;
  m_bHide = pdDemographics->bHide;
  m_bOrigHide = m_bHide;

  if(m_bResearchRefEnable == TRUE)
    {
    m_cstrLastName.Empty();
    m_cstrOrigLastName.Empty();
    m_cstrFirstName.Empty();
    m_cstrOrigFirstName.Empty();
    m_cstrMiddleInitial.Empty();
    m_cstrOrigMiddleInitial.Empty();
    }
  }

/********************************************************************
uGet_exit_sts - Get the exit status.
                  Cancel
                  OK
                  Close and save

  inputs: None.

  return: Status that the user exited with.
********************************************************************/
short unsigned CDemographics::uGet_exit_sts()
  {
  return(m_uExitSts);
  }

/********************************************************************
bChanged - Determines if any of the demographics has changed that affect
           the report.
             Demographics that affect the report:
               First name
               Last name
               Research Reference number

  inputs: None.

  return: true if the demographics affecting the report has changed.
          false if there is no change in the demographics affecting the report.
********************************************************************/
bool CDemographics::bChanged(void)
  {
  bool bRet;
  short int iIncludeRptItems[INC_RPT_LAST];

  bRet = false;
    // None of the fields changed.  Check to see if any of the items
    // have changed being included in the report.
  g_pConfig->vCopy_rpt_item(g_pConfig->m_iIncludeRptItems, iIncludeRptItems);
  for(int i = 0; i < INC_RPT_LAST; ++i)
    {
    if(m_iIncludeRptItems[i] != iIncludeRptItems[i])
      {
      bRet = true;
      break;
      }
    }
  if(bRet == false)
    {
    if((m_cstrLastName != m_cstrOrigLastName
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_PATIENT_LAST_NAME) == true)
    || (m_cstrFirstName != m_cstrOrigFirstName
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_PATIENT_FIRST_NAME) == true)
    || (m_cstrMiddleInitial != m_cstrOrigMiddleInitial
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_PATIENT_MI) == true)
    || (m_cstrResearchRefNum != m_cstrOrigResearchRefNum
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_RSRCH_REF_NUM) == true)
    || (m_cstrAddress1 != m_cstrOrigAddress1
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_ADDR1) == true)
    || (m_cstrAddress2 != m_cstrOrigAddress2
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_ADDR2) == true)
    || (m_cstrCity != m_cstrOrigCity
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_CITY) == true)
    || (m_cstrState != m_cstrOrigState
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_STATE) == true)
    || (m_cstrZipCode != m_cstrOrigZipCode
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_ZIP_CODE) == true)
    || (m_cstrDateOfBirth != m_cstrOrigDateOfBirth
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_DATE_OF_BIRTH) == true)
    || (m_cstrHospitalNum != m_cstrOrigHospitalNum
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_HOSP_NUM) == true)
    || (m_cstrPhone != m_cstrOrigPhone
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_PHONE_NUM) == true)
    || (m_cstrSocialSecurityNum != m_cstrOrigSocialSecurityNum
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_SSN) == true)
    || ((m_bMale != m_bOrigMale || m_bFemale != m_bOrigFemale)
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_MALE_FEMALE) == true)
    )
      bRet = true;
    }
  return(bRet);
  }


void CDemographics::OnBnClickedChkHuman()
  {

  UpdateData(TRUE);
  m_pDoc->m_pdPatient.uPatientType = PAT_HUMAN;
  m_cstrLastName.Empty();
  m_bDog = FALSE;
  UpdateData(FALSE);
  }

void CDemographics::OnBnClickedChkDog()
  {

  UpdateData(TRUE);
  m_pDoc->m_pdPatient.uPatientType = PAT_DOG;
  m_cstrLastName = PATIENT_TYPE_DOG;
  m_bHuman = FALSE;
  UpdateData(FALSE);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CDemographics::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_DEMOGRAPHICS);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_PATIENT_DEMOGRAPHICS));
    GetDlgItem(IDC_STATIC_PATIENT_NAME)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_PATIENT_NAME));
    GetDlgItem(IDC_STATIC_MIDDLE_INITIAL)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_MIDDLE_INITIAL));
    GetDlgItem(IDC_STATIC_LAST_NAME)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_LAST_NAME));
    GetDlgItem(IDC_STATIC_DATE_OF_BIRTH)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_DATE_OF_BIRTH));
    GetDlgItem(IDC_STATIC_DATE_FMT)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_DATEFMT));
    GetDlgItem(IDC_STATIC_SSN)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_SSN));
    GetDlgItem(IDC_STATIC_ADDRESS)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_ADDRESS));
    GetDlgItem(IDC_STATIC_ADDRESS_1)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_ADDRESS));
    GetDlgItem(IDC_STATIC_CITY)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_CITY));
    GetDlgItem(IDC_STATIC_STATE)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_STATE));
    GetDlgItem(IDC_STATIC_ZIP_CODE)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_ZIPCODE));
    GetDlgItem(IDC_STATIC_PHONE_NUM)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_PHONE_NUM));
    GetDlgItem(IDC_STATIC_HOSP_NUM)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_HOSPITAL_NUM));
    GetDlgItem(IDC_RESRCH_REF_NUM_ENA)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_RESEARCH_REF_NUM));
    GetDlgItem(IDC_CHK_HUMAN)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_HUMAN));
    GetDlgItem(IDC_CHK_DOG)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_DOG));
    GetDlgItem(IDC_CHK_MALE)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_MALE));
    GetDlgItem(IDC_CHK_FEMALE)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_FEMALE));
    GetDlgItem(IDC_CHK_HIDE)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_HIDE));

    GetDlgItem(IDC_SAVE_AND_CLOSE)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_SAVE_CLOSE));
    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrGet_text(ITEM_DG_CONTINUE));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_DEMOGRAPHICS_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    GetDlgItem(IDC_STATIC_INC_IN_RPT_DESC)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_INC_IN_RPT_DESC));
    //g_pLang->vEnd_section();
    }
  }

/********************************************************************
OnBnClickedChkIncrptFirstname - For the include in report checkbox for
                                the first name field.

  inputs: None.

  return: None.
********************************************************************/
  void CDemographics::OnBnClickedChkIncrptFirstname()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_PATIENT_FIRST_NAME, m_bChkFirstName);
  }

/********************************************************************
OnBnClickedChkIncrptMi - For the include in report checkbox for
                         the middle initial field.

  inputs: None.

  return: None.
********************************************************************/
  void CDemographics::OnBnClickedChkIncrptMi()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_PATIENT_MI, m_bChkMI);
  }

/********************************************************************
OnBnClickedChkIncrptLastname - For the include in report checkbox for
                               the last name field.

  inputs: None.

  return: None.
********************************************************************/
  void CDemographics::OnBnClickedChkIncrptLastname()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_PATIENT_LAST_NAME, m_bChkLastName);
  }

/********************************************************************
OnBnClickedChkIncrptDateofbirth - For the include in report checkbox for
                                  the date of birth field.

  inputs: None.

  return: None.
********************************************************************/
  void CDemographics::OnBnClickedChkIncrptDateofbirth()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_DATE_OF_BIRTH, m_bChkDateOfBirth);
  }

/********************************************************************
OnBnClickedChkIncrptSsn - For the include in report checkbox for
                          the social security number field.

  inputs: None.

  return: None.
********************************************************************/
  void CDemographics::OnBnClickedChkIncrptSsn()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_SSN, m_bChkSSN);
  }

/********************************************************************
OnBnClickedChkIncrptAddr1 - For the include in report checkbox for
                            the first address field field.

  inputs: None.

  return: None.
********************************************************************/
  void CDemographics::OnBnClickedChkIncrptAddr1()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_ADDR1, m_bChkAddr1);
  }

/********************************************************************
OnBnClickedChkIncrptAddr2 - For the include in report checkbox for
                            the second address field field.

  inputs: None.

  return: None.
********************************************************************/
  void CDemographics::OnBnClickedChkIncrptAddr2()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_ADDR2, m_bChkAddr2);
  }

/********************************************************************
OnBnClickedChkIncrptCity - For the include in report checkbox for
                           the city field field.

  inputs: None.

  return: None.
********************************************************************/
  void CDemographics::OnBnClickedChkIncrptCity()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_CITY, m_bChkCity);
  }

/********************************************************************
OnBnClickedChkIncrptState - For the include in report checkbox for
                            the state field field.

  inputs: None.

  return: None.
********************************************************************/
  void CDemographics::OnBnClickedChkIncrptState()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_STATE, m_bChkState);
  }

/********************************************************************
OnBnClickedChkIncrptZipcode - For the include in report checkbox for
                              the zip code field field.

  inputs: None.

  return: None.
********************************************************************/
  void CDemographics::OnBnClickedChkIncrptZipcode()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_ZIP_CODE, m_bChkZipCode);
  }

/********************************************************************
OnBnClickedChkIncrptPhonenum - For the include in report checkbox for
                               the phone number field field.

  inputs: None.

  return: None.
********************************************************************/
  void CDemographics::OnBnClickedChkIncrptPhonenum()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_PHONE_NUM, m_bChkPhoneNum);
  }

/********************************************************************
OnBnClickedChkIncrptHospnum - For the include in report checkbox for
                              the hospital number field field.

  inputs: None.

  return: None.
********************************************************************/
  void CDemographics::OnBnClickedChkIncrptHospnum()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_HOSP_NUM, m_bChkHospNum);
  }

/********************************************************************
OnBnClickedChkIncrptRsrchrefnum - For the include in report checkbox for
                                  the research reference number field field.

  inputs: None.

  return: None.
********************************************************************/
  void CDemographics::OnBnClickedChkIncrptRsrchrefnum()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_RSRCH_REF_NUM, m_bChkRsrchRefNum);
  }

void CDemographics::OnBnClickedChkMale()
  {

  UpdateData(TRUE);
  if(m_bMale == TRUE)
    m_bFemale = FALSE;
  UpdateData(FALSE);
  }

void CDemographics::OnBnClickedChkFemale()
  {

  UpdateData(TRUE);
  if(m_bFemale == TRUE)
    m_bMale = FALSE;
  UpdateData(FALSE);
  }

void CDemographics::OnBnClickedChkIncrptMaleFemale()
  {
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_MALE_FEMALE, m_bIncMaleFemale);
  }
