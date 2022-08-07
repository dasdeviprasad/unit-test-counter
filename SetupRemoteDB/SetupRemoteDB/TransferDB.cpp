/********************************************************************
TransferDB.cpp

Copyright (C) 2020 3CPM Company, Inc.  All rights reserved.

  Perform the transfer process.
  - Add all the  records from the local database to the remote database.
	- Copies the data files to the remote database directory. (.egg and .csv)
	- Validates the database records and the data files.
	- Deletes the local database and the data files.

HISTORY:
31-OCT-20  RET  New.  Version 1.0.0.1
********************************************************************/
#include "StdAfx.h"
#include "SetupRemoteDB.h"
#include "SetupRemoteDBDlg.h"
#include "DBAccess.h"
#include "TransferDB.h"

/********************************************************************
Constructor - Initialize class variables.
              Connect to the local database.

  inputs: Pointer to the main dialog (the caller) for this application.

  return: None.
********************************************************************/
CTransferDB::CTransferDB(CSetupRemoteDBDlg *pdlgMain)
{
	CString cstrErr;

	m_pdlgMain = pdlgMain;
    // Get the current directory.
  GetCurrentDirectory(1024, m_szCurDir);

	m_bErrFlag = false;
	m_cstrErrMsg.Empty();
	m_bErrorBypassed = false;

    // Database access class, local 3CPM EGGSAS database.
  m_pDBA = new CDBAccess(LOCAL_DSN_DEFAULT);
  if(m_pDBA->iDBConnect() != SUCCESS)
		vReport_error((cstrErr = _T("Error connecting to the local database.")));
	else
	{
  	m_cstrLocalDBPath = m_pDBA->cstrGet_db_path();
		theApp.vLog(_T("Connected to the local database: %s"), m_pDBA->cstrGet_db_file_path());
	}
}

CTransferDB::~CTransferDB(void)
{
}

/********************************************************************
iTransfer - Transfer the database and all the data files to the remote database.
            Validate the database and the data files.

  inputs: None.

  return: None.
********************************************************************/
short int CTransferDB::iTransfer()
{
  short int iSts = SUCCESS;
  long lOrigPatientID;
  PATIENT_TABLE PatientTable;
  PATIENT_INFO PatientInfo;
  short int iPatientPos, iStudyPos, iPatientSts, iStudySts, iIndex;
  FIND_NEXT_STUDY fnsData;
	CString cstrDB, cstrErr;
	bool bContinue = true;

	if((iIndex = m_cstrRemoteDB.ReverseFind('.')) >= 0)
		m_cstrRemoteDB = m_cstrRemoteDB.Left(iIndex);	// Remove the file extension.
	cstrDB = m_cstrRemoteDBPath + '\\' + m_cstrRemoteDB;
	m_pRemDBA = new CDBAccess(cstrDB);
	if(m_pRemDBA->iDBConnect() == SUCCESS)
	{
		theApp.vLog(_T("Connected to the remote database: %s"), cstrDB);
		iPatientPos = FIRST_RECORD;
		while((iPatientSts = m_pDBA->iGet_next_patient(iPatientPos, &PatientTable)) == SUCCESS)
		{
			theApp.vLog(_T("Got local patient: patient ID = %lu"), PatientTable.lPatientID);
			m_pdlgMain->vUpdateProgress();
			if((iSts = m_pDBA->iGet_patient_info(PatientTable.lPatientID, &PatientInfo)) == SUCCESS)
			{
		  	theApp.vLog(_T("Got local patient info: patient ID = %lu"), PatientTable.lPatientID);
				lOrigPatientID = PatientTable.lPatientID;
				PatientTable.lPatientID = INV_LONG; // For add new patient.
				if((iSts = m_pRemDBA->iSave_patient(&PatientTable)) == SUCCESS)
				{
					theApp.vLog(_T("Save patient to remote DB: local patient ID = %lu, remote patient ID = %lu"), lOrigPatientID, PatientTable.lPatientID);
					if((iSts = m_pRemDBA->iSave_patient_info(PatientTable.lPatientID, &PatientInfo)) == SUCCESS)
					{
					  theApp.vLog(_T("Save patient info to remote DB: remote patient ID = %lu"), PatientTable.lPatientID);
						fnsData.lPatientID = lOrigPatientID;
            iStudyPos = FIRST_RECORD;
            while((iStudySts = m_pDBA->iFind_next_study(iStudyPos, &fnsData)) == SUCCESS)
						{
							iSts = iCopy_study_data(fnsData.lStudyID, PatientTable.lPatientID);	// Process the study and whatif tables.
							if(iSts == SUCCESS)
								iStudyPos = NEXT_RECORD;
							else
								break;
						}
					}
					else
						vReport_error((cstrErr = _T("Error saving patient information to remote database.")));
				}
				else
					vReport_error((cstrErr = _T("Error saving patient to remote database.")));
			}
			else
				vReport_error((cstrErr = _T("Error getting patient information.")));
			if(iSts == FAIL)
			{ // There were errors in exporting this study.  Ask if user wants to continue
				// with the export.
				CString cstrMsg, cstrTitle;

				cstrTitle.LoadStringW(IDS_ERROR);
				cstrMsg.LoadString(IDS_REMOTE_ERRS_CONTINUE);
				int iResp = AfxGetApp()->m_pActiveWnd->MessageBox(cstrMsg, cstrTitle, MB_YESNO);
				if(iResp == IDNO)
				{
					bContinue = false;
					break;
				}
				else
					m_bErrorBypassed = true;
			}
			iPatientPos = NEXT_RECORD;
		} // end of outer while, get next patient
		if(bContinue == true)
		{
			if(iPatientSts == FAIL)
				vReport_error((cstrErr = _T("Error getting patient from local database.")));
			// Now copy all the .egg and .csv files.
			theApp.vLog(_T("Copying *.egg files from %s to %s."), m_pDBA->cstrGet_db_path(), m_cstrRemoteDBPath);
			m_pdlgMain->vUpdateProgress();
			if(bCopy_files(m_pDBA->cstrGet_db_path(), m_cstrRemoteDBPath, _T("*.egg")) == TRUE)
			{
				// Copy any .csv files user may have made.
				theApp.vLog(_T("Copying *.csv files from %s to %s."), m_pDBA->cstrGet_db_path(), m_cstrRemoteDBPath);
				if(bCopy_files(m_pDBA->cstrGet_db_path(), m_cstrRemoteDBPath, _T("*.csv")) == TRUE)
				{
					// Now validate the copied database.
					// Read each record from local database and compare to remote database.
					// Read each data file from local database and compare to remote database.
					if((iSts = iValidate()) == SUCCESS)
					{
						m_pdlgMain->vUpdateProgress();
						delete m_pDBA;
						m_pDBA = NULL;
						delete m_pRemDBA;
						m_pRemDBA = NULL;
						// Delete all files from the C:\3CPM folder.
						bDelete_files(_T(".mdb"));
						bDelete_files(_T(".ldb"));
						bDelete_files(_T(".bak"));
						bDelete_files(_T(".egg"));
						bDelete_files(_T(".csv"));

					}
				}
			}
		}
	}
	else
		vReport_error((cstrErr = _T("Error connecting to the remote database.")));
  return(iSts);
}

