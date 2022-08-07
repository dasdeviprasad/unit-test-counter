/********************************************************************
HwInitDlg.cpp

Copyright (C) 2012, 3CPM Company, Inc.  All rights reserved.

  Implementation file. Displays the main dialog box.

HISTORY:
07-JAN-12  RET  New.
********************************************************************/

#include "stdafx.h"
#include "HwInit.h"
#include "HwInitDlg.h"

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


// CHwInitDlg dialog




CHwInitDlg::CHwInitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHwInitDlg::IDD, pParent)
  , m_cstrExpDate(_T(""))
  , m_iNumTimesToRun(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHwInitDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_STATIC_EXP_DATE, m_cstrExpDate);
  DDX_Text(pDX, IDC_STATIC_NUM_TIMES_TO_RUN, m_iNumTimesToRun);
}

BEGIN_MESSAGE_MAP(CHwInitDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDOK, &CHwInitDlg::OnBnClickedOk)
//  ON_WM_CTLCOLOR()
ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CHwInitDlg message handlers

BOOL CHwInitDlg::OnInitDialog()
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
#ifdef DEBUG
  // If debugging, show the expiration date and the number of times the program is allowed to run.
  GetDlgItem(IDC_STATIC_EXP_DATE_TITLE)->ShowWindow(SW_SHOW);
  GetDlgItem(IDC_STATIC_EXP_DATE)->ShowWindow(SW_SHOW);
  GetDlgItem(IDC_STATIC_NUM_TIMES_TO_RUN_TITLE)->ShowWindow(SW_SHOW);
  GetDlgItem(IDC_STATIC_NUM_TIMES_TO_RUN)->ShowWindow(SW_SHOW);
