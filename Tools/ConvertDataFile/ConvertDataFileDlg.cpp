// ConvertDataFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ConvertDataFile.h"
#include "DataFile.h"
#include "ConvertDataFileDlg.h"
#include ".\convertdatafiledlg.h"

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


// CConvertDataFileDlg dialog



CConvertDataFileDlg::CConvertDataFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConvertDataFileDlg::IDD, pParent)
  , m_cstrDataFile(_T(""))
  , m_cstrConversionStatus(_T(""))
  , m_iNumBaselineMinutes(0)
  {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CConvertDataFileDlg::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_EDIT1, m_cstrDataFile);
DDX_Text(pDX, IDC_EDIT_STATUS, m_cstrConversionStatus);
DDX_Text(pDX, IDC_EDIT_NUM_BASELINE_MINUTES, m_iNumBaselineMinutes);
}

BEGIN_MESSAGE_MAP(CConvertDataFileDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BTN_SEL_DATA_FILE, OnBnClickedBtnSelDataFile)
  ON_BN_CLICKED(IDC_BTN_CONV_EGGSAS4_TO_RESRCH, OnBnClickedBtnConvEggsas4ToResrch)
END_MESSAGE_MAP()


// CConvertDataFileDlg message handlers

BOOL CConvertDataFileDlg::OnInitDialog()
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
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CConvertDataFileDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CConvertDataFileDlg::OnPaint() 
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
HCURSOR CConvertDataFileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CConvertDataFileDlg::OnBnClickedBtnSelDataFile()
  {
  CFileDialog *pdlg;
  int iIndex;

  pdlg = new CFileDialog(TRUE, _T(".egg"));
  pdlg->m_pOFN->lpstrInitialDir = _T("c:\\3cpm");
  if(pdlg->DoModal() == IDOK)
  {
    m_cstrDataFile = pdlg->GetPathName(); // Path and file name
    m_cstrFileName = pdlg->GetFileName(); // File name only
      // Remove the file name from the path.
    m_cstrFilePath = pdlg->GetPathName(); // Path and file name
    if((iIndex = m_cstrFilePath.ReverseFind('\\')) >= 0)
      m_cstrFilePath.Delete(iIndex, m_cstrFilePath.GetLength() - iIndex);
    UpdateData(FALSE);
  }
  delete pdlg;
  }

void CConvertDataFileDlg::OnBnClickedBtnConvEggsas4ToResrch()
  {
  CString cstrMsg;
  bool bContinue;
  BOOL bCopySts;
  int iNumMinutes, iSts;
  CDataFile *pdf = NULL;

  UpdateData(TRUE);
  // Find the number of minutes in the data file.
  pdf = new CDataFile();
  pdf->m_cstrDataPathFile = m_cstrDataFile;
  if((iSts = pdf->iOpen_file_get_hdr()) == SUCCESS)
    { // Validate the header.
    if(pdf->bIsValid_hdr() == true)
      { // Number of points in data file.
      iNumMinutes = pdf->iNum_data_pnts() / (DATA_POINTS_PER_MIN_1 * 4);
      }
    }

  if(m_iNumBaselineMinutes >= 4 && m_iNumBaselineMinutes < (iNumMinutes - 4))
    {
    // First create a backup copy in c:\\3cpm\\eggsas-4
    // If the backup directory doesn't exist, create it.
    bContinue = true;
    if(CreateDirectory("c:\\3cpm\\eggsas-4", NULL) == 0)
      {
      if(GetLastError() != ERROR_ALREADY_EXISTS)
        {
        cstrMsg = "Can't create backup directory.  Do you want to continue without a backup?";
        if(MessageBox(cstrMsg, "ERROR", MB_YESNO | MB_ICONSTOP) == IDYES)
          bContinue = true;
        else
          bContinue = false;
        }
      }
    if(bContinue == true)
      {
      bCopySts = bCopy_one_file("c:\\3cpm\\eggsas-4", m_cstrFilePath, m_cstrFileName, TRUE);
      if(bCopySts == TRUE)
        bContinue = true;
      else
        {
        cstrMsg = "Do you want to continue without a current backup?";
        if(MessageBox(cstrMsg, "ERROR", MB_YESNO | MB_ICONSTOP) == IDYES)
          bContinue = true;
        else
          bContinue = false;
        }
      if(bContinue == true)
        {
        // Convert the file.
        bConvert_eggsas4_to_research(pdf);
        m_cstrConversionStatus = "Conversion complete.";
        }
      else
        m_cstrConversionStatus = "Conversion not done.";
      }
    else
      {
      m_cstrConversionStatus = "Conversion not done.";
      }
    }
  else
    {
    if(m_iNumBaselineMinutes < 4)
      cstrMsg = "The baseline must be at least 4 minutes.";
    else
      cstrMsg = "The baseline is more than the total minutes in the study.";
    MessageBox(cstrMsg, "ERROR", MB_OK | MB_ICONSTOP);
    }
  if(pdf != NULL)
    delete pdf;

  UpdateData(FALSE);
  }



