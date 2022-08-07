/********************************************************************
OrphanFileRecoveryDlg.cpp

Copyright (C) 2010 - 2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the main dialog box.

  Operation:
    - Find the orphaned data files and list them.
    - Include an orphaned data file in the database.
      This may require more input from the user.
      - All studies:
        Ask user if the name is a research reference number or a patient name.
      - WaterLoad study:
        . If there were extra minutes (or periods) in the study,
          ask the user to specify the number of minutes in the baseline
          and postwater study.
      - Incomplete or cancelled study
         . Ask if user wants to include the study anyway.
           For a research, if adding a cancelled study, add it as a paused study.

HISTORY:
28-JUN-10  RET  New.
25-JAN-13  RET
             Exit automatically if there are no orphan data files and
             the command line parameter "/E" is present.
               Change method: iFind_orphan_files()
06-MAR-13  RET
             Remove error message if no orphan files are found and disable the
             add selection to database button.
               Change method: iFind_orphan_files()
             Disable the add selection to database button afterf adding a file to
             the database if there are no more files in the list.
               Change method: OnBnClickedBtnAddSelToDb()
08-JUN-16  RET
             Fix program crash if user selects add to database without selecting
             a study.
               Change method: OnBnClickedBtnAddSelToDb()
             Add hide study date if the study date is encrypted.
               Change method: iFind_orphan_files()
23-NOV-20  RET Version 1.3
             Add ability to work with a remote database.
             Change methods: iFind_orphan_files(), OnBnClickedBtnAddSelToDb(),
               OnInitDialog()
             Add method: pdbNewDB()
********************************************************************/

#include "stdafx.h"
#include "OrphanFileRecovery.h"
#include "Dbaccess.h"
#include "OrphanFileRecoveryDlg.h"
#include "DataFile.h"
#include "WaterLoadInfo.h"
#include "NameResearchNum.h"

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


// COrphanFileRecoveryDlg dialog

/********************************************************************
Constructor

********************************************************************/
COrphanFileRecoveryDlg::COrphanFileRecoveryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COrphanFileRecoveryDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/********************************************************************
Destructor

Empty the array of orphan files.
Delete the utility class.
********************************************************************/
COrphanFileRecoveryDlg::~COrphanFileRecoveryDlg(void)
  {

  m_cstraFiles.RemoveAll();
  delete m_pUtils;
  }

void COrphanFileRecoveryDlg::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Control(pDX, IDC_LIST_ORPHAN_FILES, m_cOrphanFileList);
}

BEGIN_MESSAGE_MAP(COrphanFileRecoveryDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BTN_ADD_SEL_TO_DB, OnBnClickedBtnAddSelToDb)
  ON_MESSAGE(WMUSER_FIND_ORPHAN_FILES, iFind_orphan_files)
END_MESSAGE_MAP()


// COrphanFileRecoveryDlg message handlers

