/********************************************************************
GraphRSA.h

Copyright (C) 2003,2004, 3CPM Company, Inc.  All rights reserved.

  Interface for the CGraphRSA class.

  This class draws the RSA graph.


HISTORY:
18-AUG-03  RET  New.
********************************************************************/

#if !defined(AFX_GRAPHRSA_H__3145A2D8_3485_49AB_AD05_F4FA03C646B8__INCLUDED_)
#define AFX_GRAPHRSA_H__3145A2D8_3485_49AB_AD05_F4FA03C646B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// This define is used to draw the RSA graphs is color.
// Comment it out for black and white.
#define RSA_COLOR


//#define GRAPH_SEPARATION 0.0155 // 10 (10 / 646) = 0.0155
#define RIGHT_MARGIN 0.0049 // 5  (5 / 1020) = 0.0049
#define LEFT_MARGIN 0.049 // 50  (50 / 1020) = 0.049
#define BOTTOM_MARGIN 0.108 // 70  (70 / 646) = 0.108

#define X_AXIS_HASH_MARK_LEN 8
#define Y_OFFSET_X_HASH_MARK_LABEL 10

#ifdef RSA_COLOR
#define PRE_WATER_COLOR   DARK_RED_RGB
#define POST_WATER_COLOR  BLUE_RGB
#endif

#define MAXFILLPOINTS 64000
#define MAXGUIDESEGMENTS 1000  // Max number of segments to read from user

// This structure is used by FillOut for passing data into LineDDA
typedef struct
  {
  DWORD dwPos;
  LPPOINT lpPoints; 
  } PTS;

// This structure holds the data required to know how to graph the RSA.
typedef struct
  {
  unsigned short uType; // Full screen or report.
  RECT rWin; // Client window where graph gets drawn.
  int iNumX;  // Number of points to plot along the X axis.
  int iNumY;  // Number of graphs (4-minute periods) to plot along the Y axis.
  float *pfData; // Pointer to the data to graph.
  COLORREF crPreWaterColor;
  COLORREF crPostWaterColor;
  CString cstrPatientName;
  CString cstrStudyDate;
  } RSA_DATA;


#define RSA_TYPE_FULL_SCREEN 1
#define RSA_TYPE_REPORT      2

/////////////////////////////////////////////////////////////////////
class CGraphRSA  
{
public: // Constructor and destructor
	CGraphRSA();
	virtual ~CGraphRSA();

public: // Methods
  void vDraw_graph(CDC *pDC, RSA_DATA *pRSA, CEGGSASDoc *pDoc);
  void vLabel_x_axis(CPoint *ppntStart, short int iXInc, short int iCpm, char *pszText);
  bool bPoly_draw95(CONST LPPOINT lppt, CONST LPBYTE lpbTypes, int  cCount);
  bool bRender_path_points(LPPOINT lpPoints, LPBYTE lpTypes, int iNumPts, BOOL bOutline);
  void GetRealTextExtent(LPPOINT lpPoints, int iNumPts, LPSIZE size);
  bool bText_effect(LPPOINT lpTop, LPPOINT lpBot, DWORD dwTopPts, DWORD dwBotPts,
                    LPSTR szText, BOOL bOutlineOnly);
  bool bFill_out(LPPOINT *lpPoints, LPDWORD lpdwNumPts);
  void vDev_to_log_coord(CPoint *pPoint);
  void vDev_to_log_coord(POINT *pPoint);

public: // Data
  CDC *m_pDC; // Pointer to CDC object where the graph gets drawn.

  int m_iPatientNameExt; // Patient name extent
  int m_iWaterIngestExt; // Water load extent
  POINT *m_pptArrayStart; // Holds the points being graphed.
  short int m_iZeroLine1, m_iZeroLine2; // Holds the line index for the 2 flat
                                        // lines between the baseline and postwater.
  short int m_iGraphSeparation; // Amount of space separating each graph.
    // These points are used throughout the graphing.
  float m_fMin; // Minumum Y value being graphed.
  //float m_fRange; // The absolute range between the minimum and maximum Y values
  //                // for a particular graph.
  //float m_fYRange; // The range on the display that the Y values can be plotted
  //                 // into for a particular graph.
  int m_iYBottom; // The bottom most Y coordinate for drawing the graphs.
  int m_iXStart; // Starting X coordinate for drawing graphs.
  int m_iXInc; // Amount to increment along the X axis for each Y value plotted.
};

#endif // !defined(AFX_GRAPHRSA_H__3145A2D8_3485_49AB_AD05_F4FA03C646B8__INCLUDED_)
