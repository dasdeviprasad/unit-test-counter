/********************************************************************
SelectPatient.cpp

Copyright (C) 2003 - 2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CSelectPatient class.

  This class provides for selection of an existing patient.

  If the user is selecting a patient study for viewing, then each
    study date for each patient is listed.
  If the user is selecting a patient for a new study, then only
    patient names are listed.

HISTORY:
03-OCT-03  RET  New.
16-MAR-04  RET  Add help button and message handler.
06-FEB-05  RET  Changes to use the research reference number.  If the
                  reference number is used instead of the name.
                  Changed methods:
                    vFill_patient_listbox(), vGet_patient_info()
10-FEB-05  RET  Changes for middle initial:
                  vFill_patient_listbox(), vGet_selection_data(),
                  OnOK(), vGet_patient_info()
                  Add method: cstrFormat_name().
26-MAY-07  RET  Research Version
                  Changes for displaying studies to be resumed.
                    Change methods: OnInitDialog(), vFill_patient_listbox()
30-JUN-08 RET  Version 2.05
                 Add WhatIf scenarios to the select patient dialog box.
                   Change vFill_patient_listbox(), vGet_patient_info(),
                   vGet_selection_data().
28-AUG-08  RET  Version 2.05c
                  Change so that research reference numbers are read from
                    the Patient Name database table and listed in
                    alphabetical order.
                    Change method: vFill_patient_listbox().
//// REMOTE DB
                 Implement using databases from other places.
                   Change dialog box to display the path and file name 
                   of the database.
                       Add field variable m_cstrDBFile.
                   Change OnInitDialog() to display the initial database
                     path and file name.
                   Change OnAlternateDb() to set up the database path in
                     the database class.
//// END OF REMOTE DB

07-N0V-08  RET  Version 2.08a
                  Modifications for the Reader program version.
                    Add buttons for selecting the local database or the
                      the reader database.
                      Add button handlers for the buttons:
                        OnBnClickedBtnSelectReaderDb(),
                        OnBnClickedBtnSelectLocalDb()
                    Modify OnInitDialog() to display/hide appropriate buttons.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
27-FEB-13  RET
             Changes for hiding patients.
               Add parameter to constructor to either view hidden or active patients.
               Add class variable: m_bHiddenPatients
               Change method: vFill_patient_listbox(), vGet_patient_info()
08-JUN-16  RET
             Add feature to hide study dates.
               Change methods: vFill_patient_listbox(), OnOK(), OnCancel(),
                 vGet_selection_data()
               Add method: vFree_data()
               Add variable: m_cstraStudyIDs
20-JAN-18  RET
             Provide search criteria for listing patients.
               Add search button to the form.
               Add event handler: OnBnClickedBtnSearch()
16-MAY-20  RET
             When selecting an existing patient to start a new study, program
             gets an error.  This was because it was looking for a date from
             the select patient list and there is no date because study dates
             are hidden.  Added a check that the variable m_cstraStudyIDs has
             enough items that the index of the selected patient into this
             array is valid.
06-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedSelectpatientHelp()
12-SEP-20  RET Version 2.09h
             CDDR version.  When a WhatIf study is in the study list and dates are
             hidden, any study selected after the WhatIf doesn't get displayed.  This was
             because a separate internal list of study dates is kept (so that the study
             dates are known regardless of whether or not the dates are displayed) and the
             WhatIf study date was not included in the date list.  When dates are hidden
             then the study does not have the correct date and so doesn't get displayed.
               Change method: vFill_patient_listbox()
16-OCT-20  RET Version 2.09i
             Change selecting an alternate database so that all versions work the
             same as the Reader.
               Change method: OnAlternateDb() 
14-NOV-20  RET
             Add ability to work with a remote database.
             Changed settings to allow the user to specify the remote database.
               Change method: OnAlternateDb()
15-NOV-20  RET
             Add new version CDDRE
               Change method: vFill_patient_listbox()
16-NOV-20  RET
             Add new version CDDRE
               Change methods: vFill_patient_listbox()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "Dbaccess.h"
#include "SelectPatient.h"
#include ".\selectpatient.h"
#include "util.h"
#include "SearchDB.h"
#include "TenMinView.h"
//#include ".\selectpatient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectPatient dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the active document.
          Type of selection (Perform study on existing patient, or just
            opening a patient file)
********************************************************************/
CSelectPatient::CSelectPatient(CEGGSASDoc *pDoc, short int iStudyType, BOOL bHiddenPatients, CWnd* pParent /*=NULL*/)
	: CDialog(CSelectPatient::IDD, pParent)
  , m_cstrDBFile(_T(""))
  //, m_plStudyIDs(NULL)
  {
	//{{AFX_DATA_INIT(CSelectPatient)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  m_pDoc = pDoc;
  m_iStudyType = iStudyType;
  m_bHiddenPatients = bHiddenPatients;
  }


