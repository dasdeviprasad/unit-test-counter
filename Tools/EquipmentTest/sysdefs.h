/********************************************************************
sysdefs.h

Copyright (C) 2003,2004, 3CPM Company, Inc.  All rights reserved.

  Header file for overall system defines.

HISTORY:
03-OCT-03  RET  New.
********************************************************************/

#define INV_FLOAT FLT_MAX
#define INV_LONG 0xFFFFFFFF //ULONG_MAX
#define INV_SHORT USHRT_MAX

// Return values from functions or other types of status.
//   More specific failures should be less than 0.
//   More specific successes should be greater than 1.
#define FAIL 0
#define SUCCESS 1
#define SUCCESS_NO_ACTION 2
#define SUCCESS_NO_DATA 3
#define SUCCESS_USER 1000 // First SUCCESS value that that another object
                          // can use.
  // Success codes for battery and impedance checking.
#define SUCCESS_LOW_BATTERY SUCCESS_USER  // Battery is low.

#define FAIL_USER -1000 // First FAIL value that that another object can use.
  // Fail codes for battery and impedance checking.
#define FAIL_BAD_BATTERY FAIL_USER    // Battery is bad.
#define FAIL_BAD_POS_IMPEDANCE (FAIL_USER - 1) // Bad positive impedance.
#define FAIL_BAD_NEG_IMPEDANCE (FAIL_USER - 2) // Bad negative impedance.
#define FAIL_TIMEOUT (FAIL_USER - 3) // Didn't get all the readings needed
                                     // for battery voltage or impedance in
                                     // 2.5 seconds.
  // Fail codes for reading data.
#define FAIL_EGG_DRIVER (FAIL_USER - 4) // EGG driver failed getting data points.
//#define FAIL_EGG_DRIVER_NO_EGG (FAIL_USER - 5) // EGG driver failed getting EGG data points.
//#define FAIL_EGG_DRIVER_NO_RESP (FAIL_USER - 6) // EGG driver failed getting Respiration data points.
//#define FAIL_EGG_DRIVER_LOW_BATT (FAIL_USER - 5) // EGG driver failed because of the
//                                                 // low battery warning.

#define CR 0xD // Carriage return


