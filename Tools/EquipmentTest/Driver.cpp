/********************************************************************
Driver.cpp

Copyright (C) 2003,2004,2005,2010, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CDriver class.

  This class provides the interface between the EGGSAS software the
  the EGG device driver.

  The driver initializes from the DEVICE.INI file.
    The sample rate in the file is set to 341.36.
    The EGG is channel 0.
    The Respiration is channel 1.
    Every 80th data point is kept corresponding to 4.267 Hz.

  NOTES:
		If the program crashes, or some other disaster where the program
    terminates without unloading the DLL, it will think its still loaded the
    next time it starts.  The answer is to unload the DLL and then load it
    again.  The problem is that it seems to have to go through the windows
    message loop before the DLL is unloaded or some other path that cannot
    be done within a retry loop (even with a sleep).  So the at this
    time, the initialization funtion has to exit with an error and the
    caller has to initialize again.


HISTORY:
20-OCT-03  RET  New.
03-MAR-04  RET  Added ability to generate a sine wave of varying amplitude
                  and frequency for the EGG (not respiration) so that the
                  hardware doesn't have to be used.  Add checks to all functions
                  using the hardware driver to see if system is using the hardware.
                  Added method fGet_fake_data() to generate one data point on
                  the sine wave.
03-MAR-05  RET  Change iGet_data(, iGet_data_all) to return different values
                  as follows:
                    FAIL_EGG_DRIVER, FAIL_EGG_DRIVER_LOW_BATT.
12-APR-05  RET  Change all XXDRIVER.DLL Close() functions to Pause() except for
                  the destructor.  This is in response to EZEM reported problem
                  with their hardware where they would get a XXDRIVER.DLL not
                  echoing settings error.
17-AUG-10  RET
                Copied from EGGSAS program.
                Removed methods: iReStart(), iCheck_battery_impedance(),
                  iReInitEGG(), iCheck_battery(), iCheck_impedance().
03-SEP-10  RET  Version 1.1
                  Changed references to INI file to use this program's
                    INI file.
********************************************************************/

#include "stdafx.h"
#include "EquipmentTest.h"
#include "Driver.h"

#include "driverh.h" // For hardware interface.
#include ".\driver.h"

#include <cmath>                    // for trigonometry functions

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////

/********************************************************************
Constructor

  Initialize class data.
  Read driver data from the INI file.

********************************************************************/
CDriver::CDriver()
  {

  m_hDLL = NULL;
  m_iHandle = -1;
  m_cstrErr.Empty();
  //  // Determines if the battery voltage level check is performed.
  //m_iDoBatteryCheck = AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
  //                                         INI_ITEM_BATTERY_CHECK, 1);
  //  // Determines if the electrode test is performed.
  //m_iDoElectrodeTest = AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
  //                                         INI_ITEM_ELECTRODE_TEST, 1);
  //  // Limit for the positive impedance.
  //m_fPosImpedanceLimit = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
  //                                         INI_ITEM_POS_IMPEDANCE_LIMIT, 200);
  //  // Limit for the negative impedance.
  //m_fNegImpedanceLimit = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
  //                                         INI_ITEM_NEG_IMPEDANCE_LIMIT, 200);
  //  // Find out if we are using hardware or fake data.
  //m_iUseFakeData = AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
  //                                            INI_ITEM_USE_FAKE_DATA, 0);

  m_iUseFakeData = 0;
  //m_bFirstBattVoltImpReading = true;
  }

/********************************************************************
Destructor

  Clean up class data.

********************************************************************/
CDriver::~CDriver()
  {

  if(m_iUseFakeData == 0)
    {
    if(m_iHandle >= 0)
      {
      Close(m_iHandle);
      m_iHandle = -1;
      }
    if(m_hDLL != NULL)
      {
      FreeLibrary(m_hDLL);
      m_hDLL = NULL;
      }
    }
  }

