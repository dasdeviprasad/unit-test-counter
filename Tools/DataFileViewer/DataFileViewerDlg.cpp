/********************************************************************
DataFileViewerDlg.cpp

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the main dialog box.

  Operation:
    - Select a data file.
    - Read the data file and fill in the various fields.
    - Read the database if the study is a waterload and the
      baseline and/or the post-waterload has been extended.
      If the study is not in the database, fill in the default minutes
      and display a status message indicating the number of extra minutes
      in the study.

HISTORY:
25-JUN-10  RET  New.
10-AUG-10  RET  Version 1.1
                Change dialog box to display the file type and version.
                  Change vDisplay_data().
********************************************************************/

#include "stdafx.h"
#include "DataFileViewer.h"
#include "DataFile.h"
#include "DataFileViewerDlg.h"
#include "Dbaccess.h"

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


// CDataFileViewerDlg dialog



CDataFileViewerDlg::CDataFileViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDataFileViewerDlg::IDD, pParent)
  , m_cstrDataFile(_T(""))
  , m_cstrPatientName(_T(""))
  , m_cstrDateOfStudy(_T(""))
  , m_cstrStatus(_T(""))
  , m_cstrBaseLineMinutes(_T(""))
  , m_cstrPostStimMinutes(_T(""))
  , m_cstrDataFileTypeVers(_T(""))
  {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  m_pdf = NULL;
}

CDataFileViewerDlg::~CDataFileViewerDlg()
  {
  if(m_pdf != NULL)
    delete m_pdf;
  }

void CDataFileViewerDlg::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_EDIT_DATA_FILE, m_cstrDataFile);
DDX_Text(pDX, IDC_EDIT_PATIENT_NAME, m_cstrPatientName);
DDX_Text(pDX, IDC_EDIT_DATE_OF_STUDY, m_cstrDateOfStudy);
DDX_Control(pDX, IDC_CHK_WATERLOAD_STUDY, m_chkWaterLoadStudy);
DDX_Control(pDX, IDC_CHK_RESEARCH_STUDY, m_chkResearchStudy);
DDX_Text(pDX, IDC_STATIC_STATUS, m_cstrStatus);
DDX_Text(pDX, IDC_EDIT_BASELINE_MINUTES, m_cstrBaseLineMinutes);
DDX_Text(pDX, IDC_EDIT_POST_STIM_MINUTES, m_cstrPostStimMinutes);
DDX_Text(pDX, IDC_EDIT_DATA_FILE_TYPE_VERS, m_cstrDataFileTypeVers);
}

BEGIN_MESSAGE_MAP(CDataFileViewerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BTN_SEL_DATA_FILE, OnBnClickedBtnSelDataFile)
  ON_BN_CLICKED(IDC_BTN_READ_FILE, OnBnClickedBtnReadFile)
END_MESSAGE_MAP()


// CDataFileViewerDlg message handlers

/********************************************************************
OnInitDialog

  Create the data file class.
  Create the utility class.
********************************************************************/
BOOL CDataFileViewerDlg::OnInitDialog()
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
  // Create the class that reads the data file.
  m_pdf = new CDataFile();
  // Create the utility class.
  m_pUtils = new CUtils();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDataFileViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDataFileViewerDlg::OnPaint() 
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
HCURSOR CDataFileViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/********************************************************************
Handler for the "Select Data File" button.

  Display a file selection dialog box.
  Save the selected file (including the path).
********************************************************************/
void CDataFileViewerDlg::OnBnClickedBtnSelDataFile()
  {
  CFileDialog *pdlg;

  pdlg = new CFileDialog(TRUE, _T(".egg"));
  pdlg->m_pOFN->lpstrInitialDir = _T("c:\\3cpm");
  if(pdlg->DoModal() == IDOK)
  {
    m_pdf->m_cstrDataPathFile = pdlg->GetPathName();
    m_cstrDataFile = m_pdf->m_cstrDataPathFile;
    m_cstrBaseLineMinutes = "0";
    m_cstrPostStimMinutes = "0";
    m_cstrDateOfStudy = "";
    m_cstrPatientName = "";
    UpdateData(FALSE);
  }
  delete pdlg;
  }

