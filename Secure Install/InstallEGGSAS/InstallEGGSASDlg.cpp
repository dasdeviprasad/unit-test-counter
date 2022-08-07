/********************************************************************
InstallEGGSASDlg.cpp

Copyright (C) 2010,2012, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the main dialog box.

HISTORY:
17-SEP-10  RET  New.
21-SEP-10  RET
                Change command line arguments:
                  Arg 1: The path where the secure data file exists.
                  Arg 2: Text string specific to the installation program.
                            AP2-JK4;
                Remove method: bFind_file()
                Change OnInitDialog() to parse the command line for the 
                  2 arguments.
18-SEP-12  RET
             Change to allow 3 tries to enter the correct password.
               Change methods: OnBnClickedOk()
********************************************************************/

#include "stdafx.h"
#include "InstallEGGSAS.h"
#include "InstallEGGSASDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CInstallEGGSASDlg dialog



CInstallEGGSASDlg::CInstallEGGSASDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInstallEGGSASDlg::IDD, pParent)
  , m_cstrPassword(_T(""))
  {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CInstallEGGSASDlg::~CInstallEGGSASDlg(void)
  {

  delete m_psi;
  }


void CInstallEGGSASDlg::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_EDIT_PASSWORD, m_cstrPassword);
}

BEGIN_MESSAGE_MAP(CInstallEGGSASDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
//  ON_WM_DESTROY()
END_MESSAGE_MAP()


// CInstallEGGSASDlg message handlers

BOOL CInstallEGGSASDlg::OnInitDialog()
{
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
  CString cstrErr;

  m_psi = new CSecureInstall;

  //{
	  //char szDir[1024];
	  //GetCurrentDirectory(1024, szDir);
   // MessageBox(szDir);

	//MessageBox(theApp.m_lpCmdLine);
 // }
	
  cstrErr.Empty();
  if(theApp.m_lpCmdLine[0] != NULL)
  { // Parse the command line into the starting path and the text string
	  // that indicates it is started from the install program.
    char *pszArg2, *pszTemp;

	  m_strPath = "";
    if((pszTemp = strchr(theApp.m_lpCmdLine, ',')) != NULL)
    { // Found the comma separator.
      pszArg2 = pszTemp + 1;
      *pszTemp = NULL;
#ifndef _DEBUG
      m_strPath = theApp.m_lpCmdLine;
#endif
      if(strcmp(pszArg2, "AP2-JK4;") != 0)
        cstrErr = "Please start the installation program, not this program.";
      }
    }
  else
    cstrErr = "Please start the installation program, not this program.";
  if(cstrErr.IsEmpty() == false)
  {
    MessageBox(cstrErr);
    ::PostQuitMessage(0);
  }

  m_iNumTries = 0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInstallEGGSASDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CInstallEGGSASDlg::OnPaint() 
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
HCURSOR CInstallEGGSASDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CInstallEGGSASDlg::OnBnClickedOk()
  {
  bool bOKToExit = false;
  int iExitCode;

  // Look for removable drives with the validate file on them.
  //if(bFind_file() == false)
  //  {
  //  MessageBox("A necessary file is missing.\nThe EGGSAS System can't be installed.");
  //  bOKToExit = true;
  //  vCreate_ok_file(false);
  //  }
  //else
    {
    // First check to make sure the user has entered a password.
    UpdateData(TRUE);
    if(m_cstrPassword.IsEmpty() == true)
      {
      MessageBox("Please enter your password.");
      }
    else
      {
      // Validate the password.
      // Read in the data file.
      m_psi->vSet_file_path(m_strPath);
      if(m_psi->bRead_file() == true)
        {
        if(m_iNumTries < MAX_TRIES)
          {
          if(m_psi->bValidate_password(m_cstrPassword) == true)
            { // Password is valid, OK to install.
            iExitCode = EXIT_CODE_OK;
            // Create a file to indicate that the pasword is OK.
            vCreate_ok_file(true);
            bOKToExit = true;
            }
          else
            {
            ++m_iNumTries;
            if(m_iNumTries == (MAX_TRIES - 1))
              { // One more try left.
              MessageBox("Your password is not correct.\nPlease try one more time.");
              m_cstrPassword = "";
              }
            else if(m_iNumTries == MAX_TRIES)
              { // Tried enough.  Don't continue with the install.
              iExitCode = EXIT_CODE_PASSWORD_INVALID;
              MessageBox("Your password is not correct.\nThe EGGSAS System can't be installed.");
              vCreate_ok_file(false);
              bOKToExit = true;
             }
            else
              {
              MessageBox("Your password is not correct.\nPlease try again.");
              m_cstrPassword = "";
              //GetDlgItem(IDOK)->SetFocus();
              }
            }
          }
        else
          { // Invalid password, don't install.
          iExitCode = EXIT_CODE_PASSWORD_INVALID;
          MessageBox("Your password is not correct.\nThe EGGSAS System can't be installed.");
          vCreate_ok_file(false);
          bOKToExit = true;
          }
        }
      else
        { // File error.
        CString cstrErr;

        iExitCode = EXIT_CODE_FILE_ERROR;
        cstrErr.Format("There was an error: %s.\nThe EGGSAS System can't be installed.",
          m_psi->m_cstrErr);
        MessageBox(cstrErr, "ERROR");
        vCreate_ok_file(false);
        bOKToExit = true;
        }
      }
    }
  if(bOKToExit == true)
    ::PostQuitMessage(iExitCode);
  else
    UpdateData(FALSE);
  //OnOK();
  }

/********************************************************************
The exit button from the title bar comes here also.
********************************************************************/
void CInstallEGGSASDlg::OnBnClickedCancel()
  {

  ::PostQuitMessage(EXIT_CODE_USER_CANCELLED);
  //OnCancel();
  }

void CInstallEGGSASDlg::vCreate_ok_file(bool bOk)
  {
  FILE *pf;
  char szOkFile[100];

  if(bOk == true)
    {
    sprintf(szOkFile, "%s\\pswdok.txt", m_strPath);
    if((pf = fopen(szOkFile, "w")) != NULL)
    //if((pf = fopen("pswdok.txt", "w")) != NULL)
      {
      if(bOk == true)
        fputs("OK", pf);
      else
        fputs("FAIL", pf);
      fclose(pf);
      }
    else
      MessageBox("File creation error.\nThe EGGSAS System can't be installed.");
    }
  }

//bool CInstallEGGSASDlg::bFind_file()
//  {
//  char DriveLetter = 'D';
//  char szFile[100];
//  FILE *pf;
//  bool bRet = false;
//
//  DriveLetter = 'H';
//  while(DriveLetter <= 'Z')
//    {
//    szFile[0] = DriveLetter;
//    szFile[1] = NULL;
//	//strcat(szFile, " on RThawley");
//    strcat(szFile, ":"); //\\");
//    strcat(szFile, SECURE_INSTALL_FILE);
//    if((pf = fopen(szFile, "r")) != NULL)
//      {
//      bRet = true;
//      fclose(pf);
//      m_cstrDataFilePath.Format("%c:", DriveLetter);
//      m_psi->vSet_file_path(m_cstrDataFilePath);
//      break;
//      }
//    ++DriveLetter;
//    }
//  return(bRet);
//  }
  