/********************************************************************
iInit - Initialize the Driver.
           Load the DLL
           Get pointers to all the functions.
           Initialize and get a handle for the EGG configuration.
           Get the total number of channels (must be 2).
           Read data from the channels.

  inputs: None.
  
  Output: None.

  return: SUCCESS if DLL is loaded and driver properly initialized.
          FAIL if there is an error.
********************************************************************/
int CDriver::iInit()
  {
  CString cstrErr, cstrMsg;
  float fSignalRate;
  int iTotal, iRet = SUCCESS;

  if(m_iUseFakeData == 0)
    { // Load the DLL
    m_hDLL = LoadLibrary("XXDRIVER.DLL");

    if(m_hDLL == NULL)
      {
      cstrMsg.Format(IDS_ERR_LOAD_DLL, DLL_NAME);
      iRet = FAIL;
      }
    else
      { // DLL loaded.
        // Obtaining addresses of the functions
      Initialize     = (int   (__cdecl *)(int,int))GetProcAddress(m_hDLL,(LPCSTR)"Initialize");
      Error_Message  = (char* (__cdecl *)(int))GetProcAddress(m_hDLL,(LPCSTR)"Error_Message");
      Read_Buffer    = (int   (__cdecl *)(int,int,double*,int))GetProcAddress(m_hDLL,(LPCSTR)"Read_Buffer");
      Total_Channels = (int   (__cdecl *)(int))GetProcAddress(m_hDLL,(LPCSTR)"Total_Channels");
			Maximum_Value  = (double(__cdecl *)(int,int))GetProcAddress(m_hDLL,(LPCSTR)"Maximum_Value");
      Minimum_Value  = (double(__cdecl *)(int,int))GetProcAddress(m_hDLL,(LPCSTR)"Minimum_Value");
      Signal_Rate    = (double(__cdecl *)(int,int))GetProcAddress(m_hDLL,(LPCSTR)"Signal_Rate");
      Signal_Name    = (char* (__cdecl *)(int,int))GetProcAddress(m_hDLL,(LPCSTR)"Signal_Name");
      Close          = (void  (__cdecl *)(int))GetProcAddress(m_hDLL,(LPCSTR)"Close");
      Pause          = (void  (__cdecl *)(int))GetProcAddress(m_hDLL,(LPCSTR)"Pause");
      Send_Command   = (double(__cdecl *)(int,int,double))GetProcAddress(m_hDLL,(LPCSTR)"Send_Command");
       	Set_File_Name  = (void  (__cdecl *)(char*,int))GetProcAddress(m_hDLL,(LPCSTR)"Set_File_Name");
      Read_DFT       = (int   (__cdecl *)(int,int,double*,int,int,double,int))GetProcAddress(m_hDLL,(LPCSTR)"Read_DFT");
			Is_Impedance_Mode = (int   (__cdecl *)(int))GetProcAddress(m_hDLL,(LPCSTR)"Is_Impedance_Mode");
      Reset_CO2_T0_0 = (double(__cdecl *)(int))GetProcAddress(m_hDLL,(LPCSTR)"Reset_CO2_T0_0");
      
      if(Initialize == NULL || Error_Message == NULL || Read_Buffer == NULL
      || Total_Channels == NULL || Maximum_Value == NULL || Minimum_Value == NULL
      || Signal_Rate == NULL || Signal_Name == NULL || Close == NULL
      || Pause == NULL || Send_Command == NULL || Set_File_Name == NULL
      || Read_DFT == NULL || Is_Impedance_Mode == NULL || Reset_CO2_T0_0 == NULL)
        {
        cstrMsg.Format(IDS_ERR_DLL_FUNCS_NOT_FOUND, DLL_NAME);
        ((CEquipmentTestApp *)AfxGetApp())->vLog("iInit: error getting entry points for DLL functions: %s", cstrMsg);
        iRet = FAIL;
        }
      else
        { // Got all functions for DLL.
          // Configuration # 1
        m_iHandle = Initialize(1,EGG_CONFIG);
        if(m_iHandle < 0)
          {
          cstrMsg.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
          ((CEquipmentTestApp *)AfxGetApp())->vLog("iInit: error initializing EGG configuration: %s", cstrMsg);
          iRet = FAIL;
          }
        else
          {
          iTotal = Total_Channels(m_iHandle);
          if(iTotal < 0)
            {
            cstrMsg.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
            ((CEquipmentTestApp *)AfxGetApp())->vLog("iInit: error getting number of channels: %s", cstrMsg);
            iRet = FAIL;
            }
          else if(iTotal != 2)
            {
            cstrMsg.LoadString(IDS_ERR_DLL_CHANNELS);
            ((CEquipmentTestApp *)AfxGetApp())->vLog("iInit: wrong number of channels: %s", cstrMsg);
            iRet = FAIL;
            }
          else
            {
              // Minimum value of signal
            m_fMinEGG = (float)Minimum_Value(m_iHandle, CHANNEL_EGG);
            m_fMinResp = (float)Minimum_Value(m_iHandle, CHANNEL_RESP);
              // Maximum value of signal
            m_fMaxEGG = (float)Maximum_Value(m_iHandle, CHANNEL_EGG);
            m_fMaxResp = (float)Maximum_Value(m_iHandle, CHANNEL_RESP);
              // Data rate of signal for EGG
            fSignalRate = (float)Signal_Rate(m_iHandle, CHANNEL_EGG);
            m_uEGGPtsPerSec = (unsigned)fSignalRate + 1;
            if((unsigned)fSignalRate == 341)
              m_uEGGPtsPerQtrSec = 80;
            else if((unsigned)fSignalRate == 170)
              m_uEGGPtsPerQtrSec = 40;
            else // if((unsigned)fSignalRate == 85)
              m_uEGGPtsPerQtrSec = 20;

              // Data rate of signal for Respiration
            fSignalRate = (float)Signal_Rate(m_iHandle, CHANNEL_RESP);
            m_uRespPtsPerSec = (unsigned)fSignalRate + 1;
            if((unsigned)fSignalRate == 341)
              m_uRespPtsPerQtrSec = 80;
            else if((unsigned)fSignalRate == 170)
              m_uRespPtsPerQtrSec = 40;
            else // if((unsigned)fSignalRate == 85)
              m_uRespPtsPerQtrSec = 20;

              // Get the multiplier for the respiration.  The respiration signal
              // is so it can be seen on the same graph as the EGG signal.
            m_fRespMult = (float)AfxGetApp()->GetProfileInt(INI_SEC_TEST, 
                                     INI_ITEM_RESP_MULT, 1000);
            m_fMinResp *= m_fRespMult;
            m_fMaxResp *= m_fRespMult;
            }
          }
        }
      }
    if(iRet == FAIL)
      {
      //cstrErr.LoadString(IDS_ERROR1);
      //AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErr, MB_OK);
      m_cstrErr = cstrMsg;
      if(m_iHandle >= 0)
        {
        Close(m_iHandle);
        m_iHandle = -1;
        }
      if(m_hDLL != NULL)
        {
        FreeLibrary(m_hDLL);
        m_hDLL = NULL;
        }
      }
    }
  else
    { // using fake signals.
      // Get the multiplier for the respiration.  The respiration signal
      // is so it can be seen on the same graph as the EGG signal.
    m_fRespMult = (float)AfxGetApp()->GetProfileInt(INI_SEC_TEST, 
                               INI_ITEM_RESP_MULT, 1000);
    m_fMinEGG = -1000.0F;
    m_fMinResp = 0.0F;
      // Maximum value of signal
    m_fMaxEGG = 1000.0F;
    m_fMaxResp = 2.5F * m_fRespMult;
    m_iFakePntCnt = 0;
    m_iSinCounter = 1;
    m_fFakeAmp = 200.0F;
    m_fFakeFreq = 0.075F;
    }
    // Set the point counters to  get the first point.
  m_uEGGPtCnt = 0;
  m_uRespPtCnt = 0;

  m_iTotalEGGPts = 0;
  m_iTotalRespPts = 0;

  m_iNumEGGPts = 0;
  m_iNumRespPts = 0;

  return(iRet);
  }


