/********************************************************************
HideStudyDates.cpp

Copyright (C) 2016 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CHideStudyDates class.

  This class provides the following functionality: 
    - Showing and hiding study dates by entering a password.
    - Generating a new password.
    - Creating a new password with a reset password from 3CPM.


HISTORY:
06-JUN-16  RET  New.
28-JUL-16  RET
             When study dates are changed between hide and show, reload
             the study data after changing the study dates in the database
             and refresh the screens to show or hide the study date immediately.
               Change method: OnBnClickedOk()
********************************************************************/

#include "stdafx.h"
#include "MainFrm.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "dbaccess.h"
#include "HideStudyDates.h"
#include ".\hidestudydates.h"
#include "TenMinView.h"

extern CMainFrame *g_pFrame;

// CHideStudyDates dialog

IMPLEMENT_DYNAMIC(CHideStudyDates, CDialog)
/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the currently active document.
********************************************************************/
CHideStudyDates::CHideStudyDates(CEGGSASDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CHideStudyDates::IDD, pParent)
  , m_cstrPassword(_T(""))
  , m_cstrNewPassword(_T(""))
  , m_cstrNewPassword1(_T(""))
  , m_cstrHideShowState(_T(""))
  , m_bFirstPassword(false)
  {

  m_pDoc = pDoc;
}

/********************************************************************
Destructor

********************************************************************/
CHideStudyDates::~CHideStudyDates()
{
}

void CHideStudyDates::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_EDIT_PASSWORD, m_cstrPassword);
DDV_MaxChars(pDX, m_cstrPassword, 10);
DDX_Text(pDX, IDC_EDIT_NEW_PASSWORD, m_cstrNewPassword);
DDV_MaxChars(pDX, m_cstrNewPassword, 10);
DDX_Text(pDX, IDC_EDIT_VALIDATE_PASSWORD, m_cstrNewPassword1);
DDV_MaxChars(pDX, m_cstrNewPassword1, 10);
DDX_Text(pDX, IDC_EDIT_HIDE_SHOW_STATE, m_cstrHideShowState);
}


BEGIN_MESSAGE_MAP(CHideStudyDates, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDC_BUTTON_NEW_PASSWORD, OnBnClickedButtonNewPassword)
  ON_BN_CLICKED(IDC_BUTTON_RESET_PASSWORD, OnBnClickedButtonResetPassword)
  ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHideStudyDates message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Display text from the language file.
    Disable fields not used in entering a password.
    Move fields if this is the first time.
    Initialize class variables.
********************************************************************/
BOOL CHideStudyDates::OnInitDialog()
  {
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  m_bResetPassword = false;
  vSet_text();
// SW_HIDE
// SW_SHOW
  // Hide the reset password text box until we need it.
  GetDlgItem(IDC_EDIT_RESET_PASSWORD)->ShowWindow(SW_HIDE);

  if(g_pConfig->m_cstrHideStudyDatesPassword.IsEmpty())
    { // Initial screen, ask user for new password, the first password.
    m_cstrHideShowState.Format("%s\r\n%s", g_pLang->cstrGet_text(ITEM_INITIAL_INSTRUCTION),
      g_pLang->cstrGet_text(ITEM_INITIAL_INSTRUCTION1));
    //m_cstrHideShowState = "You must enter a password and click OK.\r\nThen the study dates will be hidden.";
    GetDlgItem(IDC_STATIC_ENTER_PASSWORD)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDIT_PASSWORD)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BUTTON_NEW_PASSWORD)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BUTTON_RESET_PASSWORD)->ShowWindow(SW_HIDE);
    GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
    vMove_new_password_fields();
    m_bNewPassword = true;
    g_pConfig->m_bHideStudyDates = true;
    m_bFirstPassword = true;
    }
  else
    {
    GetDlgItem(IDC_STATIC_ENTER_NEW_PASSWORD)->EnableWindow(FALSE); //ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDIT_NEW_PASSWORD)->EnableWindow(FALSE); //ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE); //ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC_VALIDATE_PASSWORD)->EnableWindow(FALSE); //ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDIT_VALIDATE_PASSWORD)->EnableWindow(FALSE); //ShowWindow(SW_HIDE);
    m_bFirstPassword = false;

    GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
    m_bNewPassword = false;
    if(g_pConfig->m_bHideStudyDates == true)
      m_cstrHideShowState.Format("%s\r\n%s", g_pLang->cstrGet_text(ITEM_CURRENTLY_HIDDEN),
      g_pLang->cstrGet_text(ITEM_CURRENTLY_HIDDEN1)); //"Study dates are currently hidden.\r\nEnter your password and click OK to show study dates.";
    else
      m_cstrHideShowState.Format("%s\r\n%s", g_pLang->cstrGet_text(ITEM_CURRENTLY_NOT_HIDDEN),
      g_pLang->cstrGet_text(ITEM_CURRENTLY_NOT_HIDDEN1)); //"Study dates are not currently hidden.\r\nEnter your password and click OK to hide study dates.";
    }

  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnBnClickedOk

  Message handler for the OK button.
    Validate the password.
