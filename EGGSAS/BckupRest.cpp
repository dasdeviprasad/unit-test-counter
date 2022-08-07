/********************************************************************
BckupRest.cpp

Copyright (C) 2004,2007,2009,2012,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CBckupRest class.

  This class performs backups and restores of the database and
    the data files.

HISTORY:
20-JAN-04  RET  New.
16-MAR-04  RET  Add help button and message handler.
17-MAR-04  RET  Change OnBnClickedBackup() so that the backup fields are
                  not shown unless the user selects a Save As path.
                Change OnBnClickedRestore() so that the restore fields are
                  not shown unless the user selects a File Open path.
09-JUL-07  RET  Add writing backup path and reading restore path from/to
                  the INI file.
21-DEC-07  RET  Version 2.03
                  Change vSetup_progress_bar() to initialize the progress bar
                    by one less count so it finishes when just a couple of files
                    are backed up/restored.
                  Change iBackup_restore() to restore the starting directory
                    at the end of the operation so that if a USB drive is used,
                    it is released and can be removed.
03-FEB-09  RET  Version 2.08g
                  Changed OnBnClickedRestore() to remove the check for being
                    connected to the database.  It doesn't seem to be needed.
22-MAR-09  RET  Version 2.08k
                  Add validating data files.
                    Change OnBnClickedStart():
                      - Check that every study has a data file before the backup.
                      - Check that every study has a data file in the backup
                        directory after the backup.
                    Change iCopy_egg_files() to validate the file header
                      before and after copying files.  This is for both backup
                      and restore.  If a file is not valid, don't copy it.
                    Change vReport_sts() to display complete message with
                      errors if there were errors.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedBckuprestHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "BckupRest.h"
#include ".\bckuprest.h"
#include "Dbaccess.h"
#include "util.h"
#include "BckupRestErrs.h"

// CBckupRest dialog

IMPLEMENT_DYNAMIC(CBckupRest, CDialog)

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the currently active document.

********************************************************************/
CBckupRest::CBckupRest(CEGGSASDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CBckupRest::IDD, pParent)
  , m_cstrProgress(_T(""))
  , m_cstrBckupRestPath(_T(""))
  , m_cstrToFrom(_T(""))
  , m_cstrFileCopied(_T(""))
  {

  m_pDoc = pDoc;
  }

/********************************************************************
Destructor

  Destroy the dialog box.
********************************************************************/
CBckupRest::~CBckupRest()
  {
  //int iIndex, iMax;
  //CString cstrTemp;

  //if(m_cstraErrs.GetCount() > 0)
  //  { // Clear out the error strings.
    //iMax = m_cstraErrs.GetCount();
    //for(iIndex = 0; iIndex < iMax; ++iIndex)
    //  {
    //  cstrTemp = m_cstraErrs[0];
    //  m_cstraErrs.RemoveAt(0, 1);
    //  cstrTemp.Empty();
    //  }
    m_cstraErrs.RemoveAll();
    //}
  }

void CBckupRest::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_BCKUP_REST_PROG_TITLE, m_cstrProgress);
DDX_Control(pDX, IDC_PROGRESS1, m_cProgress);
DDX_Text(pDX, IDC_BCKUP_REST_PATH, m_cstrBckupRestPath);
DDX_Text(pDX, IDC_BCKUP_REST_TOFROM, m_cstrToFrom);
DDX_Text(pDX, IDC_BCKUP_REST_FILENAME, m_cstrFileCopied);
}


BEGIN_MESSAGE_MAP(CBckupRest, CDialog)
  ON_BN_CLICKED(IDC_BACKUP, OnBnClickedBackup)
  ON_BN_CLICKED(IDC_RESTORE, OnBnClickedRestore)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDC_START, OnBnClickedStart)
  ON_BN_CLICKED(IDC_BCKUPREST_HELP, OnBnClickedBckuprestHelp)
END_MESSAGE_MAP()

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Hide various fields until the user is ready to perform the backup
      or restore.
