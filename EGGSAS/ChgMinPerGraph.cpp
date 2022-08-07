/********************************************************************
ChgMinPerGraph.cpp

Copyright (C) 2007,2012, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CChgMinPerGraph class.

  Research version.
  This class allows for changing the number of minutes displayed
  on each graph in the Ten minute view.

HISTORY:
30-JUN-07  RET  New.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/

#include "stdafx.h"
#include "EGGSAS.h"
#include "EGGSASDoc.h"
#include "ChgMinPerGraph.h"
#include ".\chgminpergraph.h"


// CChgMinPerGraph dialog

IMPLEMENT_DYNAMIC(CChgMinPerGraph, CDialog)

/********************************************************************
Constructor

  Initialize class data.

  inputs: Pointer to the active document that started this dialog box.
          Pointer to the window that started this dialog box.
********************************************************************/
CChgMinPerGraph::CChgMinPerGraph(CEGGSASDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CChgMinPerGraph::IDD, pParent)
  , m_iMinPerGraph(0)
  {

  m_pDoc = pDoc;
  }

/********************************************************************
Destructor

********************************************************************/
CChgMinPerGraph::~CChgMinPerGraph()
{
}

void CChgMinPerGraph::DoDataExchange(CDataExchange* pDX)
{
CDialog::DoDataExchange(pDX);
DDX_Text(pDX, IDC_EDIT_MIN_PER_GRAPH, m_iMinPerGraph);
}


BEGIN_MESSAGE_MAP(CChgMinPerGraph, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChgMinPerGraph message handlers

/********************************************************************
OnInitDialog

  Initialize the dialog box.
    Set up initial values for various fields in the dialog box.
********************************************************************/
BOOL CChgMinPerGraph::OnInitDialog()
  {
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  vSet_text();
   // Get the number of minutes per graph from the INI file.
  //m_iMinPerGraph = AfxGetApp()->GetProfileInt(INI_SEC_GENERAL,
  //                 INI_ITEM_MIN_PER_GRAPH, DEFAULT_MIN_PER_GRAPH);
  m_iMinPerGraph = g_pConfig->iGet_item(CFG_MINUTESPERGRAPH);

  UpdateData(FALSE);
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
  }

/********************************************************************
OnBnClickedOk

  Message handler for the OK button.
    Get the minutes entered by the user and update the INI file.
********************************************************************/
void CChgMinPerGraph::OnBnClickedOk()
  {

  UpdateData(TRUE);
  //AfxGetApp()->WriteProfileInt(INI_SEC_GENERAL, INI_ITEM_MIN_PER_GRAPH, m_iMinPerGraph);
  g_pConfig->vWrite_item(CFG_MINUTESPERGRAPH, m_iMinPerGraph);

  OnOK();
  }

/////////////////////////////////////////////////////////////////////
//// Support methods

/********************************************************************
iGet_min_per_graph - Get the number of minutes per graph.

inputs: None.
  
Output: None.

return: Number of minutes per graph.
********************************************************************/
int CChgMinPerGraph::iGet_min_per_graph(void)
  {
  return m_iMinPerGraph;
  }

/********************************************************************
vSet_text - Set the text in the dialog box fields.

  inputs: None.

  return: None.
********************************************************************/
void CChgMinPerGraph::vSet_text(void)
  {

  if(g_pLang != NULL)
    {
    g_pLang->vStart_section(SEC_CHG_MIN_PER_GRAPH);
    this->SetWindowText(g_pLang->cstrGet_text(ITEM_MPG_CHG_MIN_PER_GRAPH));

    GetDlgItem(IDC_STATIC_INSTRUCTIONS)->SetWindowText(g_pLang->cstrGet_text(ITEM_MPG_INSTRUCTIONS));

    GetDlgItem(IDOK)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_OK));
    GetDlgItem(IDCANCEL)->SetWindowText(g_pLang->cstrLoad_string(ITEM_G_CANCEL));
    //g_pLang->vEnd_section();
    }
  }
