/********************************************************************
util.cpp

Copyright (C) 2003,2004,2005,2008,2011,2012,2016 3CPM Company, Inc.  All rights reserved.

  This file provides general purpose routines for use by all classes.

      vSet_font()
      lCalc_font_height()
      ctDate_to_ctime()
      uFround()
      vCenter_button_horiz()
      iValidate_date()
      fStd_dev()
      cstrGet_hrs_mins()
      iCopy_one_file()
      vFormat_patient_name()
      fRound_down_to_half_min()
      ucAscii_to_Hex()
      cHex_to_ascii()
      cstrGet_file_name()


HISTORY:
03-OCT-03  RET  New.
06-FEB-05  RET  Add function vFormat_patient_name().
10-FEB-05  RET  Change vFormat_patient_name() to add middle initial.
30-JUN-08  RET  Version 2.05
                 Add function cstrReformat_date().
25-FEB-11  RET
                Add funcions: ucAscii_to_Hex(), cHex_to_ascii()
01-OCT-11  RET
             Changes to write an ASCII raw data file and not allow any analysis.
                Add function: cstrGet_file_name()
23-FEB-12  RET  Version 
                  Changes for foreign languages.
24-AUG-12  RET
             Change method vFormat_patient_name() to format the name differently
               for a report or for display on the screen.  Added parameter to
               indicate if its for a report.  If for a report, make sure the
               item is checked to be on the report.
18-SEP-12  RET
             Change vFormat_patient_name() to not include the research reference number.
02-JUN-16  RET
             Add feature to hide study dates.
               Add functions: vEncode_char_in_string(), vEncode_byte_in_string(),
                 cDecode_char_from_string(), cstrCreate_guid(),
                 cstrEncode_study_date(), cstrDecode_study_date(),
                 vSet_file_date_time()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "sysdefs.h"
#include "math.h"
#include "Iptypes.h"
#include "Iphlpapi.h"
#include "Dbaccess.h"
#include "util.h"

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
void vSet_font(CWnd *pwnd, CDC *pDC, CFont *pcfFont, long lFontWeight,
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
  strcpy(lfLF.lfFaceName, "Arial");  // Font name is "Arial".
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
long lCalc_font_height(int iPointSize, CDC *pcdc)
  {
  return(MulDiv(iPointSize, pcdc->GetDeviceCaps(LOGPIXELSY), 72));
  }

/********************************************************************
ctDate_to_ctime - Convert a string containing a date to a CTime class.

                  The date must be in the format:
                    mm/dd/yyyy
                         
  inputs: Reference to a CString object containing the date.

  return: CTime object containing the input date.
********************************************************************/
CTime ctDate_to_ctime(CString cstrDate)
  {
  struct tm tmTime;

  tmTime.tm_hour = 0;
  tmTime.tm_min = 0;
  tmTime.tm_sec = 0;
  tmTime.tm_isdst = 0;
  tmTime.tm_mon = atoi(cstrDate.Left(2)) - 1; // 0 based month.
  tmTime.tm_mday = atoi(cstrDate.Mid(3, 2));
  tmTime.tm_year = atoi(cstrDate.Right(4)) - 1900;
  time_t tTime = mktime(&tmTime);
  return(mktime(&tmTime));
  }

/********************************************************************
cstrReformat_date - Reformat a date from yyyy/mm/dd to mm/dd/yyyy

inputs: CString object containing date to be reformatted.

return: CString object containing reformatted date.
********************************************************************/
CString cstrReformat_date(CString cstrDate)
  {
  CString cstrOut;

  cstrOut.Format("%s/%s/%s", cstrDate.Mid(5, 2), cstrDate.Mid(8, 2),
    cstrDate.Left(4));
  return(cstrOut);
  }

