/********************************************************************
GraphRSA.cpp

Copyright (C) 2003,2004,2012,2016,2020 3CPM Company, Inc.  All rights reserved.

  Implements the CGraphRSA class.

  This class draws the RSA graph.

  The procedure is as follows:
    - Fill in a RSA_DATA data structure.
        The rWin member is only used for screen display, not for printing.
    - Create the CGraphRSA object .
    - to draw the graph, Call the vDraw_graph() member with the CDC object for
      the device to be drawn to and a pointer to the data (RSA_DATA data
      structure) it needs to draw the graph.
    - delete the CGraphRSA object.


HISTORY:
18-AUG-03  RET  New.
20-FEB-04  RET  Change vDraw_graph() to put the X axis label in the resource file.
21-FEB-12  RET  Version 
                  Changes for foreign languages.
25-SEP-12  RET
             Change to make the water ingestion phase key lines and text look
             better when printed and to properly print RSA when included on
             other than first page of report.
               Change method: vDraw_graph()
29-SEP-12  RET
             Change for 60/200 CPM
               Change methods: vDraw_graph()
14-DEC-12  RET
             Change to filter out frequencies below 10 cpm in the RSA.
               Change methods: vDraw_graph()
02-JUN-16  RET
             Add feature to hide study dates.
               Change methods: vDraw_graph()
16-NOV-20  RET
             Add new version CDDRE
               Change methods: vDraw_graph()
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "GraphRSA.h"
#include "util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

VOID CALLBACK vAdd_segment(int x, int y, PTS *pts);
VOID CALLBACK vCount_points(int x, int y, LPDWORD lpdwNumPts);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/********************************************************************
Constructor

  inputs: None.
********************************************************************/
CGraphRSA::CGraphRSA()
  {

  m_pDC = NULL;
  m_iPatientNameExt = 0;
  m_iWaterIngestExt = 0;
  m_pptArrayStart = NULL;
  }

/********************************************************************
Destructor
********************************************************************/
CGraphRSA::~CGraphRSA()
  {

  if(m_pptArrayStart != NULL)
    GlobalFree(m_pptArrayStart);
  }