/********************************************************************
iGet_data - Read the data for 1 channel. Only gets points corresponding
            to 4.267 Hz. Others are discarded.

  inputs: Channel number to read (starts at 0).
          Pointer to  a data buffer that get the data read.

  return: Number of points used.
          FAIL_EGG_DRIVER if there is an error.
          FAIL_EGG_DRIVER_LOW_BATT if there is a low battery warning.
********************************************************************/
int CDriver::iGet_data(short int iChannel, float *pfData)
  {
  unsigned uOutBufIndex, uInBufIndex, uCnt;
  int iNumPts = -1;
  unsigned *puPtCnt, uPtsPerSec, uPtsPerQtrSec;
  float fMultFact;

  if(m_iUseFakeData == 0)
    {
    if(m_iHandle >= 0)
      { 
      if(iChannel == CHANNEL_EGG)
        {
        fMultFact = 1.0F; // Signal comes in a microvolts.
        puPtCnt = &m_uEGGPtCnt;
        uPtsPerSec = m_uEGGPtsPerSec;
        uPtsPerQtrSec = m_uEGGPtsPerQtrSec;
        }
      else
        {
        fMultFact = m_fRespMult; // Signal comes in as volts.
        puPtCnt = &m_uRespPtCnt;
        uPtsPerSec = m_uRespPtsPerSec;
        uPtsPerQtrSec = m_uRespPtsPerQtrSec;
        }
        // Read data from the driver.  Since this should be called every
        // quarter second, it should never exceed the max points per second.
		  iNumPts = Read_Buffer(m_iHandle, iChannel, m_dData, uPtsPerSec);
      if(iChannel == CHANNEL_EGG)
        m_iTotalEGGPts += iNumPts;
      else
        m_iTotalRespPts += iNumPts;

      if(iNumPts < 0)
        {
        m_cstrErr = Error_Message(-iNumPts); // Error in reading.
        ((CEquipmentTestApp *)AfxGetApp())->vLog("iGet_data: error reading data channel %d: %s", iChannel, m_cstrErr);
          // Set the error return.
        //if(iNumPts == WARN_LOW_BATTERY)
        //  iNumPts = FAIL_EGG_DRIVER_LOW_BATT;
        //else
          iNumPts = FAIL_EGG_DRIVER;
        }
      else
        { // Read driver.  Now get only the points we are interested in
          // corresponding to 4.267Hz.
        uOutBufIndex = 0;
        if(*puPtCnt == 0)
          pfData[uOutBufIndex++] = (float)m_dData[0] * fMultFact; // Very first point.
        uInBufIndex = uPtsPerQtrSec - *puPtCnt;
        for(uCnt = 0; uCnt < MAX_PTS_PER_READ; ++uCnt)
          {
          if(uInBufIndex < (unsigned)iNumPts)
            {
            pfData[uOutBufIndex++] = (float)m_dData[uInBufIndex] * fMultFact;
            uInBufIndex += uPtsPerQtrSec;
            }
          else
            break;
          }
        *puPtCnt = (unsigned)iNumPts - (uInBufIndex - uPtsPerQtrSec);
        iNumPts = uOutBufIndex;
        }
      }
    }
  else
    { // Using fake data.
    iNumPts = 1;
    if(iChannel == CHANNEL_EGG)
      {
      *pfData = fGet_fake_data();
      if(m_iFakePntCnt >= 15)
        {
        m_iFakePntCnt = 0;
        *(pfData + 1) = fGet_fake_data();
        ++iNumPts;
        }
      }
    else
      {
      *pfData = 1.0F;
      if(m_iFakePntCnt >= 15)
        {
        m_iFakePntCnt = 0;
        *(pfData + 1) = 1.0F;
        ++iNumPts;
        }
      }
    ++m_iFakePntCnt;
    }

  return(iNumPts);
  }