/********************************************************************
OnInitDialog

  Initialize class data.
  Set up the orphan file list box.
  Find orphan files.
********************************************************************/
BOOL COrphanFileRecoveryDlg::OnInitDialog()
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
  CString cstrItem;
  HDITEM hdiHdrItem;

  m_bRemoteDB = false; // Initialize to not using a remote database.
  // Get the data file path from the INI file.
  m_cstrPath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_DATAPATH, "");
  m_cstrPath += "\\";

  // Create the utility class.
  m_pUtils = new CUtils();

  // Set up the report list box.
  // 3 columns, patient name, date of study, study type.
  CHeaderCtrl* pHeaderCtrl = m_cOrphanFileList.GetHeaderCtrl();
  hdiHdrItem.mask = HDI_FORMAT | HDI_TEXT | HDI_WIDTH;
  // Column 1: patient name.
  cstrItem = IDS_PATIENT_NAME;
  m_cOrphanFileList.InsertColumn(0, cstrItem, LVCFMT_LEFT,
                                200, 0);
  hdiHdrItem.pszText = cstrItem.GetBuffer(cstrItem.GetLength());
  hdiHdrItem.cchTextMax = (int)strlen(hdiHdrItem.pszText);
  hdiHdrItem.cxy = 200; // Use all of window except for date.
  hdiHdrItem.fmt = HDF_LEFT | HDF_STRING; // HDF_CENTER
  pHeaderCtrl->SetItem(0, &hdiHdrItem);
  // Column 2: Date of Study
  cstrItem = IDS_DATE_OF_STUDY;
  m_cOrphanFileList.InsertColumn(1, cstrItem, LVCFMT_LEFT,
                                100, 0);
  hdiHdrItem.pszText = cstrItem.GetBuffer(cstrItem.GetLength());
  hdiHdrItem.cchTextMax = (int)strlen(hdiHdrItem.pszText);
  hdiHdrItem.cxy = 100; // Use all of window except for date.
  hdiHdrItem.fmt = HDF_LEFT | HDF_STRING; // HDF_CENTER
  pHeaderCtrl->SetItem(1, &hdiHdrItem);
  // Column 3: Type of Study
  cstrItem = IDS_TYPE_OF_STUDY;
  m_cOrphanFileList.InsertColumn(2, cstrItem, LVCFMT_LEFT,
                                100, 0);
  hdiHdrItem.pszText = cstrItem.GetBuffer(cstrItem.GetLength());
  hdiHdrItem.cchTextMax = (int)strlen(hdiHdrItem.pszText);
  hdiHdrItem.cxy = 100; // Use all of window except for date.
  hdiHdrItem.fmt = HDF_LEFT | HDF_STRING; // HDF_CENTER
  pHeaderCtrl->SetItem(2, &hdiHdrItem);

  if(theApp.m_bAutoExit == false)
    PostMessage(WMUSER_FIND_ORPHAN_FILES, 0, 0);
  else
    iFind_orphan_files(0, 0);
    
  //// Find the orphan files.
  //iFind_orphan_files();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COrphanFileRecoveryDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void COrphanFileRecoveryDlg::OnPaint() 
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
HCURSOR COrphanFileRecoveryDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/********************************************************************
iFind_orphan_files - Message handler for the WMUSER_FIND_ORPHAN_FILES
                     user message.

          Find the orphaned files.
            Method: Go through each file in the data directory and check to
                    see if its in the database.  If it is in the database
                    and the file is valid (see the description for
                    bValidate_data_file() function), add it to the orphan
                    file listbox.
          Add to the orphan file listbox: patient name, date of study
            and type of study (research or waterload) or each orphan
            file found.

inputs: Not used.
        Not used.

return: 0
********************************************************************/
HRESULT COrphanFileRecoveryDlg::iFind_orphan_files(WPARAM wParam, LPARAM lParam)
  {
  int iIndex, iSts = SUCCESS;
  HANDLE hFile;
  WIN32_FIND_DATA FindFileData;
  //char szMsg[512];
  CString cstrFile;
  CDbaccess *pDB;
  CDataFile *pdf = NULL;

  cstrFile = m_cstrPath + "*.egg";
  //pDB = new CDbaccess(DSN_DEFAULT);
  pDB = pdbNewDB();
  if((iSts = pDB->iDBConnect()) == SUCCESS)
    {
    if(m_bRemoteDB == true)
      { // Using a remote database, get the path to the data files.
      m_cstrPath = pDB->cstrGet_db_path();
      m_cstrPath += "\\";
      cstrFile = m_cstrPath + "*.egg";
     }
    hFile = FindFirstFile(cstrFile, &FindFileData);
    if(hFile != INVALID_HANDLE_VALUE)
      { // Found at least one file.
      cstrFile = m_cstrPath + FindFileData.cFileName;
      if(pDB->iFind_study(cstrFile) != SUCCESS
        && bValidate_data_file(cstrFile) == true)
        m_cstraFiles.Add(cstrFile);
      // Look for any other files.
      while(FindNextFile(hFile, &FindFileData) == TRUE)
        {
        cstrFile = m_cstrPath + FindFileData.cFileName;
        if(pDB->iFind_study(cstrFile) != SUCCESS
          && bValidate_data_file(cstrFile) == true)
          m_cstraFiles.Add(cstrFile);
        }
      FindClose(hFile);
      }
    else
      { // No files.
      // Disable the add selection to database button.
      GetDlgItem(IDC_BTN_ADD_SEL_TO_DB)->EnableWindow(FALSE);
      }
      //{ // File Error.
      //FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,  GetLastError(), 0L, szMsg, 512, NULL);
      //MessageBox(szMsg, IDS_ERROR1);
      //iSts = FAIL;
      //}
    delete pDB;
    }
  if(iSts == SUCCESS)
    {
    if(m_cstraFiles.GetCount() > 0)
      {
      // List the orphan data files.
      pdf = new CDataFile();
      for(int i = 0; i < m_cstraFiles.GetCount(); ++i)
        {
        pdf->m_cstrDataPathFile = m_cstraFiles[i];
        if((iSts = pdf->iOpen_file_get_hdr()) == SUCCESS)
          { // Validate the header.
          if((iSts = pdf->bIsValid_hdr()) == true)
            { // Read the data file.
            if((iSts = pdf->iRead()) == SUCCESS)
              {
              // Fill in the item on the orphan file list.
              // insert new item, patient name
              iIndex = m_cOrphanFileList.InsertItem(i, pdf->cstrGet_name());
              // Date of study
              // If study date is encrytped (more than 10 character), don't display it.
              if(pdf->cstrDate_of_study().GetLength() <= 10)
                m_cOrphanFileList.SetItemText(iIndex, 1, pdf->cstrDate_of_study());
              // Type of study
              if(pdf->bIsResearch_study() == true)
                m_cOrphanFileList.SetItemText(iIndex, 2, IDS_RESEARCH_STUDY);
              else
                m_cOrphanFileList.SetItemText(iIndex, 2, IDS_STANDARD_STUDY);
              }
            }
          }
        pdf->vClose_file();
        }
      if(iSts != SUCCESS)
        { // File error
        MessageBox(pdf->m_cstrErrMsg, IDS_ERROR1);
        }
      }
    else
      {
      // No orphan data files and command line indicates to automatically exit.
      if(theApp.m_bAutoExit == true)
        ::PostQuitMessage(0); // Terminate the program.
      }
    }
  if(pdf != NULL)
    delete pdf;
  return(0);
  }