/********************************************************************
vDraw_graph - Draw the graph.

  inputs: Pointer the CDC object we are drawing to.
          Pointer to a RSA_DATA data structure that contains the data
            and other information for graphing.

  return: None.
********************************************************************/
void CGraphRSA::vDraw_graph(CDC *pDC, RSA_DATA *pRSA, CEGGSASDoc *pDoc)
  {
  CFont cfFont;
  TEXTMETRIC tm;
  CPoint point, ptPrev, ptRgn[4], ptTemp;
  CPoint cpPrevLine1, cpPrevLine2;
  CRgn Rgn;
  CSize csText;
  CString cstrText;
  POINT *pptArray, *pptArrayStart;
  LPPOINT lpTop, lpBot;
//  CPen penBlack, penBlackThick, penPreWater, penPostWater;
  short int iIndex, iPt, iYIndex, iYBase, iNumZeros;
  short int iYRange; 
  short int iLeftMargin, iRightMargin, iBottomMargin;
  int iXOrg, iRight, iDCSave, iYMarginForLabel, iBottomOffset, iYIndexMax, iX;
  float *pfData, *pfEpoch;
  float fMax, fData, fScale; // fDataPos, 
  DWORD dwTop, dwBot;
  char szYLabel[50];
  int iNumX;  // Number of points to plot along the X axis.
  int iMaxY = 0;
  int iDataStartIndex; //, iMaxPt, iMaxYDataIndex; iMaxYEpochIndex, 

  iBottomOffset = 5;
  iDCSave = -1;
  lpTop = NULL;
  lpBot = NULL;
  m_pDC = pDC;
  if(g_bFilterRSA == false)
    iNumX = pRSA->iNumX;
  else
    iNumX = pRSA->iNumX - (10 * 4);

    // create and select a solid white brush
  CBrush brushWhite(RGB(255, 255, 255));
//  m_brushWhite.CreateSolidBrush(RGB(255, 255, 255));
  CBrush* pOldBrush = m_pDC->SelectObject(&brushWhite);

  m_pDC->GetTextMetrics(&tm);
  if(m_pDC->IsPrinting() == 0)
    { // Not printing. Clear the client area 
    m_pDC->PatBlt(pRSA->rWin.left, pRSA->rWin.top, pRSA->rWin.right, pRSA->rWin.bottom, WHITENESS);
    }
  m_pDC->LPtoDP(&pRSA->rWin);

    // create a thin, black pen
  CPen m_penBlack(PS_SOLID, 1, RGB(0, 0, 0));
    // Create pens for the pre and post water load.
#ifdef RSA_COLOR
  CPen m_penPreWater(PS_SOLID, 1, pRSA->crPreWaterColor);
  CPen m_penPostWater(PS_SOLID, 1, pRSA->crPostWaterColor);
#else
  m_penPreWater.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
  m_penPostWater.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
#endif
    // create a thick, black pen
  CPen m_penBlackThick(PS_SOLID, 3, RGB(0, 0, 0));

    // Select the thin black pen.
  CPen* pOldPen = m_pDC->SelectObject(&m_penBlack);

  m_pDC->StrokeAndFillPath();

  if(pRSA->uType == RSA_TYPE_FULL_SCREEN)
    { // Full screen view of RSA.
      // Calculate the left margin of the graph.
    iLeftMargin = (short int)((float)pRSA->rWin.right * LEFT_MARGIN);
      // Calculate the right margin of the graph.
    iRightMargin = (short int)((float)pRSA->rWin.right * RIGHT_MARGIN);
    }
  else
    { // Small RSA for report.
      // Calculate the left margin of the graph.
    iLeftMargin = (short int)((float)(pRSA->rWin.right - pRSA->rWin.left) * LEFT_MARGIN * 3);
    iRightMargin = (short int)((float)(pRSA->rWin.right - pRSA->rWin.left) * RIGHT_MARGIN * 3);
    }
  if(m_pDC->IsPrinting() != 0)
    { // Printing, use a smaller font.
    iDCSave = ::SaveDC(m_pDC->m_hDC);
    vSet_font(NULL, m_pDC, &cfFont, FW_THIN, 10);
    m_pDC->SelectObject(&cfFont);
    }

    // Calculate the bottom margin of the graph.
  iBottomMargin = (short int)(tm.tmHeight * 4); // Allow room for text at bottom of graph.

    // Draw vertical axis
  point.x = pRSA->rWin.left + iLeftMargin;
  point.y = pRSA->rWin.top + 5;
    // Starting point for drawing on the X axis.  Leave 2 pixels so
    // the Y axis doesn't get overwritten when drawing.
  m_iXStart = point.x + 2;
  iXOrg = point.x;
	vDev_to_log_coord(&point);
	m_pDC->MoveTo(point);
  point.x = iXOrg;
  point.y = pRSA->rWin.bottom - iBottomMargin;
  m_iYBottom = point.y;
  point.y += iBottomOffset;
	vDev_to_log_coord(&point);
	m_pDC->LineTo(point);

    // Calculate the distance between each pseudo X axis.
  iYRange = (m_iYBottom - iBottomOffset) - (short int)(pRSA->rWin.top + 10) - 5;
  m_iGraphSeparation = (short int)((float)iYRange * (float)0.65 / (float)pRSA->iNumY);
  //m_iGraphSeparation = (short int)((float)iYRange * (float)0.35 / (float)pRSA->iNumY);

    // Draw horizontal axis
  point.x = iXOrg;
  point.y = m_iYBottom + iBottomOffset;
  //m_iXInc = m_iXStart;
	vDev_to_log_coord(&point);
	m_pDC->MoveTo(point);
  point.x = pRSA->rWin.right - iRightMargin;
  //point.y = m_iYBottom + 10;
  iRight = point.x; // Right extent (side) of graph.
    // Calculate the number of pixels between data points.
  //m_iXInc -= (short int)point.x;
  //m_iXInc = abs(m_iXInc) / iNumX;
  m_iXInc = (point.x - m_iXStart) / iNumX;
  //if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
  //  m_iXInc += 1;
  point.x = m_iXStart + (iNumX  * m_iXInc);
	vDev_to_log_coord(&point);
	m_pDC->LineTo(point);

    // Draw hash mark and text for 3,6,9,12,15 cpm.
  point.x = m_iXStart;
  //point.y = m_iYBottom + iBottomOffset;
  if(g_bFilterRSA == false)
    {
    vLabel_x_axis(&point, m_iXInc, 3, "3");
    vLabel_x_axis(&point, m_iXInc, 6, "6");
    vLabel_x_axis(&point, m_iXInc, 9, "9");
    }
  vLabel_x_axis(&point, m_iXInc, 12, "12");
  vLabel_x_axis(&point, m_iXInc, 15, "15");
  if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM
    || theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
    {
    vLabel_x_axis(&point, m_iXInc, 20, "20");
    vLabel_x_axis(&point, m_iXInc, 30, "30");
    vLabel_x_axis(&point, m_iXInc, 40, "40");
    vLabel_x_axis(&point, m_iXInc, 50, "50");
    vLabel_x_axis(&point, m_iXInc, 60, "60");
    if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
      {
      vLabel_x_axis(&point, m_iXInc, 70, "70");
      vLabel_x_axis(&point, m_iXInc, 80, "80");
      vLabel_x_axis(&point, m_iXInc, 90, "90");
      vLabel_x_axis(&point, m_iXInc, 100, "100");
      vLabel_x_axis(&point, m_iXInc, 120, "120");
      vLabel_x_axis(&point, m_iXInc, 140, "140");
      vLabel_x_axis(&point, m_iXInc, 160, "160");
      vLabel_x_axis(&point, m_iXInc, 180, "180");
      vLabel_x_axis(&point, m_iXInc, 200, "200");
      }
    }

  // Write legend for X axis:
    //   "Frequency (cpm)"
    //   patient name, date channel
  //cstrText.LoadString(IDS_FREQ_CPM);
  cstrText = g_pLang->cstrLoad_string(ITEM_G_FREQ_CPM);
  if(m_iPatientNameExt == 0)
    {
    csText = m_pDC->GetOutputTextExtent(cstrText);
    m_iPatientNameExt = csText.cx;
    }
    // Center the text.
  if(m_pDC->IsPrinting() != 0)
    { // Printing.  Centering is done based on width of the rectangle.
    point.x = ((pRSA->rWin.right - pRSA->rWin.left - m_iPatientNameExt) / 2)
               + pRSA->rWin.left + (iLeftMargin / 2);
    }
  else
    point.x = (m_iXStart / 2) + ((iRight - m_iPatientNameExt) / 2);
  if(m_pDC->IsPrinting() != 0)
    point.y = m_iYBottom + tm.tmHeight + (tm.tmHeight / 2);
  else
    point.y = m_iYBottom + (tm.tmHeight * 2); // + (tm.tmHeight / 2);
	vDev_to_log_coord(&point);
  m_pDC->TextOut(point.x, point.y, cstrText);

  if(pRSA->cstrPatientName.IsEmpty() == FALSE)
    {
#if EGGSAS_SUBTYPE == SUBTYPE_HIDE_STUDY_DATES
#if EGGSAS_READER == READER
      cstrText.Format("%s", pRSA->cstrPatientName);
#else
    if(pRSA->cstrStudyDate.GetLength() <= 10)
      cstrText.Format("%s, %s", pRSA->cstrPatientName, pRSA->cstrStudyDate);
    else
      cstrText.Format("%s", pRSA->cstrPatientName);
#endif
#else
    cstrText.Format("%s, %s", pRSA->cstrPatientName, pRSA->cstrStudyDate);
#endif
    csText = m_pDC->GetOutputTextExtent(cstrText);
      // Center the text.
    point.x = m_iXStart + ((iRight - csText.cx) / 2);
    point.y = m_iYBottom + (tm.tmHeight * 3);
	  vDev_to_log_coord(&point);
    m_pDC->TextOut(point.x, point.y, cstrText);
    }

    // Look through the data and pick out the max and min.
  pfData = pRSA->pfData;
  m_iZeroLine1 = -1;
  m_iZeroLine2 = -1;
  iIndex = 1;
  if(g_bFilterRSA == false)
    {
    m_fMin = *pfData;
    fMax = *pfData;
    }
  else
    {
    m_fMin = *(pfData + (10 * 4));
    fMax = *(pfData + (10 * 4));
    }

  if(g_bFilterRSA == false)
    iDataStartIndex = 0;
  else
    iDataStartIndex = 10 * 4;
  iYIndexMax = 0;
  for(iYIndex = 0; iYIndex < pRSA->iNumY; ++iYIndex)
    {
    iNumZeros = 0;
    pfEpoch = pfData + (iYIndex * pRSA->iNumX);
    for(iIndex = iDataStartIndex; iIndex < pRSA->iNumX; ++iIndex)
      {
      fData = *(pfEpoch + iIndex);
      if(fData != (float)0.0)
        {
        if(fData < m_fMin)
          m_fMin = fData; // Got a new minimum.
        else if(fData > fMax)
          {
          fMax = fData; // Got a new maximum.
          iYIndexMax = iYIndex;
          }
        }
      else
        ++iNumZeros;
      }
    if(iNumZeros == iNumX)
      { // Save the indexes into the data array of the lines containing all 0's.
      if(m_iZeroLine1 == -1)
        m_iZeroLine1 = iYIndex;
      else
        m_iZeroLine2 = iYIndex;
      }
    }
  // datapnt = YSCALE * (ydata - m_fGraphMin)
  // YSCALE = YSIZE / (m_fGraphMax - m_fGraphMin)
  //fScale = iYRange / (fMax - m_fMin);
  iX = (m_iGraphSeparation * iYIndexMax) + iYIndexMax + m_iGraphSeparation + 6;
  fScale = (((float)iYRange * .75F) - (float)iX) / (fMax - m_fMin);
  //  // Calculate the range over which we can draw the Y values of the graph.
  //m_fRange = fMax - m_fMin;
  //m_fYRange = (float)(iYRange - (pRSA->iNumY * m_iGraphSeparation));
  //m_fYRange *= 2048;
  //m_fYRange += (3 * m_iGraphSeparation);
  //m_fYRange = (float)iYRange;

  m_pDC->SelectObject(&m_penPostWater);
  m_pptArrayStart = (POINT *)GlobalAlloc(GPTR, (iNumX + 1) * sizeof(POINT) * pRSA->iNumY);

  // determine what point is highest on screen.
  float fTemp;
  for(iYIndex = pRSA->iNumY - 1; iYIndex >= 0; --iYIndex)
    {
    pfEpoch = pfData + (iYIndex * pRSA->iNumX);
    for(iIndex = iDataStartIndex; iIndex < pRSA->iNumX; ++iIndex)
      {
        // Calculate the Y pixel for the data point.
      iPt = (short int)(fScale * (*(pfEpoch + iIndex) - m_fMin)) + ((iYIndex + 1) * m_iGraphSeparation);
      if(iPt != 0)
        { // New max pixel.
        if(iPt > iMaxY)
          iMaxY = iPt;
        }
      }
    }
  if(iMaxY < (short int)((float)iYRange * .95F))
    { // The max point on the screen doesn't take up the full height, adjust
      // the scaling factor to make the max point take up most of the height.
    fTemp = 1.05F - ((float)iMaxY / (float)iYRange);
    iYRange += (short int)((float)iYRange * fTemp);
    fScale = (((float)iYRange * .75F) - (float)iX) / (fMax - m_fMin);
    }

    // Draw the graph.
    // The first index represents the number of graphs to draw.
    // The second index is the number of points along the X axis.
    // The graphs get plotted in reverse order.
    // Each graph gets plotted at a pseudo X axis.  This pseudo X axis
    //   is calculated by adding the number of graphs left to plot times
    //   the distance between each pseudo X axis.
  pptArray = m_pptArrayStart;
  // Get the number of epochs for the last period.
  int iPeriodIndex = pDoc->m_uMaxPeriods - 1;
  int iNumOfEpochs = (int)uGet_num_epochs(pDoc->m_pdPatient.fGoodMinStart[iPeriodIndex],
      pDoc->m_pdPatient.fGoodMinEnd[iPeriodIndex]);
  //bool bHalfMin = bIs_half_minute(pDoc->m_pdPatient.fGoodMinStart[iPeriodIndex]);

      iYBase = m_iYBottom - ((pRSA->iNumY) * m_iGraphSeparation);
  for(iYIndex = pRSA->iNumY - 1; iYIndex >= 0; --iYIndex)
    {
    if(--iNumOfEpochs < 0)
      { // Get next previous period.
      --iPeriodIndex;
      if(iPeriodIndex >= 0)
        {
        iNumOfEpochs = (int)uGet_num_epochs(pDoc->m_pdPatient.fGoodMinStart[iPeriodIndex],
            pDoc->m_pdPatient.fGoodMinEnd[iPeriodIndex]);
        //bHalfMin = bIs_half_minute(pDoc->m_pdPatient.fGoodMinStart[iPeriodIndex]);
        }
      }
    pptArrayStart = pptArray;
    if(iYIndex == m_iZeroLine1 || iYIndex == m_iZeroLine2)
      m_pDC->SelectObject(&m_penBlackThick);  // Zero lines separating pre-water and post-water.
    else if(iYIndex == (m_iZeroLine1 - 1))
      m_pDC->SelectObject(&m_penPreWater);

    //if(bHalfMin == true)
    //  iYBase += (m_iGraphSeparation / 2);
    //else
      iYBase += m_iGraphSeparation;
    //if(bHalfMin == true)
    //  iYBase = m_iYBottom - ((iYIndex + 1) * m_iGraphSeparation / 2);
    //else
    //  iYBase = m_iYBottom - ((iYIndex + 1) * m_iGraphSeparation);
    pptArray->x = m_iXStart;
    pptArray->y = iYBase;
    pfEpoch = pfData + (iYIndex * pRSA->iNumX);
      // First calculate all the points and clear the regions under the points.
      // This removes the lines from any previous graphs that this graph goes
      // through.
    for(iIndex = iDataStartIndex; iIndex < pRSA->iNumX; ++iIndex)
      {
      ptRgn[0].x = pptArray->x;
      ptRgn[0].y = m_iYBottom;
      ptRgn[1].x = pptArray->x;
      ptRgn[1].y = pptArray->y;
      ++pptArray;
        // Calculate the point on the Y axis of where to put the point.
      //fDataPos = (*(pfEpoch + iIndex) - m_fMin) / m_fRange;
      //iPt = (short int)(fDataPos * m_fYRange);
      // datapnt = YSCALE * (ydata - m_fGraphMin)
      iPt = (short int)(fScale * (*(pfEpoch + iIndex) - m_fMin));

      pptArray->x = (pptArray - 1)->x + m_iXInc;
      pptArray->y = iYBase - iPt;

      ptRgn[2].x = pptArray->x;
      ptRgn[2].y = pptArray->y;
      ptRgn[3].x = pptArray->x;
      ptRgn[3].y = m_iYBottom;

      Rgn.CreatePolygonRgn(ptRgn, 4, ALTERNATE);
      m_pDC->FillRgn(&Rgn, &brushWhite);
      Rgn.DeleteObject();
      }
      // Now draw the graph.
    pptArray = pptArrayStart;
    vDev_to_log_coord(pptArray);
    m_pDC->MoveTo(*pptArray);
    for(iIndex = 0; iIndex < iNumX; ++iIndex)
      {
      ++pptArray;
      vDev_to_log_coord(pptArray);
      m_pDC->LineTo(*pptArray);
      m_pDC->MoveTo(*pptArray);
      }
    ++pptArray;
    }
  /**/

    // Add a key for the water load to the top of the graph.
  if(theApp.m_ptStudyType == PT_STANDARD)
    cstrText = g_pLang->cstrLoad_string(ITEM_G_WATER_INGESTION_PHASE);
    //cstrText.LoadString(IDS_WATER_INGESTION_PHASE);
  else
    cstrText = g_pLang->cstrLoad_string(ITEM_G_STIMULATION_MEDIUM_PHASE);
    //cstrText.LoadString(IDS_STIMULATION_MEDIUM_PHASE);
  if(m_iWaterIngestExt == 0)
    {
    csText = m_pDC->GetOutputTextExtent(cstrText);
    m_iWaterIngestExt = csText.cx;
    }
  int iLineLen, iLineSpace, iTextSpace;
  if(m_pDC->IsPrinting() != 0)
    {
    iLineLen = 50;
    iLineSpace = 20;
    iTextSpace = 20;
    }
  else
    {
    iLineLen = 15;
    iLineSpace = 10;
    iTextSpace = 5;
    }

    // Right justify the text on the line.  Add 12 pixels for the double line.
  // Draw top line.
  point.x = (m_iXStart + (iNumX * m_iXInc)) - m_iWaterIngestExt - iLineLen - iTextSpace;
  point.y = pRSA->rWin.top + tm.tmHeight + (tm.tmHeight / 2) + 2;
  m_pDC->SelectObject(&m_penBlackThick);
  ptTemp = point;
	vDev_to_log_coord(&ptTemp);
  m_pDC->MoveTo(ptTemp);
  ptTemp.x = point.x + iLineLen; //10;
  ptTemp.y = point.y;
	vDev_to_log_coord(&ptTemp);
  m_pDC->LineTo(ptTemp);
  // Draw bottom line.
  point.y = pRSA->rWin.top + tm.tmHeight + (tm.tmHeight / 2) + 2 + iLineSpace; //10; //iGraphSeparation;
  ptTemp = point;
	vDev_to_log_coord(&ptTemp);
  m_pDC->MoveTo(ptTemp);
  ptTemp.x = point.x + iLineLen; //10;
  ptTemp.y = point.y;
	vDev_to_log_coord(&ptTemp);
  m_pDC->LineTo(ptTemp);

  point.y = pRSA->rWin.top + tm.tmHeight + (tm.tmHeight / 2);
  point.x += 5;
  m_pDC->SelectObject(&m_penBlack);
	vDev_to_log_coord(&point);
  m_pDC->TextOut(point.x + iLineLen + iTextSpace, point.y, cstrText);
  if(m_pDC->IsPrinting() != 0)
    { // Printing, put the previous font back.
    cfFont.DeleteObject();
    if(iDCSave > 0)
      ::RestoreDC(m_pDC->m_hDC, iDCSave);
    }

    // Write legend for Y axis.  Make it vertical.
  iDCSave = ::SaveDC(m_pDC->m_hDC);
  vSet_font(NULL, m_pDC, &cfFont, FW_THIN, 20);
  m_pDC->SelectObject(&cfFont);

 // lpTop = NULL;
	//lpBot = NULL;
  dwTop = 2;
	dwBot = 2;

    //  "Time (1 minute between lines)"
  lpTop = (LPPOINT)GlobalAlloc(GPTR, MAXGUIDESEGMENTS * sizeof(POINT));
  lpBot = (LPPOINT)GlobalAlloc(GPTR, MAXGUIDESEGMENTS * sizeof(POINT));
  if(lpTop != NULL && lpBot != NULL)
    {
    if(m_pDC->IsPrinting() == 0)
      { // Not printing
      if(pRSA->uType == RSA_TYPE_FULL_SCREEN)
        { // Full screen view of RSA.
          // Adjust the size of the Y coordinates of the rectangle.
          // If Y offset is too large, the text comes out from bottom to top.
          // If Y is too small, there is no text.
          // Use a percentage of the entire client area of the window.
        int iYOffset;
        iYOffset = (int)((float)(pRSA->rWin.bottom - pRSA->rWin.top) * (float).4056);
//        iYOffset = (int)((float)(pRSA->rWin.bottom - pRSA->rWin.top) * (float).39);
        lpTop->x = pRSA->rWin.left + iLeftMargin - 15;
        lpTop->y = pRSA->rWin.top + iYOffset; //275; //125;
        (lpTop + 1)->x = pRSA->rWin.left + iLeftMargin - 15;
        (lpTop + 1)->y = m_iYBottom - iYOffset; //275; //125;

        lpBot->x = pRSA->rWin.left + 15;
        lpBot->y = pRSA->rWin.top + iYOffset; //275; //125;
        (lpBot + 1)->x = pRSA->rWin.left + 15;
        (lpBot + 1)->y = m_iYBottom - iYOffset; //275; //125;
        }
      else
        { // Small RSA for report.
        iYMarginForLabel = (int)((float)(pRSA->rWin.bottom - pRSA->rWin.top) * .45F);
        lpTop->x = pRSA->rWin.left + iLeftMargin - (iLeftMargin / 4); //15;
        lpTop->y = pRSA->rWin.top + iYMarginForLabel;
//        lpTop->y = pRSA->rWin.top + ((pRSA->rWin.bottom - pRSA->rWin.top) / 5);
        (lpTop + 1)->x = lpTop->x; //pRSA->rWin.left + iLeftMargin - 15;
        (lpTop + 1)->y = pRSA->rWin.bottom - iYMarginForLabel;
//        (lpTop + 1)->y = pRSA->rWin.bottom - ((pRSA->rWin.bottom - pRSA->rWin.top) / 5);

        lpBot->x = pRSA->rWin.left + (iLeftMargin / 4); //15;
        lpBot->y = lpTop->y; //pRSA->rWin.top + 80; //100;
        (lpBot + 1)->x = lpBot->x; //pRSA->rWin.left + 15;
        (lpBot + 1)->y = (lpTop + 1)->y; //m_iYBottom - 80; //100;
        }
      }
    else
      { // Printing, small RSA only.
        // Make the vertical text smaller than on the screen.
      iYMarginForLabel = (int)((float)(pRSA->rWin.bottom - pRSA->rWin.top) * .45F);
      lpTop->x = pRSA->rWin.left + iLeftMargin - (iLeftMargin / 4); //20; //15;
      lpTop->y = pRSA->rWin.top + iYMarginForLabel;
//      lpTop->y = pRSA->rWin.top + ((pRSA->rWin.bottom - pRSA->rWin.top) / 5); //175; //200; //350;
      (lpTop + 1)->x = lpTop->x; //pRSA->rWin.left + iLeftMargin - 20; //15;
      (lpTop + 1)->y = pRSA->rWin.bottom - iYMarginForLabel;
//      (lpTop + 1)->y = pRSA->rWin.bottom - ((pRSA->rWin.bottom - pRSA->rWin.top) / 5); //m_iYBottom - 175; //200; //350;

      lpBot->x = pRSA->rWin.left + (iLeftMargin / 4); //iLeftMargin - 50;
      lpBot->y = lpTop->y; //pRSA->rWin.top + 175; //200; //350;
      (lpBot + 1)->x = lpBot->x; //pRSA->rWin.left + iLeftMargin - 50;
      (lpBot + 1)->y = (lpTop + 1)->y; //m_iYBottom - 175; //200; //350;
      }
      // Expand the line segment into points
    bFill_out(&lpTop, &dwTop);
    bFill_out(&lpBot, &dwBot);
      // Display the text.
    //cstrText.LoadString(IDS_TIME);
    cstrText = g_pLang->cstrLoad_string(ITEM_G_TIME);
    strcpy(szYLabel, cstrText);
    bText_effect(lpTop, lpBot, dwTop, dwBot, szYLabel, FALSE);
    }
/***/
  if(lpTop != NULL)
    GlobalFree(lpTop);
  if(lpBot != NULL)
    GlobalFree(lpBot);
    // Put the previous font back.
  if(iDCSave > 0)
    ::RestoreDC(m_pDC->m_hDC, iDCSave);
  }

