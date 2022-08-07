/********************************************************************
Driver.cpp

Copyright (C) 2003,2004,2005,2011,2012, 3CPM Company, Inc.  All rights reserved.

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
22-FEB-11  RET
                Add new hardware driver that keeps a device ID that must match
                  with the Device ID from the software.
                    Changed methods: Constructor, iInit(), iReInitEGG()
                    Add methods: cstrDev_id_error(), bVerify_devid_src(),
                       vConert_id_to_binary(), bCreate_dev_id_from_comp_name()
                       bCreate_dev_id_from_mac(), bCreate_dev_id(), iValidate_hw()
21-FEB-12  RET  Version 
                  Changes for foreign languages.
22-SEP-12  RET
             Change for 60/200 CPM
               Add method vInitialize() to set the recording frequency and call the
                 drivers initialize function.
                   Change methods: iInit(), iReInitEGG()

Description of how the Device ID is created.
   The Device ID is 8 words longs.
   The first attempted to generate the Device ID is to do it from the
     MAC address.  If this fails (i.e. no network adapter), the second attempt
     is from the Computer Name.
   MAC address method:
     The MAC address is 6 byte long and is put into the 8 word array as follows:
       Word 1: MAC address byte 5 in high order byte
               MAC address byte 2 in low order byte
       Word 2: Random number using Word 1 as a random number seed
       Word 3: Random number using Word 2 as a random number seed
       Word 4: MAC address byte 4 in high order byte
               MAC address byte 6 in low order byte
       Word 5: Random number using Word 4 as a random number seed
       Word 6: Random number using Word 5 as a random number seed
       Word 7: MAC address byte 3 in high order byte
               MAC address byte 1 in low order byte
       Word 8: Random number using Word 7 as a random number seed
   Computer Name method:
       For each 2 characters of the computer name:
         The next device ID word gets the second character in the high byte
           and the first character in the low byte.
         If the name has an odd number of characters, the last byte of the last
           2 byte pair will be 0.
       For any remaining words of the Device ID:
         The word gets a random number using the previous word as a random
           number seed.

Format if Device ID in INI file.
  Each nibble (4 bits) of device id array is converted to an ascii character
    into a character array.  Start with index 0 into the device ID array and the
    high nibble, work down to the low nibble and then do the next word, etc.
    The first character of the character array indicates what was used to
    create the device id.
      '1' - Mac address
      '2' - Computer name.

How the Device ID is used.
  If the Device ID from the hardware is all zeros, the hardware is an
    older device and the Device ID is not used.
  If the ProgramKey indicates the Device ID has not been created (i.e.
    this is a new installation), the Device ID is created, sent to the
    hardware and written to the INI file.
  If the Device ID has previously been created, it is read from the INI
    file.  Then the parts of the Device ID relating to the computer are
    verified as follows:
      If generated using MAC address:
        Verify only the MAC address portion of the Device ID.
      If generated using computer name:
        Verify only using the computer name portion of the Device ID.
      The remaining words in the Device ID have been generated using
        random numbers and can't be verified.
    Then the entire Device ID from the INI file is compared to the
      Device ID read from the HID and they must match.

Errors are generated as follows:
  ERR_DEV_ID_MISMATCH:
    The HID Device ID doesn't match the device ID on this computer.
  ERR_DEV_ID_NEW_DEV_EXIST_INST:
    The hardware is new but the software has been used with another HID.
  ERR_DEV_ID_CREATE:
    Neither the MAC address nor the computer name is available for creating a device ID
  ERR_DEV_ID_COMP_MISMATCH:
    The Device ID on this computer does not match the device ID that was originally created.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "Driver.h"

#include "driverh.h" // For hardware interface.
#include ".\driver.h"
#include "util.h"

#include <cmath>                    // for trigonometry functions
#include "Iphlpapi.h"

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
    // Determines if the battery voltage level check is performed.
  //m_iDoBatteryCheck = AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
  //                                         INI_ITEM_BATTERY_CHECK, 1);
  m_iDoBatteryCheck = g_pConfig->iGet_item(CFG_BATTERYCHECK);
   // Determines if the electrode test is performed.
  //m_iDoElectrodeTest = AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
  //                                         INI_ITEM_ELECTRODE_TEST, 1);
  m_iDoElectrodeTest = g_pConfig->iGet_item(CFG_ELECTRODETEST);
    // Limit for the positive impedance.
  //m_fPosImpedanceLimit = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
  //                                         INI_ITEM_POS_IMPEDANCE_LIMIT, 200);
  m_fPosImpedanceLimit = g_pConfig->fGet_item(CFG_POSIMPEDANCELIMIT);
    // Limit for the negative impedance.
  //m_fNegImpedanceLimit = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
  //                                         INI_ITEM_NEG_IMPEDANCE_LIMIT, 200);
  m_fNegImpedanceLimit = g_pConfig->fGet_item(CFG_NEGIMPEDANCELIMIT);
    // Find out if we are using hardware or fake data.
  //m_iUseFakeData = AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER,
  //                                            INI_ITEM_USE_FAKE_DATA, 0);
  m_iUseFakeData = g_pConfig->iGet_item(CFG_USEFAKEDATA);

  // Read the device ID originally created from the INI file.
  //m_cstrDevID = AfxGetApp()->GetProfileString(INI_SEC_EGGDRIVER, 
  //                                             INI_ITEM_DEVICE_ID, "");
  m_cstrDevID = g_pConfig->cstrGet_item(CFG_DEVID);
  m_iDevIDErr = 0;
  m_iDevIDSrc = DEV_ID_SRC_NONE;

  m_bFirstBattVoltImpReading = true;
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
      //cstrMsg.Format(IDS_ERR_LOAD_DLL, DLL_NAME);
      cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_LOAD_DLL), DLL_NAME);
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
      //// 22-FEB-11 New driver, removed commands
      //Read_DFT       = (int   (__cdecl *)(int,int,double*,int,int,double,int))GetProcAddress(m_hDLL,(LPCSTR)"Read_DFT");
			//Is_Impedance_Mode = (int   (__cdecl *)(int))GetProcAddress(m_hDLL,(LPCSTR)"Is_Impedance_Mode");
      //Reset_CO2_T0_0 = (double(__cdecl *)(int))GetProcAddress(m_hDLL,(LPCSTR)"Reset_CO2_T0_0");
      //// 22-FEB-11 New driver, new commands
			Send_Data	   = (int   (__cdecl *)(int,short*))GetProcAddress(m_hDLL,(LPCSTR)"Send_Data");
			Get_Data	   = (int   (__cdecl *)(int,short*))GetProcAddress(m_hDLL,(LPCSTR)"Get_Data");

      if(Initialize == NULL || Error_Message == NULL || Read_Buffer == NULL
      || Total_Channels == NULL || Maximum_Value == NULL || Minimum_Value == NULL
      || Signal_Rate == NULL || Signal_Name == NULL || Close == NULL
      || Pause == NULL || Send_Command == NULL || Set_File_Name == NULL
      //|| Read_DFT == NULL || Is_Impedance_Mode == NULL || Reset_CO2_T0_0 == NULL
      || Send_Data == NULL || Get_Data == NULL)
        {
        //cstrMsg.Format(IDS_ERR_DLL_FUNCS_NOT_FOUND, DLL_NAME);
        cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_DLL_FUNCS_NOT_FOUND), DLL_NAME);
        ((CEGGSASApp *)AfxGetApp())->vLog("iInit: error getting entry points for DLL functions: %s", cstrMsg);
        iRet = FAIL;
        }
      else
        { // Got all functions for DLL.
          // Configuration # 2
        //m_iHandle = Initialize(1, EGG_CONFIG);
        vInitialize();
        if(m_iHandle < 0)
          {
          //cstrMsg.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
          cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_DLL), DLL_NAME, Error_Message(-m_iHandle));
          ((CEGGSASApp *)AfxGetApp())->vLog("iInit: error initializing EGG configuration: %s", cstrMsg);
          iRet = FAIL;
          }
        else
          {
          // Make sure the Device ID from the INI file matches that of the hardware.
          // If this is a new installation, create a Device ID.
          if(iValidate_hw() == ERR_DEV_ID_NONE)
            {
            iTotal = Total_Channels(m_iHandle);
            if(iTotal < 0)
              {
              //cstrMsg.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
              cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_DLL), DLL_NAME, Error_Message(-m_iHandle));
              ((CEGGSASApp *)AfxGetApp())->vLog("iInit: error getting number of channels: %s", cstrMsg);
              iRet = FAIL;
              }
            else if(iTotal != 2)
              {
              //cstrMsg.LoadString(IDS_ERR_DLL_CHANNELS);
              cstrMsg = g_pLang->cstrLoad_string(ITEM_G_ERR_DLL_CHANNELS);
              ((CEGGSASApp *)AfxGetApp())->vLog("iInit: wrong number of channels: %s", cstrMsg);
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
              //m_fRespMult = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER, 
              //                        INI_ITEM_RESP_MULT, 1000);
              m_fRespMult = g_pConfig->fGet_item(CFG_RESPMULT);
              m_fMinResp *= m_fRespMult;
              m_fMaxResp *= m_fRespMult;
              }
            }
          else
            { // Dev ID didn't match that of the hardware.
            cstrMsg = cstrDev_id_error();
            iRet = FAIL;
            }
          }
        }
      }
    if(iRet == FAIL)
      {
      //cstrErr.LoadString(IDS_ERROR1);
      cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERROR);
      (CEGGSASApp *)AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErr, MB_OK);
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
    //m_fRespMult = (float)AfxGetApp()->GetProfileInt(INI_SEC_EGGDRIVER, 
    //                           INI_ITEM_RESP_MULT, 1000);
    m_fRespMult = g_pConfig->fGet_item(CFG_RESPMULT);
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
        ((CEGGSASApp *)AfxGetApp())->vLog("iGet_data: error reading data channel %d: %s", iChannel, m_cstrErr);
          // Set the error return.
        if(iNumPts == WARN_LOW_BATTERY)
          iNumPts = FAIL_EGG_DRIVER_LOW_BATT;
        else
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
short int CDriver::iReStart()
  {
  CString cstrErr, cstrMsg;
  int iRet = SUCCESS;

  if(m_iUseFakeData == 0)
    {
    if(m_iHandle >= 0)
      vClear_data_buffers();
    }
  else
    { // Using fake data.
    m_iFakePntCnt = 0;
    }

  m_uEGGPtCnt = 0;
  m_uRespPtCnt = 0;

  m_iTotalEGGPts = 0;
  m_iTotalRespPts = 0;

  m_iNumEGGPts = 0;
  m_iNumRespPts = 0;

  return(iRet);
  }

/********************************************************************
iReInitEGG - Reinitialize the EGG hardware driver.  The driver must already have
             been initialized.

  inputs: None.
  
  Output: None.

  return: SUCCESS if driver is restarted.
          FAIL if there is an error.
********************************************************************/
short int CDriver::iReInitEGG()
  {
  CString cstrErr, cstrMsg;
  int iRet = SUCCESS;

  if(m_iUseFakeData == 0)
    {
    if(m_iDevIDErr == ERR_DEV_ID_NONE)
      { // The Device ID is OK.
      if(m_iHandle >= 0)
        Pause(m_iHandle); //Close(m_iHandle);

      //m_iHandle = Initialize(1, EGG_CONFIG);
      vInitialize();
      if(m_iHandle < 0)
        {
        //cstrMsg.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
        cstrMsg.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_DLL), DLL_NAME, Error_Message(-m_iHandle));
        //cstrErr.LoadString(IDS_ERROR1);
        cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERROR);
        (CEGGSASApp *)AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErr, MB_OK);
        ((CEGGSASApp *)AfxGetApp())->vLog("iReInitEGG: error initializing EGG configuration: %s", cstrMsg);
        iRet = FAIL;
        }
      }
    else
      { // Device ID mismatch.
      cstrMsg = cstrDev_id_error();
      iRet = FAIL;
      //cstrErr.LoadString(IDS_ERROR1);
      cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERROR);
      (CEGGSASApp *)AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrErr, MB_OK);
      }
    }
  else
    { // Using fake data.
    m_iFakePntCnt = 0;
    }

  m_uEGGPtCnt = 0;
  m_uRespPtCnt = 0;

  m_iTotalEGGPts = 0;
  m_iTotalRespPts = 0;

  m_iNumEGGPts = 0;
  m_iNumRespPts = 0;

  return(iRet);
  }

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

