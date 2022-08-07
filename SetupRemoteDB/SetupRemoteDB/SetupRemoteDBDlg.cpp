/********************************************************************
SetupRemoteDBDlg.cpp

Copyright (C) 2020 3CPM Company, Inc.  All rights reserved.

This file displays the main dialog box.
  - Enter the password.
	- Select the remote database
	- Perform the transfer of the database and data files.

This file also contains the about dialog box.

HISTORY:
31-OCT-20  RET  New.  Version 1.0.0.1
22-NOV-20  RET  Version 1.0.0.2
            Change the remote database path written to eggsas.ini to include
						the database file name.
						  Change methods: OnBnClickedBtnSetupRemoteDb(), OnBnClickedBtnSelRemoteDb2()
23-NOV-20  RET  Version 1.0.0.2
             Add a check to make sure the database exists.
						   Change method: OnBnClickedBtnSetupRemoteDb()
********************************************************************/

#include "stdafx.h"
#include "rpcdce.h" // Used for creating a GUID

#include "SetupRemoteDB.h"
#include "SetupRemoteDBDlg.h"
#include "TransferDB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern int g_iPasswdLenByt1;
extern int g_iPasswdLenByt2;
extern int g_iPasswdIndexByt1[]; 
extern int g_iPasswdIndexByt2[];
extern CString g_cstrDefaultPassword;


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
public:
	virtual BOOL OnInitDialog();
  bool bGet_version_info(CString &cstrProgName, wchar_t *pszInfoType, CString &cstrInfo);
	CString m_cstrProgramName;
	CString m_cstrProgramVersion;
	CString m_cstrCopyRight;
	CString m_cstrCopyRight1;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
, m_cstrProgramName(_T(""))
, m_cstrProgramVersion(_T(""))
, m_cstrCopyRight(_T(""))
, m_cstrCopyRight1(_T(""))
{
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CWnd *pcwnd;
	CFont *pfnt;
  CFont fntNew;
  LOGFONT logFont;

	pcwnd = GetDlgItem(IDC_STATIC_PROGRAM);
	pfnt = pcwnd->GetFont();
  pfnt->GetLogFont(&logFont);
  if(fntNew.m_hObject == NULL)
  { // Only need to create the font once.
    fntNew.CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,0,0,0,0,0, logFont.lfFaceName);
    pcwnd->SetFont(&fntNew);
		GetDlgItem(IDC_STATIC_PROGRAM)->SetFont(&fntNew);
		GetDlgItem(IDC_STATIC_VERSION)->SetFont(&fntNew);
		GetDlgItem(IDC_STATIC_VERSION_TITLE)->SetFont(&fntNew);
		GetDlgItem(IDC_STATIC_COPY_RIGHT)->SetFont(&fntNew);
		GetDlgItem(IDC_STATIC_COPY_RIGHT2)->SetFont(&fntNew);
		GetDlgItem(IDOK)->SetFont(&fntNew);
  }
	// Fill in the items on the about dialog screen.
	CString cstrName = _T("SetupRemoteDB.exe");
	theApp.bGet_version_info(cstrName, _T("FileDescription"), m_cstrProgramName);
  theApp.bGet_version_info(cstrName, _T("FileVersion"), m_cstrProgramVersion);
	theApp.bGet_version_info(cstrName, _T("LegalCopyright"), m_cstrCopyRight);
  m_cstrCopyRight1.LoadString(IDS_ALL_RIGHTS_RESERVED);
  UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_PROGRAM, m_cstrProgramName);
	DDX_Text(pDX, IDC_STATIC_VERSION, m_cstrProgramVersion);
	DDX_Text(pDX, IDC_STATIC_COPY_RIGHT, m_cstrCopyRight);
	DDX_Text(pDX, IDC_STATIC_COPY_RIGHT2, m_cstrCopyRight1);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSetupRemoteDBDlg dialog