/********************************************************************
vLabel_x_axis - Put a hash mark and its label on the X axis.

  inputs: Pointer to a CPoint structure that marks the start of the X axis.
          Number of pixels to increment the X axis for to get to the CPM.
          CPM of the hash mark.
          Text to put as the label for the hash mark.

  return: None.
********************************************************************/
void CGraphRSA::vLabel_x_axis(CPoint *ppntStart, short int iXInc,
                              short int iCpm, char *pszText)
  {
  CPoint point;
  short int iX;

  if(g_bFilterRSA == false)
    iX = iXInc * iCpm * 4;
  else
    iX = iXInc * (iCpm - 10) * 4;

  //point.x = ppntStart->x + (iXInc * iCpm * 4);
  point.x = ppntStart->x + iX;
  point.y = ppntStart->y;
	vDev_to_log_coord(&point);
  m_pDC->MoveTo(point);
  point.x = ppntStart->x + iX; //(iXInc * iCpm * 4);
  point.y = ppntStart->y + X_AXIS_HASH_MARK_LEN;
	vDev_to_log_coord(&point);
	m_pDC->LineTo(point);
  //point.x = ppntStart->x + (iXInc * iCpm * 4) - 2;
  point.x = ppntStart->x + iX - 2;
  point.y = ppntStart->y + Y_OFFSET_X_HASH_MARK_LABEL;
	vDev_to_log_coord(&point);
  m_pDC->TextOut(point.x, point.y, pszText);
  }

