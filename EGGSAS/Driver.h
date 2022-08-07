/********************************************************************
Driver.h

Copyright (C) 2003,2004,2005,2011, 3CPM Company, Inc.  All rights reserved.

  Header file for the CDriver class.

HISTORY:
20-OCT-03  RET  New.
03-MAR-04  RET  Add class variables used for generating data without the hardware.
                  m_iUseFakeData, m_iSinCounter, m_iFakePntCnt, m_fFakeAmp, m_fFakeFreq.
03-MAR-05  RET  Add define WARN_LOW_BATTERY.
22-FEB-11  RET
                Add suppoprt for new hardware driver that keeps a device ID that must match
                  with the Device ID from the software.
                    Add defines: DEVICE_ID_SIZE, DEVICE_MAX_WORDS, DEVICE_ID_PASSWD_1
                                 DEVICE_ID_PASSWD_2, DEVICE_ID_PASSWD_1_INDEX, 
                                 DEVICE_ID_PASSWD_2_INDEX, INI_DEV_ID_INDEX_1, 
                                 INI_DEV_ID_INDEX_2, INI_DEV_ID_NOT_GEN_1, 
                                 INI_DEV_ID_NOT_GEN_2, INI_DEV_ID_GEN_1, 
                                 INI_DEV_ID_GEN_2, DEV_ID_SRC_NONE, DEV_ID_SRC_MAC,
                                 DEV_ID_SRC_COMP_NAME, ERR_DEV_ID_NONE,
                                 ERR_DEV_ID_MISMATCH, ERR_DEV_ID_NEW_DEV_EXIST_INST,
                                 ERR_DEV_ID_CREATE
                    Add class variables: m_cstrDevID, m_iDevID[], m_iDevIDErrm m_iDevIDSrc
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

//// Device.ini file defines for the frequency the hardware is too operate at.
#define DEVICE_INI_SECTION "configuration#2"
#define DEVICE_INI_ITEM_CPM "channel#0"
#define DEVICE_INI_VALUE_15_CPM "5  18 1 //15cpm EGG input B"
#define DEVICE_INI_VALUE_60_CPM "5  19 1 //60cpm EGG input B"
#define DEVICE_INI_VALUE_200_CPM "5  20 1 //200cpm EGG input B"
//// End if Device.ini file defines for the frequency the hardware is too operate at.

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

#define DEVICE_ID_SIZE 32
#define DEVICE_MAX_WORDS 8
#define DEVICE_ID_PASSWD_1 0xBF10
#define DEVICE_ID_PASSWD_2 0x3D5C
#define DEVICE_ID_PASSWD_1_INDEX 8
#define DEVICE_ID_PASSWD_2_INDEX 9

// Defines for the INI file for whether or not the Device ID has been generated.
#define INI_DEV_ID_INDEX_1  2
#define INI_DEV_ID_INDEX_2  63
#define INI_DEV_ID_NOT_GEN_1 'E'
#define INI_DEV_ID_NOT_GEN_2 '5'
#define INI_DEV_ID_GEN_1 'A'
#define INI_DEV_ID_GEN_2 '7'

// Sources for the device ID.
#define DEV_ID_SRC_NONE 0
#define DEV_ID_SRC_MAC 1
#define DEV_ID_SRC_COMP_NAME 2

// Error codes for driver ID mismatch.
#define ERR_DEV_ID_NONE 0
#define ERR_DEV_ID_MISMATCH -1000
#define ERR_DEV_ID_NEW_DEV_EXIST_INST -1001 // New device with existing installation
#define ERR_DEV_ID_CREATE -1002
#define ERR_DEV_ID_COMP_MISMATCH -1003

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
  short int m_iDoElectrodeTest; // Read from INI file. 1 - do electrode test
                                // 0 - skip electrode test.
  float m_fRespMult; // Read from the INI file.
                     // Multiplier for the Respiration signal.
  float m_fPosImpedanceLimit; // Limit for positive electrode impedance.
  float m_fNegImpedanceLimit; // Limit for negative electrode impedance.

  short int m_iNumEGGPts; // Number of EGG data points held over from last call to get data.
  short int m_iNumRespPts; // Number of Respiration data points held over from last call to get data.
  float m_fEGGData[MAX_PTS_PER_READ]; // Buffer to hold the EGG data used from the driver.
  float m_fRespData[MAX_PTS_PER_READ]; // Buffer to hold the Respiration data used from the driver.
  int m_iFirstValidImpSignal; // First valid impedance and battery voltage signal after
                              // starting to read the signal.
  CTime m_ctStart; // Start time for reading the battery voltage and impedance.
  bool m_bFirstBattVoltImpReading; // true if this is the first reading.
  int m_iNumPtsRead; // Number of point read during the battery voltage and
                     // impedance testing.
  short int m_iDoBatteryCheck; // Read from INI file.
                               // 1 - check the battery voltage.
                               // 0 - Skip the battery check.

  CString m_cstrDevID;
  short m_iDevID[DEVICE_ID_SIZE];
  int m_iDevIDErr;
  short m_iDevIDSrc; // Source of device ID.

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
  short int iReStart();
  void vSet_window(CWnd *pWnd);
  int iGet_data_all(float *pfEGGData, float *pfRespData);
//  short int iCheck_battery_impedance(unsigned uCheck, void(__cdecl *pvShowStatus)());
  short int iReInitEGG();
  short int iCheck_battery(void);
  short int iCheck_impedance(unsigned uCheck);
  int iValidate_hw();
  bool bCreate_dev_id();
  void vConvert_id_to_binary(CString cstrDevID, short *piDevID);
  CString cstrDev_id_error();
  bool bCreate_dev_id_from_mac();
  bool bCreate_dev_id_from_comp_name();
  bool bVerify_devid_src(short *piDevID);

  float fGet_fake_data();
  void vDump_pts(short int iChannel, short int iType, int iNumPts,
                 double *pdData, float *pfData);

  void vClear_data_buffers(void);
  void vInitialize();
};

#endif // !defined(AFX_DRIVER_H__899D4182_F9DE_480B_B4E9_B18600989AA8__INCLUDED_)