/********************************************************************
iCopy_study_data - Copy the study record all the WhatID records and all the
                   Event records for the specified study from the local
									 database to the remote database.

  inputs: Study ID in the local database.
          Patient ID in the remote database.

  return: SUCCESS if study data, WhatIfs and the Event records are all processed.
          FAIL if there is an error.
********************************************************************/
short int CTransferDB::iCopy_study_data(long lStudyID, long lNewPatientID)
{
  STUDY_DATA sdStudy;
  EVENTS_DATA edEvent;
  long lNewStudyID;
  short int iPos, iSts;
  int iIndex;
  WHAT_IF wi;
  CString cstrDestFile, cstrErr;

  sdStudy.lStudyID = lStudyID;
    // Get the study data from the local database.
  if((iSts = m_pDBA->iGet_study_data(&sdStudy)) == SUCCESS)
  {
		theApp.vLog(_T("Got local study data for study ID: %lu"), sdStudy.lStudyID);
    sdStudy.lStudyID = INV_LONG;
    sdStudy.lPatientID = lNewPatientID;
      // Copy the data file.
      // Create the data file name for the destination.
    cstrDestFile = sdStudy.cstrDataFile;
    if((iIndex = cstrDestFile.ReverseFind('\\')) > 0)
      cstrDestFile = cstrDestFile.Right(cstrDestFile.GetLength() - iIndex - 1);
    cstrDestFile.Insert(0, '\\');
    cstrDestFile.Insert(0, m_cstrRemoteDBPath);
		// Save the study data in the export database.
		sdStudy.cstrDataFile = cstrDestFile;
		if((iSts = m_pRemDBA->iSave_study_data(&sdStudy)) == SUCCESS)
		{
			theApp.vLog(_T("Saved study data to remote DB: local study ID: %lu, remote study ID: "), lStudyID, sdStudy.lStudyID);
			lNewStudyID = sdStudy.lStudyID;
			// Now do the WhatIf table.
			iPos = FIRST_RECORD;
			iSts = SUCCESS;
			wi.lStudyID = lStudyID;
			while(iSts == SUCCESS)
			{  // Get the WhatIf data from one record in the local database.
				if((iSts = m_pDBA->iFind_next_whatif(iPos, &wi)) == SUCCESS)
				{
					theApp.vLog(_T("Found a WhatIf for local study ID: %lu, WhatIf ID: "), lStudyID, wi.lWhatIfID);
					wi.lStudyID = lNewStudyID;
					wi.lWhatIfID = INV_LONG;
					// Save the WhatIf data to the export database.
					if((iSts == m_pRemDBA->iSave_whatif_data(&wi)) == SUCCESS)
						theApp.vLog(_T("Saved a WhatIf to remote DB, local study ID: %lu, remote WhatIf ID: "), lStudyID, wi.lWhatIfID);
					else
					{
						cstrErr.Format(_T("Error saving WhatIf to remote database, remote WhatIf ID: "), wi.lWhatIfID);
						vReport_error(cstrErr);
					}
				}
				iPos = NEXT_RECORD;
			}
			if(iSts != FAIL)
			{
				// Now copy the events.
				iSts = SUCCESS;
				edEvent.lStudyID = lStudyID;
				while(iSts == SUCCESS)
				{  // Get an event from the local database.
					if((iSts = m_pDBA->iGet_event(&edEvent)) == SUCCESS)
					{
						theApp.vLog(_T("Found an event for local study ID: %lu, Event ID: "), lStudyID, edEvent.lStudyID);
						edEvent.lStudyID = lNewStudyID;
						// Save the Event in the export database.
						if((iSts == m_pRemDBA->iSave_event(&edEvent)) == SUCCESS)
							theApp.vLog(_T("Saved event to remote DB, local study ID: %lu, remote Event ID: "), lStudyID, edEvent.lStudyID);
						else
						{
							cstrErr.Format(_T("Error saving event to remote database, remote Event ID: "), edEvent.lStudyID);
							vReport_error(cstrErr);
						}
					}
					edEvent.lStudyID = INV_LONG; // Set up to get next event.
				}
			}
		}
		else
		{
	  	cstrErr.Format(_T("Error saving study to remote database, local Study ID: "), lStudyID);
			vReport_error(cstrErr);
		}
	}
	else if(iSts == FAIL)
	{
	  cstrErr.Format(_T("Error getting study from local database, Study ID: "), lStudyID);
		vReport_error(cstrErr);
	}
	if(iSts == SUCCESS_NO_DATA)
		iSts = SUCCESS;
	return(iSts);
}