/********************************************************************
vCenter_button_horiz - Center the specified button horizontally in the
                       specified window.

  inputs: Pointer to a CWnd object that specifies the window that contains
            the button.
          Pointer to a CWnd object that specifies the button to be centered.

  return: None.
********************************************************************/
void vCenter_button_horiz(CWnd *pwndDlg, CWnd *pwndButton)
  {
  WINDOWPLACEMENT wp;
  RECT rRect;
  int iWidth;

    // Horizontally center the button.
  pwndDlg->GetWindowRect(&rRect); // Dimensions of dialog box window.
  pwndButton->GetWindowPlacement(&wp); // Dimensions and position of button.
  iWidth = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
  wp.rcNormalPosition.left = (rRect.right / 2) - (iWidth / 2);
  wp.rcNormalPosition.right = wp.rcNormalPosition.left + iWidth;
  pwndButton->SetWindowPlacement(&wp);
  }

/****************************************************************************
uFround - Round a floating point number to an unsigned integer value.
          Used only for positive numbers.

    inputs: Floating point number.

    return: The rounded unsigned integer.
****************************************************************************/
unsigned uFround(float fNum)
    {
    double dFract, dInteg;

    dFract = modf((double)fNum, &dInteg);
    if(dFract >= .5)
     ++dInteg;
    return(unsigned(dInteg));
    }

/****************************************************************************
iValidate_date - Validate a date for a valid day, month.
                 A valid year is one that contains 4 digits and is less than
                 the current year.

    inputs: CString object containing the date in the format:
                mm/dd/yyyy

    return: SUCCESS if the date is valid
            FAIL if the date is NOT valid.
****************************************************************************/
short int iValidate_date(CString cstrDateOfBirth)
  {
  short int iMonth, iDay, iYear, iIndex, iMaxDay;
  short int iRet = FAIL;

  if((iIndex = cstrDateOfBirth.Find('/')) > 0)
    {
    iMonth = atoi(cstrDateOfBirth.Left(iIndex));
    iDay = atoi(cstrDateOfBirth.Mid(iIndex + 1, 2));
      // Check year.  It must be 4 digits and less than or equal to current year.
    if((iIndex = cstrDateOfBirth.ReverseFind('/')) > 0
    && (cstrDateOfBirth.GetLength() - iIndex - 1) == 4)
      {
      iYear = atoi(cstrDateOfBirth.Right(4));
      time_t tTime = time(NULL);
      struct tm *ptm;
      ptm = localtime(&tTime);
      if(iYear <= (ptm->tm_year + 1900))
        { // Year is OK.
        if(iMonth >= 1 && iMonth <= 12)
          { // valid month.
          if(iDay > 0)
            {
            switch(iMonth)
              {
              case 1:
              case 3:
              case 5:
              case 7:
              case 8:
              case 10:
              case 12:
                iMaxDay = 31;
                break;
              case 4:
              case 6:
              case 9:
              case 11:
                iMaxDay = 30;
                break;
              case 2:
                  // Check for leapyear
               if((iYear % 4 == 0 && (iYear % 100 != 0 || iYear % 400 == 0)) == 0)
                 iMaxDay = 28; // Not a leap year.
               else
                 iMaxDay = 29;
                break;
              }
            if(iDay <= iMaxDay)
              iRet = SUCCESS;
            }
          }
        }
      }
    }
  return(iRet);
  }

/****************************************************************************
fStd_dev - Calculate the mean and the standard deviation.

    inputs: Number of data values.
            Array containing the data values.
            Pointer to the mean that gets the calculated mean.

    return: The standard deviation.
            0 if there is less than 2 values.
****************************************************************************/
float fStd_dev(unsigned uNumVals, float *pfData, float *pfMean)
  {
  float fSumMean, fSumStdDev, fStdDev, fTemp;
  unsigned uCnt;

  fSumMean = 0.0F;
  fSumStdDev = 0.0F;
    // First calculate the mean.
  for(uCnt = 0; uCnt < uNumVals; ++uCnt)
    {
    fSumMean += *pfData;
    fSumStdDev += (*pfData * *pfData);
    ++pfData;
    }
  *pfMean = fSumMean / (float)uNumVals;
  if(uNumVals > 1)
    { // Standard deviation requires at least 2 data points.
      // Calculate the standard deviation.
    fTemp = (fSumStdDev - ((fSumMean * fSumMean) / (float)uNumVals)) / (float)(uNumVals - 1);
    fStdDev = (float)sqrt((double)fTemp);
    }
  else
    fStdDev = (float)0.0;
  return(fStdDev);
  }