/*************** DEBUG
//    ((CEGGSASApp *)AfxGetApp())->vLog("m_iNumEGGPts: %d, m_iNumRespPts: %d", m_iNumEGGPts, m_iNumRespPts);
    CString cstrDebug, cstrTemp;
    cstrDebug.Format("    iNumNewEGGPts=%d: ", iNumNewEGGPts);
    for(iCnt = 0; iCnt < m_iNumEGGPts; ++iCnt)
      {
      cstrTemp.Format("%.3f ", m_fEGGData[iCnt]);
      cstrDebug += cstrTemp;
      }
//    ((CEGGSASApp *)AfxGetApp())->vLog(cstrDebug);
    cstrDebug.Format("    iNewNumRespPts=%d: ", iNewNumRespPts);
    for(iCnt = 0; iCnt < m_iNumRespPts; ++iCnt)
      {
      cstrTemp.Format("%.3f ", m_fRespData[iCnt]);
      cstrDebug += cstrTemp;
      }
//    ((CEGGSASApp *)AfxGetApp())->vLog(cstrDebug);
************************/
    
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
//      ((CEGGSASApp *)AfxGetApp())->vLog("Updated m_iNumEGGPts: %d", m_iNumEGGPts);
      m_iNumRespPts = 0;
      memmove(m_fEGGData, &m_fEGGData[iPtsToGraph], m_iNumEGGPts * sizeof(float));
      }
    else if(m_iNumRespPts > m_iNumEGGPts)
      { // Didn't graph all EGG points, but did graph all respiration points.
      m_iNumRespPts = m_iNumRespPts - m_iNumEGGPts;
//      ((CEGGSASApp *)AfxGetApp())->vLog("Updated m_iNumRespPts: %d", m_iNumRespPts);
      m_iNumEGGPts = 0;
      memmove(m_fRespData, &m_fRespData[iPtsToGraph], m_iNumRespPts * sizeof(float));
      }
    else // if(iNumRespPts == iNumEGGPts)
      {
      m_iNumRespPts = 0;
      m_iNumEGGPts = 0;
//      ((CEGGSASApp *)AfxGetApp())->vLog("Updated m_iNumEGGPts: %d, m_iNumRespPts: %d", m_iNumEGGPts, m_iNumRespPts);
      }
    }
  else
    {
    if(iNumNewEGGPts == WARN_LOW_BATTERY || iNewNumRespPts == WARN_LOW_BATTERY)
      iPtsToGraph = FAIL_EGG_DRIVER_LOW_BATT;
    else
      iPtsToGraph = FAIL_EGG_DRIVER;
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
short int CDriver::iCheck_battery(void)
  {
  CString cstrMsg;
  CTime ctEnd;
  CTimeSpan ctsElapsed;
  float fSignalRate;
  int iNumPts;
  short int iRet = SUCCESS;

  if(m_iUseFakeData == 0)
    {
    if(m_iDoBatteryCheck == 1)
      { // Check the battery voltage.
      if(m_bFirstBattVoltImpReading == true)
        { // First reading.
        m_bFirstBattVoltImpReading = false;
          // Close the current configuration.
        if(m_iHandle >= 0)
          Pause(m_iHandle); //Close(m_iHandle);
          // Now open the battery and impedance configuration.
        m_iHandle = Initialize(1, BATT_IMP_CONFIG);
        if(m_iHandle < 0)
          {
          //m_cstrErr.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
          m_cstrErr.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_DLL), DLL_NAME, Error_Message(-m_iHandle));
          ((CEGGSASApp *)AfxGetApp())->vLog("Error initializing for battery configuration: %s", m_cstrErr);
          iRet = FAIL;
          }
        else
          { // Calculate number of samples to read before reading battery voltage.
          fSignalRate = (float)Signal_Rate(m_iHandle, CHANNEL_BATTERY);
          m_iFirstValidImpSignal = (int)(fSignalRate * READ_BATT_IMP_MIN_TIME) + 1;
          ((CEGGSASApp *)AfxGetApp())->vLog("Signal rate = %f, Number of signals needed: %d", fSignalRate, m_iFirstValidImpSignal);
          }
          // Set up a timeout so that we don't get stuck here forever.
        m_ctStart = CTime::GetCurrentTime();
        m_iNumPtsRead = 0;
        }
      if(m_iHandle >= 0)
        {
        iNumPts = Read_Buffer(m_iHandle, CHANNEL_BATTERY, m_dData, 300);
        m_iNumPtsRead += iNumPts;
        ctEnd = CTime::GetCurrentTime();
        ctsElapsed = ctEnd - m_ctStart;
        ((CEGGSASApp *)AfxGetApp())->vLog("Battery voltage, number points read = %d, total points read = %d", iNumPts, m_iNumPtsRead);
        if(ctsElapsed.GetSeconds() > READ_BATTERY_TIMEOUT)
          {
          iRet = FAIL_TIMEOUT;
          //m_cstrErr.LoadString(IDS_ERR_READ_BATTERY_TIMEOUT);
          m_cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERR_READ_BATTERY_TIMEOUT);
          ((CEGGSASApp *)AfxGetApp())->vLog("Timeout reading battery voltage: %s", m_cstrErr);
          }
        else if(iNumPts < 0)
          {
          m_cstrErr = Error_Message(-iNumPts); // Error in reading.
          ((CEGGSASApp *)AfxGetApp())->vLog("Error reading battery voltage: %s", m_cstrErr);
          iRet = FAIL;
          }
        else if(m_iNumPtsRead > m_iFirstValidImpSignal)
          { // Done reading the signals.  Now get the battery voltage.
          --iNumPts; // Convert to an index so we can look at the last point.
          if(m_dData[iNumPts] < (double)5.0)
            {
            iRet = FAIL_BAD_BATTERY;
            //m_cstrErr.LoadString(IDS_BAD_BATTERY);
            m_cstrErr = g_pLang->cstrLoad_string(ITEM_G_BAD_BATTERY);
            ((CEGGSASApp *)AfxGetApp())->vLog("Battery voltage bad: %.2f", m_dData[iNumPts]);
            }
          else if(m_dData[iNumPts] < (double)6.0)
            {
            iRet = SUCCESS_LOW_BATTERY;
            //m_cstrErr.LoadString(IDS_LOW_BATTERY);
            m_cstrErr = g_pLang->cstrLoad_string(ITEM_G_LOW_BATTERY);
            ((CEGGSASApp *)AfxGetApp())->vLog("Battery voltage low: %.2f", m_dData[iNumPts]);
            }
          else
            ((CEGGSASApp *)AfxGetApp())->vLog("Battery voltage OK: %.2f", m_dData[iNumPts]);
          }
        else
          iRet = SUCCESS_NO_DATA; // Didn't read for a long enough time yet.
        }
      if(iRet != SUCCESS_NO_DATA)
        { // Either got all the points needed and battery voltage is OK
          // or got an error or battery voltage is bad.
        m_bFirstBattVoltImpReading = true;
          // Restore the EGG handle.
        if(m_iHandle >= 0)
          Pause(m_iHandle); //Close(m_iHandle);
        m_iHandle = Initialize(1, EGG_CONFIG);
        if(m_iHandle < 0)
          {
          //m_cstrErr.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
          m_cstrErr.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_DLL), DLL_NAME, Error_Message(-m_iHandle));
          ((CEGGSASApp *)AfxGetApp())->vLog("Error initializing EGG configuration after battery voltage check: %s", m_cstrErr);
          iRet = FAIL;
          }
        }
      }
    }
  return(iRet);
  }

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
short int CDriver::iCheck_impedance(unsigned uCheck)
  {
  CTime ctEnd;
  CTimeSpan ctsElapsed;
  float fSignalRate;
  int iNumPts, iChannel;
  short int iRet = SUCCESS;

  if(m_iUseFakeData == 0)
    {
    if(m_iDoElectrodeTest == 1)
      { // check impedances.
        // Get the channel number to read.
      if(uCheck == CHK_IMPEDANCE_POS)
        iChannel = CHANNEL_POS_IMPEDANCE;
      else
        iChannel = CHANNEL_NEG_IMPEDANCE;
      if(m_bFirstBattVoltImpReading == true)
        { // First reading.
        m_bFirstBattVoltImpReading = false;
          // Close the current configuration.
        if(m_iHandle >= 0)
          Pause(m_iHandle); //Close(m_iHandle);
          // Now open the battery and impedance configuration.
        m_iHandle = Initialize(1, BATT_IMP_CONFIG);
        if(m_iHandle < 0)
          {
          //m_cstrErr.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
          m_cstrErr.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_DLL), DLL_NAME, Error_Message(-m_iHandle));
          ((CEGGSASApp *)AfxGetApp())->vLog("Error initializing for impedance configuration: %s", m_cstrErr);
          iRet = FAIL;
          }
        else
          { // Calculate number of samples to read before reading impedance.
          fSignalRate = (float)Signal_Rate(m_iHandle, iChannel);
////        fSignalRate *= 3.0F;
          m_iFirstValidImpSignal = (int)(fSignalRate * READ_BATT_IMP_MIN_TIME) + 1;
          ((CEGGSASApp *)AfxGetApp())->vLog("Signal rate = %f, Number of signals needed: %d", fSignalRate, m_iFirstValidImpSignal);
          }
          // Set up a timeout so that we don't get stuck here forever.
        m_ctStart = CTime::GetCurrentTime();
        m_iNumPtsRead = 0;
        }
      if(m_iHandle >= 0)
        { // Read the impedance.
        iNumPts = Read_Buffer(m_iHandle, iChannel, m_dData, 300);
        m_iNumPtsRead += iNumPts; // Update total points read.
          // See how long we have been reading the impedance.
        ctEnd = CTime::GetCurrentTime();
        ctsElapsed = ctEnd - m_ctStart;
        if(uCheck == CHK_IMPEDANCE_POS)
          { // Check the impedance of the positive input.
          ((CEGGSASApp *)AfxGetApp())->vLog("Pos. Impedance: number points read = %d, total points read = %d", iNumPts, m_iNumPtsRead);
            // Set up a timeout so that we don't get stuck here forever.
          if(ctsElapsed.GetSeconds() > READ_IMPEDANCE_TIMEOUT)
            {
            iRet = FAIL;
            //m_cstrErr.LoadString(IDS_ERR_READ_POS_IMP_TIMEOUT);
            m_cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERR_READ_POS_IMP_TIMEOUT);
            ((CEGGSASApp *)AfxGetApp())->vLog("Timeout reading positive impedance: %s", m_cstrErr);
            }
          else if(iNumPts < 0)
            {
            m_cstrErr = Error_Message(-iNumPts); // Error in reading.
            ((CEGGSASApp *)AfxGetApp())->vLog("Error reading positive impedance: %s", m_cstrErr);
            iRet = FAIL;
            }
          else if(m_iNumPtsRead > m_iFirstValidImpSignal)
            {
            --iNumPts; // Convert to an index so we can look at the last point.
            ((CEGGSASApp *)AfxGetApp())->vLog("Pos Impedance: %.2f (limit = %.2f)",
                                              m_dData[iNumPts], m_fPosImpedanceLimit);
            if(m_dData[iNumPts] > (double)m_fPosImpedanceLimit)
              {
              iRet = FAIL_BAD_POS_IMPEDANCE;
              //m_cstrErr.LoadString(IDS_BAD_POS_IMPEDANCE);
              m_cstrErr = g_pLang->cstrLoad_string(ITEM_G_BAD_POS_IMPEDANCE);
              ((CEGGSASApp *)AfxGetApp())->vLog("Pos Impedance too high");
              }
            }
          else
            iRet = SUCCESS_NO_DATA; // Didn't read for a long enough time yet.
          } // End of check positive impedance.
        if(uCheck == CHK_IMPEDANCE_NEG)
          { // Check the impedance of the negative input.
          ((CEGGSASApp *)AfxGetApp())->vLog("Neg. Impedance: number points read = %d, total points read = %d", iNumPts, m_iNumPtsRead);
          if(ctsElapsed.GetSeconds() > READ_IMPEDANCE_TIMEOUT)
            {
            iRet = FAIL;
            //m_cstrErr.LoadString(IDS_ERR_READ_NEG_IMP_TIMEOUT);
            m_cstrErr = g_pLang->cstrLoad_string(ITEM_G_ERR_READ_NEG_IMP_TIMEOUT);
           ((CEGGSASApp *)AfxGetApp())->vLog("Timeout reading negative impedance: %s", m_cstrErr);
            }
          else if(iNumPts < 0)
            {
            m_cstrErr = Error_Message(-iNumPts); // Error in reading.
            ((CEGGSASApp *)AfxGetApp())->vLog("Error reading negative impedance: %s", m_cstrErr);
            iRet = FAIL;
            }
          else if(m_iNumPtsRead > m_iFirstValidImpSignal)
            {
            --iNumPts; // Convert to an index so we can look at the last point.
            ((CEGGSASApp *)AfxGetApp())->vLog("Neg Impedance: %.2f (limit = %.2f)",
                                              m_dData[iNumPts], m_fNegImpedanceLimit);
            if(m_dData[iNumPts] > (double)m_fNegImpedanceLimit)
              {
              iRet = FAIL_BAD_NEG_IMPEDANCE;
              //m_cstrErr.LoadString(IDS_BAD_NEG_IMPEDANCE);
              m_cstrErr = g_pLang->cstrLoad_string(ITEM_G_BAD_NEG_IMPEDANCE);
              ((CEGGSASApp *)AfxGetApp())->vLog("Neg Impedance too high");
              }
            }
          else
            iRet = SUCCESS_NO_DATA; // Didn't read for a long enough time yet.
          } // End of check negative impedance.
        }
      if(iRet != SUCCESS_NO_DATA)
        { // Either got all the points needed and battery voltage is OK
          // or got an error or battery voltage is bad.
        m_bFirstBattVoltImpReading = true;
          // Restore the EGG handle.
        if(m_iHandle >= 0)
          Pause(m_iHandle); //Close(m_iHandle);
        m_iHandle = Initialize(1, EGG_CONFIG);
        if(m_iHandle < 0)
          {
          //m_cstrErr.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
          m_cstrErr.Format(g_pLang->cstrLoad_string(ITEM_G_ERR_DLL), DLL_NAME, Error_Message(-m_iHandle));
          ((CEGGSASApp *)AfxGetApp())->vLog("Error initializing EGG configuration after impedance check: %s", m_cstrErr);
          iRet = FAIL;
          }
        }
      }
    }
  return(iRet);
  }

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
//    ((CEGGSASApp *)AfxGetApp())->vLog("Channel %d, Data from driver:", iChannel);
  else