/********************************************************************
vStop - Stop the driver from reading the device.

  inputs: None.
  
  Output: None.

  return: None.
********************************************************************/
void CDriver::vStop()
  {

  if(m_iUseFakeData == 0)
    {
    if(m_iHandle >= 0)
      {
      Pause(m_iHandle); //Close(m_iHandle);
      m_iHandle = -1;
      }
    }
  else
    { // Using fake data.
    m_iFakePntCnt = 0;
    }
  }

/********************************************************************
iReStart - Restart the driver.  The driver must already have been
           initialized and running.
           Empty the hardware driver input buffers so this driver
           starts with a new data point.

  inputs: None.
  
  Output: None.

  return: SUCCESS if driver is restarted.
          FAIL if there is an error.
********************************************************************/
//short int CDriver::iReStart()
//  {
//  CString cstrErr, cstrMsg;
//  int iRet = SUCCESS;
//
//  if(m_iUseFakeData == 0)
//    {
//    if(m_iHandle >= 0)
//      vClear_data_buffers();
//    }
//  else
//    { // Using fake data.
//    m_iFakePntCnt = 0;
//    }
//
//  m_uEGGPtCnt = 0;
//  m_uRespPtCnt = 0;
//
//  m_iTotalEGGPts = 0;
//  m_iTotalRespPts = 0;
//
//  m_iNumEGGPts = 0;
//  m_iNumRespPts = 0;
//
//  return(iRet);
//  }

/********************************************************************
iReInitEGG - Reinitialize the EGG hardware driver.  The driver must already have
             been initialized.

  inputs: None.
  
  Output: None.

  return: SUCCESS if driver is restarted.
          FAIL if there is an error.
********************************************************************/
//short int CDriver::iReInitEGG()
//  {
//  CString cstrErr, cstrMsg;
//  int iRet = SUCCESS;
//
//  if(m_iUseFakeData == 0)
//    {
//    if(m_iHandle >= 0)
//      Pause(m_iHandle); //Close(m_iHandle);
//
//    m_iHandle = Initialize(1, EGG_CONFIG);
//    if(m_iHandle < 0)
//      {
//      m_cstrErr = Error_Message(-m_iHandle);
//      cstrMsg.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
//      cstrErr.LoadString(IDS_ERROR1);
//      AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErr, MB_OK);
//      ((CEquipmentTestApp *)AfxGetApp())->vLog("iReInitEGG: error initializing EGG configuration: %s", cstrMsg);
//      iRet = FAIL;
//      }
//    }
//  else
//    { // Using fake data.
//    m_iFakePntCnt = 0;
//    }
//
//  m_uEGGPtCnt = 0;
//  m_uRespPtCnt = 0;
//
//  m_iTotalEGGPts = 0;
//  m_iTotalRespPts = 0;
//
//  m_iNumEGGPts = 0;
//  m_iNumRespPts = 0;
//
//  return(iRet);
//  }