/****************************************************************************
cstrGet_hrs_mins - Format a string containing the hours and minutes in the
                   following format:
                     hh:mm XM

    inputs: Reference to CTime object containing the date and time.

    return: CString object containing the hours and minutes in the above format.
****************************************************************************/
CString cstrGet_hrs_mins(CTime &ctTime)
  {
  CString cstrHrMin, cstrAMPM;
  int iHour;

  iHour = ctTime.GetHour();
  if(iHour >= 12)
    {
    //cstrAMPM.LoadString(IDS_PM);
    cstrAMPM = g_pLang->cstrLoad_string(ITEM_G_PM);
    iHour -= 12;
    }
  else
    cstrAMPM = g_pLang->cstrLoad_string(ITEM_G_AM);
    //cstrAMPM.LoadString(IDS_AM);
  cstrHrMin.Format("%02d:%02d %s", iHour, ctTime.GetMinute(), cstrAMPM);
  return(cstrHrMin);
  }

/********************************************************************
iCopy_one_file - Copy one file from the source to the destination.
                 Optionally ask to overwrite an existing file based on
                 the last input.
                 Display an error message on errors.

    inputs: Pointer to the destination path.
              It must NOT have a trailing backslash.
            Pointer to the source path.
              It must NOT have a trailing backslash.
            Pointer to the file name. If the pointer is NULL, both the destination
              and source path contain the file name.
            TRUE - fail if the file already exists.
              FALSE - automatically overwrite any exisiting files.

    return: SUCCESS if the file was copied.
            FAIL if there was an error or the file was not copied.
********************************************************************/
short int iCopy_one_file(LPCTSTR pstrDestPath, LPCTSTR pstrSrcPath, LPCTSTR pstrName,
BOOL bNoAutoOverwrite)
  {
  CString cstrSrc, cstrDest, cstrErrTitle, cstrMsg;
  BOOL bFailIfExists, bSts;
  short int iRet;
  char szMsg[256];
  DWORD dwErr;

  if(pstrName != NULL)
    { // There is a file name, append it to the path.
    cstrSrc.Format("%s\\%s", pstrSrcPath, pstrName);
    cstrDest.Format("%s\\%s", pstrDestPath, pstrName);
    }
  else
    {
    cstrSrc = pstrSrcPath;
    cstrDest = pstrDestPath;
    }
  bFailIfExists = bNoAutoOverwrite; // File copy fails if file already exists.
  while((bSts = CopyFile((LPCTSTR)cstrSrc, (LPCTSTR)cstrDest, bFailIfExists)) == 0)
    { // The copy failed.
    dwErr = GetLastError();
    //cstrErrTitle.LoadString(IDS_BACKUP_ERROR);
    cstrErrTitle = g_pLang->cstrLoad_string(ITEM_G_BACKUP_ERROR);
    if(dwErr == ERROR_NOT_READY)
      { // There is no floppy in the drive.  Ask user to put a floppy
        // in the drive or cancel the backup.
      //cstrMsg.LoadString(IDS_PUT_IN_DISK);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_PUT_IN_DISK);
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, 0L, szMsg, 256, NULL);
      cstrMsg.Insert(0, szMsg);
      if(AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErrTitle, MB_OKCANCEL | MB_ICONSTOP)
      == IDCANCEL)
        break; // User canceling backup.
      }
    else if(dwErr == ERROR_FILE_EXISTS)
      { // The file already exists on the backup, ask if user wants to
        // replace it or cancel.
      //cstrMsg.Format(IDS_OVERWRITE_FILE, cstrDest);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_OVERWRITE_FILE);
      if(AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErrTitle, MB_YESNO | MB_ICONSTOP)
      == IDNO)
        break; // User canceling backup.
      bFailIfExists = FALSE; // Allow the file copy to overwrite existing file.
      }
    else if(dwErr == ERROR_DISK_FULL)
      { // Disk is full.  Ask user to put another floppy in the drive 
        // or cancel the backup.
      //cstrMsg.LoadString(IDS_PUT_IN_NEW_DISK);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_PUT_IN_NEW_DISK);
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, 0L, szMsg, 512, NULL);
      cstrMsg.Insert(0, szMsg);
      if(AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErrTitle, MB_OKCANCEL | MB_ICONSTOP)
      == IDCANCEL)
        break; // User canceling backup.
      }
    else
      { // Got an error writing to the disk.  Ask the user if he wants to 
        // try again or cancel the backup.
      //cstrMsg.LoadString(IDS_DISK_TRY_AGAIN);
      cstrMsg = g_pLang->cstrLoad_string(ITEM_G_DISK_TRY_AGAIN);
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, 0L, szMsg, 256, NULL);
      cstrMsg.Insert(0, szMsg);
      if(AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErrTitle, MB_OKCANCEL | MB_ICONSTOP)
      == IDCANCEL)
        break; // User canceling backup.
       }
    }
  if(bSts == 0)
    iRet = FAIL;
  else
    iRet = SUCCESS;
  return(iRet);
  }

