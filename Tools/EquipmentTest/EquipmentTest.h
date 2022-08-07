/********************************************************************
EquipmentTest.h

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  main header file for the Equipment Test application.


HISTORY:
17-AUG-10  RET  New.
03-SEP-10  RET  Version 1.1
                  Add INI items:
                    INI_SEC_TEST, INI_ITEM_STABLEWINDOW_EGG,
                    INI_ITEM_STABLEWINDOW_RESP, INI_ITEM_STABLETIME,
                    INI_ITEM_RESP_MULT, INI_ITEM_RESP_MAX_UNCONNSIGNAL,
                    INI_ITEM_RESP_MIN_CONNECTEDSIGNAL, INI_ITEM_RESP_MIN_PEAKTOPEAK
********************************************************************/

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "sysdefs.h"
#include "Driver.h"

//// INI file definitions for this program's INI file.
  // General section
#define INI_SEC_GENERAL "GENERAL"
#define INI_ITEM_DATAPATH "datapath"

// INI section for test parameters
#define INI_SEC_TEST "TEST"
#define INI_ITEM_STABLEWINDOW_EGG "stableEGGwindow" // in MicroVolts
#define INI_ITEM_STABLEWINDOW_RESP "stableRespwindow" // in MicroVolts
#define INI_ITEM_STABLETIME "stabletime" // in Seconds
#define INI_ITEM_RESP_MULT "respmult"
#define INI_ITEM_RESP_MAX_UNCONNSIGNAL "respmaxunconnsignal" // in MicroVolts
#define INI_ITEM_RESP_MIN_CONNECTEDSIGNAL "respminconnectedsignal"  // in MicroVolts
#define INI_ITEM_RESP_MIN_PEAKTOPEAK "respminpeaktopeak" // in MicroVolts

/////////////////////////////////////////////////////////////////////
//// Defines for the data collected during an EGG.
#define MAX_CHANNELS 2
#define MIN_PER_PERIOD 10 // Minutes per period.
#define DATA_POINT_SIZE sizeof(float) //sizeof(short int)
#define DATA_POINTS_PER_SECOND 4.267F
  // Data points per minute for one channel.
#define DATA_POINTS_PER_MIN_1 256 //260 //
#define DATA_POINTS_PER_HALF_MIN (DATA_POINTS_PER_MIN_1 / 2)
  // Data points per minute for all channels.
#define DATA_POINTS_PER_MIN_ALL (DATA_POINTS_PER_MIN_1 * MAX_CHANNELS)
#define MINUTES_PER_EPOCH 4
#define HALF_MINUTES_PER_EPOCH (MINUTES_PER_EPOCH * 2)
  // Bytes of data per minute for one channel.
#define BYTES_PER_MINUTE_1 (DATA_POINTS_PER_MIN_1 * DATA_POINT_SIZE)
  // Bytes of data per minute for all channels.
#define BYTES_PER_MINUTE_ALL (DATA_POINTS_PER_MIN_ALL * DATA_POINT_SIZE)
  // Bytes per epoch for one channel.
#define BYTES_PER_EPOCH_1 (BYTES_PER_MINUTE_1 * MINUTES_PER_EPOCH)
  // Bytes per epoch for all channels.
#define BYTES_PER_EPOCH_ALL (BYTES_PER_MINUTE_ALL * MINUTES_PER_EPOCH)
  // Data points per epoch for one channel.
#define DATA_POINTS_PER_EPOCH_1 (DATA_POINTS_PER_MIN_1 * MINUTES_PER_EPOCH)
  // Data points per epoch for all channels.
#define DATA_POINTS_PER_EPOCH_ALL (DATA_POINTS_PER_MIN_ALL * MINUTES_PER_EPOCH)
  // Number of bytes in a period
#define BYTES_PER_PERIOD (BYTES_PER_MINUTE_1 * MIN_PER_PERIOD)

#define BLUE_RGB       RGB(0, 0, 255) // Blue
#define WHITE_RGB      RGB(255, 255, 255) // white
#define BLACK_RGB      RGB(0, 0, 0) // black
#define RED_RGB        RGB(255, 0, 0) // Red
#define GREEN_RGB      RGB(0, 128, 0) // Green
#define DARK_RED_RGB   RGB(175, 0, 0) //RGB(100, 0, 0) // dark red
#define LTGRAY_RGB     RGB(150, 150, 150) // Light gray.
#define EVENT_RGB      RGB(3, 147, 252)

  // Maximum amount of time that we should wait while getting zero
  // points from the driver.  This is based on a 250 millisecond timer.
#define ZERO_POINTS_MAX_COUNT 40  // 10 seconds.

// Structure to hold the data required to set the attributes for
// the strip charts.
typedef struct
  {
//  int iMaxPoints; // Number of points on the graph.
  int iTotalPoints; // Total number of points that are shown at one time
                    // on a strip chart.
  double dMinEGG; // Minimum Y scale value on EGG graph.
  double dMaxEGG; // Maximum Y scale value on EGG graph.
  double dMinResp; // Minimum Y scale value on Respiration graph.
  double dMaxResp; // Maximum Y scale value on Respiration graph.
  double dMaxXScale; // Maximum scale of the X axis.
  } STRIP_CHART_ATTRIB;


// CEquipmentTestApp:
// See EquipmentTest.cpp for the implementation of this class
//

class CEquipmentTestApp : public CWinApp
{
public:
	CEquipmentTestApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation
public:
  void vLog(LPCTSTR pstrFmt, ...);


	DECLARE_MESSAGE_MAP()
};

extern CEquipmentTestApp theApp;