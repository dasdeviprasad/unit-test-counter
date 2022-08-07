//////////////////////////////////////////////////////////////////////////
//PathDialog.h file
//
//Written by Nguyen Tan Hung <tanhung@yahoo.com>
//////////////////////////////////////////////////////////////////////////

/***********
TheFolder=The folder:
FolderDoesNotExist=does not exist. Do you want the folder to be created?
InvalidPath=is invalid. Please reenter.
ErrCreatePath=can not be created. Please double check.

#define ITEM_G_FOLDER_DOES_NOT_EXIST "FolderDoesNotExist"
#define ITEM_G_INVALID_PATH "InvalidPath"
#define ITEM_G_ERR_CREATE_PATH "ErrCreatePath"
#define ITEM_G_THE_FOLDER "TheFolder"
*************/


#include "stdafx.h"
#include "Language.h"
#include "FolderDialog.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_FOLDERTREE		0x3741
#define IDC_TITLE			0x3742
#define IDC_STATUSTEXT		0x3743

#define IDC_NEW_EDIT_PATH	0x3744

const TCHAR c_FolderDoesNotExist[] = _T(
		"The folder:\n"
		"    %s\n"
		"does not exist. Do you want the folder to be created?");
const TCHAR c_szErrInvalidPath[] = _T(
		"The folder:"
		"\n"
		"    %s\n"
		"is invalid. Please reenter.");
const TCHAR c_szErrCreatePath[] = _T(
		"The folder:"
		"\n"
		"    %s\n"
		"can not be created. Please double check.");


static char s_szErrMsg[1024];
static CLanguage *s_pLang = NULL;

// Class CDlgWnd
BEGIN_MESSAGE_MAP(CFolderDialogSub, CWnd)
        ON_BN_CLICKED(IDOK, OnOK)
		ON_EN_CHANGE(IDC_NEW_EDIT_PATH, OnChangeEditPath)
END_MESSAGE_MAP()

void CFolderDialogSub::OnOK()
{
	::GetWindowText(::GetDlgItem(m_hWnd, IDC_NEW_EDIT_PATH),
		m_pFolderDialog->m_szPathName, MAX_PATH);

	if(CFolderDialog::MakeSurePathExists(m_pFolderDialog->m_szPathName)==0)
	{
		m_pFolderDialog->m_bGetSuccess=TRUE;
		::EndDialog(m_pFolderDialog->m_hWnd, IDOK);
	}
	else
	{
		::SetFocus(::GetDlgItem(m_hWnd, IDC_NEW_EDIT_PATH));
	}
}

void CFolderDialogSub::OnChangeEditPath()
{
	::GetWindowText(::GetDlgItem(m_hWnd, IDC_NEW_EDIT_PATH),
		m_pFolderDialog->m_szPathName, MAX_PATH);
	BOOL bEnableOKButton = (_tcslen(m_pFolderDialog->m_szPathName)>0);
	SendMessage(BFFM_ENABLEOK, 0, bEnableOKButton);
}
/////////////////////////////////////////////////////////////////////////////
// CPathDialog dialog


CFolderDialog::CFolderDialog(LPCTSTR lpszCaption, 
						 LPCTSTR lpszTitle, 
						 LPCTSTR lpszInitialPath, 
						 CWnd* pParent)
{
	m_hWnd=NULL;
  s_szErrMsg[0] = NULL;
	m_FolderDialogSub.m_pFolderDialog = this;
    m_bParentDisabled = FALSE;

	  _tcscpy(m_szPathName, lpszInitialPath);

    // Get the true parent of the dialog
    m_pParentWnd = CWnd::GetSafeOwner(pParent);

	m_lpszCaption = lpszCaption;
	m_lpszInitialPath = lpszInitialPath;

	memset(&m_bi, 0, sizeof(BROWSEINFO) );
	m_bi.hwndOwner = (m_pParentWnd==NULL)?NULL:m_pParentWnd->GetSafeHwnd();
	//m_bi.pszDisplayName = 0;
  m_bi.pszDisplayName = (LPSTR)lpszInitialPath;
  m_bi.lParam = (LPARAM)lpszInitialPath;
	m_bi.pidlRoot = 0;
	m_bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_RETURNFSANCESTORS
    | BIF_NEWDIALOGSTYLE; // | BIF_NONEWFOLDERBUTTON; // | 0x40;
	m_bi.lpfn = BrowseCallbackProc;
	m_bi.lpszTitle = lpszTitle;
}