/********************************************************************
vFormat_patient_name - Format the patient name into one string.
                       If the research reference number is being used,
                       use that instead of the name.

    inputs: Pointer to PATIENT_DATA structure.
            Reference to a CString object that gets the formatted string.
            true if formatting the name for a report.

    return: None.
********************************************************************/
void vFormat_patient_name(PATIENT_DATA *ppdPatient, CString &cstrName, bool bForReport)
  {

  //if(ppdPatient->bUseResrchRefNum == TRUE)
  //  cstrName = ppdPatient->cstrResearchRefNum;
  //else
  //  {
  if(bForReport == true)
    {
    cstrName = "";
    if(ppdPatient->cstrFirstName != ""
      && g_pConfig->bGet_inc_rpt_item(INC_RPT_PATIENT_FIRST_NAME) == true)
      cstrName = ppdPatient->cstrFirstName;
    if(ppdPatient->cstrMI != ""
      && g_pConfig->bGet_inc_rpt_item(INC_RPT_PATIENT_MI) == true)
      {
      if(cstrName != "")
        cstrName += " ";
      cstrName += (ppdPatient->cstrMI + ".");
      }
    if(ppdPatient->cstrLastName != ""
      && g_pConfig->bGet_inc_rpt_item(INC_RPT_PATIENT_LAST_NAME) == true)
      {
      if(cstrName != "")
        cstrName += " ";
      cstrName += ppdPatient->cstrLastName;
      }
    }
  else
    {
    if(ppdPatient->cstrMI.IsEmpty() == FALSE)
      cstrName.Format("%s %s. %s", ppdPatient->cstrFirstName, ppdPatient->cstrMI,
                    ppdPatient->cstrLastName);
    else
      cstrName.Format("%s %s", ppdPatient->cstrFirstName, ppdPatient->cstrLastName);
    }
    //}
  }

/********************************************************************
fRound_down_to_half_min - Round a value down to the next half minute.
                          The input is never negative.

    inputs: Value to be rounded down.

    return: Rounded down value.
********************************************************************/
float fRound_down_to_half_min(float fStartValue)
  {
  float fEndValue, fFraction;

    // Drop any fractional values.
  fEndValue = (float)((int)fStartValue);
    // Look at fractional part.
  fFraction = fStartValue - fEndValue;
  if(fFraction >= (float)0.5)
    fEndValue += (float)0.5;
  return(fEndValue);
  }

