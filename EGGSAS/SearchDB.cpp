/********************************************************************
SearchDB.cpp

Copyright (C) 2018,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CSearchDB class.

  This class provides for entering search criteria for searching the database.

TODO:
  Finish adding text to language file.
  The lastname label should change between last name and research reference number.
  Test



HISTORY:
20-JAN-18  RET  New.
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedSearchHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "Dbaccess.h"
#include "SearchDB.h"
#include ".\searchdb.h"
#include "util.h"


// CSearchDB dialog

IMPLEMENT_DYNAMIC(CSearchDB, CDialog)
CSearchDB::CSearchDB(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchDB::IDD, pParent)
  , m_bLastName(FALSE)
  , m_bResRefNum(FALSE)
  , m_bNameAsc(FALSE)
  , m_bNameDesc(FALSE)
  , m_cstrName(_T(""))
  , m_bDateAsc(FALSE)
  , m_bDateDesc(FALSE)
  , m_bDatesOlder(FALSE)
  , m_bDatesNewer(FALSE)
  , m_bDatesEqual(FALSE)
  , m_cstrMonth(_T(""))
  , m_cstrDay(_T(""))
  , m_cstrYear(_T(""))
  , m_cstrSQL(_T(""))
  , m_iDateSrchType(0)
  , m_cstrDate(_T(""))
  {
}

CSearchDB::~CSearchDB()
{
}

void CSearchDB::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Check(pDX, IDC_CHK_LAST_NAME, m_bLastName);
DDX_Check(pDX, IDC_CHK_RRNUM, m_bResRefNum);
DDX_Check(pDX, IDC_CHK_NAME_ASC, m_bNameAsc);
DDX_Check(pDX, IDC_CHK_NAME_DESC, m_bNameDesc);
DDX_Text(pDX, IDC_EDIT_NAME_RRNUM, m_cstrName);
DDX_Check(pDX, IDC_CHK_DATE_OLDER, m_bDatesOlder);
DDX_Check(pDX, IDC_CHK_DATE_NEWER, m_bDatesNewer);
DDX_Check(pDX, IDC_CHK_DATE_EQUAL, m_bDatesEqual);
DDX_Text(pDX, IDC_EDIT_MONTH, m_cstrMonth);
DDX_Text(pDX, IDC_EDIT_DAY, m_cstrDay);
DDX_Text(pDX, IDC_EDIT_YEAR, m_cstrYear);
}


BEGIN_MESSAGE_MAP(CSearchDB, CDialog)
  ON_EN_CHANGE(IDC_EDIT_MONTH, OnEnChangeEditMonth)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDC_CHK_LAST_NAME, OnBnClickedChkLastName)
  ON_BN_CLICKED(IDC_CHK_RRNUM, OnBnClickedChkRrnum)
  ON_BN_CLICKED(IDC_CHK_NAME_ASC, OnBnClickedChkNameAsc)
  ON_BN_CLICKED(IDC_CHK_NAME_DESC, OnBnClickedChkNameDesc)
  ON_BN_CLICKED(IDC_CHK_DATE_OLDER, OnBnClickedChkDateOlder)
  ON_BN_CLICKED(IDC_CHK_DATE_NEWER, OnBnClickedChkDateNewer)
  ON_BN_CLICKED(IDC_CHK_DATE_EQUAL, OnBnClickedChkDateEqual)
  ON_EN_CHANGE(IDC_EDIT_DAY, OnEnChangeEditDay)
  ON_EN_CHANGE(IDC_EDIT_YEAR, OnEnChangeEditYear)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
  ON_BN_CLICKED(IDC_SEARCH_HELP, OnBnClickedSearchHelp)
END_MESSAGE_MAP()


// CSearchDB message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Set the text for the fields.
    Enable/disable appropriate fields.
********************************************************************/
BOOL CSearchDB::OnInitDialog()
  {
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  m_bLastName = TRUE;
  m_bNameAsc = TRUE;
  m_bDateAsc = TRUE;
  UpdateData(FALSE);

  vSet_text();

#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
  if(g_pConfig->m_bHideStudyDates == true)
    { // Hiding study dates.
    // Disable all date fields
    GetDlgItem(IDC_EDIT_MONTH)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_DAY)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_YEAR)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_STUDY_DATE)->EnableWindow(FALSE);
    GetDlgItem(IDC_CHK_DATE_OLDER)->EnableWindow(FALSE);
    GetDlgItem(IDC_CHK_DATE_NEWER)->EnableWindow(FALSE);
    GetDlgItem(IDC_CHK_DATE_EQUAL)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_DATE_HELP)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_LABEL_DATE)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_MONTH_HELP)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_DAY_HELP)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_YEAR_HELP)->EnableWindow(FALSE);
    }
  else
    vEnable_date_fields(); // Not hiding study dates.