********************************************************************/
BOOL CBckupRest::OnInitDialog()
  {
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  vSet_text();
  GetDlgItem(IDC_BCKUP_REST_PROG_TITLE)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_PROGRESS1)->ShowWindow(SW_HIDE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

// CBckupRest message handlers

/********************************************************************
OnBnClickedBackup

  Message handler for Backup button.
    Set up the data for the backup.
    Set up the dialog box fields for the backup.
********************************************************************/
void CBckupRest::OnBnClickedBackup()
  {

  m_iBckupRest = BACKUP;
  if(iBackup_restore() ==  SUCCESS)
    {
    GetDlgItem(IDC_BCKUP_REST_PROG_TITLE)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_PROGRESS1)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_FILE_COPIED)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_BCKUP_REST_FILENAME)->ShowWindow(SW_SHOW);
    m_cstrProgress = g_pLang->cstrLoad_string(ITEM_G_BACKUP_PROGRESS);
    //m_cstrProgress.LoadString(IDS_PROGRESS_BACKUP);
    GetDlgItem(IDC_START)->EnableWindow(TRUE);
    vSetup_progress_bar(); // Initialize the progress bar.
    UpdateData(FALSE);
    }
  }

/********************************************************************
OnBnClickedRestore

  Message handler for Restore button.
    Check the database connection and if we are connected, don't allow
      the restore.
    Set up the data for the restore.
    Set up the dialog box fields for the restore.
********************************************************************/
void CBckupRest::OnBnClickedRestore()
  {

  //if(m_pDoc->m_pDB->m_bDBConnected == true)
  //  {
  //  CString cstrMsg, cstrTitle;
  //  cstrMsg.LoadString(IDS_RESTORE_RESTART);
  //  cstrTitle.LoadString(IDS_WARNING);
  //  MessageBox(cstrMsg, cstrTitle);
  //  }
  //else
    { // Database hasn't been connected yet, proceed with restore.
    m_iBckupRest = RESTORE;
    if(iBackup_restore() == SUCCESS)
      {
      GetDlgItem(IDC_BCKUP_REST_PROG_TITLE)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_PROGRESS1)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_FILE_COPIED)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_BCKUP_REST_FILENAME)->ShowWindow(SW_SHOW);
      //m_cstrProgress.LoadString(IDS_PROGRESS_RESTORE);
      m_cstrProgress = g_pLang->cstrLoad_string(ITEM_G_RESTORE_PROGRESS);
      GetDlgItem(IDC_START)->EnableWindow(TRUE);
      vSetup_progress_bar(); // Initialize the progress bar.
      UpdateData(FALSE);
      }
    }
  }

/********************************************************************
OnBnClickedOk

  Message handler for OK button.
    Does nothing.
********************************************************************/
void CBckupRest::OnBnClickedOk()
  {
  // TODO: Add your control notification handler code here
  OnOK();
  }

/********************************************************************
OnBnClickedStart

  Message handler for Start button.
    Perform the backup or restore.
********************************************************************/
void CBckupRest::OnBnClickedStart()
  {
  CString cstrTemp;
  int iRet;
  BOOL bNoAutoOverwrite;

  BeginWaitCursor();
  if(m_iBckupRest == BACKUP)
    {
    bNoAutoOverwrite = FALSE; // Backup, "Save As" dialog box would have already
                              // asked to overwrite.
      // Save the backup path.
    //AfxGetApp()->WriteProfileString(INI_SEC_GENERAL, INI_ITEM_BACKUP_PATH, m_cstrDestPath);
    g_pConfig->vWrite_item(CFG_BACKUPPATH, m_cstrDestPath);
    }
  else
    bNoAutoOverwrite = TRUE; // Restore
  m_cstrFileCopied = m_pDoc->m_pDB->cstrGet_db_name();
  UpdateData(FALSE);
    // Initialize error list.
  m_cstraErrs.RemoveAll();
  if(m_iBckupRest == BACKUP)
    bCheck_data_files(); // Only for backup: Check for studies that have no data file.
  if((iRet = iCopy_one_file((LPCTSTR)m_cstrDestPath, (LPCTSTR)m_cstrSrcPath,
  (LPCTSTR)m_pDoc->m_pDB->cstrGet_db_name(), bNoAutoOverwrite)) == SUCCESS)
    { // Database table is copied.
    m_cProgress.StepIt(); // Update the progress bar.
      // If a restore, make sure the file is read/write.
    if(m_iBckupRest == RESTORE)
      { // Create the file name.
      cstrTemp.Format("%s\\%s", m_cstrDestPath, (LPCTSTR)m_pDoc->m_pDB->cstrGet_db_name());
      SetFileAttributes(cstrTemp, FILE_ATTRIBUTE_NORMAL);
      }
      // Copy the data files.
//      // Get the path of the data files from the INI file.
//    m_cstrSrcPath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_DATAPATH, "");
    iRet = iCopy_egg_files();
       // For backup only: check for studies that have no data file in backup
       // directory.  Only get error on a particular data file if there were
       // no other errors on that file.
    if(m_iBckupRest == BACKUP)
      bCheck_bckup_data_files();
    }
  EndWaitCursor();
  vReport_sts(iRet);
  if(m_cstraErrs.GetCount() > 0)
    {
    CBckupRestErrs *pdlg = new CBckupRestErrs(this);
    pdlg->DoModal();
    delete pdlg;
    }
  }

