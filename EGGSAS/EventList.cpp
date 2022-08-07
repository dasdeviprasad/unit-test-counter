/********************************************************************
EventList.cpp

Copyright (C) 2007,2008,2012, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CEventList class.

  Displays a dialog box with the list of events for the entire study.
  Each period is labeled with a title line in the listbox.
  There is a field for viewing the description of an event when the user
    clicks on the event in the listbox.
  For viewing only.

HISTORY:
08-AUG-07  RET  New.
19-FEB-08  RET  Version 2.04
                  When selecting an event, if a title line is selected
                    (Baseline, or Post-stimulation period x), clear the
                    description field.
17-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "EventList.h"
#include ".\eventlist.h"


// CEventList dialog

IMPLEMENT_DYNAMIC(CEventList, CDialog)
/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the active document that started this dialog box.
********************************************************************/
CEventList::CEventList(CEGGSASDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CEventList::IDD, pParent)
  , m_cstrDescription(_T(""))
  {

  m_pDoc = pDoc;
  }

/********************************************************************
Destructor
********************************************************************/
CEventList::~CEventList()
{
}

void CEventList::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Control(pDX, IDC_LIST_EVENTS, m_cEventList);
DDX_Text(pDX, IDC_EDIT_EVENT_DESCRIPTION, m_cstrDescription);
}


BEGIN_MESSAGE_MAP(CEventList, CDialog)
  ON_LBN_SELCHANGE(IDC_LIST_EVENTS, OnLbnSelchangeListEvents)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////
//// CEventList message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Set up initial values for various fields in the dialog box.
    Fill in the Events listbox.
********************************************************************/
BOOL CEventList::OnInitDialog()
  {
  CString cstrLine, cstrBaseTitle;
  CEvnt *pEvent;
  int iIndex, iLBIndex, iLBEventIndex, iStartSecond, iEndSecond;
  unsigned uIndex;

  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  vSet_text();
    // Build the listbox.
    // The data item for each listbox line is the index into the events array.
    //   Title lines have an index of -1.
    // First events are the baseline events.
  //cstrLine.LoadString(IDS_BASELINE);
  cstrLine = g_pLang->cstrLoad_string(ITEM_G_BASELINE);
  iLBIndex = m_cEventList.AddString(cstrLine);
  m_cEventList.SetItemData(iLBIndex, -1);
  if(m_pDoc->m_pdPatient.Events.m_BLEvents.GetCount() > 0)
    {
    for(iIndex = 0; iIndex < m_pDoc->m_pdPatient.Events.m_BLEvents.GetCount(); ++iIndex)
      {
      pEvent = m_pDoc->m_pdPatient.Events.m_BLEvents[iIndex];
      iLBIndex = m_cEventList.AddString(cstrFmt_event_line(pEvent));
      m_cEventList.SetItemData(iLBIndex, iIndex);
      }
    iLBEventIndex = iIndex;
    }
  else
    iLBEventIndex = 0;
    // Add post-stimulation periods.  Label each period on a separate line.
  //cstrBaseTitle.LoadString(IDS_POST_STIM_PERIOD);
  cstrBaseTitle = g_pLang->cstrLoad_string(ITEM_G_POST_STIM_PERIOD);
  if(m_pDoc->m_pdPatient.Events.m_PSEvents.GetCount() > 0)
    {
    for(uIndex = 0; uIndex < m_pDoc->m_pdPatient.uNumPeriods; ++uIndex)
      {
      cstrLine.Format("%s %d", cstrBaseTitle, uIndex + 1);
      iLBIndex = m_cEventList.AddString(cstrLine);
      m_cEventList.SetItemData(iLBIndex, -1);
      iStartSecond = (int)(m_pDoc->m_pdPatient.fPeriodStart[uIndex + 1] * (float)60.0);
      iEndSecond = iStartSecond
                   + (int)(m_pDoc->m_pdPatient.fPeriodLen[uIndex + 1] * (float)60.0);
      for(iIndex = 0; iIndex < m_pDoc->m_pdPatient.Events.m_PSEvents.GetCount(); ++iIndex)
        {
        pEvent = m_pDoc->m_pdPatient.Events.m_PSEvents[iIndex];
        if(pEvent->m_iSeconds >= iStartSecond && pEvent->m_iSeconds <= iEndSecond)
          {
          iLBIndex = m_cEventList.AddString(cstrFmt_event_line(pEvent));
          m_cEventList.SetItemData(iLBIndex, iLBEventIndex++);
          }
        }
      }
    }
  if(iLBEventIndex > 0)
    {
    m_cEventList.SetCurSel(1);
    if(m_pDoc->m_pdPatient.Events.m_BLEvents.GetCount() > 0)
      pEvent = m_pDoc->m_pdPatient.Events.m_BLEvents[0];
    else // if(m_pDoc->m_pdPatient.Events.m_PSEvents.GetCount() > 0)
      pEvent = m_pDoc->m_pdPatient.Events.m_PSEvents[0];
    m_cstrDescription = pEvent->m_cstrDescription;
    }
  else
    m_cstrDescription.Empty();
  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnLbnSelchangeListEvents

  Event handler for changing the selection in the event listbox.
  For Research Version.
    Show the event description in the description edit box.
********************************************************************/
void CEventList::OnLbnSelchangeListEvents()
  {
  CEvnt *pEvent;
  int iSel, iEventIndex, iLastBaselineIndex;

  UpdateData(TRUE);
  iSel = m_cEventList.GetCurSel();
  iEventIndex = (int)m_cEventList.GetItemData(iSel);
  if(iEventIndex >= 0)
    {
    iLastBaselineIndex = m_pDoc->m_pdPatient.Events.m_BLEvents.GetCount() - 1;
    if(iEventIndex <= iLastBaselineIndex)
      { // Baseline.
      pEvent = m_pDoc->m_pdPatient.Events.m_BLEvents[iEventIndex];
      }
    else
      { // Post-stimulation period
      pEvent = m_pDoc->m_pdPatient.Events.m_PSEvents[iEventIndex - iLastBaselineIndex - 1];
      }
    m_cstrDescription = pEvent->m_cstrDescription;
    }
  else
    m_cstrDescription = ""; // User selected a title line.
  UpdateData(FALSE);
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
cstrFmt_event_line - Format a line of Event data for the listbox.
                     Format:
                       - 4 leading spaces
                       - Minutes:Seconds
                       - dash
                       - Indications.

  inputs: Pointer to a CEvnt object for the event.
  
  return: CString object containing for formatted line.
********************************************************************/
CString CEventList::cstrFmt_event_line(CEvnt *pEvent)
  {
  CString cstrLine;
  int iMinutes;

  iMinutes = pEvent->m_iSeconds / 60;
  cstrLine.Format("    %d:%02d - %s", iMinutes, pEvent->m_iSeconds - (iMinutes * 60),
                  pEvent->m_cstrIndications);
  return(cstrLine);
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CEventList::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_EVENT_LIST);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_EV_EVENTS));

    GetDlgItem(IDC_STATIC_DESCRIPTION)->SetWindowText(g_pLang->cstrGet_text(ITEM_EV_DESCRIPTION));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    }
  }