/********************************************************************
bCopy_files - Copy the all files from the local database to the
              remote database.

  inputs: Source path to copy from.
          Destination path to copy to.
					File extension which specifies the type of file to copy.

  return: TRUE if all files for the extension were copied.
          FALSE if there is an error.
********************************************************************/
BOOL CTransferDB::bCopy_files(LPCTSTR strSrcPath, LPCTSTR strDest, LPCTSTR strFileExt)
{
  BOOL bRet;
	HANDLE hSearch;
	WIN32_FIND_DATA File;
	CString cstrSearch, cstrSrc, cstrDest, cstrErr;

	cstrSearch.Format(_T("%s\\%s"), strSrcPath, strFileExt);
	cstrSrc = strSrcPath + '\\';
	hSearch = FindFirstFile(cstrSearch, &File);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		do
		{
			cstrSrc.Format(_T("%s\\%s"), strSrcPath, File.cFileName);
			cstrDest.Format(_T("%s\\%s"), strDest, File.cFileName);
			theApp.vLog(_T("    Copying file: %s"), File.cFileName);
			bRet = bCopy_file(cstrSrc, cstrDest);
		} while (FindNextFile(hSearch, &File) && bRet == TRUE);

		FindClose(hSearch);
		if(bRet == FALSE)
		{
			cstrErr.Format(_T("Error copying %s files"), strFileExt);
			vReport_error(cstrErr);
		}
	}
	else
	{
		bRet = TRUE;
		theApp.vLog(_T("There are no %s files to copy."), strFileExt);
	}

  return(bRet);
}

/********************************************************************
bCopy_file - Copy a file from the source to the destination.

  inputs: String containing the source file and path.
          String containing the destination file and path.

  return: TRUE if the file was successfully copied.
          FALSE if there was an error.
********************************************************************/
BOOL CTransferDB::bCopy_file(LPCTSTR strSrc, LPCTSTR strDest)
{
  BOOL bRet;

  if((bRet = CopyFile(strSrc, strDest, FALSE)) == FALSE)
  { // Failed copying file.
    DWORD dwErr;
    wchar_t szMsg[256];
    CString cstrErr, cstrTitle;
  
    dwErr = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErr, 0L, szMsg, 256, NULL);
    cstrErr.Format(IDS_FAIL_COPY_FILE, strSrc, strDest);
    cstrErr += _T("\r\n");
    cstrErr += szMsg;
    cstrTitle.LoadStringW(IDS_ERROR);
    AfxGetApp()->m_pActiveWnd->MessageBox((LPCTSTR)cstrErr, cstrTitle);
    theApp.vLog((LPCTSTR)cstrErr);
  }
	else
  	theApp.vLog(_T("        Copied file from %s to %s: "), strSrc, strDest);
    // This function changes the current directory, set it back.
  SetCurrentDirectory(m_szCurDir);
  return(bRet);
}