/********************************************************************
cHex_to_ascii - Convert hex number, 0 to F to the Ascii representation.

inputs: Hex number, 0 to F.

return: Ascii representation.
********************************************************************/
char cHex_to_ascii(unsigned char ucHex)
  {
  char c;

  if(ucHex <= 9)
    c = ucHex + '0';
  else
    c = 'a' + ucHex - 0xA;
  return(c);
  }

/********************************************************************
ucAscii_to_Hex - Convert an Ascii number, '0' to 'F' to a hex number.

inputs: Ascii number, '0' to 'F'.

return: Hex number.
********************************************************************/
unsigned char ucAscii_to_Hex(char cAscii)
  {
  unsigned char ucHex;

  if(cAscii <= '9')
    ucHex = cAscii - '0';
  else
    ucHex = cAscii - 'a' + 0xA;
  return(ucHex);
  }

///********************************************************************
//iAscii_num_to_int - Convert an Ascii number to an integer between 0 and 9 inclusive.
//
//inputs: Ascii number, '0' to '9'.
//
//return: short integer.
//********************************************************************/
//short int iAscii_num_to_int(char cAscii)
//  {
//  short int iValue;
//
//  if(cAscii <= '9')
//    iValue = cAscii - '0';
//  else
//    iValue = 0;
//  return(iValue);
//  }


/********************************************************************
cstrGet_file_name - Get the file name from the complete path and file name.
                    This does not include the extension.

inputs: Path and file name.

return: File name.
********************************************************************/
CString cstrGet_file_name(CString cstrPathFile)
  {
  CString cstrFName;
  char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];

  _splitpath((LPCTSTR)cstrPathFile, szDrive, szDir, szFName, szExt);
  cstrFName = szFName;
  return(cstrFName);
  }

/********************************************************************
bIs_half_minute - Determine if the minutes include a half minute.

inputs: Minutes in period.

return: true if the minutes include a half minute.
********************************************************************/
bool bIs_half_minute(float fMin)
  {
  bool bRet;
  unsigned int uNumHalfMins;

  uNumHalfMins = (unsigned int)(fMin / 0.5F);
  if((uNumHalfMins % 2) != 0)
    bRet = true;
  else
    bRet = false;
  return(bRet);
  }

/********************************************************************
uGet_num_epochs - Get the number of epochs between the start and end
                  minutes.

inputs: Start minute.
        End minute.

return: Number of minutes between the start and end minute.
********************************************************************/
unsigned int uGet_num_epochs(float fStartMin, float fEndMin)
  {
  unsigned int uNumOfPoints; // uNumHalfMins, 

  //uNumOfPoints = ((unsigned)fEndMin - (unsigned)fStartMin - 3);
  uNumOfPoints = (unsigned)(fEndMin - fStartMin - 3);
  //uNumHalfMins = (unsigned int)(fStartMin / 0.5F);
  //if((uNumHalfMins % 2) != 0)
  if(bIs_half_minute(fStartMin) == true)
    uNumOfPoints *= 2; //--uNumOfPoints;
  return(uNumOfPoints);
  }

/********************************************************************
vEncode_char_in_string - Encode a character into specific locations in a string.

inputs: Character to encode.
        Reference to the string to put the character into.
        Index into the string for the high nibble of the character.
        Index into the string for the low nibble of the character.

return: None.
********************************************************************/
void vEncode_char_in_string(char c, CString &cstrDest, int iIndex, int iIndex1)
  {
  unsigned char ucItem;
  char cItem;

  ucItem = c - '0';
  cItem = (char)(((ucItem & 0xf0) >> 4) + '0'); // High nibble
  cstrDest.SetAt(iIndex, cItem);
  cItem = (char)((ucItem & 0xf) + '0'); // Low nibble
  cstrDest.SetAt(iIndex1, cItem);
  }

/********************************************************************
vEncode_byte_in_string - Encode a byte into a specific location in a string.

inputs: byte to encode.  This must be <= 15
        Reference to the string to put the character into.
        Index into the string for the byte.

return: None.
********************************************************************/
void vEncode_byte_in_string(unsigned char uc, CString &cstrDest, int iIndex)
  {

  if(uc < 0xa)
    uc += '0';
  else
    uc = (uc - 0xa) + 'A';
  cstrDest.SetAt(iIndex, uc);
  }

