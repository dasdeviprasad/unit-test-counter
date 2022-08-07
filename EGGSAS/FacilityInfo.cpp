/********************************************************************
FacilityInfo.cpp

Copyright (C) 2003,2004,2009,2012,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CFacilityInfo class.

  This class provides for entering the Facility/practice information.

  Logo requirements:
    BMP, JPG, PNG files.
    Max size is 200 pixels wide, 200 pixels high
    If image size is less than this, the image will be displayed as is.
    If image size is greater than this, it will be shrunk to fit.

HISTORY:
08-NOV-03  RET  New.
16-MAR-04  RET  Add help button and message handler.
14-JAN-09  RET  Version CDDRdr.2.08c
                  Change OnInitDialog():
                    - To disable all fields.
                    - Hide the "Save and Close" button.
                    - Change the text for the "Cancel" button to "Close".
                    - Center the "Close" and "Help" buttons.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
22-AUG-12  RET
             Add checkboxes to each field.  If checkbox is checked, the item
             is included in the report.
             Add feature to select a logo.
               Change methods: OnInitDialog(), OnCancel(), OnOK(),
                 bChanged(), vSet_text()
               Add event handlers: OnBnClickedChkFacLogo(), OnBnClickedChkFacPhonenum(),
                 OnBnClickedChkFacZipcode(), OnBnClickedChkFacState(),
                 OnBnClickedChkFacCity(), OnBnClickedChkFacAddr2(),
                 OnBnClickedChkFacAddr1(), OnBnClickedChkFacName()
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedFacilityinfoHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "Dbaccess.h"
#include "FacilityInfo.h"
#include ".\facilityinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFacilityInfo dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the currently active document.
          Pointer to parent window (default is NULL).
********************************************************************/
CFacilityInfo::CFacilityInfo(CEGGSASDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CFacilityInfo::IDD, pParent)
  , m_bChkName(FALSE)
  , m_bChkAddr1(FALSE)
  , m_bChkAddr2(FALSE)
  , m_bChkCity(FALSE)
  , m_bChkState(FALSE)
  , m_bChkZipCode(FALSE)
  , m_bChkPhoneNum(FALSE)
  , m_bChkLogo(FALSE)
  , m_cstrLogo(_T(""))
{
	//{{AFX_DATA_INIT(CFacilityInfo)
	m_cstrFacAddr1 = _T("");
	m_cstrFacAddr2 = _T("");
	m_cstrFacCity = _T("");
	m_cstrFacName = _T("");
	m_cstrFacPhone = _T("");
	m_cstrFacState = _T("");
	m_cstrFacZipcode = _T("");
	//}}AFX_DATA_INIT

  m_pDoc = pDoc;
  m_lFacilityID = INV_LONG;
  }


void CFacilityInfo::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CFacilityInfo)
  DDX_Text(pDX, IDC_FACILITY_ADDRESS1, m_cstrFacAddr1);
  DDX_Text(pDX, IDC_FACILITY_ADDRESS2, m_cstrFacAddr2);
  DDX_Text(pDX, IDC_FACILITY_CITY, m_cstrFacCity);
  DDX_Text(pDX, IDC_FACILITY_NAME, m_cstrFacName);
  DDX_Text(pDX, IDC_FACILITY_PHONE, m_cstrFacPhone);
  DDX_Text(pDX, IDC_FACILITY_STATE, m_cstrFacState);
  DDX_Text(pDX, IDC_FACILITY_ZIPCODE, m_cstrFacZipcode);
  //}}AFX_DATA_MAP
  DDX_Check(pDX, IDC_CHK_FAC_NAME, m_bChkName);
  DDX_Check(pDX, IDC_CHK_FAC_ADDR1, m_bChkAddr1);
  DDX_Check(pDX, IDC_CHK_FAC_ADDR2, m_bChkAddr2);
  DDX_Check(pDX, IDC_CHK_FAC_CITY, m_bChkCity);
  DDX_Check(pDX, IDC_CHK_FAC_STATE, m_bChkState);
  DDX_Check(pDX, IDC_CHK_FAC_ZIPCODE, m_bChkZipCode);
  DDX_Check(pDX, IDC_CHK_FAC_PHONENUM, m_bChkPhoneNum);
  DDX_Check(pDX, IDC_CHK_FAC_LOGO, m_bChkLogo);
  DDX_Text(pDX, IDC_EDIT_FAC_LOGO, m_cstrLogo);
}