#else
  // Not hiding study dates.
  // Disable the date fields until a date checkbox is checked.
  vEnable_date_fields();
#endif

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnBnClickedOk

  Message handler for the OK button.

  Validate the date.
  Set date variables for caller.
  Create SQL statements for the last name/research ref num for caller.
********************************************************************/
void CSearchDB::OnBnClickedOk()
  {
  bool bDateOK = true;
  bool bOKToExit = true;
  bool bDateErr = false;
  int iIndex; //, iValue;

  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  if(m_bDatesOlder == TRUE || m_bDatesNewer == TRUE || m_bDatesEqual == TRUE)
    { // User selected to search by date equal so the date fields must be filled in.
    m_cstrDate = m_cstrMonth + "/" + m_cstrDay + "/" + m_cstrYear;
    if(m_cstrMonth.IsEmpty() == true || m_cstrDay.IsEmpty() == true || m_cstrYear.IsEmpty() == true)
      {
      bOKToExit = false;
      MessageBox("Please enter a complete date.", g_pLang->cstrLoad_string(ITEM_G_ERROR), MB_OK);
      }
    if((iIndex = m_cstrYear.Find("*") ) < 0)
      { // User entered a year and not an *.
      if(m_cstrYear.GetLength() < 4)
        {
        bOKToExit = false;
        MessageBox("Please enter a 4 digit year.", g_pLang->cstrLoad_string(ITEM_G_ERROR), MB_OK);
        }
      }
    }
  else
    m_cstrDate.Empty();

  if(m_cstrName.IsEmpty() == false)
    {
    m_cstrName.Replace('*', '%');
    if(m_bLastName == TRUE)
      {
      m_cstrSQL.Format("SELECT PatientID, LastName, FirstName, MiddleInitial, UseResrchRefNum, ResearchRefNum, Hide\
                       FROM Patient WHERE LastName Like '%s' And LastName<>''", m_cstrName);
      if(m_bNameAsc == TRUE)
        m_cstrSQL.Append("ORDER BY LastName, FirstName, MiddleInitial");
      else
        m_cstrSQL.Append(" ORDER BY LastName DESC");
      }
    else if(m_bResRefNum == TRUE)
      {
      m_cstrSQL.Format("SELECT PatientID, LastName, FirstName, MiddleInitial, UseResrchRefNum, ResearchRefNum, Hide\
                       FROM Patient WHERE ResearchRefNum Like '%s' And ResearchRefNum<>'' ORDER BY ResearchRefNum",
                       m_cstrName);
      if(m_bNameDesc == TRUE)
        m_cstrSQL.Append(" DESC");
      }
    }
  else
    { // Name is empty.
    if(m_bLastName == TRUE)
      {
      m_cstrSQL = "SELECT PatientID, LastName, FirstName, MiddleInitial, UseResrchRefNum, ResearchRefNum, Hide\
                       FROM Patient WHERE LastName<>''";
      if(m_bNameAsc == TRUE)
        m_cstrSQL.Append("ORDER BY LastName, FirstName, MiddleInitial");
      else
        m_cstrSQL.Append(" ORDER BY LastName DESC");
      }
    else if(m_bResRefNum == TRUE)
      {
      m_cstrSQL = "SELECT PatientID, LastName, FirstName, MiddleInitial, UseResrchRefNum, ResearchRefNum, Hide\
                       FROM Patient WHERE ResearchRefNum<>'' ORDER BY ResearchRefNum";
      if(m_bNameDesc == TRUE)
        m_cstrSQL.Append(" DESC");
      }
    }

  if(bOKToExit == true)
    { // Process the date.
    if(m_bDatesOlder == TRUE)
      m_iDateSrchType = DB_SRCH_LESS_THAN;
    else if(m_bDatesNewer == TRUE)
      m_iDateSrchType = DB_SRCH_GREATER_THAN;
    else if(m_bDatesEqual == TRUE)
      m_iDateSrchType = DB_SRCH_EQUAL_TO;
    else
      m_iDateSrchType = DB_SRCH_NONE;
    }

  if(bOKToExit == true)
    OnOK();
  }

