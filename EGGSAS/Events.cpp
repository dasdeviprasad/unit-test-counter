/********************************************************************
Events.cpp

Copyright (C) 2007, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CEvents and CEvnt classes.

  CEvnt is an object containing information for one event.

  CEvents holds 2 arrays of CEvnt objects.


HISTORY:
06-AUG-07  RET  New.
********************************************************************/

#include "stdafx.h"
#include "Events.h"

/********************************************************************
Constructor

  Initialize class data.

  inputs: None.
********************************************************************/
CEvnt::CEvnt(void)
  {

  m_iDataPointIndex = 0;
  m_iSeconds = 0;
  m_cstrIndications.Empty();
  m_cstrDescription.Empty();
  }

/********************************************************************
Destructor

********************************************************************/
CEvnt::~CEvnt(void)
  {
  }

/////////////////////////////////////////////////////////////////////

/********************************************************************
Constructor

  Initialize class data.

  inputs: None.
********************************************************************/
CEvents::CEvents(void)
  {
  }

/********************************************************************
Destructor

********************************************************************/
CEvents::~CEvents(void)
  {
  }