BEGIN_MESSAGE_MAP(CFacilityInfo, CDialog)
	//{{AFX_MSG_MAP(CFacilityInfo)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_FACILITYINFO_HELP, OnBnClickedFacilityinfoHelp)
  ON_BN_CLICKED(IDC_CHK_FAC_NAME, OnBnClickedChkFacName)
  ON_BN_CLICKED(IDC_CHK_FAC_ADDR1, OnBnClickedChkFacAddr1)
  ON_BN_CLICKED(IDC_CHK_FAC_ADDR2, OnBnClickedChkFacAddr2)
  ON_BN_CLICKED(IDC_CHK_FAC_CITY, OnBnClickedChkFacCity)
  ON_BN_CLICKED(IDC_CHK_FAC_STATE, OnBnClickedChkFacState)
  ON_BN_CLICKED(IDC_CHK_FAC_ZIPCODE, OnBnClickedChkFacZipcode)
  ON_BN_CLICKED(IDC_CHK_FAC_PHONENUM, OnBnClickedChkFacPhonenum)
  ON_BN_CLICKED(IDC_CHK_FAC_LOGO, OnBnClickedChkFacLogo)
  ON_BN_CLICKED(IDC_BTN_BROWSE_LOGO, OnBnClickedBtnBrowseLogo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFacilityInfo message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Get the facility information from the database.
********************************************************************/
BOOL CFacilityInfo::OnInitDialog() 
  {
  FACILITY_INFO fiInfo;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();
    // Get the facility information from the database.
  fiInfo.lFacilityID = INV_LONG;
  if(m_pDoc->m_pDB != NULL)
    {
    if(m_pDoc->m_pDB->iGet_facility_info(&fiInfo) == SUCCESS)
      {
      m_lFacilityID = fiInfo.lFacilityID;
			m_cstrFacAddr1 = fiInfo.cstrAddr1;
			m_cstrFacAddr2 = fiInfo.cstrAddr2;
			m_cstrFacCity = fiInfo.cstrCity;
			m_cstrFacName = fiInfo.cstrName;
			m_cstrFacPhone = fiInfo.cstrPhone;
			m_cstrFacState = fiInfo.cstrState;
			m_cstrFacZipcode = fiInfo.cstrZipcode;
      }
    }
  m_cstrOrigFacAddr1 = m_cstrFacAddr1;
  m_cstrOrigFacAddr2 = m_cstrFacAddr2;
  m_cstrOrigFacCity = m_cstrFacCity;
  m_cstrOrigFacName = m_cstrFacName;
  m_cstrOrigFacPhone = m_cstrFacPhone;
  m_cstrOrigFacState = m_cstrFacState;
  m_cstrOrigFacZipcode = m_cstrFacZipcode;
  m_cstrOrigLogo = m_cstrLogo;

#if EGGSAS_READER == READER_DEPOT
  GetDlgItem(IDC_FACILITY_NAME)->EnableWindow(FALSE);
  GetDlgItem(IDC_FACILITY_ADDRESS1)->EnableWindow(FALSE);
  GetDlgItem(IDC_FACILITY_ADDRESS2)->EnableWindow(FALSE);
  GetDlgItem(IDC_FACILITY_CITY)->EnableWindow(FALSE);
  GetDlgItem(IDC_FACILITY_STATE)->EnableWindow(FALSE);
  GetDlgItem(IDC_FACILITY_ZIPCODE)->EnableWindow(FALSE);
  GetDlgItem(IDC_FACILITY_PHONE)->EnableWindow(FALSE);
  GetDlgItem(IDC_CHK_FAC_NAME)->EnableWindow(FALSE);
  GetDlgItem(IDC_CHK_FAC_ADDR1)->EnableWindow(FALSE);
  GetDlgItem(IDC_CHK_FAC_ADDR2)->EnableWindow(FALSE);
  GetDlgItem(IDC_CHK_FAC_CITY)->EnableWindow(FALSE);
  GetDlgItem(IDC_CHK_FAC_STATE)->EnableWindow(FALSE);
  GetDlgItem(IDC_CHK_FAC_ZIPCODE)->EnableWindow(FALSE);
  GetDlgItem(IDC_CHK_FAC_PHONENUM)->EnableWindow(FALSE);
  GetDlgItem(IDC_CHK_FAC_LOGO)->EnableWindow(FALSE);
    // Hide the "Save and Close" button.
  GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
    // Change the text for the "Cancel" button to "Close".
  GetDlgItem(IDCANCEL)->SetWindowText("Close");
    // Move the 2 buttons to center in the dialog box.
  WINDOWPLACEMENT wp1, wp2;
  RECT rRect;
  int iWidth, iWidth1, iWidth2, iSepDist;

  GetWindowRect(&rRect); // Dimensions of dialog box window.
     // Dimensions and position of "Close" button.
  GetDlgItem(IDCANCEL)->GetWindowPlacement(&wp1);
     // Dimensions and position of "Help" button.
  GetDlgItem(IDC_FACILITYINFO_HELP)->GetWindowPlacement(&wp2);
    // Calculate new position.
  iWidth = wp2.rcNormalPosition.right - wp1.rcNormalPosition.left;
  iWidth1 = wp1.rcNormalPosition.right - wp1.rcNormalPosition.left;
  iWidth2 = wp2.rcNormalPosition.right - wp2.rcNormalPosition.left;
  iSepDist = wp2.rcNormalPosition.left - wp1.rcNormalPosition.right;
    // Calculate and set new positin for "Close" button.
  wp1.rcNormalPosition.left = (rRect.right / 2) - (iWidth / 2);
  wp1.rcNormalPosition.right = wp1.rcNormalPosition.left + iWidth1;
  GetDlgItem(IDCANCEL)->SetWindowPlacement(&wp1);
    // Calculate and set new positin for "Help" button.
  wp2.rcNormalPosition.left = wp1.rcNormalPosition.right + iSepDist;
  wp2.rcNormalPosition.right = wp2.rcNormalPosition.left + iWidth2;
  GetDlgItem(IDC_FACILITYINFO_HELP)->SetWindowPlacement(&wp2);
#endif

    // Read in the logo from the INI file.
    m_cstrLogo = g_pConfig->cstrGet_item(CFG_LOGO);
    // Check the include in report checkboxes.
    // Save the original state of the checkboxes.
    g_pConfig->vCopy_rpt_item(g_pConfig->m_iIncludeRptItems, m_iIncludeRptItems);
    if(m_iIncludeRptItems[INC_RPT_FAC_NAME] == 1)
      m_bChkName = TRUE;
    else
      m_bChkName = FALSE;
    if(m_iIncludeRptItems[INC_RPT_FAC_ADDR1] == 1)
      m_bChkAddr1 = TRUE;
    else
      m_bChkAddr1 = FALSE;
    if(m_iIncludeRptItems[INC_RPT_FAC_ADDR2] == 1)
      m_bChkAddr2 = TRUE;
    else
      m_bChkAddr2 = FALSE;
    if(m_iIncludeRptItems[INC_RPT_FAC_CITY] == 1)
      m_bChkCity = TRUE;
    else
      m_bChkCity = FALSE;
    if(m_iIncludeRptItems[INC_RPT_FAC_STATE] == 1)
      m_bChkState = TRUE;
    else
      m_bChkState = FALSE;
    if(m_iIncludeRptItems[INC_RPT_FAC_ZIP_CODE] == 1)
      m_bChkZipCode = TRUE;
    else
      m_bChkZipCode = FALSE;
    if(m_iIncludeRptItems[INC_RPT_FAC_PHONE_NUM] == 1)
      m_bChkPhoneNum = TRUE;
    else
      m_bChkPhoneNum = FALSE;
    if(m_iIncludeRptItems[INC_RPT_FAC_LOGO] == 1)
      m_bChkLogo = TRUE;
    else
      m_bChkLogo = FALSE;

  UpdateData(FALSE);
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnOK

  Message handler for the Save and close button button.
    Save the facility information to the database.
********************************************************************/
void CFacilityInfo::OnOK() 
  {
  FACILITY_INFO fiInfo;

  UpdateData(TRUE);
  if(m_pDoc->m_pDB != NULL)
    {
    fiInfo.lFacilityID = m_lFacilityID;
		fiInfo.cstrAddr1 = m_cstrFacAddr1;
		fiInfo.cstrAddr2 = m_cstrFacAddr2;
		fiInfo.cstrCity = m_cstrFacCity;
		fiInfo.cstrName = m_cstrFacName;
		fiInfo.cstrPhone = m_cstrFacPhone;
		fiInfo.cstrState = m_cstrFacState;
		fiInfo.cstrZipcode = m_cstrFacZipcode;
    m_pDoc->m_pDB->iSave_facility_info(&fiInfo);
    }
    // Save the items to be included/excluded in the report.
  g_pConfig->vWrite_inc_rpt_item();
  // Save the logo.
  g_pConfig->vWrite_item(CFG_LOGO, m_cstrLogo);
	CDialog::OnOK();
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button button.
********************************************************************/
void CFacilityInfo::OnCancel() 
  {
	
    // Restore the items to be included/excluded in the report.
  g_pConfig->vCopy_rpt_item(m_iIncludeRptItems, g_pConfig->m_iIncludeRptItems);
  CDialog::OnCancel();
  }

/********************************************************************
OnBnClickedFacilityinfoHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CFacilityInfo::OnBnClickedFacilityinfoHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_FACILITYINFO_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_FACILITYINFO_HELP);
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
bChanged - Determines if any of the facility information has changed.

  inputs: None.

  return: true if the facility information has changed.
          false if there is no change in the facility information.
********************************************************************/
bool CFacilityInfo::bChanged()
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
    if((m_cstrOrigFacAddr1 != m_cstrFacAddr1
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_ADDR1) == true)
    || (m_cstrOrigFacAddr2 != m_cstrFacAddr2
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_ADDR2) == true)
    || (m_cstrOrigFacCity != m_cstrFacCity
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_CITY) == true)
    || (m_cstrOrigFacName != m_cstrFacName
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_NAME) == true)
    || (m_cstrOrigFacPhone != m_cstrFacPhone
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_PHONE_NUM) == true)
    || (m_cstrOrigFacState != m_cstrFacState
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_STATE) == true)
    || (m_cstrOrigFacZipcode != m_cstrFacZipcode
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_ZIP_CODE) == true)
    || (m_cstrOrigLogo != m_cstrLogo
    && g_pConfig->bGet_inc_rpt_item(INC_RPT_FAC_LOGO) == true)
    )
      bRet = true;
    }
  return(bRet);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CFacilityInfo::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_FACILITY_INFO);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_FI_FACILITY_INFO));

    GetDlgItem(IDC_STATIC_PRACTICE_NAME)->SetWindowText(g_pLang->cstrGet_text(ITEM_FI_FACILITY_PRACTICE));
    GetDlgItem(IDC_STATIC_ADDRESS)->SetWindowText(g_pLang->cstrGet_text(ITEM_FI_ADDRESS));
    GetDlgItem(IDC_STATIC_ADDRESS1)->SetWindowText(g_pLang->cstrGet_text(ITEM_FI_ADDRESS));
    GetDlgItem(IDC_STATIC_CITY)->SetWindowText(g_pLang->cstrGet_text(ITEM_FI_CITY));
    GetDlgItem(IDC_STATIC_STATE)->SetWindowText(g_pLang->cstrGet_text(ITEM_FI_STATE));
    GetDlgItem(IDC_STATIC_ZIPCODE)->SetWindowText(g_pLang->cstrGet_text(ITEM_FI_ZIPCODE));
    GetDlgItem(IDC_STATIC_PHONE)->SetWindowText(g_pLang->cstrGet_text(ITEM_FI_PHONE_NUM));
    GetDlgItem(IDC_STATIC_LOGO_REQ)->SetWindowText(g_pLang->cstrGet_text(ITEM_FI_LOGO_REQ));
    GetDlgItem(IDC_STATIC_LOGO_REQ2)->SetWindowText(g_pLang->cstrGet_text(ITEM_FI_LOGO_REQ2));
    GetDlgItem(IDC_STATIC_LOGO_REQ3)->SetWindowText(g_pLang->cstrGet_text(ITEM_FI_LOGO_REQ3));
    GetDlgItem(IDC_STATIC_LOGO_REQ4)->SetWindowText(g_pLang->cstrGet_text(ITEM_FI_LOGO_REQ4));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_SAVE_CLOSE));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_FACILITYINFO_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    GetDlgItem(IDC_STATIC_FAC_INC_IN_RPT_DESC)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_INC_IN_RPT_DESC));
    }
  }