********************************************************************/
void CHideStudyDates::OnBnClickedOk()
  {
  bool bOK = true;

  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  if(m_bNewPassword == false)
    { // The password already exists, compare what the user entered to the current password.
    if(m_cstrPassword.IsEmpty() == false)
      { // User entered a password.
      // Validate the password and if valid, flop the hide study dates flag.
      if(g_pConfig->m_cstrHideStudyDatesPassword.Compare(m_cstrPassword) != 0)
        {
        MessageBox(g_pLang->cstrGet_text(ITEM_PSWD_NOT_CORRECT), "Error", MB_ICONERROR);
        //MessageBox("The password is not correct.", "Error", MB_ICONERROR);
        GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
        bOK = false;
        }
      else
        {
        if(g_pConfig->m_bHideStudyDates == true)
          g_pConfig->m_bHideStudyDates = false;
        else
          g_pConfig->m_bHideStudyDates = true;
        g_pConfig->vWrite_study_date_info();
        }

      // Change all study dates in the database.
      if(m_pDoc->m_pDB != NULL)
        {
          m_pDoc->m_pDB->iEncode_study_dates(g_pConfig->m_bHideStudyDates);
          m_pDoc->m_pdPatient.cstrDate.Empty();
          m_pDoc->m_pDB->iGet_study_data(&m_pDoc->m_pdPatient);
          if(g_pFrame->m_uViewID == VIEWID_TEN_MIN)
            m_pDoc->m_p10MinView->vSet_patient_info(&m_pDoc->m_pdPatient);
          else
            g_pFrame->Invalidate();
        }
      //if(g_pConfig->m_cstrHideStudyDatesPassword.IsEmpty() == false)
      //  {
      //  g_pConfig->m_cstrHideStudyDatesPassword = m_cstrNewPassword;
      //  g_pConfig->vWrite_study_date_info();
      //  }
      }
    else
      { // User didn't enter a password.
        MessageBox(g_pLang->cstrGet_text(ITEM_ENTER_PSWD_CANCEL), "Error", MB_ICONERROR);
      //MessageBox("Please enter a password or select Cancel", "Error", MB_ICONERROR);
      GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
      bOK = false;
      }
    }
  else
    { // This is a new password, put it in the INI file.
    if(m_cstrNewPassword.IsEmpty() == true && g_pConfig->m_cstrHideStudyDatesPassword.IsEmpty() == true)
        {
        MessageBox(g_pLang->cstrGet_text(ITEM_MUST_ENTER_PSWD), "Error", MB_ICONERROR);
        //MessageBox("You must enter a password", "Error", MB_ICONERROR);
        GetDlgItem(IDC_EDIT_NEW_PASSWORD)->SetFocus();
        bOK = false;
        }
    else
      { // User entered a new password.
        // Compare the 2 password entries.
      if(m_cstrNewPassword.Compare(m_cstrNewPassword1) != 0)
        {
        MessageBox(g_pLang->cstrGet_text(ITEM_PSWD_ENTRIES_DONT_MATCH), "Error", MB_ICONERROR);
        //MessageBox("The 2 password entries don't match", "Error", MB_ICONERROR);
        GetDlgItem(IDC_EDIT_NEW_PASSWORD)->SetFocus();
        bOK = false;
        }
      else
        {
        g_pConfig->m_cstrHideStudyDatesPassword = m_cstrNewPassword;
        g_pConfig->vWrite_study_date_info();
        if(m_bFirstPassword == true)
          {// This is the first time the program is run, change all study dates in the database.
          if(m_pDoc->m_pDB != NULL)
            m_pDoc->m_pDB->iEncode_study_dates(g_pConfig->m_bHideStudyDates);
          }
       }
      }
    }

    //// Get password from INI file and compare to this one.
    //if(g_pConfig->m_cstrHideStudyDatesPassword.IsEmpty())
    //  {
    //  if(m_cstrNewPassword.IsEmpty() == false)
    //    {
    //    g_pConfig->m_cstrHideStudyDatesPassword = m_cstrNewPassword;
    //    g_pConfig->vWrite_study_date_info();
    //    }
    //  }
    //}

  if(bOK == true)
    OnOK();
  }

