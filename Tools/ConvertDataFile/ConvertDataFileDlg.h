// ConvertDataFileDlg.h : header file
//

#pragma once


// CConvertDataFileDlg dialog
class CConvertDataFileDlg : public CDialog
{
// Construction
public:
	CConvertDataFileDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CONVERTDATAFILE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
public: // Data
  CString m_cstrFileName;
  CString m_cstrFilePath;

public: // Methods
  BOOL bCopy_one_file(LPCTSTR pstrDestPath, LPCTSTR pstrSrcPath, LPCTSTR pstrName,
                      BOOL bNoAutoOverwrite);
  bool bConvert_eggsas4_to_research(CDataFile *pdf);


protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedBtnSelDataFile();
  afx_msg void OnBnClickedBtnConvEggsas4ToResrch();
  CString m_cstrDataFile;
  CString m_cstrConversionStatus;
  int m_iNumBaselineMinutes;
};
