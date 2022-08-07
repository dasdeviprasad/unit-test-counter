/********************************************************************
sysdefs.h

Copyright (C) 2005, 3CPM Company, Inc.  All rights reserved.

  Header file for overall system defines.

HISTORY:
28-FEB-05  RET  New.
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
#define SUCCESS_WITH_INFO 4
#define SUCCESS_USER 1000 // First SUCCESS value that that another object
                          // can use.
#define FAIL_USER -1000 // First FALI value that that another object can use.


