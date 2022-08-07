/********************************************************************
ChgMinPerGraph.h

Copyright (C) 2007,2012, 3CPM Company, Inc.  All rights reserved.

  Header file for the CChgMinPerGraph class.

  For the Research version.

HISTORY:
30-JUN-07  RET  New.
16-FEB-12  RET  Version 
                  Changes for foreign languages.
********************************************************************/
#pragma once


// CChgMinPerGraph dialog

class CChgMinPerGraph : public CDialog
{
	DECLARE_DYNAMIC(CChgMinPerGraph)

public:
	CChgMinPerGraph(CEGGSASDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CChgMinPerGraph();

public: // Data
  CEGGSASDoc *m_pDoc; // Pointer to the document.

  void vSet_text(void);

// Dialog Data
	enum { IDD = IDD_CHG_MIN_PER_GRAPH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
  int m_iMinPerGraph;
  virtual BOOL OnInitDialog();
  afx_msg void OnBnClickedOk();
  int iGet_min_per_graph(void);
};
