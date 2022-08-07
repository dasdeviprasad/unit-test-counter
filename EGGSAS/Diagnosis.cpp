/********************************************************************
Diagnosis.cpp

Copyright (C) 2003,2004,2005,2007,2009,2012,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CDiagnosis class.

  This class examines the study results and produces a suggested
  diagnosis.


HISTORY:
08-NOV-03  RET  New.
16-MAR-04  RET  Add help button and message handler.
21-APR-04  RET  Change iDiagnose() to use the low control range for 3cpm
                  to determine if signal is above or below normal.
01-MAR-05  RET  Change iDiagnose_postwater() to look for Tachygastria and
                  Bradygastria, then look for duodenal activity.
01-JUL-07  RET  Research version
                  Can only enter a diagnosis.  There is no suggested
                    diagnosis.
14-JAN-09  RET  Version CDDRdr.2.08c
                  Display only if view mode.  Change Constructor.
                  Change vGet_diagnosis() to return the original data
                    from the CDoc class in View mode.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
24-AUG-12  RET
             Add new fields for diagnosis and ICD codes.
               For Diagnosis: "Add to report" button
                              "Save description to list" button 
                              "Remove from list" button 
                              Combobox to list diagnoses titles.
               For ICD codes: "Include ICD Code" button 
                              "Save ICD Code to list" button 
                              "Remove from list" button 
                              Combobox to list ICD codes.
               Change methods: vSet_text()
               Add event handlers: OnBnClickedBtnAddToReport(), 
                 OnBnClickedBtnSaveDiagToList(), OnBnClickedBtnRemoveFromList(),
                 OnCbnSelchangeCbxDiagList(), OnBnClickedBtnIncludeIcdCode(),
                 OnBnClickedBtnSaveIcdToList(), OnBnClickedBtnRemoveIcdFromList(),
                 OnCbnSelchangeCbxIcdList()
27-AUG-12  RET
             Add code to implement Diagnosis ID and description.
               Change methods: OnInitDialog(), vGet_diagnosis(), vSet_diagnosis(),
                 bChanged()
               Add methods: vEnable_diagdesc_icd_fields(), vFill_diag_desc_id()
31-AUG-12  RET
             Implement ICD codes.
               Change methods: OnInitDialog(), vGet_diagnosis(), vSet_diagnosis(),
                 bChanged()
20-SEP-12  RET
             Redo how saving diagnosis descriptions and ICD codes work based on
             bug found during test.
               Change methods: OnBnClickedBtnSaveDiagToList(), OnBnClickedBtnSaveIcdToList()
             For listing diagnosis key phrases and ICD codes, instead of
             empty line, use "Add new key phrase" and "Add new ICD code" respectively.
               Change methods: vFill_diag_desc_id(), vFill_icd_code()
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedDiagnosisHelp()


Notes:
  On entry: 
    - display both the suggested and user diagnosis.
    - If diagnosis key phrase, display that in the dropdown listbox.
    - If ICD code, append to diagnosis box and select in the ICD code
      dropdown listbox.
  If user selects Accept, append the suggested diagnosis to the diagnosis box
    on a new line.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "Dbaccess.h"
#include "analyze.h"
#include "Diagnosis.h"
#include "util.h"
#include ".\diagnosis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern float g_fControl[CONTROL_MIN_HI_LO][CONTROL_FREQ_BANDS];

/////////////////////////////////////////////////////////////////////////////
// CDiagnosis dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the currently active document.
          Where the diaglog box is being called from (edit, view, etc).

********************************************************************/
CDiagnosis::CDiagnosis(CEGGSASDoc *pDoc, unsigned uFrom, CWnd* pParent /*=NULL*/)
	: CDialog(CDiagnosis::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CDiagnosis)
	m_cstrSuggestedDiagnosis = _T("");
	m_cstrDiagnosis = _T("");
	m_iDiagAction = -1;
	//}}AFX_DATA_INIT

  m_pDoc = pDoc;
#if EGGSAS_READER == READER_DEPOT
  m_uFrom = SM_VIEW;
#else
  m_uFrom = uFrom;
#endif
  m_bUserDiagnosis = FALSE;
  }


void CDiagnosis::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDiagnosis)
  DDX_Text(pDX, IDC_SUGGESTED_DIAGNOSIS, m_cstrSuggestedDiagnosis);
  DDX_Text(pDX, IDC_DIAGNOSIS, m_cstrDiagnosis);
  DDX_Radio(pDX, IDC_ACCEPT_DIAGNOSIS, m_iDiagAction);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_CBX_DIAG_LIST, m_cDiagList);
  DDX_Control(pDX, IDC_CBX_ICD_LIST, m_cICDList);
}


BEGIN_MESSAGE_MAP(CDiagnosis, CDialog)
	//{{AFX_MSG_MAP(CDiagnosis)
	ON_BN_CLICKED(IDC_ACCEPT_DIAGNOSIS, OnAcceptDiagnosis)
	ON_BN_CLICKED(IDC_MODIFY_DIAGNOSIS, OnModifyDiagnosis)
	ON_BN_CLICKED(IDC_REJECT_DIAGNOSIS, OnRejectDiagnosis)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_DIAGNOSIS_HELP, OnBnClickedDiagnosisHelp)
  ON_BN_CLICKED(IDC_BTN_ADD_TO_REPORT, OnBnClickedBtnAddToReport)
  ON_BN_CLICKED(IDC_BTN_SAVE_DIAG_TO_LIST, OnBnClickedBtnSaveDiagToList)
  ON_BN_CLICKED(IDC_BTN_REMOVE_FROM_LIST, OnBnClickedBtnRemoveFromList)
  ON_CBN_SELCHANGE(IDC_CBX_DIAG_LIST, OnCbnSelchangeCbxDiagList)
  ON_BN_CLICKED(IDC_BTN_INCLUDE_ICD_CODE, OnBnClickedBtnIncludeIcdCode)
  ON_BN_CLICKED(IDC_BTN_SAVE_ICD_TO_LIST, OnBnClickedBtnSaveIcdToList)
  ON_BN_CLICKED(IDC_BTN_REMOVE_ICD_FROM_LIST, OnBnClickedBtnRemoveIcdFromList)
  ON_CBN_SELCHANGE(IDC_CBX_ICD_LIST, OnCbnSelchangeCbxIcdList)