/********************************************************************
OnBnClickedButtonNewPassword

  Message handler for the Create New Password button.
    Validate the current password.
    Set up fields for entering a new password.
********************************************************************/
void CHideStudyDates::OnBnClickedButtonNewPassword()
  {
  bool bContinue;
  CString cstrTemp;

  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  // The user must first have had to enter the existing password.
  bContinue = false;
  if(m_cstrPassword.IsEmpty() == true)
    {
    cstrTemp.Format("%s\r\n%s", g_pLang->cstrGet_text(ITEM_PLEASE_ENTER_PSWD),
      g_pLang->cstrGet_text(ITEM_PLEASE_ENTER_PSWD1));
    MessageBox(cstrTemp, "Error", MB_ICONERROR);
    //MessageBox("Please enter your password.\nIf you have forgotten your password, contact 3CPM.",
    //"Error", MB_ICONERROR);
    GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
    }
  else
    { // User entered a password.
    // Validate the password and if valid, toggle the hide study dates flag.
    if(g_pConfig->m_cstrHideStudyDatesPassword.Compare(m_cstrPassword) != 0)
      {
      MessageBox(g_pLang->cstrGet_text(ITEM_PSWD_NOT_CORRECT), "Error", MB_ICONERROR);
      //MessageBox("The password is not correct.", "Error", MB_ICONERROR);
      GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
      }
    else
      bContinue = true;
    }

  if(bContinue == true)
    {
    m_bNewPassword = true;
    GetDlgItem(IDC_STATIC_ENTER_NEW_PASSWORD)->EnableWindow(TRUE); //ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDIT_NEW_PASSWORD)->EnableWindow(TRUE); //ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE); //ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC_VALIDATE_PASSWORD)->EnableWindow(TRUE); //ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDIT_VALIDATE_PASSWORD)->EnableWindow(TRUE); //ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATIC_ENTER_PASSWORD)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_NEW_PASSWORD)->SetFocus();

    m_cstrHideShowState = "";
    UpdateData(FALSE);
    }
  }

/********************************************************************
OnBnClickedButtonResetPassword

  Message handler for the Reset Password button.
    Validate the password received from 3CPM.
    Set up fields for entering a new password.
********************************************************************/
void CHideStudyDates::OnBnClickedButtonResetPassword()
  {
  //CWnd *pcwnd, *pcwnd1;
  //WINDOWPLACEMENT wp, wp1;
  //int iHeight, iWidth;

  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  if(m_cstrPassword.IsEmpty() == true)
    {
    MessageBox(g_pLang->cstrGet_text(ITEM_ENTER_3CPM_PSWD), "Error", MB_ICONERROR);
    GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
    }
  else
    {
    // Validate the password from 3CPM.
    if(bValidate_reset_password() == true)
      {
      m_bResetPassword = true;
      m_bNewPassword = true;
      GetDlgItem(IDC_STATIC_ENTER_NEW_PASSWORD)->EnableWindow(TRUE); //ShowWindow(SW_HIDE);
      GetDlgItem(IDC_EDIT_NEW_PASSWORD)->EnableWindow(TRUE); //ShowWindow(SW_HIDE);
      GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE); //ShowWindow(SW_HIDE);
      GetDlgItem(IDC_STATIC_VALIDATE_PASSWORD)->EnableWindow(TRUE); //ShowWindow(SW_HIDE);
      GetDlgItem(IDC_EDIT_VALIDATE_PASSWORD)->EnableWindow(TRUE); //ShowWindow(SW_HIDE);
      GetDlgItem(IDC_STATIC_ENTER_PASSWORD)->EnableWindow(FALSE);
      GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);
      GetDlgItem(IDC_EDIT_NEW_PASSWORD)->SetFocus();

      //// Hide the normal password text box.
      //// Move the reset password text box over the password text box and show it.
      //pcwnd1 = GetDlgItem(IDC_EDIT_PASSWORD);
      //pcwnd->ShowWindow(SW_HIDE);
      //pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
      //pcwnd1 = GetDlgItem(IDC_EDIT_RESET_PASSWORD);
      //pcwnd1->ShowWindow(SW_SHOW);
      //pcwnd1->GetWindowPlacement(&wp1);
      //// Move the Next button to the right side of the password and show it.
      //pcwnd = GetDlgItem(IDC_BUTTON_NEXT);
      //pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field of Next button.
      //iHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
      //iWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
      //wp.rcNormalPosition.top = wp1.rcNormalPosition.top;
      //wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + iHeight;
      //wp.rcNormalPosition.left = wp1.rcNormalPosition.right + 10;
      //wp.rcNormalPosition.right = wp.rcNormalPosition.left + iWidth;
      //pcwnd->SetWindowPlacement(&wp);
      //pcwnd->ShowWindow(SW_SHOW);
      //pcwnd->EnableWindow(TRUE);

      //m_cstrHideShowState = "Enter the password you received from 3CPM and click Reset Password.";
      m_cstrHideShowState = "Enter your new password, validate it and then click OK.";
      }
    else
      MessageBox(g_pLang->cstrGet_text(ITEM_PSWD_OUT_OF_DATE), "Error", MB_ICONERROR);
      //MessageBox("The password from 3CPM is out of date.  Please contact 3CPM for a new password.", "Error", MB_ICONERROR);
    UpdateData(FALSE);
    }
  }