CSetupRemoteDBDlg::CSetupRemoteDBDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupRemoteDBDlg::IDD, pParent)
	, m_cstrPassword(_T(""))
	, m_cstrPassword2(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSetupRemoteDBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ENTER_PASSWORD, m_cstrPassword);
	DDV_MaxChars(pDX, m_cstrPassword, 15);
	DDX_Text(pDX, IDC_EDIT_ENTER_PASSWORD2, m_cstrPassword2);
	DDV_MaxChars(pDX, m_cstrPassword2, 15);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgress);
}

BEGIN_MESSAGE_MAP(CSetupRemoteDBDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_SETUP_REMOTE_DB, &CSetupRemoteDBDlg::OnBnClickedBtnSetupRemoteDb)
	ON_BN_CLICKED(IDC_BTN_SEL_REMOTE_DB2, &CSetupRemoteDBDlg::OnBnClickedBtnSelRemoteDb2)
END_MESSAGE_MAP()


// CSetupRemoteDBDlg message handlers

BOOL CSetupRemoteDBDlg::OnInitDialog()
{
	CString cstrVersion, cstrTitle, cstrName, cstrVersionTitle, cstrNewTitle;

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
	// Add the version number to the title.
	cstrName = _T("SetupRemoteDB.exe");
  theApp.bGet_version_info(cstrName, _T("FileVersion"), cstrVersion);
	this->GetWindowText(cstrTitle);
	cstrVersionTitle.LoadString(IDS_VERSION_TITLE);
	cstrNewTitle.Format(_T("%s  %s %s"), cstrTitle, cstrVersionTitle, cstrVersion);
	this->SetWindowText(cstrNewTitle);

	m_cstrRemoteDBPath.Empty();

	// Set a bold font.
	CWnd *pcwnd;
	CFont *pfnt;
  CFont fntNew;
  LOGFONT logFont;
	CString cstrMsg;

	pcwnd = GetDlgItem(IDC_TITLE_ENTER_PASSWORD);
	pfnt = pcwnd->GetFont();
  pfnt->GetLogFont(&logFont);
  if(fntNew.m_hObject == NULL)
  { // Only need to create the font once.
    fntNew.CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,0,0,0,0,0, logFont.lfFaceName);
    pcwnd->SetFont(&fntNew);
		GetDlgItem(IDC_TITLE_ENTER_PASSWORD2)->SetFont(&fntNew);
		GetDlgItem(IDC_TITLE_SEL_REMOTE_DB)->SetFont(&fntNew);
		GetDlgItem(IDC_BTN_SEL_REMOTE_DB2)->SetFont(&fntNew);
		GetDlgItem(IDC_REMOTE_DB)->SetFont(&fntNew);
		GetDlgItem(IDC_BTN_SETUP_REMOTE_DB)->SetFont(&fntNew);
		GetDlgItem(IDC_STATIC_PROGRESS_TITLE)->SetFont(&fntNew);
		GetDlgItem(IDOK)->SetFont(&fntNew);
		GetDlgItem(IDC_STATIC_STATUS)->SetFont(&fntNew);
		GetDlgItem(IDC_STATIC_STATUS_TITLE)->SetFont(&fntNew);
  }

	cstrMsg = _T("The password is required if the remote database path needs to be changed after this setup is complete.");
	cstrMsg += _T("\n");
	cstrMsg += _T("You may enter up to 15 characters for a password.");
	GetDlgItem(IDC_STATIC_PASSWD_HELP)->SetWindowText(cstrMsg);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSetupRemoteDBDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSetupRemoteDBDlg::OnPaint()
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
HCURSOR CSetupRemoteDBDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/********************************************************************
OnBnClickedBtnSetupRemoteDb
        Event handler for clicking on the remote setup database button.
          Performs the entire setup for using the remote database.
					  See above for setup details.
********************************************************************/
void CSetupRemoteDBDlg::OnBnClickedBtnSetupRemoteDb()
{
	CString cstrGuidPasswd, cstrGuidDefPasswd, cstrMsg, cstrFile;
	int iLen, iPatientCount, iSts;
	char c;
	FILE *pf;

	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	// Verify that the 2 passwords are the same.
	if(m_cstrPassword.IsEmpty() == true || m_cstrPassword2.IsEmpty() == true)
		MessageBox(_T("You must enter a password."));
	else if(m_cstrPassword != m_cstrPassword2)
		MessageBox(_T("The 2 passwords don't match."));
	else if(m_cstrRemoteDBPath.IsEmpty() == true)
		MessageBox(_T("You must select a remote database."));
	else
	{
	// Encode both passwords.
		cstrGuidPasswd = cstrCreate_guid();
		cstrGuidPasswd += _T("-");
		cstrGuidPasswd += cstrCreate_guid();
		iLen = m_cstrPassword.GetLength();
		c = cHex_to_ascii(iLen);
		// Put the password length into the GUID string.
		vEncode_char_in_string(c, cstrGuidPasswd, g_iPasswdLenByt1, g_iPasswdLenByt2);
		// Put the password into the GUID string.
		for(int i = 0; i < iLen; ++i)
			vEncode_char_in_string(m_cstrPassword.GetAt(i), cstrGuidPasswd, g_iPasswdIndexByt1[i], g_iPasswdIndexByt2[i]);

		// Do the default password.
		cstrGuidDefPasswd = cstrCreate_guid();
		cstrGuidDefPasswd += _T("-");
		cstrGuidDefPasswd += cstrCreate_guid();
		iLen = g_cstrDefaultPassword.GetLength();
		c = cHex_to_ascii(iLen);
		// Put the default password length into the GUID string.
		vEncode_char_in_string(c, cstrGuidDefPasswd, g_iPasswdLenByt1, g_iPasswdLenByt2);
		// Put the default password into the GUID string.
		for(int i = 0; i < iLen; ++i)
			vEncode_char_in_string(g_cstrDefaultPassword.GetAt(i), cstrGuidDefPasswd, g_iPasswdIndexByt1[i], g_iPasswdIndexByt2[i]);

		// Write to the INI file for EGGSAS: remote database path, password, default password.
		WritePrivateProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_PATH, m_cstrRemoteDBPathFile, INI_FILE);
		WritePrivateProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_PASSWORD, cstrGuidPasswd, INI_FILE);
		WritePrivateProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_DEF_PASSWORD, cstrGuidDefPasswd, INI_FILE);

		// Make sure the database exists.
		cstrFile.Format(_T("C:\\3cpm\\%s.%s"), LOCAL_DSN_DEFAULT, _T("mdb"));
		if((pf = _wfopen(cstrFile.GetBuffer(), _T("r"))) == NULL)
			{
				cstrMsg.Format(_T("The database doesn't exist: %s"), cstrFile);
				MessageBox(cstrMsg);
				GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(cstrMsg);
			}
		else
		{ // Database exists.
			fclose(pf);
			CTransferDB *pclsTransfer = new CTransferDB(this);
			pclsTransfer->m_cstrRemoteDBPath = m_cstrRemoteDBPath;
			pclsTransfer->m_cstrRemoteDB = m_cstrRemoteDBFile;

			if((iSts = pclsTransfer->iGet_patient_count(&iPatientCount)) == SUCCESS)
			{
				theApp.vLog(_T("Have %d patients to transfer"), iPatientCount);
				m_ctrlProgress.SetRange(0, (iPatientCount * 2) + 2);
				m_ctrlProgress.SetStep(1);

				iSts = pclsTransfer->iTransfer();
				if(iSts == SUCCESS && pclsTransfer->m_bErrorBypassed == false)
				{
					cstrMsg.Format(_T("The database and all data files have been successfully transferred to: %s"), m_cstrRemoteDBPath);
					MessageBox(cstrMsg);
					GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(cstrMsg);
				}
				else if(iSts == FAIL || pclsTransfer->m_bErrorBypassed == true)
				{
					const size_t cSize = strlen(LOG_FILE) + 1;
					wchar_t* wcLogFile = new wchar_t[cSize];
					mbstowcs (wcLogFile, LOG_FILE, cSize);

					cstrMsg.Format(_T("There has been an error in transferring the database and data files to: %s\nSee the log file %s"),
						m_cstrRemoteDBPath, wcLogFile);
					MessageBox(cstrMsg);
					GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(cstrMsg);
				}
			}
			else if(iSts == SUCCESS_NO_DATA)
				MessageBox(_T("There are no patients to transfer to the remote database."));
			else
				GetDlgItem(IDC_BTN_SETUP_REMOTE_DB)->EnableWindow(FALSE);
			delete pclsTransfer;
		}
	}

}