/********************************************************************
iValidate - Validate the remote database records with the local database records.
            Validate the remote data files with the local data files.

  inputs: None.

  return: SUCCESS if all records and data files validated.
          FAIL if there is an error.
********************************************************************/
short int CTransferDB::iValidate()
{
	bool bSts;
	short int iSts = SUCCESS;
  PATIENT_TABLE PatientTable, PatientTable2;
  PATIENT_INFO PatientInfo, PatientInfo2;
  short int iPatientPos, iPatientSts;
  FIND_NEXT_STUDY fnsData;
	CString cstrDB, cstrErr;

	theApp.vLog(_T("Validating"));
	iPatientPos = FIRST_RECORD;
	while((iPatientSts = m_pDBA->iGet_next_patient(iPatientPos, &PatientTable)) == SUCCESS)
	{
		theApp.vLog(_T("Got patient from local database, patient ID: %lu"), PatientTable.lPatientID);
		m_pdlgMain->vUpdateProgress();
		PatientTable2.lPatientID = INV_LONG;
		PatientTable2.cstrFirstName = PatientTable.cstrFirstName;
		PatientTable2.cstrLastName = PatientTable.cstrLastName;
		PatientTable2.cstrMI = PatientTable.cstrMI;
		PatientTable2.cstrResearchRefNum = PatientTable.cstrResearchRefNum;
		if((iSts = m_pRemDBA->iGet_patient(&PatientTable2)) == SUCCESS)
		{
			theApp.vLog(_T("Got patient from remote database, patient ID: %lu"), PatientTable2.lPatientID);
			theApp.vLog(_T("Comparing Patient records, local patient ID: %lu, remote patient ID: %lu"), PatientTable.lPatientID, PatientTable2.lPatientID);
			if((bSts = bCompare_patient_record(&PatientTable, &PatientTable2)) == false)
				vReport_error((cstrErr = _T("Error comparing PATIENT table.")));
			else
			{
				if((iSts = m_pDBA->iGet_patient_info(PatientTable.lPatientID, &PatientInfo)) == SUCCESS)
				{
					theApp.vLog(_T("Got patient info from local database, patient ID: %lu"), PatientTable.lPatientID);
					if((iSts = m_pRemDBA->iGet_patient_info(PatientTable2.lPatientID, &PatientInfo2)) == SUCCESS)
					{
						theApp.vLog(_T("Got patient info from remote database, patient ID: %lu"), PatientTable2.lPatientID);
      			theApp.vLog(_T("Comparing Patient info records, local patient ID: %lu, remote patient ID: %lu"), PatientTable.lPatientID, PatientTable2.lPatientID);
						if((bSts = bCompare_patient_info_record(&PatientInfo, &PatientInfo2)) == false)
							vReport_error((cstrErr = _T("Error comparing PATIENT_INFO table.")));
						else
						{
							if((bSts = bCompare_studies(PatientTable.lPatientID, PatientTable2.lPatientID)) == false)
								vReport_error((cstrErr = _T("Error comparing patient studies.")));
						}
					}
					else
						vReport_error((cstrErr = _T("Error reading remote PATIENT_INFO table.")));
				}
				else
					vReport_error((cstrErr = _T("Error reading local PATIENT_INFO table.")));
			}
		}
		else
			vReport_error((cstrErr = _T("Error reading remote PATIENT table.")));
		iPatientPos = NEXT_RECORD;
		if(bSts == false || iSts == FAIL)
			break;
	}

  if(bSts == true && iSts != FAIL)
	  bSts = bCompare_all_data_files(); // Validate the data files.
	if(bSts == false)
		iSts = FAIL;
	return(iSts);
}

/********************************************************************
bCompare_patient_record - Compare two patient records.

  inputs: PATIENT_TABLE containing one record.
	        PATIENT_TABLE containing a second record.

  return: true if both records are the same.
          FAIL if the records are not the same.
********************************************************************/
bool CTransferDB::bCompare_patient_record(PATIENT_TABLE *pPatientTable, PATIENT_TABLE *pPatientTable2)
{
	bool bRet = false;

	if(pPatientTable->bHide == pPatientTable2->bHide
		&& pPatientTable->bUseResrchRefNum == pPatientTable2->bUseResrchRefNum
		&& pPatientTable->cstrFirstName == pPatientTable2->cstrFirstName
		&& pPatientTable->cstrLastName == pPatientTable2->cstrLastName
		&& pPatientTable->cstrMI == pPatientTable2->cstrMI
		&& pPatientTable->cstrResearchRefNum == pPatientTable2->cstrResearchRefNum)
		  bRet = true;
	return(bRet);
}

/********************************************************************
bCompare_patient_info_record - Compare two patient information records.

  inputs: PATIENT_INFO containing one record.
	        PATIENT_INFO containing a second record.

  return: true if both records are the same.
          FAIL if the records are not the same.
********************************************************************/
bool CTransferDB::bCompare_patient_info_record(PATIENT_INFO *pPatientInfo, PATIENT_INFO *pPatientInfo2)
{
	bool bRet = false;

	if(pPatientInfo->bFemale == pPatientInfo2->bFemale
		&& pPatientInfo->bMale == pPatientInfo2->bMale
		&& pPatientInfo->cstrAddress1 == pPatientInfo2->cstrAddress1
		&& pPatientInfo->cstrAddress2 == pPatientInfo2->cstrAddress2
		&& pPatientInfo->cstrCity == pPatientInfo2->cstrCity
		&& pPatientInfo->cstrDateOfBirth == pPatientInfo2->cstrDateOfBirth
		&& pPatientInfo->cstrHospitalNumber == pPatientInfo2->cstrHospitalNumber
		&& pPatientInfo->cstrPhone == pPatientInfo2->cstrPhone
		&& pPatientInfo->cstrSocialSecurityNum == pPatientInfo2->cstrSocialSecurityNum
		&& pPatientInfo->cstrState == pPatientInfo2->cstrState
		&& pPatientInfo->cstrZipCode == pPatientInfo2->cstrZipCode)
		  bRet = true;
	return(bRet);
}