/********************************************************************
OnBnClickedBckuprestHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CBckupRest::OnBnClickedBckuprestHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_BCKUPREST_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_BCKUPREST_HELP);
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
iBackup_restore - Display the "Save As" or "File Open" dialog box to
                  get the path for the backup or restore.

  inputs: None.

  return: SUCCESS if user selected a path.
          FAIL if user cancels.
********************************************************************/
int CBckupRest::iBackup_restore(void)
  {
  CFileDialog *pdlg;
  CString cstrDlgboxPath;
  int iIndex, iRet;
  BOOL bOpenFileDlgbox;
  char szCurDir[1024];

  GetCurrentDirectory(1024, szCurDir);
  if(m_iBckupRest == BACKUP)
    bOpenFileDlgbox = FALSE; // Backup, display a "Save As" dialog box.
  else
    bOpenFileDlgbox = TRUE; // Restore, display a "File Open" dialog box.
  pdlg = new CFileDialog(bOpenFileDlgbox, ".egg", "3CPM EGGSAS.MDB");//, 
//                         OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, 
//                         "EGG Files (*.egg)|*.egg|All Files (*.*)|*.*||", this);
  if(m_iBckupRest == RESTORE)
    { // Set the starting directory.
    cstrDlgboxPath = g_pConfig->cstrGet_item(CFG_BACKUPPATH);
    //cstrDlgboxPath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_BACKUP_PATH, "");
    if(cstrDlgboxPath.IsEmpty() == FALSE)
      pdlg->m_pOFN->lpstrInitialDir = cstrDlgboxPath;
   }
  if(pdlg->DoModal() == IDOK)
    { // User selected a data file.
    iRet = SUCCESS;
    cstrDlgboxPath = pdlg->GetPathName(); // Destination/source Path and file
      // Remove the file name from the path.
    if((iIndex = cstrDlgboxPath.ReverseFind('\\')) >= 0)
      cstrDlgboxPath.Delete(iIndex, cstrDlgboxPath.GetLength() - iIndex);
    if(m_iBckupRest == BACKUP)
      { // Backup
      m_cstrSrcPath = m_pDoc->m_pDB->cstrGet_db_path(); // Path of database and data files to backup.
      m_cstrDestPath = cstrDlgboxPath;
      //m_cstrToFrom.LoadString(IDS_BACKUP_TO);
      m_cstrToFrom = g_pLang->cstrLoad_string(ITEM_G_BACKUP_TO);
      m_cstrBckupRestPath = m_cstrDestPath;
      }
    else
      { // Restore
      m_cstrDestPath = m_pDoc->m_pDB->cstrGet_db_path(); // Path of database and data files to backup.
      m_cstrSrcPath = cstrDlgboxPath;
      //m_cstrToFrom.LoadString(IDS_RESTORE_FROM);
      m_cstrToFrom = g_pLang->cstrLoad_string(ITEM_G_RESTORE_FROM);
      m_cstrBckupRestPath = m_cstrSrcPath;
      }
    UpdateData(FALSE);
    }
  else
    iRet = FAIL;
  delete pdlg;
    // Restore the directory we started from.  If backup/restore used a USB drive
    // this releases the USB drive so it can be removed.
  SetCurrentDirectory(szCurDir);
  return(iRet);
  }