//  ON_CBN_EDITCHANGE(IDC_CBX_DIAG_LIST, OnCbnEditchangeCbxDiagList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiagnosis message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Modify the fields for editing/viewing.
********************************************************************/
BOOL CDiagnosis::OnInitDialog() 
  {
  CString cstrTemp;
  short int iDiagnosis;
  DIAGNOSIS_DESC_RECORD ddrRecord;
  ICD_CODE_RECORD icdRecord;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();
    // Set up the dialog box fields for view/edit/new study
  if(m_uFrom == SM_VIEW)
    { // View only
    GetDlgItem(IDC_SUGGESTED_DIAGNOSIS)->EnableWindow(FALSE);
	  GetDlgItem(IDC_DIAGNOSIS)->EnableWindow(FALSE);
	  GetDlgItem(IDC_ACCEPT_DIAGNOSIS)->ShowWindow(SW_HIDE);
	  GetDlgItem(IDC_MODIFY_DIAGNOSIS)->ShowWindow(SW_HIDE);
	  GetDlgItem(IDC_REJECT_DIAGNOSIS)->ShowWindow(SW_HIDE);
    vEnable_diagdesc_icd_fields(FALSE);
	  
    GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
    vCenter_button_horiz(this, GetDlgItem(IDOK)); // Center the OK button.
    //vDisplay_diagnosis();
    //vDisplay_ICD_code();
    }
  else if(m_uFrom == SM_EDIT)
    { // Edit
    if(theApp.m_ptStudyType == PT_STANDARD) // && theApp.m_iFreqLimit == FREQ_LIMIT_15_CPM)
      { // Standard study
      if(m_bUserDiagnosis == TRUE)
        GetDlgItem(IDC_SUGGESTED_DIAGNOSIS)->EnableWindow(FALSE);
      }
    else
      { // Research study
      m_bUserDiagnosis = TRUE;
      GetDlgItem(IDC_SUGGESTED_DIAGNOSIS)->EnableWindow(FALSE);
	    GetDlgItem(IDC_ACCEPT_DIAGNOSIS)->EnableWindow(FALSE);
	    GetDlgItem(IDC_MODIFY_DIAGNOSIS)->EnableWindow(FALSE);
  	  GetDlgItem(IDC_REJECT_DIAGNOSIS)->EnableWindow(FALSE);
        // Set the focus to the diagnosis.
      GetDlgItem(IDC_DIAGNOSIS)->SetFocus();
      }
    //vDisplay_diagnosis();
    //vDisplay_ICD_code();
    }
  else
    { // New study
    if(theApp.m_ptStudyType == PT_STANDARD)
      { // Standard study
      if(m_pDoc->m_pdPatient.bManualMode == FALSE)
        { // Perform an analysis on all periods.
        if(iPerform_analysis() == SUCCESS)
          { // Do the diagnosis.
          iDiagnosis = iDiagnose();
				  m_cstrSuggestedDiagnosis = cstrGet_diagnosis(iDiagnosis);
          vEnable_diagdesc_icd_fields(FALSE);
          }
        }
      else
        { // In manual mode, hide the accept/modify/reject buttons.
	      GetDlgItem(IDC_ACCEPT_DIAGNOSIS)->ShowWindow(SW_HIDE);
	      GetDlgItem(IDC_MODIFY_DIAGNOSIS)->ShowWindow(SW_HIDE);
  		  GetDlgItem(IDC_REJECT_DIAGNOSIS)->ShowWindow(SW_HIDE);
        }
      }
    else
      { // Research study
      GetDlgItem(IDC_SUGGESTED_DIAGNOSIS)->EnableWindow(FALSE);
	    GetDlgItem(IDC_ACCEPT_DIAGNOSIS)->EnableWindow(FALSE);
	    GetDlgItem(IDC_MODIFY_DIAGNOSIS)->EnableWindow(FALSE);
  	  GetDlgItem(IDC_REJECT_DIAGNOSIS)->EnableWindow(FALSE);
      }
    }
  m_iDiagAction = -1; // Default to with no buttons selected so user
                      // must select a button.

  // Fill the diagnostic description ID list.
  vFill_diag_desc_id();

  // Select the ID from the Study.
  // Get the diagnosis ID from the study table.  If none (older studies) or
  // if can't find it, select the first entry in the combbox (empty line).
  ddrRecord.cstrDiagnosisKeyPhrase = m_pDoc->m_pdPatient.cstrDiagnosisKeyPhrase;
  if(m_pDoc->m_pDB->iFind_diagnosis_desc(&ddrRecord) == SUCCESS)
    m_iDiagnosisSelIndex = m_cDiagList.SelectString(0, m_pDoc->m_pdPatient.cstrDiagnosisKeyPhrase);
  else
    m_iDiagnosisSelIndex = 0;
  if(m_iDiagnosisSelIndex >= 0)
    m_cDiagList.SetCurSel(m_iDiagnosisSelIndex);
  m_cstrOrigDiagnosisKeyPhrase = ddrRecord.cstrDiagnosisKeyPhrase;
  
  // Fill the ICD list.
  vFill_icd_code();
                        
  // Select the ICD from the Study.
  // Get the diagnosis ID from the study table.  If none (older studies) or
  // if can't find it, select the first entry in the combbox (empty line).
  icdRecord.cstrICDCode = m_pDoc->m_pdPatient.cstrICDCode;
  if(m_pDoc->m_pDB->iFind_ICDCode(&icdRecord) == SUCCESS)
    m_iICDCodeSelIndex = m_cICDList.SelectString(0, m_pDoc->m_pdPatient.cstrICDCode);
  else
    m_iICDCodeSelIndex = 0;
  if(m_iICDCodeSelIndex >= 0)
    m_cICDList.SetCurSel(m_iICDCodeSelIndex);
  m_cstrOrigICDCode = icdRecord.cstrICDCode;
  
  UpdateData(FALSE);
	return FALSE;  // return TRUE unless you set the focus to a control
//	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnOK

  Message handler for the OK button.
********************************************************************/
void CDiagnosis::OnOK() 
  {

  UpdateData(TRUE);
	CDialog::OnOK();
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.
********************************************************************/
void CDiagnosis::OnCancel() 
  {
	
	CDialog::OnCancel();
  }

/********************************************************************
OnAcceptDiagnosis

  Message handler for the Accept button.
    Update the class variable for the button.
********************************************************************/
void CDiagnosis::OnAcceptDiagnosis() 
  {
  UpdateData(TRUE);
  m_bUserDiagnosis = FALSE;
	m_cstrDiagnosis = m_cstrSuggestedDiagnosis;
  GetDlgItem(IDC_SUGGESTED_DIAGNOSIS)->EnableWindow(TRUE);
  GetDlgItem(IDC_DIAGNOSIS)->EnableWindow(FALSE);
  
  vEnable_diagdesc_icd_fields(FALSE);
  
  UpdateData(FALSE);
  }

/********************************************************************
OnModifyDiagnosis

  Message handler for the Modify button.
    Update the class variable for the button.
    Copy the suggested diagnosis to the diagnosis field.
********************************************************************/
void CDiagnosis::OnModifyDiagnosis() 
  {
	
  UpdateData(TRUE);
	//m_cstrDiagnosis = m_cstrSuggestedDiagnosis;
  m_bUserDiagnosis = TRUE;
  GetDlgItem(IDC_SUGGESTED_DIAGNOSIS)->EnableWindow(FALSE);
  GetDlgItem(IDC_DIAGNOSIS)->EnableWindow(TRUE);
  
  vEnable_diagdesc_icd_fields(TRUE);
  vDisplay_diagnosis();
  
  UpdateData(FALSE);
  }

/********************************************************************
OnRejectDiagnosis

  Message handler for the Reject button.
    Update the class variable for the button.
    Clear to the diagnosis field.
********************************************************************/
void CDiagnosis::OnRejectDiagnosis() 
  {
	
  UpdateData(TRUE);
	m_cstrDiagnosis = m_cstrSuggestedDiagnosis;
  m_bUserDiagnosis = TRUE;
  GetDlgItem(IDC_SUGGESTED_DIAGNOSIS)->EnableWindow(FALSE);
  GetDlgItem(IDC_DIAGNOSIS)->EnableWindow(TRUE);
    
  vEnable_diagdesc_icd_fields(TRUE);
  vDisplay_diagnosis();

  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedDiagnosisHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CDiagnosis::OnBnClickedDiagnosisHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_DIAGNOSIS_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_DIAGNOSIS_HELP);
  }

/////////////////////////////////////////////////////////////////////
//// Support Methods

/********************************************************************
iPerform_analysis - Do an analysis on the data for purposes of determining
                    the suggested diagnosis.

  inputs: None.

  return: SUCCESS if the analysis is successful.
          FAIL if there was an error.
********************************************************************/
short int CDiagnosis::iPerform_analysis()
  {
  REQ_ANALYZE_DATA reqAnlData;
  short int iIndex, iRet = SUCCESS;
  unsigned short uSize, iMaxFreqPts;
  float fLastMinute, fStartMinute;

    // First find out how many data points will be needed and allocate an array.
    // total CPM * 4 so that we get four points for each cpm.
  iMaxFreqPts = ((15 - 0) * 4) + 1;
  uSize = 0;
  for(iIndex = 0; iIndex < m_pDoc->m_uMaxPeriods; ++iIndex)
    {
    if(m_pDoc->m_pdPatient.fGoodMinStart[iIndex] != INV_MINUTE)
      { // Now find out how many data points for this period.
        // Add and extra 2 for the blanks lines separating pre and post water periods.
      uSize += (((unsigned short)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
                - (unsigned short)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 2 + 2)
                * iMaxFreqPts);
      }
    else
      iRet = FAIL;
    }
  if(iRet == SUCCESS)
    {
    m_adData.pfRawData = NULL;
    m_adData.pfPwrSum = NULL;
    for(iIndex = 0; iIndex < m_pDoc->m_uMaxPeriods * MAX_FREQ_BANDS_DIAG; ++iIndex)
      m_adData.fAvgPwr[iIndex] = 0.0F;

      // For each period that has good minutes defined.
    for(iIndex = 0; iIndex < m_pDoc->m_uMaxPeriods; ++iIndex)
      {
        { // Now for each valid period, analyze all the 4 minute epochs.
        // Read in and graph the baseline period.
        if(m_pDoc->iRead_period(iIndex, m_pDoc->m_fData) == SUCCESS)
          {
          reqAnlData.iChannel = 0; // EGG channel.
          reqAnlData.iNumChannelsInFile = 2;
          reqAnlData.iType = DIAGNOSIS_TYPE;
          reqAnlData.padData = &m_adData;
          reqAnlData.bUsePartMinAtDataEnd = false;
          reqAnlData.uTimeBand = iIndex;
          fLastMinute = m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] - 4;
          fStartMinute = m_pDoc->m_pdPatient.fGoodMinStart[iIndex];
          while(fStartMinute <= fLastMinute)
            {
            if(theApp.m_ptStudyType == PT_STANDARD)
              reqAnlData.fEpochNum = fStartMinute; // Standard study
            else // Research study
              reqAnlData.fEpochNum = fStartMinute - m_pDoc->m_pdPatient.fPeriodStart[iIndex];
            reqAnlData.uRunningEpochCnt = 0;
            if(m_pDoc->m_pAnalyze->bAnalyze_data(&reqAnlData) == true)
              { // Save the data for graphing.
//              memcpy((unsigned char *)pfData, adData.fData, iMaxFreqPts * sizeof(float));
//              pfData += m_iMaxFreqPts;
              }
            else
              {
              iRet = FAIL;
              break;
              }
            ++fStartMinute;
            }

  unsigned int uNumHalfMins; //, uNumOfPoints;

  reqAnlData.uRunningEpochCnt = ((unsigned)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
     - (unsigned)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 2);
  uNumHalfMins = (unsigned int)(m_pDoc->m_pdPatient.fGoodMinStart[iIndex] / 0.5F);
  if((uNumHalfMins % 2) != 0)
    --reqAnlData.uRunningEpochCnt;

          //reqAnlData.uRunningEpochCnt = (unsigned short)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
          //                               - (unsigned short)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 2;
          m_pDoc->m_pAnalyze->vCalc_diagnosis(&reqAnlData);
          }
        else
          {
          iRet = FAIL; // Reading data file failed
          break;
          }
        }
      }
    }
  return(iRet);
  }