/////////////////////////////////////////////////////////////////////////////
// CPathDialog message handlers

CString CFolderDialog::GetPathName()
{
	return CString(m_szPathName);
}

int CALLBACK CFolderDialog::BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam, LPARAM pData) 
{
	CFolderDialog* pDlg = (CFolderDialog*)pData;

	switch(uMsg) 
	{
	case BFFM_INITIALIZED: 
		{
			RECT rc;
			HWND hEdit;
			HFONT hFont;

			pDlg->m_hWnd = hwnd;

			if(pDlg->m_lpszCaption!=NULL)
			{
				::SetWindowText(hwnd, pDlg->m_lpszCaption);
			}

			VERIFY(pDlg->m_FolderDialogSub.SubclassWindow(hwnd));
			::ShowWindow(::GetDlgItem(hwnd, IDC_STATUSTEXT), SW_HIDE);
			::GetWindowRect(::GetDlgItem(hwnd, IDC_FOLDERTREE), &rc);
			rc.bottom = rc.top - 4;
			rc.top = rc.bottom - 23;
			::ScreenToClient(hwnd, (LPPOINT)&rc);
			::ScreenToClient(hwnd, ((LPPOINT)&rc)+1);
			hEdit = ::CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""),
				WS_CHILD|WS_TABSTOP|WS_VISIBLE|ES_AUTOHSCROLL,
				rc.left, rc.top, 
				rc.right-rc.left, rc.bottom-rc.top, 
				hwnd, NULL, NULL, NULL);
			::SetWindowLong(hEdit, GWL_ID, IDC_NEW_EDIT_PATH);
			::ShowWindow(hEdit, SW_SHOW);

			hFont = (HFONT)::SendMessage(hwnd, WM_GETFONT, 0, 0);
			::SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

			LPCTSTR lpszPath = pDlg->m_lpszInitialPath;
			TCHAR szTemp[MAX_PATH];
			if(lpszPath==NULL)
			{
				::GetCurrentDirectory(MAX_PATH, szTemp );
				lpszPath = szTemp;
			}
			// WParam is TRUE since you are passing a path.
			// It would be FALSE if you were passing a pidl.
			::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,
				(LPARAM)lpszPath);
			break;
		}
	case BFFM_SELCHANGED:
		{
			char szSelection[MAX_PATH];
			//if(!::SHGetPathFromIDList((LPITEMIDLIST)lParam, szSelection) ||
			//	szSelection[1]!=':')
			//{
			//	szSelection[0] = '\0';
			//	::SendMessage(hwnd, BFFM_ENABLEOK, 0, FALSE);
			//}
			//else
			//{
			//	::SendMessage(hwnd, BFFM_ENABLEOK, 0, TRUE);
			//}
			if(!::SHGetPathFromIDList((LPITEMIDLIST)lParam, szSelection))
			{
				szSelection[0] = '\0';
				::SendMessage(hwnd, BFFM_ENABLEOK, 0, FALSE);
			}
			else
			{
				::SendMessage(hwnd, BFFM_ENABLEOK, 0, TRUE);
        //strcpy(pDlg->m_szPathName, szSelection);
			}
			//if(!::SHGetPathFromIDList((LPITEMIDLIST)lParam, szSelection))
			//{
			//	//szSelection[0] = '\0';
			//	//::SendMessage(hwnd, BFFM_ENABLEOK, 0, FALSE);
   //     strcpy(szSelection, pDlg->m_szPathName);
			//	::SendMessage(hwnd, BFFM_ENABLEOK, 0, TRUE);
			//}
			//else
			//{
			//	::SendMessage(hwnd, BFFM_ENABLEOK, 0, TRUE);
			//}
			::SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szSelection);
			::SetWindowText(::GetDlgItem(hwnd, IDC_NEW_EDIT_PATH), szSelection);
			break;
		}
	default:
		break;
	}
	return 0;
}