/********************************************************************
bCompare_studies - Compare two sets of study records specified by patient IDs.
                   Comparisons are for the study records, the WhatIf records
									 and the Event records.

  inputs: Patient ID 1
	        Patient ID 2

  return: true if all records are the same.
          FAIL if the records are not the same.
********************************************************************/
bool CTransferDB::bCompare_studies(long lPatientID, long lPatientID2)
{
	bool bRet = true;
  FIND_NEXT_STUDY fnsData, fnsData2;
	STUDY_DATA sdStudy, sdStudy2;
  WHAT_IF wi, wi2;
  EVENTS_DATA edEvent, edEvent2;
	short int iStudyPos, iStudySts, iSts = 0, iPos;
	CString cstrDataFile, cstrErr;
	int iIndex;

	theApp.vLog(_T("Comparing studies for local patient ID: %lu and remote patient ID: %lu"), lPatientID, lPatientID2);
	fnsData.lPatientID = lPatientID;
	iStudyPos = FIRST_RECORD;
	while((iStudySts = m_pDBA->iFind_next_study(iStudyPos, &fnsData)) == SUCCESS)
	{
		theApp.vLog(_T("Found local study to compare, study ID: %lu"), fnsData.lStudyID);
		sdStudy.lStudyID = fnsData.lStudyID;
		if((iSts = m_pDBA->iGet_study_data(&sdStudy)) == SUCCESS)
		{
	  	theApp.vLog(_T("Got local study data to compare, study ID: %lu"), fnsData.lStudyID);
			// Get study data using the data file name.
			// Use the file name from the local database and the path name user entered.
			cstrDataFile = m_cstrRemoteDBPath + '\\';
			if((iIndex = fnsData.cstrFileName.ReverseFind('\\')) >= 0)
				cstrDataFile += fnsData.cstrFileName.Right(fnsData.cstrFileName.GetLength() - iIndex - 1);
			else
				cstrDataFile += fnsData.cstrFileName;

			sdStudy2.lStudyID = INV_LONG;
			sdStudy2.cstrDataFile = cstrDataFile;
			if((iStudySts = m_pRemDBA->iGet_study_data(&sdStudy2)) == SUCCESS)
			{
	    	theApp.vLog(_T("Got remote study data to compare, study ID: %lu"), sdStudy2.lStudyID);
	    	theApp.vLog(_T("Comparing study data, local study ID: %lu, remote study ID: %lu"), sdStudy.lStudyID, sdStudy2.lStudyID);
				if((bRet = bCompare_study(&sdStudy, &sdStudy2)) == true)
				{
					// Now validate the WhatIf table.
					iPos = FIRST_RECORD;
					iSts = SUCCESS;
					wi.lStudyID = fnsData.lStudyID;
					while(iSts == SUCCESS)
					{  // Get the WhatIf data from one record in the local database.
						if((iSts = m_pDBA->iFind_next_whatif(iPos, &wi)) == SUCCESS)
						{
							theApp.vLog(_T("Found a local WhatIf, WhatIf ID: %lu"), wi.lWhatIfID);
							wi2.lStudyID = INV_LONG;
							wi2.cstrDateTimeSaved = wi.cstrDateTimeSaved;
							wi2.cstrDescription = wi.cstrDescription;
							if((iSts = m_pRemDBA->iFind_next_whatif(iPos, &wi2)) == SUCCESS)
							{
								theApp.vLog(_T("Found a remote WhatIf, WhatIf ID: %lu"), wi.lWhatIfID);
								theApp.vLog(_T("Comparing WhatIfs, local WhatIf ID: %lu, remote WhatIf ID: %lu"), wi.lWhatIfID, wi2.lWhatIfID);
								if((bRet = bCompare_whatif(&wi, &wi2)) == false)
								{
									vReport_error((cstrErr = _T("Error reading comparing whatif records.")));
									break;
								}
							}
						}
						iPos = NEXT_RECORD;
					}
					if(iSts == SUCCESS_NO_DATA)
						iSts = SUCCESS;
					// Now validate all the events for this study.
					if(iSts == SUCCESS)
					{
						edEvent.lStudyID = sdStudy.lStudyID;
						edEvent.lSeconds = INV_LONG;
						while(iSts == SUCCESS)
						{  // Get an event from the local database.
							if((iSts = m_pDBA->iGet_event(&edEvent)) == SUCCESS)
							{
								theApp.vLog(_T("Found a local event, Event ID: %lu"), edEvent.lEventID);
								edEvent2.lStudyID = sdStudy2.lStudyID;
								edEvent2.lSeconds = edEvent.lSeconds;
								if((iSts = m_pRemDBA->iGet_event(&edEvent2)) == SUCCESS)
								{
									theApp.vLog(_T("Found the remote event, Event ID: %lu"), edEvent2.lEventID);
									theApp.vLog(_T("Comparing events, local Event ID: %lu, remote Event ID: %lu"), edEvent.lEventID, edEvent2.lEventID);
									if((bRet = bCompare_event(&edEvent, &edEvent2)) == false)
									{
										vReport_error((cstrErr = _T("Error reading comparing event records.")));
										break;
									}
								}
							}
							edEvent.lStudyID = INV_LONG; // Set up to get next event.
						}
					}
				}
				else
					vReport_error((cstrErr = _T("Error comparing studies.")));
			}
	  	else
  			vReport_error((cstrErr = _T("Error getting study data from remote database.")));
		}
		if(iSts == SUCCESS)
			iStudyPos = NEXT_RECORD;
		else
			break;
	} // End of while find next study

	return(bRet);
}