/********************************************************************
OnBnClickedButtonNext

  Event handler for the Next button.
    Current not used.
********************************************************************/
void CHideStudyDates::OnBnClickedButtonNext()
  {
  // TODO: Add your control notification handler code here
  // Decode the date, get the current date and if not more than 7 days
  //   apart, allow user to enter a new password.
  }

/********************************************************************
vMove_new_password_fields

  Move password fields around for entering a password the first time.

Inputs: None.

Return: None.
********************************************************************/
void CHideStudyDates::vMove_new_password_fields(void)
  {
  CWnd *pcwnd, *pcwnd1;
  WINDOWPLACEMENT wp, wp1;
  int iHeight; // iWidth;

  pcwnd = GetDlgItem(IDC_STATIC_ENTER_PASSWORD);
  pcwnd1 = GetDlgItem(IDC_STATIC_ENTER_NEW_PASSWORD);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
  pcwnd1->GetWindowPlacement(&wp1); // Dimensions and position of field.
  //wp1.rcNormalPosition.left = wp.rcNormalPosition.left;
  wp1.rcNormalPosition.top = wp.rcNormalPosition.top + 30;
  wp1.rcNormalPosition.bottom = wp1.rcNormalPosition.top + iHeight;
  pcwnd1->SetWindowPlacement(&wp1);

  pcwnd = GetDlgItem(IDC_EDIT_NEW_PASSWORD);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
  wp.rcNormalPosition.top = wp1.rcNormalPosition.bottom + 10;
  wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + iHeight;
  pcwnd->SetWindowPlacement(&wp);
  
  //pcwnd = GetDlgItem(IDC_BUTTON_NEXT);
  //pcwnd->GetWindowPlacement(&wp1); // Dimensions and position of field.
  //iHeight = wp1.rcNormalPosition.bottom - wp1.rcNormalPosition.top;
  ////iWidth = wp1.rcNormalPosition.right - wp1.rcNormalPosition.left;
  //wp1.rcNormalPosition.top = wp.rcNormalPosition.top;
  //wp1.rcNormalPosition.bottom = wp1.rcNormalPosition.top + iHeight;
  ////wp1.rcNormalPosition.left = wp.rcNormalPosition.right + 10;
  ////wp1.rcNormalPosition.right = wp1.rcNormalPosition.left + iWidth;
  //pcwnd->SetWindowPlacement(&wp1);

  pcwnd = GetDlgItem(IDC_STATIC_VALIDATE_PASSWORD);
  pcwnd->GetWindowPlacement(&wp1); // Dimensions and position of field.
  iHeight = wp1.rcNormalPosition.bottom - wp1.rcNormalPosition.top;
  wp1.rcNormalPosition.top = wp.rcNormalPosition.bottom + 10;
  wp1.rcNormalPosition.bottom = wp1.rcNormalPosition.top + iHeight;
  pcwnd->SetWindowPlacement(&wp1);

  pcwnd = GetDlgItem(IDC_EDIT_VALIDATE_PASSWORD);
  pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  iHeight = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
  wp.rcNormalPosition.top = wp1.rcNormalPosition.bottom + 10;
  wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + iHeight;
  pcwnd->SetWindowPlacement(&wp);

  pcwnd = GetDlgItem(IDOK);
  pcwnd->GetWindowPlacement(&wp1); // Dimensions and position of field.
  iHeight = wp1.rcNormalPosition.bottom - wp1.rcNormalPosition.top;
  wp1.rcNormalPosition.top = wp.rcNormalPosition.bottom + 20;
  wp1.rcNormalPosition.bottom = wp1.rcNormalPosition.top + iHeight;
  pcwnd->SetWindowPlacement(&wp1);
  
  //pcwnd = GetDlgItem(IDCANCEL);
  //pcwnd->GetWindowPlacement(&wp); // Dimensions and position of field.
  //wp.rcNormalPosition.top = wp1.rcNormalPosition.top;
  //wp.rcNormalPosition.bottom = wp1.rcNormalPosition.bottom;
  //pcwnd->SetWindowPlacement(&wp);

  //pcwnd = g_pFrame->GetTopWindow();
  //GetWindowPlacement(&wp);
  //wp.rcNormalPosition.bottom -= 40; //wp1.rcNormalPosition.bottom + 10;
  //SetWindowPlacement(&wp);
  
  }