//    ((CEGGSASApp *)AfxGetApp())->vLog("Channel %d, Data used:", iChannel);

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
//    ((CEGGSASApp *)AfxGetApp())->vLog(cstrLine);
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

/********************************************************************
iValidate_hw - Handles the validating/creating the Device ID that is
               initially put into the HID.

inputs: None.

return: If no errors, ERR_DEV_ID_NONE.
        If errors, either a driver error or an error listed in Driver.h
********************************************************************/
int CDriver::iValidate_hw()
  {
  int iIndex, iIndex1;
  CString cstrDevID, cstrPgmKey;
  bool bDevIDOk, bOldDev;
  short iDevID[DEVICE_ID_SIZE]; // Hardware Device ID.
  short iINIDevID[DEVICE_ID_SIZE]; // Hardware Device ID converted from the INI file.
  char szDevID[DEVICE_MAX_WORDS * 5];

  // Check the Hardware device ID
  m_iDevIDErr = Get_Data(m_iHandle, iDevID);
  if(m_iDevIDErr == 0)
    {
    bDevIDOk = false;
    bOldDev = true;
    // First check is for all zeros, this indicates older hardware.
    for(iIndex = 0;  iIndex < DEVICE_MAX_WORDS; ++iIndex)
      {
      if(iDevID[iIndex] != 0)
        {
        bOldDev = false;
        break; // Not older device.
        }
      }
    if(bOldDev == false)
      { // Not and older device.
        // Check to see if this is a new installation, and if so, generate the device ID.
      //cstrPgmKey = AfxGetApp()->GetProfileString(INI_SEC_GENERAL, INI_ITEM_PGMKEY, "");
      cstrPgmKey = g_pConfig->cstrGet_item(CFG_PGMKEY);
      if(cstrPgmKey[INI_DEV_ID_INDEX_1] == INI_DEV_ID_NOT_GEN_1
        && cstrPgmKey[INI_DEV_ID_INDEX_2] == INI_DEV_ID_NOT_GEN_2)
        {
        m_cstrDevID.Empty();
        bDevIDOk = false;
        }
      else
        {
        // If the first 8 words are all minus 1, this is new hardware, so send it
        // an 8 word (16 byte) data ID.
        for(iIndex = 0; iIndex < DEVICE_MAX_WORDS; ++iIndex)
          {
          if(iDevID[iIndex] != -1)
            {
            bDevIDOk = true;
            break; // already initialized.
            }
          }
        }
      if(bDevIDOk == false)
        { // Device is a new, uninitialized device.
        // The ID kept in the INI file must be empty or else it is a new device
        // with and existing installation.  This is not allowed.
        if(m_cstrDevID.IsEmpty() == true)
          {
          // generate a 16 bit ID and send it to the device, the Software Device ID.
          if(bCreate_dev_id() == true)
            { // This put it in m_iSWDevID[], the function always returns true.
              // Add as the leading character the source of the device ID so when
              // we verify it, we know what to verify.
            szDevID[0] = m_iDevIDSrc + '0';
              // Convert to an ASCII string to write to the INI file.
            for(iIndex = 1, iIndex1 = 0;  iIndex1 < DEVICE_MAX_WORDS; ++iIndex, ++iIndex1)
              {
              szDevID[iIndex] = cHex_to_ascii((m_iDevID[iIndex1] & 0xF000) >> 12);
              szDevID[++iIndex] = cHex_to_ascii((m_iDevID[iIndex1] & 0x0F00) >> 8);
              szDevID[++iIndex] = cHex_to_ascii((m_iDevID[iIndex1] & 0x00F0) >> 4);
              szDevID[++iIndex] = cHex_to_ascii(m_iDevID[iIndex1] & 0x000F);
              }
            szDevID[iIndex] = NULL;
            cstrDevID = szDevID;
            // Write the software device ID to the INI file.
            //AfxGetApp()->WriteProfileString(INI_SEC_EGGDRIVER, 
            //                                INI_ITEM_DEVICE_ID, cstrDevID);
            g_pConfig->vWrite_item(CFG_DEVID, cstrDevID);
            // Update the program key to indicate that the device id had been generated.
            cstrPgmKey.SetAt(INI_DEV_ID_INDEX_1, INI_DEV_ID_GEN_1);
            cstrPgmKey.SetAt(INI_DEV_ID_INDEX_2, INI_DEV_ID_GEN_2);
            //cstrPgmKey[INI_DEV_ID_INDEX_1] = INI_DEV_ID_GEN_1;
            //cstrPgmKey[INI_DEV_ID_INDEX_2] = INI_DEV_ID_GEN_2;
            //AfxGetApp()->WriteProfileString(INI_SEC_GENERAL, INI_ITEM_PGMKEY, cstrPgmKey);
            g_pConfig->vWrite_item(CFG_PGMKEY, cstrPgmKey);
           // Write the software device ID to the hardware.
			      m_iDevID[DEVICE_ID_PASSWD_1_INDEX] = (short)DEVICE_ID_PASSWD_1;	// WriteID PassWord 1
			      m_iDevID[DEVICE_ID_PASSWD_2_INDEX] = (short)DEVICE_ID_PASSWD_2;	// WriteID PassWord 2 
			      //Write device ID number
			      m_iDevIDErr = Send_Data(m_iHandle, m_iDevID);
            }
          }
        else
          { // New device with existing installation, error.
          m_iDevIDErr = ERR_DEV_ID_NEW_DEV_EXIST_INST;
          }
        }
      else
        { // Device already has an ID, verify it with what we have.
        if(m_cstrDevID.IsEmpty() == true)
          m_iDevIDErr = ERR_DEV_ID_COMP_MISMATCH;
        else
          {
          // If hardware device ID is valid, read the software device ID
          // from the INI file and send it to the hardware.
          // Convert the hardware ID string from the INI file to a binary array of words.
          vConvert_id_to_binary(m_cstrDevID, iINIDevID);

          // First verify the source of the device ID.
          if(bVerify_devid_src(iINIDevID) == true)
            {
            // Now compare the ID from the INI file and the ID from the hardware.
            for(iIndex = 0; iIndex < DEVICE_MAX_WORDS; ++iIndex)
              {
              if(iDevID[iIndex] != iINIDevID[iIndex])
                break;
              }
            if(iIndex < DEVICE_MAX_WORDS)
              m_iDevIDErr = ERR_DEV_ID_MISMATCH;
            else
              { // Device ID is correct.
              m_iDevIDErr = ERR_DEV_ID_NONE;
              }
            }
          else
            m_iDevIDErr = ERR_DEV_ID_COMP_MISMATCH;
          }
        }
      }
    }

  return(m_iDevIDErr);
  }