/********************************************************************
bCompare_study - Compare two study records.

  inputs: STUDY_DATA record 1
	        STUDY_DATA record 2

  return: true if both records are the same.
          FAIL if the records are not the same.
********************************************************************/
bool CTransferDB::bCompare_study(STUDY_DATA *psdStudy, STUDY_DATA *psdStudy2)
{
	bool bRet = false;
	CString cstrFile, cstrFile2, cstrErr, cstrMsg;
	CFile cfDataFile; // Pointer to the opened data file.
	CFileException FileEx;

	if(psdStudy->bAnorexia == psdStudy2->bAnorexia
		&& psdStudy->bBloating == psdStudy2->bBloating
		&& psdStudy->bDelayedGastricEmptying == psdStudy2->bDelayedGastricEmptying
		&& psdStudy->bDyspepsia == psdStudy2->bDyspepsia
		&& psdStudy->bDyspepsiaBloatSatiety == psdStudy2->bDyspepsiaBloatSatiety
		&& psdStudy->bEarlySateity == psdStudy2->bEarlySateity
		&& psdStudy->bEructus == psdStudy2->bEructus
		&& psdStudy->bEvalEffectMed == psdStudy2->bEvalEffectMed
		&& psdStudy->bExtra10Min == psdStudy2->bExtra10Min
		&& psdStudy->bGastroparesis == psdStudy2->bGastroparesis
		&& psdStudy->bGerd == psdStudy2->bGerd
		&& psdStudy->bManualMode == psdStudy2->bManualMode
		&& psdStudy->bNausea == psdStudy2->bNausea
		&& psdStudy->bPause == psdStudy2->bPause
		&& psdStudy->bPostDilationEval == psdStudy2->bPostDilationEval
		&& psdStudy->bPostPrandialFullness == psdStudy2->bPostPrandialFullness
		&& psdStudy->bRefractoryGERD == psdStudy2->bRefractoryGERD
		&& psdStudy->bUserDiagnosis == psdStudy2->bUserDiagnosis
		&& psdStudy->bVomiting == psdStudy2->bVomiting
		&& psdStudy->bWeightLoss == psdStudy2->bWeightLoss
		&& psdStudy->cstrAttendingPhysician == psdStudy2->cstrAttendingPhysician
		&& psdStudy->cstrDateOfStudy == psdStudy2->cstrDateOfStudy
		&& psdStudy->cstrDiagnosis == psdStudy2->cstrDiagnosis
		&& psdStudy->cstrDiagnosisKeyPhrase == psdStudy2->cstrDiagnosisKeyPhrase
		&& psdStudy->cstrEndMinutes == psdStudy2->cstrEndMinutes
		&& psdStudy->cstrICDCode == psdStudy2->cstrICDCode
		&& psdStudy->cstrIFPComments == psdStudy2->cstrIFPComments
		&& psdStudy->cstrMealTime == psdStudy2->cstrMealTime
		&& psdStudy->cstrMedications == psdStudy2->cstrMedications
		&& psdStudy->cstrPeriodLen == psdStudy2->cstrPeriodLen
		&& psdStudy->cstrPeriodStart == psdStudy2->cstrPeriodStart
		&& psdStudy->cstrRecommendationKeyPhrase == psdStudy2->cstrRecommendationKeyPhrase
		&& psdStudy->cstrRecommendations == psdStudy2->cstrRecommendations
		&& psdStudy->cstrReferingPhysician == psdStudy2->cstrReferingPhysician
		&& psdStudy->cstrRespirationRate == psdStudy2->cstrRespirationRate
		&& psdStudy->cstrStartMinutes == psdStudy2->cstrStartMinutes
		&& psdStudy->cstrStimulationMedium == psdStudy2->cstrStimulationMedium
		&& psdStudy->cstrSuggestedDiagnosis == psdStudy2->cstrSuggestedDiagnosis
		&& psdStudy->cstrTechnician == psdStudy2->cstrTechnician
		&& psdStudy->cstrWaterAmountUnits == psdStudy2->cstrWaterAmountUnits
		&& psdStudy->lTestMaxSignal == psdStudy2->lTestMaxSignal
		&& psdStudy->lTestMinSignal == psdStudy2->lTestMinSignal
		&& psdStudy->uFrequency == psdStudy2->uFrequency
		&& psdStudy->uiBLDataPoints == psdStudy2->uiBLDataPoints
		&& psdStudy->uiPostStimDataPoints == psdStudy2->uiPostStimDataPoints
		&& psdStudy->uWaterAmount == psdStudy2->uWaterAmount)
	{
		// compare just the file names, not the path because the path will be different.
		char szFileName[1024], szExt[20], szFileName2[1024], szExt2[20];
		CT2A strDataFile(psdStudy->cstrDataFile);
		_splitpath(strDataFile.m_psz, NULL, NULL, szFileName, szExt);
		CT2A strDataFile2(psdStudy2->cstrDataFile);
		_splitpath(strDataFile2.m_psz, NULL, NULL, szFileName2, szExt2);
		if(strcmp(szFileName, szFileName2) == 0 && strcmp(szExt, szExt2) == 0)
		  bRet = true;
	}
	if(bRet == true)
	{
		// Now check out that the data files can be opened.  They will be verified later.
		if(cfDataFile.Open(psdStudy->cstrDataFile, CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary, &FileEx) == FALSE)
		{
			bRet = false;
			cstrMsg.LoadString(IDS_ERR_FILE_OPEN);
			cstrErr.Format(cstrMsg, psdStudy->cstrDataFile);
			vReport_error(cstrErr);
		}
		else
			cfDataFile.Close();
		if(cfDataFile.Open(psdStudy2->cstrDataFile, CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary, &FileEx) == FALSE)
		{
			bRet = false;
			cstrMsg.LoadString(IDS_ERR_FILE_OPEN);
			cstrErr.Format(cstrMsg, psdStudy2->cstrDataFile);
			vReport_error(cstrErr);
		}
		else
			cfDataFile.Close();
	}

	return(bRet);
}