/********************************************************************
OnBnClickedChkFacName - For the include in report checkbox for
                        the name field field.

  inputs: None.

  return: None.
********************************************************************/
  void CFacilityInfo::OnBnClickedChkFacName()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_FAC_NAME, m_bChkName);
  }

/********************************************************************
OnBnClickedChkFacAddr1 - For the include in report checkbox for
                         the first address field field.

  inputs: None.

  return: None.
********************************************************************/
  void CFacilityInfo::OnBnClickedChkFacAddr1()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_FAC_ADDR1, m_bChkAddr1);
  }

/********************************************************************
OnBnClickedChkFacAddr2 - For the include in report checkbox for
                         the second address field field.

  inputs: None.

  return: None.
********************************************************************/
  void CFacilityInfo::OnBnClickedChkFacAddr2()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_FAC_ADDR2, m_bChkAddr2);
  }

/********************************************************************
OnBnClickedChkFacCity - For the include in report checkbox for
                        the city field field.

  inputs: None.

  return: None.
********************************************************************/
  void CFacilityInfo::OnBnClickedChkFacCity()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_FAC_CITY, m_bChkCity);
  }

/********************************************************************
OnBnClickedChkFacState - For the include in report checkbox for
                         the state field field.

  inputs: None.

  return: None.
********************************************************************/
  void CFacilityInfo::OnBnClickedChkFacState()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_FAC_STATE, m_bChkState);
  }

