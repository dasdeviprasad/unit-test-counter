/********************************************************************
ImportDupName.cpp

Copyright (C) 2018 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CImportDupName class.

  This class provides for importing a patient that already exists in the database under
  a different name or research reference number.



HISTORY:
28-MAY-18  RET  New.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "Dbaccess.h"
#include "util.h"
#include "ImportRemoteDB.h"
#include "ImportDupName.h"
#include ".\importdupname.h"


// CImportDupName dialog

IMPLEMENT_DYNAMIC(CImportDupName, CDialog)
CImportDupName::CImportDupName(CWnd* pParent /*=NULL*/)
	: CDialog(CImportDupName::IDD, pParent)
  , m_cstrName(_T(""))
  , m_bUseResrchRefNum(false)
  {

  m_bUseResrchRefNum = FALSE; // Initialize to using name.
}

CImportDupName::~CImportDupName()
{
}

void CImportDupName::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_EDIT_NAME, m_cstrName);
}


BEGIN_MESSAGE_MAP(CImportDupName, CDialog)
  ON_BN_CLICKED(IDC_RADIO_SKIP, OnBnClickedRadioSkip)
  ON_BN_CLICKED(IDC_RADIO_OVERWRITE, OnBnClickedRadioOverwrite)
  ON_BN_CLICKED(IDC_RADIO_CHANGE_NAME, OnBnClickedRadioChangeName)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CImportDupName message handlers

BOOL CImportDupName::OnInitDialog()
  {
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  vSet_text();

  GetDlgItem(IDC_EDIT_NAME)->EnableWindow(FALSE);
  GetDlgItem(IDC_STATIC_NAME)->EnableWindow(FALSE);

  CheckRadioButton(IDC_RADIO_SKIP, IDC_RADIO_CHANGE_NAME, IDC_RADIO_SKIP);
  m_iDupAction = IMPORT_DUP_SKIP;

  UpdateData(TRUE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

void CImportDupName::OnBnClickedRadioSkip()
  {
  // TODO: Add your control notification handler code here
  if(IsDlgButtonChecked(IDC_RADIO_SKIP) != 0)
    {
    GetDlgItem(IDC_EDIT_NAME)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_NAME)->EnableWindow(FALSE);
    m_iDupAction = IMPORT_DUP_SKIP;
    }
  }

void CImportDupName::OnBnClickedRadioOverwrite()
  {
  // TODO: Add your control notification handler code here
  if(IsDlgButtonChecked(IDC_RADIO_OVERWRITE) != 0)
    {
    GetDlgItem(IDC_EDIT_NAME)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_NAME)->EnableWindow(FALSE);
    m_iDupAction = IMPORT_DUP_OVERWRITE;
    }
  }

void CImportDupName::OnBnClickedRadioChangeName()
  {
  // TODO: Add your control notification handler code here
  if(IsDlgButtonChecked(IDC_RADIO_CHANGE_NAME) != 0)
    {
    GetDlgItem(IDC_EDIT_NAME)->EnableWindow(TRUE);
    GetDlgItem(IDC_STATIC_NAME)->EnableWindow(TRUE);
    m_iDupAction = IMPORT_DUP_CHANGE;
    }
  }

void CImportDupName::OnBnClickedOk()
  {
  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  OnOK();
  }

void CImportDupName::OnBnClickedCancel()
  {
  // TODO: Add your control notification handler code here
  OnCancel();
  }


void CImportDupName::vSet_text(void)
  {
  CString strText;

  SetWindowText(m_cstrFullName);
  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_IMPORT_DUP_NAME);
    strText.Format(g_pLang->cstrGet_text(ITEM_DUP_NAME_TITLE), m_cstrFullName);
    strText += "\n" + g_pLang->cstrGet_text(ITEM_DUP_NAME_TITLE1);
    GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(strText); 
    GetDlgItem(IDC_RADIO_SKIP)->SetWindowText(g_pLang->cstrGet_text(ITEM_SKIP));
    GetDlgItem(IDC_RADIO_OVERWRITE)->SetWindowText(g_pLang->cstrGet_text(ITEM_OVERWRITE));
    GetDlgItem(IDC_RADIO_CHANGE_NAME)->SetWindowText(g_pLang->cstrGet_text(ITEM_CHANGE));

    if(m_bUseResrchRefNum == TRUE)
      GetDlgItem(IDC_STATIC_NAME)->SetWindowText(g_pLang->cstrGet_text(ITEM_RSRCH_REF_NUM));
    else
      GetDlgItem(IDC_STATIC_NAME)->SetWindowText(g_pLang->cstrGet_text(ITEM_LAST_NAME));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    }
  }