/********************************************************************
OnBnClickedBtnAddSelToDb - Handler for the "Add Selection to Database" button.

- Read the selected data file and validate the header.
- Calculate the baseline minutes and post-stimulation minutes.
- If the baseline is too short or there is not enough data points in either
  the baseline or post-stimulation data, display a message to ask if the
  user still wants to add to the database.  For a research study, if it was
  cancelled, it can be added as a paused study.
- Ask the user if the patient name is a patient name or a researh number.
********************************************************************/
  void COrphanFileRecoveryDlg::OnBnClickedBtnAddSelToDb()
    {
    int iSel, iSts;
    float fTotalMinutes, fNumPeriods;
    unsigned short uNumPeriods;
    long lBaselineDataPnts, lPostStimDataPnts;
    bool bPausedStudy, bAddAsPausedStudy, bOkToAddAsPausedStudy;
    CDbaccess *pDB;
    CDataFile *pdf = new CDataFile();
    CString cstrMsg, cstrRemoteDBPath;
    ePgmType ptStudyType;
    FIND_PATIENT_INFO fpiData;

    bPausedStudy = false;
    bAddAsPausedStudy = false;
    bOkToAddAsPausedStudy = false;
    // Get the selected file.
    UpdateData(TRUE);
    iSel = m_cOrphanFileList.GetSelectionMark();
    if(iSel >= 0)
      {
      pdf->m_cstrDataPathFile = m_cstraFiles[iSel];

      // Read the data file for the information.
      if((iSts = pdf->iOpen_file_get_hdr()) == SUCCESS)
        { // Validate the header.
        if(pdf->bIsValid_hdr() == true)
          iSts = pdf->iRead(); // Read the data file.
        else
          iSts = FAIL;
        }
      pdf->vClose_file();
      if(iSts == SUCCESS)
        { // Before we go any further, check the status of the data file.
        cstrMsg.Empty();
        if(pdf->bIsResearch_study() == true)
          {
          // For research check for:
          // - No baseline trailer (cancelled)
          // - Baseline trailer and no post-stimulation data points (paused).
          // - Too little baseline data points to analyze (must be at least 4 minutes).
          // - Too little post-stimulation data points to analyze (must be at least 4 minutes).
          if(pdf->m_btTrailer.lDataSamples == 0)
            { // Study was cancelled.
            cstrMsg = IDS_STUDY_CANCELLED;
            if((pdf->m_lTotalDataPoints / MAX_CHANNELS) >= (DATA_POINTS_PER_MIN_1 * 4))
              bAddAsPausedStudy = true;
            }
          else
            {
            lPostStimDataPnts = pdf->m_lPostStimDataPoints / MAX_CHANNELS;
            lBaselineDataPnts = pdf->m_btTrailer.lDataSamples / MAX_CHANNELS;
            if(lBaselineDataPnts < (DATA_POINTS_PER_MIN_1 * 4))
              cstrMsg = IDS_NOT_ENOUGH_BASELINE;
            else if(pdf->m_lPostStimDataPoints == 0)
              bPausedStudy = true;
            else if(lPostStimDataPnts < (DATA_POINTS_PER_MIN_1 * 4))
              cstrMsg = IDS_NOT_ENOUGH_POSTSTIM_MIN;
            }
          }
        else
          {
          // For waterload check for:
          // - Incomplete study, less than 10 minutes (cancelled during baseline)
          //                     10 minutes, not post-waterload
          //                     > 10 minutes, cancelled during post-waterload
          fTotalMinutes = (float)pdf->m_lTotalDataPoints / (float)DATA_POINTS_PER_MIN_ALL;
          fNumPeriods = fTotalMinutes / (float)MIN_PER_PERIOD;
          uNumPeriods = (unsigned short)m_pUtils->fRound(fNumPeriods, 0);
          if(fTotalMinutes < 10.0F)
            cstrMsg = IDS_STUDY_CANCELLED_DURING_BASELINE;
          else if(uNumPeriods == 1)
            cstrMsg = IDS_NO_POSTWATER_STUDY;
          else if (uNumPeriods < 4)
            cstrMsg = IDS_STUDY_CANCELLED_DURING_POSTWATER_STUDY;
          }
        if(cstrMsg.IsEmpty() == FALSE)
          { // Ask user if study should be added to the database.
          cstrMsg += "\r\n";
          cstrMsg += IDS_WANT_TO_ADD_TO_DB;
          if(bAddAsPausedStudy == true)
            {
            cstrMsg += "\r\n\r\n";
            cstrMsg += IDS_ADD_AS_PAUSED_STUDY;
            }
          iSts = MessageBox(cstrMsg, "", MB_ICONQUESTION | MB_YESNO);
          if(iSts == IDYES)
            {
            iSts = SUCCESS;
            if(bAddAsPausedStudy == true)
              bOkToAddAsPausedStudy = true;
            }
          else
            iSts = FAIL;
          }
        }
      if(iSts == SUCCESS)
        {
        pDB = pdbNewDB();
        if(pDB->iDBConnect() == SUCCESS)
          {
          if(m_bRemoteDB == true)
            { // Using a remote database, get the path to the data files.
            m_cstrPath = pDB->cstrGet_db_path();
            m_cstrPath += "\\";
            }
          if(pdf->bIsResearch_study() == true)
            ptStudyType = PT_RESEARCH;
          else
            ptStudyType = PT_STANDARD;
          vInit_patient_data(ptStudyType);
          m_pdPatient.bPause = bPausedStudy;
          // Ask if using research number.
          CNameResearchNum *pdlg = new CNameResearchNum(pdf->cstrGet_name());
          pdlg->DoModal();
          if(pdlg->m_bPatientName == TRUE)
            {
            m_pdPatient.cstrFirstName = pdf->m_cstrFirstName;
            m_pdPatient.cstrLastName = pdf->m_cstrLastName;
            m_pdPatient.cstrMI = pdf->m_cstrMI;
            m_pdPatient.bUseResrchRefNum = false;
            m_pdPatient.cstrResearchRefNum = "";
            }
          else
            {
            m_pdPatient.bUseResrchRefNum = true;
            m_pdPatient.cstrResearchRefNum = pdf->cstrGet_name();
            m_pdPatient.cstrFirstName = "";
            m_pdPatient.cstrLastName = "";
            m_pdPatient.cstrMI = "";
            }
          delete pdlg;
          iSts = SUCCESS;
          // For Waterload, ask user for number of baseline and postwater
          // study periods if there are more than 4 total periods (i.e. 
          // the baseline and/or the postwater study has been extended).
          if(ptStudyType == PT_STANDARD)
            {
            if(uNumPeriods > 4)
              {
              CWaterLoadInfo *pwliDlg = new CWaterLoadInfo();
              pwliDlg->m_uTotalMin = uNumPeriods * 10;
              if(pwliDlg->DoModal() == IDOK)
                { // Set up the baseline and postwater periods and lengths.
                // Baseline
                if(pwliDlg->m_uBaselineMin > 10)
                  m_pdPatient.fPeriodLen[0] = (float)20;
                else
                  m_pdPatient.fPeriodLen[0] = (float)10;
                m_pdPatient.fPeriodStart[0] = (float)0;
                // Postwater.
                m_pdPatient.fPeriodStart[1] = m_pdPatient.fPeriodLen[0];
                m_pdPatient.fPeriodLen[1] = (float)10;
                m_pdPatient.fPeriodStart[2] = m_pdPatient.fPeriodStart[1] + (float)10;
                m_pdPatient.fPeriodLen[2] = (float)10;
                m_pdPatient.fPeriodStart[3] = m_pdPatient.fPeriodStart[2] + (float)10;
                // Check to see if the third post-waterload period been extended.
                // If it has been extended, change the period length.
                if((m_pdPatient.fPeriodLen[0] + 30) < pwliDlg->m_uTotalMin)
                  m_pdPatient.fPeriodLen[3] = (float)20;
                else
                  m_pdPatient.fPeriodLen[3] = (float)10;
                //m_pdPatient.fPeriodStart[4] = m_pdPatient.fPeriodStart[3] + (float)10;
                //m_pdPatient.fPeriodLen[4] = (float)10;
                }
              else
                iSts = FAIL; // User didn't specify minutes.
              delete pwliDlg;
              }
            }
          else
            { // Research study.
            m_pdPatient.bManualMode = true;
            if(pdf->m_btTrailer.lDataSamples == 0)
              m_pdPatient.uDataPtsInBaseline = pdf->m_lTotalDataPoints;
            else
              m_pdPatient.uDataPtsInBaseline = pdf->m_btTrailer.lDataSamples;
            m_pdPatient.uDataPtsInBaseline /= 2; // EGG and respiration for each data point.
            m_pdPatient.uPostStimDataPoints = pdf->m_lPostStimDataPoints;
            if(bOkToAddAsPausedStudy == true || pdf->m_lPostStimDataPoints == 0)
              m_pdPatient.bPause = true;
            // Set initial baseline start and length.
            m_pdPatient.fPeriodStart[PERIOD_BL] = (float)0.0;
            m_pdPatient.fPeriodLen[PERIOD_BL] = 
              m_pUtils->fRound_down_to_half_min(
              (float)m_pdPatient.uDataPtsInBaseline / (float)DATA_POINTS_PER_MIN_1);
            // Set the first post-stimulation start and length.
            m_pdPatient.fPeriodStart[1] = (float)0.0;
            m_pdPatient.fPeriodLen[1] = m_pUtils->fRound_down_to_half_min(
              (float)m_pdPatient.uPostStimDataPoints / (float)(DATA_POINTS_PER_MIN_1 * 2));
            }
          if(iSts == SUCCESS)
            {
            if(bOkToAddAsPausedStudy == true)
              {  // Convert the cancelled study to a paused study.
              // Add a baseline trailer to the end of the data file.
              if((iSts = pdf->iAdd_baseline_trailer()) != SUCCESS)
                MessageBox(pdf->m_cstrErrMsg, IDS_ERROR1);
              }
            if(iSts == SUCCESS)
              {
              // See if the patient already exists in the database.
              fpiData.bUseResrchRefNum = m_pdPatient.bUseResrchRefNum;
              fpiData.cstrResearchRefNum = m_pdPatient.cstrResearchRefNum;
              fpiData.cstrFirstName = m_pdPatient.cstrFirstName;
              fpiData.cstrLastName = m_pdPatient.cstrLastName;
              fpiData.cstrMI = m_pdPatient.cstrMI;
              if(pDB->iFind_patient_name(&fpiData) == SUCCESS)
                m_pdPatient.lPatientID = fpiData.lPatientID; // Patient in database.
              // Add to patient name table
              // Add to patient info table
              if(pDB->iSave_patient(&m_pdPatient) == SUCCESS
                && pDB->iSave_patient_info(&m_pdPatient) == SUCCESS)
                { // Add to study table
                // For a waterload study, set up the period lengths.
                m_pdPatient.cstrDate = pdf->cstrDate_of_study();
                m_pdPatient.cstrFile = pdf->m_cstrDataPathFile;
                if(pDB->iSave_study_data(&m_pdPatient) == SUCCESS)
                  {
                  cstrMsg.Format(IDS_STUDY_ADD_TO_DB, pdf->cstrGet_name(), pdf->cstrDate_of_study());
                  MessageBox(cstrMsg, IDS_INFORMATION);
                  // Remove the data file from the listbox.
                  iSel = m_cOrphanFileList.GetSelectionMark();
                  m_cstraFiles.RemoveAt(iSel);
                  m_cOrphanFileList.DeleteItem(iSel);
                  }
                }
              }
            }
          }
        delete pDB;
        }
      pdf->vClose_file();
      delete pdf;
      if(m_cOrphanFileList.GetItemCount() == 0)
        { // No orphan files left in list.
        // Disable the add selection to database button.
        GetDlgItem(IDC_BTN_ADD_SEL_TO_DB)->EnableWindow(FALSE);
        }
      }
      else
         MessageBox(IDS_PLEASE_MAKE_SELECTION, IDS_ERROR1);
    }