int CFolderDialog::DoModal() 
{

	/////////////////////////////////////////////////////////
	TCHAR szPathTemp[MAX_PATH];
  TCHAR szSelectedFolder[MAX_PATH];

    m_bi.lpfn = BrowseCallbackProc;  // address of callback function
    m_bi.lParam = (LPARAM)this;      // pass address of object to callback function
	m_bi.pszDisplayName = szPathTemp;

	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;

	int iResult=-1;
	if(SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		//m_bGetSuccess = FALSE;
    pidl = ::SHBrowseForFolder(&m_bi);
		if(m_bGetSuccess)
		{
			iResult = IDOK;
        ::SHGetPathFromIDList(pidl, szSelectedFolder);
        strcpy(m_szPathName, szSelectedFolder);
		}
		if (pidl!=NULL) 
      {
  		if(m_bGetSuccess)
        {
        ::SHGetPathFromIDList(pidl, szSelectedFolder);
        strcpy(m_szPathName, szSelectedFolder);
        }
			pMalloc->Free(pidl);
      }
		pMalloc->Release();
	}

    if(m_bParentDisabled && (m_pParentWnd!=NULL))
	{
		m_pParentWnd->EnableWindow(TRUE);
	}
    m_bParentDisabled=FALSE;

	return iResult;
}

//BOOL CFolderDialog::IsFileNameValid(LPCTSTR lpFileName)
//  {
//  BOOL bRet = TRUE;
//  DWORD dwSts, dwErr;
//
//	if(lpFileName == NULL)
//    bRet = FALSE;
//  else
//    {
//    dwSts = GetFileAttributes(lpFileName);
//    if(dwSts == INVALID_FILE_ATTRIBUTES)
//	    {
//      dwErr = GetLastError();
//      if(dwErr == ERROR_INVALID_NAME)
//        bRet = FALSE;
//      else if(dwErr == ERROR_PATH_NOT_FOUND || dwErr == ERROR_FILE_NOT_FOUND)
//        bRet = FALSE;
//      }
//    }
//
//	return(bRet);
//}

//return -1: user break;
//return 0: no error
//return 1: lpPath is invalid
//return 2: can not create lpPath
int CFolderDialog::MakeSurePathExists(LPCTSTR lpPath)
  {
	CString cstrMsg;
	int iRet;
  DWORD dwSts, dwErr;

  iRet = ERR_NONE;
	//validate path
	//iRet = Touch(lpPath, TRUE);

	if(lpPath == NULL)
    iRet = ERR_INV_NAME;
  else
    {
    dwSts = GetFileAttributes(lpPath);
    if(dwSts == INVALID_FILE_ATTRIBUTES)
	    {
      dwErr = GetLastError();
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, 0L, s_szErrMsg,
        sizeof(s_szErrMsg), NULL);
      if(dwErr == ERROR_INVALID_NAME)
        iRet = ERR_INV_NAME;
	    }
    }
  if(iRet == ERR_NONE)
    {
  	//if(!IsFileNameValid(lpPath))
   //   iRet = ERR_EXIST;
    dwSts = GetFileAttributes(lpPath);
    if(dwSts == INVALID_FILE_ATTRIBUTES)
	    {
      dwErr = GetLastError();
      if(dwErr == ERROR_INVALID_NAME)
        iRet = ERR_INV_NAME;
      //else if(dwErr == ERROR_PATH_NOT_FOUND || dwErr == ERROR_FILE_NOT_FOUND)
      //  iRet = ERR_EXIST;
      }
    }

	if(iRet == ERR_EXIST)
    {
    if(s_pLang == NULL)
		  cstrMsg.Format(c_FolderDoesNotExist, lpPath);
    else
      {
      cstrMsg = s_pLang->cstrLoad_string(ITEM_G_THE_FOLDER) + "\n    "
        + lpPath + "\n"
        + s_pLang->cstrLoad_string(ITEM_G_FOLDER_DOES_NOT_EXIST);
      }
		if(AfxMessageBox(cstrMsg, MB_YESNO|MB_ICONQUESTION) != IDYES)
			iRet = ERR_EXIST;
    else
      { // create path
			if(CreateDirectory(lpPath, NULL) == 0)
        { // Error, get the error message.
        iRet = ERR_CREATE;
        dwErr = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, 0L, s_szErrMsg, sizeof(s_szErrMsg), NULL);
        }
      else
        iRet = ERR_NONE;
		  //iRet = Touch(lpPath, FALSE);
      }
    }
  if(iRet != ERR_NONE)
	  {
		switch(iRet)
		  {
		  case ERR_INV_NAME:
        if(s_pLang == NULL)
  			  cstrMsg.Format(c_szErrInvalidPath, lpPath);
        else
          {
          cstrMsg = s_pLang->cstrLoad_string(ITEM_G_THE_FOLDER) + "\n    "
            + lpPath + "\n"
            + s_pLang->cstrLoad_string(ITEM_G_INVALID_PATH);
          }
			  break;
      //case ERR_NO_ACCESS:
      //  if(s_pLang == NULL)
      //    cstrMsg.Format("Can't access the folder: %s", lpPath);
      //  break;
		  case ERR_CREATE:
		  default:
        if(s_pLang == NULL)
  			  cstrMsg.Format(c_szErrCreatePath, lpPath);
        else
          {
          cstrMsg = s_pLang->cstrLoad_string(ITEM_G_THE_FOLDER) + "\n    "
            + lpPath + "\n"
            + s_pLang->cstrLoad_string(ITEM_G_ERR_CREATE_PATH);
          }
			  break;
		  }
    if(s_szErrMsg[0] != NULL)
      {
      cstrMsg += "\n\n";
      cstrMsg += s_szErrMsg;
      }
		AfxMessageBox(cstrMsg, MB_OK|MB_ICONEXCLAMATION);
	  }
	return(iRet);
}