void CSelectPatient::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
//{{AFX_DATA_MAP(CSelectPatient)
DDX_Control(pDX, IDC_PATIENT_DB_LIST, m_cPatientDBList);
//}}AFX_DATA_MAP
DDX_Text(pDX, IDC_STATIC_DB_FILE, m_cstrDBFile);
}


BEGIN_MESSAGE_MAP(CSelectPatient, CDialog)
	//{{AFX_MSG_MAP(CSelectPatient)
	ON_BN_CLICKED(IDC_ALTERNATE_DB, OnAlternateDb)
	ON_NOTIFY(NM_DBLCLK, IDC_PATIENT_DB_LIST, OnDblclkPatientDbList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_PATIENT_DB_LIST, OnColumnclickPatientDbList)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_SELECTPATIENT_HELP, OnBnClickedSelectpatientHelp)
  ON_BN_CLICKED(IDC_BTN_SELECT_READER_DB, OnBnClickedBtnSelectReaderDb)
  ON_BN_CLICKED(IDC_BTN_SELECT_LOCAL_DB, OnBnClickedBtnSelectLocalDb)
  ON_BN_CLICKED(IDC_BTN_SEARCH, OnBnClickedBtnSearch)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectPatient message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Set the titles for the list box columns.
    Read existing patients from the database and put in the list.
********************************************************************/
BOOL CSelectPatient::OnInitDialog() 
  {
  HDITEM hdiHdrItem;
  CString cstrItem1, cstrItem2;
  RECT rRect;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();

  vFree_data();

//#ifdef EGGSAS_READER
#if EGGSAS_READER > READER_NONE
  GetDlgItem(IDC_ALTERNATE_DB)->ShowWindow(SW_HIDE);
  if(m_pDoc->m_pDB->m_iConnType == DB_CONN_LOCAL)
      m_cstrDBFile = g_pLang->cstrLoad_string(ITEM_G_LOCAL_DB);
    //m_cstrDBFile.LoadString(IDS_LOCAL_DB);
  else
    {
    m_cstrDBFile = m_pDoc->m_pDB->cstrGet_db_file_path();
    if(m_cstrDBFile.IsEmpty() == TRUE)
      m_cstrDBFile = g_pLang->cstrLoad_string(ITEM_G_SEL_RDR_LOC_DB);
      //m_cstrDBFile.LoadString(IDS_SEL_READER_DB);
    }
#else
  GetDlgItem(IDC_BTN_SELECT_LOCAL_DB)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_BTN_SELECT_READER_DB)->ShowWindow(SW_HIDE);
#endif
    // insert two columns (REPORT mode) and modify the new header items
  //cstrItem1.LoadString(IDS_PATIENT_NAME);
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_PATIENT_NAME);
  if(m_iStudyType == STUDY_NONE || m_iStudyType == STUDY_RESUME)
    cstrItem2 = g_pLang->cstrLoad_string(ITEM_G_DATE_OF_STUDY);
    //cstrItem2.LoadString(IDS_DATE_OF_STUDY);
  m_cPatientDBList.GetWindowRect(&rRect);
  m_cPatientDBList.InsertColumn(0, cstrItem1, LVCFMT_LEFT,
                                rRect.right - rRect.left - 100, 0);
  if(m_iStudyType == STUDY_NONE || m_iStudyType == STUDY_RESUME)
    m_cPatientDBList.InsertColumn(1, cstrItem2, LVCFMT_LEFT, 100, 1);
    // Now add the header items.
  CHeaderCtrl* pHeaderCtrl = m_cPatientDBList.GetHeaderCtrl();
  hdiHdrItem.mask = HDI_FORMAT | HDI_TEXT | HDI_WIDTH;
  hdiHdrItem.pszText = cstrItem1.GetBuffer(cstrItem1.GetLength());
  hdiHdrItem.cchTextMax = strlen(hdiHdrItem.pszText);
  hdiHdrItem.cxy = rRect.right - rRect.left - 100; // Use all of window except for date.
  hdiHdrItem.fmt = HDF_LEFT | HDF_STRING; // HDF_CENTER
  pHeaderCtrl->SetItem(0, &hdiHdrItem);

  if(m_iStudyType == STUDY_NONE || m_iStudyType == STUDY_RESUME)
    { // This is not for a study, label the second column.
    hdiHdrItem.pszText = cstrItem2.GetBuffer(cstrItem2.GetLength());
    hdiHdrItem.cchTextMax = strlen(hdiHdrItem.pszText);
    hdiHdrItem.cxy = 100; // Make all columns 100 pixels wide.
    pHeaderCtrl->SetItem(1, &hdiHdrItem);
    }

    // Allows full row to show selection.  Also allows mouse click
    // for selection to be anywhere on the row.
  m_cPatientDBList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    // Fill the listbox with names and dates from the database.
  if(m_pDoc->m_pDB != NULL)
    {
//#ifndef EGGSAS_READER
    vFill_patient_listbox();
//#endif
//// REMOTE DB
    //vDisplay_db_file();
    }

  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }


/********************************************************************
OnBnClickedBtnSelectReaderDb

  Message handler for the select reader database button.

  - Clear out the patient list.
  - Display a file open dialog box.
  - Fill in patient list from the reader database.
  - Change databases to the reader database.
********************************************************************/
void CSelectPatient::OnBnClickedBtnSelectReaderDb()
  {
  CString cstrDB;

    // Display a browse window for selecting location of database.
  cstrDB = m_pDoc->cstrSelect_reader_db();
  if(cstrDB.IsEmpty() == FALSE)
    {
    vFree_data();
    m_cstrDBFile = cstrDB;
    m_pDoc->vChange_db(IMPORT_DB, m_cstrDBFile);
    vFill_patient_listbox();
    }
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedBtnSelectLocalDb

  Message handler for the select local database button.

  - Clear out the patient list.
  - Fill in patient list from the local database.
  - Change databases to the local database.
********************************************************************/
void CSelectPatient::OnBnClickedBtnSelectLocalDb()
  {
  CString cstrItem;

  vFree_data();

  // If using a remote database, switch to the remote instead of local.
  cstrItem = g_pConfig->cstrGet_item(CFG_REMOTE_DB_PASSWD);
  if(cstrItem.IsEmpty() == true)
    m_pDoc->vChange_db(LOCAL_DB, "");
  else
    {
    cstrItem = g_pConfig->cstrGet_item(CFG_REMOTE_DB_PATH);
    m_pDoc->vChange_db(IMPORT_DB, cstrItem);
    }

  //m_cstrDBFile.LoadString(IDS_LOCAL_DB);
  m_cstrDBFile = g_pLang->cstrLoad_string(ITEM_G_LOCAL_DB);
  vFill_patient_listbox();

  UpdateData(FALSE);
  }

/********************************************************************
OnAlternateDb

  Message handler for the select alternate database button.
********************************************************************/
void CSelectPatient::OnAlternateDb() 
  {
/*****************************************/
  //CString cstrTitle;
  CString cstrItem;

  //cstrTitle.LoadString(IDS_INFORMATION);
  //cstrTitle = g_pLang->cstrLoad_string(ITEM_G_INFORMATION);
  // Find out if this operation is allowed.
  cstrItem = g_pConfig->cstrGet_item(CFG_REMOTE_DB_PASSWD);
  if(cstrItem.IsEmpty() == true)
    MessageBox(g_pLang->cstrLoad_string(ITEM_G_REQ_ACTIVATION), g_pLang->cstrLoad_string(ITEM_G_INFORMATION));
  else
    MessageBox(g_pLang->cstrLoad_string(ITEM_G_CHANGE_IN_SETTINGS), g_pLang->cstrLoad_string(ITEM_G_INFORMATION));


/***********************************************/

//// REMOTE DB
  //OnBnClickedBtnSelectReaderDb(); // Version 2.09i
/*****************************************/
//  CFileDialog *pdlg;
//  CString cstrDBFileName, cstrPath;
//  int iIndex;
//
//     // Create an open file dialog box. Operator can select only a single file.
//  pdlg = new CFileDialog(TRUE, ".mdb", NULL, 
//                         OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, 
//                         "Patient databases (*.mdb)|*.mdb|All Files (*.*)|*.*||", this);
//  if(pdlg->DoModal() == IDOK)
//    { // User selected a database.
//      // Get the database name without the extension.
//    cstrDBFileName = pdlg->GetFileTitle();
//    cstrPath = pdlg->GetPathName();
//    if((iIndex = cstrPath.ReverseFind('\\')) > 0)
//      cstrPath = cstrPath.Left(iIndex);
//      // Make a new database connection.
//    m_pDoc->m_pDB->sqlrDB_disconnect();
//    m_pDoc->m_pDB->vSet_DSN(cstrDBFileName);
//    m_pDoc->m_pDB->vSet_alt_db_path(cstrPath);
//    m_pDoc->m_pDB->sqlrDB_connect();
//      // Clear out the patient listbox and refill it from the
//      // new database.
//    m_cPatientDBList.DeleteAllItems();
//    vFill_patient_listbox();
////// REMOTE DB
//    vDisplay_db_file();
//    UpdateData(FALSE);
//    // Show a blank screen with the logo just like when the program first starts.
//    m_pDoc->m_p10MinView->vShow_standard_fields(false);
//    m_pDoc->m_p10MinView->vShow_research_fields(false);
//    m_pDoc->m_p10MinView->vInit();
//    m_pDoc->m_p10MinView->GetDlgItem(IDC_LOGO_3CPM)->ShowWindow(TRUE);
//
//    }
//  delete pdlg;
/***********************************************/
  }

/********************************************************************
OnOK

  Message handler for the OK button.

  Get the name and date from the listbox for the selected patient.
********************************************************************/
void CSelectPatient::OnOK() 
  {
  int iSel;

  UpdateData(TRUE);
  iSel = m_cPatientDBList.GetSelectionMark();
  if(iSel >= 0)
    { // Patient name has been selected, get the name and date.
    vGet_selection_data(iSel);
    vFree_data();
  	CDialog::OnOK();
    }
  else
    { // Nothing has been selected.  Exit the dialog box with Cancel.
    m_cstrLastName.Empty();
    m_cstrFirstName.Empty();
    m_cstrMiddleInitial.Empty();
    m_cstrDate.Empty();
    vFree_data();
    CDialog::OnCancel();
    }
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.
********************************************************************/
void CSelectPatient::OnCancel() 
  {

  vFree_data();
  CDialog::OnCancel();
  }

/********************************************************************
OnDblclkPatientDbList

  Message handler for double clicking a patient name.
    Get the patient name and date and then exit the dialog box.
      Same as single clicking a patient name and selecting the OK button.
********************************************************************/
void CSelectPatient::OnDblclkPatientDbList(NMHDR* pNMHDR, LRESULT* pResult) 
  {
  int iSel;

	*pResult = 0;
  UpdateData(TRUE);
  iSel = m_cPatientDBList.GetSelectionMark();
  if(iSel >= 0)
    {
    vGet_selection_data(iSel);
    CDialog::OnOK();
    }
  }

/********************************************************************
OnColumnclickPatientDbList

  Message handler for clicking a column title.
    This will reorder the list according to the column selected.
********************************************************************/
void CSelectPatient::OnColumnclickPatientDbList(NMHDR* pNMHDR, LRESULT* pResult) 
  {

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
  }

/********************************************************************
OnBnClickedSelectpatientHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CSelectPatient::OnBnClickedSelectpatientHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_SELECTPATIENT_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_SELECTPATIENT_HELP);
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
vGet_patient_info - Get the entered patient information.

  inputs: Pointer to a PATIENT_DATA structure that gets the patient information.
          Pointer to a long that gets the WhatIF ID.  INV_LONG if there is none.

  return: None.
********************************************************************/
void CSelectPatient::vGet_patient_info(PATIENT_DATA *pdInfo, long *plWhatIfID)
  {
  PATIENT_DATA pdInfoTemp;

  pdInfoTemp.lPatientID = m_lPatientID;
  if(m_pDoc->m_pDB->iGet_patient(&pdInfoTemp) == SUCCESS)
    {
    pdInfo->cstrLastName = pdInfoTemp.cstrLastName;
    pdInfo->cstrFirstName = pdInfoTemp.cstrFirstName;
    pdInfo->cstrMI = pdInfoTemp.cstrMI;
    pdInfo->bUseResrchRefNum = pdInfoTemp.bUseResrchRefNum;
    pdInfo->cstrResearchRefNum = pdInfoTemp.cstrResearchRefNum;
    pdInfo->bHide = pdInfoTemp.bHide;
    }
  else
    {
    pdInfo->cstrLastName = m_cstrLastName;
    pdInfo->cstrFirstName = m_cstrFirstName;
    pdInfo->cstrMI = m_cstrMiddleInitial;
    pdInfo->bUseResrchRefNum = FALSE;
    pdInfo->cstrResearchRefNum = "";
    pdInfo->bHide = FALSE;
    }
  pdInfo->cstrDate = m_cstrDate;
  pdInfo->lPatientID = m_lPatientID;
  *plWhatIfID = m_lWhatIfID;
  }

/********************************************************************
vGet_selection_data - Get the patient name and date from the listbox.
                      The name is separated into first and last.

  inputs: Index of selected item in the listbox.

  return: None.
********************************************************************/
void CSelectPatient::vGet_selection_data(int iSel)
  {
  CString cstrName;
  WHAT_IF wi;
  PATIENT_DATA pdInfo;
  int iIndex;

  cstrName = m_cPatientDBList.GetItemText(iSel, 0);
  if(cstrName.Find(WHATIF_PREFIX) < 0)
    { // Study
    m_lWhatIfID = INV_LONG;
      // Parse the name into first and last name.
    if((iIndex = cstrName.Find(',')) > 0)
      {
      m_cstrLastName = cstrName.Left(iIndex);
        // Now get just the first name and middle initial.
      cstrName = cstrName.Right(cstrName.GetLength() - iIndex - 2);
        // A space separates the first name from the middle initial.
      if((iIndex = cstrName.Find(' ')) > 0)
        {
        m_cstrFirstName = cstrName.Left(iIndex);
        m_cstrMiddleInitial = cstrName.Right(cstrName.GetLength() - iIndex - 2);
        }
      else
        { // No middle initial.
        m_cstrFirstName = cstrName;
        m_cstrMiddleInitial.Empty();
        }
      }
    else
      {
      m_cstrLastName = cstrName;
      m_cstrFirstName.Empty();
      m_cstrMiddleInitial.Empty();
      }
    m_lPatientID = (long)m_cPatientDBList.GetItemData(iSel);
    m_cstrDate = m_cPatientDBList.GetItemText(iSel, 1);
#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
    if(m_cstrDate.IsEmpty() == true)
      {
      // Make sure that this array has enough items in it to get the date.
      if(iSel < m_cstraStudyIDs.GetCount())
        m_cstrDate = m_cstraStudyIDs.GetAt(iSel);
      //if(m_plStudyIDs != NULL)
      //  m_pDoc->m_pDB->iGet_study_date(*(m_plStudyIDs + iSel), m_cstrDate);
      }
#endif
    }
  else
    { // What If
    m_lWhatIfID = INV_LONG;
    m_cstrLastName.Empty();
    m_cstrFirstName.Empty();
    m_cstrMiddleInitial.Empty();
    m_lPatientID = INV_LONG;
    m_cstrDate.Empty();
    wi.lWhatIfID = (long)m_cPatientDBList.GetItemData(iSel);
    if(m_pDoc->m_pDB->iGet_whatif_data(&wi) == SUCCESS)
      { // Use the study ID to get the Study information.
      pdInfo.lStudyID = wi.lStudyID;
      if(m_pDoc->m_pDB->iGet_study_data(&pdInfo) == SUCCESS)
        { // Use the patient ID to get the patient information.
        if(m_pDoc->m_pDB->iGet_patient(&pdInfo) == SUCCESS)
          {
          m_lWhatIfID = wi.lWhatIfID;
          m_cstrLastName = pdInfo.cstrLastName;
          m_cstrFirstName = pdInfo.cstrFirstName;
          m_cstrMiddleInitial = pdInfo.cstrMI;
          m_lPatientID = pdInfo.lPatientID;
          m_cstrDate = pdInfo.cstrDate;
          }
        }
      }
    }
  }

/********************************************************************
vFill_patient_listbox - Fill in the listbox with patient names and dates
                        of study from the database.

  inputs: None.

  return: None.
********************************************************************/
void CSelectPatient::vFill_patient_listbox()
  {
  CString cstrItem1, cstrItem2, cstrDate, cstrWIDate;
  FIND_PATIENT_INFO fpiData;
//  PATIENT_DATA pdInfo;
  FIND_NEXT_STUDY fnsData;
  WHAT_IF wi;
  short int iPatientPos, iStudyPos, iPatientSts, iStudySts;
  int iIndex, iCnt, iWhatIfPos;
  bool bAddToLstBox;

//#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
//  if(m_pDoc->m_pDB->iGet_study_count_1(&iCnt) == SUCCESS)
//    { // Allocate an array to hold study IDs.  If study dates are hidden, the
//      // study date will not be shown and it is needed to get the study data from
//      // the database if selected.  In this case, the study ID will be used to get the
//      // date from the study table if selected.
//    m_plStudyIDs = (long *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, iCnt * sizeof(long));
//    }
//#endif
  // First make sure the listbox is empty.
  m_cPatientDBList.DeleteAllItems();
    // Display the hourglass cursor.
  SetCursor(LoadCursor(NULL, IDC_WAIT));
  ShowCursor(TRUE);
    // Fill in the list box from the database.
  if(m_iStudyType == STUDY_NONE)
    {
    iPatientSts = SUCCESS;
    iPatientPos = FIRST_RECORD;
    iCnt = 0;
    while(iPatientSts == SUCCESS)
      {
      fpiData.bHide = m_bHiddenPatients;
      fpiData.bAll = FALSE;
      if((iPatientSts = m_pDoc->m_pDB->iFind_next_patient(iPatientPos, &fpiData)) == SUCCESS)
        {
        iStudySts = SUCCESS;
        iStudyPos = FIRST_RECORD;
          // Find out if the research reference number is being used and if it
          // is, use it instead of the patient name.
//        pdInfo.lPatientID = fpiData.lPatientID;
//        if(m_pDoc->m_pDB->iGet_patient_info(&pdInfo) == SUCCESS)
//          {
          if(fpiData.bUseResrchRefNum == TRUE)
            cstrItem1 = fpiData.cstrResearchRefNum;
          else
            cstrItem1 = cstrFormat_name(&fpiData);
//          }
        fnsData.lPatientID = fpiData.lPatientID;
        while(iStudySts == SUCCESS)
          {
//          if((iStudySts = m_pDoc->m_pDB->iFind_next_study(iStudyPos, cstrItem2,
//          &bPause, fpiData.lPatientID)) == SUCCESS)
          if((iStudySts = m_pDoc->m_pDB->iFind_next_study(iStudyPos, &fnsData)) == SUCCESS)
            {
//            // If this is a standard program and the study is a research
//              // study, don't put it in the listbox.
//            if(theApp.m_ptPgmType == PT_STANDARD && fnsData.uDataPtsInBaseline != 0)
//              continue;
            bAddToLstBox = false;
            if(theApp.m_ptPgmType == PT_STD_RSCH)
               bAddToLstBox = true; // Add all files.
            else if(theApp.m_ptPgmType == PT_RESEARCH)
              { // Research version.
              if(fnsData.uDataPtsInBaseline != 0)
                bAddToLstBox = true; // Research file.
              }
            else //if(theApp.m_ptPgmType == PT_STANDARD)
              { // Standard version.
              if(fnsData.uDataPtsInBaseline == 0)
                bAddToLstBox = true; // Standard file.
              }
            if(bAddToLstBox == true)
              {
              iIndex = m_cPatientDBList.InsertItem(iCnt, cstrItem1); // insert new item, patient name
              // Index is the position in the listbox.
              // Can we save the encoded date?  Listbox data item?

//#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
//              if(fnsData.cstrDate.GetLength() <= 10)
//                m_cPatientDBList.SetItemText(iIndex, 1, fnsData.cstrDate); // Date of study
//              m_cstraStudyIDs.Add(fnsData.cstrDate);
//              //if(m_plStudyIDs != NULL)
//              //  *(m_plStudyIDs + iIndex) = fnsData.lStudyID;
//#else
//              m_cPatientDBList.SetItemText(iIndex, 1, fnsData.cstrDate); // Date of study
//#endif
#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
#if EGGSAS_READER == READER_DEPOT
              if(g_pConfig->m_bHideStudyDates == false)
                { // showing all study dates.
                if(fnsData.cstrDate.GetLength() <= 10)
                  cstrDate = fnsData.cstrDate; // Date of study already plain
                else
                  { // Decrypt the study date and display it.
                  cstrDate = cstrDecode_study_date(fnsData.cstrDate);
                  }
                }
              else
                cstrDate.Empty(); // study dates are hidden.
#elif EGGSAS_READER == READER_DEPOT_E
              if(g_pConfig->m_bHideStudyDates == false)
                { // showing all study dates.
                if(fnsData.cstrDate.GetLength() <= 10)
                  cstrDate = fnsData.cstrDate; // Date of study already plain
                else
                  { // Decrypt the study date and display it.
                  cstrDate = cstrDecode_study_date(fnsData.cstrDate);
                  }
                }
              else
                cstrDate.Empty(); // study dates are hidden.
#else
              // all other versions for hide study dates.
              // all dates are hidden.
              //m_cImportList.SetItemText(iIndex, 1, fnsData.cstrDate); // Date of study
#endif
#else
              cstrDate = fnsData.cstrDate; // Date of study
#endif
#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
              // Add the study date to and array of study dates corresponding in order
              // of the studies in the listbox.  This is needed so that the study dates
              // are available to retrieve the study from the database.
              m_cstraStudyIDs.Add(fnsData.cstrDate);
#endif

              // Show the study date if not hidden.
              if(cstrDate.IsEmpty() == false)
                m_cPatientDBList.SetItemText(iIndex, 1, cstrDate); // Date of study
              m_cPatientDBList.SetItemData(iIndex, (DWORD)fpiData.lPatientID); // Patient ID
              ++iCnt;
                // Now find any WhatIF studies.
              if(theApp.m_ptPgmType == PT_RESEARCH || theApp.m_ptPgmType == PT_STD_RSCH)
                {
                wi.lStudyID =  fnsData.lStudyID; // m_pDoc->m_pdPatient.lStudyID;
                iWhatIfPos = FIRST_RECORD;
                while(m_pDoc->m_pDB->iFind_next_whatif(iWhatIfPos, &wi) == SUCCESS)
                  { // Have a record, display it.
//                  vAdd_item_to_list(wi.cstrDateTime, wi.cstrDescription, wi.lWhatIfID);
                  wi.cstrDescription.Insert(0, WHATIF_PREFIX);
                    // insert What IF description
                  iIndex = m_cPatientDBList.InsertItem(iCnt, wi.cstrDescription);
                     // Date of What IF
                  cstrWIDate = cstrReformat_date(wi.cstrDateTime);

// 09-SEP-20  RET Version 2.09h
#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
#if EGGSAS_READER == READER_DEPOT
                  if(g_pConfig->m_bHideStudyDates == false)
                    { // showing all study dates.
                    if(cstrWIDate.GetLength() <= 10)
                      cstrDate = cstrWIDate; // Date of study already plain
                    else
                      { // Decrypt the study date and display it.
                      cstrDate = cstrDecode_study_date(cstrWIDate);
                      }
                    }
                  else
                    cstrDate.Empty(); // study dates are hidden.
#elif EGGSAS_READER == READER_DEPOT
                  if(g_pConfig->m_bHideStudyDates == false)
                    { // showing all study dates.
                    if(cstrWIDate.GetLength() <= 10)
                      cstrDate = cstrWIDate; // Date of study already plain
                    else
                      { // Decrypt the study date and display it.
                      cstrDate = cstrDecode_study_date(cstrWIDate);
                      }
                    }
                  else
                    cstrDate.Empty(); // study dates are hidden.
#endif
#else
                  cstrDate = cstrWIDate; // Date of study
#endif
#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
                  // Add the study date to and array of study dates corresponding in order
                  // of the studies in the listbox.  This is needed so that the study dates
                  // are available to retrieve the study from the database.
                  m_cstraStudyIDs.Add(cstrWIDate);
#endif
                  //m_cPatientDBList.SetItemText(iIndex, 1, cstrReformat_date(wi.cstrDateTime));
                  m_cPatientDBList.SetItemText(iIndex, 1, cstrDate);


                  m_cPatientDBList.SetItemData(iIndex, (DWORD)wi.lWhatIfID); // WhatIF ID
                  ++iCnt;
                  iWhatIfPos = NEXT_RECORD;
                  }
                }
              }
            }
          iStudyPos = NEXT_RECORD;
          }
        }
      iPatientPos = NEXT_RECORD;
      }
    }
  else if(m_iStudyType == STUDY_RESUME)
    { // Resuming an study that has completed the baseline.
    iPatientSts = SUCCESS;
    iPatientPos = FIRST_RECORD;
    iCnt = 0;
    while(iPatientSts == SUCCESS)
      {
      fpiData.bHide = FALSE; // Only Active patients.
      fpiData.bAll = FALSE;
      if((iPatientSts = m_pDoc->m_pDB->iFind_next_patient(iPatientPos, &fpiData)) == SUCCESS)
        {
        iStudySts = SUCCESS;
        iStudyPos = FIRST_RECORD;
          // Find out if the research reference number is being used and if it
          // is, use it instead of the patient name.
//        pdInfo.lPatientID = fpiData.lPatientID;
//        if(m_pDoc->m_pDB->iGet_patient_info(&pdInfo) == SUCCESS)
//          {
          if(fpiData.bUseResrchRefNum == TRUE)
            cstrItem1 = fpiData.cstrResearchRefNum;
          else
            cstrItem1 = cstrFormat_name(&fpiData);
//          }
        fnsData.lPatientID = fpiData.lPatientID;
        while(iStudySts == SUCCESS)
          {
//          if((iStudySts = m_pDoc->m_pDB->iFind_next_study(iStudyPos, cstrItem2, 
//          &bPause, fpiData.lPatientID)) == SUCCESS)
          if((iStudySts = m_pDoc->m_pDB->iFind_next_study(iStudyPos, &fnsData)) == SUCCESS)
            {
            if(fnsData.bPause == TRUE)
              {
              iIndex = m_cPatientDBList.InsertItem(iCnt, cstrItem1); // insert new item, patient name
              m_cPatientDBList.SetItemText(iIndex, 1, fnsData.cstrDate); // Date of study
              m_cPatientDBList.SetItemData(iIndex, (DWORD)fpiData.lPatientID); // Patient ID
              ++iCnt;
              }
            }
          iStudyPos = NEXT_RECORD;
          }
        }
      iPatientPos = NEXT_RECORD;
      }
    }
  else
    { // Existing patient.
    iPatientSts = SUCCESS;
    iPatientPos = FIRST_RECORD;
    iCnt = 0;
    while(iPatientSts == SUCCESS)
      {
      fpiData.bHide = FALSE;
      fpiData.bAll = FALSE;
      if((iPatientSts = m_pDoc->m_pDB->iFind_next_patient(iPatientPos, &fpiData)) == SUCCESS)
        {
          // Find out if the research reference number is being used and if it
          // is, use it instead of the patient name.
//        pdInfo.lPatientID = fpiData.lPatientID;
//        if(m_pDoc->m_pDB->iGet_patient_info(&pdInfo) == SUCCESS)
//          {
          if(fpiData.bUseResrchRefNum == TRUE)
            cstrItem1 = fpiData.cstrResearchRefNum;
          else
            cstrItem1 = cstrFormat_name(&fpiData);
//          }
//        else
//          cstrItem1 = cstrFormat_name(&fpiData);
        iIndex = m_cPatientDBList.InsertItem(iCnt, cstrItem1); // insert new item, patient name
        m_cPatientDBList.SetItemData(iIndex, (DWORD)fpiData.lPatientID); // Patient ID
        ++iCnt;
        }
      iPatientPos = NEXT_RECORD;
      }
    }
    // Restore the arrow cursor.
  SetCursor(LoadCursor(NULL, IDC_ARROW));
  ShowCursor(TRUE);
  }

/********************************************************************
cstrFormat_name - Format the patient name.  Use the middle initial if there
                  is one.

  inputs: FIND_PATIENT_INFO structure.

  return: None.
********************************************************************/
CString CSelectPatient::cstrFormat_name(FIND_PATIENT_INFO *pfpiData)
  {
  CString cstrName;

  if(pfpiData->cstrMI.IsEmpty() == FALSE)
    cstrName.Format("%s, %s %s", pfpiData->cstrLastName, pfpiData->cstrFirstName,
                     pfpiData->cstrMI);
  else
    cstrName.Format("%s, %s", pfpiData->cstrLastName, pfpiData->cstrFirstName);
  return(cstrName);
  }

  void CSelectPatient::vSet_text(void)
    {

    if(g_pLang != NULL)
      {
      g_pLang->vStart_section(SEC_SELECTPATIENT);
      this->SetWindowText(g_pLang->cstrGet_text(ITEM_SP_SEL_PATIENT_TITLE));
      GetDlgItem(IDC_STATIC_CURRENT_DB)->SetWindowText(
        g_pLang->cstrGet_text(ITEM_SP_CURRENT_DB));
      GetDlgItem(IDC_BTN_SELECT_LOCAL_DB)->SetWindowText(
        g_pLang->cstrGet_text(ITEM_SP_SEL_LOCAL_DB));
      GetDlgItem(IDC_BTN_SELECT_READER_DB)->SetWindowText(
        g_pLang->cstrGet_text(ITEM_SP_SEL_READER_DB));
      GetDlgItem(IDC_ALTERNATE_DB)->SetWindowText(
        g_pLang->cstrGet_text(ITEM_SP_SEL_PATIENT_DB));
      GetDlgItem(IDC_IMPORT)->SetWindowText(
        g_pLang->cstrGet_text(ITEM_SP_IMP_PATIENT_FILE));
      GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
      GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
      GetDlgItem(IDC_SELECTPATIENT_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
      g_pLang->vEnd_section();
      }
    }

  void CSelectPatient::vFree_data(void)
    {
#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
    //if(m_plStudyIDs != NULL)
    //  HeapFree(GetProcessHeap(), 0, (LPVOID)m_plStudyIDs);
    m_cstraStudyIDs.RemoveAll();
#endif
    }


//// REMOTE DB
//void CSelectPatient::vDisplay_db_file()
//  {
//
//  m_cstrDBFile = m_pDoc->m_pDB->cstrGet_db_file_path();
//  if(m_cstrDBFile == m_pDoc->m_pDB->m_cstrLocalDBPath)
//    m_cstrDBFile.Empty();
//  }

  void CSelectPatient::OnBnClickedBtnSearch()
    {
    CSearchDB *pdlg;
    CString cstrDate, cstrSearchSQL, cstrMonth, cstrDay, cstrYear;
    //int iDateSrchType;
    //short int iStudyPos, iStudySts;
    FIND_NEXT_STUDY fnsData;
    CList<int,int> lstStudyID, lstPatientID;
    //DATE_SRCH_INFO dsi;
    CTime ctSearch, ctStudy;

    pdlg = new CSearchDB();
    if(pdlg->DoModal() == IDOK)
      {
      m_pDoc->m_pDB->m_cstrSearchSQL = pdlg->m_cstrSQL;
      m_pDoc->m_pDB->m_iDateSrchType = pdlg->m_iDateSrchType;
      m_pDoc->m_pDB->m_cstrSrchDate = pdlg->m_cstrDate;
      if(pdlg->m_iDateSrchType != DB_SRCH_NONE && pdlg->m_cstrDate.Find("*") < 0)
        m_pDoc->m_pDB->m_ctSrchDate = CTime(atoi(pdlg->m_cstrYear), atoi(pdlg->m_cstrMonth), atoi(pdlg->m_cstrDay), 0, 0, 0);
      vFill_patient_listbox();
      m_pDoc->m_pDB->m_cstrSearchSQL.Empty();
      m_pDoc->m_pDB->m_iDateSrchType = 0;
      }
    //else
    //  { // User cancelled.
    //  m_pDoc->m_pDB->m_cstrSearchSQL.Empty();
    //  m_pDoc->m_pDB->m_iDateSrchType = 0;
    //  vFill_patient_listbox();
    // }
    delete pdlg;

    //if(cstrSearchSQL != "" || iDateSrchType > 0)
    //  { // Have a database search.
    //  m_pDoc->m_pDB->m_cstrSearchSQL = cstrSearchSQL;
    //  if(iDateSrchType > 0)
    //    { // Have a date search.
    //    if(iDateSrchType == LESS_THAN)
    //      {
    //      ctSearch = CTime(atoi(cstrYear), atoi(cstrMonth), atoi(cstrDay), 0, 0, 0);
    //      // For each study record, if the date is less than specified date save the study ID and the patient ID.
    //      iStudySts = SUCCESS;
    //      iStudyPos = FIRST_RECORD;
    //      while(iStudySts == SUCCESS)
    //        {
    //        if((iStudySts = m_pDoc->m_pDB->iFind_next_study(iStudyPos, &fnsData)) == SUCCESS)
    //          {
    //          ctStudy = ctDate_to_ctime(fnsData.cstrDate);
    //          if(ctStudy < ctSearch)
    //            {
    //            lstPatientID.AddTail(fnsData.lPatientID);
    //            lstPatientID.AddTail(fnsData.lStudyID);
    //            }
    //          }
    //        iStudyPos = NEXT_RECORD;
    //        }


    //      }
    //    }
    //  vFill_patient_listbox();
    //  m_pDoc->m_pDB->m_cstrSearchSQL.Empty();
    //  }
    }
