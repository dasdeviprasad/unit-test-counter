/********************************************************************
SelectStudyType.cpp

Copyright (C) 2007,2012, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CSelectStudyType class.

  This class provides for selection of the study type: Standard or Research.

HISTORY:
28-JUL-07  RET  New.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "SelectStudyType.h"
#include ".\selectstudytype.h"
#include "util.h"


// CSelectStudyType dialog

IMPLEMENT_DYNAMIC(CSelectStudyType, CDialog)

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to parent window.
********************************************************************/
CSelectStudyType::CSelectStudyType(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectStudyType::IDD, pParent)
  , m_bStandard(FALSE)
  , m_bResearch(FALSE)
  {
  }

/********************************************************************
Destructor

  Clean up class data.
********************************************************************/
CSelectStudyType::~CSelectStudyType()
{
}

void CSelectStudyType::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Check(pDX, IDC_CHK_STANDARD, m_bStandard);
DDX_Check(pDX, IDC_CHK_RESEARCH, m_bResearch);
}


BEGIN_MESSAGE_MAP(CSelectStudyType, CDialog)
  ON_BN_CLICKED(IDC_CHK_STANDARD, OnBnClickedChkStandard)
  ON_BN_CLICKED(IDC_CHK_RESEARCH, OnBnClickedChkResearch)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectStudyType message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
********************************************************************/
BOOL CSelectStudyType::OnInitDialog()
  {
  CFont cfFont;

  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  vSet_text();
    // Set the font for the title.
  vSet_font(this, NULL, &cfFont, FW_THIN, 10);
  GetDlgItem(IDC_TITLE)->SetFont(&cfFont);

  m_bResearch = TRUE;
  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnBnClickedChkStandard

  Message handler for the Standard study check box.
********************************************************************/
void CSelectStudyType::OnBnClickedChkStandard()
  {

  UpdateData(TRUE);
    // If Standard study is checked, uncheck the Research study checkbox.
  if(m_bStandard == TRUE)
    m_bResearch = FALSE;
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedChkResearch

  Message handler for the Research study check box.
********************************************************************/
void CSelectStudyType::OnBnClickedChkResearch()
  {

  UpdateData(TRUE);
    // If Research study is checked, uncheck the Standard study checkbox.
  if(m_bResearch == TRUE)
    m_bStandard = FALSE;
  UpdateData(FALSE);
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
ptGet_study_type - Get the study type.

  inputs: None.

  return: Study type from ePgmType enumerated list in EGGSAS.h.
********************************************************************/
ePgmType CSelectStudyType::ptGet_study_type()
  {
  ePgmType ptRet;

  if(m_bResearch == TRUE)
    ptRet = PT_RESEARCH;
  else
    ptRet = PT_STANDARD;
  return(ptRet);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CSelectStudyType::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_STUDY_TYPE);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_ST_STUDY_TYPE));

    GetDlgItem(IDC_TITLE)->SetWindowText(g_pLang->cstrGet_text(ITEM_ST_SEL_STUDY_TYPE));
    GetDlgItem(IDC_CHK_STANDARD)->SetWindowText(g_pLang->cstrGet_text(ITEM_ST_STD_STUDY));
    GetDlgItem(IDC_CHK_RESEARCH)->SetWindowText(g_pLang->cstrGet_text(ITEM_ST_RSCH_STUDY));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    }
  }