/********************************************************************
iCopy_one_file - Copy one file from the source to the destination.
                 Optionally ask to overwrite an existing file based on
                 the last input.
                 Display an error message on errors.

    inputs: Pointer to the destination path.
              It must NOT have a trailing backslash.
            Pointer to the source path.
              It must NOT have a trailing backslash.
            Pointer to the file name. If the pointer is NULL, both the destination
              and source path contain the file name.
            TRUE - fail if the file already exists.
              FALSE - automatically overwrite any exisiting files.

    return: SUCCESS if the file was copied.
            FAIL if there was an error or the file was not copied.
********************************************************************/
BOOL CConvertDataFileDlg::bCopy_one_file(LPCTSTR pstrDestPath, LPCTSTR pstrSrcPath,
LPCTSTR pstrName, BOOL bNoAutoOverwrite)
  {
  CString cstrSrc, cstrDest, cstrMsg;
  BOOL bFailIfExists, bSts;
  char szMsg[256];
  DWORD dwErr;

  if(pstrName != NULL)
    { // There is a file name, append it to the path.
    cstrSrc.Format("%s\\%s", pstrSrcPath, pstrName);
    cstrDest.Format("%s\\%s", pstrDestPath, pstrName);
    }
  else
    {
    cstrSrc = pstrSrcPath;
    cstrDest = pstrDestPath;
    }
  bFailIfExists = bNoAutoOverwrite; // File copy fails if file already exists.
  while((bSts = CopyFile((LPCTSTR)cstrSrc, (LPCTSTR)cstrDest, bFailIfExists)) == 0)
    { // The copy failed.
    dwErr = GetLastError();
    //cstrErrTitle.LoadString(IDS_BACKUP_ERROR);
    if(dwErr == ERROR_FILE_EXISTS)
      { // The file already exists on the backup, ask if user wants to
        // replace it or cancel.
      cstrMsg.Format("The backup file, %s, already exists.  Do you want to overwrite it?", cstrDest);
      if(MessageBox(cstrMsg, "BACKUP ERROR", MB_YESNO | MB_ICONSTOP) == IDNO)
        break; // User canceling backup.
      bFailIfExists = FALSE; // Allow the file copy to overwrite existing file.
      }
    else
      { // Got an error writing to the disk.  Ask the user if he wants to 
        // try again or cancel the backup.
      //cstrMsg.LoadString(IDS_DISK_TRY_AGAIN);
      cstrMsg = "If you want to try again press OK. Or press Cancel to cancel the conversion.";
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, 0L, szMsg, 256, NULL);
      cstrMsg.Insert(0, szMsg);
      if(MessageBox(cstrMsg, "BACKUP ERROR", MB_OKCANCEL | MB_ICONSTOP) == IDCANCEL)
        break; // User canceling backup.
       }
    }
  return(bSts);
  }


bool CConvertDataFileDlg::bConvert_eggsas4_to_research(CDataFile *pdf)
  {
  bool bRet = true;
  int iSts = SUCCESS;

  if((iSts = pdf->iRead_EGGSAS4()) == SUCCESS)
    {
    if(pdf->iWrite_research_data_file(m_iNumBaselineMinutes) != SUCCESS)
      {
      bRet = false;
      MessageBox(pdf->m_cstrErrMsg, "CONVERSION ERROR", MB_OK | MB_ICONSTOP);
      }
    pdf->vClose_file();
    }
  else
    {
    bRet = false;
    MessageBox(pdf->m_cstrErrMsg, "CONVERSION ERROR", MB_OK | MB_ICONSTOP);
    }
  return(bRet);
  }
