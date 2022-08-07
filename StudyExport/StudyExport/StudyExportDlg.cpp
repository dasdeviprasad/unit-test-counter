/********************************************************************
StudyExportDlg.cpp

Copyright (C) 2008 - 2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CStudyExportDlg class, the main window for
    the application.

HISTORY:
03-NOV-08  RET  New.
Version 1.02
  07-DEC-08  RET  Change OnInitDialog() to create a path for the INI file.
Version 1.03
  05-FEB-09  RET  Add bFileExists().
                  Change vFill_patient_listbox() to check if the data file exists
                    for the study and if it doesn't exist, don't put the study in the list.
                  Change iCopy_study_data() to only copy the data file once and not for
                    each WhatIf record.
                  Change iCreate_exp_db() to ask if user wants to continue with the
                    export if there is an error.
Version 1.04
  27-FEB-09  RET  Add Help button.
Version 1.05
  27-JAN-11  RET  Add help in static text field displayed below the listbox
                    for selecting multiple studies.
                      Add methods: lCalc_font_height(), vSet_font()
                      Change methods: OnInitDialog()
09-JUN-16  RET Version 1.06
             Add ability to hide study dates if they are encoded.
               Change method: vFill_patient_listbox()
25-NOV-17  RET Version 1.07
             Users don't seem to know how to make multiple list box selections
						 (i.e don't know how to use Ctrl Left Click).
						 Changed the Left Click to behave as a Ctrl Left Click.
						   Add methods: vCtrl_key_down(), vCtrl_key_up()
							 Add event handlers: OnEnKillfocusEditDestination(), OnEnSetfocusEditDestination(),
							   PreTranslateMessage(), OnClose(), OnBnClickedOk(), OnNMClickListPatient()
							 Change methods: OnBnClickedBtnHelp(), OnBnClickedBtnExport(),
							   OnBnClickedBtnSelectDest(), OnInitDialog()
						   Add variable: m_bAllowCtrlKeyUp
30-JAN-18  RET Version 1.08
             Reworked multiple selection without using the control key.
						   Remove methods: vCtrl_key_down(), vCtrl_key_up()
						   Remove variable: m_bAllowCtrlKeyUp
							 Remove event handlers: OnEnKillfocusEditDestination(), OnEnSetfocusEditDestination(),
							   PreTranslateMessage()
19-MAY-18  RET Version 1.09
             Add database search button to main screen.  This allows the user
						 to select studies to export based on names/research reference numbers
						 and/or dates.  Both allows use of wild cards.
						   Add methods: OnBnClickedBtnSearchDb()
8-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedBtnHelp()
27-NOV-20  RET
            Changes to allow using a remote database.
              Change method: OnInitDialog()
							Add method: pdbNewDB()
********************************************************************/

#include "stdafx.h"
#include "StudyExport.h"
#include "StudyExportDlg.h"
#include "SearchDB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define HELP_SEL_STUDIES _T("To Export more than one study, hold down the control key (Ctrl) and select all studies to export.  Then click on the Export button.")
#define HELP_SEL_STUDIES _T("To Export more than one study, select all studies to export.  Then click on the Export button.")
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
//  afx_msg void OnStnClickedStaticVersion();
//  afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//  ON_STN_CLICKED(IDC_STATIC_VERSION, &CAboutDlg::OnStnClickedStaticVersion)
//ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CStudyExportDlg dialog




CStudyExportDlg::CStudyExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStudyExportDlg::IDD, pParent)
  , m_cstrDestination(_T(""))
  , m_cstrProgress(_T(""))
  , m_iPatientsInList(0)
  , m_iStudiesInList(0)
  , m_iSelectedPatients(0)
  , m_iSelectedStudies(0)
	, m_bSelected(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_pDBA = NULL;
  m_pSett = NULL;
}

CStudyExportDlg::~CStudyExportDlg(void)
{

  if(m_pDBA != NULL)
    delete m_pDBA;
  if(m_pSett != NULL)
    delete m_pSett;
}

void CStudyExportDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_PATIENT, m_cPatientList);
  DDX_Text(pDX, IDC_EDIT_DESTINATION, m_cstrDestination);
  DDX_Control(pDX, IDC_PROGRESS_EXPORT, m_cExportProgressBar);
  DDX_Text(pDX, IDC_STATIC_PROGRESS, m_cstrProgress);
  DDX_Text(pDX, IDC_EDIT_PATIENTS_IN_LIST, m_iPatientsInList);
  DDX_Text(pDX, IDC_EDIT_STUDIES_IN_LIST2, m_iStudiesInList);
  DDX_Text(pDX, IDC_EDIT_SELECTED_PATIENTS, m_iSelectedPatients);
  DDX_Text(pDX, IDC_EDIT_SELECTED_STUDIES, m_iSelectedStudies);
}

BEGIN_MESSAGE_MAP(CStudyExportDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_BTN_SELECT_DEST, &CStudyExportDlg::OnBnClickedBtnSelectDest)
  ON_BN_CLICKED(IDC_BTN_EXPORT, &CStudyExportDlg::OnBnClickedBtnExport)
  ON_BN_CLICKED(IDC_BTN_HELP, &CStudyExportDlg::OnBnClickedBtnHelp)
  ON_NOTIFY(NM_CLICK, IDC_LIST_PATIENT, &CStudyExportDlg::OnNMClickListPatient)
  ON_BN_CLICKED(IDOK, &CStudyExportDlg::OnBnClickedOk)
  ON_WM_CLOSE()
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PATIENT, &CStudyExportDlg::OnLvnItemchangedListPatient)
ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST_PATIENT, &CStudyExportDlg::OnLvnItemchangingListPatient)
ON_BN_CLICKED(IDC_BTN_SEARCH_DB, &CStudyExportDlg::OnBnClickedBtnSearchDb)
END_MESSAGE_MAP()


// CStudyExportDlg message handlers

/********************************************************************
OnInitDialog

- Initialize the dialog box.
- Create the settings object.
- Create the local database object.
- Set up the patient listbox and fill it.
********************************************************************/
BOOL CStudyExportDlg::OnInitDialog()
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
  HDITEM hdiHdrItem;
  CString cstrItem1, cstrItem2, cstrIniFile;
  RECT rRect;
  int iColWidth;
	wchar_t szDataPath[1024];

    // Get the current directory.
  GetCurrentDirectory(1024, m_szCurDir);
#ifdef _DEBUG
  cstrIniFile = INI_FILE;
#else
    // Make a path for the ini file.  The current directory is different depending
    // on if this program was started by EGGSAS program or started by itself.
  cstrItem1 = m_szCurDir;
  if(cstrItem1.Find(PROGRAM_DIR) < 0)
  {
    cstrItem1 += _T("\\");
    cstrItem1 += PROGRAM_DIR;
  }
  cstrIniFile.Format(_T("%s\\%s"), cstrItem1, INI_FILE);
#endif

    // insert 4 columns (REPORT mode) and modify the new header items
  m_cPatientList.GetWindowRect(&rRect);
  cstrItem1.LoadStringW(IDS_LAST_NAME);
  iColWidth = (rRect.right - rRect.left) / 5;
  m_cPatientList.InsertColumn(0, cstrItem1, LVCFMT_LEFT, iColWidth, 0);
  cstrItem2.LoadStringW(IDS_FIRST_NAME);
  m_cPatientList.InsertColumn(1, cstrItem2, LVCFMT_LEFT, iColWidth, 1);
  cstrItem2.LoadStringW(IDS_REF_NUMBER);
  m_cPatientList.InsertColumn(2, cstrItem2, LVCFMT_LEFT, iColWidth, 2);
  cstrItem2.LoadStringW(IDS_ADDRESS);
  m_cPatientList.InsertColumn(3, cstrItem2, LVCFMT_LEFT, iColWidth, 3);
  cstrItem2.LoadStringW(IDS_DATE_OF_STUDY);
  m_cPatientList.InsertColumn(4, cstrItem2, LVCFMT_LEFT, iColWidth, 4);

    // Now add the header items.
  CHeaderCtrl* pHeaderCtrl = m_cPatientList.GetHeaderCtrl();
  hdiHdrItem.mask = HDI_FORMAT | HDI_TEXT | HDI_WIDTH;
  hdiHdrItem.pszText = cstrItem1.GetBuffer(cstrItem1.GetLength());
  hdiHdrItem.cchTextMax = cstrItem1.GetLength(); //strlen(hdiHdrItem.pszText);
  hdiHdrItem.cxy = iColWidth;
  hdiHdrItem.fmt = HDF_LEFT | HDF_STRING; // HDF_CENTER
  pHeaderCtrl->SetItem(0, &hdiHdrItem);

    // Allows full row to show selection.  Also allows mouse click
    // for selection to be anywhere on the row.
  m_cPatientList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    // Class for program settings.
  m_pSett = new CSettings(cstrIniFile);

    // Database access class, local 3CPM EGGSAS database or remote database.
	m_pDBA = pdbNewDB();

  //m_pDBA = new CDBAccess(LOCAL_DSN_DEFAULT);
  m_pDBA->iDBConnect();

  m_cstrDestination = m_pSett->cstrGet_item(SETT_ITEM_EXPORT_DIR);


  vFill_patient_listbox(); // Fill the patient listbox from the local database.

  m_cstrProgress.LoadStringW(IDS_EXPORT_PROGRESS);

  // Set the font for the help text.
  CFont cfBoldFont;
  vSet_font(this, NULL, &cfBoldFont, FW_BOLD);
    // Set the font in the dialog box fields.
  GetDlgItem(IDC_STATIC_HELP)->SetFont(&cfBoldFont);
  GetDlgItem(IDC_STATIC_HELP)->SetWindowTextW(HELP_SEL_STUDIES);

  UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CStudyExportDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CStudyExportDlg::OnPaint()
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
HCURSOR CStudyExportDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/********************************************************************
OnBnClickedBtnSelectDest - Handler for the "Select Destination" button.

- Select the destination for the exported database.
- Display a file save dialog box.
********************************************************************/
void CStudyExportDlg::OnBnClickedBtnSelectDest()
{

  iSelectDestination();
	m_cPatientList.SetFocus();
}