/********************************************************************
OnBnClickedBtnSelRemoteDb2
        Event handler for selecting the remote database.
********************************************************************/
void CSetupRemoteDBDlg::OnBnClickedBtnSelRemoteDb2()
{
  CFileDialog *pdlg;
  CString cstrDatabaseName;
  int iIndex;
  TCHAR szCurDir[1024];

	cstrDatabaseName.Format(_T("%s.%s"), LOCAL_DSN_DEFAULT, _T("mdb"));
  GetCurrentDirectory(1024, szCurDir);
    // Get the path to start the Open File dialog box with.  If there is no last
    // path the user selected, use the data path.
     // Create an open file dialog box. Operator can select only a single file.
  pdlg = new CFileDialog(TRUE, _T(".mdb"), cstrDatabaseName, 
                         OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, 
                         _T("Patient databases (*.mdb)|*.mdb|All Files (*.*)|*.*||"));
  pdlg->m_pOFN->lpstrInitialDir = szCurDir;
  if(pdlg->DoModal() == IDOK)
    { // User selected a database.
    m_cstrRemoteDBPathFile = pdlg->GetPathName();
    if((iIndex = m_cstrRemoteDBPathFile.ReverseFind('\\')) > 0)
      m_cstrRemoteDBPath = m_cstrRemoteDBPathFile.Left(iIndex);
		m_cstrRemoteDBFile = pdlg->GetFileName();
    }
  else
    m_cstrRemoteDBPath = "";
  delete pdlg;
    // Restore the directory we started from.  If backup/restore used a USB drive
    // this releases the USB drive so it can be removed.
  SetCurrentDirectory(szCurDir);

	GetDlgItem(IDC_REMOTE_DB)->SetWindowText(m_cstrRemoteDBPathFile);
}

