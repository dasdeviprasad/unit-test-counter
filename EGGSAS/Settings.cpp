/********************************************************************
Settings.cpp

Copyright (C) 2012,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CSettings class.

  This class provides the user to set the settings for the program.
    Settings:
     - Select a language
     - Select a location for the data files, *.egg. - REMOVED
     - Select a path for the remote database.

---------------------------------------------------
	This describes how the password used with the remote database is encoded into a double GUID.
          1  1    1 2  2      3    33   4    4    5    5    6         7 7
01234567890  3    8 0  3      0    56   0    5    0    5    0         0 2
12979280-16d8-4e74-97d3-837d540f8021-c295a5e8-c488-4792-ac8c-17846e422161


                    byte 1   byte 2
Password: 
	password length     21       7
	char 1              4        1
	char 2              10       32
	char 3              31       19
	char 4              38       11
	char 5              34       2
	char 6              48       6
	char 7              17       29
	char 8              24       15
	char 9              33       61
	char 10             12       56
	char 11             70       25
	char 12             20       41
	char 13             5        27
	char 14             16       30
	char 15             9        35
---------------------------------------------------


HISTORY:
23-FEB-12  RET  New
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedSettingsHelp()
14-NOV-20  RET
             Add ability to work with a remote database.
             Changed settings to allow the user to specify the remote database.
               Change the dialog box to remove specifying a path for data files
                 and added fields to enter a password and select a remote database.
               Change methods: OnInitDialog(), OnBnClickedDataFileLocBrowse(), BrowseCtrlCallback(),
                 vSet_text(), OnBnClickedOk()
               Add methods: OnBnClickedRemoteDbLocBrowse()
               Add static variables: s_iPasswdLenByt1, s_iPasswdLenByt2, s_iPasswdIndexByt1[],
                 s_iPasswdIndexByt2[]
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "Dbaccess.h"
#include "Settings.h"
#include ".\settings.h"
#include "util.h"

static int s_iPasswdLenByt1 = 21;
static int s_iPasswdLenByt2 = 7;
static int s_iPasswdIndexByt1[15] = 
       {4, 10, 31, 38, 34, 48, 17, 24, 33, 12, 70, 20, 5, 16, 9};
static int s_iPasswdIndexByt2[15] = 
       {1, 32, 19, 11, 2, 6, 29, 15, 61, 56, 25, 41, 27, 30, 35};

// CSettings dialog

IMPLEMENT_DYNAMIC(CSettings, CDialog)

/********************************************************************
Constructor/destructor
********************************************************************/
CSettings::CSettings(CEGGSASDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CSettings::IDD, pParent)
  , m_cstrDataFileLoc(_T(""))
  , m_cstrRemoteDBPasswd(_T(""))
  , m_cstrRemoteDBPath(_T(""))
  {

  m_bSetDataFilePath = false;
  m_pDoc = pDoc;
}

CSettings::~CSettings()
{
}

void CSettings::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Control(pDX, IDC_COMBO_LANGUAGE, m_cLanguageList);
DDX_Text(pDX, IDC_EDIT_DATA_FILE_LOC, m_cstrDataFileLoc);
DDX_Text(pDX, IDC_EDIT_REMOTE_DB_PASSWD, m_cstrRemoteDBPasswd);
DDX_Text(pDX, IDC_EDIT_REMOTE_DB_LOC, m_cstrRemoteDBPath);
}


BEGIN_MESSAGE_MAP(CSettings, CDialog)
  ON_BN_CLICKED(IDC_DATA_FILE_LOC_BROWSE, OnBnClickedDataFileLocBrowse)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDC_SETTINGS_HELP, OnBnClickedSettingsHelp)
  ON_BN_CLICKED(IDC_REMOTE_DB_LOC_BROWSE, OnBnClickedRemoteDbLocBrowse)
END_MESSAGE_MAP()


// CSettings message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Fill in the language combobox with the available languages.
    Display the path for data files.