/********************************************************************
vReport_sts - Report the final status of the backup/restore.

  inputs: Status.

  return: None.
********************************************************************/
void CBckupRest::vReport_sts(int iSts)
  {
  CString cstrMsg;

  cstrMsg.Empty();
  if(iSts == SUCCESS)
    {
    if(m_cstraErrs.GetCount() == 0)
      {
      if(m_iBckupRest == BACKUP)
        cstrMsg = g_pLang->cstrLoad_string(ITEM_G_BACKUP_SUCCESS);
        //cstrMsg.LoadString(IDS_BACKUP_SUCCESS);
      else
        cstrMsg = g_pLang->cstrLoad_string(ITEM_G_RESTORE_SUCCESS);
        //cstrMsg.LoadString(IDS_RESTORE_SUCCESS);
      }
    else
      {
      if(m_iBckupRest == BACKUP)
        cstrMsg = g_pLang->cstrLoad_string(ITEM_G_BACKUP_ERRORS);
        //cstrMsg.LoadString(IDS_BCKUP_COMP_ERRS);
      else
        cstrMsg = g_pLang->cstrLoad_string(ITEM_G_RESTORE_ERRORS);
        //cstrMsg.LoadString(IDS_REST_COMP_ERRS);
      }
    }
  else if(iSts == FAIL)
    {
    if(m_iBckupRest == BACKUP)
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_BACKUP_FAILED);
      //cstrMsg.LoadString(IDS_BACKUP_FAIL);
    else
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_RESTORE_RAILED);
      //cstrMsg.LoadString(IDS_RESTORE_FAIL);
    }
  if(cstrMsg.IsEmpty() == FALSE)
    {
    MessageBox(cstrMsg);
    }
  }

/********************************************************************
iCopy_egg_files - Copy ".egg" files from the source to the destination.

  inputs: None.

  return: SUCCESS if all files were successfully copied.
          FAIL if there was an error.
********************************************************************/
//// Do a new method without using file attributes.
int CBckupRest::iCopy_egg_files(void)
  {
  HANDLE hFindFile;
  WIN32_FIND_DATA w32fdFileData;
  CString cstrTemp, cstrMsg, cstrErrTitle;
  short int iRet = SUCCESS;

     // Create a path and filename
  cstrTemp.Format("%s\\*.egg", m_cstrSrcPath);
  if((hFindFile = FindFirstFile((LPCTSTR)cstrTemp, &w32fdFileData)) 
  != INVALID_HANDLE_VALUE)
    { // There is at least one file in the directory.
      // Display the file being copied.
    m_cstrFileCopied = w32fdFileData.cFileName;
    UpdateData(FALSE);
    if(bValidate_file(w32fdFileData.cFileName, m_cstrSrcPath) == true)
      {
        // Copy the first file.
      if((iRet = iCopy_one_file((LPCTSTR)m_cstrDestPath, (LPCTSTR)m_cstrSrcPath,
      w32fdFileData.cFileName, FALSE)) == FAIL)
        {
        cstrMsg = g_pLang->cstrGet_text(ITEM_G_COPY_ERR_CONT);
        //cstrMsg.LoadString(IDS_COPY_ERR_CONTINUE);
        cstrErrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
        //cstrErrTitle.LoadString(IDS_ERROR);
        if(AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErrTitle, MB_YESNO | MB_ICONSTOP)
        == IDYES)
          iRet = SUCCESS; // Continue with backup.
        }
      else
        { // Copy file succeeded.  If a restore, make sure the file is read/write.
        if(m_iBckupRest == RESTORE)
          { // Create the file name.
          cstrTemp.Format("%s\\%s", m_cstrDestPath, w32fdFileData.cFileName);
          SetFileAttributes(cstrTemp, FILE_ATTRIBUTE_NORMAL);
          }
        // Make sure the file got copied OK.
        bValidate_file(w32fdFileData.cFileName, m_cstrDestPath);
        }
      m_cProgress.StepIt(); // Update the progress bar.
      }
        // Copy all the remaining files.
    while(iRet == SUCCESS && FindNextFile(hFindFile, &w32fdFileData) != 0)
      {
        // Display the file being copied.
      m_cstrFileCopied = w32fdFileData.cFileName;
      UpdateData(FALSE);
      if(bValidate_file(w32fdFileData.cFileName, m_cstrSrcPath) == true)
        {
          // Copy the file.
        if((iRet = iCopy_one_file((LPCTSTR)m_cstrDestPath, (LPCTSTR)m_cstrSrcPath,
        w32fdFileData.cFileName, FALSE)) == FAIL)
          {
          //cstrMsg.LoadString(IDS_COPY_ERR_CONTINUE);
          //cstrErrTitle.LoadString(IDS_ERROR);
          cstrMsg = g_pLang->cstrLoad_string(ITEM_G_COPY_ERR_CONT);
          cstrErrTitle = g_pLang->cstrLoad_string(ITEM_G_ERROR);
          if(AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErrTitle, MB_YESNO | MB_ICONSTOP)
          == IDYES)
            iRet = SUCCESS; // Continue with backup.
          }
        else
          { // Copy file succeeded.  If a restore, make sure the file is read/write.
          if(m_iBckupRest == RESTORE)
            { // Create the file name.
            cstrTemp.Format("%s\\%s", m_cstrDestPath, w32fdFileData.cFileName);
            SetFileAttributes(cstrTemp, FILE_ATTRIBUTE_NORMAL);
            }
          // Make sure the file got copied OK.
          bValidate_file(w32fdFileData.cFileName, m_cstrDestPath);
          }
        }
      m_cProgress.StepIt(); // Update the progress bar.
      }
    FindClose(hFindFile);
    }
  return(iRet);
  }