/////////////////////////////////////////////////////////////////////
// The diagnosis routines are based on the document: Diagnostic Aid.doc

/********************************************************************
iDiagnose - Determine the suggested diagnosis based on the document:
            Diagnostic Aid.doc.

  inputs: None.

  return: Indicator of the suggested diagnosis.
********************************************************************/
short int CDiagnosis::iDiagnose()
  {
//  float *pfStart;
  short int iRet;

/************* Out for version II
    // Determine pre-water values.
    // Set all values to NORMAL or 0.
  memset(m_uPreWater, 0, sizeof(m_uPreWater));
  if(m_adData.fPctGrp1[0] > g_fControl[1][0])
    m_uPreWater[RI_BRADY] = ABOVE_NORM;  // bradygastria
  if(m_adData.fPctGrp1[MAX_FREQ_BANDS] > g_fControl[3][0])
    m_uPreWater[RI_3CPM] = ABOVE_NORM; // normal
  if(m_adData.fPctGrp1[(2 * MAX_FREQ_BANDS)] > g_fControl[5][0])
    m_uPreWater[RI_TACHY] = ABOVE_NORM; // tachygastria
  if(m_adData.fPctGrp1[(3 * MAX_FREQ_BANDS)] > g_fControl[7][0])
    m_uPreWater[RI_DUOD] = ABOVE_NORM; // duodenal/respiratory
  if((iRet = iDiagnose_prewater()) == DIAG_NORMAL || iRet == DIAG_POS_GAS_OUT_OBS)
*****************/
    { // Pre-water is normal or possible gastric outlet obstruction, check post-water
      // Determine post-water values.
      // Set all values to NORMAL or 0.
    memset(m_uPostWater10, 0, sizeof(m_uPostWater10));
    memset(m_uPostWater20, 0, sizeof(m_uPostWater20));
    memset(m_uPostWater30, 0, sizeof(m_uPostWater30));
      // 10 minute
//    pfStart = m_adData.pfPctGrp1 + 1;
    if(m_adData.fPctGrp1[0 + MAX_FREQ_BANDS_DIAG] > g_fControl[1][1])
      m_uPostWater10[RI_BRADY] = ABOVE_NORM;  // bradygastria
    if(m_adData.fPctGrp1[1 + MAX_FREQ_BANDS_DIAG] > g_fControl[3][1])
      m_uPostWater10[RI_3CPM] = ABOVE_NORM;  // normal
    else if(m_adData.fPctGrp1[1 + MAX_FREQ_BANDS_DIAG] < g_fControl[2][1])
      m_uPostWater10[RI_3CPM] = BELOW_NORM;  // normal
    if(m_adData.fPctGrp1[2 + MAX_FREQ_BANDS_DIAG] > g_fControl[5][1])
      m_uPostWater10[RI_TACHY] = ABOVE_NORM; // tachygastria
    if(m_adData.fPctGrp1[3 + MAX_FREQ_BANDS_DIAG] > g_fControl[7][1])
      m_uPostWater10[RI_DUOD] = ABOVE_NORM; // duodenal/respiratory
      // 20 minute
//    pfStart = m_adData.pfPctGrp1 + 2;
    if(m_adData.fPctGrp1[0 + (2 * MAX_FREQ_BANDS_DIAG)] > g_fControl[1][2])
      m_uPostWater20[RI_BRADY] = ABOVE_NORM;  // bradygastria
    if(m_adData.fPctGrp1[1 + (2 * MAX_FREQ_BANDS_DIAG)] > g_fControl[3][2])
      m_uPostWater20[RI_3CPM] = ABOVE_NORM;  // normal
    else if(m_adData.fPctGrp1[1 + (2 * MAX_FREQ_BANDS_DIAG)] < g_fControl[2][2])
      m_uPostWater20[RI_3CPM] = BELOW_NORM;  // normal
    if(m_adData.fPctGrp1[2 + (2 * MAX_FREQ_BANDS_DIAG)] > g_fControl[5][2])
      m_uPostWater20[RI_TACHY] = ABOVE_NORM; // tachygastria
    if(m_adData.fPctGrp1[3 + (2 * MAX_FREQ_BANDS_DIAG)] > g_fControl[7][2])
      m_uPostWater20[RI_DUOD] = ABOVE_NORM; // duodenal/respiratory
      // 30 minute
//    pfStart = m_adData.pfPctGrp1 + 3;
    if(m_adData.fPctGrp1[0 + (3 * MAX_FREQ_BANDS_DIAG)] > g_fControl[1][3])
      m_uPostWater30[RI_BRADY] = ABOVE_NORM;  // bradygastria
    if(m_adData.fPctGrp1[1 + (3 * MAX_FREQ_BANDS_DIAG)] > g_fControl[3][3])
      m_uPostWater30[RI_3CPM] = ABOVE_NORM;  // normal
    else if(m_adData.fPctGrp1[1 + (3 * MAX_FREQ_BANDS_DIAG)] < g_fControl[2][3])
      m_uPostWater30[RI_3CPM] = BELOW_NORM;  // normal
    if(m_adData.fPctGrp1[2 + (3 * MAX_FREQ_BANDS_DIAG)] > g_fControl[5][3])
      m_uPostWater30[RI_TACHY] = ABOVE_NORM; // tachygastria
    if(m_adData.fPctGrp1[3 + (3 * MAX_FREQ_BANDS_DIAG)] > g_fControl[7][3])
      m_uPostWater30[RI_DUOD] = ABOVE_NORM; // duodenal/respiratory

    iRet = iDiagnose_postwater();
    }


  return(iRet);
  }

