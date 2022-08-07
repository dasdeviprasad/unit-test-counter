/********************************************************************
SearchDB.cpp

Copyright (C) 2018 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CSearchDB class.

  This class provides for entering search criteria for searching the database.


HISTORY:
19-MAY-18  RET  New.
********************************************************************/

#include "stdafx.h"
#include "StudyExport.h"
#include "Dbaccess.h"
#include "SearchDB.h"


// CSearchDB dialog

IMPLEMENT_DYNAMIC(CSearchDB, CDialog)

CSearchDB::CSearchDB(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchDB::IDD, pParent)
	//, m_bLastName(false)
	//, m_bResRefNum(false)
	//, m_bNameAsc(false)
	//, m_bNameDesc(false)
	//, m_cstrName(_T(""))
	//, m_bDatesOlder(false)
	//, m_bDatesNewer(false)
	//, m_bDatesEqual(false)
	//, m_cstrMonth(_T(""))
	//, m_cstrDay(_T(""))
	//, m_cstrYear(_T(""))
	, m_bLastName(FALSE)
	, m_bResRefNum(FALSE)
	, m_bNameAsc(FALSE)
	, m_bNameDesc(FALSE)
	, m_cstrName(_T(""))
	, m_bDatesOlder(FALSE)
	, m_bDatesNewer(FALSE)
	, m_bDatesEqual(FALSE)
	, m_cstrMonth(_T(""))
	, m_cstrDay(_T(""))
	, m_cstrYear(_T(""))
{

}

CSearchDB::~CSearchDB()
{
}