/********************************************************************
vInit_patient_data - Initialize all data associated with a patient.
                         
  inputs: None.

  return: None.
********************************************************************/
void COrphanFileRecoveryDlg::vInit_patient_data(ePgmType ptStudyType)
  {
  short int iIndex;

    // Initialize the PATIENT_DATA structure for the current patient.
  m_pdPatient.lPatientID = INV_LONG;
	m_pdPatient.cstrLastName.Empty();
	m_pdPatient.cstrFirstName.Empty();
  m_pdPatient.cstrMI.Empty();
	m_pdPatient.cstrAddress1.Empty();
	m_pdPatient.cstrAddress2.Empty();
	m_pdPatient.cstrCity.Empty();
	m_pdPatient.cstrDateOfBirth.Empty();
	m_pdPatient.cstrHospitalNum.Empty();
	m_pdPatient.cstrPhone.Empty();
  m_pdPatient.bUseResrchRefNum = 0;
	m_pdPatient.cstrResearchRefNum.Empty();
	m_pdPatient.cstrSocialSecurityNum.Empty();
	m_pdPatient.cstrState.Empty();
	m_pdPatient.cstrZipCode.Empty();
    // Pre-study information
  m_pdPatient.cstrMealTime.Empty();
	m_pdPatient.cstrAttendPhys.Empty();
	m_pdPatient.cstrIFPComments.Empty();
	m_pdPatient.cstrReferPhys.Empty();
	m_pdPatient.cstrTechnician.Empty();
	m_pdPatient.bBloating = FALSE;
	m_pdPatient.bEarlySateity = FALSE;
	m_pdPatient.bEructus = FALSE;
	m_pdPatient.bGerd = FALSE;
	m_pdPatient.bNausea = FALSE;
	m_pdPatient.bPPFullness = FALSE;
	m_pdPatient.bVomiting = FALSE;
  m_pdPatient.bDelayedGastricEmptying = FALSE;
    // Water
  m_pdPatient.uWaterAmount = 0; // Amount of water
  m_pdPatient.uWaterUnits = 0; // Units for the amount of water.
    // Study data
  m_pdPatient.lStudyID = INV_LONG;
  m_pdPatient.cstrDate.Empty(); // Date of study.
  m_pdPatient.cstrFile.Empty(); // Name of file.
  m_pdPatient.uNumDataPts = 0; // Number of data points read from file for period.
  if(ptStudyType == PT_STANDARD)
    { // Standard version
    for(iIndex = 0; iIndex < MAX_MANUAL_MODE_PERIODS; ++iIndex)
      {
      m_pdPatient.fPeriodLen[iIndex] = (float)10;
      m_pdPatient.fPeriodStart[iIndex] = (float)(iIndex * 10);
      m_pdPatient.fGoodMinStart[iIndex] = INV_MINUTE;
      m_pdPatient.fGoodMinEnd[iIndex] = INV_MINUTE;
      m_pdPatient.uRespRate[iIndex] = 0;
      }
    }
  else
    { // Research version
    for(iIndex = 0; iIndex < MAX_TOTAL_PERIODS; ++iIndex)
      {
      m_pdPatient.fPeriodLen[iIndex] = (float)INV_PERIOD_LEN;
      m_pdPatient.fPeriodStart[iIndex] = (float)INV_PERIOD_START;
      m_pdPatient.fGoodMinStart[iIndex] = INV_MINUTE;
      m_pdPatient.fGoodMinEnd[iIndex] = INV_MINUTE;
      m_pdPatient.uRespRate[iIndex] = 0;
      }
    }
  m_pdPatient.fMaxEGG = (float)0.0;
  m_pdPatient.fMinEGG = (float)0.0;
  m_pdPatient.fMaxResp = (float)0.0;
  m_pdPatient.fMinResp = (float)0.0;
  m_pdPatient.fTestMaxEGG = (float)0.0;
  m_pdPatient.fTestMinEGG = (float)0.0;
  m_pdPatient.fMeanBaseline = INV_FLOAT;

  for(iIndex = 0; iIndex < MAX_TOTAL_PERIODS; ++iIndex)
    m_pdPatient.bPeriodsAnalyzed[iIndex] = false;

  m_pdPatient.bExtra10Min = FALSE;
  m_pdPatient.bManualMode = FALSE;

  m_pdPatient.bUserDiagnosis = FALSE;
  m_pdPatient.cstrDiagnosis.Empty();
  m_pdPatient.cstrSuggestedDiagnosis.Empty();

  m_pdPatient.bHavePatientInfo = false;
  m_pdPatient.uNumPeriods = 0;
  m_pdPatient.cstrStimMedDesc.Empty();
  m_pdPatient.uDataPtsInBaseline = 0;
  m_pdPatient.bPause = FALSE;
  m_pdPatient.uPostStimDataPoints = 0;
  m_pdPatient.uPatientType = PAT_HUMAN; // Default patient type.
  }