/********************************************************************
iGet_data_all - Read the data from both the EGG channels.

                This returns the same number of points for both channels.
                If there are different numbers of points, the extra
                points are held until the next call.

  inputs: Pointer to  a data buffer that will get the points read.
  
  return: Number of points being returned in the data buffers.
          FAIL_EGG_DRIVER if there is an error.
          FAIL_EGG_DRIVER_LOW_BATT if there is a low battery warning.
********************************************************************/
int CDriver::iGet_data_all(float *pfEGGData, float *pfRespData)
  {
  int iNumNewEGGPts, iNewNumRespPts, iPtsToGraph, iCnt;

  iNumNewEGGPts = iGet_data(CHANNEL_EGG, &m_fEGGData[m_iNumEGGPts]);
  iNewNumRespPts = iGet_data(CHANNEL_RESP, &m_fRespData[m_iNumRespPts]);
  if(iNumNewEGGPts >= 0 && iNewNumRespPts >= 0)
    {
    m_iNumEGGPts += iNumNewEGGPts;
    m_iNumRespPts += iNewNumRespPts;
    iPtsToGraph = __min(m_iNumEGGPts, m_iNumRespPts);
    for(iCnt = 0; iCnt < iPtsToGraph; ++iCnt)
      {
      *pfEGGData = m_fEGGData[iCnt];
      *pfRespData = m_fRespData[iCnt];
      ++pfEGGData;
      ++pfRespData;
      }

      // Update the points retrieved from the driver that we didn't graph.
    if(m_iNumEGGPts > m_iNumRespPts)
      { // Didn't graph all EGG points, but did graph all respiration points.
      m_iNumEGGPts = m_iNumEGGPts - m_iNumRespPts;
      m_iNumRespPts = 0;
      memmove(m_fEGGData, &m_fEGGData[iPtsToGraph], m_iNumEGGPts * sizeof(float));
      }
    else if(m_iNumRespPts > m_iNumEGGPts)
      { // Didn't graph all EGG points, but did graph all respiration points.
      m_iNumRespPts = m_iNumRespPts - m_iNumEGGPts;
      m_iNumEGGPts = 0;
      memmove(m_fRespData, &m_fRespData[iPtsToGraph], m_iNumRespPts * sizeof(float));
      }
    else // if(iNumRespPts == iNumEGGPts)
      {
      m_iNumRespPts = 0;
      m_iNumEGGPts = 0;
      }
    // FOR DEBUG
    //if(iPtsToGraph == 0)
    //  {
    //  iCnt = -1;
    //  m_cstrErr = "Failed reading both channels.";
    //  }
    }
  else
    {
    //if(iNumNewEGGPts == WARN_LOW_BATTERY || iNewNumRespPts == WARN_LOW_BATTERY)
    //  iPtsToGraph = FAIL_EGG_DRIVER_LOW_BATT;
    //else
    //if(iNumNewEGGPts <= 0 && iNewNumRespPts <= 0)
      iPtsToGraph = FAIL_EGG_DRIVER;
    //else if(iNumNewEGGPts <= 0)
    //  iPtsToGraph = FAIL_EGG_DRIVER_NO_EGG;
    //else if(iNewNumRespPts <= 0)
    //  iPtsToGraph = FAIL_EGG_DRIVER_NO_RESP;
    }
  return(iPtsToGraph);
  }