void CSearchDB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Check(pDX, IDC_CHK_LAST_NAME, m_bLastName);
	//DDX_Check(pDX, IDC_CHK_RRNUM, m_bResRefNum);
	//DDX_Check(pDX, IDC_CHK_NAME_ASC, m_bNameAsc);
	//DDX_Check(pDX, IDC_CHK_NAME_DESC, m_bNameDesc);
	//DDX_Text(pDX, IDC_EDIT_NAME_RRNUM, m_cstrName);
	//DDX_Check(pDX, IDC_CHK_DATE_OLDER, m_bDatesOlder);
	//DDX_Check(pDX, IDC_CHK_DATE_NEWER, m_bDatesNewer);
	//DDX_Check(pDX, IDC_CHK_DATE_EQUAL, m_bDatesEqual);
	//DDX_Text(pDX, IDC_EDIT_MONTH, m_cstrMonth);
	//DDX_Text(pDX, IDC_EDIT_DAY, m_cstrDay);
	//DDX_Text(pDX, IDC_EDIT_YEAR, m_cstrYear);
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
	ON_BN_CLICKED(IDOK, &CSearchDB::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSearchDB::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHK_LAST_NAME, &CSearchDB::OnBnClickedChkLastName)
	ON_BN_CLICKED(IDC_CHK_RRNUM, &CSearchDB::OnBnClickedChkRrnum)
	ON_BN_CLICKED(IDC_CHK_NAME_ASC, &CSearchDB::OnBnClickedChkNameAsc)
	ON_BN_CLICKED(IDC_CHK_NAME_DESC, &CSearchDB::OnBnClickedChkNameDesc)
	ON_BN_CLICKED(IDC_CHK_DATE_OLDER, &CSearchDB::OnBnClickedChkDateOlder)
	ON_BN_CLICKED(IDC_CHK_DATE_NEWER, &CSearchDB::OnBnClickedChkDateNewer)
	ON_BN_CLICKED(IDC_CHK_DATE_EQUAL, &CSearchDB::OnBnClickedChkDateEqual)
	ON_EN_CHANGE(IDC_EDIT_MONTH, &CSearchDB::OnEnChangeEditMonth)
	ON_EN_CHANGE(IDC_EDIT_DAY, &CSearchDB::OnEnChangeEditDay)
	ON_EN_CHANGE(IDC_EDIT_YEAR, &CSearchDB::OnEnChangeEditYear)
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
  UpdateData(FALSE);

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
	CString cstrErr, cstrTitle;

  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  if(m_bDatesOlder == TRUE || m_bDatesNewer == TRUE || m_bDatesEqual == TRUE)
    { // User selected to search by date equal so the date fields must be filled in.
    m_cstrDate = m_cstrMonth + _T("/") + m_cstrDay + _T("/") + m_cstrYear;
    if(m_cstrMonth.IsEmpty() == true || m_cstrDay.IsEmpty() == true || m_cstrYear.IsEmpty() == true)
      {
      bOKToExit = false;
			cstrErr.LoadStringW(IDS_SELECT_STUDY);
			cstrTitle.LoadStringW(IDS_ENTER_COMPLETE_DATE);
      MessageBox(cstrTitle, cstrErr, MB_OK);
      }
    if((iIndex = m_cstrYear.Find(_T("*")) ) < 0)
      { // User entered a year and not an *.
      if(m_cstrYear.GetLength() < 4)
        {
        bOKToExit = false;
				cstrErr.LoadStringW(IDS_SELECT_STUDY);
				cstrTitle.LoadStringW(IDS_ENTER_4_DIG_YEAR);
        MessageBox(cstrTitle, cstrErr, MB_OK);
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
      m_cstrSQL.Format(_T("SELECT PatientID, LastName, FirstName, MiddleInitial, UseResrchRefNum, ResearchRefNum, Hide\
                       FROM Patient WHERE LastName Like '%s' And LastName<>''"), m_cstrName);

			//m_cstrSQL.Format(_T("Select PatientID,LastName,FirstName,MiddleInitial,UseResrchRefNum,ResearchRefNum,Hide \
			//										from Patient WHERE LastName Like '%s' And LastName<>''"), m_cstrName);

      if(m_bNameAsc == TRUE)
        m_cstrSQL.Append(_T("ORDER BY LastName, FirstName, MiddleInitial"));
      else
        m_cstrSQL.Append(_T(" ORDER BY LastName DESC"));
      }
    else if(m_bResRefNum == TRUE)
      {
      m_cstrSQL.Format(_T("SELECT PatientID, LastName, FirstName, MiddleInitial, UseResrchRefNum, ResearchRefNum, Hide\
                       FROM Patient WHERE ResearchRefNum Like '%s' And ResearchRefNum<>'' ORDER BY ResearchRefNum"),
                       m_cstrName);
      if(m_bNameDesc == TRUE)
        m_cstrSQL.Append(_T(" DESC"));
      }
    }
  else
    { // Name is empty.
    if(m_bLastName == TRUE)
      {
      m_cstrSQL = _T("SELECT PatientID, LastName, FirstName, MiddleInitial, UseResrchRefNum, ResearchRefNum, Hide\
                       FROM Patient WHERE LastName<>''");
      if(m_bNameAsc == TRUE)
        m_cstrSQL.Append(_T("ORDER BY LastName, FirstName, MiddleInitial"));
      else
        m_cstrSQL.Append(_T(" ORDER BY LastName DESC"));
      }
    else if(m_bResRefNum == TRUE)
      {
      m_cstrSQL = _T("SELECT PatientID, LastName, FirstName, MiddleInitial, UseResrchRefNum, ResearchRefNum, Hide\
                       FROM Patient WHERE ResearchRefNum<>'' ORDER BY ResearchRefNum");
      if(m_bNameDesc == TRUE)
        m_cstrSQL.Append(_T(" DESC"));
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
	CString cstrText;

	// TODO: Add your control notification handler code here
  UpdateData(TRUE);
  if(m_bLastName == TRUE)
  	m_bResRefNum = FALSE; 
  else
    m_bLastName = TRUE;
	cstrText.LoadStringW(IDS_LAST_NAME_RES_REF_NUM);
  GetDlgItem(IDC_STATIC_LABEL_NAME)->SetWindowText(cstrText);
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
	CString cstrText;

  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  if(m_bResRefNum == TRUE)
  	m_bLastName = FALSE; 
  else
    m_bResRefNum = TRUE;
	cstrText.LoadStringW(IDS_RSRCH_REFNUM);
  GetDlgItem(IDC_STATIC_LABEL_NAME)->SetWindowText(cstrText);
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
	// TODO: Add your control notification handler code here
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
	// TODO: Add your control notification handler code here
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
	// TODO: Add your control notification handler code here
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
	// TODO: Add your control notification handler code here
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
	// TODO: Add your control notification handler code here
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
          iIndex = cstrItem.Find(_T("*"));
          if(cstrItem.Find(_T("*"), iIndex + 1) >= 0)
            cstrItem.Delete(i, 1);
          }
        }
      else
        cstrItem.Delete(i, 1);
      }
    else if(iEntryType == YEAR_ENTRY)
      { // User entered a digit for the year, make sure there is not an
        // * before the digit.
      if((iIndex = cstrItem.Find(_T("*"))) >= 0 && iIndex < i)
        cstrItem.Delete(i, 1);
      }
    } // End of for
  // Evaluate the day and month
  if(iEntryType == MONTH_ENTRY || iEntryType == DAY_ENTRY)
    {
    if(cstrItem.GetLength() > 1)
      {
      CW2A pszText(cstrItem);
      iNum = atoi(pszText);
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
void CSearchDB::vEnable_date_fields(void)
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
