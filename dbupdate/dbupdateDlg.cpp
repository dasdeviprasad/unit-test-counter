/********************************************************************
dbupdateDlg.cpp

Copyright (C) 2005,2007,2008,2012,2013,2019, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CdbupdateDlg class.

  Displays the main dialog box window.

  Processes the changing of the database.
    - Determine if the database needs updating.
    - If the database needs updating
      . Make a backup copy of the database
      . Update the database
      . If updating the database fails, restore the database with the
        backup copy.

HISTORY:
28-FEB-05  RET  New.
09-JUL-07  RET  Add update to version 2.0 of the database for the
                  Research Version of EGGSAS.
06-AUG-07  RET  Program version 1.2, Database version 2.01
                  Research Version of EGGSAS.
29-AUG-08  RET  Version 2.03 of the database.
                  Change iUpdate_database().
02-SEP-12  RET  Version 2.04 of the database.
                  Change iUpdate_database().
27-FEB-13  RET  Version 2.05 of the database.
                  Change iUpdate_database().
16-MAR-13  RET  Version 2.06 of the database.
                  Change iUpdate_database().
22-JAN-19  RET  DB Ver 2.06, product ver 1.6
                  Add ability to specify a different database on the  command line.
                    Change methods: iUpdate_database()
********************************************************************/

#include "stdafx.h"
#include "dbupdate.h"
#include "sysdefs.h"
#include "Dbaccess.h"
#include "dbupdateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CString g_cstrDBFile;

/////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////
// CdbupdateDlg dialog

CdbupdateDlg::CdbupdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CdbupdateDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CdbupdateDlg::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Control(pDX, IDC_UPDATE_STATUS, m_clbUpdateStatus);
}

