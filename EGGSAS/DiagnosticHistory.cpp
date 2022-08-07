/********************************************************************
DiagnosticHistory.cpp

Copyright (C) 2003,2004,2009,2012,2016,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CDiagnosticHistory class.

  This class provides a screen for entering/editing/viewing patient
  diagnositc history.


HISTORY:
03-OCT-03  RET  New.
16-MAR-04  RET  Add help button and message handler.
14-JAN-09  RET  Version CDDRdr.2.08c
                  Display only if view mode.  Change Constructor.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
09-JUN-16  RET
             Add feature to hide study dates.
               Don't display the date if its encoded (i.e. too long)
                 Change method: OnInitDialog() 
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedDiaghistHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "Dbaccess.h"
#include "DiagnosticHistory.h"
#include "util.h"
#include ".\diagnostichistory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiagnosticHistory dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the currently active document.
          Where the diaglog box is being called from (edit, view, etc).
********************************************************************/
CDiagnosticHistory::CDiagnosticHistory(CEGGSASDoc *pDoc, unsigned uFrom, CWnd* pParent /*=NULL*/)
	: CDialog(CDiagnosticHistory::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CDiagnosticHistory)
	m_cstrDiagnosis = _T("");
	m_cstrDate = _T("");
	//}}AFX_DATA_INIT

  m_pDoc = pDoc;
#if EGGSAS_READER == READER_DEPOT
  m_uFrom = SM_VIEW;
#else
  m_uFrom = uFrom;
#endif
  }


void CDiagnosticHistory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiagnosticHistory)
	DDX_Control(pDX, IDC_DIAGNOSIS_LIST, m_cDiagnosisList);
	DDX_Text(pDX, IDC_DIAGNOSIS, m_cstrDiagnosis);
	DDX_Text(pDX, IDC_DATE, m_cstrDate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDiagnosticHistory, CDialog)
	//{{AFX_MSG_MAP(CDiagnosticHistory)
	ON_NOTIFY(NM_CLICK, IDC_DIAGNOSIS_LIST, OnClickDiagnosisList)
	ON_NOTIFY(NM_DBLCLK, IDC_DIAGNOSIS_LIST, OnDblclkDiagnosisList)
	ON_BN_CLICKED(IDC_GOTO_STUDY, OnGotoStudy)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_DIAGHIST_HELP, OnBnClickedDiaghistHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiagnosticHistory message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Add columns and column titles for the diagnosis history list.
    Modify the fields for editing/viewing.
********************************************************************/
BOOL CDiagnosticHistory::OnInitDialog() 
  {
  HDITEM hdiHdrItem;
  CString cstrItem1, cstrItem2;
  RECT rRect;
  short int iStudyPos, iStudySts;
  int iIndex, iCnt;
  long lStudyID;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();
    // insert two columns (REPORT mode) and modify the new header items
  //cstrItem1.LoadString(IDS_DATE_OF_STUDY);
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_DATE_OF_STUDY);
  //cstrItem2.LoadString(IDS_DIAGNOSIS);
  cstrItem2 = g_pLang->cstrLoad_string(ITEM_G_DIAGNOSIS);
  m_cDiagnosisList.GetWindowRect(&rRect);
  m_cDiagnosisList.InsertColumn(0, cstrItem1, LVCFMT_LEFT,
                                rRect.right - rRect.left - 100, 0);
  m_cDiagnosisList.InsertColumn(1, cstrItem2, LVCFMT_LEFT, 100, 1);
    // Now add the header items.
  CHeaderCtrl* pHeaderCtrl = m_cDiagnosisList.GetHeaderCtrl();
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
      if((iStudySts = m_pDoc->m_pDB->iFind_next_diagnosis(iStudyPos, cstrItem1,
      cstrItem2, &lStudyID, m_pDoc->m_pdPatient.lPatientID)) == SUCCESS)
        {
        if(cstrItem1.GetLength() <= 10)
          iIndex = m_cDiagnosisList.InsertItem(iCnt, cstrItem1); // insert new item, date of study
        else
          iIndex = m_cDiagnosisList.InsertItem(iCnt, ""); // insert new item, empty date
        m_cDiagnosisList.SetItemText(iIndex, 1, cstrItem2); // diagnosis
        m_cDiagnosisList.SetItemData(iIndex, (DWORD)lStudyID); // Study ID
        ++iCnt;
        }
      iStudyPos = NEXT_RECORD;
      }
    }

  Invalidate();
  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnGotoStudy

  Message handler for the Go To Study button.
    Exit the dialog box.  This dialog box has the study ID which the
    caller uses to load the selected study.
********************************************************************/
void CDiagnosticHistory::OnGotoStudy() 
  {

	CDialog::OnOK();
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.
********************************************************************/
void CDiagnosticHistory::OnCancel() 
  {
	
	CDialog::OnCancel();
  }

/********************************************************************
OnOK

  Message handler for the OK button.
    Set study ID to invalid, so the caller knows not to load another study.
********************************************************************/
void CDiagnosticHistory::OnOK() 
  {

  m_lStudyID = INV_LONG;
	CDialog::OnOK();
  }

/********************************************************************
OnClickDiagnosisList

  Message handler for selecting a diagnosis with a mouse click.
    Place the selected diagnosis and the date in the appropriate fields
    for editing.
********************************************************************/
void CDiagnosticHistory::OnClickDiagnosisList(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  int iSel;

  UpdateData(TRUE);
  iSel = m_cDiagnosisList.GetSelectionMark();
  if(iSel >= 0)
    {
    m_cstrDiagnosis = m_cDiagnosisList.GetItemText(iSel, 1);
    m_cstrDate = m_cDiagnosisList.GetItemText(iSel, 0);
    m_lStudyID = (long)m_cDiagnosisList.GetItemData(iSel); // Study ID
    GetDlgItem(IDC_GOTO_STUDY)->EnableWindow(TRUE);
    }
  UpdateData(FALSE);

	*pResult = 0;
  }

/********************************************************************
OnDblclkDiagnosisList

  Message handler for double clicking a diagnosis.
    Get the selected diagnosis.
    Exit the dialog box.  This dialog box has the study ID which the
      caller uses to load the selected study.
********************************************************************/
void CDiagnosticHistory::OnDblclkDiagnosisList(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  int iSel;

	// TODO: Add your control notification handler code here
  UpdateData(TRUE);
  iSel = m_cDiagnosisList.GetSelectionMark();
  if(iSel >= 0)
    {
    m_lStudyID = (long)m_cDiagnosisList.GetItemData(iSel); // Study ID
    CDialog::OnOK();
    }
	
  *pResult = 0;
  }

/********************************************************************
OnBnClickedDiaghistHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CDiagnosticHistory::OnBnClickedDiaghistHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_DIAGHIST_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_DIAGHIST_HELP);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CDiagnosticHistory::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_DIAGNOSTIC_HISTORY);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_DH_DIAG_HIST));

    GetDlgItem(IDC_DATE_TITLE)->SetWindowText(g_pLang->cstrGet_text(ITEM_DH_DATE));
    GetDlgItem(IDC_DIAGNOSIS_TITLE)->SetWindowText(g_pLang->cstrGet_text(ITEM_DH_DIAGNOSIS));
    GetDlgItem(IDC_GOTO_STUDY)->SetWindowText(g_pLang->cstrGet_text(ITEM_DH_GO_TO_STUDY));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_DIAGHIST_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    //g_pLang->vEnd_section();
    }
  }