/********************************************************
int CBckupRest::iCopy_egg_files(void)
  {
  HANDLE hFindFile;
  WIN32_FIND_DATA w32fdFileData;
  CString cstrTemp, cstrMsg, cstrErrTitle;
  DWORD dwFileAttr, dwSrcFileAttrib;
  short int iRet = SUCCESS;

  dwFileAttr = FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE;
     // Create a path and filename
  cstrTemp.Format("%s\\*.egg", m_cstrSrcPath);
  if((hFindFile = FindFirstFile((LPCTSTR)cstrTemp, &w32fdFileData)) 
  != INVALID_HANDLE_VALUE)
    { // There is at least one file in the directory.
      // Pick out all those files and copy to the destination path.
    if((dwSrcFileAttrib = (w32fdFileData.dwFileAttributes & dwFileAttr)) == 0)
      { // Found no files with normal attributes, if the restore directory or
        // drive is read only, we need to set the read only attribute, so do this.
      if(m_iBckupRest == RESTORE)
        {
        dwFileAttr = FILE_ATTRIBUTE_READONLY;
        dwSrcFileAttrib = (w32fdFileData.dwFileAttributes & dwFileAttr);
        }
      }
    if(dwSrcFileAttrib != 0)
      {  // Display the file being copied.
      m_cstrFileCopied = w32fdFileData.cFileName;
      UpdateData(FALSE);
        // Copy the first file.
      if((iRet = iCopy_one_file((LPCTSTR)m_cstrDestPath, (LPCTSTR)m_cstrSrcPath,
      w32fdFileData.cFileName, FALSE)) == FAIL)
        {
        cstrMsg.LoadString(IDS_COPY_ERR_CONTINUE);
        cstrErrTitle.LoadString(IDS_ERROR);
        if(AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErrTitle, MB_YESNO | MB_ICONSTOP)
        == IDYES)
          iRet = SUCCESS; // Continue with backup.
        }
      else
        { // Copy file succeeded.  If a restore, make sure the file is read/write.
        if(m_iBckupRest == RESTORE)
          { // Create the file name.
          cstrTemp.Format("%s\\%s", m_cstrDestPath, w32fdFileData.cFileName);
          SetFileAttributes(cstrTemp, FILE_ATTRIBUTE_NORMAL);
          }
        }
      m_cProgress.StepIt(); // Update the progress bar.
      }
      // Copy all the remaining files.
    while(iRet == SUCCESS && FindNextFile(hFindFile, &w32fdFileData) != 0)
      {
      if((w32fdFileData.dwFileAttributes & dwFileAttr) != 0)
        {  // Display the file being copied.
        m_cstrFileCopied = w32fdFileData.cFileName;
        UpdateData(FALSE);
          // Copy the file.
        if((iRet = iCopy_one_file((LPCTSTR)m_cstrDestPath, (LPCTSTR)m_cstrSrcPath,
        w32fdFileData.cFileName, FALSE)) == FAIL)
          {
          cstrMsg.LoadString(IDS_COPY_ERR_CONTINUE);
          cstrErrTitle.LoadString(IDS_ERROR);
          if(AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErrTitle, MB_YESNO | MB_ICONSTOP)
          == IDYES)
            iRet = SUCCESS; // Continue with backup.
          }
        else
          { // Copy file succeeded.  If a restore, make sure the file is read/write.
          if(m_iBckupRest == RESTORE)
            { // Create the file name.
            cstrTemp.Format("%s\\%s", m_cstrDestPath, w32fdFileData.cFileName);
            SetFileAttributes(cstrTemp, FILE_ATTRIBUTE_NORMAL);
            }
          }
        m_cProgress.StepIt(); // Update the progress bar.
        }
      }
    FindClose(hFindFile);
    }
  return(iRet);
  }
******************************************************/