/********************************************************************
OnBnClickedChkFacZipcode - For the include in report checkbox for
                           the zip code field field.

  inputs: None.

  return: None.
********************************************************************/
  void CFacilityInfo::OnBnClickedChkFacZipcode()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_FAC_ZIP_CODE, m_bChkZipCode);
  }

/********************************************************************
OnBnClickedChkFacPhonenum - For the include in report checkbox for
                            the phone number field field.

  inputs: None.

  return: None.
********************************************************************/
  void CFacilityInfo::OnBnClickedChkFacPhonenum()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_FAC_PHONE_NUM, m_bChkPhoneNum);
  }

/********************************************************************
OnBnClickedChkFacLogo - For the include in report checkbox for
                        the logo field field.

  inputs: None.

  return: None.
********************************************************************/
  void CFacilityInfo::OnBnClickedChkFacLogo()
  {
    // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_RPT_FAC_LOGO, m_bChkLogo);
  }

  void CFacilityInfo::OnBnClickedBtnBrowseLogo()
  {
  CFileDialog *pdlg;

  pdlg = new CFileDialog(TRUE, _T(".bmp"));
  //pdlg->m_pOFN->lpstrInitialDir = _T("c:\\3cpm");
  if(pdlg->DoModal() == IDOK)
  {
    m_cstrLogo = pdlg->GetPathName();
    UpdateData(FALSE);
  }
  delete pdlg;
  }