/********************************************************************
iCheck_battery - Check the battery voltage level.
                 Battery voltage is valid only after 2.5 sec.

  inputs: None.
  
  return: SUCCESS if elapsed time is 2.5 seconds and battery voltage is OK.
          SUCCESS_NO_DATA if less than 2.5 seconds.
          SUCCESS_LOW_BATTERY if the battery is low but useable.
          FAIL if there is an error reading the hardware.
          FAIL_TIMEOUT if didn't get enough readings in 2.5 seconds.
          FAIL_BAD_BATTERY if the battery is too low.
********************************************************************/
//short int CDriver::iCheck_battery(void)
//  {
//  CString cstrMsg;
//  CTime ctEnd;
//  CTimeSpan ctsElapsed;
//  float fSignalRate;
//  int iNumPts;
//  short int iRet = SUCCESS;
//
//  if(m_iUseFakeData == 0)
//    {
//    if(m_iDoBatteryCheck == 1)
//      { // Check the battery voltage.
//      if(m_bFirstBattVoltImpReading == true)
//        { // First reading.
//        m_bFirstBattVoltImpReading = false;
//          // Close the current configuration.
//        if(m_iHandle >= 0)
//          Pause(m_iHandle); //Close(m_iHandle);
//          // Now open the battery and impedance configuration.
//        m_iHandle = Initialize(1, BATT_IMP_CONFIG);
//        if(m_iHandle < 0)
//          {
//          m_cstrErr.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
//          ((CEquipmentTestApp *)AfxGetApp())->vLog("Error initializing for battery configuration: %s", m_cstrErr);
//          iRet = FAIL;
//          }
//        else
//          { // Calculate number of samples to read before reading battery voltage.
//          fSignalRate = (float)Signal_Rate(m_iHandle, CHANNEL_BATTERY);
//          m_iFirstValidImpSignal = (int)(fSignalRate * READ_BATT_IMP_MIN_TIME) + 1;
//          ((CEquipmentTestApp *)AfxGetApp())->vLog("Signal rate = %f, Number of signals needed: %d", fSignalRate, m_iFirstValidImpSignal);
//          }
//          // Set up a timeout so that we don't get stuck here forever.
//        m_ctStart = CTime::GetCurrentTime();
//        m_iNumPtsRead = 0;
//        }
//      if(m_iHandle >= 0)
//        {
//        iNumPts = Read_Buffer(m_iHandle, CHANNEL_BATTERY, m_dData, 300);
//        m_iNumPtsRead += iNumPts;
//        ctEnd = CTime::GetCurrentTime();
//        ctsElapsed = ctEnd - m_ctStart;
//        ((CEquipmentTestApp *)AfxGetApp())->vLog("Battery voltage, number points read = %d, total points read = %d", iNumPts, m_iNumPtsRead);
//        if(ctsElapsed.GetSeconds() > READ_BATTERY_TIMEOUT)
//          {
//          iRet = FAIL_TIMEOUT;
//          m_cstrErr.LoadString(IDS_ERR_READ_BATTERY_TIMEOUT);
//          ((CEquipmentTestApp *)AfxGetApp())->vLog("Timeout reading battery voltage: %s", m_cstrErr);
//          }
//        else if(iNumPts < 0)
//          {
//          m_cstrErr = Error_Message(-iNumPts); // Error in reading.
//          ((CEquipmentTestApp *)AfxGetApp())->vLog("Error reading battery voltage: %s", m_cstrErr);
//          iRet = FAIL;
//          }
//        else if(m_iNumPtsRead > m_iFirstValidImpSignal)
//          { // Done reading the signals.  Now get the battery voltage.
//          --iNumPts; // Convert to an index so we can look at the last point.
//          if(m_dData[iNumPts] < (double)5.0)
//            {
//            iRet = FAIL_BAD_BATTERY;
//            m_cstrErr.LoadString(IDS_BAD_BATTERY);
//            ((CEquipmentTestApp *)AfxGetApp())->vLog("Battery voltage bad: %.2f", m_dData[iNumPts]);
//            }
//          else if(m_dData[iNumPts] < (double)6.0)
//            {
//            iRet = SUCCESS_LOW_BATTERY;
//            m_cstrErr.LoadString(IDS_LOW_BATTERY);
//            ((CEquipmentTestApp *)AfxGetApp())->vLog("Battery voltage low: %.2f", m_dData[iNumPts]);
//            }
//          else
//            ((CEquipmentTestApp *)AfxGetApp())->vLog("Battery voltage OK: %.2f", m_dData[iNumPts]);
//          }
//        else
//          iRet = SUCCESS_NO_DATA; // Didn't read for a long enough time yet.
//        }
//      if(iRet != SUCCESS_NO_DATA)
//        { // Either got all the points needed and battery voltage is OK
//          // or got an error or battery voltage is bad.
//        m_bFirstBattVoltImpReading = true;
//          // Restore the EGG handle.
//        if(m_iHandle >= 0)
//          Pause(m_iHandle); //Close(m_iHandle);
//        m_iHandle = Initialize(1, EGG_CONFIG);
//        if(m_iHandle < 0)
//          {
//          m_cstrErr.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
//          ((CEquipmentTestApp *)AfxGetApp())->vLog("Error initializing EGG configuration after battery voltage check: %s", m_cstrErr);
//          iRet = FAIL;
//          }
//        }
//      }
//    }
//  return(iRet);
//  }

