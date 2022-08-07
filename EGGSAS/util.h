/********************************************************************
util.h

Copyright (C) 2003,2004,2005,2008,2011,2016 3CPM Company, Inc.  All rights reserved.

  Header file for the util.cpp file.

HISTORY:
03-OCT-03  RET  New.
06-FEB-05  RET  Add prototype for function vFormat_patient_name().
30-JUN-08 RET  Version 2.05
                 Add prototype for cstrReformat_date().
25-FEB-11  RET
                Add prototypes for funcions: ucAscii_to_Hex(), cHex_to_ascii()
02-JUN-16  RET
             Add feature to hide study dates.
               Add prototypes for functions:
                 vEncode_char_in_string(), vEncode_byte_in_string(),
                 cDecode_char_from_string(), cstrCreate_guid(),
                 cstrEncode_study_date(),cstrDecode_study_date(),
                 vSet_file_date_time()
********************************************************************/

long lCalc_font_height(int iPointSize, CDC *pcdc);
void vSet_font(CWnd *pwnd, CDC *pDC, CFont *pcfFont, long lFontWeight,
               long lFontSize = 0);

CTime ctDate_to_ctime(CString cstrDate);
void vCenter_button_horiz(CWnd *pwndDlg, CWnd *pwndButton);

unsigned uFround(float fNum);
short int iValidate_date(CString cstrDateOfBirth);
float fStd_dev(unsigned uNumVals, float *pfData, float *pfMean);
CString cstrGet_hrs_mins(CTime &ctTime);
short int iCopy_one_file(LPCTSTR pstrDestPath, LPCTSTR pstrSrcPath, LPCTSTR pstrName,
                         BOOL bNoAutoOverwrite);
void vFormat_patient_name(PATIENT_DATA *ppdPatient, CString &cstrName, bool bForReport);
float fRound_down_to_half_min(float fStartValue);
CString cstrReformat_date(CString cstrDate);
char cHex_to_ascii(unsigned char ucHex);
unsigned char ucAscii_to_Hex(char cAscii);
CString cstrGet_file_name(CString cstrPathFile);
unsigned int uGet_num_epochs(float fStartMin, float fEndMin);
bool bIs_half_minute(float fMin);

void vEncode_char_in_string(char c, CString &cstrDest, int iIndex, int iIndex1);
char cDecode_char_from_string(CString cstrSrc, int iIndex, int iIndex1);
CString cstrCreate_guid(void);
CString cstrEncode_study_date(CString cstrDate);
CString cstrDecode_study_date(CString cstrDate);
//short int iAscii_num_to_int(char cAscii);
void vSet_file_date_time(CString cstrFile);

//unsigned char ascii_to_hex(char cChar);
//CString cstrEncode(CString cstrKey, CString cstrText);






