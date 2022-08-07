/********************************************************************
RSAView.cpp

Copyright (C) 2003,2004,2005,2007,2012, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CRSAView class.

  This class implements the view that contains the RSA.


HISTORY:
08-NOV-03  RET  New.
10-FEB-05  RET  Changes for middle initial.  Change OnDraw().
10-MAY-07  RET  Research Version
                  Change OnInitialUpdate(), vAnalyze_data_for_RSA()
                    to use different number of max periods based
                    on research/standard version.
22-SEP-12  RET
             Change for 60/200 CPM
               Change vAnalyze_data_for_RSA() calculate the maximum frequency
                 points for the new frequencies.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "analyze.h"
#include "RSAView.h"
#include "GraphRSA.h"
#include "util.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRSAView

IMPLEMENT_DYNCREATE(CRSAView, CView)

/********************************************************************
Constructor

  Initialize class data.
********************************************************************/
CRSAView::CRSAView()
  {

  m_pfData = NULL;
  }

/********************************************************************
Destructor

  Clean up class data.
    - Free all allocated memory used for the data analyses.
********************************************************************/
CRSAView::~CRSAView()
  {

  if(m_pfData != NULL)
    HeapFree(GetProcessHeap(), 0, (LPVOID)m_pfData);
  }

BEGIN_MESSAGE_MAP(CRSAView, CView)
	//{{AFX_MSG_MAP(CRSAView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/********************************************************************
OnInitialUpdate

  Perform initialization of the view.
  Analyze the data for the RSA.
********************************************************************/
void CRSAView::OnInitialUpdate() 
  {

	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
#ifdef RSA_COLOR
  m_crPreWaterColor = PRE_WATER_COLOR;
#else
  m_crPreWaterColor = BLACK_RGB;
#endif

  m_pDoc = (CEGGSASDoc *)GetDocument();

  if(theApp.m_ptStudyType == PT_STANDARD)
    { // Standard version
    m_uMaxPeriods = m_pDoc->m_uMaxPeriods;
    }
  else
    { // Research version
      // Number of post_stimulation periods plus 1 baseline period.
    m_uMaxPeriods = m_pDoc->m_pdPatient.uNumPeriods + 1;
    }

  vAnalyze_data_for_RSA();
  }

/////////////////////////////////////////////////////////////////////////////
// CRSAView drawing

/********************************************************************
OnDraw - Message handler to display the data on the device which may be
         either the screen or a printer.

  inputs: Pointer to the device context.

  return: None.
********************************************************************/
void CRSAView::OnDraw(CDC* pDC)
  {
  CGraphRSA *pGraphRsa;
  RSA_DATA rsaData;

	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
  GetClientRect(&rsaData.rWin); // Not printing

//  rsaData.rWin.left += (LONG)((float)rsaData.rWin.right * (float).05);
  //// Narrowing the window cause the Time text to be displayed, but it is 
  //// backwards.  Without the narrowing, it gets displayed as a couple of line segments.
//  int iBottom;
//  iBottom = rsaData.rWin.bottom;
//  rsaData.rWin.bottom -= (LONG)((float)rsaData.rWin.bottom * (float).2);
//  rsaData.rWin.top += (LONG)((float)iBottom * (float).2);
//      WINDOWPLACEMENT wp;
//GetWindowPlacement(&wp);

  rsaData.uType = RSA_TYPE_FULL_SCREEN;
  rsaData.pfData = m_pfData;
  rsaData.iNumY = m_iMaxGraphs;
  rsaData.iNumX = m_iMaxFreqPts;
  rsaData.crPreWaterColor = m_crPreWaterColor;
  rsaData.crPostWaterColor = POST_WATER_COLOR;
  if(m_pDoc->m_pdPatient.bUseResrchRefNum == TRUE)
    rsaData.cstrPatientName.Format("%s", m_pDoc->m_pdPatient.cstrResearchRefNum);
  else
    vFormat_patient_name(&m_pDoc->m_pdPatient, rsaData.cstrPatientName, false);
  rsaData.cstrStudyDate = m_pDoc->m_pdPatient.cstrDate;

  pGraphRsa = new CGraphRSA();
  pGraphRsa->vDraw_graph(pDC, &rsaData, (CEGGSASDoc *)pDoc);
  delete pGraphRsa;
  }

/////////////////////////////////////////////////////////////////////////////
// CRSAView diagnostics

#ifdef _DEBUG
void CRSAView::AssertValid() const
{
	CView::AssertValid();
}

void CRSAView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRSAView message handlers


/////////////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
vAnalyze_data_for_RSA - Analyze the data for the RSA.

  inputs: None.

  return: None.
