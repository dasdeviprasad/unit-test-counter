// Copyright (C) 2004, 3CPM Company, Inc.  All rights reserved.

#define BATT_IMP_CONFIG 1
#define EGG_CONFIG 2

float m_fPosImpedanceLimit = 200.0F; // Limit for positive electrode impedance.
float m_fNegImpedanceLimit = 200.0F; // Limit for negative electrode impedance.
double m_dData[342]; // Buffer to hold EGG data.

/********************************************************************
iCheck_battery_impedance - Check the battery voltage level and the
                           electrode impedance.

  inputs: Indicator of what to check.
            CHK_BATTERY: check the battery.
            CHK_IMPEDANCE_POS: check the positive impedance
            CHK_IMPEDANCE_NEG: Check the negative impedance.
            These values may be ORed together.
  
  return: SUCCESS if everything that's checked is OK.
          FAIL if there is an error reading the hardware.
          IDS_BAD_BATTERY if the battery is too low.
          IDS_LOW_BATTERY if the battery is low but useable.
          IDS_BAD_POS_IMPEDANCE if the positive impedance is bad indicating
            a bad connection.
          IDS_BAD_NEG_IMPEDANCE if the negative impedance is bad indicating
            a bad connection.
********************************************************************/
short int CDriver::iCheck_battery_impedance(unsigned uCheck)
  {
  CString cstrMsg;
  int iNumPts;
  short int iRet = SUCCESS;

    // First assume that the EGG configuration is active, so close it.
  if(m_iHandle >= 0)
    Close(m_iHandle);
    // Now open the battery and impedance configuration.
  m_iHandle = Initialize(1, BATT_IMP_CONFIG);
  if(m_iHandle < 0)
    {
    m_cstrErr.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
    iRet = FAIL;
    }
  else
    {
    if(m_iDoElectrodeTest != 0)
      { // Do electrode tests.
      if(iRet >= SUCCESS)
        { // Battery check passed or didn't do it.
        m_cstrErr.Empty();
        if((uCheck & CHK_IMPEDANCE_NEG) != 0)
          { // Check the impedance of the negative input.
          do
            {
	          iNumPts = Read_Buffer(m_iHandle, 2, m_dData, 300);
            } while(iNumPts == 0);
          if(iNumPts < 0)
            {
            m_cstrErr = Error_Message(-iNumPts); // Error in reading.
            iRet = FAIL;
            }
          else
            {
            --iNumPts; // Convert to an index so we can look at the last point.
            if(m_dData[iNumPts] > (double)m_fNegImpedanceLimit)
              {
              iRet = FAIL_BAD_NEG_IMPEDANCE;
              m_cstrErr.LoadString(IDS_BAD_NEG_IMPEDANCE);
              }
            }
          } // End of check negative impedance.
        if((uCheck & CHK_IMPEDANCE_POS) != 0)
          { // Check the impedance of the positive input.
          do
            {
	          iNumPts = Read_Buffer(m_iHandle, 1, m_dData, 300);
            } while(iNumPts == 0);
          if(iNumPts < 0)
            {
            m_cstrErr = Error_Message(-iNumPts); // Error in reading.
            iRet = FAIL;
            }
          else
            {
            --iNumPts; // Convert to an index so we can look at the last point.
            if(m_dData[iNumPts] > (double)m_fPosImpedanceLimit)
              {
              iRet = FAIL_BAD_POS_IMPEDANCE;
              cstrMsg.LoadString(IDS_BAD_POS_IMPEDANCE);
              if(m_cstrErr.IsEmpty() == FALSE)
                m_cstrErr += "\r\n";
              m_cstrErr += cstrMsg;
              }
            }
          }
        }
      }
    }
    // Restore the EGG handle.
  if(m_iHandle >= 0)
    Close(m_iHandle);
  m_iHandle = Initialize(1, EGG_CONFIG);
  if(m_iHandle < 0)
    {
    m_cstrErr.Format(IDS_ERR_DLL, DLL_NAME, Error_Message(-m_iHandle));
    iRet = FAIL;
    }

  return(iRet);
  }