/********************************************************************
iDiagnose_postwater - Determine the suggested diagnosis using the
                      post-water periods based on the document:
                      Diagnostic Aid.doc.

  inputs: None.

  return: Indicator of the suggested diagnosis.
********************************************************************/
short int CDiagnosis::iDiagnose_postwater()
  {
  short unsigned uNumTachyAboveNorm, uNumBradyAboveNorm, uNumDuodAboveNorm;
  short unsigned uNum3cpmBelowNorm, uNum3cpmAboveNorm, uRespRate, uPeriodCnt;
//  short int iRespRate, iRespHighCnt;
  short int iRet = DIAG_NONE;

  uNumTachyAboveNorm = 0;
  if(m_uPostWater10[RI_TACHY] == ABOVE_NORM)
    ++uNumTachyAboveNorm;
  if(m_uPostWater20[RI_TACHY] == ABOVE_NORM)
    ++uNumTachyAboveNorm;
  if(m_uPostWater30[RI_TACHY] == ABOVE_NORM)
    ++uNumTachyAboveNorm;

  uNumBradyAboveNorm = 0;
  if(m_uPostWater10[RI_BRADY] == ABOVE_NORM)
    ++uNumBradyAboveNorm;
  if(m_uPostWater20[RI_BRADY] == ABOVE_NORM)
    ++uNumBradyAboveNorm;
  if(m_uPostWater30[RI_BRADY] == ABOVE_NORM)
    ++uNumBradyAboveNorm;

  uNum3cpmBelowNorm = 0;
  uNum3cpmAboveNorm = 0;
  if(m_uPostWater10[RI_3CPM] == ABOVE_NORM)
    ++uNum3cpmAboveNorm;
  else if(m_uPostWater10[RI_3CPM] == BELOW_NORM)
    ++uNum3cpmBelowNorm;
  if(m_uPostWater20[RI_3CPM] == ABOVE_NORM)
    ++uNum3cpmAboveNorm;
  else if(m_uPostWater20[RI_3CPM] == BELOW_NORM)
    ++uNum3cpmBelowNorm;
  if(m_uPostWater30[RI_3CPM] == ABOVE_NORM)
    ++uNum3cpmAboveNorm;
  else if(m_uPostWater30[RI_3CPM] == BELOW_NORM)
    ++uNum3cpmBelowNorm;

  uNumDuodAboveNorm = 0;
  if(m_uPostWater10[RI_DUOD] == ABOVE_NORM)
    ++uNumDuodAboveNorm;
  if(m_uPostWater20[RI_DUOD] == ABOVE_NORM)
    ++uNumDuodAboveNorm;
  if(m_uPostWater30[RI_DUOD] == ABOVE_NORM)
    ++uNumDuodAboveNorm;

    // First look for normal conditions:
  if(uNumBradyAboveNorm == 0 && uNumTachyAboveNorm == 0)
    {
    if(uNum3cpmBelowNorm >= 2)
      iRet = DIAG_NORMAL;
    else if(uNum3cpmBelowNorm == 1)
      iRet = DIAG_SUB_NORMAL;
    }
  if(iRet == DIAG_NONE)
    {
    if(uNumBradyAboveNorm == 0 && uNumTachyAboveNorm > 0)
      { // Look for tachygastria
      if(uNumTachyAboveNorm >= 2)
        {
        if(uNum3cpmBelowNorm >= 2)
          iRet = DIAG_TACHY;
        else
          iRet = DIAG_PROB_TACHY;
        }
      else // if(uNumTachyAboveNorm == 1)
        {
        if(uNum3cpmBelowNorm >= 2)
          iRet = DIAG_PROB_TACHY;
        else
          iRet = DIAG_POS_TACHY_CC;
        }
      }
    else if(uNumTachyAboveNorm == 0 && uNumBradyAboveNorm > 0)
      { // Look for bradygastria.
      if(uNumBradyAboveNorm == 3)
        {
        if(uNum3cpmBelowNorm >= 2)
          iRet = DIAG_BRADY;
        else
          iRet = DIAG_PROB_BRADY;
        }
      else if(uNumBradyAboveNorm == 2)
        {
        if(uNum3cpmBelowNorm >= 2)
          iRet = DIAG_BRADY;
        else
          iRet = DIAG_POS_BRADY_CC;
        }
      else // uNumBradyAboveNorm == 1
        {
        if(uNum3cpmBelowNorm >= 2)
          iRet = DIAG_POS_BRADY_CC;
        else
          iRet = DIAG_PROB_NORM;
        }
      }
    else if(uNumBradyAboveNorm == 0 && uNumTachyAboveNorm == 0)
      { // All values are in normal range, check for obstruction.
      if(uNum3cpmAboveNorm == 3)
        iRet = DIAG_GAS_OUT_OBS;
      else if(uNum3cpmAboveNorm == 2)
        iRet = DIAG_PROB_NORM;
      else
        iRet = DIAG_NORMAL;
      }
    }
  if(iRet == DIAG_NONE && uNumDuodAboveNorm >= 2)
    { // Look at duodenal for respiration effect.
      // Average the respiration rate for all the periods with
      // high duodenal.
    uRespRate = 0;
    uPeriodCnt = 0;
    if(m_uPostWater10[RI_DUOD] == ABOVE_NORM)
      {
      uRespRate += m_pDoc->m_pdPatient.uRespRate[PERIOD_10];
      ++uPeriodCnt;
      }
    if(m_uPostWater20[RI_DUOD] == ABOVE_NORM)
      {
      uRespRate += m_pDoc->m_pdPatient.uRespRate[PERIOD_20];
      ++uPeriodCnt;
      }
    if(m_uPostWater30[RI_DUOD] == ABOVE_NORM)
      {
      uRespRate += m_pDoc->m_pdPatient.uRespRate[PERIOD_30];
      ++uPeriodCnt;
      }
    if(uPeriodCnt != 0)
      uRespRate /= uPeriodCnt;
    if(uRespRate > 15)
      iRet = DIAG_DUOD_HYPER;
    }
  if(iRet == DIAG_NONE)
    iRet = DIAG_MIX_DYS;
  return(iRet);
/**** Version III
//          if(m_uPostWater20[RI_3CPM] == ABOVE_NORM && m_uPostWater30[RI_3CPM] == ABOVE_NORM)
//            iRet = DIAG_PROB_NORM_GAS_OUT_OBS;
//          else
//            iRet = DIAG_NORMAL;
******/
  }