/********************************************************************
iCheck_impedance - Check the electrode impedance.

                   Impedance is valid only after 2.5 sec.

  inputs: Indicator of what to check.
            CHK_IMPEDANCE_POS: check the positive impedance
            CHK_IMPEDANCE_NEG: Check the negative impedance.
  
  return: SUCCESS if 2.5 seconds has elapsed and the impedance is OK.
          SUCCESS_NO_DATA if less than 2.5 seconds.
          FAIL if there is an error reading the hardware.
          FAIL_TIMEOUT if didn't get enough readings in 2.5 seconds.
          FAIL_BAD_POS_IMPEDANCE if the positive impedance is bad indicating
            a bad connection.
          FAIL_BAD_NEG_IMPEDANCE if the negative impedance is bad indicating
            a bad connection.
********************************************************************/
//short int CDriver::iCheck_impedance(unsigned uCheck)
//  {
//  CTime ctEnd;
//  CTimeSpan ctsElapsed;
//  float fSignalRate;
//  int iNumPts, iChannel;
//  short int iRet = SUCCESS;
//
//  if(m_iUseFakeData == 0)
//    {
//    if(m_iDoElectrodeTest == 1)
//      { // check impedances.
//        // Get the channel number to read.
//      if(uCheck == CHK_IMPEDANCE_POS)
//        iChannel = CHANNEL_POS_IMPEDANCE;
//      else
//        iChannel = CHANNEL_NEG_IMPEDANCE;
//      if(m_bFirstBattVoltImpReading == true)
//        { // First reading.
//        m_bFirstBattVoltImpReading = false;
//          // Close the current configuration.
//        if(m_iHandle >= 0)
//          Pause(m_iHandle); //Close(m_iHandle);
//          // Now open the battery and impedance configuration.
//        m_iHandle = Initialize(1, BATT_IMP_CONFIG);
//        if(m_iHandle < 0)
//          {
//          m_cstrErr.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
//          ((CEquipmentTestApp *)AfxGetApp())->vLog("Error initializing for impedance configuration: %s", m_cstrErr);
//          iRet = FAIL;
//          }
//        else
//          { // Calculate number of samples to read before reading impedance.
//          fSignalRate = (float)Signal_Rate(m_iHandle, iChannel);
//////        fSignalRate *= 3.0F;
//          m_iFirstValidImpSignal = (int)(fSignalRate * READ_BATT_IMP_MIN_TIME) + 1;
//          ((CEquipmentTestApp *)AfxGetApp())->vLog("Signal rate = %f, Number of signals needed: %d", fSignalRate, m_iFirstValidImpSignal);
//          }
//          // Set up a timeout so that we don't get stuck here forever.
//        m_ctStart = CTime::GetCurrentTime();
//        m_iNumPtsRead = 0;
//        }
//      if(m_iHandle >= 0)
//        { // Read the impedance.
//        iNumPts = Read_Buffer(m_iHandle, iChannel, m_dData, 300);
//        m_iNumPtsRead += iNumPts; // Update total points read.
//          // See how long we have been reading the impedance.
//        ctEnd = CTime::GetCurrentTime();
//        ctsElapsed = ctEnd - m_ctStart;
//        if(uCheck == CHK_IMPEDANCE_POS)
//          { // Check the impedance of the positive input.
//          ((CEquipmentTestApp *)AfxGetApp())->vLog("Pos. Impedance: number points read = %d, total points read = %d", iNumPts, m_iNumPtsRead);
//            // Set up a timeout so that we don't get stuck here forever.
//          if(ctsElapsed.GetSeconds() > READ_IMPEDANCE_TIMEOUT)
//            {
//            iRet = FAIL;
//            m_cstrErr.LoadString(IDS_ERR_READ_POS_IMP_TIMEOUT);
//            ((CEquipmentTestApp *)AfxGetApp())->vLog("Timeout reading positive impedance: %s", m_cstrErr);
//            }
//          else if(iNumPts < 0)
//            {
//            m_cstrErr = Error_Message(-iNumPts); // Error in reading.
//            ((CEquipmentTestApp *)AfxGetApp())->vLog("Error reading positive impedance: %s", m_cstrErr);
//            iRet = FAIL;
//            }
//          else if(m_iNumPtsRead > m_iFirstValidImpSignal)
//            {
//            --iNumPts; // Convert to an index so we can look at the last point.
//            ((CEquipmentTestApp *)AfxGetApp())->vLog("Pos Impedance: %.2f (limit = %.2f)",
//                                              m_dData[iNumPts], m_fPosImpedanceLimit);
//            if(m_dData[iNumPts] > (double)m_fPosImpedanceLimit)
//              {
//              iRet = FAIL_BAD_POS_IMPEDANCE;
//              m_cstrErr.LoadString(IDS_BAD_POS_IMPEDANCE);
//              ((CEquipmentTestApp *)AfxGetApp())->vLog("Pos Impedance too high");
//              }
//            }
//          else
//            iRet = SUCCESS_NO_DATA; // Didn't read for a long enough time yet.
//          } // End of check positive impedance.
//        if(uCheck == CHK_IMPEDANCE_NEG)
//          { // Check the impedance of the negative input.
//          ((CEquipmentTestApp *)AfxGetApp())->vLog("Neg. Impedance: number points read = %d, total points read = %d", iNumPts, m_iNumPtsRead);
//          if(ctsElapsed.GetSeconds() > READ_IMPEDANCE_TIMEOUT)
//            {
//            iRet = FAIL;
//            m_cstrErr.LoadString(IDS_ERR_READ_NEG_IMP_TIMEOUT);
//            ((CEquipmentTestApp *)AfxGetApp())->vLog("Timeout reading negative impedance: %s", m_cstrErr);
//            }
//          else if(iNumPts < 0)
//            {
//            m_cstrErr = Error_Message(-iNumPts); // Error in reading.
//            ((CEquipmentTestApp *)AfxGetApp())->vLog("Error reading negative impedance: %s", m_cstrErr);
//            iRet = FAIL;
//            }
//          else if(m_iNumPtsRead > m_iFirstValidImpSignal)
//            {
//            --iNumPts; // Convert to an index so we can look at the last point.
//            ((CEquipmentTestApp *)AfxGetApp())->vLog("Neg Impedance: %.2f (limit = %.2f)",
//                                              m_dData[iNumPts], m_fNegImpedanceLimit);
//            if(m_dData[iNumPts] > (double)m_fNegImpedanceLimit)
//              {
//              iRet = FAIL_BAD_NEG_IMPEDANCE;
//              m_cstrErr.LoadString(IDS_BAD_NEG_IMPEDANCE);
//              ((CEquipmentTestApp *)AfxGetApp())->vLog("Neg Impedance too high");
//              }
//            }
//          else
//            iRet = SUCCESS_NO_DATA; // Didn't read for a long enough time yet.
//          } // End of check negative impedance.
//        }
//      if(iRet != SUCCESS_NO_DATA)
//        { // Either got all the points needed and battery voltage is OK
//          // or got an error or battery voltage is bad.
//        m_bFirstBattVoltImpReading = true;
//          // Restore the EGG handle.
//        if(m_iHandle >= 0)
//          Pause(m_iHandle); //Close(m_iHandle);
//        m_iHandle = Initialize(1, EGG_CONFIG);
//        if(m_iHandle < 0)
//          {
//          m_cstrErr.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
//          ((CEquipmentTestApp *)AfxGetApp())->vLog("Error initializing EGG configuration after impedance check: %s", m_cstrErr);
//          iRet = FAIL;
//          }
//        }
//      }
//    }
//  return(iRet);
//  }

