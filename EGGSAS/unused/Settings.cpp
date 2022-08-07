/********************************************************************
Settings.cpp

Copyright (C) 2012, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CSettings class.

  This class provides the user to set the settings for the program.
    Settings:
     - Select a language
     - Select a location for the data files, *.egg.


HISTORY:
23-FEB-12  RET  New
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
//#include "FolderDialog.h"
#include "Settings.h"
#include ".\settings.h"


// CSettings dialog

IMPLEMENT_DYNAMIC(CSettings, CDialog)

/********************************************************************
Constructor/destructor
********************************************************************/
CSettings::CSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CSettings::IDD, pParent)
  , m_cstrDataFileLoc(_T(""))
  {

  m_bSetDataFilePath = false;
}

CSettings::~CSettings()
{
}

void CSettings::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Control(pDX, IDC_COMBO_LANGUAGE, m_cLanguageList);
DDX_Text(pDX, IDC_EDIT_DATA_FILE_LOC, m_cstrDataFileLoc);
}


BEGIN_MESSAGE_MAP(CSettings, CDialog)
  ON_BN_CLICKED(IDC_DATA_FILE_LOC_BROWSE, OnBnClickedDataFileLocBrowse)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
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

  m_cstrDataFileLoc = cstrGet_data_file_path();

  UpdateData(FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnBnClickedDataFileLocBrowse - Handler for the browse button.

    Display a select folder dialog box using Window's shell extension.
    Defines are in shlobj.h

********************************************************************/
void CSettings::OnBnClickedDataFileLocBrowse()
  {
  CString cstrPath, cstrText;

  // TODO: Add your control notification handler code here
  // Display a Browse for file dialog box so user can select location.
    // Get the path to start the Open File dialog box with.  If there is no last
    // path the user selected, use the data path.
  if(m_bSetDataFilePath == true)
    cstrPath = m_cstrDataFileLoc;
  else
    {
    cstrPath = cstrGet_data_file_path();
    //cstrPath = g_pConfig->cstrGet_item(CFG_DATA_FILE_STORAGE);
    m_cstrDataFilePath = cstrPath;
    //if(cstrPath.IsEmpty() == TRUE)
    //  cstrPath = g_pConfig->cstrGet_item(CFG_DATAPATH);
    }

  //CFileDialog *pdlg;
  //pdlg = new CFileDialog(FALSE);
  //pdlg->m_pOFN->lpfnHook = OFNHookProcOldStyle;
  //pdlg->m_pOFN->Flags = OFN_ENABLEHOOK | OFN_EXPLORER;
  //pdlg->DoModal();

  //CFolderSelDlg *pdlg = new CFolderSelDlg(TRUE, NULL, NULL, 0, NULL, NULL);
  //pdlg->DoModal();

	//CFolderDialog dlg("Caption", "Title", cstrPath);
 // dlg.vSet_language(g_pLang);
	//if(dlg.DoModal()==IDOK)
	//{
	//	m_cstrDataFileLoc = dlg.GetPathName();
 //   m_bSetDataFilePath = true;
	//}

	LPMALLOC		pMalloc;
	BROWSEINFO		bi;
	LPITEMIDLIST	pidl;
	TCHAR szSelectedFolder[MAX_PATH];
  ::ZeroMemory(&bi, sizeof(bi));
	// Gets the Shell's default allocator
	if (::SHGetMalloc(&pMalloc) == NOERROR)
	  {
		// Get help on BROWSEINFO struct - it's got all the bit settings.
    bi.hwndOwner = this->m_hWnd;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = (LPSTR)(LPCTSTR)cstrPath;
    cstrText = g_pLang->cstrLoad_string(ITEM_G_SEL_DATA_PATH);
		bi.lpszTitle = cstrText;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS | 0x40; // | BIF_STATUSTEXT; //
		bi.lpfn = BrowseCtrlCallback;
		bi.lParam = (LPARAM)(LPCTSTR)cstrPath;
		// This next call issues the dialog box.
		if ((pidl = ::SHBrowseForFolder(&bi)) != NULL)
		  {
			if (::SHGetPathFromIDList(pidl, szSelectedFolder))
        {
        // User selected a folder.  Save it in the config file.
        // Set a flag so that when the user selects OK, we know to 
        // save the new data file path.
        m_bSetDataFilePath = true;
        m_cstrDataFileLoc = szSelectedFolder;
        }
			pMalloc->Free(pidl);	// Free the PIDL allocated by SHBrowseForFolder.
		  }
		pMalloc->Release(); // Release the shell's allocator.
	  }
  UpdateData(FALSE);
  }

/********************************************************************
BrowseCtrlCallback - Callback function for the select folder dialog box.

    Display a select folder dialog box using Window's shell extension.
    Defines are in shlobj.h

********************************************************************/
int __stdcall CSettings::BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED && lpData != NULL)
	{
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	else // uMsg == BFFM_SELCHANGED
	{
	}

	return 0;
} // End of BrowseCtrlCallback

  //CString g_cstrDataFilePath = ""; 
//bool g_bChg = false;

//UINT_PTR CALLBACK OFNHookProcOldStyle(          HWND hdlg,
//    UINT uiMsg,
//    WPARAM wParam,
//    LPARAM lParam
//)
//  {
//  if(uiMsg == WM_COMMAND)
//    {
//    if(wParam == CDN_FOLDERCHANGE)
//      g_bChg = true;
//    //g_cstrDataFilePath = "";
//    }
//  else if(uiMsg == WM_NOTIFY)
//    {
//    if(wParam == CDN_FOLDERCHANGE)
//      g_bChg = true;
//    else if(wParam == CDN_FILEOK)
//      g_bChg = true;
//    }
//  return(0);
//  }


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
    GetDlgItem(IDC_STATIC_DATA_FILE_LOC)->SetWindowText(g_pLang->cstrGet_text(ITEM_SET_SEL_LOC_FOR_DATA_FILES));
    GetDlgItem(IDC_DATA_FILE_LOC_BROWSE)->SetWindowText(g_pLang->cstrGet_text(ITEM_SET_BROWSE));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_SETTINGS_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    //g_pLang->vEnd_section();
    }
  }

/********************************************************************
cstrGet_data_file_path - Get the data file path from the config settings.
                         If none, default it to he config setting for the data path.

  inputs: None.

  return: CString object containing the data file path.
********************************************************************/
CString CSettings::cstrGet_data_file_path(void)
  {
  CString cstrPath;

  cstrPath = g_pConfig->cstrGet_item(CFG_DATA_FILE_STORAGE);
  if(cstrPath.IsEmpty() == TRUE)
    cstrPath = g_pConfig->cstrGet_item(CFG_DATAPATH);
  return (cstrPath);
  }