/********************************************************************
bCompare_whatif - Compare two WhatIf records.

  inputs: WHAT_IF record 1
	        WHAT_IF record 2

  return: true if both records are the same.
          FAIL if the records are not the same.
********************************************************************/
bool CTransferDB::bCompare_whatif(WHAT_IF *pwi, WHAT_IF *pwi2)
{
	bool bRet = false;

	if(pwi->cstrDateTimeSaved == pwi2->cstrDateTimeSaved
		&& pwi->cstrDescription == pwi2->cstrDescription
		&& pwi->cstrEndMinutes == pwi2->cstrEndMinutes
		&& pwi->cstrPeriodLen == pwi2->cstrPeriodLen
		&& pwi->cstrPeriodStart == pwi2->cstrPeriodStart
		&& pwi->cstrRespirationRate == pwi2->cstrRespirationRate
		&& pwi->cstrStartMinutes == pwi2->cstrStartMinutes
		&& pwi->uFrequency == pwi2->uFrequency
		&& pwi->uNumPeriods == pwi2->uNumPeriods)
		  bRet = true;
	return(bRet);
}

/********************************************************************
bCompare_event - Compare two Event records.

  inputs: EVENTS_DATA record 1
	        EVENTS_DATA record 2

  return: true if both records are the same.
          FAIL if the records are not the same.
********************************************************************/
bool CTransferDB::bCompare_event(EVENTS_DATA *pedEvent, EVENTS_DATA *pedEvent2)
{
	bool bRet = false;

	if(pedEvent->cstrDescription == pedEvent->cstrDescription
		&& pedEvent->cstrType == pedEvent->cstrType
		&& pedEvent->iPeriodType == pedEvent->iPeriodType
		&& pedEvent->lDataPoint == pedEvent->lDataPoint
		&& pedEvent->lSeconds == pedEvent->lSeconds)
		  bRet = true;
	return(bRet);
}

/********************************************************************
bCompare_all_data_files - Compare all data files between the remote and
                          local databases.

  inputs: None.

  return: true if all data files match.
********************************************************************/
bool CTransferDB::bCompare_all_data_files()
{
  bool bRet = true, bErr = false;
	HANDLE hSearch;
	WIN32_FIND_DATA File;
	CString cstrSearch;
	CString cstrSrc, cstrDest;
	int iFileType = 0;

  while(true)
	{
		if(iFileType == 0)
  		cstrSearch.Format(_T("%s\\*.egg"), m_pDBA->cstrGet_db_path());
		else if(iFileType == 1)
  		cstrSearch.Format(_T("%s\\*.csv"), m_pDBA->cstrGet_db_path());
		else
			break;
		hSearch = FindFirstFile(cstrSearch, &File);
		if (hSearch != INVALID_HANDLE_VALUE)
		{
			do
			{
				cstrSrc.Format(_T("%s\\%s"), m_pDBA->cstrGet_db_path(), File.cFileName);
				cstrDest.Format(_T("%s\\%s"), m_cstrRemoteDBPath, File.cFileName);
				theApp.vLog(_T("Compare data files: %s, %s"), cstrSrc, cstrDest);
				bErr = bCompare_data_files(cstrSrc, cstrDest);
			} while (FindNextFile(hSearch, &File) && bErr == true);

			FindClose(hSearch);
			if(bErr == false)
				bRet = false;
		}
		++iFileType;
	}

  return(bRet);
}