/********************************************************************
vClear_data_buffers - Clear out the driver's data buffers.

  inputs: None.
  
  return: None.
********************************************************************/
void CDriver::vClear_data_buffers(void)
  {
  int iNumPts;

  if(m_iUseFakeData == 0)
    {
    if(m_iHandle >= 0)
      {
      while((iNumPts = Read_Buffer(m_iHandle, CHANNEL_EGG, m_dData, m_uEGGPtsPerSec)) != 0)
        ;
  	  while((iNumPts = Read_Buffer(m_iHandle, CHANNEL_RESP, m_dData, m_uRespPtsPerSec)) != 0)
        ;
      }
    }
  }

/////////////////////////////////////////////////////////////////////
//// Debugging only.
// dump the specified number of data points.
void CDriver::vDump_pts(short int iChannel, short int iType, int iNumPts,
double *pdData, float *pfData)
  {
/*****************
  CString cstrLine, cstrTemp;
  int iCnt, iCnt1;

  if(iType == 0)
//    ((CEquipmentTestApp *)AfxGetApp())->vLog("Channel %d, Data from driver:", iChannel);
  else
//    ((CEquipmentTestApp *)AfxGetApp())->vLog("Channel %d, Data used:", iChannel);

  for(iCnt = 0; iCnt < iNumPts; )
    {
    cstrLine = "    ";
    for(iCnt1 = 0; iCnt1 < 5 && iCnt < iNumPts; ++iCnt, ++iCnt1)
      {
      if(pdData != NULL)
        {
        cstrTemp.Format("%.3f ", *pdData);
        ++pdData;
        }
      else if(pfData != NULL)
        {
        cstrTemp.Format("%.3f ", *pfData);
        ++pfData;
        }
      cstrLine += cstrTemp;
      }
//    ((CEquipmentTestApp *)AfxGetApp())->vLog(cstrLine);
    }
*******************/
  }


/********************************************************************
fGet_fake_data - Generate one data point that is on a sine wave.
                 The amplitude and frequency can be varied.

  inputs: None.
  
  return: Generated data point.
********************************************************************/
//#define GetRandom( min, max ) ((rand() % (int)(((max)+1) - (min))) + (min))

float CDriver::fGet_fake_data()
  {
  float fValue;

    // First get a random number that is between 1 and 15.
  fValue = (float)((rand() % (int)((15 + 1) - 1)) + 1);
    // Create a point that is on a sine wave from the random number.
  fValue = 50.0F + (sin(m_iSinCounter * m_fFakeFreq) * m_fFakeAmp) + fValue; //GetRandom(1, 15);
//  fValue = 50.0F + (sin(m_iSinCounter * 0.075F) * 30.0F) + fValue; //GetRandom(1, 15);
    // SinCounter is only to produce sin wave data //
  if(++m_iSinCounter > 30000)
    m_iSinCounter = 1;
  return(fValue);
  }

