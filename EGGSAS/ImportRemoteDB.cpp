/********************************************************************
ImportRemoteDB.cpp

Copyright (C) 2008,2009,2011,2012,2016,2018,2019,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CImportRemoteDB class.

  This class provides functionality for importing a reader database into
  the local database.


HISTORY:
12-NOV-08  RET  Version R.2.08a
                  New.
30-NOV-08  RET  Version R.2.08b
                  Add Help button to dialog box.
12-FEB-09  RET  Version 2.08h
                  Put a list of studies to be imported in the Import dialog box
                    so user can select only some or all studies to import.
                    Required rewriting most of the import functions.
                    Add method: iSetup_databases().
13-FEB-09  RET  Version 2.08i
                    Fix bug in OnBnClickedBtnBrowse() to set the database
                      object pointer to NULL after it is deleted.
                    Change iSetup_databases():
                      - To return FAIL if the database location is empty.
                        This prevents the list box from being populated and
                        also prevents an initial error the first time the
                        import function is used.
                      - Check the version of the database to be imported and
                        if not the version the program is expecting, display
                        an error message.
01-FEB-11  RET
                Change OnInitDialog() to disable all buttons allowing for selecting
                  and importing studies for the Reader Demo.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
02-JUN-16  RET
             Add feature to hide study dates.
               Change method:
                 vFill_patient_listbox() to show or hide study dates.
                 OnBnClickedBtnImport() to change study date for the imported
                   studies to be either plain or encrypted depending on
                   the show/hide flag.
28-MAY-18  RET
             Add ability to rename an imported patient that already exists in the database.
               Add dialog box for user to select skip/overwrite/change name.
               Change methods: iImport_db()
03-FEB-19  RET  Version 2.09f-1.3
             Add ability to update an imported database if it is of an older version.
               Change methods: iImport_db(), iSetup_databases()
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedImportRemoteDbHelp()
14-NOV-20  RET
             Add ability to work with a remote database.
             Changed settings to allow the user to specify the remote database.
               Change methods: OnBnClickedBtnImport(), iSetup_databases()
16-NOV-20  RET
             Add new version CDDRE
               Change methods: vFill_patient_listbox()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "Dbaccess.h"
#include "util.h"
#include "ImportDupName.h"
#include "ImportRemoteDB.h"
//#include "importremotedb.h"
//#include "dbaccessupd.h"
#include "dbupdate.h"


// CImportRemoteDB dialog

IMPLEMENT_DYNAMIC(CImportRemoteDB, CDialog)
/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the active document that started this dialog box.
          Pointer to the window that started this dialog box.

********************************************************************/
CImportRemoteDB::CImportRemoteDB(CEGGSASDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CImportRemoteDB::IDD, pParent)
  , m_cstrProgressTitle(_T(""))
  , m_cstrDBLocation(_T(""))
  {

  m_pDoc = pDoc;
  }

/********************************************************************
Destructor

  Clean up class data.

********************************************************************/
CImportRemoteDB::~CImportRemoteDB()
{

  if(m_pImpDB != NULL)
    {
    delete m_pImpDB;
    m_pImpDB = NULL;
    }

  g_pLang->vEnd_section();
}

void CImportRemoteDB::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_STATIC_PROGRESS_TITLE, m_cstrProgressTitle);
DDX_Text(pDX, IDC_EDIT_DB_LOCATION, m_cstrDBLocation);
DDX_Control(pDX, IDC_PROGRESS1, m_cImportProgressBar);
DDX_Control(pDX, IDC_LIST_IMPORT, m_cImportList);
}


BEGIN_MESSAGE_MAP(CImportRemoteDB, CDialog)
  ON_BN_CLICKED(IDC_BTN_BROWSE, OnBnClickedBtnBrowse)
  ON_BN_CLICKED(IDC_IMPORT_REMOTE_DB_HELP, OnBnClickedImportRemoteDbHelp)
  ON_WM_CLOSE()
  ON_BN_CLICKED(IDC_BTN_IMPORT, OnBnClickedBtnImport)
  ON_BN_CLICKED(IDC_BTN_IMPORT, OnBnClickedBtnImport)
  ON_BN_CLICKED(IDC_BTN_SELECT_ALL, OnBnClickedBtnSelectAll)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CImportRemoteDB message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Set up initial values for various fields in the dialog box.