/********************************************************************
bCreate_dev_id - Create a new device ID.  See the top of the file
                 for a description of how the device ID is created.

inputs: None.

return: true if the Device ID was created, otherwise false.
********************************************************************/
bool CDriver::bCreate_dev_id()
  {
  bool bRet;

  // Needs to be 8 words long.
  if((bRet = bCreate_dev_id_from_mac()) == false)
    bRet = bCreate_dev_id_from_comp_name();
  if(bRet == false)
    { // Couldn't get a unique ID.
    m_iDevIDErr = ERR_DEV_ID_CREATE;
    }

  return(bRet);
  }

/********************************************************************
bCreate_dev_id_from_mac - Create a new device ID from the MAC address.

                          See the top of the file for a description of
                          how the device ID is created.

inputs: None.

return: true if the Device ID was created, otherwise false.
********************************************************************/
bool CDriver::bCreate_dev_id_from_mac()
  {
  bool bRet;
  CString cstrMacAddr;
  IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information for up to 16 NICs
  DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer
  DWORD dwStatus;

  // Needs to be 8 words long.

  // Call GetAdapterInfo ([out] buffer to receive data, [in] size of receive data buffer)
  dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);

  if(dwStatus == ERROR_SUCCESS)
    {
    bRet = true;
    m_iDevIDSrc = DEV_ID_SRC_MAC;
    // Get pointer to linked list of current adapter info
    PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
    // Adapter Mac address is 6 bytes.
    m_iDevID[0] = (short)(pAdapterInfo->Address[4] << 8) | pAdapterInfo->Address[1];
    srand(m_iDevID[0]);
    m_iDevID[1] = rand();
    srand(m_iDevID[1]);
    m_iDevID[2] = rand();

    m_iDevID[3] = (short)(pAdapterInfo->Address[3] << 8) | pAdapterInfo->Address[5];
    srand(m_iDevID[3]);
    m_iDevID[4] = rand();
    srand(m_iDevID[4]);
    m_iDevID[5] = rand();

    m_iDevID[6] = (short)(pAdapterInfo->Address[2] << 8) | pAdapterInfo->Address[0];
    srand(m_iDevID[6]);
    m_iDevID[7] = rand();
    }
  else
    {
    bRet = false;
    m_iDevIDSrc = DEV_ID_SRC_NONE;
    }
  return(bRet);
  }