/********************************************************************
bValidate_reset_password

  Validate the reset password received from 3CPM.
    - It must resolve into a valid date.
    - The date must be <= 7 days from today.

Inputs: None.

Return: true if date is valid.
        false if date is not valid.
********************************************************************/
bool CHideStudyDates::bValidate_reset_password(void)
  {
  bool bRet;
  int iDay, iMonth, iYear;
  char szDay[5], szMonth[5], szYear[10];
  CTime ctNow, ctPassword;
  CTimeSpan ctsElapsed;

#ifdef _DEBUg
  bRet = true;
#else
  bRet = false;
  if(m_cstrPassword.GetLength() >= 10)
    { // Have proper length.
    // y2,m1,d3,y3,d1,@,y1,m2,d2,y4
    // Day
    sprintf(szDay, "%c%c%c", m_cstrPassword[4], m_cstrPassword[8], m_cstrPassword[2]);
    sprintf(szMonth, "%c%c", m_cstrPassword[1], m_cstrPassword[7]);
    sprintf(szYear, "%c%c%c%c", m_cstrPassword[6], m_cstrPassword[0], m_cstrPassword[3], m_cstrPassword[9]);

    iDay = atoi(szDay);
    iMonth = atoi(szMonth);
    iYear = atoi(szYear);

    iDay = ((iDay / 2) - 11) / 3;
    iMonth = ((iMonth - iDay) - 5) / 2;
    iYear = (((iYear - iDay) - 100) / 3) + 1000;

    if(iDay >= 1 && iMonth >= 1 && iYear >= 1)
      { // Day, Month, Year appear to be valid.
      // Now validate the date, the password must be used within 7 days.
      ctNow = CTime::GetCurrentTime();
      ctPassword = CTime(iYear, iMonth, iDay, 0, 0, 0);
      ctsElapsed = ctNow - ctPassword;
      if(ctsElapsed.GetDays() <= 7)
        bRet = true; // Password is good.
      }
    }
#endif
  return(bRet);
  }

/********************************************************************
vSet_text

  Display the text from the language file in all the dialog box fields.

Inputs: None.

Return: None.
********************************************************************/
void CHideStudyDates::vSet_text(void)
  {
  CString cstrTemp;

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_HIDE_STUDY_DATE);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_HIDE_STUDY_DATES_TITLE));

    GetDlgItem(IDC_STATIC_ENTER_PASSWORD)->SetWindowText(g_pLang->cstrGet_text(ITEM_ENTER_PSWD));
    GetDlgItem(IDC_BUTTON_NEW_PASSWORD)->SetWindowText(g_pLang->cstrGet_text(ITEM_CREATE_NEW_PSWD));
    GetDlgItem(IDC_BUTTON_RESET_PASSWORD)->SetWindowText(g_pLang->cstrGet_text(ITEM_RESET_PSWD));
    GetDlgItem(IDC_STATIC_ENTER_NEW_PASSWORD)->SetWindowText(g_pLang->cstrGet_text(ITEM_ENTER_NEW_PSWD));
    GetDlgItem(IDC_BUTTON_NEXT)->SetWindowText(g_pLang->cstrGet_text(ITEM_NEXT));
    GetDlgItem(IDC_STATIC_VALIDATE_PASSWORD)->SetWindowText(g_pLang->cstrGet_text(ITEM_REENTER_PSWD));
    cstrTemp = g_pLang->cstrLoad_string(ITEM_G_OK);
    GetDlgItem(IDOK)->SetWindowText((LPCTSTR)cstrTemp);
    cstrTemp = g_pLang->cstrLoad_string(ITEM_G_CANCEL);
    GetDlgItem(IDCANCEL)->SetWindowText((LPCTSTR)cstrTemp);

    }
  }
