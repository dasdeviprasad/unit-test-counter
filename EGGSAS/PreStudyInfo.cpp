/********************************************************************
PreStudyInfo.cpp

Copyright (C) 2003,2004,2005,2008,2009,2012,2020 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CPreStudyInfo class.

  This class provides for entry/edit/viewing of prestudy information.

HISTORY:
03-OCT-03  RET  New.
16-MAR-04  RET  Add help button and message handler.
10-FEB-05  RET  Add Delayed gastric emptying check box and variable.
                  Changed following methods:
                    OnInitDialog(), OnOK(), bChanged()
07-N0V-08  RET  Version 2.08a
                  Added new time picker control so that the time can be
                    selected and changed.
14-JAN-09  RET  Version CDDRdr.2.08c
                  Display only if view mode.  Change Constructor.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
13-SEP-12  RET
             Changes to indications:
               gastroparesis - 536.3  substitutes for “delayed emptying”
               dyspepsia - bloating and/or satiety - 536.80 substitutes for bloating and satiety
               dyspepsia - 536.80…this is additional
               nausea - 787.02  substitutes for nausea
               vomiting - 787.01 substitutes for vomiting
               anorexia - 783.0…this is additional
               Refractory GERD – 530.11….this is additional
               Weight loss – 783.20 ….this is additional
               Post dilation evaluation……this is additional
               Evaluate effect of medication….this is additional

               Existing indication (checkbox) variables:
                 m_bDelayedGastricEmptying --> gastroparesis - 536.3
                 m_bBloating               --
                                             |--> dyspepsia - bloating and/or satiety - 536.80
                 m_bEarlySateity           --
                 m_bNausea                 --> nausea - 787.02
                 m_bVomiting               --> vomiting - 787.01
                 m_bEructus                --> Remove
                 m_bGerd                   --> Remove
                 m_bPPFullness             --> Remove

               New indication variables: 
	               bGastroparesis; // gastroparesis - 536.3  substitutes for "delayed emptying"
	               bDyspepsiaBloatSatiety; // dyspepsia - bloating and/or satiety - 536.80 substitutes for bloating and satiety
	               bDyspepsia; // dyspepsia - 536.80
	               bAnorexia; // anorexia - 783.0
	               bRefractoryGERD; // Refractory GERD – 530.11
	               bWeightLoss; // Weight loss
	               bPostDilationEval; // Post dilation evaluation
                 bEvalEffectMed; // Evaluate effect of medication

               For dyspepsia - bloating and/or satiety - 536.80:
                 When read up the database if m_bBloating or m_bEarlySateity is
                   TRUE then set bDyspepsiaBloatSatiety to TRUE.
                 When exit this dialgbox, set m_bBloating and m_bEarlySateity
                   to FALSE.
                 Don't indicate database has changed unless bDyspepsiaBloatSatiety
                   checkbox changed state.
               For gastroparesis - 536.3:
                 When read up the database if m_bDelayedGastricEmptying is
                   TRUE then set bGastroparesis to TRUE.
                 When exit this dialgbox, set m_bDelayedGastricEmptying to FALSE.
                 Don't indicate database has changed unless bGastroparesis
                   checkbox changed state.
               Change methods: OnInitDialog(), OnOK(), bChanged(), vSet_text()
18-SEP-12  RET
             Add Include in Report Checkboxes for the attending physician,
               the refering physician and the technician.
                 Add class variables: m_bChkAttendingPhysician, m_bChkReferingPhysician,
                   m_bChkTechnician, m_bOrigChkAttendingPhysician,
                   m_bOrigChkReferingPhysician, m_bOrigChkTechnician
                 Change methods: OnInitDialog(), OnOK(), bChanged(),
                   vSet_text()
                 Add event handlers for the checkboxes:
                   OnBnClickedChkAttendPhys(), OnBnClickedChkReferPhys()
                   OnBnClickedChkTechnician()
6-JUL-20  RET
            ".hlp" files are no longer supported in Win10.  Change from
            calling WinHelp() to HtmlHelp().
              Change method: OnBnClickedPrestudyHelp()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "PreStudyInfo.h"
#include "util.h"
#include ".\prestudyinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreStudyInfo dialog

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the currently active document.
          Indicates why this object is being used (new study, edit, view).
          Pointer to parent window (default is NULL).
********************************************************************/
CPreStudyInfo::CPreStudyInfo(CEGGSASDoc *pDoc, unsigned uFrom, CWnd* pParent /*=NULL*/)
	: CDialog(CPreStudyInfo::IDD, pParent)
  , m_bGastroparesis(FALSE)
  , m_bDyspepsiaBloatSatiety(FALSE)
  , m_bDyspepsia(FALSE)
  , m_bAnorexia(FALSE)
  , m_bRefractoryGERD(FALSE)
  , m_bWeightLoss(FALSE)
  , m_bPostDilationEval(FALSE)
  , m_bEvalEffectMed(FALSE)
  , m_bChkAttendingPhysician(FALSE)
  , m_bChkReferingPhysician(FALSE)
  , m_bChkTechnician(FALSE)
  {
	//{{AFX_DATA_INIT(CPreStudyInfo)
	m_cstrAttendPhys = _T("");
	m_cstrIFPComments = _T("");
	m_bNausea = FALSE;
	m_bVomiting = FALSE;
	m_cstrReferPhys = _T("");
	m_cstrTechnician = _T("");
	//}}AFX_DATA_INIT

  m_pDoc = pDoc;
#if EGGSAS_READER == READER_DEPOT
  m_uFrom = SM_VIEW;
#else
  m_uFrom = uFrom;
#endif
  }