/********************************************************************
vEncode_char_in_string - Encode a character into specific locations in a string.

inputs: Character to encode.
        Reference to the string to put the character into.
        Index into the string for the high nibble of the character.
        Index into the string for the low nibble of the character.

return: None.
********************************************************************/
void CSetupRemoteDBDlg::vEncode_char_in_string(wchar_t c, CString &cstrDest, int iIndex, int iIndex1)
  {
  unsigned char ucItem;
  char cItem;

  ucItem = c - '0';
  cItem = (char)(((ucItem & 0xf0) >> 4) + '0'); // High nibble
  cstrDest.SetAt(iIndex, cItem);
  cItem = (char)((ucItem & 0xf) + '0'); // Low nibble
  cstrDest.SetAt(iIndex1, cItem);
  }

/********************************************************************
cstrCreate_guid - Create a unique GUID.

inputs: None.

return: string containing the GUID.
********************************************************************/
CString CSetupRemoteDBDlg::cstrCreate_guid(void)
  {
  GUID guid;    // generated GUID
  //unsigned char *pszGUID;
	RPC_WSTR strGuid;
  CString cstrGUID;

  //cstrValue.Empty();
  guid = GUID_NULL;
  HRESULT hr = ::UuidCreate(&guid);
  UuidToString(&guid, &strGuid);
  cstrGUID = (LPCTSTR)strGuid;
  return(cstrGUID);
  }

/********************************************************************
cHex_to_ascii - Convert hex number, 0 to F to the Ascii representation.

inputs: Hex number, 0 to F.

return: Ascii representation.
********************************************************************/
char CSetupRemoteDBDlg::cHex_to_ascii(unsigned char ucHex)
  {
  char c;

  if(ucHex <= 9)
    c = ucHex + '0';
  else
    c = 'a' + ucHex - 0xA;
  return(c);
  }

/********************************************************************
vUpdateProgress - Update the progress bar, step it by one.

inputs: None.

return: None.
********************************************************************/

void CSetupRemoteDBDlg::vUpdateProgress()
{
	m_ctrlProgress.StepIt();
}