/********************************************************************
Handler for the "Read Data File" button.

  Read the data file and validate the data file header.
  Display the information from the data file.
********************************************************************/
void CDataFileViewerDlg::OnBnClickedBtnReadFile()
  {
  bool bErr = false;

  UpdateData(TRUE);
  // Read the data file and determine number of baseline and post stimulation points.
  // Open the data file and read the header.
  if(m_pdf->iOpen_file_get_hdr() == SUCCESS)
    { // Validate the header.
    if(m_pdf->bIsValid_hdr() == true)
      { // Read the data file.
      if(m_pdf->iRead() == SUCCESS)
        vDisplay_data(); // Display the data.
      else
        bErr = true;
      }
    else
      bErr = true;
    m_pdf->vClose_file();
    }
  else
    bErr = true;
  UpdateData(FALSE);
  if(bErr == true)
    MessageBox(m_pdf->m_cstrErrMsg, IDS_ERROR1);
  }

/********************************************************************
vDisplay_data - Display the data from the data file.
                Calculate the baseline and post-stimulation lengths.

                If this is a waterload study and its incomplete, an attempt
                is made to assign the minutes in the study to baseline and
                post-waterload.  In addition a status message is displayed
                indicating the total minutes in the study.

                For a research study, indicate through a status message:
                - If there are not enough minutes in the baseline or post-stimulation
                  study to analyze (need at least 4 minutes).
                - If the study was paused.
                - If the study was cancelled.

  inputs: None.

  return: None.
********************************************************************/
void CDataFileViewerDlg::vDisplay_data(void)
  {
  CFileStatus cfStatus;
  long lPos,lBaselineDataPnts, lPostStimDataPnts;
  unsigned short uBaselineMin, uPostStimMin, uNumPeriods;
  float fNumPeriods, fTotalMinutes, fTemp, fPostStimMin;

  // Display data file type and version
  m_cstrDataFileTypeVers.Format("%s, version %d", m_pdf->cstrGet_data_file_type(),
    m_pdf->uiGet_data_file_version());

  // Get the patient name.
  m_cstrPatientName = m_pdf->cstrGet_name();

  // Date of study
  m_cstrDateOfStudy = m_pdf->cstrDate_of_study();

  if(m_pdf->bIsResearch_study() == true)
    {
    m_chkWaterLoadStudy.SetCheck(BST_UNCHECKED);
    m_chkResearchStudy.SetCheck(BST_CHECKED);

    // Number of Baseline data points.
    // 2 data samples (egg and respiration) are one point.
    // If there was no baseline trailer, then use the total data points for
    // the baseline.
    if(m_pdf->m_btTrailer.lDataSamples != 0)
      lBaselineDataPnts = m_pdf->m_btTrailer.lDataSamples / MAX_CHANNELS;
    else
      lBaselineDataPnts = m_pdf->m_lTotalDataPoints / MAX_CHANNELS;

    // Length of baseline in minutes to the half minute.
    fTemp = (float)lBaselineDataPnts / DATA_POINTS_PER_MIN_1;
    m_cstrBaseLineMinutes.Format("%.1f", fTemp);

      // Calculate the post stimulation data points.
    if(m_pdf->m_btTrailer.lDataSamples != 0)
      {
      // 1 data sample (egg OR respiration) are one point.
      m_pdf->m_cfDataFile.GetStatus(cfStatus);
      lPos = (long)m_pdf->m_cfDataFile.GetPosition();
      lPostStimDataPnts = ((long)cfStatus.m_size - lPos) / 4;
      }
    else
      lPostStimDataPnts = 0;

    // Number of minutes in the post stimulation period to the half minute.
    fPostStimMin = ((float)lPostStimDataPnts / DATA_POINTS_PER_MIN_1) / 2.0F;
    m_cstrPostStimMinutes.Format("%.1f", fPostStimMin);

    if(lBaselineDataPnts < (DATA_POINTS_PER_MIN_1 * 4))
      m_cstrStatus = IDS_NOT_ENOUGH_BASELINE;
    else if(m_pdf->m_btTrailer.lDataSamples != 0
      && m_pdf->m_lPostStimDataPoints == 0)
      m_cstrStatus = IDS_STUDY_PAUSED;
    else if(m_pdf->m_btTrailer.lDataSamples == 0)
      { // Study either cancelled or paused.
      m_cstrStatus = IDS_NO_POSTSTIM_DATA;
      }
    else if(fPostStimMin < 4.0F)
      m_cstrStatus = IDS_NOT_ENOUGH_POSTSTIM_MIN;
    }
  else
    { // Waterload study
    m_chkWaterLoadStudy.SetCheck(BST_CHECKED);
    m_chkResearchStudy.SetCheck(BST_UNCHECKED);
    // Get the total data samples.
    // Baseline is 10 minutes and Post water is 30 minutes.
    fTotalMinutes = (float)m_pdf->m_lTotalDataPoints / (float)DATA_POINTS_PER_MIN_ALL;
    fNumPeriods = fTotalMinutes / (float)MIN_PER_PERIOD;
    uNumPeriods = (unsigned short)m_pUtils->fRound(fNumPeriods, 0);
    if(uNumPeriods >= 4)
      {
      uBaselineMin = 10; // Normal baseline minutes.
      uPostStimMin = 30; // Normal post-waterload minutes.
      if(uNumPeriods > 4)
        { // There is at least 10 extra minutes, read the database to
          // find out where they are.
        vGet_db_info(uNumPeriods, &uBaselineMin, &uPostStimMin);
        }
      m_cstrBaseLineMinutes.Format("%.1f", (float)uBaselineMin);
      m_cstrPostStimMinutes.Format("%.1f", (float)uPostStimMin);
      }
    else
      { // Incomplete study.  Make an attempt to assign the minutes
        // in the study to baseline and post-waterload.
      if(uNumPeriods >= 1)
        {
        if(fTotalMinutes >= 10.0F)
          m_cstrBaseLineMinutes = "10.0";
        else
          m_cstrBaseLineMinutes.Format("%.1f", fTotalMinutes);
        if(uNumPeriods > 1)
          {
          fTemp = fTotalMinutes - 10.0F;
          m_cstrPostStimMinutes.Format("%.1f", fTemp);
          }
        else
          m_cstrPostStimMinutes = "0";
        }
      else
        {
        m_cstrBaseLineMinutes.Format("%.1f", fTotalMinutes);
        m_cstrPostStimMinutes = "0";
        }
      m_cstrStatus.Format(IDS_STUDY_NOT_COMPLETE, fTotalMinutes);
      MessageBox(m_cstrStatus, IDS_INFORMATION);
      }
    }
  }