void CPreStudyInfo::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
//{{AFX_DATA_MAP(CPreStudyInfo)
DDX_Text(pDX, IDC_ATTEND_PHYS, m_cstrAttendPhys);
DDX_Text(pDX, IDC_IFP_COMMENTS, m_cstrIFPComments);
DDX_Check(pDX, IDC_IFP_NAUSEA, m_bNausea);
DDX_Check(pDX, IDC_IFP_VOMITING, m_bVomiting);
DDX_Text(pDX, IDC_REFER_PHYS, m_cstrReferPhys);
DDX_Text(pDX, IDC_TECHNICIAN, m_cstrTechnician);
//}}AFX_DATA_MAP
DDX_Control(pDX, IDC_DATETIMEPICKER_MEAL, m_cPreTestMealTime);
DDX_Check(pDX, IDC_CHK_GASTROPARESIS, m_bGastroparesis);
DDX_Check(pDX, IDC_CHK_DYSPEPSIA_BLOAT_SATIETY, m_bDyspepsiaBloatSatiety);
DDX_Check(pDX, IDC_CHK_DYSPEPSIA, m_bDyspepsia);
DDX_Check(pDX, IDC_CHK_ANOREXIA, m_bAnorexia);
DDX_Check(pDX, IDC_CHK_REFRACTORY_GERD, m_bRefractoryGERD);
DDX_Check(pDX, IDC_CHK_WEIGHTLOSS, m_bWeightLoss);
DDX_Check(pDX, IDC_CHK_POST_DILATION_EVAL, m_bPostDilationEval);
DDX_Check(pDX, IDC_CHK_EVAL_EFFECT_MED, m_bEvalEffectMed);
DDX_Check(pDX, IDC_CHK_ATTEND_PHYS, m_bChkAttendingPhysician);
DDX_Check(pDX, IDC_CHK_REFER_PHYS, m_bChkReferingPhysician);
DDX_Check(pDX, IDC_CHK_TECHNICIAN, m_bChkTechnician);
}


BEGIN_MESSAGE_MAP(CPreStudyInfo, CDialog)
	//{{AFX_MSG_MAP(CPreStudyInfo)
	//}}AFX_MSG_MAP
  ON_BN_CLICKED(IDC_PRESTUDY_HELP, OnBnClickedPrestudyHelp)
  ON_BN_CLICKED(IDC_CHK_ATTEND_PHYS, OnBnClickedChkAttendPhys)
  ON_BN_CLICKED(IDC_CHK_REFER_PHYS, OnBnClickedChkReferPhys)
  ON_BN_CLICKED(IDC_CHK_TECHNICIAN, OnBnClickedChkTechnician)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreStudyInfo message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Modify the fields for editing/viewing.
