/********************************************************************
BckupRestErrs.cpp

Copyright (C) 2009,2012, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CBckupRestErrs class.

  This class displays errors encountered during a backup or restore.

HISTORY:
22-MAR-09  RET  New. Version 2.08k
16-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "BckupRest.h"
#include "BckupRestErrs.h"


// CBckupRestErrs dialog

IMPLEMENT_DYNAMIC(CBckupRestErrs, CDialog)
/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the dialogbox that called this.
            Backup/restore dialog box.

********************************************************************/
CBckupRestErrs::CBckupRestErrs(CWnd* pParent /*=NULL*/)
	: CDialog(CBckupRestErrs::IDD, pParent)
{

m_pBckupRest = (CBckupRest *)pParent;
}

CBckupRestErrs::~CBckupRestErrs()
{
}

void CBckupRestErrs::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Control(pDX, IDC_LIST_ERRORS, m_cBckupRestList);
}


BEGIN_MESSAGE_MAP(CBckupRestErrs, CDialog)
END_MESSAGE_MAP()


// CBckupRestErrs message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    - Display the dialog box title based on whether backing up or restoring.
    - Display number of errors based on whether backing up or restoring.
    - Fill in the error listbox.
********************************************************************/
BOOL CBckupRestErrs::OnInitDialog()
  {
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  vSet_text();
  CWnd *pcwnd = GetDlgItem(IDC_STATIC_NUM_ERRS);
  CString cstrErrTitle, cstrWinTitle;
  int iArrayLen, iIndex;

  iArrayLen = m_pBckupRest->m_cstraErrs.GetCount();
    // Add appropriate title to the dialog box.
    // Add number of errors.
  if(m_pBckupRest->m_iBckupRest == BACKUP)
    { // Title for backup.
    //cstrWinTitle.LoadString(IDS_BACKUP_ERRORS);
    //cstrErrTitle.Format(IDS_BACKUP_ERRORS_NUM, iArrayLen);
    cstrWinTitle = g_pLang->cstrLoad_string(ITEM_G_BCKUP_ERRS);
    cstrErrTitle.Format(g_pLang->cstrLoad_string(ITEM_G_BCKUP_ERRS_NUM), iArrayLen);
    }
  else
    { // Title for restore.
    //cstrWinTitle.LoadString(IDS_RESTORE_ERRORS);
    //cstrErrTitle.Format(IDS_RESTORE_ERRORS_NUM, iArrayLen);
    cstrWinTitle = g_pLang->cstrLoad_string(ITEM_G_RESTORE_ERRS);
    cstrErrTitle.Format(g_pLang->cstrLoad_string(ITEM_G_RESTORE_ERRS_NUM), iArrayLen);
    }
  SetWindowText(cstrWinTitle);
  pcwnd->SetWindowText(cstrErrTitle);
    // Fill in the error listbox.
  for(iIndex = 0; iIndex < iArrayLen; ++iIndex)
    vAdd_to_msg_list(m_pBckupRest->m_cstraErrs[iIndex]);
  m_cBckupRestList.SetCurSel(0);

  UpdateData(FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
vAdd_to_msg_list - Add a new message to the Error listbox.
                   Update the horizontal scroll bar.

  inputs: String containing the new message.

  Returns: None.
********************************************************************/
void CBckupRestErrs::vAdd_to_msg_list(CString cstrLine)
  {
  CDC *pcdcLb;
  CSize csExt;

    // Make sure the horizontal scroll bar is displayed if it needs to be.
  pcdcLb = GetDC();
  csExt = pcdcLb->GetTextExtent(cstrLine);
  if(csExt.cx > m_cBckupRestList.GetHorizontalExtent())
    { // Have a new longest line.
      // Tell the listbox about it so it can display the horizontal
      // scroll bar if it needs to and update how much to scroll.
    csExt.cx *= 2;
    m_cBckupRestList.SetHorizontalExtent(csExt.cx);
    }
  ReleaseDC(pcdcLb);
  m_cBckupRestList.AddString(cstrLine);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CBckupRestErrs::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_BACKUP_RESTORE);
    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    }
  }