/********************************************************************
bPoly_draw95 - Draws the points returned from a call to GetPath() to an HDC.

               This function is similar to the Windows NT function
               PolyDraw which draws a set of line segments and Bézier
               curves.  Since PolyDraw is not supported on Windows 95
               this bPoly_draw95 is used instead.

  inputs:  handle of a device context 
           address of array of points 
           address of line and curve identifiers
           count of points 

  return: true
********************************************************************/
bool CGraphRSA::bPoly_draw95(CONST LPPOINT lppt, CONST LPBYTE lpbTypes, int  cCount)
  {
  int i;
  
  for(i = 0; i < cCount; i++) 
    {
    switch(lpbTypes[i])
      {
      case PT_MOVETO : 
         m_pDC->MoveTo(lppt[i].x, lppt[i].y); 
         break;
      case PT_LINETO | PT_CLOSEFIGURE:
      case PT_LINETO : 
         m_pDC->LineTo(lppt[i].x, lppt[i].y); 
         break;
      case PT_BEZIERTO | PT_CLOSEFIGURE:
      case PT_BEZIERTO :
	     m_pDC->PolyBezierTo(&lppt[i], 3);
       i+=2;
       break;
      }
    }
  return(true);
  }  


/********************************************************************
bRender_path_points - Renders the points returned from a call to GetPath()
                      by converting them back into a path and calling either
                      FillPath or StrokePath to display them.

                      The R2_MERGEPENNOT is used here so that things like the
                      inside of and "a" or an "o" get painted correctly when
                      using FillPath.
********************************************************************/
bool CGraphRSA::bRender_path_points(LPPOINT lpPoints, LPBYTE lpTypes,
int iNumPts, BOOL bOutline)
  {
  HPEN hPen;
  HBRUSH hBrush;
  int iROP2;

  m_pDC->BeginPath(); // Draw into a path so that we can use FillPath()

  bPoly_draw95(lpPoints, lpTypes, iNumPts);

  m_pDC->CloseFigure();
  m_pDC->EndPath();   

    // Draw the path
  if (bOutline)  
    m_pDC->StrokePath(); // As an outline
  else 
    {              // As solid figures
    iROP2 = m_pDC->SetROP2(R2_MERGEPENNOT);
    hPen = (HPEN)m_pDC->SelectObject(GetStockObject(NULL_PEN)); // Just say "no" to outlines
    hBrush = (HBRUSH)m_pDC->SelectObject(GetStockObject(BLACK_BRUSH)); // Paint It Black

    m_pDC->FillPath();  

      // Restore the DC to its previous state
    m_pDC->SetROP2(iROP2);        
    m_pDC->SelectObject(hPen);
    m_pDC->SelectObject(hBrush);
    } 
   return(true);
  }