********************************************************************/
BOOL CSettings::OnInitDialog()
  {
  CString cstrLanguage;

  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  vSet_text();
  m_bSetRemoteDBPath = false;

  g_pLang->vGet_languages();
  cstrLanguage = g_pLang->cstrGet_first_language();
  while(cstrLanguage != "")
    {
    m_cLanguageList.AddString(cstrLanguage);
    cstrLanguage = g_pLang->cstrGet_next_language();
    }
  // Select the currently selected language.
  cstrLanguage = g_pLang->cstrGet_language();
  m_cLanguageList.SelectString(0, cstrLanguage);

  //m_cstrDataFileLoc = cstrGet_data_file_path();
  m_cstrRemoteDBPath = g_pConfig->cstrGet_item(CFG_REMOTE_DB_PATH);
  if(m_cstrRemoteDBPath.IsEmpty() == true)
    {
    GetDlgItem(IDC_STATIC_REMOTE_DB_PASSWD)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_REMOTE_DB_PASSWD)->EnableWindow(FALSE);
    GetDlgItem(IDC_STATIC_REMOTE_DB_LOC)->EnableWindow(FALSE);
    GetDlgItem(IDC_REMOTE_DB_LOC_BROWSE)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT_REMOTE_DB_LOC)->EnableWindow(FALSE);
    }

  UpdateData(FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnBnClickedDataFileLocBrowse - Handler for the browse button.

    Display a select folder dialog box using Window's shell extension.
    Defines are in shlobj.h

Password encoding
  Each character in password results in two characters to be encoded.
    Character converted to hex number. (Ascii to hex).
    Decimal number converted to hex.
    first index is high byte, second index is low byte.
    //Number / 10 and number
 "04E101D6972539-05673AC2376-0480B44E6A-0549A54B902-39098-B510B4D52070-2308ED"


********************************************************************/
void CSettings::OnBnClickedDataFileLocBrowse()
  {
  //CString cstrDB;

  //  // Display a browse window for selecting location of database.
  //cstrDB = g_pConfig->cstrGet_item(CFG_REMOTE_DB_PATH);
  //if(cstrDB.IsEmpty() == FALSE)
  //  {
  //  // Ask for password.
  //  cstrDB = m_pDoc->cstrSelect_reader_db();
  //  if(cstrDB.IsEmpty() == FALSE)
  //    {
  //    m_pDoc->vChange_db(IMPORT_DB, cstrDB);
  //    m_cstrDataFileLoc = cstrDB;
  //    }
  //  }
  //else
  //  {
  //  MessageBox(g_pLang->cstrLoad_string(ITEM_G_REQ_ACTIVATION),
  //    g_pLang->cstrLoad_string(ITEM_G_INFORMATION));
  //  }
  //UpdateData(FALSE);

	//LPMALLOC		pMalloc;
	//BROWSEINFO		bi;
	//LPITEMIDLIST	pidl;
	//TCHAR szSelectedFolder[MAX_PATH];
 // CString cstrPath, cstrText;

 // // TODO: Add your control notification handler code here
 // // Display a Browse for file dialog box so user can select location.
 //   // Get the path to start the Open File dialog box with.  If there is no last
 //   // path the user selected, use the data path.
 // if(m_bSetDataFilePath == true)
 //   cstrPath = m_cstrDataFileLoc;
 // else
 //   {
 //   cstrPath = cstrGet_data_file_path();
 //   m_cstrDataFilePath = cstrPath;
 //   }

 // ::ZeroMemory(&bi, sizeof(bi));
	//// Gets the Shell's default allocator
	//if (::SHGetMalloc(&pMalloc) == NOERROR)
	//  {
	//	// Get help on BROWSEINFO struct - it's got all the bit settings.
 //   bi.hwndOwner = this->m_hWnd;
	//	bi.pidlRoot = NULL;
	//	bi.pszDisplayName = (LPSTR)(LPCTSTR)cstrPath;
 //   cstrText = g_pLang->cstrLoad_string(ITEM_G_SEL_DATA_PATH);
	//	bi.lpszTitle = cstrText;
	//	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS | 0x40; // | BIF_STATUSTEXT; //
	//	bi.lpfn = BrowseCtrlCallback;
	//	bi.lParam = (LPARAM)(LPCTSTR)cstrPath;
	//	// This next call issues the dialog box.
	//	if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
	//	  {
	//		if (::SHGetPathFromIDList(pidl, szSelectedFolder))
 //       {
 //       // User selected a folder.  Save it in the config file.
 //       // Set a flag so that when the user selects OK, we know to 
 //       // save the new data file path.
 //       m_bSetDataFilePath = true;
 //       m_cstrDataFileLoc = szSelectedFolder;
 //       }
	//		pMalloc->Free(pidl);	// Free the PIDL allocated by SHBrowseForFolder.
	//	  }
	//	pMalloc->Release(); // Release the shell's allocator.
	//  }
 // UpdateData(FALSE);
  }

/********************************************************************
BrowseCtrlCallback - Callback function for the select folder dialog box.

    Display a select folder dialog box using Window's shell extension.
    Defines are in shlobj.h

********************************************************************/
//int __stdcall CSettings::BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
// {
//
//	if (uMsg == BFFM_INITIALIZED && lpData != NULL)
//	{
//		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
//	}
//	else // uMsg == BFFM_SELCHANGED
//	{
//	}
//	return 0;
// }


/********************************************************************
OnBnClickedOk - Handler for the OK button.

    Save the setting in the INI file.

********************************************************************/
void CSettings::OnBnClickedOk()
  {
  CString cstrLanguage;

  // TODO: Add your control notification handler code here
  // Save the new data file path if the user selected a new one.
  if(m_bSetDataFilePath == true)
    g_pConfig->vWrite_item(CFG_DATA_FILE_STORAGE, m_cstrDataFileLoc);
  // Save the currently selected language.
  m_cLanguageList.GetLBText(m_cLanguageList.GetCurSel(), cstrLanguage);
  g_pLang->vSet_language(cstrLanguage);

  if(m_bSetRemoteDBPath == true)
    { // Save the new path to the remote database.
    // Write the new path back to the INI file.
    g_pConfig->vWrite_item(CFG_REMOTE_DB_PATH, m_cstrRemoteDBPath);
    // Set the new path.
    m_pDoc->vChange_db(IMPORT_DB, m_cstrRemoteDBPath);
    }

  OnOK();
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CSettings::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_SETTINGS);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_SET_SETTINGS));
    GetDlgItem(IDC_STATIC_SEL_LANGUAGE)->SetWindowText(g_pLang->cstrGet_text(ITEM_SET_SEL_LANG));
    //GetDlgItem(IDC_STATIC_DATA_FILE_LOC)->SetWindowText(g_pLang->cstrGet_text(ITEM_SET_SEL_LOC_FOR_DATA_FILES));
    //GetDlgItem(IDC_STATIC_DATA_FILE_LOC)->SetWindowText(g_pLang->cstrGet_text(ITEM_SET_SEL_LOC_FOR_DB));
    //GetDlgItem(IDC_DATA_FILE_LOC_BROWSE)->SetWindowText(g_pLang->cstrGet_text(ITEM_SET_BROWSE));
    GetDlgItem(IDC_STATIC_REMOTE_DB_PASSWD)->SetWindowText(g_pLang->cstrGet_text(ITEM_SET_ENTER_REMOTE_DB_PASSWD));
    GetDlgItem(IDC_STATIC_REMOTE_DB_LOC)->SetWindowText(g_pLang->cstrGet_text(ITEM_SET_SEL_REMOTE_DB_PATH));
    GetDlgItem(IDC_REMOTE_DB_LOC_BROWSE)->SetWindowText(g_pLang->cstrGet_text(ITEM_SET_BROWSE));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_SETTINGS_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    }
  }