/********************************************************************
bCompare_data_files - Compare two data files.

  inputs: CString object containing one data file.
	        CString object containing a second data file.

  return: true if both data files match.
********************************************************************/
bool CTransferDB::bCompare_data_files(CString cstrDataFile, CString cstrDataFile2)
{
	bool bRet = true;
	CFile cfDataFile, cfDataFile2; // Pointer to the opened data files.
	CString cstrErr, cstrMsg;
	CFileException FileEx;
	unsigned char ucData[1000], ucData2[1000];
	int iNumBytes, iNumBytes2, iCnt, iErrLocation;

	try
	{ // Open the file.
		iErrLocation = 0;
		theApp.vLog(_T("Opening data file for comparison: %s"), cstrDataFile);
		if(cfDataFile.Open(cstrDataFile, CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary, &FileEx) == FALSE)
		{
			bRet = false;
			cstrMsg.LoadString(IDS_ERR_FILE_OPEN);
			cstrErr.Format(cstrMsg, cstrDataFile);
			vReport_error(cstrErr);
		}
		else
		{ // Open the second file.
			iErrLocation = 1;
			theApp.vLog(_T("Opening data file for comparison: %s"), cstrDataFile2);
			if(cfDataFile2.Open(cstrDataFile2, CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary, &FileEx) == FALSE)
			{
				bRet = false;
		  	cstrMsg.LoadString(IDS_ERR_FILE_OPEN);
				cstrErr.Format(cstrMsg, cstrDataFile2);
				vReport_error(cstrErr);
			}
			else
			{
				// Compare the 2 files.
				while(bRet == true)
				{
					if((iNumBytes = cfDataFile.Read(ucData, sizeof(ucData))) == 0)
						break;
					if((iNumBytes2 = cfDataFile2.Read(ucData2, sizeof(ucData2))) == 0)
						break;
					if(iNumBytes != iNumBytes2)
					{
						bRet = false;
						break;
					}
					for(iCnt = 0; iCnt < iNumBytes; ++iCnt)
					{
						if(ucData[iCnt] != ucData2[iCnt])
						{
							bRet = false;
							break;
						}
					}
				}
  			cfDataFile2.Close();
			}
			cfDataFile.Close();
		}
	}
	catch(CFileException *pfe)
	{ // Got a file open error.
		cstrMsg.LoadString(IDS_ERR_FILE_OPEN2);
		if(iErrLocation == 0)
  		cstrErr.Format(cstrMsg, cstrDataFile, strerror(pfe->m_lOsError));
		else
  		cstrErr.Format(cstrMsg, cstrDataFile2, strerror(pfe->m_lOsError));
		vReport_error(cstrErr);
		pfe->Delete();
  	bRet = false;
	}

	if(bRet == false)
	{
		cstrErr.Format(_T("Error comparing data files: %s, %s"), cstrDataFile, cstrDataFile2);
		vReport_error(cstrErr);
	}
	return(bRet);
}

/********************************************************************
vReport_error - Show a message box containing the error.  Also write
                the error to the log file.

  inputs: CString object containing the error message.

  return: None.
********************************************************************/
void CTransferDB::vReport_error(CString cstrErrMsg)
{
	CString cstrErr;

	m_bErrFlag = true;
	m_cstrErrMsg = cstrErrMsg;
	cstrErr.LoadString(IDS_ERROR);
	theApp.m_pActiveWnd->MessageBox(cstrErrMsg, cstrErr, MB_OK);
	cstrErrMsg.Insert(0, _T("ERROR: "));
	theApp.vLog(cstrErrMsg);
}

/********************************************************************
iGet_patient_count - Get the number of Patient records.

  inputs: pointer to in integer that get the number of records.

  return: SUCCESS if all records and data files validated.
	        SUCCESS_NO_DATA if there are no patient records.
          FAIL if there is an error.
********************************************************************/
int CTransferDB::iGet_patient_count(int *piCount)
{
	int iCount = 0, iSts;
	CString cstrErr;

  if((iSts = m_pDBA->iGet_patient_count(piCount)) == FAIL)
	{
		*piCount  = 0;
    vReport_error((cstrErr = _T("Error getting patient record count.")));
	}
	else if(iSts == SUCCESS_NO_DATA)
		*piCount  = 0;
	return(iSts);
}

/********************************************************************
bDelete_files - Delete the database and data files from the c:\3cpm folder.
                Any files that can't be deleted are written to the log file
								along with why it couldn't be deleted.

  inputs: CString object containing the file type (file extension) to delete.

  return: true.
********************************************************************/
bool CTransferDB::bDelete_files(CString cstrExt)
{
	bool bRet = true;
	HANDLE hSearch;
	WIN32_FIND_DATA File;
	CString cstrSearch, cstrFile;

	cstrSearch.Format(_T("%s\\*%s"), m_cstrLocalDBPath, cstrExt);
	hSearch = FindFirstFile(cstrSearch, &File);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		do
		{
			cstrFile.Format(_T("%s\\%s"), m_cstrLocalDBPath, File.cFileName);
			if(DeleteFile(cstrFile) == FALSE)
			{
        wchar_t szMsg[256];
				CString cstrErr, cstrTitle;
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0L, szMsg, 256, NULL);
				cstrErr.Format(_T("Error deleting file: %s\n%s"), cstrFile, szMsg);
				vReport_error(cstrErr);
			}
		} while (FindNextFile(hSearch, &File));

		FindClose(hSearch);
	}
	return(bRet);
}