/********************************************************************
vGet_db_info - Get the information needed from the database for this
               study.

               This is used only when the study is a waterload study and
               either the baseline and/or the postwater study has been extended
               for 10 minutes.  In this case, the number of periods is 4 (1 for
               the baseline and 3 for the postwater study) plus 1 period for
               each 10 minute extension.

               The database is read only for the period lengths.
               - Normal periods is 4. Periods to decide is Periods in study
                 minus the normal periods.
               - If the baseline period length is 20 minutes then the baseline has
                 been extended for 10 minutes or 1 period.  Subtract 1 from the
                 periods to decide.
               - The remaining periods to decide is the number of periods the
                 postwater study has been extended.

               If the database can't be accessed or the study isn't in the database,
               a status message is displayed that indicates the number of extra minutes
               in the study.

  inputs: Number of 10 minute periods in the study.
          Pointer to the number of minutes in the baseline.  This gets changed
            if the baseline has an extra 10 minute period.
          Pointer to the number of minutes in the postwater study.  This gets changed
            if the postwater study has one or more extra 10 minute periods.

  return: None.
********************************************************************/
void CDataFileViewerDlg::vGet_db_info(unsigned short uNumPeriods,
unsigned short *puBaselineMin, unsigned short *puPostStimMin)
  {
  CDbaccess *pDB;
  STUDY_DATA_RECORD sd;
  int iIndex, iLen;
  bool bDBErr;

  uNumPeriods -= 4; // Remaining periods after the 4 regular periods.
  bDBErr = false;
  pDB = new CDbaccess(DSN_DEFAULT);
  if(pDB->iDBConnect() == SUCCESS)
    { // Get the study data for the selected data file.
    if(pDB->iGet_study_data(m_cstrDataFile, &sd) == SUCCESS)
      { // Look at the first period length, the baseline length.
      if((iIndex = sd.cstrPeriodLen.Find(",", 0)) > 0)
        {
        iLen = atoi(sd.cstrPeriodLen.Left(iIndex));
        if(iLen > 10)
          { // Base line has been extended for 10 minutes.
          *puBaselineMin += 10;
          --uNumPeriods; // Decrement number of remaining periods.
          }
        // Any remaining periods belong to the postwater study.
        if(uNumPeriods > 0)
          *puPostStimMin += (uNumPeriods * 10);
        }
      }
    else
      m_cstrStatus.Format(IDS_ERR_STUDY_NOT_IN_DB, uNumPeriods * 10); // Study not in database.
    }
  else
    m_cstrStatus.Format(IDS_ERR_CANT_ACCESS_DB, uNumPeriods * 10); // Database can't be accessed.
  delete pDB;
  }