/********************************************************************
cDecode_char_from_string - Decode a character from  specific locations in a string.

inputs: Reference to the string to put the character into.
        Index into the string for the high nibble of the character.
        Index into the string for the low nibble of the character.

return: Character decoded from the string.
********************************************************************/
char cDecode_char_from_string(CString cstrSrc, int iIndex, int iIndex1)
  {
  unsigned char ucItem, ucItem1;
  char cItem;

    ucItem = cstrSrc[iIndex]; // High nibble
    ucItem -= '0';
    ucItem = ucItem << 4;
    ucItem1 = (cstrSrc[iIndex1] - '0'); // Low nibble
    cItem = (char)(ucItem | ucItem1) + '0';
    return(cItem);
  }

/********************************************************************
cstrCreate_guid - Create a unique GUID.

inputs: None.

return: string containing the GUID.
********************************************************************/
CString cstrCreate_guid(void)
  {
  GUID guid;    // generated GUID
  unsigned char *pszGUID;
  CString cstrGUID;

  //cstrValue.Empty();
  guid = GUID_NULL;
  HRESULT hr = ::UuidCreate(&guid);
  UuidToString(&guid, &pszGUID);
  cstrGUID = pszGUID;
  return(cstrGUID);
  }


/********************************************************************
cstrEncode_study_date - Encode a date into a GUID if the date is not already encoded.
                        The format for the date is: mm/dd/yyyy

inputs: string containing the date.

return: GUID string containing the encoded date.

     format:
      day 2 bytes, 1 to 31
        1: if day is less than 15 (0xf), 0
           else 15
        2: if day is less than 15, day
           else day - 15
      month is 1 byte 1 to 12
      year is 4 bytes
        1: 1000's
        2: 100's
        3: 10's
        4: 1's
      Location in GUID string
            9bff6d0a-e611-4e38-a
            01234567890123456789
              Dashes are 8, 13, 18
       day1:   14
       day2:   3
       month:  7
       year1:  10
       year2:  17
       year3:  19
       year4:  11
********************************************************************/
CString cstrEncode_study_date(CString cstrDate)
  {
  int iDay, iMonth, iYear;
  CString cstrGUID;

  if(cstrDate.GetLength() <= 10)
    { // Study date not encoded.
    // format: mm/dd/yyyy
    iMonth = atoi((LPCTSTR)cstrDate.Left(2));
    iDay = atoi((LPCTSTR)cstrDate.Mid(3, 2));
    iYear = atoi((LPCTSTR)cstrDate.Right(4));

    cstrGUID = cstrCreate_guid();
    cstrGUID = cstrGUID.Left(20);

    cstrGUID.SetAt(STUDY_DATE_DAY_1, cHex_to_ascii(iDay / 10));
    iDay %= 10;
    cstrGUID.SetAt(STUDY_DATE_DAY_2, cHex_to_ascii(iDay));

    cstrGUID.SetAt(STUDY_DATE_MONTH, cHex_to_ascii(iMonth));

    cstrGUID.SetAt(STUDY_DATE_YEAR_1, cHex_to_ascii(iYear / 1000));
    iYear -= (iYear / 1000) * 1000;
    cstrGUID.SetAt(STUDY_DATE_YEAR_2, cHex_to_ascii(iYear / 100));
    iYear -= (iYear / 100) * 100;
    cstrGUID.SetAt(STUDY_DATE_YEAR_3, cHex_to_ascii(iYear / 10));
    iYear -= (iYear / 10) * 10;
    cstrGUID.SetAt(STUDY_DATE_YEAR_4, cHex_to_ascii(iYear));
    }
  else
    cstrGUID = cstrDate;
  return(cstrGUID);
  }