/********************************************************************
cstrGet_diagnosis - Get the text for the suggested diagnosis from the 
                    indicator.

  inputs: Indicator for the suggested diagnosis.

  return: CString object containing the suggested diagnosis.
********************************************************************/
CString CDiagnosis::cstrGet_diagnosis(short int iDiagnosis)
  {
  CString cstrDiagnosis;

  switch(iDiagnosis)
    {
    case DIAG_NORMAL:
      //cstrDiagnosis.LoadString(IDS_DIAG_PP_NORM);
      cstrDiagnosis = g_pLang->cstrLoad_string(ITEM_G_DIAG_PP_NORM);
      break;
    case DIAG_PROB_NORM:
      //cstrDiagnosis.LoadString(IDS_DIAG_PP_PROB_NORM);
      cstrDiagnosis = g_pLang->cstrLoad_string(ITEM_G_DIAG_PP_PROB_NORM);
      break;
    case DIAG_SUB_NORMAL:
      //cstrDiagnosis.LoadString(IDS_DIAG_PP_SUB_PROB);
      cstrDiagnosis = g_pLang->cstrLoad_string(ITEM_G_DIAG_PP_SUB_PROB);
      break;
    case DIAG_TACHY:
      //cstrDiagnosis.LoadString(IDS_DIAG_PP_TACHY);
      cstrDiagnosis = g_pLang->cstrLoad_string(ITEM_G_DIAG_PP_TACHY);
      break;
    case DIAG_PROB_TACHY:
      //cstrDiagnosis.LoadString(IDS_DIAG_PP_PROB_TACHY);
      cstrDiagnosis = g_pLang->cstrLoad_string(ITEM_G_DIAG_PP_PROB_TACHY);
      break;
    case DIAG_POS_TACHY_CC:
      //cstrDiagnosis.LoadString(IDS_DIAG_PP_POS_TACHY_CC);
      cstrDiagnosis = g_pLang->cstrLoad_string(ITEM_G_DIAG_PP_POS_TACHY_CC);
      break;
    case DIAG_BRADY:
      //cstrDiagnosis.LoadString(IDS_DIAG_PP_BRADY);
      cstrDiagnosis = g_pLang->cstrLoad_string(ITEM_G_DIAG_PP_BRADY);
      break;
    case DIAG_PROB_BRADY:
      //cstrDiagnosis.LoadString(IDS_DIAG_PP_PROB_BRADY);
      cstrDiagnosis = g_pLang->cstrLoad_string(ITEM_G_DIAG_PP_PROB_BRADY);
      break;
    case DIAG_POS_BRADY_CC:
      //cstrDiagnosis.LoadString(IDS_DIAG_PP_POS_BRADY_CC);
      cstrDiagnosis = g_pLang->cstrLoad_string(ITEM_G_DIAG_PP_POS_BRADY_CC);
      break;
    case DIAG_GAS_OUT_OBS:
      //cstrDiagnosis.LoadString(IDS_DIAG_PP_GOO);
      cstrDiagnosis = g_pLang->cstrLoad_string(ITEM_G_DIAG_PP_GOO);
      break;
    case DIAG_MIX_DYS:
      //cstrDiagnosis.LoadString(IDS_DIAG_PP_MXNS); //IDS_DIAG_PP_UGR);
      cstrDiagnosis = g_pLang->cstrLoad_string(ITEM_G_DIAG_PP_MXNS);
      break;
    case DIAG_DUOD_HYPER:
      //cstrDiagnosis.LoadString(IDS_DIAG_DUOD_HYPER);
      cstrDiagnosis = g_pLang->cstrLoad_string(ITEM_G_DIAG_DUOD_HYPER);
      break;
    default:
      cstrDiagnosis.Empty();
      break;
    }
  return(cstrDiagnosis);
  }

