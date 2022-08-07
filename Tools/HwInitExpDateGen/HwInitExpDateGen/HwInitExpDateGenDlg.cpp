/********************************************************************
HwInitExpDateGenDlg.cpp

Copyright (C) 2012, 3CPM Company, Inc.  All rights reserved.

  Implementation file. Displays the main dialog box.

  NOTES:
    This dialog box provides for the user entries of the expiration date and the number of
    times the program that resets the hardware binding for EGGSAS is allowed to run.

    It also creates the file. See HwInitExpDateGenDlg.h for the details of the
    file and how items are encoded into it.


HISTORY:
07-JAN-12  RET  New.
********************************************************************/

#include "stdafx.h"
#include "HwInitExpDateGen.h"
#include "HwInitExpDateGenDlg.h"

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


// CHwInitExpDateGenDlg dialog




CHwInitExpDateGenDlg::CHwInitExpDateGenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHwInitExpDateGenDlg::IDD, pParent)
  , m_iNumTimesToRun(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHwInitExpDateGenDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_TIMES_TO_RUN, m_iNumTimesToRun);
  DDX_Control(pDX, IDC_DATETIMEPICKER_EXP_DATE, c_dtcExpDate);
}

BEGIN_MESSAGE_MAP(CHwInitExpDateGenDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDOK, &CHwInitExpDateGenDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CHwInitExpDateGenDlg message handlers

BOOL CHwInitExpDateGenDlg::OnInitDialog()
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
  CFont cfBoldFont;  // Object to hold the bold font being used for dialog box.

  //vSet_font(GetDlgItem(IDC_STATIC_MSG), NULL, &cfBoldFont, FW_BOLD);
  vSet_font(this, NULL, &cfBoldFont, FW_BOLD);
    // Set the font in the dialog box fields to a bold font.
  GetDlgItem(IDC_STATIC_MSG)->SetFont(&cfBoldFont);
  GetDlgItem(IDC_STATIC_MSG1)->SetFont(&cfBoldFont);
  GetDlgItem(IDC_STATIC_MSG2)->SetFont(&cfBoldFont);
  GetDlgItem(IDC_STATIC_MSG3)->SetFont(&cfBoldFont);
  GetDlgItem(IDC_STATIC_MSG4)->SetFont(&cfBoldFont);
  GetDlgItem(IDOK)->SetFont(&cfBoldFont);
  GetDlgItem(IDCANCEL)->SetFont(&cfBoldFont);
  GetDlgItem(IDC_EDIT_EXP_DATE)->SetFont(&cfBoldFont);
  GetDlgItem(IDC_EDIT_TIMES_TO_RUN)->SetFont(&cfBoldFont);
 
  // Set the timedatepicker control for just the date as mm/dd/yyyy.
  c_dtcExpDate.SetFormat(_T("MM/dd/yyyy"));
  CTime ctCurrent = CTime::GetCurrentTime();
  // Set minimum range so can't get a date before today.
  c_dtcExpDate.SetRange(&ctCurrent, NULL);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHwInitExpDateGenDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHwInitExpDateGenDlg::OnPaint()
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
HCURSOR CHwInitExpDateGenDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
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
void CHwInitExpDateGenDlg::vSet_font(CWnd *pwnd, CDC *pDC, CFont *pcfFont, long lFontWeight,
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
long CHwInitExpDateGenDlg::lCalc_font_height(int iPointSize, CDC *pcdc)
  {
  return(MulDiv(iPointSize, pcdc->GetDeviceCaps(LOGPIXELSY), 72));
  }

/********************************************************************
OnBnClickedOk - Handler for the OK button.
                  - Checks to make sure the date is valid.
                  - Create the output file.
                         
********************************************************************/
void CHwInitExpDateGenDlg::OnBnClickedOk()
{

  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  vCreate_file();
  OnOK();
}

/********************************************************************
vCreate_file - Create the output file.
                 Encodes the month. 20 is added to the month.
                 Encodes the day.  30 is added to the day.
                 Encodes the year.  The year is doubled.
                 Limits the number of times the binding reset program can run.
                 Write all items to the output file.
                         
  inputs: None.

  return: None.
********************************************************************/
void CHwInitExpDateGenDlg::vCreate_file(void)
  {
  int iMonth, iDay, iYear, iValue;
  char cTemp;
  CTime ctExpDate;

  UpdateData(TRUE);

  m_cstrLine1 = LINE1;
  m_cstrLine2 = LINE2;
  m_cstrLine3 = LINE3;
  m_cstrLine4 = LINE4;

  // Get the date and alter it.
  c_dtcExpDate.GetTime(ctExpDate);
  iMonth = ctExpDate.GetMonth() + 20;
  iDay = ctExpDate.GetDay() + 30;
  iYear = ctExpDate.GetYear() * 2;

  cTemp = (iMonth / 10) + '0';
  vEncode_item(MONTH1_LINE, MONTH1_IDX, cTemp);
  cTemp = (iMonth % 10) + '0';
  vEncode_item(MONTH2_LINE, MONTH2_IDX, cTemp);

  cTemp = (iDay / 10) + '0';
  vEncode_item(DAY1_LINE, DAY1_IDX, cTemp);
  cTemp = (iDay % 10) + '0';
  vEncode_item(DAY2_LINE, DAY2_IDX, cTemp);

  cTemp = (iYear / 1000) + '0';
  vEncode_item(YEAR1_LINE, YEAR1_IDX, cTemp);
  iYear = iYear % 1000;
  cTemp = (iYear / 100) + '0';
  vEncode_item(YEAR2_LINE, YEAR2_IDX, cTemp);
  iYear = iYear % 100;
  cTemp = (iYear / 10) + '0';
  vEncode_item(YEAR3_LINE, YEAR3_IDX, cTemp);
  iYear = iYear % 10;
  cTemp = iYear + '0';
  vEncode_item(YEAR4_LINE, YEAR4_IDX, cTemp);

  // Now for the number of times it can run.
  if(m_iNumTimesToRun > 0)
  {
    if(m_iNumTimesToRun > 999)
      m_iNumTimesToRun = 999;
    iValue = m_iNumTimesToRun;
    cTemp = (iValue / 100) + '0';
    vEncode_item(NUM_TIMES_TO_RUN1_LINE, NUM_TIMES_TO_RUN1_IDX, cTemp);
    iValue = iValue % 100;
    cTemp = (iValue / 10) + '0';
    vEncode_item(NUM_TIMES_TO_RUN2_LINE, NUM_TIMES_TO_RUN2_IDX, cTemp);
    iValue = iValue % 10;
    cTemp = iValue + '0';
    vEncode_item(NUM_TIMES_TO_RUN3_LINE, NUM_TIMES_TO_RUN3_IDX, cTemp);
  }
  else
  { // Not use number of times it can ru
    vEncode_item(NUM_TIMES_TO_RUN1_LINE, NUM_TIMES_TO_RUN1_IDX, 'F');
    vEncode_item(NUM_TIMES_TO_RUN2_LINE, NUM_TIMES_TO_RUN2_IDX, 'F');
    vEncode_item(NUM_TIMES_TO_RUN3_LINE, NUM_TIMES_TO_RUN3_IDX, 'F');
  }

  CStdioFile csfOutput;
  if(!csfOutput.Open(_T(EXP_DATE_FILE_NAME), CFile::modeCreate | CFile::modeWrite | CFile::typeText)) 
    MessageBox(_T("Unable to open file\n"), _T("ERROR"));
  else
    {
    csfOutput.WriteString(m_cstrLine1);
    csfOutput.WriteString(m_cstrLine2);
    csfOutput.WriteString(m_cstrLine3);
    csfOutput.WriteString(m_cstrLine4);
    csfOutput.Close();
    }
  }

/********************************************************************
vEncode_item - Places the specified character on the specified out put line at the
               specified index.
                         
  inputs: Line number (starting at 1) that the item is placed on.
          Index of the location on the line for the item.
          Character item that gets placed at the index.

  return: None.
********************************************************************/
void CHwInitExpDateGenDlg::vEncode_item(int iLine, int iLineIndex, char cItem)
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

///********************************************************************
//bValidate_date - Make sure the date is valid.  Also the date has to be
//                 later than the current date.
//
//                 Date format: mm/dd/yyyy
//                         
//  inputs: CString object containing the date.
//
//  return: true if the date is valid, otherwise false.
//********************************************************************/
//bool CHwInitExpDateGenDlg::bValidate_date(CString cstrDate)
//{
//  bool bRet = false;
//  int iMonth, iDay, iYear;
//  CTime ctCurrent;
//  CString cstrResult;
//
//  // First look for slashes.
//  if(cstrDate.Find(_T("/")) == 2 && cstrDate.Find(_T("/"), 3) == 5)
//  {
//    ctCurrent = CTime::GetCurrentTime();
//    iMonth = ((cstrDate.GetAt(0) - '0') * 10) + cstrDate.GetAt(1) - '0';
//    if(iMonth >= 1 && iMonth <= 12 && iMonth >= ctCurrent.GetMonth())
//    {
//      iYear = ((cstrDate.GetAt(6) - '0') * 1000) + ((cstrDate.GetAt(7) - '0') * 100)
//        + ((cstrDate.GetAt(8) - '0') * 10) + cstrDate.GetAt(9) - '0';
//      if(iYear >= ctCurrent.GetYear())
//      {
//        iDay = ((cstrDate.GetAt(3) - '0') * 10) + cstrDate.GetAt(4) - '0';
//        if(iDay <= 0 || iDay > iDays_in_month(iMonth, iYear) || iDay < ctCurrent.GetDay())
//          MessageBox(_T("The Day is wrong."), _T("ERROR"));
//        else
//          bRet = true;
//      }
//      else
//        MessageBox(_T("The year is wrong."), _T("ERROR"));
//    }
//    else
//      MessageBox(_T("The month is wrong."), _T("ERROR"));
//  }
//  else
//    MessageBox(_T("Date is in the wrong format."), _T("ERROR"));
//
//  return(bRet);
//}

/********************************************************************
iDays_in_month - Get the number of days in the month of the specified year.
                         
  inputs: Month starting at 1.
          4 digit year.

  return: Number of days in the month.
********************************************************************/
int CHwInitExpDateGenDlg::iDays_in_month(int iMonth, int iYear)
{
  int iDaysInMonth;

  if(iMonth == 1 || iMonth == 3 || iMonth == 5 || iMonth == 7 ||iMonth == 8
    || iMonth == 10 || iMonth == 12)
    iDaysInMonth = 31;
  else if(iMonth == 2)
  { // February
    // Check for leapyear
    if((iYear % 4 == 0 && (iYear % 100 != 0 || iYear % 400 == 0)) == 0)
      iDaysInMonth = 28; // Not a leap year.
    else
      iDaysInMonth = 29;
  }
  else
    iDaysInMonth = 30;
  return(iDaysInMonth);
}