/********************************************************************
vSetup_progress_bar - Initialize the progress bar.  To do this we have
                      to count the files.

  inputs: None.

  return: None.
********************************************************************/
void CBckupRest::vSetup_progress_bar(void)
  {
  HANDLE hFindFile;
  WIN32_FIND_DATA w32fdFileData;
  DWORD dwFileAttr;
  CString cstrTemp; //, cstrSrcPath;
  int iCnt, iDBCnt;

  iCnt = 1; // Start with 1 for the database.
  dwFileAttr = FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE;
     // Create a path and filename
  //cstrSrcPath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_DATAPATH, "");
  cstrTemp.Format("%s\\*.egg", m_cstrSrcPath);
  if((hFindFile = FindFirstFile((LPCTSTR)cstrTemp, &w32fdFileData)) 
  != INVALID_HANDLE_VALUE)
    { // There is at least one file in the directory.
      // Count all the files to be copied.
    if((w32fdFileData.dwFileAttributes & dwFileAttr) != 0)
      ++iCnt;
    while(FindNextFile(hFindFile, &w32fdFileData) != 0)
      {
      if((w32fdFileData.dwFileAttributes & dwFileAttr) != 0)
        ++iCnt;
      }
    FindClose(hFindFile);
    }
  if(m_iBckupRest == BACKUP)
    {
    if((m_pDoc->m_pDB->iGet_study_count(&iDBCnt)) == SUCCESS)
      iCnt += (2 * iDBCnt);
    }
  if(iCnt > 0)
    { // Set the progress bar range to one more than the number of files
      // so that it won't wrap around.
    m_cProgress.SetRange(0, iCnt); // + 1);
    m_cProgress.SetPos(0);
    m_cProgress.SetStep(1);
    }
  }

/********************************************************************
bValidate_file - Make sure the file exists and that the file header
                 is valid.

  inputs: Pointer to a buffer that contains the file name.  If pointer
            is NULL, the second input contains the complete path and
            file name.
          CString object containing the path if the first input is not NULL.
            It contains the full path and file name if the first input
            is NULL.

  return: true if the file exists and if valid, otherwise false.
********************************************************************/
bool CBckupRest::bValidate_file(char *pszName, CString cstrPath)
  {
  bool bRet;
  DATA_FILE_HEADER dfhHdr;
  CFile *pcfDataFile;
  CString cstrTemp, cstrMsg;

    // Before copying the file, make sure it is valid.
  if(pszName != NULL)
    cstrTemp.Format("%s\\%s", cstrPath, pszName);
  else
    cstrTemp = cstrPath;
  if(m_pDoc->iOpen_file_get_hdr(&pcfDataFile, cstrTemp, &dfhHdr, false) == SUCCESS)
    {
    if(m_pDoc->bValid_file_hdr(&dfhHdr) == false)
      {
      //cstrMsg.Format(IDS_INV_FILE_HDR, cstrTemp);
      cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_INV_FILE_HDR), cstrTemp);
      m_cstraErrs.Add(cstrMsg);
      theApp.vLog(cstrMsg);
      bRet = false;
      }
    else
      bRet = true;
    pcfDataFile->Close(); // Close the file.
    delete pcfDataFile;
    }
  else
    { // File didn't open.
    m_cstraErrs.Add(m_pDoc->m_cstrErrMsg);
    bRet = false;
    }
  return(bRet);
  }