/********************************************************************
vGet_diagnosis - Get the diagnosis.

  inputs: Pointer to a PATIENT_DATA structure that gets the diagnoses.

  return: None.
********************************************************************/
void CDiagnosis::vGet_diagnosis(PATIENT_DATA *pdDiag)
  {

  if(m_uFrom == SM_VIEW)
    { // View only
    pdDiag->cstrSuggestedDiagnosis = m_pDoc->m_pdPatient.cstrSuggestedDiagnosis;
    pdDiag->cstrDiagnosis = m_pDoc->m_pdPatient.cstrDiagnosis;
    pdDiag->iDiagType = m_pDoc->m_pdPatient.iDiagType;
    pdDiag->bUserDiagnosis = m_pDoc->m_pdPatient.bUserDiagnosis;
    pdDiag->cstrDiagnosisKeyPhrase = m_pDoc->m_pdPatient.cstrDiagnosisKeyPhrase;
    pdDiag->cstrICDCode = m_pDoc->m_pdPatient.cstrICDCode;
   }
  else
    {
    if(m_cstrDiagnosis == "")
      {
      m_bUserDiagnosis = FALSE;
      m_cstrSuggestedDiagnosis = "";
      m_cstrDiagnosisKeyPhrase = "";
      m_cstrICDCode = "";
      }
    pdDiag->cstrSuggestedDiagnosis = m_cstrSuggestedDiagnosis; // Suggested diagnosis
    //if(m_bUserDiagnosis == TRUE)
      pdDiag->cstrDiagnosis = m_cstrDiagnosis; // User changed diagnosis.
    //else
    //  pdDiag->cstrDiagnosis.Empty(); // User accepted suggested diagnosis.
    pdDiag->iDiagType = m_iDiagAction;
    pdDiag->bUserDiagnosis = m_bUserDiagnosis;
    //if(m_cstrDiagnosis == "")
    //  {
    //  pdDiag->cstrDiagnosisKeyPhrase = "";
    //  pdDiag->cstrICDCode = "";
    //  }
    //else
    //  {
      pdDiag->cstrDiagnosisKeyPhrase = m_cstrDiagnosisKeyPhrase;
      pdDiag->cstrICDCode = m_cstrICDCode;
      //}
    }
  }

/********************************************************************
vSet_diagnosis - Set the diagnosis that this dialog box displays.

  inputs: Reference to a CString object that contains the suggested diagnosis.
          Reference to a CString object that contains the user entered diagnosis.

  return: None.
********************************************************************/
void CDiagnosis::vSet_diagnosis(PATIENT_DATA *pdDiag)
  {
  m_cstrSuggestedDiagnosis = pdDiag->cstrSuggestedDiagnosis; // Suggested diagnosis
  m_cstrDiagnosis = pdDiag->cstrDiagnosis; // User changed diagnosis.
  m_cstrOrigDiagnosis = m_cstrDiagnosis;
  m_bUserDiagnosis = pdDiag->bUserDiagnosis;
  m_cstrDiagnosisKeyPhrase = pdDiag->cstrDiagnosisKeyPhrase;
  m_cstrOrigDiagnosisKeyPhrase = m_cstrDiagnosisKeyPhrase;
  m_cstrICDCode = pdDiag->cstrICDCode;
  m_cstrOrigICDCode = m_cstrICDCode;
  }

/********************************************************************
bChanged - Determines if diagnosis has changed.

  inputs: None.

  return: true if the diagnosis has changed.
          false if there is no change in the diagnosis.
********************************************************************/
bool CDiagnosis::bChanged()
  {
  bool bRet;

  if(m_cstrOrigDiagnosis != m_cstrDiagnosis
  || m_cstrOrigDiagnosisKeyPhrase != m_cstrDiagnosisKeyPhrase
  || m_bUserDiagnosis == FALSE
  || m_cstrOrigICDCode != m_cstrICDCode)
    bRet = true;
  else
    bRet = false;

  return(bRet);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CDiagnosis::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_DIAGNOSIS);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_D_DIAGNOSIS));

    GetDlgItem(IDC_STATIC_SUGGESTED_DIAGNOSIS)->SetWindowText(g_pLang->cstrGet_text(ITEM_D_SUGG_DIAG));
    GetDlgItem(IDC_ACCEPT_DIAGNOSIS)->SetWindowText(g_pLang->cstrGet_text(ITEM_D_ACCEPT));
    GetDlgItem(IDC_MODIFY_DIAGNOSIS)->SetWindowText(g_pLang->cstrGet_text(ITEM_D_MODIFY));
    GetDlgItem(IDC_REJECT_DIAGNOSIS)->SetWindowText(g_pLang->cstrGet_text(ITEM_D_REJECT));
    GetDlgItem(IDC_STATIC_DIAGNOSIS)->SetWindowText(g_pLang->cstrGet_text(ITEM_D_DIAGNOSIS));
    
    GetDlgItem(IDC_BTN_ADD_TO_REPORT)->SetWindowText(g_pLang->cstrGet_text(ITEM_D_ADD_TO_REPORT));
    GetDlgItem(IDC_BTN_SAVE_DIAG_TO_LIST)->SetWindowText(g_pLang->cstrGet_text(ITEM_D_SAVE_DESC_TO_LIST));
    GetDlgItem(IDC_BTN_REMOVE_FROM_LIST)->SetWindowText(g_pLang->cstrGet_text(ITEM_D_REMOVE_DESC_FROM_LIST));
    GetDlgItem(IDC_BTN_INCLUDE_ICD_CODE)->SetWindowText(g_pLang->cstrGet_text(ITEM_D_INCLUDE_ICD_CODE));
    GetDlgItem(IDC_BTN_SAVE_ICD_TO_LIST)->SetWindowText(g_pLang->cstrGet_text(ITEM_D_ADD_ICD_TO_LIST));
    GetDlgItem(IDC_BTN_REMOVE_ICD_FROM_LIST)->SetWindowText(g_pLang->cstrGet_text(ITEM_D_REMOVE_ICD_FROM_LIST));
    GetDlgItem(IDC_STATIC_ADDITIONAL_FEATURE_INFO)->SetWindowText(g_pLang->cstrGet_text(ITEM_D_USE_ADDITIONAL_FEATURES));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_DIAGNOSIS_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    //g_pLang->vEnd_section();
    }
  }

/********************************************************************
OnBnClickedBtnAddToReport

  Message handler for the "Add to report" button.
    Save the diagnosis corresponding to the Diagnosis ID so it can
      be added to the report.
********************************************************************/
void CDiagnosis::OnBnClickedBtnAddToReport()
{
  DIAGNOSIS_DESC_RECORD ddrRecord;
  
  UpdateData(TRUE);
  ddrRecord.cstrDiagnosisKeyPhrase = m_cstrDiagnosisKeyPhrase;
  if(m_pDoc->m_pDB->iFind_diagnosis_desc(&ddrRecord) == SUCCESS)
    {
    // If diagnosis is already in the report, don't add it again.
    if(m_cstrDiagnosis.Find(ddrRecord.cstrDiagDesc) < 0)
      {
      if(m_cstrDiagnosis != "")
        m_cstrDiagnosis += "\r\n";
      m_cstrDiagnosis += ddrRecord.cstrDiagDesc;
      m_bUserDiagnosis = true;
      }
    }
  UpdateData(FALSE);
}

