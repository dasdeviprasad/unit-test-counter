/********************************************************************
Driver.h

Copyright (C) 2003,2004,2005,2010, 3CPM Company, Inc.  All rights reserved.

  Header file for the CDriver class.

HISTORY:
20-OCT-03  RET  New.
03-MAR-04  RET  Add class variables used for generating data without the hardware.
                  m_iUseFakeData, m_iSinCounter, m_iFakePntCnt, m_fFakeAmp, m_fFakeFreq.
03-MAR-05  RET  Add define WARN_LOW_BATTERY.
17-AUG-10  RET
                Copied from EGGSAS program.
                Remove class variables: m_iDoElectrodeTest,
                  m_fPosImpedanceLimit, m_fNegImpedanceLimit, m_iFirstValidImpSignal
                  m_bFirstBattVoltImpReading, m_iDoBatteryCheck
********************************************************************/

#if !defined(AFX_DRIVER_H__899D4182_F9DE_480B_B4E9_B18600989AA8__INCLUDED_)
#define AFX_DRIVER_H__899D4182_F9DE_480B_B4E9_B18600989AA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DLL_NAME "XXDRIVER.DLL"
  // Configurations as specified in DEVICE.INI.
#define BATT_IMP_CONFIG 1
#define EGG_CONFIG 2

  // Defines for the EGG unit channels based in the INI file.
#define CHANNEL_EGG  0 // EGG channel.
#define CHANNEL_RESP 1 // Respiration channel

  // Defines for the battery voltage and impedance channels based on
  // the INI file
#define CHANNEL_BATTERY 0 // Battery voltage channel.
#define CHANNEL_POS_IMPEDANCE 1 // Positive impedance channel.
#define CHANNEL_NEG_IMPEDANCE 2 // Negative impedance channel.

  // Maximum number of points that can be used for each call to
  // the driver.
#define MAX_PTS_PER_READ 5

  // Items to check the hardware for.  These are bit fields and can be
  // ORed together.
#define CHK_BATTERY       0x1 // Check the battery.
#define CHK_IMPEDANCE_POS 0x2 // Check the impedance of the positive input.
#define CHK_IMPEDANCE_NEG 0x4 // Check the impedance of the negative input.
#define CHK_ALL_IMPEDANCE 0x6 // Check all impedance items.
#define CHK_ALL           0x7 // Check all items.

#define READ_BATT_IMP_MIN_TIME 2.5F // Minimum time to read the battery
                                    // voltage and impedance to get a good
                                    // reading.
#define READ_BATTERY_TIMEOUT 5 // Time limit in seconds to read the
                               // battery voltage.
#define READ_IMPEDANCE_TIMEOUT 10 // Time limit in seconds to read the
                                 // impedance.

  // Warning returns from the Read_Buffer() function.
  // The value is that specified in the EGG hardware driver
  // manual.
#define WARN_LOW_BATTERY -100 // Low battery warning

/////////////////////////////////////////////////////////////////////
class CDriver  
{
public:
	CDriver();
	virtual ~CDriver();

public: // Data
  HINSTANCE m_hDLL;     // Handle to device driver DLL
  int m_iHandle;	    // Handle or Error
  float m_fMinEGG; // Minimum EGG signal read from driver.
  float m_fMaxEGG; // Maximum EGG signal read from driver.
  float m_fMaxResp; // Minimum Respiration signal read from driver.
  float m_fMinResp; // Maximum Respiration signal read from driver.
  double m_dData[342]; // Buffer to hold EGG data.
  unsigned m_uEGGPtsPerSec; // Number of EGG points scanned per second.
  unsigned m_uRespPtsPerSec; // Number of respiration points scanned per second.
  unsigned m_uEGGPtsPerQtrSec;  // The EGG point number to look at every quarter second.
  unsigned m_uRespPtsPerQtrSec;  // The Respiration point number to look at every quarter second.
    // Buffer to hold only the EGG points we are interested in.
  float m_fEGGPts[MAX_PTS_PER_READ];
    // Buffer to hold only the Respiration points we are interested in.
  float m_fRespPts[MAX_PTS_PER_READ];
    // Number of EGG points input that haven't reached points per quarter second.
  unsigned m_uEGGPtCnt;
    // Number of Respiration points input that haven't reached points per quarter second.
  unsigned m_uRespPtCnt;
  CString m_cstrErr; // Used to hold error messages.
  //short int m_iDoElectrodeTest; // Read from INI file. 1 - do electrode test
  //                              // 0 - skip electrode test.
  float m_fRespMult; // Read from the INI file.
                     // Multiplier for the Respiration signal.
  //float m_fPosImpedanceLimit; // Limit for positive electrode impedance.
  //float m_fNegImpedanceLimit; // Limit for negative electrode impedance.

  short int m_iNumEGGPts; // Number of EGG data points held over from last call to get data.
  short int m_iNumRespPts; // Number of Respiration data points held over from last call to get data.
  float m_fEGGData[MAX_PTS_PER_READ]; // Buffer to hold the EGG data used from the driver.
  float m_fRespData[MAX_PTS_PER_READ]; // Buffer to hold the Respiration data used from the driver.
  //int m_iFirstValidImpSignal; // First valid impedance and battery voltage signal after
  //                            // starting to read the signal.
  CTime m_ctStart; // Start time for reading the battery voltage and impedance.
  //bool m_bFirstBattVoltImpReading; // true if this is the first reading.
  int m_iNumPtsRead; // Number of point read during the battery voltage and
                     // impedance testing.
  //short int m_iDoBatteryCheck; // Read from INI file.
  //                             // 1 - check the battery voltage.
  //                             // 0 - Skip the battery check.
    // Data used for generating data without the hardware.
  short int m_iUseFakeData; // Value from INI file that indicates if using
                            // hardware.
                            // 1 = generate data without hardware.
                            // 0 = use hardware.
  int m_iSinCounter; // Counter for generating data on a sine wave.
  short int m_iFakePntCnt; // Number of artificial data points generated so far.
                           // At 256 points per minute and retrieving data every
                           // 250 milliseconds, when this reaches 15, generate 2
                           // data points.
  float m_fFakeAmp; // Used to varying the amplitude.
  float m_fFakeFreq; // Used to vary the frequency.
    //// End of data used to generate artificial data.

      // DEBUGGING DATA
  int m_iTotalEGGPts;
  int m_iTotalRespPts;

public: // Methods
  int iInit();
  int iGet_data(short int iChannel, float *pfData); //float **ppfData);
  void vStop();
  //short int iReStart();
  void vSet_window(CWnd *pWnd);
  int iGet_data_all(float *pfEGGData, float *pfRespData);
//  short int iCheck_battery_impedance(unsigned uCheck, void(__cdecl *pvShowStatus)());
  //short int iReInitEGG();
  //short int iCheck_battery(void);
  //short int iCheck_impedance(unsigned uCheck);

  float fGet_fake_data();
  void vDump_pts(short int iChannel, short int iType, int iNumPts,
                 double *pdData, float *pfData);

  void vClear_data_buffers(void);
};

#endif // !defined(AFX_DRIVER_H__899D4182_F9DE_480B_B4E9_B18600989AA8__INCLUDED_)