void CFolderDialog::vSet_language(CLanguage *pLang)
  {
  s_pLang = pLang;
  }

//return 0: no error
//return 1: lpPath is invalid
//return 2: lpPath can not be created(bValidate==FALSE)
//int CFolderDialog::Touch(LPCTSTR lpPath, BOOL bValidate)
//  {
//	TCHAR szPath[MAX_PATH];
//  int iRet, iLen;
//  DWORD dwSts, dwErr;
//
//  iRet = ERR_NONE;
//	if(lpPath == NULL)
//    iRet = ERR_INV_NAME;
//  else
//    {
//	  _tcscpy(szPath, lpPath);
//	  iLen = _tcslen(szPath);
//
//    dwSts = GetFileAttributes(szPath);
//    if(dwSts == INVALID_FILE_ATTRIBUTES)
//	    {
//      dwErr = GetLastError();
//      if(dwErr == ERROR_INVALID_NAME)
//        iRet = ERR_INV_NAME;
//	    }
//    }
//  if(iRet == ERR_NONE)
//    {
//		if(!bValidate)
//		  {
//			if(_access(szPath, 0) != 0)
//        iRet = ERR_NO_ACCESS;
//		  }
//    }
//  if(iRet == ERR_NONE)
//    {
//  	if(!IsFileNameValid(lpPath))
//      iRet = ERR_EXIST;
//    }
//
//  return(iRet);
//
//	//i = 3;
//	//BOOL bLastOne=TRUE;
//	//LPTSTR lpCurrentName;
//	//while(szPath[i]!=0)
//	//{
//	//	lpCurrentName = &szPath[i];
//	//	while( (szPath[i]!=0) && (szPath[i]!=_T('\\')) )
//	//	{
//	//		i++;
//	//	}
//
//	//	bLastOne =(szPath[i]==0);
//	//	szPath[i] = 0;
//
//	//	if( !IsFileNameValid(lpCurrentName) )
//	//	{
//	//		return 1;
//	//	}
//
//	//	if(!bValidate)
//	//	{
//	//		CreateDirectory(szPath, NULL);
//	//		if(_taccess(szPath, 0)!=0)
//	//		{
//	//			return 2;
//	//		}
//	//	}
//
//	//	if(bLastOne)
//	//	{
//	//		break; //it's done
//	//	}
//	//	else
//	//	{
//	//		szPath[i] = _T('\\');
//	//	}
//
//	//	i++;
//	//}
//
//	//return (bLastOne?0:1);
//}

//return 0: ok
//return 1: error
//int CFolderDialog::ConcatPath(LPTSTR lpRoot, LPCTSTR lpMorePath)
//{
//	if(lpRoot==NULL)
//	{
//		return 1;
//	}
//
//	int nLen = _tcslen(lpRoot);
//
//	if(nLen<3)
//	{
//		return 1;
//	}
//
//	if(lpMorePath==NULL)
//	{
//		return 0;
//	}
//
//	if(nLen==3)
//	{
//		_tcscat(lpRoot, lpMorePath);
//		return 0;
//	}
//
//	_tcscat(lpRoot, _T("\\"));
//	_tcscat(lpRoot, lpMorePath);
//
//	return 0;
//}