/********************************************************************
OnBnClickedBtnSaveDiagToList

  Message handler for the "Save description to list" button.
    Save the diagnosis ID and description to the DiagnosisDesc database
    table.
********************************************************************/
void CDiagnosis::OnBnClickedBtnSaveDiagToList()
  {
  DIAGNOSIS_DESC_RECORD ddrRecord;
  CString cstrWinTxtKeyPhrase;
  
  UpdateData(TRUE);
  m_cDiagList.GetWindowText(cstrWinTxtKeyPhrase);

  if(cstrWinTxtKeyPhrase == "")
    {
      g_pLang->vStart_section(SEC_DIAGNOSIS);
      MessageBox(g_pLang->cstrGet_text(ITEM_D_ENTER_DIAG_KEY_PHRASE), //  "Please enter a key phrase for the diagnosis.", 
        g_pLang->cstrLoad_string(ITEM_G_INFORMATION));
    }
  else if(m_cstrDiagnosis == "")
    {
    g_pLang->vStart_section(SEC_DIAGNOSIS);
    MessageBox(g_pLang->cstrGet_text(ITEM_D_ADD_DIAG_DESC), //  "Please add a description of the diagnosis.", 
      g_pLang->cstrLoad_string(ITEM_G_INFORMATION));
    }
  else
    {
    // If the selected item is 0, it is a new key phrase.  Otherwise it is an
    // existing key phrase being updated.
    if(m_iDiagnosisSelIndex == 0)
      {
      ddrRecord.cstrDiagnosisKeyPhrase = cstrWinTxtKeyPhrase;
      ddrRecord.cstrDiagDesc = m_cstrDiagnosis;
      // Save the record.  If the ID already exists, update it.
      m_pDoc->m_pDB->iSave_diagnosis_desc(&ddrRecord);
      }
    else if(m_iDiagnosisSelIndex > 0)
      {
      // Get the existing key phrase.
      //m_cDiagList.GetLBText(iIndex, m_cstrDiagnosisKeyPhrase);
      ddrRecord.cstrDiagnosisKeyPhrase = cstrWinTxtKeyPhrase;
      ddrRecord.cstrDiagDesc = m_cstrDiagnosis;
      m_pDoc->m_pDB->iUpdate_diagnosis_desc(m_cstrDiagnosisKeyPhrase, &ddrRecord);
      }
    // Rebuild the combobox and select the empty line.
    vFill_diag_desc_id();
    // Select the empty entry.
    m_cDiagList.SetCurSel(0);
    m_iDiagnosisSelIndex = 0;
    m_cICDList.SetCurSel(0);
    m_iICDCodeSelIndex = 0;
    m_cstrDiagnosis = "";
    UpdateData(FALSE);
    }
  }

/********************************************************************
OnBnClickedBtnRemoveFromList

  Message handler for the "Remove from list" button.
    Remove the diagnosis ID and description from the DiagnosisDesc database
    table.
********************************************************************/
void CDiagnosis::OnBnClickedBtnRemoveFromList()
{

  UpdateData(TRUE);
  if(m_cstrDiagnosis != "")
  {
    g_pLang->vStart_section(SEC_DIAGNOSIS);
    MessageBox(g_pLang->cstrGet_text(ITEM_D_ERASE_DESC), //"Please erase the diagnosis description and ICD code.", 
      g_pLang->cstrLoad_string(ITEM_G_INFORMATION));
  }
  else
  {
    m_pDoc->m_pDB->iDelete_diagnosis_desc(m_cstrDiagnosisKeyPhrase);
    m_cstrDiagnosisKeyPhrase = "";
    // Rebuild the combobox and select the empty line.
    vFill_diag_desc_id();
    m_cDiagList.SetCurSel(0);
    UpdateData(FALSE);
  }
}

/********************************************************************
OnCbnSelchangeCbxDiagList

  Message handler for the Diagnosis dropdown list box.
    Save the currently selected Diagnosis ID.
********************************************************************/
void CDiagnosis::OnCbnSelchangeCbxDiagList()
{

  UpdateData(TRUE);
  m_iDiagnosisSelIndex = m_cDiagList.GetCurSel();
  m_cDiagList.GetLBText(m_iDiagnosisSelIndex, m_cstrDiagnosisKeyPhrase);
  if(m_iDiagnosisSelIndex <= 0)
    m_cstrDiagnosis = "";
  UpdateData(FALSE);
}

/********************************************************************
OnBnClickedBtnIncludeIcdCode

  Message handler for the "Include ICD Code" button.
    Save the ICD Code so it can be added to the report.
********************************************************************/
void CDiagnosis::OnBnClickedBtnIncludeIcdCode()
  {
  bool bAdd = true;
  CString cstrText;
  
  UpdateData(TRUE);
  cstrText = g_pLang->cstrLoad_string(ITEM_G_ICDCODE) + ":";
  if(m_cstrDiagnosis.Find(cstrText) >= 0)
    { // Alread have an ICD code.
    if(m_cstrDiagnosis.Find(m_cstrICDCode) < 0)
      { // The ICD is different.
      g_pLang->vStart_section(SEC_DIAGNOSIS);
      MessageBox(g_pLang->cstrGet_text(ITEM_D_DEL_ICD_CODE_LINE), //"Please delete the entire line containing the ICD code.", 
        g_pLang->cstrLoad_string(ITEM_G_INFORMATION));
      bAdd = false;
      }
    else
      bAdd = false; // ICD code is the same, don't add it again.
    }
  if(bAdd == true)
    {
    if(m_cstrDiagnosis != "")
      m_cstrDiagnosis += "\r\n";
    m_cstrDiagnosis += (cstrText + " " + m_cstrICDCode);
    }
  UpdateData(FALSE);
  }

/********************************************************************
OnBnClickedBtnSaveIcdToList

  Message handler for the "Save ICD Code to list" button.
    Save the ICD Code to the ICDCode database table.
********************************************************************/
void CDiagnosis::OnBnClickedBtnSaveIcdToList()
  {
  ICD_CODE_RECORD icdRecord;
  CString cstrWinTxtCode;
  
  UpdateData(TRUE);
  m_cICDList.GetWindowText(cstrWinTxtCode);
  if(cstrWinTxtCode == "")
    {
    g_pLang->vStart_section(SEC_DIAGNOSIS);
    MessageBox(g_pLang->cstrGet_text(ITEM_D_ENTER_ICD_CODE), //"Please enter an ICD code.", 
      g_pLang->cstrLoad_string(ITEM_G_INFORMATION));
    }
  else
    {
    // If the selected item is 0, it is a new ICD Code.  Otherwise it is an
    // existing ICD Code being updated.
    if(m_iICDCodeSelIndex == 0)
      {
      icdRecord.cstrICDCode = cstrWinTxtCode;
      // Save the record.  If the ID already exists, update it.
      m_pDoc->m_pDB->iSave_ICDCode(&icdRecord);
      }
    else if(m_iICDCodeSelIndex > 0)
      {
      ICD_CODE_RECORD icdNew;

      icdRecord.cstrICDCode = m_cstrICDCode; // Existing ICD code.
      icdNew.cstrICDCode = cstrWinTxtCode; // New code.
      if(m_pDoc->m_pDB->iFind_ICDCode(&icdRecord) == SUCCESS)
        { // Found the record, update it.
        m_pDoc->m_pDB->iUpdate_ICDCode(&icdRecord, &icdNew);
        }
      else
        { // Couldn't update it, probably removed/renamed it and added it right back in.
          // Try adding it to the database.
        m_pDoc->m_pDB->iSave_ICDCode(&icdNew);
        }
      }
    // Rebuild the combobox and select the empty line.
    vFill_icd_code();
    // Select the empty line.
    m_cDiagList.SetCurSel(0);
    m_iDiagnosisSelIndex = 0;
    m_cICDList.SetCurSel(0);
    m_iICDCodeSelIndex = 0;
    m_cstrDiagnosis = "";
    UpdateData(FALSE);
    }
  }

