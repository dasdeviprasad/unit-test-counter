/********************************************************************
MakeSecureDataFileDlg.cpp

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the main dialog box.

  This class provides the event handlers and methods.

HISTORY:
10-AUG-10  RET  New.
********************************************************************/

#include "stdafx.h"
#include "..\common\SecureInstall.h"
#include "MakeSecureDataFile.h"
#include "MakeSecureDataFileDlg.h"
#include ".\makesecuredatafiledlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
// CMakeSecureDataFileDlg dialog

/********************************************************************
Constructor

********************************************************************/
CMakeSecureDataFileDlg::CMakeSecureDataFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMakeSecureDataFileDlg::IDD, pParent)
  , m_cstrPassword(_T(""))
  {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  }

/********************************************************************
Destructor

  Clean up any data or classes.
********************************************************************/
CMakeSecureDataFileDlg::~CMakeSecureDataFileDlg(void)
  {

  delete m_psi;
  }

void CMakeSecureDataFileDlg::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Control(pDX, IDC_LIST_RESULTS, m_cResultsList);
DDX_Text(pDX, IDC_EDIT_PASSWORD, m_cstrPassword);
}

BEGIN_MESSAGE_MAP(CMakeSecureDataFileDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BTN_CREATE_BACKUP, OnBnClickedBtnCreateBackup)
  ON_BN_CLICKED(IDC_BTN_TEST, OnBnClickedBtnTest)
  ON_BN_CLICKED(IDC_BTN_RESTORE_BACKUP, OnBnClickedBtnRestoreBackup)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
  ON_BN_CLICKED(IDC_BTN_CREATE_DATA_FILE, OnBnClickedBtnCreateDataFile)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////
// CMakeSecureDataFileDlg message handlers

/********************************************************************
OnInitDialog

  Initialize the main dialog box.
********************************************************************/
BOOL CMakeSecureDataFileDlg::OnInitDialog()
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
  m_bRestoreFile = false;
  m_psi = new CSecureInstall;
  // Read in the password from the INI file.
  m_cstrPassword = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_PASSWORD, "3cpm");
  UpdateData(FALSE);
//  PostMessage(WMUSER_CREATE_FILE, 0, 0);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMakeSecureDataFileDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMakeSecureDataFileDlg::OnPaint() 
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
HCURSOR CMakeSecureDataFileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/********************************************************************
Handler for the "Create backup" button.

  Creates a backup of the secure data file.
********************************************************************/
void CMakeSecureDataFileDlg::OnBnClickedBtnCreateBackup()
  {
  BOOL bSts;

  bSts = CopyFile(SECURE_INSTALL_FILE, SECURE_INSTALL_FILE_BACKUP, FALSE);
  if(bSts == FALSE)
    MessageBox("Failed to create backup file.", "ERROR");
  }

/********************************************************************
Handler for the "Test" button.

  Validates the password.
********************************************************************/
void CMakeSecureDataFileDlg::OnBnClickedBtnTest()
  {

  // Read in the data file.
  if(m_psi->bRead_file() == true)
    {
    if(m_psi->bValidate_password() == true)
      vAdd_to_result_list("Password is valid.");
    else
      vAdd_to_result_list("Password is NOT valid.");
    }
  m_bRestoreFile = true;
  if(m_psi->m_cstrErr != "")
    vAdd_to_result_list(m_psi->m_cstrErr);

  UpdateData(FALSE);
  }

/********************************************************************
Handler for the "Restore backup" button.

  Copies the backup of the secure data file to the secure data file.
********************************************************************/
void CMakeSecureDataFileDlg::OnBnClickedBtnRestoreBackup()
  {

  vRestore_file(true);
  }

/********************************************************************
Handler for the "Close" button.

  Checks to make sure if the data file was tested it is restored.
  Closes the dialog box.
********************************************************************/
void CMakeSecureDataFileDlg::OnBnClickedCancel()
  {

  if(m_bRestoreFile == true)
    vRestore_file(false);
  OnCancel();
  }

void CMakeSecureDataFileDlg::OnBnClickedBtnCreateDataFile()
  {

  UpdateData(TRUE);
  AfxGetApp()->WriteProfileString(INI_SEC_GENERAL, INI_ITEM_PASSWORD, m_cstrPassword);
  vCreate_secure_file();
  }

/********************************************************************
Handler for the WMUSER_CREATE_FILE messge.

  Creates the data file.
********************************************************************/
void CMakeSecureDataFileDlg::vCreate_secure_file(void)
  {
  CString cstrMsg;

  if(m_psi->bCreate_secure_file() == false)
    vAdd_to_result_list(m_psi->m_cstrErr);
  else
    {
    cstrMsg.Format("Secure data file successfully created, %s", SECURE_INSTALL_FILE);
    vAdd_to_result_list(cstrMsg);
    }
  UpdateData(FALSE);
  }

/////////////////////////////////////////////////////////////////////
// Methods

void CMakeSecureDataFileDlg::vAdd_to_result_list(CString cstrMsg)
  {
  int iIndex;
  CDC *pcdcLb;
  CSize csExt;

    // Make sure the horizontal scroll bar is displayed if it needs to be.
  pcdcLb = GetDC();
  csExt = pcdcLb->GetTextExtent(cstrMsg);
  if(csExt.cx > m_cResultsList.GetHorizontalExtent())
    { // Have a new longest line.
      // Tell the listbox about it so it can display the horizontal
      // scroll bar if it needs to and update how much to scroll.
    csExt.cx *= 2;
    m_cResultsList.SetHorizontalExtent(csExt.cx);
    }
  ReleaseDC(pcdcLb);
  iIndex = m_cResultsList.AddString(cstrMsg);
  m_cResultsList.SetCurSel(iIndex);
  }


void CMakeSecureDataFileDlg::vRestore_file(bool bAddToList)
  {
  BOOL bSts;
  CString cstrMsg;

  bSts = CopyFile(SECURE_INSTALL_FILE_BACKUP, SECURE_INSTALL_FILE, FALSE);
  if(bSts == FALSE)
    cstrMsg = "Failed to restore backup file.";
  else
    {
    m_bRestoreFile = false;
    cstrMsg = "File has been restored.";
    }
  if(bAddToList == true)
    vAdd_to_result_list(cstrMsg);
  else
    {
    if(bSts == FALSE)
      MessageBox(cstrMsg, "ERROR");
    else
      MessageBox(cstrMsg);
    }
  }

