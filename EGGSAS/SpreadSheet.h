/********************************************************************
SpreadSheet.h

Copyright (C) 2007,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CSpreadSheet class.

HISTORY:
28-JUL-07  RET  New.
21-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/
#pragma once

// COMMENT THIS OUT IF CREATING A TEXT FILE FOR IMPORTING INTO A SPREADSHEET.
//#define USE_SPREADSHEET_PROGRAM

#ifdef USE_SPREADSHEET_PROGRAM
// Excel headers.
#include "CApplication.h"
#include "CWorkbooks.h"
#include "CWorkbook.h"
#include "CWorksheets.h"
#include "CWorksheet.h"
#include "CRange.h"
// End of Excel headers

//  Name of spreadsheet template.
#define SPREADSHEET_NAME "DataSheet.xls"

// Defines for positioning in the spreadsheet
#define START_COLUMN 'B'
#define START_ROW 7
#define LAST_COLUMN 'G'
#endif

#define LAST_COLUMN 7

#define MIN_ROWS_DIST_AVG_PWR_FREQ_REG 3
#define MIN_ROWS_RATIO_AVG_PWR 2
#define MIN_ROWS_DIST_AVG_PWR_FREQ_RANGE 2
#define MIN_ROWS_AVG_DOM_FREQ 2
#define MIN_ROWS_PCT_TIME 2

class CSpreadSheet
  {
  public:
    CSpreadSheet(CEGGSASDoc *pDoc);
    virtual ~CSpreadSheet(void);

  // Data
  CEGGSASDoc *m_pDoc; // Pointer to the current document.
  ANALYZED_DATA m_adData; // Hold information and data for analysis.
  unsigned short m_uMaxPeriods; // Maximum number of periods (timebands).
  CFile *m_pcfSSFile; // Pointer to the opened spreadsheet file.
  CString m_cstrFileName; // File name for spreadsheet file.
  CString m_cstrSSPathFile; // Path and file name for spreadsheet file.

#ifdef USE_SPREADSHEET_PROGRAM
  CApplication m_appExcel;
  CWorkbooks m_Books;
  CWorkbook m_Book;
  CWorksheets m_Sheets;
  CWorksheet m_Sheet;
#endif

  // Methods
  void vFree_data_sheet_arrays();
  void vAnalyze_data();
  bool bSelect_path();
  bool bCreate_file();
  short int iWrite_to_ss(CString cstrLine, bool bEmptyLine);
  void vCreate_data_sheet();
  CString cstrGet_timeband_start_end_min(int iIndex);
  short int iAdd_left_title(int iColTitleIndex);
  CString cstrGet_col_title(unsigned int uiID);

#ifdef USE_SPREADSHEET_PROGRAM
  void vCreate_data_sheet();
  void vAdd_left_title(int iColTitleIndex, int iRow);
#endif
  };