/********************************************************************
GetRealTextExtent - Makes sure that the extents in a SIZE are large enough
                    to bound the points in an array.

                    Why not just use GetTextExtentPoint32?  Well, the text
                    extents returned by GetTextExtentPoint32 don't include
                    the overhang of an italics character into the next
                    cell.  This function expands the extents to include
                    the greatest x, and y extents of the string data.

  inputs: Array of points.
          Number of points in the array.
          Pointer to a SIZE structure the gets the extent.

  return: None.
*********************************************************************/
void CGraphRSA::GetRealTextExtent(LPPOINT lpPoints, int iNumPts, LPSIZE size)
  {
  int i;

  for(i = 0; i < iNumPts; i++)
    {
    if (lpPoints[i].x > size->cx)
      size->cx = lpPoints[i].x;
    if (lpPoints[i].y > size->cy)
      size->cy = lpPoints[i].y;
    }
  }

/********************************************************************
bText_effect - Display the text along the path.

  inputs: DC to display into
          Top guide line
          Bottom guide line
          Number of points in top guide
          Number of points in bottom guide
          Text string to apply effects to
          Print as outline or as solid text

  return: true if the text was displayed.
          false if there was an error.
********************************************************************/
bool CGraphRSA::bText_effect(LPPOINT lpTop, LPPOINT lpBot, DWORD dwTopPts,
                            DWORD dwBotPts, LPSTR szText, BOOL bOutlineOnly)
  {
  LPPOINT lpPoints;		  // Path data points
  LPBYTE lpTypes;			  // Path data types
  int i, iNumPts;
  SIZE size;				  // Text size info
  float fXScale, fYScale;	  // Scaling values
  int iTopInd, iBotInd;	  // Guide array indices
  bool bRelocErr, bRet = false;

  lpPoints = NULL;
  lpTypes = NULL;
    // Set to transparent so we dont get an outline around the text string
  m_pDC->SetBkMode(TRANSPARENT);
     
    // Output the text into a path
  m_pDC->BeginPath();
  m_pDC->TextOut(0, 0, szText, strlen(szText));
  m_pDC->EndPath();    
     
    // How many points are in the path
  if((iNumPts = m_pDC->GetPath(NULL, NULL, 0)) != -1)
    { // Allocate room for the points
    if((lpPoints = (LPPOINT)GlobalAlloc(GPTR, sizeof(POINT) * iNumPts)) != NULL)
      { // Allocate room for the point types
      if((lpTypes = (LPBYTE)GlobalAlloc(GPTR, iNumPts)) != NULL)
        { // Get the points and types from the current path
          // Even more error checking
        if((iNumPts = m_pDC->GetPath(lpPoints, lpTypes, iNumPts)) != -1)
          { // Get extents of the text string for scaling purposes  
          GetTextExtentPoint32(m_pDC->m_hDC, szText, strlen(szText), &size);
            // OK, but lets make sure our extents are big enough (handle italics fonts)
          GetRealTextExtent(lpPoints, iNumPts, &size);
            // Relocate the points in the path based on the guide lines
          bRelocErr = false;
          for(i = 0; i < iNumPts; i++)
            { // How far along is this point on the x-axis
            fXScale = (float)lpPoints[i].x / (float)size.cx;
              // What point on the top guide does this coorespond to
            iTopInd = (int)(fXScale * (dwTopPts-1));
          	  // What point on the bottom guide does this coorespond to
            iBotInd = (int)(fXScale * (dwBotPts-1));
            if(iTopInd < 0 || iTopInd > (int)dwTopPts || iBotInd < 0 || iBotInd > (int)dwBotPts)
              {
              bRelocErr = true;
              break;
              }
              // How far along is this point on the y-axis
            fYScale = (float)lpPoints[i].y / (float)size.cy;
              // Scale the points to their new locations
            lpPoints[i].x = (int)((lpBot[iBotInd].x * fYScale) + (lpTop[iTopInd].x * (1.0f-fYScale)));
            lpPoints[i].y = (int)((lpBot[iBotInd].y * fYScale) + (lpTop[iTopInd].y * (1.0f-fYScale)));
            }
          if(bRelocErr == false)
            { // Draw the new path 
            bRender_path_points(lpPoints, lpTypes, iNumPts, bOutlineOnly);
            bRet = true;
            }
          }
        }
      }
    }
  if(lpPoints != NULL)
    GlobalFree(lpPoints);
  if(lpTypes != NULL)
    GlobalFree(lpTypes);

  return(bRet);
  }