/********************************************************************
bCreate_dev_id_from_comp_name - Create a new device ID from the Computer Name.

                                See the top of the file for a description of
                                how the device ID is created.

inputs: None.

return: true if the Device ID was created, otherwise false.
********************************************************************/
bool CDriver::bCreate_dev_id_from_comp_name()
  {
  bool bRet;
  char szCompName[MAX_COMPUTERNAME_LENGTH * 2];
  DWORD dwSize;
  unsigned uIndex;
  int iIDIndex;

  dwSize = sizeof(szCompName);
  if(GetComputerName(szCompName, &dwSize) == TRUE && dwSize > 0)
    {
    //strcpy(szCompName, "RTCC-DELLABCDEFGHIJK");
    //dwSize = strlen(szCompName);
    bRet = true;
    m_iDevIDSrc = DEV_ID_SRC_COMP_NAME;
    // Put the computer name into the device ID array.
    for(uIndex = 0, iIDIndex = 0; uIndex < dwSize && uIndex < 16; ++iIDIndex, uIndex += 2)
      m_iDevID[iIDIndex] = (short)(szCompName[uIndex + 1] << 8) | szCompName[uIndex];
      // Computer name wasn't long enough to fill in the complete device ID.
    while(iIDIndex < DEVICE_MAX_WORDS)
      {
      srand(m_iDevID[iIDIndex - 1]);
      m_iDevID[iIDIndex] = rand();
      ++iIDIndex;
      }
    }
  else
    {
    bRet = false;
    m_iDevIDSrc = DEV_ID_SRC_NONE;
    }
  return(bRet);
  }