********************************************************************/
BOOL CImportRemoteDB::OnInitDialog()
  {
  HDITEM hdiHdrItem;
  CString cstrItem1, cstrItem2;
  RECT rRect;

  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  vSet_text();
    // Get path for the remote database from the INI file.
  m_pImpDB = NULL;

  //cstrItem1 = AfxGetApp()->GetProfileString(INI_SEC_READER, INI_ITEM_READER_DB_PATH, "");
  cstrItem1 = g_pConfig->cstrGet_item(CFG_READERDBPATH);
  if(cstrItem1.IsEmpty() == TRUE)
    m_cstrDBLocation.Empty();
  else
    m_cstrDBLocation.Format("%s\\%s.mdb", cstrItem1, EXP_DSN_DEFAULT);
    // Fill in the title for the progress bar.
  //m_cstrProgressTitle.LoadString(IDS_IMPORT_PROGRESS);
  m_cstrProgressTitle = g_pLang->cstrLoad_string(ITEM_G_IMPORT_PROG);

  // Set up the list of studies to be imported
  

  //cstrItem1.LoadString(IDS_PATIENT_NAME);
  cstrItem1 = g_pLang->cstrLoad_string(ITEM_G_PATIENT_NAME);
  cstrItem2 = g_pLang->cstrLoad_string(ITEM_G_DATE_OF_STUDY);
  //cstrItem2.LoadString(IDS_DATE_OF_STUDY);
  m_cImportList.GetWindowRect(&rRect);
  m_cImportList.InsertColumn(0, cstrItem1, LVCFMT_LEFT,
                                rRect.right - rRect.left - 100, 0);
  m_cImportList.InsertColumn(1, cstrItem2, LVCFMT_LEFT, 100, 1);
    // Now add the header items.
  CHeaderCtrl* pHeaderCtrl = m_cImportList.GetHeaderCtrl();
  hdiHdrItem.mask = HDI_FORMAT | HDI_TEXT | HDI_WIDTH;
  hdiHdrItem.pszText = cstrItem1.GetBuffer(cstrItem1.GetLength());
  hdiHdrItem.cchTextMax = strlen(hdiHdrItem.pszText);
  hdiHdrItem.cxy = rRect.right - rRect.left - 100; // Use all of window except for date.
  hdiHdrItem.fmt = HDF_LEFT | HDF_STRING; // HDF_CENTER
  pHeaderCtrl->SetItem(0, &hdiHdrItem);

    // Allows full row to show selection.  Also allows mouse click
    // for selection to be anywhere on the row.
  m_cImportList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

  if(m_cstrDBLocation.IsEmpty() == FALSE)
    {
    if(iSetup_databases() == SUCCESS)
      vFill_patient_listbox();
    }

#if EGGSAS_READER != READER_NONE
   // reader.
#if EGGSAS_SUBTYPE == SUBTYPE_READER_DEMO
      // Disable the "Select Database to import",
      // "Select All Studies", "Import" buttons.
  GetDlgItem(IDC_BTN_BROWSE)->EnableWindow(FALSE);
  GetDlgItem(IDC_BTN_SELECT_ALL)->EnableWindow(FALSE);
  GetDlgItem(IDC_BTN_IMPORT)->EnableWindow(FALSE);
#endif
#endif

  UpdateData(FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

void CImportRemoteDB::OnBnClickedOk()
  {

    // Delete the database object for the remote database.
  if(m_pImpDB != NULL)
    {
    delete m_pImpDB;
    m_pImpDB = NULL;
    }

  OnOK();
  }

void CImportRemoteDB::OnClose()
  {

    // Delete the database object for the remote database.
  if(m_pImpDB != NULL)
    {
    delete m_pImpDB;
    m_pImpDB = NULL;
    }

  g_pLang->vEnd_section();

  CDialog::OnClose();
  }

/********************************************************************
OnBnClickedBtnBrowse

  Message handler for the "Select Database to import" button.
    Display a file open dialog box.
********************************************************************/
void CImportRemoteDB::OnBnClickedBtnBrowse()
  {

    // Display File Open dialog box to select the reader database.
  m_cstrDBLocation = m_pDoc->cstrSelect_reader_db();
  if(m_pImpDB != NULL)
    {
    delete m_pImpDB;
    m_pImpDB = NULL;
    }
  if(m_cstrDBLocation.IsEmpty() == FALSE)
    {
    if(iSetup_databases() == SUCCESS)
      vFill_patient_listbox();
    }
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedBtnImport

  Message handler for the "Import" button.
    Get the path selected by the user.
    Create a temporary databse object for this database.
    Import the remote database into the local database.
********************************************************************/
void CImportRemoteDB::OnBnClickedBtnImport()
  {
  CString cstrMsg, cstrTitle; // cstrCurrentDBPath
  int iRet;

  UpdateData(TRUE);
  if(m_cstrDBLocation.IsEmpty() == FALSE)
    {
    if(m_cImportList.GetSelectedCount() == 0)
      { // User didn't select any studies from the list.
      //cstrMsg.LoadString(IDS_SELECT_STUDY_FROM_LIST);
      //cstrTitle.LoadString(IDS_ERROR1);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_SEL_STUDY_FROM_LIST);
      cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
      }
    else
      {
      //cstrTitle.LoadString(IDS_WARNING);
      //cstrMsg.LoadString(IDS_ADD_EXT_STUDIES_SURE);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_ADD_EXT_STUDIES_SURE)
                + "\n\n" + g_pLang->cstrLoad_string(ITEM_G_ADD_EXT_STUDIES_SURE1);
      cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
      if(MessageBox(cstrMsg, cstrTitle, MB_ICONWARNING | MB_YESNO) == IDYES)
        {
        //m_cstrProgressTitle.LoadString(IDS_IMPORT_PROGRESS);
        m_cstrProgressTitle = g_pLang->cstrLoad_string(ITEM_G_IMPORT_PROG);
        UpdateData(FALSE);
        //  // Find out what database the program's database access object is using and
        //  // save it.
        //cstrCurrentDBPath = m_pDoc->m_pDB->cstrGet_db_file_path();
        //  // Set the program's database access object to the local database and
        //  // create a new database access object connected to the reader database.
        //if(cstrCurrentDBPath != DSN_DEFAULT)
        //  m_pDoc->vChange_db(LOCAL_DB, "");
        //pDB = new CDbaccess(m_cstrDBLocation);
          // Connected to both databases.
          // Import the reader database.
        g_pLang->vEnd_section();
        iRet = iImport_db(m_pDoc->m_pDB, m_pImpDB);
        if(g_pConfig->cstrGet_item(CFG_REMOTE_DB_PATH).IsEmpty() == true)
          {
          // Restore the program's database access object.
          if(m_cstrCurrentDBPath != DSN_DEFAULT)
            m_pDoc->vChange_db(LOCAL_DB, m_cstrCurrentDBPath);
          }
        //delete m_pImpDB;
        if(iRet == SUCCESS_NO_ACTION)
          m_cstrProgressTitle = g_pLang->cstrLoad_string(ITEM_G_IMPORTCANCELLED);
          //m_cstrProgressTitle.LoadString(IDS_IMPORT_CANCELLED);
        else if(iRet == FAIL)
          m_cstrProgressTitle = g_pLang->cstrLoad_string(ITEM_G_IMPORTFAILED);
          //m_cstrProgressTitle.LoadString(IDS_IMPORT_FAILED);
        else
          {
#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
#if EGGSAS_READER != READER_DEPOT
          if(m_pDoc->m_pDB != NULL)
            m_pDoc->m_pDB->iEncode_study_dates(true);
#else

          if(m_pDoc->m_pDB != NULL)
            m_pDoc->m_pDB->iEncode_study_dates(g_pConfig->m_bHideStudyDates);
#endif
#endif
          m_cstrProgressTitle = g_pLang->cstrLoad_string(ITEM_G_IMPORTCOMPLETE);
          //m_cstrProgressTitle.LoadString(IDS_IMPORT_COMPLETE);
          }
      
        UpdateData(FALSE);
        }
      }
    }
  else
    {
    //cstrTitle.LoadString(IDS_ERROR1);
    //cstrMsg.LoadString(IDS_SEL_REMOTE_DB_LOC);
    cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_SEL_REMOTE_DB_LOC);
    MessageBox(cstrMsg, cstrTitle);
    }
  }

void CImportRemoteDB::OnBnClickedBtnSelectAll()
  {
  LVITEM lvItem;
  int iCnt;

  lvItem.mask = LVIF_STATE;
  lvItem.stateMask = LVIS_SELECTED;
  lvItem.state = LVIS_SELECTED;
  for(iCnt = 0; iCnt < m_cImportList.GetItemCount(); ++iCnt)
    {
    m_cImportList.SetItemState(iCnt, &lvItem);
    }
  m_cImportList.SetFocus();
  }

/********************************************************************
OnBnClickedImportRemoteDbHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CImportRemoteDB::OnBnClickedImportRemoteDbHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_IMPORT_REMOTE_DB_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_IMPORT_REMOTE_DB_HELP);
  }

/////////////////////////////////////////////////////////////////////
//// Support methods.

/********************************************************************
iImport_db - Import a database into the local database.

               For each patient
                 Import the patient table and if the patient record
                   already exist, as to overwrite it.
                 If new patient or overwritten patient, import the
                   patient info table.
                 For each study
                   Get the data file name from the remote DB.
                   If data data file already exists locally, as to
                     overwrite it.
                   If new data file, or overwriting data file
                     Import study data and automatically overwrite if it
                       already exists.
                     Delete all WhatIf scenarios for the study and import
                       the ones from the remote database.
                     Delete all Event records for the study and import
                       the ones from the remote database.


  inputs: Pointer to a database object for the local database.
          Pointer to Dialoga database object for the database to be imported.

  return: SUCCESS if the import is successful.
          FAIL if there is an error.
********************************************************************/
int CImportRemoteDB::iImport_db(CDbaccess *pDBLocal, CDbaccess *pDBRemote)
  {
  int iRet = SUCCESS;
  int iSts, iDupAction; // iPatientPos, iStudyPos, , iMBSts 
  unsigned int uiStudyCnt, uiNumRecs, uiCnt, uiImportCnt;
  short int iFCSts, iDBSts;
  FIND_PATIENT_INFO fpiData;
  PATIENT_DATA PatientData;
  PATIENT_INFO piInfo;
//  FIND_NEXT_STUDY fns;
  STUDY_DATA_RECORD StudyData;
  long lPatientIDLocal, lPatientIDRemote, lStudyIDLocal, lStudyIDRemote;
  bool bOverwrite, bCancel, bSkipPatientData;
  CString cstrTemp, cstrTemp1, cstrTitle, cstrDest, cstrSrc;
  POSITION pos;

  // Set up progress bar.
  uiNumRecs = m_cImportList.GetSelectedCount();
  m_cImportProgressBar.SetRange(0, (short)uiNumRecs);
  m_cImportProgressBar.SetPos(0);
  m_cImportProgressBar.SetStep(1);
  UpdateData(FALSE);

  // Create an array to hold the Patient IDs of patients in the remote
  // database that have already been imported.
  unsigned int *puiaPatientID = new unsigned int[uiNumRecs];
  // Holds the new patient ID if the user renames the patient.
  unsigned int *puiaPatientNewID = new unsigned int[uiNumRecs];
  // Create an array that holds a new patient ID if user prevously
  // selected to rename a patient.  the renamed patient would that have
  // a new ID.
  for(uiStudyCnt = 0; uiStudyCnt < uiNumRecs; ++uiStudyCnt)
    {
    puiaPatientID[uiStudyCnt] = 0;
    puiaPatientNewID[uiStudyCnt] = 0;
    }
  
  bCancel = false;
  bSkipPatientData = false;
  iDupAction = IMPORT_DUP_SKIP;
  //for(uiStudyCnt = 0; uiStudyCnt < uiNumRecs && bCancel == false && iRet != FAIL;
  //++uiStudyCnt)
  uiImportCnt = 0;
  pos = m_cImportList.GetFirstSelectedItemPosition();
  while(pos != NULL)
    {  // Get the study ID from the selected study.
    uiStudyCnt = m_cImportList.GetNextSelectedItem(pos);
    bSkipPatientData = false;
    lStudyIDRemote = m_cImportList.GetItemData(uiStudyCnt);
    StudyData.lStudyID = lStudyIDRemote;
    StudyData.cstrDateOfStudy = ""; // Force getting study data on study ID.
    if((iRet = pDBRemote->iGet_study_data(&StudyData)) == SUCCESS)
      { // Got the study data from the database.  Now we have the patient ID.
        // Get the patient information.
      lPatientIDRemote = StudyData.lPatientID;
      PatientData.lPatientID = StudyData.lPatientID;
      if((iDBSts = pDBRemote->iGet_patient(&PatientData)) == SUCCESS)
        { // Got the patient information for the imported database.
        bOverwrite = true;
        iSts = SUCCESS;
        fpiData.cstrFirstName = PatientData.cstrFirstName;
        fpiData.cstrLastName = PatientData.cstrLastName;
        fpiData.cstrMI = PatientData.cstrMI;
        fpiData.cstrResearchRefNum = PatientData.cstrResearchRefNum;
        fpiData.bUseResrchRefNum = PatientData.bUseResrchRefNum;
        piInfo.lPatientID = PatientData.lPatientID; //lPatientIDRemote;
           // Save patient in local db (if already exists, ask to overwrite)
        if((iDBSts = pDBLocal->iFind_patient_name(&fpiData)) == SUCCESS)
          { // Patient already exists in the local database, ask to overwite.
          lPatientIDLocal = fpiData.lPatientID;
            // If we already have imported the patient (not the study), skip
            // finding the patient.
          bSkipPatientData = false;
          for(uiCnt = 0; uiCnt < uiNumRecs; ++uiCnt)
            {
            // Check the new ID array also.
            // If a patient was overwritten, the new ID would be 0.
            // If a patient was renamed, the new ID would be non-zero.
            if(puiaPatientID[uiCnt] == lPatientIDRemote)
              {
              bSkipPatientData = true;
              if(puiaPatientNewID[uiCnt] != 0)
                { // This patient has been renamed.  Get the patient data for
                  // the renamed patient.
                lPatientIDLocal = puiaPatientNewID[uiCnt]; // Patient is renamed with new local ID.
                PatientData.lPatientID = lPatientIDLocal;
                if((iDBSts = pDBLocal->iGet_patient(&PatientData)) != SUCCESS)
                  { // Couldn't find the new patient, use the old patient.
                  PatientData.lPatientID = puiaPatientID[uiCnt];
                  iDBSts = pDBLocal->iGet_patient(&PatientData);
                  bSkipPatientData = false;
                  }
                }
              break;
              }
            else if(puiaPatientID[uiCnt] == 0)
              break; // done with patients already processed.
            }
          if(bSkipPatientData == false)
            {
            // Change to a new dialog box asking to overwrite, skip, or change name.
            // Also need to know if name or research ref num.
            // If change name, add a new patient and new patient info.
            //   Save the new patient ID in the new patient ID array.
            //   Add the study as new.
            CImportDupName *pdlg = new CImportDupName();
            //cstrTitle = g_pLang->cstrLoad_string(ITEM_G_OVERWRITE_PATIENT)
            //            + "\n " + g_pLang->cstrLoad_string(ITEM_G_OVERWRITE_PATIENT1);
            if(fpiData.bUseResrchRefNum == TRUE)
              {
              //cstrTemp.Format(IDS_OVERWRITE_PATIENT, fpiData.cstrResearchRefNum);
              //cstrTemp.Format(cstrTitle, fpiData.cstrResearchRefNum);
              pdlg->m_cstrName = fpiData.cstrResearchRefNum;
              pdlg->m_cstrFullName = fpiData.cstrResearchRefNum;
              }
            else
              {
              cstrTemp1.Format("%s %s", fpiData.cstrFirstName, fpiData.cstrLastName);
              //cstrTemp.Format(cstrTitle, cstrTemp1);
              //cstrTemp.Format(IDS_OVERWRITE_PATIENT, cstrTemp1);
              pdlg->m_cstrName = fpiData.cstrLastName;
              pdlg->m_cstrFullName = cstrTemp1;
              }

            pdlg->m_bUseResrchRefNum = fpiData.bUseResrchRefNum;
            if(pdlg->DoModal() == IDOK)
              {
              bool bChanged = false;
              if(fpiData.bUseResrchRefNum == TRUE)
                {
                if(fpiData.cstrResearchRefNum != pdlg->m_cstrName)
                  { // New research reference number.
                  PatientData.cstrResearchRefNum = pdlg->m_cstrName;
                  bChanged = true;
                  }
                }
              else
                {
                if(fpiData.cstrLastName != pdlg->m_cstrName)
                  { // New last name.
                  PatientData.cstrLastName = pdlg->m_cstrName;
                  bChanged = true;
                  }
                }
              if(bChanged == true)
                {
                // Cause new patient and study to be created.
                lPatientIDLocal = INV_LONG;
                //PatientData.lPatientID = INV_LONG;
                //StudyData.lStudyID = INV_LONG;
                }
              iDupAction = pdlg->m_iDupAction;
             }
            delete pdlg;

            ////cstrTitle.LoadString(IDS_WARNING);
            //cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
            //if((iMBSts = MessageBox(cstrTemp, cstrTitle, MB_YESNOCANCEL)) == IDNO)
            //  bOverwrite = false;
            //else if(iMBSts == IDCANCEL)
            //  bCancel = true;
            } // End of if(bSkipPatientData == false)
          } // End of if Patient already exists
        else
          iDupAction = IMPORT_DUP_CHANGE;
        }
      if(iDBSts == FAIL)
        {
        iRet = FAIL;
        break;
        }
      else if(iDBSts != SUCCESS)
        lPatientIDLocal = INV_LONG;
    
      //if(bCancel == false)
      if(iDupAction != IMPORT_DUP_SKIP)
        {
          // Save the Patient ID so if we get another study to import for this
          // patient, we don't have to ask to overwrite the patient again.
        puiaPatientID[uiImportCnt] = lPatientIDRemote;
        if(bSkipPatientData == false)
          {
          //if(bOverwrite == true)
            { // Either new patient or its OK to overwrite existing patient.
            //PatientData.cstrFirstName = fpiData.cstrFirstName;
            //PatientData.cstrLastName = fpiData.cstrLastName;
            //PatientData.cstrMI = fpiData.cstrMI;
            //PatientData.bUseResrchRefNum = fpiData.bUseResrchRefNum;
            //PatientData.cstrResearchRefNum = fpiData.cstrResearchRefNum;
            PatientData.lPatientID = lPatientIDLocal;
            // This will also create a new patient record if the name was changed.
            if((iSts = pDBLocal->iSave_patient(&PatientData)) == SUCCESS)
              {  // Save patient info if overwrite or new patient
              lPatientIDLocal = PatientData.lPatientID;
              if(iDupAction == IMPORT_DUP_CHANGE)
                {
                puiaPatientNewID[uiImportCnt] = lPatientIDLocal; // New ID in local database.
                }
              //piInfo.lPatientID = lPatientIDRemote;
              if((iSts = pDBRemote->iGet_patient_info(&piInfo)) == SUCCESS)
                {
                //if(iDupAction == IMPORT_DUP_CHANGE)
                //  piInfo.lPatientID = lPatientIDRemote;
                //else
                  piInfo.lPatientID = lPatientIDLocal;
                iSts = pDBLocal->iSave_patient_info(&piInfo);
                }
              }
            }
          }
        if(iSts == SUCCESS)
          { // Import the study.
            // Copy data file
            // Create an information message for the user if the destination data
            // file already exists.
            // skip this the the name was changed.
          cstrTemp1 = g_pLang->cstrLoad_string(ITEM_G_OVERWRITE_STUDY)
                      + "\n    " + g_pLang->cstrLoad_string(ITEM_G_OVERWRITE_STUDY1)
                      + "\n    " + g_pLang->cstrLoad_string(ITEM_G_OVERWRITE_STUDY2)
                      + "\n\n" + g_pLang->cstrLoad_string(ITEM_G_OVERWRITE_STUDY3);
          if(fpiData.bUseResrchRefNum == TRUE)
            cstrTemp.Format(cstrTemp1, fpiData.cstrResearchRefNum, "",
                            StudyData.cstrDateOfStudy);
            //cstrTemp.Format(IDS_OVERWRITE_STUDY, fpiData.cstrResearchRefNum,
            //                "", StudyData.cstrDateOfStudy);
          else
            cstrTemp.Format(cstrTemp1, fpiData.cstrFirstName, fpiData.cstrLastName,
                            StudyData.cstrDateOfStudy);
            //cstrTemp.Format(IDS_OVERWRITE_STUDY, fpiData.cstrFirstName,
            //                fpiData.cstrLastName, StudyData.cstrDateOfStudy);
            // If data file already exists, ask to overwrite.
            // If overwrite OK or new study, copy file and save study.
            // Extract the file name from the data file.
          cstrTemp1 = cstrGet_data_file_name(StudyData.cstrDataFile);
          // If patient name is changed, change the name of the datafile.
          if(iDupAction == IMPORT_DUP_CHANGE)
            { // Misty Snow - 09-25-2012.egg
            CString cstrOrigName = cstrTemp1;
            if(fpiData.bUseResrchRefNum == TRUE)
              { // PatientData has the new research reference number.
              if(cstrTemp1.Replace(fpiData.cstrResearchRefNum, PatientData.cstrResearchRefNum) == 0)
                cstrTemp1.Insert(0, PatientData.cstrResearchRefNum);
              }
            else
              { // PatientData has the new last name.
              if(cstrTemp1.Replace(fpiData.cstrLastName, PatientData.cstrLastName) == 0)
                cstrTemp1.Insert(0, PatientData.cstrLastName);
              }
            cstrDest.Format("%s\\%s", pDBLocal->cstrGet_db_path(), cstrTemp1);
            cstrSrc.Format("%s\\%s", pDBRemote->cstrGet_db_path(), cstrOrigName);
            // Change the study's file name.
            StudyData.cstrDataFile = cstrDest;
            }
          else
            {
            cstrDest.Format("%s\\%s", pDBLocal->cstrGet_db_path(), cstrTemp1);
            cstrSrc.Format("%s\\%s", pDBRemote->cstrGet_db_path(), cstrTemp1);
            }
          iFCSts = iCopy_file(cstrSrc, cstrDest, cstrTemp);
          if(iFCSts == FILE_COPY_NEW || iFCSts == FILE_COPY_OVERWRITTEN)
            { // File successfully copied, either new or overwritten.
              // Import the study and overwrite if it already exists.
            if(iFCSts == FILE_COPY_OVERWRITTEN)
              { // Data file already exists, look for the study.
              cstrTemp.Format("%s\\%s", pDBLocal->cstrGet_db_path(),
                              cstrGet_data_file_name(StudyData.cstrDataFile));
              lStudyIDLocal = pDBLocal->lFind_study(cstrTemp);
              if(lStudyIDLocal == INV_LONG)
                iFCSts = FILE_COPY_NEW;
              StudyData.lStudyID = lStudyIDLocal;
              }
            else
              StudyData.lStudyID = INV_LONG; // New study.
              // Save the study.
            StudyData.lPatientID = lPatientIDLocal;
            if(pDBLocal->iSave_study_data(&StudyData) == SUCCESS)
              { // Import the WhatIf records for this study.
              lStudyIDLocal = StudyData.lStudyID;
                // If existing study not overwritten, DON'T IMPORT WHATIF RECORDS.
              if(bOverwrite == true)
                { // Either overwritten existing study, or a new study.
                iImport_whatif(lStudyIDLocal, lStudyIDRemote, pDBLocal, pDBRemote, iFCSts);
                iImport_events(lStudyIDLocal, lStudyIDRemote, pDBLocal, pDBRemote, iFCSts);
                m_cImportProgressBar.StepIt();
                }
              }
            }
          }
        ++uiImportCnt;
        } // End of if(iDupAction != IMPORT_DUP_SKIP)
      }
    if(iDBSts == FAIL)
      iRet = FAIL;
    } // End of for next selected study.
  delete puiaPatientID;
  delete puiaPatientNewID;
  if(bCancel == true)
    iRet = SUCCESS_NO_ACTION;
  return(iRet);
  }

/********************************************************************
iCopy_file - Copy a file from the source to the destination.

  inputs: String containing the source file and path.
          String containing the destination file and path.

  return: TRUE if the file was successfully copied.
          FALSE if there was an error or the file was not overwritten.
********************************************************************/
short int CImportRemoteDB::iCopy_file(LPCTSTR strSrc, LPCTSTR strDest,
CString cstrOverwriteInfo)
  {
  short int iRet;

    // Fail if already exists.  make user decide to overwrite.
  if(CopyFile(strSrc, strDest, TRUE) == FALSE)
    { // Failed copying file.
    DWORD dwErr;
    char szMsg[256];
    CString cstrErr, cstrTitle;
  
    dwErr = GetLastError();
    if(dwErr == ERROR_FILE_EXISTS)
      { // File already exists, ask to overwrite.
      if(cstrOverwriteInfo.IsEmpty() == TRUE)
        cstrErr.Format(g_pLang->cstrLoad_string(ITEM_G_OVERWRITE_FILE), strDest);
        //cstrErr.Format(IDS_OVERWRITE_FILE, strDest);
      else
        cstrErr = cstrOverwriteInfo;
      //cstrTitle.LoadString(IDS_WARNING);
      cstrTitle = g_pLang->cstrLoad_string(ITEM_G_WARNING);
      if(MessageBox(cstrErr, cstrTitle, MB_YESNO) == IDYES)
        {
        if(CopyFile(strSrc, strDest, FALSE) == FALSE)
          dwErr = GetLastError();
        else
          iRet = FILE_COPY_OVERWRITTEN;
        }
      else
        iRet = FILE_COPY_NOT; // Not overwriting file.
      }
    if(dwErr != ERROR_FILE_EXISTS)
      { // File copy error.
      iRet = FILE_COPY_FAIL;
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, 0L, szMsg, 256, NULL);
      //cstrErr.Format(IDS_FILE_COPY_ERR, strSrc, strDest);
      cstrErr.Format(g_pLang->cstrLoad_string(ITEM_G_FILE_COPY_ERR), strSrc, strDest);
      cstrErr += "\r\n";
      cstrErr += szMsg;
      //cstrTitle.LoadString(IDS_ERROR1);
      cstrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
      MessageBox(cstrErr, cstrTitle);
      theApp.vLog((LPCTSTR)cstrErr);
      }
    }
  else
    iRet = FILE_COPY_NEW;
  return(iRet);
  }

/********************************************************************
iImport_whatif - Import all the records from the remote WhatIf table
                 for the specified study to the local database.
                 If the study was overwritten (i.e. previously existed in
                 the local database), all the WhatIf records are first
                 deleted.

  inputs: Study ID for the local database.
          Study ID for the remote database.
          Pointer to the database object for the local database.
          Pointer to the database object for the remote database.
          Indicates if the study is a new study to the local database or
            was overwritten.

  return: SUCCESS if the table was imported.
          FAIL if there was an error.
********************************************************************/
short int CImportRemoteDB::iImport_whatif(long lStudyIDLocal, long lStudyIDRemote,
CDbaccess *pDBLocal, CDbaccess *pDBRemote, short int iStudyType)
  {
  short int iPos, iRet = SUCCESS;
  WHAT_IF_RECORD wi;

    // If overwritten study, delete all WhatIf records for the study.
  if(iStudyType == FILE_COPY_OVERWRITTEN)
    { // Existing study overwritten, delete all WhatIf records.
    iRet = pDBLocal->iDelete_whatif_for_study(lStudyIDLocal);
    }
  if(iRet == SUCCESS)
    { // If new study or overwritten study, import all WhatIf records.
    iPos = FIRST_RECORD;
    wi.lStudyID = lStudyIDRemote;
    while(pDBRemote->iFind_next_whatif(iPos, &wi) == SUCCESS)
      {
      wi.lWhatIfID = INV_LONG;
      wi.lStudyID = lStudyIDLocal;
      if((iRet = pDBLocal->iSave_whatif_data(&wi)) == FAIL)
        break;
      iPos = NEXT_RECORD;
      }
    }

  return(iRet);
  }

/********************************************************************
iImport_events - Import all the records from the remote Events table
                 for the specified study to the local database.
                 If the study was overwritten (i.e. previously existed in
                 the local database), all the Event records are first
                 deleted.

  inputs: Study ID for the local database.
          Study ID for the remote database.
          Pointer to the database object for the local database.
          Pointer to the database object for the remote database.
          Indicates if the study is a new study to the local database or
            was overwritten.

  return: SUCCESS if the table was imported.
          FAIL if there was an error.
********************************************************************/
short int CImportRemoteDB::iImport_events(long lStudyIDLocal, long lStudyIDRemote,
CDbaccess *pDBLocal, CDbaccess *pDBRemote, short int iStudyType)
  {
  short int iRet = SUCCESS;
  EVENT_RECORD Event;

    // If overwritten study, delete all WhatIf records for the study.
  if(iStudyType == FILE_COPY_OVERWRITTEN)
    { // Existing study overwritten, delete all WhatIf records.
    iRet = pDBLocal->iDelete_events(lStudyIDLocal);
    }
  if(iRet == SUCCESS)
    { // If new study or overwritten study, import all WhatIf records.
    Event.lStudyID = lStudyIDRemote;
    while(pDBRemote->iGet_event(&Event) == SUCCESS)
      {
      Event.lStudyID = lStudyIDLocal;
      if((iRet = pDBLocal->iSave_event(&Event)) == FAIL)
        break;
      Event.lStudyID = INV_LONG;
      }
    }

  return(iRet);
  }

/********************************************************************
cstrGet_data_file_name - Get the file name from the complete path and
                         file name.

  inputs: CString object containing the complete path/file name.

  return: CString object containing the file name.
********************************************************************/
CString CImportRemoteDB::cstrGet_data_file_name(CString cstrFilePath)
  {
  CString cstrFile;
  int iIndex;

  if((iIndex = cstrFilePath.ReverseFind('\\')) > 0)
    cstrFile = cstrFilePath.Right(cstrFilePath.GetLength() - iIndex - 1);
  else
    cstrFile = cstrFilePath;
  return(cstrFile);
  }

/********************************************************************
vFill_patient_listbox - Fill in the listbox with patient names and dates
                        of study from the database.

  inputs: None.

  return: None.
********************************************************************/
void CImportRemoteDB::vFill_patient_listbox()
  {
  CString cstrItem1, cstrItem2, cstrDate;
  FIND_PATIENT_INFO fpiData;
  FIND_NEXT_STUDY fnsData;
  WHAT_IF wi;
  short int iPatientPos, iStudyPos, iPatientSts, iStudySts;
  int iIndex, iCnt;
  bool bAddToLstBox;

  // First make sure the listbox is empty.
  m_cImportList.DeleteAllItems();
    // Display the hourglass cursor.
  SetCursor(LoadCursor(NULL, IDC_WAIT));
  ShowCursor(TRUE);
    // Fill in the list box from the database.
  iPatientSts = SUCCESS;
  iPatientPos = FIRST_RECORD;
  iCnt = 0;
  while(iPatientSts == SUCCESS)
    {
    fpiData.bAll = TRUE;
    if((iPatientSts = m_pImpDB->iFind_next_patient(iPatientPos, &fpiData)) == SUCCESS)
      {
      iStudySts = SUCCESS;
      iStudyPos = FIRST_RECORD;
        // Find out if the research reference number is being used and if it
        // is, use it instead of the patient name.
      if(fpiData.bUseResrchRefNum == TRUE)
        cstrItem1 = fpiData.cstrResearchRefNum;
      else
        cstrItem1 = cstrFormat_name(&fpiData);
      fnsData.lPatientID = fpiData.lPatientID;
      while(iStudySts == SUCCESS)
        {
        if((iStudySts = m_pImpDB->iFind_next_study(iStudyPos, &fnsData)) == SUCCESS)
          {
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
            iIndex = m_cImportList.InsertItem(iCnt, cstrItem1); // insert new item, patient name
            cstrDate.Empty();
#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
#if EGGSAS_READER == READER_DEPOT
            //if(fnsData.cstrDate.GetLength() <= 10)
            if(g_pConfig->m_bHideStudyDates == false)
              { // showing all study dates.
              if(fnsData.cstrDate.GetLength() <= 10)
                cstrDate = fnsData.cstrDate; // Date of study
              else
                { // Decrypt the study date and display it.
                cstrDate = cstrDecode_study_date(fnsData.cstrDate);
                }
              }
            else
              cstrDate.Empty(); // study dates are hidden.
#elif EGGSAS_READER == READER_DEPOT_E
            //if(fnsData.cstrDate.GetLength() <= 10)
            if(g_pConfig->m_bHideStudyDates == false)
              { // showing all study dates.
              if(fnsData.cstrDate.GetLength() <= 10)
                cstrDate = fnsData.cstrDate; // Date of study
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

            if(cstrDate.IsEmpty() == false)
              m_cImportList.SetItemText(iIndex, 1, cstrDate); // Date of study

            m_cImportList.SetItemData(iIndex, (DWORD)fnsData.lStudyID); // Study ID
            ++iCnt;
            }
          }
        iStudyPos = NEXT_RECORD;
        }
      }
    iPatientPos = NEXT_RECORD;
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
CString CImportRemoteDB::cstrFormat_name(FIND_PATIENT_INFO *pfpiData)
  {
  CString cstrName;

  if(pfpiData->cstrMI.IsEmpty() == FALSE)
    cstrName.Format("%s, %s %s", pfpiData->cstrLastName, pfpiData->cstrFirstName,
                     pfpiData->cstrMI);
  else
    cstrName.Format("%s, %s", pfpiData->cstrLastName, pfpiData->cstrFirstName);
  return(cstrName);
  }

/********************************************************************
iSetup_databases - Set up the databases so the local and remote database
                    object point to the correct database.

  inputs: None.

  return: SUCCESS if the remote databaseis connect to.
          FAIL if there is an error.
********************************************************************/
int CImportRemoteDB::iSetup_databases()
  {
  int iRet; // = SUCCESS;

  if(m_cstrDBLocation.IsEmpty() == FALSE)
    { // There is a database location.
      // Find out what database the program's database access object is using and
      // save it.
    m_cstrCurrentDBPath = m_pDoc->m_pDB->cstrGet_db_file_path();
      // Set the program's database access object to the local database and
      // create a new database access object connected to the reader database.
    if(g_pConfig->cstrGet_item(CFG_REMOTE_DB_PATH).IsEmpty() == true)
      {
      if(m_cstrCurrentDBPath != DSN_DEFAULT)
        m_pDoc->vChange_db(LOCAL_DB, "");
      }
    m_pImpDB = new CDbaccess(m_cstrDBLocation);
    if((iRet = m_pImpDB->iDBConnect()) == SUCCESS)
      { // Now make sure the database version is what the program is expecting.
        // If not, try to update it.
      if((iRet = m_pImpDB->iValidate_version()) != SUCCESS)
        { // The imported database is not the correct version.  Try updating
          // the imported database to the current version.
        // first disconnect from the imported database.
        delete m_pImpDB;
        m_pImpDB = NULL;
        CDBUpdate *pdbUpd = new CDBUpdate();
        // Update the database.
        pdbUpd->iUpdate_database(m_cstrDBLocation);

        ////  Starting dbupdate.exe doesn't work in Win10.
        //STARTUPINFO si;
        //PROCESS_INFORMATION pi;
        ////char szText[1024];
        ////int iLen;
        //CString cstrCmd;

        //ZeroMemory(&si,sizeof(si));
        //ZeroMemory(&pi,sizeof(pi));
        //si.cb=sizeof(si);
        //CWaitCursor wait; // Change to wait cursor in case update takes a while.
        //// first disconnect from the imported database.
        //delete m_pImpDB;
        //// Enclose the imported database in double quotes in case it has spaces in it.
        //// It is needed in order for the database update program to properly parse the
        //// command line.
        //CString cstrImpDB;
        //cstrImpDB.Format("\"%s\"", m_cstrDBLocation);
        ////szText[0] = '"';
        ////strcpy(&szText[1], m_cstrDBLocation);
        ////iLen = strlen(szText);
        ////szText[iLen] = '"';
        ////szText[iLen + 1] = NULL;
        ////// Format the database update program path and name.
        //char szCurDir[512];
        //GetCurrentDirectory(512, szCurDir); // Path program is in.
        //CString cstrPgm, cstrPgm1;
        //cstrPgm1.Format("\"%s\\%s\"", szCurDir, "dbupdate.exe");
        //cstrPgm = "dbupdate.exe";

        //// Format the arguments on the command line.
        //// The first argument is the path and name of the program to start.
        //// The second argument is the path and name of the database to update.
        //cstrCmd.Format("%s %s", cstrPgm, cstrImpDB);
        //theApp.vLog("Starting dbupdate.exe: %s", cstrCmd);

        //CString cstrMsg;
        //cstrMsg.Format("Starting dbupdate.exe: %s %s", cstrPgm, cstrCmd);
        //MessageBox(cstrMsg);

        //SECURITY_ATTRIBUTES sa;
        //sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        //sa.lpSecurityDescriptor = NULL;
        //sa.bInheritHandle = TRUE;

        //if(CreateProcess(cstrPgm, (LPTSTR)cstrCmd.GetBuffer(), &sa, NULL, FALSE, 0, NULL, NULL, &si, &pi) == 0)
        //  {
        //  char szBuf[1024]; 
        //  char szMsg[256];
        //  DWORD dwErr = GetLastError(); 

        //  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, 0L, szMsg, 256, NULL);
        //  sprintf(szBuf, "dbupdate failed: GetLastError returned %u: %s\n", dwErr, szMsg); 

        //  MessageBox(szBuf, "Error", MB_OK); 
        //}
        //// Wait for the database update program to terminate.
        //WaitForSingleObject( pi.hProcess, INFINITE );
        //// Release handles
        //CloseHandle(pi.hProcess);
        //CloseHandle(pi.hThread);
        //wait.Restore(); // Restore the wait cursor to the normal cursor.

        //m_cstrDBLocation.ReleaseBuffer();
        // Now connect again to the updated database.
        m_pImpDB = new CDbaccess(m_cstrDBLocation);
        if((iRet = m_pImpDB->iDBConnect()) == SUCCESS)
          { // Now make sure the database version is what the program is expecting.
            // If not, go no further.
          if((iRet = m_pImpDB->iValidate_version()) != SUCCESS)
            { // Wrong database version.  display error message.
              // empty the listbox so the user doesn't get confused if there was a 
              // previously good database.
            m_cImportList.DeleteAllItems();
            m_pDoc->vDisplay_version_error(m_pImpDB);
            }
          }
        }
      }
    }
  else
    iRet = FAIL;
  return(iRet);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CImportRemoteDB::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_IMPORT_REMOTE_DB);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_BR_IMPORT_REMOTE_DB));

    GetDlgItem(IDC_STATIC_IMPORT_DB_FROM)->SetWindowText(g_pLang->cstrGet_text(ITEM_IDB_IMPORT_DB));
    GetDlgItem(IDC_BTN_BROWSE)->SetWindowText(g_pLang->cstrGet_text(ITEM_IDB_SELECT_DB));
    GetDlgItem(IDC_BTN_SELECT_ALL)->SetWindowText(g_pLang->cstrGet_text(ITEM_IDB_SEL_ALL_STUDIES));
    GetDlgItem(IDC_BTN_IMPORT)->SetWindowText(g_pLang->cstrGet_text(ITEM_IDB_IMPORT));
    GetDlgItem(IDC_STATIC_PROGRESS_TITLE)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_IMPORT_PROG));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CLOSE));
    GetDlgItem(IDC_IMPORT_REMOTE_DB_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    g_pLang->vEnd_section();
    }
  }