/********************************************************************
bFill_out - Convert an array of line segments into an array of all
            of the points comprising the line segments.
                         
  inputs: Pointer to an array of points that will point to the array
            of points in all of the  line segments.
          Pointer to the total number of points in all the line segments.

  output: Sets the first input to point to the array of points comprising
            all the line segments.

  return: true if the process was completed.
          false if there was an error:
            Less than 2 points
            Too many points.
********************************************************************/
bool CGraphRSA::bFill_out(LPPOINT *lpPoints, LPDWORD lpdwNumPts)
  {
  DWORD i, dwNumPts;
  LPPOINT lpPts = *lpPoints;
  PTS pts;
  bool bRet;

    // Make sure we have at least two points
  if(*lpdwNumPts < 2)
    {
    MessageBox(NULL, "You need at least two points for a guide line.", "Not enough points!", MB_ICONSTOP);
    *lpdwNumPts = 0;
    bRet = false;
    }
  else
    { // Find out how many points are on the segments
    dwNumPts = 0;
    for(i = 0; i < *lpdwNumPts - 1; i++) 
      LineDDA(lpPts[i].x, lpPts[i].y, lpPts[i+1].x, lpPts[i+1].y, (LINEDDAPROC)vCount_points, (LPARAM)&dwNumPts);
  
      // If there are too many points, print an anoying message so the user doesn't do it again
    if(dwNumPts > MAXFILLPOINTS)
      {
      MessageBox(NULL, "Make your guide lines a bit shorter please!", "Too many points!", MB_ICONSTOP);
      *lpdwNumPts = 0;
      bRet = false;
      }
    else
      { // Allocate memory for the the points and initialize our "last point" index
      pts.lpPoints = (LPPOINT)GlobalAlloc(GPTR, dwNumPts * sizeof(POINT));
      pts.dwPos = 0;
        // Convert the segments to points
      for (i=0; i<*lpdwNumPts - 1; i++) 
        LineDDA(lpPts[i].x, lpPts[i].y, lpPts[i+1].x, lpPts[i+1].y, (LINEDDAPROC)vAdd_segment, (LPARAM)&pts);

        // Get rid of the original array of segments...
      GlobalFree(lpPts);
 
        // ... and replace it with the new points
      *lpPoints = pts.lpPoints;
      *lpdwNumPts = pts.dwPos;
      
        // Check to see if anything hit the fan
      if(!pts.dwPos)
        bRet = false;
      else
        bRet = true;
      }
    }
  return(bRet);  
  }