/********************************************************************
vConvert_id_to_binary - Convert an ASCII representation of the Device ID to
                        a binary number.

                        Each 4 Ascii characters get converted to a binary word.

inputs: CString object containing the Device ID.
        Pointer to a short array that gets the converted Device ID.

return:  None.
********************************************************************/
void CDriver::vConvert_id_to_binary(CString cstrDevID, short *piDevID)
  {
  short iTemp1, iTemp2, iTemp3, iTemp4;
  int iIndex, iIndex1;

  m_iDevIDSrc = ucAscii_to_Hex(cstrDevID[0]);
  //for(iIndex = 0, iIndex1 = 1;  cstrDevID[iIndex1] != NULL; ++iIndex, ++iIndex1)
  for(iIndex = 0, iIndex1 = 1; iIndex1 < cstrDevID.GetLength(); ++iIndex, ++iIndex1)
    {
    iTemp1 = (short)ucAscii_to_Hex(cstrDevID[iIndex1]) << 12;
    if(++iIndex1 < cstrDevID.GetLength()) //cstrDevID[++iIndex1] != NULL)
      {
      iTemp2 = (short)ucAscii_to_Hex(cstrDevID[iIndex1]) << 8;
      if(++iIndex1 < cstrDevID.GetLength()) //cstrDevID[++iIndex1] != NULL)
        {
        iTemp3 = (short)ucAscii_to_Hex(cstrDevID[iIndex1]) << 4;
        if(++iIndex1 < cstrDevID.GetLength()) //cstrDevID[++iIndex1] != NULL)
          {
          iTemp4 = (short)ucAscii_to_Hex(cstrDevID[iIndex1]);
          *(piDevID + iIndex) = iTemp1 + iTemp2 + iTemp3 + iTemp4;
          }
        }
      }
    }
  }

