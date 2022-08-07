/********************************************************************
Utils.cpp

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Class contains various utility functions.

HISTORY:
25-JUN-10  RET  New.
********************************************************************/

#include "StdAfx.h"
#include "math.h"
#include "Utils.h"

/********************************************************************
Constructor

********************************************************************/
CUtils::CUtils(void)
  {

  }

/********************************************************************
Destructor

********************************************************************/
CUtils::~CUtils(void)
  {
  }

/****************************************************************************
fRound - Round a floating point number.

    inputs: Floating point number.
            Number of decimal places to round the number to.

    return: The rounded number.
****************************************************************************/
float CUtils::fRound(float fNum, int iNumDec)
    {
    float fFract, fInteg;

    if(iNumDec > 0)
      {
      fNum *= (float)pow(10, iNumDec);
      fFract = modf(fNum, &fInteg);
      if(fFract >= .5)
      ++fInteg;
      else if(fFract <= -.5 && fFract < 0)
        --fInteg;
      fInteg /= (float)pow(10, iNumDec);
      }
    else
      {
      fFract = modf(fNum, &fInteg);
      if(fFract >= .5)
      ++fInteg;
      else if(fFract <= -.5 && fFract < 0)
        --fInteg;
      }
    return(fInteg);
    }

/********************************************************************
fRound_down_to_half_min - Round a value down to the next half minute.
                          The input is never negative.

    inputs: Value to be rounded down.

    return: Rounded down value.
********************************************************************/
float CUtils::fRound_down_to_half_min(float fStartValue)
  {
  float fEndValue, fFraction;

    // Drop any fractional values.
  fEndValue = (float)((int)fStartValue);
    // Look at fractional part.
  fFraction = fStartValue - fEndValue;
  if(fFraction >= (float)0.5)
    fEndValue += (float)0.5;
  return(fEndValue);
  }