/********************************************************************
OnBnClickedBtnRemoveIcdFromList

  Message handler for the "Remove from list" button.
    Remove the ICD Code from the ICDCode database table.
********************************************************************/
void CDiagnosis::OnBnClickedBtnRemoveIcdFromList()
  {

  UpdateData(TRUE);
  if(m_cstrDiagnosis.Find(g_pLang->cstrLoad_string(ITEM_G_ICDCODE)) >= 0)
    {
    g_pLang->vStart_section(SEC_DIAGNOSIS);
    MessageBox(g_pLang->cstrGet_text(ITEM_D_DEL_ICD_CODE_LINE), //"Please erase the diagnosis description and ICD code.", 
      g_pLang->cstrLoad_string(ITEM_G_INFORMATION));
    }
  else
  {
    m_pDoc->m_pDB->iDelete_ICDCode(m_cstrICDCode);
    // Rebuild the combobox and select the empty line.
    vFill_icd_code();
    // Select the empty line.
    m_cICDList.SetCurSel(0);
    m_cstrDiagnosis = "";
    UpdateData(FALSE);
  }
  }

/********************************************************************
OnCbnSelchangeCbxIcdList

  Message handler for the ICD Code dropdown list box.
    Save the currently selected ICD Code.
********************************************************************/
void CDiagnosis::OnCbnSelchangeCbxIcdList()
  {
  int iIndex;

  UpdateData(TRUE);
  iIndex = m_cICDList.GetCurSel();
  m_cICDList.GetLBText(iIndex, m_cstrICDCode);
  if(iIndex <= 0)
    m_cstrICDCode = "";
  UpdateData(FALSE);
  }

/********************************************************************
vEnable_diagdesc_icd_fields - Enable/disable the diagnosis ID and ICD
                              code fields.

  inputs: TRUE to enable the fields, FALSE to disable the fields.

  return: None.
********************************************************************/
void CDiagnosis::vEnable_diagdesc_icd_fields(BOOL bEnable)
{
  GetDlgItem(IDC_BTN_ADD_TO_REPORT)->EnableWindow(bEnable);
  GetDlgItem(IDC_BTN_SAVE_DIAG_TO_LIST)->EnableWindow(bEnable);
  GetDlgItem(IDC_BTN_REMOVE_FROM_LIST)->EnableWindow(bEnable);
  GetDlgItem(IDC_BTN_INCLUDE_ICD_CODE)->EnableWindow(bEnable);
  GetDlgItem(IDC_BTN_SAVE_ICD_TO_LIST)->EnableWindow(bEnable);
  GetDlgItem(IDC_BTN_REMOVE_ICD_FROM_LIST)->EnableWindow(bEnable);
  GetDlgItem(IDC_STATIC_ADDITIONAL_FEATURE_INFO)->EnableWindow(bEnable);
  GetDlgItem(IDC_CBX_DIAG_LIST)->EnableWindow(bEnable);
  GetDlgItem(IDC_CBX_ICD_LIST)->EnableWindow(bEnable);
}

/********************************************************************
vFill_diag_desc_id - Fill the Diagnosis ID dropdown list box from the
                     DiagnosisDesc database table.

  inputs: None.

  return: None.
********************************************************************/
void CDiagnosis::vFill_diag_desc_id()
{
  DIAGNOSIS_DESC_RECORD ddrRecord;

  m_cDiagList.ResetContent();
  m_cDiagList.AddString(g_pLang->cstrLoad_string(ITEM_G_ADD_NEW_KEY_PHRASE)); 
  if(m_pDoc->m_pDB->iGet_diagnosis_desc(FIRST_RECORD, &ddrRecord) == SUCCESS)
    {
    m_cDiagList.AddString(ddrRecord.cstrDiagnosisKeyPhrase);
    while(m_pDoc->m_pDB->iGet_diagnosis_desc(NEXT_RECORD, &ddrRecord) == SUCCESS)
      m_cDiagList.AddString(ddrRecord.cstrDiagnosisKeyPhrase);
    }
}

/********************************************************************
vFill_icd_code - Fill the ICD Code dropdown list box from the
                     ICDCode database table.

  inputs: None.

  return: None.
********************************************************************/
void CDiagnosis::vFill_icd_code()
{
  ICD_CODE_RECORD icdRecord;

  m_cICDList.ResetContent();
  m_cICDList.AddString(g_pLang->cstrGet_text(ITEM_D_ADD_NEW_ICD_CODE)); // Add an empty line.
  if(m_pDoc->m_pDB->iGet_ICDCode(FIRST_RECORD, &icdRecord) == SUCCESS)
    {
      m_cICDList.AddString(icdRecord.cstrICDCode);
    while(m_pDoc->m_pDB->iGet_ICDCode(NEXT_RECORD, &icdRecord) == SUCCESS)
      m_cICDList.AddString(icdRecord.cstrICDCode);
    }
}

/********************************************************************
vDisplay_diagnosis - 

    - display both the suggested and user diagnosis.
    - If diagnosis key phrase, display that in the dropdown listbox.


  inputs: None.

  return: None.
********************************************************************/
void CDiagnosis::vDisplay_diagnosis(void)
  {
  int iIndex;
  DIAGNOSIS_DESC_RECORD ddrRecord;

  //if(m_pDoc->m_pdPatient.cstrSuggestedDiagnosis != "")
  //  m_cstrDiagnosis = m_pDoc->m_pdPatient.cstrSuggestedDiagnosis;
  //if(m_pDoc->m_pdPatient.cstrDiagnosis != "")
  //  {
  //  if(m_cstrDiagnosis != "")
  //    m_cstrDiagnosis += "\r\n";
    //m_cstrDiagnosis += (m_pDoc->m_pdPatient.cstrDiagnosis + "\r\n");
    //}
  if(m_pDoc->m_pdPatient.cstrDiagnosisKeyPhrase != "")
    {
    // Select the diagnosis key phrase.
    ddrRecord.cstrDiagnosisKeyPhrase = m_pDoc->m_pdPatient.cstrDiagnosisKeyPhrase;
    if(m_pDoc->m_pDB->iFind_diagnosis_desc(&ddrRecord) == SUCCESS)
      iIndex = m_cDiagList.SelectString(0, m_pDoc->m_pdPatient.cstrDiagnosisKeyPhrase);
    else
      iIndex = 0;
    if(iIndex >= 0)
      m_cDiagList.SetCurSel(iIndex);
    }
  }

void CDiagnosis::vDisplay_ICD_code(void)
  {
  ICD_CODE_RECORD icdRecord;
  int iIndex;

  if(m_pDoc->m_pdPatient.cstrICDCode != "")
    {
    // Select the diagnosis key phrase.
    icdRecord.cstrICDCode = m_pDoc->m_pdPatient.cstrICDCode;
    if(m_pDoc->m_pDB->iFind_ICDCode(&icdRecord) == SUCCESS)
      iIndex = m_cICDList.SelectString(0, m_pDoc->m_pdPatient.cstrICDCode);
    else
      iIndex = 0;
    if(iIndex >= 0)
      m_cICDList.SetCurSel(iIndex);
    }
  }