********************************************************************/
BOOL CPreStudyInfo::OnInitDialog() 
  {
  CString cstrText;
  time_t tCurrent, tMealTime;
  struct tm *ptmCurrent;
  int iIndex;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
  vSet_text();
  if(m_uFrom == SM_EDIT || m_uFrom == SM_VIEW)
    {
    if(m_uFrom == SM_VIEW)
      { // In View mode, nothing is changeable.
      GetDlgItem(IDC_DATETIMEPICKER_MEAL)->EnableWindow(FALSE); //IDC_MEAL_TIME)->EnableWindow(FALSE);
	    GetDlgItem(IDC_ATTEND_PHYS)->EnableWindow(FALSE);
	    GetDlgItem(IDC_IFP_NAUSEA)->EnableWindow(FALSE);
	    GetDlgItem(IDC_IFP_VOMITING)->EnableWindow(FALSE);
	    GetDlgItem(IDC_CHK_GASTROPARESIS)->EnableWindow(FALSE);
	    GetDlgItem(IDC_CHK_DYSPEPSIA_BLOAT_SATIETY)->EnableWindow(FALSE);
	    GetDlgItem(IDC_CHK_DYSPEPSIA)->EnableWindow(FALSE);
	    GetDlgItem(IDC_CHK_ANOREXIA)->EnableWindow(FALSE);
	    GetDlgItem(IDC_CHK_REFRACTORY_GERD)->EnableWindow(FALSE);
	    GetDlgItem(IDC_CHK_WEIGHTLOSS)->EnableWindow(FALSE);
	    GetDlgItem(IDC_CHK_POST_DILATION_EVAL)->EnableWindow(FALSE);
	    GetDlgItem(IDC_CHK_EVAL_EFFECT_MED)->EnableWindow(FALSE);
	    GetDlgItem(IDC_IFP_COMMENTS)->EnableWindow(FALSE);
	    GetDlgItem(IDC_REFER_PHYS)->EnableWindow(FALSE);
	    GetDlgItem(IDC_TECHNICIAN)->EnableWindow(FALSE);
	    GetDlgItem(IDC_CHK_ATTEND_PHYS)->EnableWindow(FALSE);
	    GetDlgItem(IDC_CHK_REFER_PHYS)->EnableWindow(FALSE);
	    GetDlgItem(IDC_CHK_TECHNICIAN)->EnableWindow(FALSE);
      GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
      //cstrText.LoadString(IDS_OK); // Change OK button text.
      cstrText = g_pLang->cstrLoad_string(ITEM_G_OK);
        // Horizontally center the OK button.
      vCenter_button_horiz(this, GetDlgItem(IDOK));
      }
    else
      cstrText = g_pLang->cstrLoad_string(ITEM_G_OK);
      //cstrText.LoadString(IDS_OK); // Change OK button text.
    GetDlgItem(IDOK)->SetWindowText((LPCTSTR)cstrText);
    }
	
    // Get any prestudy information.
    // Format the time picker control for 1 or 2 digit hour
    // and 2 digit minute and AM/PM.
  m_cPreTestMealTime.SetFormat("h:mm tt");
    // Get the current time, change the hours and minutes to those of the patient's
    // meal time, then set the time picker control.
  tCurrent = time(NULL);
  ptmCurrent = localtime(&tCurrent);
  if((iIndex = m_pDoc->m_pdPatient.cstrMealTime.Find(':')) > 0)
    {
    ptmCurrent->tm_hour = atoi(m_pDoc->m_pdPatient.cstrMealTime);
    if(m_pDoc->m_pdPatient.cstrMealTime.Find("PM") > 0)
      ptmCurrent->tm_hour += 12;
    if(iIndex < m_pDoc->m_pdPatient.cstrMealTime.GetLength())
      ptmCurrent->tm_min = 
          atoi(m_pDoc->m_pdPatient.cstrMealTime.Right(m_pDoc->m_pdPatient.cstrMealTime.GetLength() - iIndex - 1));
    ptmCurrent->tm_sec = 0;
    }
  tMealTime = mktime(ptmCurrent);
  CTime ctCurrent(tMealTime);
  m_cPreTestMealTime.SetTime(&ctCurrent);

	m_cstrAttendPhys = m_pDoc->m_pdPatient.cstrAttendPhys;
	m_cstrIFPComments = m_pDoc->m_pdPatient.cstrIFPComments;
	m_cstrReferPhys = m_pDoc->m_pdPatient.cstrReferPhys;
	m_cstrTechnician = m_pDoc->m_pdPatient.cstrTechnician;
	m_bNausea = m_pDoc->m_pdPatient.bNausea;
	m_bVomiting = m_pDoc->m_pdPatient.bVomiting;
  m_bDelayedGastricEmptying = m_pDoc->m_pdPatient.bDelayedGastricEmptying;

  m_bGastroparesis = m_pDoc->m_pdPatient.bGastroparesis;
  m_bDyspepsiaBloatSatiety = m_pDoc->m_pdPatient.bDyspepsiaBloatSatiety;
  m_bDyspepsia = m_pDoc->m_pdPatient.bDyspepsia;
  m_bAnorexia = m_pDoc->m_pdPatient.bAnorexia;
  m_bRefractoryGERD = m_pDoc->m_pdPatient.bRefractoryGERD;
  m_bWeightLoss = m_pDoc->m_pdPatient.bWeightLoss;
  m_bPostDilationEval = m_pDoc->m_pdPatient.bPostDilationEval;
  m_bEvalEffectMed = m_pDoc->m_pdPatient.bEvalEffectMed;

  m_cstrOrigMealTime = m_pDoc->m_pdPatient.cstrMealTime;
  m_cstrOrigAttendPhys = m_cstrAttendPhys;
  m_cstrOrigIFPComments = m_cstrIFPComments;
  m_cstrOrigReferPhys = m_cstrReferPhys;
  m_cstrOrigTechnician = m_cstrTechnician;
  m_bOrigNausea = m_bNausea;
  m_bOrigVomiting = m_bVomiting;
  m_bOrigDelayedGastricEmptying = m_bDelayedGastricEmptying;

  m_bOrigGastroparesis = m_bGastroparesis;
  m_bOrigDyspepsiaBloatSatiety = m_bDyspepsiaBloatSatiety;
  m_bOrigDyspepsia = m_bDyspepsia;
  m_bOrigAnorexia = m_bAnorexia;
  m_bOrigRefractoryGERD = m_bRefractoryGERD;
  m_bOrigWeightLoss = m_bWeightLoss;
  m_bOrigPostDilationEval = m_bPostDilationEval;
  m_bOrigEvalEffectMed = m_bEvalEffectMed;

  // Get the include in report items.
  if(g_pConfig->bGet_inc_rpt_item(INC_ATTENDING_PHYSICIAN) == true)
    m_bChkAttendingPhysician = TRUE;
  else
    m_bChkAttendingPhysician = FALSE;
  m_bOrigChkAttendingPhysician = m_bChkAttendingPhysician;

  if(g_pConfig->bGet_inc_rpt_item(INC_REFERING_PHYSICIAN) == true)
    m_bChkReferingPhysician = TRUE;
  else
    m_bChkReferingPhysician = FALSE;
  m_bOrigChkReferingPhysician = m_bChkReferingPhysician;

  if(g_pConfig->bGet_inc_rpt_item(INC_TECHNICIAN) == true)
    m_bChkTechnician = TRUE;
  else
    m_bChkTechnician = FALSE;
  m_bOrigChkTechnician = m_bChkTechnician;

  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnOK

  Message handler for the OK button.
    Get the contents of all the fields and store in the patient information
      structure in the currently active document.
********************************************************************/
void CPreStudyInfo::OnOK() 
  {
  CTime ctMealTime;

  UpdateData(TRUE);
  if(m_uFrom != SM_VIEW)
    {
    m_cPreTestMealTime.GetTime(ctMealTime);
    m_pDoc->m_pdPatient.cstrMealTime = cstrGet_hrs_mins(ctMealTime);
		m_pDoc->m_pdPatient.cstrAttendPhys = m_cstrAttendPhys;
		m_pDoc->m_pdPatient.cstrIFPComments = m_cstrIFPComments;
		m_pDoc->m_pdPatient.cstrReferPhys = m_cstrReferPhys;
		m_pDoc->m_pdPatient.cstrTechnician = m_cstrTechnician;
		m_pDoc->m_pdPatient.bBloating = FALSE; //m_bBloating;
		m_pDoc->m_pdPatient.bEarlySateity = FALSE; //m_bEarlySateity;
		m_pDoc->m_pdPatient.bNausea = m_bNausea;
		m_pDoc->m_pdPatient.bVomiting = m_bVomiting;
    m_pDoc->m_pdPatient.bDelayedGastricEmptying = FALSE; //m_bDelayedGastricEmptying;
    // New and changed checkboxes.
    m_pDoc->m_pdPatient.bGastroparesis = m_bGastroparesis;
    m_pDoc->m_pdPatient.bDyspepsiaBloatSatiety = m_bDyspepsiaBloatSatiety;
    m_pDoc->m_pdPatient.bDyspepsia = m_bDyspepsia;
    m_pDoc->m_pdPatient.bAnorexia = m_bAnorexia;
    m_pDoc->m_pdPatient.bRefractoryGERD = m_bRefractoryGERD;
    m_pDoc->m_pdPatient.bWeightLoss = m_bWeightLoss;
    m_pDoc->m_pdPatient.bPostDilationEval = m_bPostDilationEval;
    m_pDoc->m_pdPatient.bEvalEffectMed = m_bEvalEffectMed;

    g_pConfig->vSet_inc_rpt_item(INC_ATTENDING_PHYSICIAN, m_bChkAttendingPhysician);
    g_pConfig->vSet_inc_rpt_item(INC_REFERING_PHYSICIAN, m_bChkReferingPhysician);
    g_pConfig->vSet_inc_rpt_item(INC_TECHNICIAN, m_bChkTechnician);
    g_pConfig->vWrite_inc_rpt_item();
    }

	CDialog::OnOK();
  }

/********************************************************************
OnCancel

  Message handler for the Cancel button.
    If this is a new study, make sure the user really wants to cancel the
      study.
********************************************************************/
void CPreStudyInfo::OnCancel() 
  {
  CString cstrMsg, cstrCaption;

  if(m_uFrom == SM_NEW_STUDY)
    {
    //cstrMsg.LoadString(IDS_END_STUDY_ARE_YOU_SURE);
    cstrMsg = g_pLang->cstrLoad_string(ITEM_G_END_STUDY_SURE);
    //cstrCaption.LoadString(IDS_WARNING);
    cstrCaption = g_pLang->cstrLoad_string(ITEM_G_WARNING);
    if(MessageBox(cstrMsg, cstrCaption, MB_YESNO) == IDYES)
      CDialog::OnCancel();
    }
  else
    CDialog::OnCancel();
  }

/********************************************************************
OnBnClickedPrestudyHelp - Handler for the "Help" button.

   Display the help window for this dialog box.
********************************************************************/
void CPreStudyInfo::OnBnClickedPrestudyHelp()
  {
  //AfxGetApp()->WinHelp(HIDC_PRESTUDY_HELP, HELP_CONTEXT);

  HtmlHelp(HIDC_PRESTUDY_HELP);
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
bChanged - Determines if any of the prestudy information has changed.

  inputs: None.

  return: true if the prestudy information has changed.
          false if there is no change in the prestudy information.
********************************************************************/
bool CPreStudyInfo::bChanged()
  {
  bool bRet;

  if(m_bOrigGastroparesis != m_bGastroparesis
  || m_bOrigDyspepsiaBloatSatiety != m_bDyspepsiaBloatSatiety
  || m_bOrigDyspepsia != m_bDyspepsia
  || m_bOrigAnorexia != m_bAnorexia
  || m_bOrigRefractoryGERD != m_bRefractoryGERD
  || m_bOrigWeightLoss != m_bWeightLoss
  || m_bOrigPostDilationEval != m_bPostDilationEval
  || m_bOrigEvalEffectMed != m_bEvalEffectMed
  || m_bOrigNausea != m_bNausea
  || m_bOrigVomiting != m_bVomiting
  || m_cstrOrigMealTime != m_pDoc->m_pdPatient.cstrMealTime
  || m_cstrOrigAttendPhys != m_cstrAttendPhys
  || m_cstrOrigIFPComments != m_cstrIFPComments
  || m_cstrOrigReferPhys != m_cstrReferPhys
  || m_cstrOrigTechnician != m_cstrTechnician
  || m_bOrigChkAttendingPhysician != m_bChkAttendingPhysician
  || m_bOrigChkReferingPhysician != m_bChkReferingPhysician
  || m_bOrigChkTechnician != m_bChkTechnician)
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
void CPreStudyInfo::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_PRESTUDY_INFO);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_PRESTUDY_INFO));

    GetDlgItem(IDC_STATIC_MEAL_TIME)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_MEAL_TIME));
    
    GetDlgItem(IDC_STATIC_BOX_INDICATIONS)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_INDIC_FOR_PROC));
    GetDlgItem(IDC_IFP_NAUSEA)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_NAUSEA));
    GetDlgItem(IDC_IFP_VOMITING)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_VOMITING));
    GetDlgItem(IDC_CHK_GASTROPARESIS)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_GASTROPARESIS));
    GetDlgItem(IDC_CHK_DYSPEPSIA_BLOAT_SATIETY)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_DYSPEPSIA_BLOAT_SATIETY));
    GetDlgItem(IDC_CHK_DYSPEPSIA)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_DYSPEPSIA));
    GetDlgItem(IDC_CHK_ANOREXIA)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_ANOREXIA));
    GetDlgItem(IDC_CHK_REFRACTORY_GERD)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_REFRACTORY_GERD));
    GetDlgItem(IDC_CHK_WEIGHTLOSS)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_WEIGHTLOSS));
    GetDlgItem(IDC_CHK_POST_DILATION_EVAL)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_POST_DILATION_EVAL));
    GetDlgItem(IDC_CHK_EVAL_EFFECT_MED)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_EVAL_EFFECT_MED));

    GetDlgItem(IDC_IFP_COMMENTS_TITLE)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_ADD_INDICATIONS));
    GetDlgItem(IDC_STATIC_ATTENDING_PHYSICIAN)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_ATTEND_PHYS));
    GetDlgItem(IDC_STATIC_REFERRING_PHYSICIAN)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_REFER_PHYS));
    GetDlgItem(IDC_STATIC_TECHNICIAN)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_TECHNICIAN));
    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrGet_text(ITEM_PSI_BEGIN_STUDY));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    GetDlgItem(IDC_PRESTUDY_HELP)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_Help));
    GetDlgItem(IDC_STATIC_INC_IN_RPT_DESC)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_INC_IN_RPT_DESC));

    //g_pLang->vEnd_section();
    }
  }




void CPreStudyInfo::OnBnClickedChkAttendPhys()
  {
  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_ATTENDING_PHYSICIAN, m_bChkAttendingPhysician);
  }

void CPreStudyInfo::OnBnClickedChkReferPhys()
  {
  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_REFERING_PHYSICIAN, m_bChkReferingPhysician);
  }

void CPreStudyInfo::OnBnClickedChkTechnician()
  {
  // TODO: Add your control notification handler code here
  UpdateData(TRUE);
  g_pConfig->vSet_inc_rpt_item(INC_TECHNICIAN, m_bChkTechnician);
  }