/********************************************************************
cstrGet_data_file_path - Get the data file path from the config settings.
                         If none, default it to he config setting for the data path.

  inputs: None.

  return: CString object containing the data file path.
********************************************************************/
//CString CSettings::cstrGet_data_file_path(void)
//  {
//  CString cstrPath;
//  //CString cstrRemoteDBPath;
//
//
//  cstrPath = g_pConfig->cstrGet_item(CFG_REMOTE_DB_PATH);
//  if(cstrPath.IsEmpty() == TRUE)
//    cstrPath = m_pDoc->m_pDB->cstrGet_db_path();
//
//  //cstrPath = g_pConfig->cstrGet_item(CFG_DATA_FILE_STORAGE);
//  //if(cstrPath.IsEmpty() == TRUE)
//  //  cstrPath = g_pConfig->cstrGet_item(CFG_DATAPATH);
//  return (cstrPath);
//  }

/********************************************************************
OnBnClickedSettingsHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CSettings::OnBnClickedSettingsHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_SETTINGS_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_SETTINGS_HELP);
  }

/********************************************************************
OnBnClickedRemoteDbLocBrowse - Handler for the "Browse" button for selecting
                               a remote database.

   Make sure the password is valid.
   Display a file select dialog box.
********************************************************************/
void CSettings::OnBnClickedRemoteDbLocBrowse()
  {
  int iLenPasswd;
  CString cstrPasswd, cstrEncodedPasswd;
  char cTemp, szTemp[2];
  bool bContinue = false;

  UpdateData(TRUE);
  // Check that the password is valid.
  // Decode the  password.
  if(m_cstrRemoteDBPasswd.IsEmpty() == true)
    {
    g_pLang->vStart_section(SEC_HIDE_STUDY_DATE);
    MessageBox(g_pLang->cstrGet_text(ITEM_MUST_ENTER_PSWD),  g_pLang->cstrLoad_string(ITEM_G_INFORMATION));
    g_pLang->vEnd_section();
    }
  else
    {
    szTemp[1] = NULL; 
    cstrPasswd.Empty();
    cstrEncodedPasswd = g_pConfig->cstrGet_item(CFG_REMOTE_DB_PASSWD);
    cTemp = cDecode_char_from_string(cstrEncodedPasswd, s_iPasswdLenByt1, s_iPasswdLenByt2);
    iLenPasswd = ucAscii_to_Hex(cTemp);
    for(int i = 0; i < iLenPasswd; ++i)
      {
      szTemp[0] = cDecode_char_from_string(cstrEncodedPasswd, s_iPasswdIndexByt1[i], s_iPasswdIndexByt2[i]);
      cstrPasswd += szTemp;
      }
    if(cstrPasswd == m_cstrRemoteDBPasswd)
      bContinue = true;
    else
      {
      // Look at the default password.
      cstrPasswd.Empty();
      cstrEncodedPasswd = g_pConfig->cstrGet_item(CFG_REMOTE_DB_DEF_PASSWD);
      cTemp = cDecode_char_from_string(cstrEncodedPasswd, s_iPasswdLenByt1, s_iPasswdLenByt2);
      iLenPasswd = ucAscii_to_Hex(cTemp);
      for(int i = 0; i < iLenPasswd; ++i)
        {
        szTemp[0] = cDecode_char_from_string(cstrEncodedPasswd, s_iPasswdIndexByt1[i], s_iPasswdIndexByt2[i]);
        cstrPasswd += szTemp;
        }
      if(cstrPasswd == m_cstrRemoteDBPasswd)
        bContinue = true;
      else
        {
        g_pLang->vStart_section(SEC_HIDE_STUDY_DATE);
        MessageBox(g_pLang->cstrGet_text(ITEM_PSWD_NOT_CORRECT),  g_pLang->cstrLoad_string(ITEM_G_ERROR));
        g_pLang->vEnd_section();
        }
      }
    if(bContinue == true)
      {
      m_cstrRemoteDBPath = m_pDoc->cstrSelect_reader_db("3CPM EGGSAS.mdb");
      if(m_cstrRemoteDBPath.IsEmpty() == FALSE)
        {
        m_bSetRemoteDBPath = true; // Set flag to the Close button will save the new remote database.
        }
      }
    }
  UpdateData(FALSE);
  }