/********************************************************************
lCalc_font_height - Calculate the font height.
                    Formula: 
                      (Point size * Num Pixels per logical inch along Y) / 72.
                         
  inputs: Point size.
          Pointer to the CDC object for the printer.

  return: Height of the font in pixels.
********************************************************************/
/***************** in util.cpp
long CGraphRSA::lCalc_font_height(int iPointSize, CDC *pcdc)
  {
  return(MulDiv(iPointSize, pcdc->GetDeviceCaps(LOGPIXELSY), 72));
  }
*******************/

/********************************************************************
vAdd_segment - Used with LineDDA to add points from a line segment
               to an array containing all the points on a set of line segments

  inputs: X coordinate.
          Y coordinate.
          Pointer to a PTS structure.

  output: Puts the point into the point array of the PTS structure and
            increments the number of points in the array.

  return: None.
********************************************************************/
VOID CALLBACK vAdd_segment(int x, int y, PTS *pts)
  {
  pts->lpPoints[pts->dwPos].x = x;
  pts->lpPoints[pts->dwPos].y = y;
  pts->dwPos++;
  } 

/********************************************************************
vCount_points - Used with LineDDA to determine the number of points
                needed to store all the points on a set of line segments.

  inputs: X coordinate. Not used.
          Y coordinate. Not used.
          Pointer to the number of points.

  output: Increments the number of points (third parameter)

  return: None.
********************************************************************/
VOID CALLBACK vCount_points(int x, int y, LPDWORD lpdwNumPts)
  {
  (*lpdwNumPts)++;

  UNREFERENCED_PARAMETER(x);
  UNREFERENCED_PARAMETER(y);
  } 

/********************************************************************
vDev_to_log_coord - Convert the input point from device units to logical
                    units.
                    Overloaded method.

  inputs: Pointer to a CPoint object containing the point to get
            converted.
  return: None.
********************************************************************/
void CGraphRSA::vDev_to_log_coord(CPoint *pPoint)
  {

  m_pDC->DPtoLP(pPoint); // not printing
  }

/********************************************************************
vDev_to_log_coord - Convert the input point from device units to logical
                    units.
                    Overloaded method.

  inputs: Pointer to a Point structure containing the point to get
            converted.
  return: None.
********************************************************************/
void CGraphRSA::vDev_to_log_coord(POINT *pPoint)
  {

  m_pDC->DPtoLP(pPoint); // not printing
  }