/********************************************************************
OnBnClickedCancel

  Message handler for the Cancel button.

********************************************************************/
void CSearchDB::OnBnClickedCancel()
  {
  // TODO: Add your control notification handler code here
  OnCancel();
  }

/********************************************************************
OnBnClickedChkLastName

  Message handler for the Last name checkbox.

  If checked, uncheck the research reference number checkbox.
  If unchecked, check it.
********************************************************************/
void CSearchDB::OnBnClickedChkLastName()
  {
  UpdateData(TRUE);
  if(m_bLastName == TRUE)
  	m_bResRefNum = FALSE; 
  else
    m_bLastName = TRUE;
  GetDlgItem(IDC_STATIC_LABEL_NAME)->SetWindowText(g_pLang->cstrGet_text(ITEM_LAST_NAME));
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedChkRrnum

  Message handler for the research reference number checkbox.

  If checked, uncheck the Last name checkbox.
  If unchecked, it.
********************************************************************/
void CSearchDB::OnBnClickedChkRrnum()
  {
  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  if(m_bResRefNum == TRUE)
  	m_bLastName = FALSE; 
  else
    m_bResRefNum = TRUE;
  GetDlgItem(IDC_STATIC_LABEL_NAME)->SetWindowText(g_pLang->cstrGet_text(ITEM_RSRCH_REFNUM));
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedChkNameAsc

  Message handler for the Name Ascending order checkbox.

  If checked, uncheck the Name Descending order checkbox.
  If unchecked, check it.
********************************************************************/
void CSearchDB::OnBnClickedChkNameAsc()
  {
  UpdateData(TRUE);
  if(m_bNameAsc == TRUE)
  	m_bNameDesc = FALSE; 
  else
    m_bNameAsc = TRUE;
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedChkNameDesc

  Message handler for the Name Desscending order checkbox.

  If checked, uncheck the Name Ascending order checkbox.
  If unchecked, check it.
********************************************************************/
void CSearchDB::OnBnClickedChkNameDesc()
  {
  UpdateData(TRUE);
  if(m_bNameDesc == TRUE)
  	m_bNameAsc = FALSE;
  else
    m_bNameDesc = TRUE;
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedChkDateOlder

  Message handler for the Date older checkbox.

  If checked, uncheck the Date newer and Date equal checkboxes.
********************************************************************/
void CSearchDB::OnBnClickedChkDateOlder()
  {
  UpdateData(TRUE);
	m_bDatesNewer = FALSE; 
	m_bDatesEqual = FALSE; 
  vEnable_date_fields();
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedChkDateNewer

  Message handler for the Date newer checkbox.

  If checked, uncheck the Date older and Date equal checkboxes.
********************************************************************/
void CSearchDB::OnBnClickedChkDateNewer()
  {
  UpdateData(TRUE);
	m_bDatesOlder = FALSE; 
	m_bDatesEqual = FALSE; 
  vEnable_date_fields();
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedChkDateEqual

  Message handler for the Date equal checkbox.

  If checked, uncheck the Date older and Date newer checkboxes.
********************************************************************/
void CSearchDB::OnBnClickedChkDateEqual()
  {
  UpdateData(TRUE);
	m_bDatesOlder = FALSE; 
	m_bDatesNewer = FALSE;
  vEnable_date_fields();
  UpdateData(FALSE);
  }

/********************************************************************
OnEnChangeEditMonth

  Message handler for the changing text in the Month editbox.

  Validate the entry.
********************************************************************/
void CSearchDB::OnEnChangeEditMonth()
  {
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialog::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  // TODO:  Add your control notification handler code here
  UpdateData(TRUE);
  // Make sure all characters are digits.  If any aren't, remove them
  vValidate_date_entry(MONTH_ENTRY, m_cstrMonth, 2);
  UpdateData(FALSE);
  }

/********************************************************************
OnEnChangeEditDay

  Message handler for the changing text in the Day editbox.

  Validate the entry.
********************************************************************/
void CSearchDB::OnEnChangeEditDay()
  {
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialog::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  // TODO:  Add your control notification handler code here
  UpdateData(TRUE);
  // Make sure all characters are digits.  If any aren't, remove them
  vValidate_date_entry(DAY_ENTRY, m_cstrDay, 2);
  UpdateData(FALSE);
  }

/********************************************************************
OnEnChangeEditYear

  Message handler for the changing text in the Year editbox.

  Validate the entry.
********************************************************************/
void CSearchDB::OnEnChangeEditYear()
  {
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialog::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  // TODO:  Add your control notification handler code here
  UpdateData(TRUE);
  // Make sure all characters are digits.  If any aren't, remove them
  vValidate_date_entry(YEAR_ENTRY, m_cstrYear, 4);
  UpdateData(FALSE);
  }

/********************************************************************
vValidate_date_entry - Validate the part of the date indicated by the
                       first parameter.

  inputs: Type of date entry (month, day, year).
          Reference to a CString containing the text for the editbox.
          Maximum length that is allowed in the field.

  return: None.
********************************************************************/
void CSearchDB::vValidate_date_entry(int iEntryType, CString & cstrItem, int iLen)
  {
  int iIndex, iNum;

  if(cstrItem.GetLength() > iLen)
    cstrItem = cstrItem.Left(iLen);
  for(int i = 0; i < iLen; ++i)
    {
    if(i >= cstrItem.GetLength())
      break;
    if(isdigit(cstrItem[i]) == 0)
      { // Non-digit.
      // Allow an asterisk only if the Date Equal checkbox is checked.
      if(cstrItem[i] == '*')
        {
        if(m_bDatesEqual == FALSE)
          cstrItem.Delete(i, 1);
        if(iEntryType == MONTH_ENTRY || iEntryType == DAY_ENTRY)
          { // Make sure there are no digits in the field and if so
            // don't allow this *.
          if(cstrItem.GetLength() > 1)
            cstrItem.Delete(cstrItem.GetLength() - 1, 1);
          }
        else// if(iEntryType == YEAR_ENTRY)
          { // For year, make sure there is not already an *.
          iIndex = cstrItem.Find("*");
          if(cstrItem.Find("*", iIndex + 1) >= 0)
            cstrItem.Delete(i, 1);
          }
        }
      else
        cstrItem.Delete(i, 1);
      }
    else if(iEntryType == YEAR_ENTRY)
      { // User entered a digit for the year, make sure there is not an
        // * before the digit.
      if((iIndex = cstrItem.Find("*")) >= 0 && iIndex < i)
        cstrItem.Delete(i, 1);
      }
    } // End of for
  // Evaluate the day and month
  if(iEntryType == MONTH_ENTRY || iEntryType == DAY_ENTRY)
    {
    if(cstrItem.GetLength() > 1)
      {
      iNum = atoi(cstrItem);
      if(iNum == 0)
        cstrItem.Empty();
      else if(iEntryType == MONTH_ENTRY && iNum > 12)
        cstrItem.Delete(cstrItem.GetLength() - 1, 1);
      else if(iEntryType == DAY_ENTRY && iNum > 31)
        cstrItem.Delete(cstrItem.GetLength() - 1, 1);
      }
    }
  }

/********************************************************************
vEnable_date_fields - Enable/disable the date entry fields based on any
                       of the date checkboxes being checked.

  inputs: None.

  return: None.
********************************************************************/
void CSearchDB::vEnable_date_fields()
  {
  bool bEnable;

  if(m_bDatesOlder == TRUE || m_bDatesNewer == TRUE || m_bDatesEqual == TRUE)
    bEnable = true;
  else
    bEnable = false;
  GetDlgItem(IDC_EDIT_MONTH)->EnableWindow(bEnable);
  GetDlgItem(IDC_EDIT_DAY)->EnableWindow(bEnable);
  GetDlgItem(IDC_EDIT_YEAR)->EnableWindow(bEnable);
  }

/********************************************************************
vSet_text - Set the text for the fields using the correct language.

  inputs: None.

  return: None.
********************************************************************/
void CSearchDB::vSet_text(void)
    {

    if(g_pLang != NULL)
      {
      g_pLang->vStart_section(SEC_SEARCH);
      this->SetWindowText(g_pLang->cstrGet_text(ITEM_SEARCH_TITLE));
      GetDlgItem(IDC_STATIC_NAME_RRNUM)->SetWindowText(g_pLang->cstrGet_text(ITEM_LAST_NAME_RSRCH_REF_NUM));
      GetDlgItem(IDC_CHK_LAST_NAME)->SetWindowText(g_pLang->cstrGet_text(ITEM_LAST_NAME));
      GetDlgItem(IDC_CHK_RRNUM)->SetWindowText(g_pLang->cstrGet_text(ITEM_RSRCH_REFNUM));
      GetDlgItem(IDC_CHK_NAME_ASC)->SetWindowText(g_pLang->cstrGet_text(ITEM_ASC_ORDER));
      GetDlgItem(IDC_CHK_NAME_DESC)->SetWindowText(g_pLang->cstrGet_text(ITEM_DESC_ORDER));
      GetDlgItem(IDC_STATIC_LABEL_NAME)->SetWindowText(g_pLang->cstrGet_text(ITEM_LAST_NAME));
      GetDlgItem(IDC_STATIC_NAME_HELP)->SetWindowText(g_pLang->cstrGet_text(ITEM_NAME_HELP));
      GetDlgItem(IDC_STATIC_STUDY_DATE)->SetWindowText(g_pLang->cstrGet_text(ITEM_STUDY_DATE));
      GetDlgItem(IDC_CHK_DATE_OLDER)->SetWindowText(g_pLang->cstrGet_text(ITEM_DATE_OLDER));
      GetDlgItem(IDC_CHK_DATE_NEWER)->SetWindowText(g_pLang->cstrGet_text(ITEM_DATE_NEWER));
      GetDlgItem(IDC_CHK_DATE_EQUAL)->SetWindowText(g_pLang->cstrGet_text(ITEM_DATE_EQUAL));
      GetDlgItem(IDC_STATIC_DATE_HELP)->SetWindowText(g_pLang->cstrGet_text(ITEM_DATE_HELP));
      GetDlgItem(IDC_STATIC_LABEL_DATE)->SetWindowText(g_pLang->cstrGet_text(ITEM_STUDY_DATE));
      GetDlgItem(IDC_STATIC_MONTH_HELP)->SetWindowText(g_pLang->cstrGet_text(ITEM_MONTH_HELP));
      GetDlgItem(IDC_STATIC_DAY_HELP)->SetWindowText(g_pLang->cstrGet_text(ITEM_DAY_HELP));
      GetDlgItem(IDC_STATIC_YEAR_HELP)->SetWindowText(g_pLang->cstrGet_text(ITEM_YEAR_HELP));
      GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
      GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
      GetDlgItem(IDC_SEARCH_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
      }
    }

/********************************************************************
OnBnClickedSearchHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
  void CSearchDB::OnBnClickedSearchHelp()
    {
    // TODO: Add your control notification handler code here
    //AfxGetApp()->WinHelp(HIDC_SEARCH_HELP, HELP_CONTEXT);

    HtmlHelp(HIDC_SEARCH_HELP);
    }