/********************************************************************
bVerify_devid_src - Verify that the Device ID from the INI file is the one
                    that was generated originally.
                    If generated using MAC address:
                      Verify only the MAC address portion of the Device ID.
                    If generated using computer name:
                      Verify only using the computer name portion of the Device ID.
                    The remaining words in the Device ID have been generated using
                      random numbers and can't be verified.

inputs: Pointer to a short array containing Device ID from the INI file.

return:  true if the Device ID successfully verified, otherwise false.
********************************************************************/
bool CDriver::bVerify_devid_src(short *piDevID)
  {
  bool bRet = false;

  if(m_iDevIDSrc == DEV_ID_SRC_MAC)
    { // Verify against the MAC address.
    IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information for up to 16 NICs
    DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer
    DWORD dwStatus;

    // Call GetAdapterInfo ([out] buffer to receive data, [in] size of receive data buffer)
    if((dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen)) == ERROR_SUCCESS)
      {
      PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
      if(*piDevID == ((short)(pAdapterInfo->Address[4] << 8) | pAdapterInfo->Address[1])
        && *(piDevID + 3) == ((short)(pAdapterInfo->Address[3] << 8) | pAdapterInfo->Address[5])
        && *(piDevID + 6) == ((short)(pAdapterInfo->Address[2] << 8) | pAdapterInfo->Address[0]))
          bRet = true;
      }
    }
  else if(m_iDevIDSrc == DEV_ID_SRC_COMP_NAME)
    { // Verify against the computer name.
    char szCompName[MAX_COMPUTERNAME_LENGTH * 2];
    DWORD dwSize;
    unsigned uIndex;
    short iIDIndex;

    dwSize = sizeof(szCompName);
    if(GetComputerName(szCompName, &dwSize) == TRUE && dwSize > 0)
      {
      bRet = true;
      for(uIndex = 0, iIDIndex = 0; uIndex < dwSize && uIndex < 16; ++iIDIndex, uIndex += 2)
        {
        if(*(piDevID + iIDIndex) != ((short)(szCompName[uIndex + 1] << 8) | szCompName[uIndex]))
          {
          bRet = false;
          break;
          }
        }
      }
    }

  return(bRet);
  }