/********************************************************************
cstrDecode_study_date - Decode a date from a GUID string.
                        The decode date will have the format: mm/dd/yyyy

inputs: GUID string containing the encoded date.

return: String containing the decoded date.
********************************************************************/
CString cstrDecode_study_date(CString cstrDate)
  {
  int iDay, iMonth, iYear;
  CString cstrDecodedDate;

  if(cstrDate.GetLength() > 10)
    { // Date is encoded.
    iDay = (ucAscii_to_Hex(cstrDate.GetAt(STUDY_DATE_DAY_1)) * 10)
      + ucAscii_to_Hex(cstrDate.GetAt(STUDY_DATE_DAY_2));
    iMonth = ucAscii_to_Hex(cstrDate.GetAt(STUDY_DATE_MONTH));
    iYear = (ucAscii_to_Hex(cstrDate.GetAt(STUDY_DATE_YEAR_1)) * 1000)
      + (ucAscii_to_Hex(cstrDate.GetAt(STUDY_DATE_YEAR_2)) * 100)
      + (ucAscii_to_Hex(cstrDate.GetAt(STUDY_DATE_YEAR_3)) * 10)
      + ucAscii_to_Hex(cstrDate.GetAt(STUDY_DATE_YEAR_4));

    // format: mm/dd/yyyy
    cstrDecodedDate.Format("%02d/%02d/%04d", iMonth, iDay, iYear);
    }
  else
    cstrDecodedDate = cstrDate;
  return(cstrDecodedDate);
  }

/********************************************************************
vSet_file_date_time - Change the file date/time to a predefined date
                      and time.  This is to hide the real date/time
                      which is when the study was done.

inputs: File name

return: None.
********************************************************************/
void vSet_file_date_time(CString cstrFile)
  {
  SYSTEMTIME systime;
  FILETIME filetime;
  HANDLE hFindFile;
  char szMsg[1024];

  systime.wDay = 1;
  systime.wHour = 1;
  systime.wMilliseconds = 0;
  systime.wMinute = 1;
  systime.wMonth = 1;
  systime.wSecond = 1;
  systime.wYear = 2000;

  if(SystemTimeToFileTime(&systime, &filetime) == TRUE)
    {
    //get the handle to the file
    hFindFile = CreateFile((LPCTSTR)cstrFile,   
      FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
      FILE_ATTRIBUTE_NORMAL, NULL);
    //set the filetime on the file
    if(SetFileTime(hFindFile, &filetime, &filetime, &filetime) == FALSE)
      {
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0L, szMsg, 1024, NULL);
      //MessageBox(szMsg, "Error", MB_ICONERROR);
      }

    //close the handle.
    CloseHandle(hFindFile);
    }
  }

//unsigned char ascii_to_hex(char cChar)
//{
//   unsigned char hundred, ten, unit, value;
//
//   hundred = (cChar-0x30)*100;
//   ten = ((cChar + 1)-0x30)*10;
//   unit = (cChar+2)-0x30;     
//
//   value = (hundred + ten + unit);
//   //printf("\nValue: %#04x \n", value);
//
//   return value; 
//}

//CString cstrEncode(CString cstrKey, CString cstrText)
//{
//char cKey, cEncode;
//CString cstrResult;
//
//cstrResult.Empty();
//for(int i = 0; i < cstrText.GetLength(); ++i)
//  {
//  cKey = (char)cstrKey.GetAt(i % cstrKey.GetLength());
//  cEncode = (char)(ascii_to_hex((char)cstrText.GetAt(i)) + ascii_to_hex(cKey) % 256);
//  cstrResult += cEncode;
//  }
//return(cstrResult);
//}


/*******************

def encode(key, string):
    encoded_chars = []
    for i in xrange(len(string)):
        key_c = key[i % len(key)]
        encoded_c = chr(ord(string[i]) + ord(key_c) % 256)
        encoded_chars.append(encoded_c)
    encoded_string = "".join(encoded_chars)
    return base64.urlsafe_b64encode(encoded_string)

    Decode is pretty much the same, except you subtract the key.

********************/