/********************************************************************
OnBnClickedBtnExport - Handler for the "Export" button.

- Copy the database records for the selected studies to the destination database.
********************************************************************/
void CStudyExportDlg::OnBnClickedBtnExport()
{

  vExport();
	m_cPatientList.SetFocus();
}

/********************************************************************
OnBnClickedBtnHelp - Handler for the "Help" button.

- Display Help.
********************************************************************/
void CStudyExportDlg::OnBnClickedBtnHelp()
{

  //AfxGetApp()->WinHelp(0, HELP_CONTENTS);
  HtmlHelp(1001);

	m_cPatientList.SetFocus();
}

/********************************************************************
OnClose - Handler for clicking on the "Ok" button.

********************************************************************/
void CStudyExportDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

/********************************************************************
OnClose - Handler for closing the window.

********************************************************************/
void CStudyExportDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnClose();
}

/********************************************************************
OnNMClickListPatient - Handler for left clicking in the list box

********************************************************************/
void CStudyExportDlg::OnNMClickListPatient(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;
	int State = m_cPatientList.GetItemState(pNMListView->iItem,LVIS_SELECTED) & LVIS_SELECTED;

	m_bSelected = true; // Item is being selected.
	// Set the item selected and give it the focus.
	m_cPatientList.SetItemState(pNMListView->iItem,(State ^ LVIS_SELECTED)|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
	m_bSelected = false;

	*pResult = 0;
}

/********************************************************************
OnNMClickListPatient - Handler for an item in the list box than has changed.

********************************************************************/
void CStudyExportDlg::OnLvnItemchangedListPatient(NMHDR *pNMHDR, LRESULT *pResult)
{
	//// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;
	if(!m_bSelected && pNMListView->uChanged == LVIF_STATE)
	{
		if ((pNMListView->uOldState ^ pNMListView->uNewState) & LVIS_SELECTED)
		{
			// restore old selection
			m_bSelected = true;
			// Set item to selected.
			m_cPatientList.SetItemState(pNMListView->iItem,pNMListView->uOldState & LVIS_SELECTED,LVIS_SELECTED);
			m_bSelected = false;
		}
	}
}

/********************************************************************
OnNMClickListPatient - Handler for an item in the list box than is changing.

********************************************************************/
void CStudyExportDlg::OnLvnItemchangingListPatient(NMHDR *pNMHDR, LRESULT *pResult)
{
	//// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0; // Allows item to be changed.
	if(!m_bSelected && pNMListView->uChanged == LVIF_STATE)
	{
		if(!((pNMListView->uOldState ^ pNMListView->uNewState) & LVIS_FOCUSED))
			*pResult = 1; // Item has the focus so it shouldn't be changed now.
	}
}

/////////////////////////////////////////////////////////////////////
////  SUPPORT METHODS

/********************************************************************
iSelectDestination - Display a "Save As" dialog box for the user to select
                     a destination drive/directory.

  inputs: None.

  return: SUCCESS if destination is selected, otherwise FAIL.
********************************************************************/
int CStudyExportDlg::iSelectDestination(void)
  {
  CFileDialog *pdlg;
  CString cstrDlgboxPath, cstrStartPath;
  int iIndex, iRet;

    // Get the last path selected by the user from the settings file.
  cstrStartPath = m_pSett->cstrGet_item(SETT_ITEM_EXPORT_DIR); //_T("ExportDir"));
    // Display a "Save As" dialog box.
  pdlg = new CFileDialog(FALSE, (LPCTSTR)_T(".mdb"), (LPCTSTR)_T("3CPM EGGSAS EXP.MDB"));//, 
  if(cstrStartPath.IsEmpty() == false)
    pdlg->GetOFN().lpstrInitialDir = (LPCTSTR)cstrStartPath;
  if(pdlg->DoModal() == IDOK)
    { // User selected a data file.
    iRet = SUCCESS;
    cstrDlgboxPath = pdlg->GetPathName(); // Destination/source Path and file
      // Remove the file name from the path.
    if((iIndex = cstrDlgboxPath.ReverseFind('\\')) >= 0)
      cstrDlgboxPath.Delete(iIndex, cstrDlgboxPath.GetLength() - iIndex);
    m_cstrDestination = cstrDlgboxPath; // Save the destination.
    UpdateData(FALSE);
    }
  else
    iRet = FAIL;
  delete pdlg;
    // Restore the directory we started from.  If backup/restore used a USB drive
    // this releases the USB drive so it can be removed.
  SetCurrentDirectory(m_szCurDir);
  return(iRet);
  }

/********************************************************************
vFill_patient_listbox - Fill in the listbox with patient names and dates
                        of study from the database.

                        The study ID is saved as the data item for each
                        listbox entry.

  inputs: None.

  return: None.
********************************************************************/
void CStudyExportDlg::vFill_patient_listbox(void)
{
  CString cstrAddr;
  PATIENT_TABLE PatientTable;
  FIND_NEXT_STUDY fnsData;
  PATIENT_INFO PatientInfo;
  short int iPatientPos, iStudyPos, iPatientSts, iStudySts;
  int iIndex, iCnt;

    // Display the hourglass cursor.
  SetCursor(LoadCursor(NULL, IDC_WAIT));
  ShowCursor(TRUE);
    // Fill in the list box from the database.
  iPatientSts = SUCCESS;
  iPatientPos = FIRST_RECORD;
  iStudySts = SUCCESS;
  iCnt = 0;
  m_iPatientsInList = 0;
  m_iStudiesInList = 0;
  while(iPatientSts == SUCCESS
  && (iStudySts == SUCCESS || iStudySts == SUCCESS_NO_DATA))
  {
    if((iPatientSts = m_pDBA->iGet_next_patient(iPatientPos, &PatientTable)) == SUCCESS)
    {
      PatientInfo.cstrAddress1.Empty();
      if(m_pDBA->iGet_patient_info(PatientTable.lPatientID, &PatientInfo) == SUCCESS)
      {
        iStudySts = SUCCESS;
        iStudyPos = FIRST_RECORD;
        fnsData.lPatientID = PatientTable.lPatientID;
        while(iStudySts == SUCCESS)
        {
          if((iStudySts = m_pDBA->iFind_next_study(iStudyPos, &fnsData)) == SUCCESS)
          {
            // Got the study, make sure the data file exists.
            if(bFileExists(fnsData.cstrFileName) == true)
            {
              if(iStudyPos == FIRST_RECORD)
                ++m_iPatientsInList;
              ++m_iStudiesInList;
              iIndex = m_cPatientList.InsertItem(iCnt, PatientTable.cstrLastName);
              m_cPatientList.SetItemText(iIndex, 1, PatientTable.cstrFirstName);
              m_cPatientList.SetItemText(iIndex, 2, PatientTable.cstrResearchRefNum);
              m_cPatientList.SetItemText(iIndex, 3, PatientInfo.cstrAddress1);
              // If the date is longer than 10 characters, then the date is encrypted
              // and don't display it.
              if(fnsData.cstrDate.GetLength() <= 10)
                m_cPatientList.SetItemText(iIndex, 4, fnsData.cstrDate);
                // Save the study ID.
              m_cPatientList.SetItemData(iIndex, (DWORD)fnsData.lStudyID);
              ++iCnt;
            }
          }
          else if (iStudySts != SUCCESS_NO_DATA)
            break;
          iStudyPos = NEXT_RECORD;
        }
      }
    }
    iPatientPos = NEXT_RECORD;
  }
    // Restore the arrow cursor.
  SetCursor(LoadCursor(NULL, IDC_ARROW));
  ShowCursor(TRUE);
  UpdateData(FALSE);
}

/********************************************************************
vExport - Copy all database records for the studies selected in the patient
          listbox to the destination.

  inputs: None.

  return: None.
********************************************************************/
void CStudyExportDlg::vExport(void)
{
  CString cstrExistFile, cstrNewFile, cstrConn, cstrMsg, cstrTitle;
  CDBAccess *pDBA = NULL;
  POSITION pos;
  short int iSts = SUCCESS;
  bool bOkToExport;

  UpdateData(TRUE);

    // First make sure at least one study was selected.
  pos = m_cPatientList.GetFirstSelectedItemPosition();
  if (pos == NULL)
  {
    CString cstrErr, cstrTitle;
    cstrErr.LoadStringW(IDS_SELECT_STUDY);
    cstrTitle.LoadStringW(IDS_ERROR);
    MessageBox(cstrErr, cstrTitle);
  }
  else
  {
      // Set up progress bar.
    m_cstrProgress.LoadStringW(IDS_EXPORT_PROGRESS);
    m_cExportProgressBar.SetRange(0, m_cPatientList.GetSelectedCount());
    m_cExportProgressBar.SetPos(0);
    m_cExportProgressBar.SetStep(1);
    UpdateData(FALSE);

    //Copy empty "3CPM EGGSAS.mdb" to "3CPM EGGSAS EXP.mdb" in the target directory.
/****************
#ifdef _DEBUG
    cstrExistFile.Format(_T("C:\\3CPM\\%s%s"), EMPTY_DB, DB_EXTENSION);
#else
    cstrExistFile.Format(_T("..\\%s%s"), EMPTY_DB, DB_EXTENSION);
#endif
*******************/
    cstrExistFile.Format(_T("%s\\%s%s"), m_pSett->cstrGet_item(SETT_ITEM_DATA_PATH),
      EMPTY_DB, DB_EXTENSION);
    bOkToExport = true;
      // Find out if the destination directory exists.
    if(SetCurrentDirectory(m_cstrDestination) == FALSE)
    { // Directory doesn't exist.  Ask if user wants to create it.
      cstrMsg.Format(IDS_DIR_CREATE, m_cstrDestination);
      if(MessageBox(cstrMsg, NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
      { // Create the directory.
        if(CreateDirectory(m_cstrDestination, NULL) == FALSE)
        { // Directory create failed.
          CString cstrTitle;
          cstrTitle.LoadStringW(IDS_ERROR);
          cstrMsg.Format(IDS_CREATE_DIR_ERR, m_cstrDestination);
          MessageBox(cstrMsg, cstrTitle, MB_ICONERROR | MB_OK);
          bOkToExport = false;
        }
      }
      else
        bOkToExport = false; // Doesn't want to create directory.
        // Set the current directory back again.
      SetCurrentDirectory(m_szCurDir);
    }
    if(bOkToExport == true)
    {
      cstrNewFile.Format(_T("%s\\%s%s"), m_cstrDestination, EXP_DSN_DEFAULT, DB_EXTENSION);
      if(bCopy_file((LPCTSTR)cstrExistFile, (LPCTSTR)cstrNewFile) == TRUE)
      {
        m_pSett->vWrite_string_item(SETT_ITEM_EXPORT_DIR, m_cstrDestination);
        // Connect to export database.
        cstrConn.Format(_T("%s\\%s"), m_cstrDestination, EXP_DSN_DEFAULT);
        pDBA = new CDBAccess(cstrConn);
        if(pDBA->iDBConnect() == SUCCESS)
        { // Copy all selected records to the export database.
          iSts = iCreate_exp_db(pDBA);
        }
        delete pDBA;
      }
      if(iSts != FAIL)
        m_cstrProgress.LoadStringW(IDS_EXPORT_COMPLETE);
      else
        m_cstrProgress.LoadStringW(IDS_EXPORT_ERRORS);
        //Set focus back to Patient list
      m_cPatientList.SetFocus();
    }
    else
    { // Not going to export.
    m_cstrProgress.LoadStringW(IDS_EXPORT_CANCELLED);
    }
    UpdateData(FALSE);
  }
}

/********************************************************************
vCreate_exp_db - Copy all the records for the selected studies from the
                 local database to the export database.

                 For each selected study, copy all data from all tables
                   to the export database.
                 Must update the ID for each table.

  inputs: Pointer to a CDBAccess object for the export database.

  return: None.
********************************************************************/
short int CStudyExportDlg::iCreate_exp_db(CDBAccess *pDBA)
{
  POSITION pos;
  int iItem;
  short int iSts = SUCCESS, iSts1 = SUCCESS;
  long lOrigPatientID, lNewPatientID, lStudyID;
  PATIENT_TABLE PatientTable;
  PATIENT_INFO PatientInfo;

  m_iSelectedPatients = 0;
  m_iSelectedStudies = 0;
  pos = m_cPatientList.GetFirstSelectedItemPosition();
    // Process each selected study from the listbox.
  while (pos != NULL)
  {
    m_cExportProgressBar.StepIt(); // Increment the progress bar.
    iItem = m_cPatientList.GetNextSelectedItem(pos);
      // Get the Study ID from the listbox selection.
    lStudyID = (long)m_cPatientList.GetItemData(iItem);
      // Find Patient ID from the StudyID.
    if((iSts1 = m_pDBA->iGet_patient_id_from_study(lStudyID, &lOrigPatientID)) == SUCCESS)
    {
      PatientTable.lPatientID = lOrigPatientID;
        // Get the patient data from the local databasee.
      if((iSts1 = m_pDBA->iGet_patient(&PatientTable)) == SUCCESS)
      {
        // Look for the patient in the export database.  If already there,
        // skip Patient table and PatientInfo table.
        if((iSts1 = pDBA->iFind_patient(&PatientTable)) == SUCCESS_NO_DATA)
        { // Patient not yet in export database.  Add patient.
          PatientTable.lPatientID = INV_LONG; // For add new patient.
            // Save the patient data in the export file.
          if((iSts = pDBA->iSave_patient(&PatientTable)) == SUCCESS)
          { // Copy the PatientInfo table.
            ++m_iSelectedPatients;
            lNewPatientID = PatientTable.lPatientID;
              // Get the patient info data from the local database.
            if((iSts = m_pDBA->iGet_patient_info(lOrigPatientID, &PatientInfo)) == SUCCESS)
            {  // Save the patient info data to the export database.
              iSts = pDBA->iSave_patient_info(lNewPatientID, &PatientInfo);
            }
            iSts1 = iSts;
          }
        }
      }
      if(iSts == SUCCESS)
        // Process the study and whatif tables.
        iSts1 = iCopy_study_data(pDBA, lStudyID, lNewPatientID);
    }
    if(iSts == FAIL || iSts1 == FAIL)
    { // There were errors in exporting this study.  Ask if user wants to continue
      // with the export.
      CString cstrMsg, cstrTitle;

      cstrTitle.LoadStringW(IDS_ERROR);
      cstrMsg.LoadStringW(IDS_EXPORT_ERRS_CONTINUE);
      int iResp = MessageBox(cstrMsg, cstrTitle, MB_YESNO);
      if(iResp == IDNO)
        break;
    }
    //  break;
  } // end of while
  return(iSts1);
}

/********************************************************************
iCopy_study_data - Copy the study record and all the WhatID records for
                   the specified study from the local database to the
                   export database.

  inputs: Pointer to a CDBAccess object for the export database.
          Study ID in the local database.
          Patient ID in the export database.

  return: SUCCESS if study data, WhatIfs and the data file are all processed.
          FAIL if there is an error.
********************************************************************/
short int CStudyExportDlg::iCopy_study_data(CDBAccess *pDBA, long lStudyID, long lNewPatientID)
{
  STUDY_DATA sdStudy;
  EVENTS_DATA edEvent;
  long lNewStudyID;
  short int iPos, iSts, iSts1;
  int iIndex;
  WHAT_IF wi;
  CString cstrDestFile;

  sdStudy.lStudyID = lStudyID;
    // Get the study data from the local database.
  if((iSts1 = m_pDBA->iGet_study_data(&sdStudy)) == SUCCESS)
  {
    ++m_iSelectedStudies;
    sdStudy.lStudyID = INV_LONG;
    sdStudy.lPatientID = lNewPatientID;
      // Copy the data file.
      // Create the data file name for the destination.
    cstrDestFile = sdStudy.cstrDataFile;
    if((iIndex = cstrDestFile.ReverseFind('\\')) > 0)
      cstrDestFile = cstrDestFile.Right(cstrDestFile.GetLength() - iIndex - 1);
    cstrDestFile.Insert(0, '\\');
    cstrDestFile.Insert(0, m_cstrDestination);
    if(bCopy_file((LPCTSTR)sdStudy.cstrDataFile, (LPCTSTR)cstrDestFile) == false)
      iSts1 = FAIL;
    else
    {// Save the study data in the export database.
      if((iSts1 = pDBA->iSave_study_data(&sdStudy)) == SUCCESS)
      {
        lNewStudyID = sdStudy.lStudyID;
          // Now do the WhatIf table.
        iPos = FIRST_RECORD;
        iSts = SUCCESS;
        wi.lStudyID = lStudyID;
        while(iSts == SUCCESS)
        {  // Get the WhatIf data from one record in the local database.
          if((iSts = m_pDBA->iFind_next_whatif(iPos, &wi)) == SUCCESS)
          {
            wi.lStudyID = lNewStudyID;
            wi.lWhatIfID = INV_LONG;
              // Save the WhatIf data to the export database.
            iSts = pDBA->iSave_whatif_data(&wi);
          }
          iSts1 = iSts;
          if(iSts1 == FAIL)
            break;
          iPos = NEXT_RECORD;
        }
        if(iSts1 != FAIL)
        {
           // Now copy the events.
          iSts1 = SUCCESS;
          edEvent.lStudyID = lStudyID;
          while(iSts1 == SUCCESS)
          {  // Get an event from the local database.
            if((iSts1 = m_pDBA->iGet_event(&edEvent)) == SUCCESS)
            {
              edEvent.lStudyID = lNewStudyID;
                // Save the Event in the export database.
              iSts1 = pDBA->iSave_event(&edEvent);
            }
            edEvent.lStudyID = INV_LONG; // Set up to get next event.
          }
        }
      }
    }
  }
  return(iSts1);
}

/********************************************************************
bCopy_file - Copy a file from the source to the destination.

  inputs: String containing the source file and path.
          String containing the destination file and path.

  return: TRUE if the file was successfully copied.
          FALSE if there was an error.
********************************************************************/
BOOL CStudyExportDlg::bCopy_file(LPCTSTR strSrc, LPCTSTR strDest)
{
  BOOL bRet;

  if((bRet = CopyFile(strSrc, strDest, FALSE)) == FALSE)
  { // Failed copying file.
    DWORD dwErr;
    wchar_t szMsg[256];
    CString cstrErr, cstrTitle;
  
    dwErr = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, 0L, szMsg, 256, NULL);
    cstrErr.Format(IDS_FAIL_COPY_FILE, strSrc, strDest);
    cstrErr += _T("\r\n");
    cstrErr += szMsg;
    cstrTitle.LoadStringW(IDS_ERROR);
    MessageBox((LPCTSTR)cstrErr, cstrTitle);
    theApp.vLog((LPCTSTR)cstrErr);
  }
    // This function changes the current directory, set it back.
  SetCurrentDirectory(m_szCurDir);
  return(bRet);
}

/********************************************************************
bFileExists - Determine if a file exists.

  inputs: String containing the file and path.

  return: true if the file exists.
          false if the file doesn't exist.
********************************************************************/
bool CStudyExportDlg::bFileExists(CString cstrFile)
{
  HANDLE hFile;
  bool bRet;

  hFile = CreateFile(cstrFile, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if(hFile != INVALID_HANDLE_VALUE)
  {
    bRet = true;
    CloseHandle(hFile);
  }
  else
    bRet = false;
  return(bRet);
}

/********************************************************************
vSet_font - Create a font using "Arial" for a specific window or CDC.

  inputs: Pointer to a window. If the CDC pointer is not NULL, this
            pointer is ignored.
          Pointer to a CDC object. If this is NULL, the window pointer
            is used to get the CDC object.
          Pointer to a pointer to a CFont object which gets created font.
          Weight of the Font.
          Font size. Default is 0. If 0, the character height is used
            as the size.

  return: None.
********************************************************************/
void CStudyExportDlg::vSet_font(CWnd *pwnd, CDC *pDC, CFont *pcfFont, long lFontWeight,
long lFontSize)
  {
  CDC *pDCUse = NULL;
  TEXTMETRIC tm;
  LOGFONT lfLF;

  if(pDC == NULL)
    pDCUse = pwnd->GetDC();
  else
    pDCUse = pDC;
  pDCUse->GetTextMetrics(&tm);
  memset(&lfLF, 0, sizeof(LOGFONT)); // zero out structure
    // Set the size of the font depending on the last input.
  if(lFontSize != 0)
    lfLF.lfHeight = lCalc_font_height(lFontSize, pDCUse); // Calculate new font size.
  else
    lfLF.lfHeight = tm.tmHeight; // Use the standard font size.

  //strcpy(lfLF.lfFaceName, "Arial");  // Font name is "Arial".
  wcscpy_s(lfLF.lfFaceName, LF_FACESIZE, _T("Arial"));  // Font name is "Arial".

  lfLF.lfWeight = lFontWeight; // Set to weight of font.
  pcfFont->CreateFontIndirect(&lfLF); // Create the font.
  if(pDC == NULL)
    pwnd->ReleaseDC(pDCUse);
  }

/********************************************************************
lCalc_font_height - Calculate the font height.
                    Formula: 
                      (Point size * Num Pixels per logical inch along Y) / 72.
                         
  inputs: Point size.
          Pointer to the CDC object for the printer.

  return: Height of the font in pixels.
********************************************************************/
long CStudyExportDlg::lCalc_font_height(int iPointSize, CDC *pcdc)
  {
  return(MulDiv(iPointSize, pcdc->GetDeviceCaps(LOGPIXELSY), 72));
  }


void CStudyExportDlg::OnBnClickedBtnSearchDb()
{
	// TODO: Add your control notification handler code here
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
      m_pDBA->m_cstrSearchSQL = pdlg->m_cstrSQL;
      m_pDBA->m_iDateSrchType = pdlg->m_iDateSrchType;
      m_pDBA->m_cstrSrchDate = pdlg->m_cstrDate;
      if(pdlg->m_iDateSrchType != DB_SRCH_NONE && pdlg->m_cstrDate.Find(_T("*")) < 0)
			{
      	CW2A pszDay(pdlg->m_cstrDay);
      	CW2A pszMonth(pdlg->m_cstrMonth);
      	CW2A pszYear(pdlg->m_cstrYear);
        m_pDBA->m_ctSrchDate = CTime(atoi(pszYear), atoi(pszMonth), atoi(pszDay), 0, 0, 0);
			}
		  m_cPatientList.DeleteAllItems();
      vFill_patient_listbox();
      m_pDBA->m_cstrSearchSQL.Empty();
      m_pDBA->m_iDateSrchType = 0;
      }
    delete pdlg;
}

/********************************************************************
pdbNewDB - Create a new database object.
           A check is made to see if using a remote database.
                         
  inputs: CString object containing the file name.

  return: true if the file is valid.
          false if no data points, the file header is invalid or there
            was a file error.
********************************************************************/
CDBAccess *CStudyExportDlg::pdbNewDB()
  {
  CDBAccess *pDB;
  wchar_t szDB[2048];
	CString cstrDBPath;

  // The DEFAULT database may not exist if user is using a database on a remote
  // computer.  So we have to get the remote database path from the EGGSAS.INI file.
  // If it is empty then we use the default.
  //cstrRemoteDBPath = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_PATH, "");
  GetPrivateProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_PATH, _T(""), szDB, 2048, INI_FILE_EGGSAS);
  if(szDB[0] == NULL)
    {
    GetPrivateProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_PATH, _T(""), szDB, 2048, INI_FILE_CDDR);
    if(szDB[0] == NULL)
      GetPrivateProfileString(INI_SEC_GENERAL, INI_ITEM_REMOTE_DB_PATH, _T(""), szDB, 2048, INI_FILE_RDR);
    }
  if(szDB[0] == NULL)
    pDB = new CDBAccess(LOCAL_DSN_DEFAULT);
  else
    {
    pDB = new CDBAccess(szDB);
    //m_bRemoteDB = true;
		// Set the data file path in the settings class.
		m_pSett->vWrite_string_item(SETT_ITEM_DATA_PATH, szDB);
    }
  return(pDB);
  }