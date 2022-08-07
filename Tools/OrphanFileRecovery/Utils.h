/********************************************************************
Utils.h

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Header for CUtils Class.

HISTORY:
25-JUN-10  RET  New.
********************************************************************/

#pragma once

class CUtils
  {
  public:
    CUtils(void);
    virtual ~CUtils(void);

  float fRound(float fNum, int iNumDec);
  float fRound_down_to_half_min(float fStartValue);
  };