/********************************************************************
bValidate_data_file - Validate a data file. Open the file and read
                      it.  Then make sure the header is valid and there
                      are data points in the file.
                         
  inputs: CString object containing the file name.

  return: true if the file is valid.
          false if no data points, the file header is invalid or there
            was a file error.
********************************************************************/
bool COrphanFileRecoveryDlg::bValidate_data_file(CString cstrFile)
  {
  bool bRet = false;
  CDataFile *pdf = new CDataFile();

  pdf->m_cstrDataPathFile = cstrFile;
  // Read the data file for the information.
  if(pdf->iOpen_file_get_hdr() == SUCCESS
  && pdf->bIsValid_hdr() == true
  && pdf->iRead() == SUCCESS)
    {
    if(pdf->m_lTotalDataPoints > 0)
      bRet = true;
    }
  pdf->vClose_file();
  return(bRet);
  }

/********************************************************************
pdbNewDB - Create a new database object.
           A check is made to see if using a remote database.
                         
  inputs: CString object containing the file name.

  return: true if the file is valid.
          false if no data points, the file header is invalid or there
            was a file error.
********************************************************************/
CDbaccess *COrphanFileRecoveryDlg::pdbNewDB()
  {
  CDbaccess *pDB;
  char szDB[2048];

  // The DEFAULT database may not exist if user is using a database on a remote
  // computer.  So we have to get the remote database path from the EGGSAS.INI file.
  // If it is empty then we use the default.
  //cstrRemoteDBPath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_PATH, "");
  GetPrivateProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_PATH, "", szDB, 2048, INI_FILE);
  if(szDB[0] == NULL)
    {
    GetPrivateProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_PATH, "", szDB, 2048, INI_FILE_CDDR);
    if(szDB[0] == NULL)
      GetPrivateProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_PATH, "", szDB, 2048, INI_FILE_RDR);
    }
  if(szDB[0] == NULL)
    pDB = new CDbaccess(DSN_DEFAULT);
  else
    {
    pDB = new CDbaccess(szDB);
    m_bRemoteDB = true;
    }
  return(pDB);
  }