/********************************************************************
bCheck_data_files - Get the data file for each study in the database
                    and make sure there is a data file for it in the
                    current working directory.

                    Record each study that doesn't have a data file.

  inputs: None.

  return: true if all studies have a data file.
          false if at least one study doesn't have a data file.
********************************************************************/
bool CBckupRest::bCheck_data_files()
  {
  bool bRet = true;
  short int iPos;
  CString cstrFile, cstrMsg;
  FILE *pf;

  iPos = FIRST_RECORD;
  while(true)
    {
    if(m_pDoc->m_pDB->iGet_next_data_file(iPos, cstrFile) != SUCCESS)
      break;
    else if(cstrFile.IsEmpty() == FALSE)
      { // Got a data file.  Now open it to see if it exists.
      if((pf = fopen((LPCTSTR)cstrFile, "r")) == NULL)
        {
        //cstrMsg.Format(IDS_DATA_FILE_MISSING, cstrFile);
        cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_DATA_FILE_MISSING), cstrFile);
        m_cstraErrs.Add(cstrMsg);
        theApp.vLog(cstrMsg);
        bRet = false;
        }
      else
        fclose(pf);
      }
    iPos = NEXT_RECORD;
    m_cProgress.StepIt(); // Update the progress bar.
    }
  return(bRet);
  }

/********************************************************************
bCheck_bckup_data_files - Get the data file for each study in the database
                          and make sure there is a data file for it in
                          the backup directory.
                          
                          If the data file doesn't exist in the backup
                          directory, check to make sure that the data
                          file existed and was valid in the current
                          working directory.
                          
                          Record each study that doesnt have a data file
                          and the data file either didn't exist or was
                          corrupt in the working directory.


  inputs: None.

  return: true if all studies have a data file.
          false if at least one study doesn't have a data file.
********************************************************************/
bool CBckupRest::bCheck_bckup_data_files()
  {
  bool bRet = true;
  short int iPos;
  int iIndex;
  CString cstrFile, cstrSrcFile, cstrBckupFile, cstrMsg;
  FILE *pf;

  iPos = FIRST_RECORD;
  while(true)
    {
    if(m_pDoc->m_pDB->iGet_next_data_file(iPos, cstrSrcFile) != SUCCESS)
      break;
    else if(cstrSrcFile.IsEmpty() == FALSE)
      { // Got a data file.
        // Make file name with the destination path.
      if((iIndex = cstrSrcFile.ReverseFind('\\')) > 0)
        cstrFile = cstrSrcFile.Right(cstrSrcFile.GetLength() - (iIndex + 1));
      cstrBckupFile.Format("%s\\%s", m_cstrDestPath, cstrFile);
        // Open it to see if it exists.
      if((pf = fopen((LPCTSTR)cstrBckupFile, "r")) == NULL)
        {
        // Before we record an error, check to see if the source file is there
        // and valid.
        if((pf = fopen((LPCTSTR)cstrSrcFile, "r")) != NULL)
          { // Source file is there.
          if(bValidate_file(NULL, cstrSrcFile) == true)
            { // Valid source file.  Report an error.
            //cstrMsg.Format(IDS_BCKUP_DATA_FILE_MISSING, cstrFile);
            cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_BCKUP_DATA_FILE_MISSING), cstrFile);
            m_cstraErrs.Add(cstrMsg);
            theApp.vLog(cstrMsg);
            bRet = false;
            }
          fclose(pf);
          }
        }
      else
        fclose(pf);
      }
    iPos = NEXT_RECORD;
    m_cProgress.StepIt(); // Update the progress bar.
    }
  return(bRet);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CBckupRest::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_BACKUP_RESTORE);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_BR_BACKUP_RESTORE_DATA));

    GetDlgItem(IDC_BACKUP)->SetWindowText(g_pLang->cstrGet_text(ITEM_BR_BACKUP));
    GetDlgItem(IDC_RESTORE)->SetWindowText(g_pLang->cstrGet_text(ITEM_BR_RESTORE));
    GetDlgItem(IDC_BCKUP_REST_TOFROM)->SetWindowText(g_pLang->cstrGet_text(ITEM_BR_BACKUP_TO_RESTORE_FROM));
    GetDlgItem(IDC_START)->SetWindowText(g_pLang->cstrGet_text(ITEM_BR_START));
    GetDlgItem(IDC_FILE_COPIED)->SetWindowText(g_pLang->cstrGet_text(ITEM_BR_FILE_BEING_COPIED));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CLOSE));
    GetDlgItem(IDC_BCKUPREST_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    //g_pLang->vEnd_section();
    }
  }
