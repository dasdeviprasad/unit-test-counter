/********************************************************************
TransferDB.h

Copyright (C) 2020 3CPM Company, Inc.  All rights reserved.

 header file for the CTransferDB class

 This class takes care of transferring the database, all the data files
 and validating the data base and all the data files.

HISTORY:
31-OCT-20  RET  New.  Version 1.0.0.1
********************************************************************/

#pragma once

class CTransferDB
{
public:
	CTransferDB(CSetupRemoteDBDlg *pdlgMain);
	~CTransferDB(void);

	CDBAccess *m_pDBA; // Pointer to CDBAccess object for the local database.
	CDBAccess *m_pRemDBA; // Pointer to CDBAccess object for the remote database.
	CString m_cstrRemoteDB; // Path and file name of the remote database.
	CString m_cstrRemoteDBPath; // Path of remote database.
	CString m_cstrLocalDBPath; // Path of local database.
  _TCHAR m_szCurDir[1024]; // Current directory
	CSetupRemoteDBDlg *m_pdlgMain; // Pointer to the main dialog box class.
	bool m_bErrFlag; // true indicates that there was an error in transferring/validating.
	CString m_cstrErrMsg; // Error message.
  bool m_bErrorBypassed; // true if the user wants to bypass the error.

	short int iTransfer();
  short int iCopy_study_data(long lStudyID, long lNewPatientID);
  BOOL bCopy_file(LPCTSTR strSrc, LPCTSTR strDest);
  short int iValidate();
  bool bCompare_patient_record(PATIENT_TABLE *pPatientTable, PATIENT_TABLE *pPatientTable2);
  bool bCompare_patient_info_record(PATIENT_INFO *pPatientInfo, PATIENT_INFO *pPatientInfo2);
  bool bCompare_studies(long lPatientID, long lPatientID2);
	bool bCompare_study(STUDY_DATA *psdStudy, STUDY_DATA *psdStudy2);
	bool bCompare_whatif(WHAT_IF *pwi, WHAT_IF *pwi2);
	bool bCompare_event(EVENTS_DATA *pedEvent, EVENTS_DATA *pedEvent2);
	bool bCompare_data_files(CString cstrDataFile, CString cstrDataFile2);
  BOOL CTransferDB::bCopy_files(LPCTSTR strSrcPath, LPCTSTR strDest, LPCTSTR strFileExt);
  bool bCompare_all_data_files();
	bool bDelete_files(CString cstrExt);

	void vReport_error(CString cstrErrMsg);
	int iGet_patient_count(int *piCount);

};