BEGIN_MESSAGE_MAP(CdbupdateDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////
// CdbupdateDlg message handlers

BOOL CdbupdateDlg::OnInitDialog()
  {
  char szIniFile[256];

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    // Get the EGGSAS INI file.
  GetCurrentDirectory(sizeof(szIniFile), szIniFile);
  strcat(szIniFile, "\\");
  strcat(szIniFile, "eggsas.ini");
  free((void *)AfxGetApp()->m_pszProfileName);
  AfxGetApp()->m_pszProfileName = _strdup(szIniFile);

  //MessageBox("DBUpdate ver 1.6");
    // Open the database.
  m_pDB = new CDbaccess();
  iUpdate_database();
	
  delete m_pDB;

#ifndef _DEBUG
  PostQuitMessage(0);
#endif

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CdbupdateDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CdbupdateDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CdbupdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
iUpdate_database - Update the database.
                   This is the first version, so we only have to update
                     from the previous version and not worry about earlier
                     versions.

  inputs: None.

  output: Updates database tables.

  return: SUCCESS if the database is updated.
          FAIL if there was an error.
********************************************************************/
int CdbupdateDlg::iUpdate_database(void)
  {
  CString cstrVersion, cstrDBFileName, cstrDBBackupFileName, cstrErr;
  short int iRet, iMajorVersion, iMinorVersion;
  bool bContinue = true;

  theApp.vLog("DB from the command line: %s", g_cstrDBFile);
  if((iRet = m_pDB->iGet_version(cstrVersion)) == SUCCESS)
    {
    iMajorVersion = m_pDB->iGet_major_version(cstrVersion);
    iMinorVersion = m_pDB->iGet_minor_version(cstrVersion);
    if(iMajorVersion < DB_VERSION_MAJOR || iMinorVersion < DB_VERSION_MINOR)
      { // make a backup copy of the database.
      theApp.vLog("*** Updating database ***");
      if(m_pDB->m_bStdDB == true)
        { // Updating the standard database.
        cstrDBFileName = m_pDB->cstrGet_db_path();
        cstrDBFileName += "\\";
        cstrDBFileName += m_pDB->cstrGet_db_name();
        theApp.vLog("Updating standard DB: %s", cstrDBFileName);
        }
      else
        { // Updating the database specified on the command line.
        cstrDBFileName = g_cstrDBFile;
        theApp.vLog("Updating DB from the command line: %s", cstrDBFileName);
        }
      cstrDBBackupFileName = cstrDBFileName;
      cstrDBBackupFileName += ".bak";
      if(CopyFile(cstrDBFileName, cstrDBBackupFileName, FALSE) == FALSE)
        {
        if(m_pDB->m_bStdDB == true)
          { // Updating the standard database.
          cstrErr.Format("Error backing up database: %s", theApp.cstrGet_sys_err_msg());
          vUpdate_status_list(cstrErr, true);
          if(MessageBox("You are about to update your database and a backup copy can't be made\nDo you want to continue without a backup copy?",
            "ERROR", MB_YESNO) == IDNO)
            bContinue = false;
          }
        }
      if(bContinue == true)
        { // Update the patient table.
        iRet = SUCCESS;
        if(iMajorVersion == 1 && iMinorVersion == 0)
          {
          iRet = FAIL;
          if((iRet = m_pDB->iUpdate_patient_100_101()) >= SUCCESS)
            {
            vUpdate_status_list("Patient table updated", true);
            if(iRet == SUCCESS_WITH_INFO)
              vUpdate_status_list(m_pDB->m_cstrInfo, false);
              // Update the study table
            if((iRet = m_pDB->iUpdate_study_100_101()) >= SUCCESS)
              {
              vUpdate_status_list("StudyData table updated", true);
              if(iRet == SUCCESS_WITH_INFO)
                vUpdate_status_list(m_pDB->m_cstrInfo, false);
              iRet = SUCCESS;
              iMajorVersion = 1;
              iMinorVersion = 1;
                // Update the version table.
              if((iRet = m_pDB->iUpdate_version()) >= SUCCESS)
                {
                vUpdate_status_list("Version updated", true);
                if(iRet == SUCCESS_WITH_INFO)
                  vUpdate_status_list(m_pDB->m_cstrInfo, false);
                iRet = SUCCESS;
                }
              else
                vUpdate_status_list(m_pDB->m_cstrErr, false); // Version table fail
              }
            else
              vUpdate_status_list(m_pDB->m_cstrErr, false); // StudyData table fail
            }
          else
            vUpdate_status_list(m_pDB->m_cstrErr, false); // Patient table fail
          }
        if(iRet == SUCCESS && iMajorVersion == 1 && iMinorVersion == 1)
          {
          if((iRet = m_pDB->iUpdate_study_101_200()) >= SUCCESS)
            {
            vUpdate_status_list("StudyData table updated", true);
            if(iRet == SUCCESS_WITH_INFO)
              vUpdate_status_list(m_pDB->m_cstrInfo, false);
              // Update the version table.
            if((iRet = m_pDB->iUpdate_version()) >= SUCCESS)
              {
              vUpdate_status_list("Version updated", true);
              if(iRet == SUCCESS_WITH_INFO)
                vUpdate_status_list(m_pDB->m_cstrInfo, false);
              iRet = SUCCESS;
              iMajorVersion = 2;
              iMinorVersion = 0;
              }
            }
          }
        if(iRet == SUCCESS && iMajorVersion == 2 && iMinorVersion == 0)
          {
          if((iRet = m_pDB->iAdd_event_table_201()) >= SUCCESS)
            {
            vUpdate_status_list("Events table added", true);
            if(iRet == SUCCESS_WITH_INFO)
              vUpdate_status_list(m_pDB->m_cstrInfo, false);
              // Update the version table.
            if((iRet = m_pDB->iUpdate_version()) >= SUCCESS)
              {
              vUpdate_status_list("Version updated", true);
              if(iRet == SUCCESS_WITH_INFO)
                vUpdate_status_list(m_pDB->m_cstrInfo, false);
              iRet = SUCCESS;
              iMajorVersion = 2;
              iMinorVersion = 1;
              }
            }
          }
        if(iRet == SUCCESS && iMajorVersion == 2 && iMinorVersion == 1)
          {
          if((iRet = m_pDB->iAdd_whatif_table_202()) >= SUCCESS)
            {
            vUpdate_status_list("WhatIf table added", true);
            if(iRet == SUCCESS_WITH_INFO)
              vUpdate_status_list(m_pDB->m_cstrInfo, false);
              // Update the study table.
            if((iRet = m_pDB->iUpdate_study_201_202()) >= SUCCESS)
              {
              vUpdate_status_list("Study table updated", true);
              if(iRet == SUCCESS_WITH_INFO)
                vUpdate_status_list(m_pDB->m_cstrInfo, false);
                // Update the version table.
              if((iRet = m_pDB->iUpdate_version()) >= SUCCESS)
                {
                vUpdate_status_list("Version updated", true);
                if(iRet == SUCCESS_WITH_INFO)
                  vUpdate_status_list(m_pDB->m_cstrInfo, false);
                iRet = SUCCESS;
                iMajorVersion = 2;
                iMinorVersion = 2;
                }
              }
            }
          }
        if(iRet == SUCCESS && iMajorVersion == 2 && iMinorVersion == 2)
          { // Move research reference numbers from patient info table
            // to patient table.
          if((iRet = m_pDB->iUpdate_patient_202_203()) >= SUCCESS)
            {
            vUpdate_status_list("PATIENT and PATIENTINFO tables updated.", true);
            if(iRet == SUCCESS_WITH_INFO)
              vUpdate_status_list(m_pDB->m_cstrInfo, false);
              // Update the version table.
            if((iRet = m_pDB->iUpdate_version()) >= SUCCESS)
              {
              vUpdate_status_list("Version updated", true);
              if(iRet == SUCCESS_WITH_INFO)
                vUpdate_status_list(m_pDB->m_cstrInfo, false);
              iRet = SUCCESS;
              iMajorVersion = 2;
              iMinorVersion = 3;
              }
            }
          }
        if(iRet == SUCCESS && iMajorVersion == 2 && iMinorVersion == 3)
          {
          if((iRet = m_pDB->iUpdate_patient_203_204()) >= SUCCESS)
            {
            vUpdate_status_list("DiagnosisDesc table created.", true);
            vUpdate_status_list("ICDCode table created.", true);
            vUpdate_status_list("Recommendations table created.", true);
            vUpdate_status_list("StudyData table updated.", true);
            vUpdate_status_list("Diagnosis key phrases and descriptions were added.", true);
            vUpdate_status_list("ICD codes were added.", true);
            if(iRet == SUCCESS_WITH_INFO)
              vUpdate_status_list(m_pDB->m_cstrInfo, false);
              // Update the version table.
            if((iRet = m_pDB->iUpdate_version()) >= SUCCESS)
              {
              vUpdate_status_list("Version updated", true);
              if(iRet == SUCCESS_WITH_INFO)
                vUpdate_status_list(m_pDB->m_cstrInfo, false);
              iRet = SUCCESS;
              iMajorVersion = 2;
              iMinorVersion = 4;
              }
            }
          }
        if(iRet == SUCCESS && iMajorVersion == 2 && iMinorVersion == 4)
          {
          if((iRet = m_pDB->iUpdate_patient_204_205()) >= SUCCESS)
            {
            vUpdate_status_list("PATIENT and PATIENTINFO tables updated.", true);
            if(iRet == SUCCESS_WITH_INFO)
              vUpdate_status_list(m_pDB->m_cstrInfo, false);
              // Update the version table.
            if((iRet = m_pDB->iUpdate_version()) >= SUCCESS)
              {
              vUpdate_status_list("Version updated", true);
              if(iRet == SUCCESS_WITH_INFO)
                vUpdate_status_list(m_pDB->m_cstrInfo, false);
              iRet = SUCCESS;
              iMajorVersion = 2;
              iMinorVersion = 5;
              }
            }
          }

        if(iRet == SUCCESS && iMajorVersion == 2 && iMinorVersion == 5)
          {
          if((iRet = m_pDB->iUpdate_patient_205_206()) >= SUCCESS)
            {
            vUpdate_status_list("StudyData and WhatIf tables updated.", true);
            if(iRet == SUCCESS_WITH_INFO)
              vUpdate_status_list(m_pDB->m_cstrInfo, false);
              // Update the version table.
            if((iRet = m_pDB->iUpdate_version()) >= SUCCESS)
              {
              vUpdate_status_list("Version updated", true);
              if(iRet == SUCCESS_WITH_INFO)
                vUpdate_status_list(m_pDB->m_cstrInfo, false);
              iRet = SUCCESS;
              iMajorVersion = 2;
              iMinorVersion = 6;
              }
            }
          }

        if(iRet == FAIL)
          { // Failed in updating the database, restore the backup version.
          vUpdate_status_list("Database update failed, original database restored", true);
          if(CopyFile(cstrDBBackupFileName, cstrDBFileName, FALSE) == FALSE)
            {
            cstrErr.Format("Error restoring database: %s", theApp.cstrGet_sys_err_msg());
            vUpdate_status_list(cstrErr, true);
            }
          else
            DeleteFile(cstrDBBackupFileName); // Delete the backup up copy.
          }
        }
      theApp.vLog("*** Finished updating database ***");
      }
    else
      {
      vUpdate_status_list("Database is up to date.", true);
      iRet = SUCCESS;
      }
    }

  m_pDB->sqlrDB_disconnect();
  return 0;
  }

/********************************************************************
vUpdate_status_list - Update the status message area of the display.
                      When the message is added to the listbox, the
                      horizontal scrollbar is made to display so that
                      the entire message can be seen.

    inputs: Pointer to the status message.
            true - write the message to the log file, otherwise false

    return: None.
********************************************************************/
void CdbupdateDlg::vUpdate_status_list(LPCTSTR pstrMsg, bool bLog)
  {
  CDC *pcdcLb;
  CSize csExt;

    // Add the message to the list box.  If the listbox is full, delete
    // items from it starting at the top until the new item fits in.
  while(m_clbUpdateStatus.AddString(pstrMsg) < 0)
    m_clbUpdateStatus.DeleteString(0);
    // Make sure the horizontal scroll bar is displayed if it needs to be.
  pcdcLb = GetDC();
  csExt = pcdcLb->GetTextExtent(pstrMsg);
  if(csExt.cx > m_clbUpdateStatus.GetHorizontalExtent())
    { // Have a new longest line.
      // Tell the listbox about it so it can display the horizontal
      // scroll bar if it needs to and update how much to scroll.
    m_clbUpdateStatus.SetHorizontalExtent(csExt.cx);
    }
  ReleaseDC(pcdcLb);
    // Make sure the last line is displayed.
  m_clbUpdateStatus.SetCurSel(m_clbUpdateStatus.GetCount() - 1);
  if(bLog == true)
    theApp.vLog(pstrMsg);
  }