/********************************************************************
cstrDev_id_error - Get an error message corresponding to the Device ID error.

inputs: None.

return:  CString object containing the error message.
********************************************************************/
CString CDriver::cstrDev_id_error()
  {
  CString cstrErr;

  if(m_iDevIDErr <= ERR_DEV_ID_MISMATCH)
    {
    switch(m_iDevIDErr)
      {
      case ERR_DEV_ID_MISMATCH:
        cstrErr = "The HID Device ID doesn't match the device ID on this computer.";
        break;
      case ERR_DEV_ID_NEW_DEV_EXIST_INST:
        cstrErr = "The hardware is new but the software has been used with another HID.";
        break;
      case ERR_DEV_ID_CREATE:
        cstrErr = "Neither the MAC address nor the computer name is available for creating a device ID";
        break;
      case ERR_DEV_ID_COMP_MISMATCH:
        cstrErr = "The Device ID on this computer does not match the device ID that was originally created.";
        break;
      default:
        cstrErr = "Unknown device ID error.";
        break;
      }
    }
  else
    {
    cstrErr = Error_Message(-m_iDevIDErr);
    }
  return(cstrErr);
  }

/********************************************************************
vInitialize - Initialize the hardware, set the frequency limit.

inputs: None.

return:  None.
********************************************************************/
void CDriver::vInitialize()
  {
    // Set up the frequency in the INI file before initiazing.
    theApp.vSet_ini_file("device.ini"); // Set the ini file.
    if(theApp.m_iFreqLimit == FREQ_LIMIT_15_CPM)
      AfxGetApp()->WriteProfileString(DEVICE_INI_SECTION, DEVICE_INI_ITEM_CPM,
      DEVICE_INI_VALUE_15_CPM);
    else if(theApp.m_iFreqLimit == FREQ_LIMIT_60_CPM)
      AfxGetApp()->WriteProfileString(DEVICE_INI_SECTION, DEVICE_INI_ITEM_CPM,
      DEVICE_INI_VALUE_60_CPM);
    else if(theApp.m_iFreqLimit == FREQ_LIMIT_200_CPM)
      AfxGetApp()->WriteProfileString(DEVICE_INI_SECTION, DEVICE_INI_ITEM_CPM,
      DEVICE_INI_VALUE_200_CPM);

    m_iHandle = Initialize(1, EGG_CONFIG);
  }


