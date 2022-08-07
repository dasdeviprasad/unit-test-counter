/********************************************************************
NameResearchNum.cpp

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the Patient Name/Research Reference Number dialog box.

  Operation:
    - Display the name on the study from the study file name.
    - There are 2 checkboxes, one to use the name as a patient name and the other
      to use the name as a research reference number.  One of the checkboxes is
      always checked (i.e. the user cannot uncheck both checkboxes).
    - One of the checkboxes is initially checked based based on the following:
      . If there is a space in the name, it is most likely a patient name
        and the patient name checkbox is checked.
      . If there is no space in the name, it is most likely a research reference
        number and that checkbox is checked.
    - The user can change the default selection.

HISTORY:
01-JUL-10  RET  New.
********************************************************************/

#include "stdafx.h"
#include "OrphanFileRecovery.h"
#include "NameResearchNum.h"
#include ".\nameresearchnum.h"


// CNameResearchNum dialog

IMPLEMENT_DYNAMIC(CNameResearchNum, CDialog)

/********************************************************************
Constructor

Inputs: Name of study on the data file.
********************************************************************/
CNameResearchNum::CNameResearchNum(CString cstrStudyName, CWnd* pParent /*=NULL*/)
	: CDialog(CNameResearchNum::IDD, pParent)
  , m_cstrStudyName(_T(""))
  , m_bPatientName(FALSE)
  , m_bResearchRefNum(FALSE)
  {

  m_cstrStudyName = cstrStudyName;
}

/********************************************************************
Destructor

********************************************************************/
CNameResearchNum::~CNameResearchNum()
{
}

void CNameResearchNum::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_EDIT_STUDY_NAME, m_cstrStudyName);
DDX_Check(pDX, IDC_CHK_PATIENT_NAME, m_bPatientName);
DDX_Check(pDX, IDC_CHK_RESEARCH_NUM, m_bResearchRefNum);
}


BEGIN_MESSAGE_MAP(CNameResearchNum, CDialog)
  ON_BN_CLICKED(IDC_CHK_PATIENT_NAME, OnBnClickedChkPatientName)
  ON_BN_CLICKED(IDC_CHK_RESEARCH_NUM, OnBnClickedChkResearchNum)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CNameResearchNum message handlers

/********************************************************************
OnInitDialog

  Initialize class data.
  Decide which checkbox should be checked.
********************************************************************/
BOOL CNameResearchNum::OnInitDialog()
  {
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  // Decide whether to initially check the patient name checkbox or
  // the research reference number checkbox.
  // If the study name has a space in it, it is most likely a patient name.
  if(m_cstrStudyName.Find(" ") > 0)
    m_bPatientName = TRUE;
  else
    m_bResearchRefNum = TRUE;
  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnBnClickedOk - Handler for the "OK" button.

Make sure the dialog box variables are updated with the state of
the checkboxes.
********************************************************************/
void CNameResearchNum::OnBnClickedOk()
  {

  UpdateData(TRUE);
  OnOK();
  }

/********************************************************************
OnBnClickedChkPatientName - Handler for the "Patient Name" checkbox.

If the checkbox has been checked, uncheck the research reference number
  checkbox.
If the checkbox has been unchecked (it was checked which means that the
  research reference number checkbox is unchecked), check it again to
  make sure one of the checkboxes is always checked.
********************************************************************/
void CNameResearchNum::OnBnClickedChkPatientName()
  {

  UpdateData(TRUE);
  if(m_bPatientName == TRUE)
    m_bResearchRefNum = FALSE;
  else
    m_bPatientName = TRUE;
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedChkResearchNum - Handler for the "Research Reference Number" checkbox.

If the checkbox has been checked, uncheck the patient name checkbox.
If the checkbox has been unchecked (it was checked which means that the
  patient name checkbox is unchecked), check it again to make sure one
  of the checkboxes is always checked.
********************************************************************/
void CNameResearchNum::OnBnClickedChkResearchNum()
  {

  UpdateData(TRUE);
  if(m_bResearchRefNum == TRUE)
    m_bPatientName = FALSE;
  else
    m_bResearchRefNum = TRUE;
  UpdateData(FALSE);
  }

