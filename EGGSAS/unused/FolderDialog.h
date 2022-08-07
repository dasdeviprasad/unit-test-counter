//////////////////////////////////////////////////////////////////////////
//PathDialog.h file
//
//Written by Nguyen Tan Hung <tanhung@yahoo.com>
//////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FOLDERDIALOG_H__0F70BC86_11DB_11D4_B012_0000E8DD8DAA__INCLUDED_)
#define AFX_FOLDERDIALOG_H__0F70BC86_11DB_11D4_B012_0000E8DD8DAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//// Errors
#define ERR_NONE 0
#define ERR_INV_NAME -1 // Invalid path.
#define ERR_EXIST -2  // Folder doesn't exist
#define ERR_CREATE -3  // Can't create the folder
#define ERR_NO_ACCESS -4 // Can't access the folder.
//// end of errors

class CFolderDialog;

// CPathDialogSub - intercepts messages from child controls
class CFolderDialogSub : public CWnd
{
	friend CFolderDialog;
public:
	CFolderDialog* m_pFolderDialog;
protected:
    afx_msg void OnOK();              // OK button clicked
	afx_msg void OnChangeEditPath();
    DECLARE_MESSAGE_MAP()
private:
};

/////////////////////////////////////////////////////////////////////////////
// CPathDialog dialog

class CFolderDialog
{
	friend CFolderDialogSub;
  // Construction
  public:
	  CFolderDialog(LPCTSTR lpszCaption=NULL,
		LPCTSTR lpszTitle=NULL,
		LPCTSTR lpszInitialPath=NULL, 
		CWnd* pParent = NULL);

	  CString GetPathName();
	  virtual int DoModal();

	  //static Touch(LPCTSTR lpPath, BOOL bValidate=TRUE);
	  static int MakeSurePathExists(LPCTSTR lpPath);
	  //static BOOL IsFileNameValid(LPCTSTR lpFileName);
	  //static int ConcatPath(LPTSTR lpRoot, LPCTSTR lpMorePath);

    void vSet_language(CLanguage *pLang);

  private:
	  static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam, LPARAM pData);

	  LPCTSTR m_lpszCaption;
	  LPCTSTR m_lpszInitialPath;

	  TCHAR m_szPathName[MAX_PATH];

	  BROWSEINFO m_bi;
	  HWND m_hWnd;
	  CWnd*	m_pParentWnd;
	  BOOL m_bParentDisabled;
	  BOOL m_bGetSuccess;

	  CFolderDialogSub m_FolderDialogSub;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOLDERDIALOG_H__0F70BC86_11DB_11D4_B012_0000E8DD8DAA__INCLUDED_)
