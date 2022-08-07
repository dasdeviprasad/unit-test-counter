/********************************************************************
Medications.cpp

Copyright (C) 2003,2004,2008,2009,2012,2016,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CMedications class.

  This class provides a screen for entering/editing/viewing patient
  medications.


HISTORY:
03-OCT-03  RET  New.
16-MAR-04  RET  Add help button and message handler.
30-JUN-08 RET  Version 2.05
                 Disabled "Go to Study" button until a study is selected.
14-JAN-09  RET  Version CDDRdr.2.08c
                  Display only if view mode.  Change Constructor.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
09-JUN-16  RET
             Add feature to hide study dates.
               Don't display the date if its encoded (i.e. too long)
                 Change method: OnInitDialog() 
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedMedicationsHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "Dbaccess.h"
#include "Medications.h"
#include "util.h"
#include ".\medications.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMedications dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the currently active document.
          Indicates why this object is being used (new study, edit, view).
          Pointer to parent window (default is NULL).
********************************************************************/
CMedications::CMedications(CEGGSASDoc *pDoc, unsigned uFrom, CWnd* pParent /*=NULL*/)
	: CDialog(CMedications::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CMedications)
	m_cstrMedication = _T("");
	m_cstrDate = _T("");
	//}}AFX_DATA_INIT

#if EGGSAS_READER == READER_DEPOT
  m_uFrom = SM_VIEW;
#else
  m_uFrom = uFrom;
#endif
  m_pDoc = pDoc;
  }


void CMedications::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMedications)
	DDX_Control(pDX, IDC_MEDICATION_LIST, m_cMedicationList);
	DDX_Text(pDX, IDC_MEDICATION, m_cstrMedication);
	DDX_Text(pDX, IDC_MED_DATE, m_cstrDate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMedications, CDialog)
	//{{AFX_MSG_MAP(CMedications)
	ON_NOTIFY(NM_CLICK, IDC_MEDICATION_LIST, OnClickMedicationList)
	ON_BN_CLICKED(IDC_MED_UPDATE, OnMedUpdate)
	ON_BN_CLICKED(IDC_MED_GOTO_STUDY, OnMedGotoStudy)
	ON_NOTIFY(NM_DBLCLK, IDC_MEDICATION_LIST, OnDblclkMedicationList)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_MEDICATIONS_HELP, OnBnClickedMedicationsHelp)
//  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMedications message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Add columns and titles for the medication list.
    Modify the fields for editing/viewing.
********************************************************************/
BOOL CMedications::OnInitDialog() 
  {
  HDITEM hdiHdrItem;
  CString cstrItem1, cstrItem2;
  RECT rRect;
  short int iStudyPos, iStudySts;
  int iIndex, iCnt, iSelIndex;
  long lStudyID;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();
  if(m_uFrom == SM_EDIT || m_uFrom == SM_VIEW)
    {
    if(m_uFrom == SM_VIEW)
      { // In View mode, nothing is changeable.
      GetDlgItem(IDC_MED_UPDATE)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_MEDICATION)->EnableWindow(FALSE);
        // Horizontally center the OK button.
      vCenter_button_horiz(this, GetDlgItem(IDC_MED_GOTO_STUDY));
      }
    }
    // insert two columns (REPORT mode) and modify the new header items
  //cstrItem1.LoadString(IDS_DATE_OF_STUDY);
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_DATE_OF_STUDY);
  //cstrItem2.LoadString(IDS_MEDICATION);
  cstrItem2 = g_pLang->cstrLoad_string(ITEM_G_MEDICATION);
  m_cMedicationList.GetWindowRect(&rRect);
  m_cMedicationList.InsertColumn(0, cstrItem1, LVCFMT_LEFT,
                                rRect.right - rRect.left - 100, 0);
  m_cMedicationList.InsertColumn(1, cstrItem2, LVCFMT_LEFT, 100, 1);
    // Now add the header items.
  CHeaderCtrl* pHeaderCtrl = m_cMedicationList.GetHeaderCtrl();
  hdiHdrItem.mask = HDI_FORMAT | HDI_TEXT | HDI_WIDTH;
  hdiHdrItem.pszText = cstrItem1.GetBuffer(cstrItem1.GetLength());
  hdiHdrItem.cchTextMax = strlen(hdiHdrItem.pszText);
  hdiHdrItem.fmt = HDF_LEFT | HDF_STRING; // HDF_CENTER
  hdiHdrItem.cxy = 100; // Make all columns 100 pixels wide.
  pHeaderCtrl->SetItem(0, &hdiHdrItem);

  hdiHdrItem.pszText = cstrItem2.GetBuffer(cstrItem2.GetLength());
  hdiHdrItem.cchTextMax = strlen(hdiHdrItem.pszText);
  hdiHdrItem.cxy = rRect.right - rRect.left - 100; // Use all of window except for date.
  pHeaderCtrl->SetItem(1, &hdiHdrItem);
    // Fill in the list box from the database.
  if(m_pDoc->m_pDB != NULL)
    { // Fill in the list box from the database.
    iCnt = 0;
    iStudySts = SUCCESS;
    iStudyPos = FIRST_RECORD;
    while(iStudySts == SUCCESS)
      {
      if((iStudySts = m_pDoc->m_pDB->iFind_next_medication(iStudyPos, cstrItem1,
      cstrItem2, &lStudyID, m_pDoc->m_pdPatient.lPatientID)) == SUCCESS)
        {
        if(cstrItem1.GetLength() <= 10)
          iIndex = m_cMedicationList.InsertItem(iCnt, cstrItem1); // insert new item, date of study
        else
          iIndex = m_cMedicationList.InsertItem(iCnt, ""); // insert new item, empty date
        m_cMedicationList.SetItemText(iIndex, 1, cstrItem2); // diagnosis
        m_cMedicationList.SetItemData(iIndex, (DWORD)lStudyID); // Study ID
        if(lStudyID == m_pDoc->m_pdPatient.lStudyID)
          iSelIndex = iIndex;
        ++iCnt;
        }
      iStudyPos = NEXT_RECORD;
      }
    }
  GetDlgItem(IDC_MED_GOTO_STUDY)->EnableWindow(FALSE);
  Invalidate();
  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }
/********************************************************************
OnMedUpdate

  Message handler for the Replace medication button.
    Replace the selected medication from the list with the medication
    in the edit window.
    Update the medication in the database.
********************************************************************/
void CMedications::OnMedUpdate() 
  {
  CString cstrDate;
  int iSel;

  UpdateData(TRUE);
  iSel = m_cMedicationList.GetSelectionMark();
  if(iSel >= 0)
    {
    m_lStudyID = (long)m_cMedicationList.GetItemData(iSel); // Study ID
      // Update the database.
    if(m_pDoc->m_pDB != NULL)
      {
      if(m_pDoc->m_pDB->iUpdate_medication(m_lStudyID, m_cstrMedication) == SUCCESS)
        { // Update the list.
        m_cMedicationList.DeleteItem(iSel);
        iSel = m_cMedicationList.InsertItem(iSel, m_cstrDate); // insert new item
        m_cMedicationList.SetItemText(iSel, 1, m_cstrMedication);
        m_cMedicationList.SetItemData(iSel, (DWORD)m_lStudyID); // Study ID
        m_cMedicationList.SetSelectionMark(iSel);
        m_cMedicationList.SetItemState(iSel, LVIS_SELECTED, LVIS_SELECTED);
        m_cMedicationList.SetFocus();
        }
      }
    }
  UpdateData(FALSE);
  }

/********************************************************************
OnClickMedicationList

  Message handler for clicking on an item in the medication list.
    Place the selected medication and the date in the appropriate fields
    for editing.
********************************************************************/
void CMedications::OnClickMedicationList(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  int iSel;

  UpdateData(TRUE);
  iSel = m_cMedicationList.GetSelectionMark();
  if(iSel >= 0)
    {
    GetDlgItem(IDC_MED_GOTO_STUDY)->EnableWindow(TRUE);
    m_cstrMedication = m_cMedicationList.GetItemText(iSel, 1);
    m_cstrDate = m_cMedicationList.GetItemText(iSel, 0);
    m_lStudyID = (long)m_cMedicationList.GetItemData(iSel); // Study ID
    }
  UpdateData(FALSE);

	*pResult = 0;
  }

/********************************************************************
OnOK

  Message handler for the OK button.
    Set Study ID to invalid so caller won't go to the study.
********************************************************************/
void CMedications::OnOK() 
  {

	// TODO: Add extra validation here
  m_lStudyID = INV_LONG;
	
	CDialog::OnOK();
  }

/********************************************************************
OnMedGotoStudy

  Message handler for the Go to Study button.
    Leave Study ID valid so caller will go to the study.
    Close the dialog box.
********************************************************************/
void CMedications::OnMedGotoStudy() 
  {
	
	CDialog::OnOK();
  }

/********************************************************************
OnDblclkMedicationList

  Message handler for double clicking on an item in the medication list.
    Get the study ID from the medication list item.
    Close the dialog box.
********************************************************************/
void CMedications::OnDblclkMedicationList(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  int iSel;

	// TODO: Add your control notification handler code here
  UpdateData(TRUE);
  iSel = m_cMedicationList.GetSelectionMark();
  if(iSel >= 0)
    {
    m_lStudyID = (long)m_cMedicationList.GetItemData(iSel); // Study ID
    CDialog::OnOK();
    }
	
  *pResult = 0;
  }

/********************************************************************
OnBnClickedMedicationsHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CMedications::OnBnClickedMedicationsHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_MEDICATIONS_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_MEDICATIONS_HELP);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CMedications::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_MEDICATIONS);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_M_MEDICATIONS));

    GetDlgItem(IDC_DATE_TITLE)->SetWindowText(g_pLang->cstrGet_text(ITEM_M_DATE));
    GetDlgItem(IDC_MEDICATION_TITLE)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_MEDICATION));
    GetDlgItem(IDC_MED_UPDATE)->SetWindowText(g_pLang->cstrGet_text(ITEM_M_UPD_MED));
    GetDlgItem(IDC_MED_GOTO_STUDY)->SetWindowText(g_pLang->cstrGet_text(ITEM_M_GO_TO_STUDY));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    GetDlgItem(IDC_MEDICATIONS_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
   }
  }