#endif
  m_cstrErrMsg = "";
  // Make all fields bold to see easier.
  CFont cfBoldFont;  // Object to hold the bold font being used for dialog box.
  vSet_font(this, NULL, &cfBoldFont, FW_BOLD);
    // Set the font in the dialog box fields.
  GetDlgItem(IDC_STATIC_MSG)->SetFont(&cfBoldFont);
  GetDlgItem(IDOK)->SetFont(&cfBoldFont);
  GetDlgItem(IDCANCEL)->SetFont(&cfBoldFont);
  //// Read the "HwInit.txt" file and extract the expiration date.
  //if(bRead_file() == true)
  //{
  //  // If the expiration date has not been exceeded, allow changing the EGGSAS.INI file.
  //  // If the current date is later than the expiration date, display an error message and exit.
  //  // If the number of times the program can run is zero, display an error message.
  //  if(bIsExpired() == true || bCan_run() == false)
  //  {
  //    m_cstrErrMsg = _T("This software has expired");
  //    GetDlgItem(IDC_STATIC_MSG)->SetWindowTextW(m_cstrErrMsg);
  //  }
  //  else
  //  {
  //    GetDlgItem(IDC_STATIC_MSG)->SetWindowTextW(_T("Select OK to allow the EGGSAS program to link to the Human Interface Module."));
  //  }
  //}
  //else
  //{
  //  GetDlgItem(IDC_STATIC_MSG)->SetWindowTextW(m_cstrErrMsg);
  //}
  GetDlgItem(IDC_STATIC_MSG)->SetWindowTextW(_T("Select OK to allow the EGGSAS program to link to the Human Interface Module."));
  if(m_cstrErrMsg.IsEmpty() == false)
  {
    GetDlgItem(IDOK)->EnableWindow(FALSE);
  }

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHwInitDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHwInitDlg::OnPaint()
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
HCURSOR CHwInitDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/********************************************************************
OnBnClickedOk - Handler for the OK button.
                  - Update the EGGSAS.INI file to allow re-binding to the hardware.
                  - If the number of times to run is used:
                      Decrement it.
                      Write it back to the HWINIT.TXT file.
                         
********************************************************************/
void CHwInitDlg::OnBnClickedOk()
{
  CString cstrPgmKey;
  char cTemp;
  int iValue;

  // Modify the EGGSAS.INI file so it will re-bind to the hardware.
  cstrPgmKey = AfxGetApp()->GetProfileString(_T(INI_SEC_GENERAL), _T(INI_ITEM_PGMKEY), (LPCTSTR)"");
  cstrPgmKey.SetAt(INI_DEV_ID_INDEX_1, INI_DEV_ID_NOT_GEN_1);
  cstrPgmKey.SetAt(INI_DEV_ID_INDEX_2, INI_DEV_ID_NOT_GEN_2);
  // Write the program key back to the INI file.
  AfxGetApp()->WriteProfileString(_T(INI_SEC_GENERAL), _T(INI_ITEM_PGMKEY), cstrPgmKey);
 // if(m_iNumTimesToRun != NUM_TIMES_TO_RUN_NOT_USED)
 // { // Update the number of times to run.
 //   if(m_iNumTimesToRun > 0)
 //     --m_iNumTimesToRun;
 //   // Write the number of times to run back to the txt file.
 //   iValue = m_iNumTimesToRun;
 //   cTemp = (iValue / 100) + '0';
 //   vEncode_item(NUM_TIMES_TO_RUN1_LINE, NUM_TIMES_TO_RUN1_IDX, cTemp);
 //   iValue = iValue % 100;
 //   cTemp = (iValue / 10) + '0';
 //   vEncode_item(NUM_TIMES_TO_RUN2_LINE, NUM_TIMES_TO_RUN2_IDX, cTemp);
 //   iValue = iValue % 10;
 //   cTemp = iValue + '0';
 //   vEncode_item(NUM_TIMES_TO_RUN3_LINE, NUM_TIMES_TO_RUN3_IDX, cTemp);
 //   CStdioFile csfOutput;
 //   if(!csfOutput.Open(_T(EXP_DATE_FILE_NAME), CFile::modeCreate | CFile::modeWrite | CFile::typeText)) 
 //     MessageBox(_T("Unable to open file\n"), _T("ERROR"));
 //   else
 //   {
 //     CString cstrNewLine = _T("\r\n");
 //     csfOutput.WriteString(m_cstrLine1);
 //     csfOutput.WriteString(cstrNewLine);
 //     csfOutput.WriteString(m_cstrLine2);
 //     csfOutput.WriteString(cstrNewLine);
 //     csfOutput.WriteString(m_cstrLine3);
 //     csfOutput.WriteString(cstrNewLine);
 //     csfOutput.WriteString(m_cstrLine4);
 //     csfOutput.WriteString(cstrNewLine);
 //     csfOutput.Close();
 //  }
 //}
  OnOK();
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
void CHwInitDlg::vSet_font(CWnd *pwnd, CDC *pDC, CFont *pcfFont, long lFontWeight,
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
  _tcsncpy_s(lfLF.lfFaceName, LF_FACESIZE, _T("Arial"), 7); // request a face name "Arial"

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
long CHwInitDlg::lCalc_font_height(int iPointSize, CDC *pcdc)
  {
  return(MulDiv(iPointSize, pcdc->GetDeviceCaps(LOGPIXELSY), 72));
  }


/********************************************************************
bRead_file - read the HWINIT.TXT file.
                         
  inputs: None.

  return: true if the file could be read, false if the file is missing or corrupt.
********************************************************************/
bool CHwInitDlg::bRead_file(void)
{
  bool bRet = false;

  CStdioFile csfInput;
  if(!csfInput.Open(_T(EXP_DATE_FILE_NAME), CFile::modeRead | CFile::typeText)) 
    m_cstrErrMsg = _T("Unable to open file");
  else
    {
    csfInput.ReadString(m_cstrLine1);
    csfInput.ReadString(m_cstrLine2);
    csfInput.ReadString(m_cstrLine3);
    csfInput.ReadString(m_cstrLine4);
    csfInput.Close();
    if(m_cstrLine1.GetLength() < LAST_INDEX || m_cstrLine2.GetLength() < LAST_INDEX
      || m_cstrLine3.GetLength() < LAST_INDEX || m_cstrLine4.GetLength() < LAST_INDEX)
    {
      m_cstrErrMsg = _T("Contents of file is corrupt");
      bRet = false;
    }
    else
      bRet = true;
    }
  return(bRet);
}

/********************************************************************
bIsExpired - Check the expiration date.
                         
  inputs: None.

  return: true if the expiration date is past, false if the expiration
          date hasn't been reached yet.
********************************************************************/
bool CHwInitDlg::bIsExpired(void)
  {
  int iMonth, iDay, iYear;
  bool bRet;
  CTime ctCurrent;

  // Decode expiration date from the pgmkey1 in the INI file.
  iMonth = (m_cstrLine3.GetAt(MONTH1_IDX) - '0') * 10;
  iMonth += (m_cstrLine1.GetAt(MONTH2_IDX) - '0');
  iMonth -= 20;
  iDay = (m_cstrLine4.GetAt(DAY1_IDX) - '0') * 10;
  iDay += (m_cstrLine2.GetAt(DAY2_IDX) - '0');
  iDay -= 30;
  iYear = (m_cstrLine1.GetAt(YEAR1_IDX) - '0') * 1000;
  iYear += (m_cstrLine3.GetAt(YEAR2_IDX) - '0') * 100;
  iYear += (m_cstrLine2.GetAt(YEAR3_IDX) - '0') * 10;
  iYear += m_cstrLine4.GetAt(YEAR4_IDX) - '0';
  iYear /= 2;
  CTime ctExpDate(iYear, iMonth, iDay, 0, 0, 0);

 #ifdef DEBUG
    // show the expiration date.
  m_cstrExpDate.Format(_T("%02d/%02d/%04d"), iMonth, iDay, iYear);
  UpdateData(FALSE);
 #endif

  ctCurrent = CTime::GetCurrentTime(); // Current time
  CTime ctCurrentDate(ctCurrent.GetYear(), ctCurrent.GetMonth(), ctCurrent.GetDay(), 0, 0, 0);

  if(ctExpDate >= ctCurrentDate)
    bRet = false; // Not expired
  else
    bRet = true; // Expired

  //// Now get the current date.
  //int iCurrentDay, iCurrentMonth, iCurrentYear;
  //ctCurrent = CTime::GetCurrentTime();
  //iCurrentDay = ctCurrent.GetDay();
  //iCurrentMonth = ctCurrent.GetMonth();
  //iCurrentYear = ctCurrent.GetYear();
  //if(ctCurrent.GetYear() <= iYear
  //  && ctCurrent.GetMonth() <= iMonth
  //  && ctCurrent.GetDay() <= iDay)
  //  bRet = false; // Not expired
  //else
  //  bRet = true; // Expired

  return(bRet);
  }

/********************************************************************
bCan_run - Determine if the number of times the program can run has been
           exceeded.

           The program can run if the number of times it can run hasn't
             been decremented down to 0 or it is not being used (0xFFF).
                         
  inputs: None.

  return: true if the program can run, otherwise false.
********************************************************************/
bool CHwInitDlg::bCan_run(void)
{
  bool bRet;
  int iValue1, iValue2, iValue3;
  char c1, c2, c3;

  c1 = (char)m_cstrLine2.GetAt(NUM_TIMES_TO_RUN1_IDX);
  c2 = (char)m_cstrLine1.GetAt(NUM_TIMES_TO_RUN2_IDX);
  c3 = (char)m_cstrLine4.GetAt(NUM_TIMES_TO_RUN3_IDX);
  if(c1 == 'F' && c2 == 'F' && c3 == 'F')
  {
    m_iNumTimesToRun = 0xFFF;
    bRet = true; // Not using
  }
  else
  {
    iValue1 = c1 - '0';
    iValue2 = c2 - '0';
    iValue3 = c3 - '0';
    m_iNumTimesToRun = (iValue1 * 100) + (iValue2 * 10) + iValue3;
    if(m_iNumTimesToRun > 0)
      bRet = true; // Ok to run.
    else
      bRet = false; // Can't run.
  }
 #ifdef DEBUG
    // show the number of times to run.
  UpdateData(FALSE);
 #endif
  return(bRet);
}

/********************************************************************
vEncode_item - Places the specified character on the specified out put line at the
               specified index.
                         
  inputs: Line number (starting at 1) that the item is placed on.
          Index of the location on the line for the item.
          Character item that gets placed at the index.

  return: None.
********************************************************************/
void CHwInitDlg::vEncode_item(int iLine, int iLineIndex, char cItem)
{

  switch(iLine)
  {
  case 1:
    m_cstrLine1.SetAt(iLineIndex, cItem);
    break;
  case 2:
    m_cstrLine2.SetAt(iLineIndex, cItem);
    break;
  case 3:
    m_cstrLine3.SetAt(iLineIndex, cItem);
    break;
  case 4:
    m_cstrLine4.SetAt(iLineIndex, cItem);
    break;
  }
}

/********************************************************************
OnCtlColor - Handler for setting the color in a field.
             This is used to display error messages in red.
                         
********************************************************************/
HBRUSH CHwInitDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

  // TODO:  Change any attributes of the DC here
  //if(pWnd == GetDlgItem(IDC_EDIT1))
  if(pWnd->GetDlgCtrlID() == IDC_STATIC_MSG)
  {
    if(m_cstrErrMsg.IsEmpty() == false)
      pDC->SetTextColor(RGB(255, 0, 0)); // red
   //pDC->SetBkColor(RGB(255, 0, 0)); // red
  }

  // TODO:  Return a different brush if the default is not desired
  return hbr;
}

