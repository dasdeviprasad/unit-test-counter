/********************************************************************
Events.h

Copyright (C) 2007, 3CPM Company, Inc.  All rights reserved.

  Header file for the Events class.

HISTORY:
06-AUG-07  RET  New.
********************************************************************/
#pragma once
#include "afx.h"

#define EVENT_BASELINE 1
#define EVENT_POST_STIM 2

class CEvnt :
  public CObject
  {
  public:
    CEvnt(void);
    virtual ~CEvnt(void);

  public:
    int m_iDataPointIndex; // Index of data point where the event ocurred.
    int m_iSeconds; // Time in seconds of the event.
    short int m_iPeriodType; // Baseline(1) or Post-stimulation (2)
    CString m_cstrIndications; // List of items from the dialog checkbox.
    CString m_cstrDescription; // Description of Event.
  };

/////////////////////////////////////////////////////////////////////
//// Array of events.

typedef CTypedPtrArray<CObArray,CEvnt*> CEvntArray;

class CEvents
  {
  public:
    CEvents(void);
    virtual ~CEvents(void);

	CEvntArray m_BLEvents;  // Baseline events.
	CEvntArray m_PSEvents;  // Post-stimulation events.

  };