********************************************************************/
void CRSAView::vAnalyze_data_for_RSA()
  {
  ANALYZED_DATA adData;
  REQ_ANALYZE_DATA reqAnlData;
  short int iIndex, iIndex1;
  unsigned short uSize, uEpochCnt;
  float *pfData, fLastMinute, fStartMinute;
  bool bAddZeroLines;
  short int iChannel;
  bool bHalfMin;

    // Read the INI file for the channel to analyze.
  //iChannel = (short int)AfxGetApp()->GetProfileInt(INI_SEC_ANALYSIS, INI_ITEM_CHANNEL, 1);
  iChannel = g_pConfig->iGet_item(CFG_CHANNEL);
  m_pDoc = (CEGGSASDoc *)GetDocument();
  adData.pfRawData = NULL;
    // First find out how many data points will be needed and allocate an array.
    // total CPM * 4 so that we get four points for each cpm.
  m_iMaxGraphs = 0;
  m_iMaxFreqPts = m_pDoc->m_pAnalyze->m_iMaxFreqPoints; //((15 - 0) * 4) + 1;
  //if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
  //  m_iMaxFreqPts = ((60 - 0) * 4) + 1;
  uSize = 0;
  for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
    {
    if(m_pDoc->m_pdPatient.fGoodMinStart[iIndex] != INV_MINUTE)
      { // Now find out how many data points for this period.
        // Add and extra 2 for the blanks lines separating pre and post water periods.
      uSize += ((uGet_num_epochs(m_pDoc->m_pdPatient.fGoodMinStart[iIndex],
        m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]) + 2) * m_iMaxFreqPts);
      //uSize += (((unsigned short)m_pDoc->m_pdPatient.fGoodMinEnd[iIndex]
      //          - (unsigned short)m_pDoc->m_pdPatient.fGoodMinStart[iIndex] - 2 + 2)
      //          * m_iMaxFreqPts);
      }
    }
  if((m_pfData = (float *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
  uSize * sizeof(float))) != NULL)
    {
    pfData = m_pfData;
    bAddZeroLines = false;
    uEpochCnt = 0;
      // For each period that has good minutes defined.
    for(iIndex = 0; iIndex < m_uMaxPeriods; ++iIndex)
      {
      if(m_pDoc->m_pdPatient.fGoodMinStart[iIndex] != INV_MINUTE)
        { // Now for each valid period, analyze all the 4 minute epochs.
        // Read in and graph the baseline period.
        if(m_pDoc->iRead_period(iIndex, m_pDoc->m_fData) == SUCCESS)
          {
          if(bAddZeroLines == true)
            { // Add 2 lines of 0's to separate pre and post water periods.
            for(iIndex1 = 0; iIndex1 < (m_iMaxFreqPts * 2); ++iIndex1)
              {
              *pfData = (float)0.0;
              ++pfData;
              }
            m_iMaxGraphs += 2;
            bAddZeroLines = false;
            }
          reqAnlData.iChannel = iChannel; //0; // EGG channel.
          reqAnlData.iNumChannelsInFile = 2;
          reqAnlData.iType = RSA_TYPE;
          reqAnlData.padData = &adData;
          reqAnlData.bUsePartMinAtDataEnd = false;
          reqAnlData.uTimeBand = iIndex;
          fLastMinute = m_pDoc->m_pdPatient.fGoodMinEnd[iIndex] - 4;
          fStartMinute = m_pDoc->m_pdPatient.fGoodMinStart[iIndex];
          bHalfMin = bIs_half_minute(fStartMinute);
          while(fStartMinute <= fLastMinute)
            {
            if(theApp.m_ptStudyType == PT_STANDARD)
              reqAnlData.fEpochNum = fStartMinute; // Standard study
            else // Research study
              reqAnlData.fEpochNum = fStartMinute - m_pDoc->m_pdPatient.fPeriodStart[iIndex];
            //reqAnlData.uRunningEpochCnt = 0;
            reqAnlData.uRunningEpochCnt = uEpochCnt;
            if(m_pDoc->m_pAnalyze->bAnalyze_data(&reqAnlData) == true)
              { // Save the data for graphing.
              memcpy((unsigned char *)pfData, adData.fData, m_iMaxFreqPts * sizeof(float));
              pfData += m_iMaxFreqPts;
              ++m_iMaxGraphs;
              }
            else
              break;
            // Find out if start minutes are incremented on the half or whole minute.
            if(bHalfMin == true)
              fStartMinute += 0.5F;
            else
              ++fStartMinute;
            ++uEpochCnt;
            }
          if(iIndex == PERIOD_BL)
            bAddZeroLines = true;
          }
        else
          break;
        }
      }
    }
  }



