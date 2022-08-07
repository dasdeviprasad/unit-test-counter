/********************************************************************
Language.h

Copyright (C) 2012,2013,2016,2020 3CPM Company, Inc.  All rights reserved.

  Header file for the CLanguage class.

  This contains INI section and item defines and data used by the class..

HISTORY:
23-FEB-12  RET  New
22-AUG-12  RET
             Add language items: ITEM_G_INC_IN_RPT_DESC, ITEM_CR_FIRST_PAGE,
               ITEM_CR_OTHER_PAGE
24-AUG-12  RET
             Add language items: ITEM_D_ADD_TO_REPORT, ITEM_D_SAVE_DESC_TO_LIST,
               ITEM_D_REMOVE_DESC_FROM_LIST, ITEM_D_INCLUDE_ICD_CODE,
               ITEM_D_ADD_ICD_TO_LIST, ITEM_D_REMOVE_ICD_FROM_LIST,
               ITEM_D_USE_ADDITIONAL_FEATURES, ITEM_FI_LOGO_REQ,
               ITEM_FI_LOGO_REQ2, ITEM_FI_LOGO_REQ3, ITEM_FI_LOGO_REQ4,
               ITEM_G_EGG_RPT_TITLE, ITEM_G_AMTWATERINGESTED
31-AUG-12  RET
             Add section item: SEC_RECOMMENDATIONS
             Add language items: ITEM_D_ERASE_DESC, ITEM_D_ENTER_DIAG_KEY_PHRASE,
               ITEM_D_ADD_DIAG_DESC, ITEM_D_ENTER_ICD_CODE, ITEM_D_DEL_ICD_CODE_LINE,
               ITEM_REC_ADD_TO_REPORT, ITEM_REC_SAVE_RECOMMENDATION_TO_LIST,
               ITEM_REC_REMOVE_FROM_LIST
02-SEP-12  RET
             Add language items: ITEM_REC_ADD_TO_REPORT,
               ITEM_REC_SAVE_RECOMMENDATION_TO_LIST, ITEM_REC_REMOVE_FROM_LIST,
               ITEM_REC_ENTER_RECOMM_KEY_PHRASE, ITEM_REC_ADD_RECOMM_DESC,
               ITEM_REC_ERASE_RECOMM_DESC, ITEM_G_ICDCODE
13-SEP-12  RET
             Add language items to SEC_PRESTUDY_INFO section:
               ITEM_PSI_GASTROPARESIS, ITEM_PSI_DYSPEPSIA_BLOAT_SATIETY,
               ITEM_PSI_DYSPEPSIA, ITEM_PSI_ANOREXIA, ITEM_PSI_REFRACTORY_GERD,
               ITEM_PSI_WEIGHTLOSS, ITEM_PSI_POST_DILATION_EVAL,
               ITEM_PSI_EVAL_EFFECT_MED
29-SEP-12  RET
05-OCT-12  RET
             Change for 60/200 CPM
               Add section item: SEC_FREQUENCYRANGE
               Add language items: ITEM_G_CPM, ITEM_FR_TITLE, ITEM_FR_SELECT
               Add menu item: ITEM_M_FREQUENCY_RANGE
13-DEC-12  RET
             Change to allow summary graphs to be drawn for research version.
               Add language items: ITEM_G_GOTO_WHATIF1, ITEM_G_GOTO_WHATIF2
14-DEC-12  RET
             Change to filter out frequencies below 10 cpm in the RSA.
               Add language items: SEC_FILTERRSA, ITEM_FRSA_TITLE,
                 ITEM_FRSA_FILTER_RSA_CHKBOX, ITEM_TT_FILTERRSA,
                 ITEM_CR_INCLUDE_BOTH_RSA
               Add class variable: cstrFilterRSA
25-FEB-13  RET
             Add language items: ITEM_DG_MALE, ITEM_DG_FEMALE, ITEM_DG_HIDE,
               ITEM_M_VIEW_HIDDEN_PATIENTS
13-MAR-13  RET
             Add Select study type and frequency range dialog box.
               Add section: SEC_STUDY_TYPE_FREQ_RANGE
               Add section items: ITEM_STFR_TITLE, ITEM_STFR_LABEL,
                 ITEM_STFR_STD_0_15_CHKBOX, ITEM_STFR_RSCH_0_15_CHKBOX,
                 ITEM_STFR_RSCH_0_60_CHKBOX, ITEM_STFR_RSCH_0_200_CHKBOX
               Add item to general section: ITEM_G_SEL_STUDY_TYPE_FREQ_RANGE
06-JUN-16  RET
             Add text to the language file for the Hide/show study dates dialog box.
               Add section: SEC_HIDE_STUDY_DATE
               Add section items: ITEM_HIDE_STUDY_DATES_TITLE, ITEM_ENTER_PSWD,
                 ITEM_CREATE_NEW_PSWD, ITEM_RESET_PSWD, ITEM_ENTER_NEW_PSWD, ITEM_NEXT,
                 ITEM_REENTER_PSWD, ITEM_CURRENTLY_HIDDEN, ITEM_CURRENTLY_HIDDEN1,
                 ITEM_CURRENTLY_NOT_HIDDEN,ITEM_CURRENTLY_NOT_HIDDEN1, ITEM_PSWD_NOT_CORRECT,
                 ITEM_ENTER_PSWD_CANCEL, ITEM_MUST_ENTER_PSWD, ITEM_PSWD_ENTRIES_DONT_MATCH,
                 ITEM_PLEASE_ENTER_PSWD,ITEM_PLEASE_ENTER_PSWD1, ITEM_ENTER_3CPM_PSWD,
                 ITEM_ENTER_PSWD_VALIDATE, ITEM_PSWD_OUT_OF_DATE, ITEM_INITIAL_INSTRUCTION,
                 ITEM_INITIAL_INSTRUCTION1
20-JAN-18  RET
             Add text to the language file for the Search database dialog box.
               Add section: SEC_SEARCH
               Add section items: ITEM_SEARCH_TITLE, ITEM_LAST_NAME_RSRCH_REF_NUM,
                 ITEM_LAST_NAME, ITEM_RSRCH_REFNUM, ITEM_ASC_ORDER, ITEM_DESC_ORDER,
                 ITEM_NAME_HELP, ITEM_STUDY_DATE, ITEM_DATE_OLDER, ITEM_DATE_NEWER,
                 ITEM_DATE_EQUAL, ITEM_DATE_HELP, ITEM_MONTH_HELP, ITEM_DAY_HELP,
                 ITEM_YEAR_HELP
14-NOV-20  RET
             Add ability to work with a remote database.
             Changed settings to allow the user to specify the remote database.
               Add section items: ITEM_SET_SEL_REMOTE_DB_PATH, ITEM_SET_ENTER_REMOTE_DB_PASSWD
********************************************************************/

#pragma once

//// INI section defines
#define SEC_MENU "MENU"
#define SEC_TOOLTIPS "TOOLTIPS"
#define SEC_SELECTPATIENT "SELECTPATIENT"
#define SEC_GENERAL "GENERAL"
#define SEC_DEMOGRAPHICS "DEMOGRAPHICS"
#define SEC_PRESTUDY_INFO "PRESTUDY_INFO"
#define SEC_EQUIPMENT_TEST "EQUIPMENT_TEST"
#define SEC_BACKUP_RESTORE "BACKUP_RESTORE"
#define SEC_IMPORT_REMOTE_DB "IMPORT_REMOTE_DB"
#define SEC_CHG_MIN_PER_GRAPH "CHG_MIN_PER_GRAPH"
#define SEC_CUSTOM_REPORT "CUSTOM_REPORT"
#define SEC_DIAGNOSIS "DIAGNOSIS"
#define SEC_DIAGNOSTIC_HISTORY "DIAGNOSTIC_HISTORY"
#define SEC_RESPIRATION "RESPIRATION"
#define SEC_EVENT_LIST "EVENT_LIST"
#define SEC_EVENT_RECORDER "EVENT_RECORDER"
#define SEC_FACILITY_INFO "FACILITY_INFO"
#define SEC_FINISH_STUDY "FINISH_STUDY"
#define SEC_MEDICATIONS "MEDICATIONS"
#define SEC_POST_WATER_TEST "POST_WATER_TEST"
#define SEC_STUDY_TYPE "STUDY_TYPE"
#define SEC_STIMULATION_MEDIUM "STIMULATION_MEDIUM"
#define SEC_WHAT_IF "WHAT_IF"
#define SEC_BASELINE "BASELINE"
#define SEC_SETTINGS "SETTINGS"
#define SEC_LANGUAGE "LANGUAGE"
#define SEC_RECOMMENDATIONS "RECOMMENDATIONS"
#define SEC_FREQUENCYRANGE "FREQUENCYRANGE"
#define SEC_FILTERRSA "FILTER_RSA"
#define SEC_STUDY_TYPE_FREQ_RANGE "STUDY_TYPE_FREQ_RANGE"
#define SEC_HIDE_STUDY_DATE "HIDE_STUDY_DATE"
#define SEC_SEARCH "SEARCH"
#define SEC_IMPORT_DUP_NAME "IMPORT_DUP_NAME"
//#define SEC_ ""
//// End of INI section defines

//// INI item defines
//// Menus
// File menu
#define ITEM_M_FILE "File"
#define ITEM_M_NEW_STUDY "NewStudy"
#define ITEM_M_CTRLN "CtrlN"
#define ITEM_M_NEW_PATIENT "NewPatient"
#define ITEM_M_EXISTING_PATIENT "ExistingPatient"
#define ITEM_M_OPEN_PRIOR "OpenPrior"
#define ITEM_M_CTRLO "CtrlO"
#define ITEM_M_SAVE_PATIENT "SavePatient"
#define ITEM_M_CTRLS "CtrlS"
#define ITEM_M_SAVE_RAW_DATA "SaveRawData"
#define ITEM_M_BACKUP "Backup"
#define ITEM_M_EXPORT_STUDIES "ExportStudies"
#define ITEM_M_IMPORT_STUDIES "ImportStudies"
#define ITEM_M_PRINT "Print"
#define ITEM_M_CTRLP "CtrlP"
#define ITEM_M_PRINT_PREVIEW "PrintPreview"
#define ITEM_M_PRINT_SETUP "PrintSetup"
#define ITEM_M_REVERT_TO_ORGINAL "RevertToOrginal"
#define ITEM_M_RECENT_FILE "RecentFile"
#define ITEM_M_EXIT "Exit"
#define ITEM_M_RESUME_STUDY "ResumeStudy"

// Edit menu
#define ITEM_M_EDIT "Edit"
#define ITEM_M_DEMOGRAPHICS "Demographics"
#define ITEM_M_DIAG_HIST "DiagHist"
#define ITEM_M_MEDICATIONS "Medications"
#define ITEM_M_PRESTUDY_INFO "PreStudyInfo"
#define ITEM_M_RESPIRATION "Respiration"
#define ITEM_M_AMOUNT_WATER "AmountWater"
#define ITEM_M_SUGGESTED_DIAG "SuggestedDiag"
#define ITEM_M_RECOMMENDATIONS "Recommendations"
#define ITEM_M_STIM_MEDIUM_QTY "StimMediumQty"
#define ITEM_M_WHAT_IF "WhatIf"

// View menu (some items in the View menu are the same as the Edit menu.)
#define ITEM_M_VIEW "View"
#define ITEM_M_BASELINE "Baseline"
#define ITEM_M_FIRST_PERIOD "FirstPeriod"
#define ITEM_M_SECOND_PERIOD "SecondPeriod"
#define ITEM_M_THIRD_PERIOD "ThirdPeriod"
#define ITEM_M_RSA "RSA"
#define ITEM_M_SUMMARY_GRAPHS "SummaryGraphs"
#define ITEM_M_DATA_SHEET "DataSheet"
#define ITEM_M_EVENTS "Events"
#define ITEM_M_CHG_MIN_PER_GRPH "ChgMinPerGrph"
#define ITEM_M_VIEW_HIDDEN_PATIENTS "ViewHiddenPatients"

// Go to menu
#define ITEM_M_GOTO "Goto"
#define ITEM_M_START_OF_STUDY "StartOfStudy"
#define ITEM_M_PREV_10_MIN_PD "Prev10MinPd"
#define ITEM_M_CURRENT_PD "CurrentPd"
#define ITEM_M_NEXT_10_MIN_PD "Next10MinPd"
#define ITEM_M_END_OF_STUDY "EndOfStudy"
#define ITEM_M_PREV_PERIOD "PrevPeriod"
#define ITEM_M_NXT_PERIOD "NxtPeriod"

// Analyze menu
#define ITEM_M_ANALYZE "Analyze"
#define ITEM_M_ANALYZE_DIAGNOSIS "AnalyzeDiagnosis"

// Report menu
#define ITEM_M_REPORT "Report"
#define ITEM_M_VIEW_REPORT "ViewReport"
#define ITEM_M_PRINT_REPORT "PrintReport"
#define ITEM_M_CONFIG_REPORT "ConfigReport"
#define ITEM_M_FACILITY_INFO "FacilityInfo"
#define ITEM_M_CREATE_SPREAD_SHEET_OF_DATA_SHEET "CreateSpreadSheetOfDataSheet"
#define ITEM_M_SHOW_HIDE_STUDY_DATES "ShowHideStudyDates"

// Help menu
#define ITEM_M_HELP "Help"
#define ITEM_M_HELP_TOPICS "HelpTopics"
#define ITEM_M_ABOUT_EGGSAS "AboutEGGSAS"
#define ITEM_M_ABOUT_READER "AboutReader"
#define ITEM_M_ABOUT_CENTRAL_DATA_­DEPOT "AboutCentralDataDepot"
// Settings menu
#define ITEM_M_SETTINGS "Settings"
#define ITEM_M_FREQUENCY_RANGE "FrequencyRange"
//#define ITEM_M_ ""
//#define ITEM_M_ ""
//#define ITEM_M_ ""

////Tool tips
#define ITEM_TT_NEW_STUDY "NewStudy"
#define ITEM_TT_OPEN_PRIOR_STUDY "OpenPriorStudy"
#define ITEM_TT_SAVE_PATIENT "SavePatient"
#define ITEM_TT_PRINT_REPORT "PrintReport"
#define ITEM_TT_START_OF_STUDY "StartOfStudy"
#define ITEM_TT_PREV_10_MIN_PD "Prev10MinPd"
#define ITEM_TT_PREV_PD "PrevPd"
#define ITEM_TT_CURRENT_PERIOD "CurrentPd"
#define ITEM_TT_NEXT_10_MIN_PD "Next10MinPd"
#define ITEM_TT_NEXT_PD "NextPd"
#define ITEM_TT_END_OF_STUDY "EndOfStudy"
#define ITEM_TT_RSA "RSA"
#define ITEM_TT_SUMMARY_GRAPHS "SummaryGraphs"
#define ITEM_TT_DATA_SHEET "DataSheet"
#define ITEM_TT_DIAGNOSIS "Diagnosis"
#define ITEM_TT_RECOMMENDATIONS "Recommendations"
#define ITEM_TT_WHAT_IF_SCENARIO "WhatIfScenario"
#define ITEM_TT_ABOUT "About"
#define ITEM_TT_HELP_TOPICS "HelpTopics"
#define ITEM_TT_FILTERRSA "FilterRSA"

// Select Patient Dialog box
#define ITEM_SP_SEL_PATIENT_TITLE "SelPatientTitle"
#define ITEM_SP_CURRENT_DB "CurrentDB"
#define ITEM_SP_SEL_LOCAL_DB "SelLocalDB"
#define ITEM_SP_SEL_READER_DB "SelReaderDB"
#define ITEM_SP_SEL_PATIENT_DB "SelPatientDB"
#define ITEM_SP_IMP_PATIENT_FILE "ImpPatientFile"
//#define ITEM_SP_ ""

// DEMOGRAPHICS section
#define ITEM_DG_PATIENT_DEMOGRAPHICS "PatientDemographics"
#define ITEM_DG_MIDDLE_INITIAL "MiddleInitial"
#define ITEM_DG_LAST_NAME "LastName"
#define ITEM_DG_DATE_OF_BIRTH "DateOfBirth"
#define ITEM_DG_DATEFMT "DateFmt"
#define ITEM_DG_SSN "SSN"
#define ITEM_DG_ADDRESS "Address"
#define ITEM_DG_CITY "City"
#define ITEM_DG_STATE "State"
#define ITEM_DG_ZIPCODE "ZipCode"
#define ITEM_DG_PHONE_NUM "PhoneNum"
#define ITEM_DG_HOSPITAL_NUM "HospitalNum"
#define ITEM_DG_RESEARCH_REF_NUM "ResearchRefNum"
#define ITEM_DG_HUMAN "Human"
#define ITEM_DG_DOG "Dog"
#define ITEM_DG_CONTINUE "Continue"
#define ITEM_DG_MALE "Male"
#define ITEM_DG_FEMALE "Female"
#define ITEM_DG_HIDE "Hide"
//#define ITEM_DG_ ""

// PreStudy Information
#define ITEM_PSI_PRESTUDY_INFO "PreStudyInfo"
#define ITEM_PSI_MEAL_TIME "MealTime"
#define ITEM_PSI_INDIC_FOR_PROC "IndicForProc"
#define ITEM_PSI_NAUSEA "Nausea"
#define ITEM_PSI_VOMITING "Vomiting"
//#define ITEM_PSI_BLOATING "Bloating"
//#define ITEM_PSI_GERD "GERD"
//#define ITEM_PSI_ERUCTUS "Eructus"
//#define ITEM_PSI_EARLY_SATIETY "EarlySatiety"
//#define ITEM_PSI_POSTPRANDIAL_FULL "PostPrandialFull"
//#define ITEM_PSI_DELAYED_GASTRIC_EMPTY "DelayedGastricEmpty"
#define ITEM_PSI_ADD_INDICATIONS "AddIndications"
#define ITEM_PSI_ATTEND_PHYS "AttendPhys"
#define ITEM_PSI_REFER_PHYS "ReferPhys"
#define ITEM_PSI_TECHNICIAN "Technician"
#define ITEM_PSI_BEGIN_STUDY "BeginStudy"
#define ITEM_PSI_GASTROPARESIS "Gastroparesis"
#define ITEM_PSI_DYSPEPSIA_BLOAT_SATIETY "DyspepsiaBloatSatiety"
#define ITEM_PSI_DYSPEPSIA "Dyspepsia"
#define ITEM_PSI_ANOREXIA "Anorexia"
#define ITEM_PSI_REFRACTORY_GERD "RefractoryGERD"
#define ITEM_PSI_WEIGHTLOSS "WeightLoss"
#define ITEM_PSI_POST_DILATION_EVAL "PostDilationEval"
#define ITEM_PSI_EVAL_EFFECT_MED "EvalEffectMed"
//#define ITEM_PSI_ ""

// Equipment test section
#define ITEM_ET_EQUIP_TEST "EquipTest"
#define ITEM_ET_START_EQUIP_TEST "StartEquipTest"
#define ITEM_ET_RESP_SENSOR "RespSensor"
#define ITEM_ET_GASTRIC_ELECT "GastricElect"
#define ITEM_ET_BEGIN_BL "BeginBL"
#define ITEM_ET_CHK_POS_IMPED "ChkPosImped"
#define ITEM_ET_CHK_NEG_IMPED "ChkNegImped"
#define ITEM_ET_CHK_BATT_VOLT "ChkBattVolt"
//#define ITEM_ET_ ""

// Backup/Restore section
#define ITEM_BR_IMPORT_REMOTE_DB "ImportRemoteDB"
#define ITEM_BR_BACKUP_RESTORE_DATA "BackupRestoreData"
#define ITEM_BR_BACKUP "Backup"
#define ITEM_BR_RESTORE "Restore"
#define ITEM_BR_BACKUP_TO_RESTORE_FROM "BackupToRestoreFrom"
#define ITEM_BR_START "Start"
#define ITEM_BR_FILE_BEING_COPIED "FileBeingCopied"

// Import Remote DB dialog box
#define ITEM_IDB_IMPORT_DB "ImportDB"
#define ITEM_IDB_SELECT_DB "SelectDB"
#define ITEM_IDB_SEL_ALL_STUDIES "SelAllStudies"
#define ITEM_IDB_IMPORT "Import"
//#define ITEM_IDB_ ""

// Change minutes per graph dialog box
#define ITEM_MPG_CHG_MIN_PER_GRAPH "ChgMinPerGraph"
#define ITEM_MPG_INSTRUCTIONS "Instructions"
//#define ITEM_MPG_ ""

// Configure report dialog box
#define ITEM_CR_CONFIG_REPORT "ConfigReport"
#define ITEM_CR_CORE_REPORT "CoreReport"
#define ITEM_CR_INSTRUCTIONS "Instructions"
#define ITEM_CR_PATIENT_INFO "PatientInfo"
#define ITEM_CR_DIAG_RECOMM "DiagRecomm"
#define ITEM_CR_RSA "RSA"
#define ITEM_CR_DATA_SHEET "DataSheet"
#define ITEM_CR_SUMM_GRAPHS "SummGraphs"
#define ITEM_CR_RAW_SIGNAL_SEL_MIN "RawSignalSelMin"
#define ITEM_CR_ENTIRE_RAW_SIGNAL "EntireRawSignal"
#define ITEM_CR_EVENT_LOG "EventLog"
#define ITEM_CR_FIRST_PAGE "FirstPage"
#define ITEM_CR_OTHER_PAGE "OtherPage"
#define ITEM_CR_INSTRUCTIONS2 "Instructions2"
#define ITEM_CR_INCLUDE_BOTH_RSA "IncludeBothRSA"
//#define ITEM_CR_ ""

// Diagnosis dialog box
#define ITEM_D_DIAGNOSIS "Diagnosis"
#define ITEM_D_SUGG_DIAG "SuggDiag"
#define ITEM_D_ACCEPT "Accept"
#define ITEM_D_MODIFY "Modify"
#define ITEM_D_REJECT "Reject"
#define ITEM_D_USE_ADDITIONAL_FEATURES "UseAdditionalFeatures"
#define ITEM_D_ADD_TO_REPORT "AddToReport"
#define ITEM_D_SAVE_DESC_TO_LIST "SaveDescToList"
#define ITEM_D_REMOVE_DESC_FROM_LIST "RemoveDescFromList"
#define ITEM_D_INCLUDE_ICD_CODE "IncludeICDCode"
#define ITEM_D_ADD_ICD_TO_LIST "AddICDToList"
#define ITEM_D_REMOVE_ICD_FROM_LIST "RemoveICDFromList"
#define ITEM_D_ERASE_DESC "EraseDesc"
#define ITEM_D_ENTER_DIAG_KEY_PHRASE "EnterDiagKeyPhrase"
#define ITEM_D_ADD_DIAG_DESC "AddDiagDesc"
#define ITEM_D_ENTER_ICD_CODE "EnterICDCode"
#define ITEM_D_DEL_ICD_CODE_LINE "DelICDCodeLine"
#define ITEM_D_ADD_NEW_ICD_CODE "AddNewICDCode"
//#define ITEM_D_ ""

// Diagnostic History dialog box
#define ITEM_DH_DIAG_HIST "DiagHist"
#define ITEM_DH_DATE "Date"
#define ITEM_DH_DIAGNOSIS "Diagnosis"
#define ITEM_DH_GO_TO_STUDY "GoToStudy"
//#define ITEM_DH_ ""

// Respiration dialog boxes
#define ITEM_R_RESPIRATION "Respiration"
#define ITEM_R_CHANGE_RESP "ChangeResp"
#define ITEM_R_SET_RESP "SetResp"
#define ITEM_R_RESPRATE "RespRate"
#define ITEM_R_INSTRUCTIONS "Instructions"
//#define ITEM_R_ ""

// Event List dialog box
#define ITEM_EV_EVENTS "Events"
#define ITEM_EV_DESCRIPTION "Description"
//#define ITEM_EV_ ""

// Record Event dialog box
#define ITEM_RE_RECORD_EVENT "RecordEvent"
#define ITEM_RE_TIME "Time"
#define ITEM_RE_COUGH "Cough"
#define ITEM_RE_INGESTION "Ingestion"
#define ITEM_RE_MOVEMENT "Movement"
#define ITEM_RE_NAUSEA "Nausea"
#define ITEM_RE_PAIN "Pain"
#define ITEM_RE_TALKING "Talking"
#define ITEM_RE_VOMITING "Vomiting"
#define ITEM_RE_OTHER "Other"
#define ITEM_RE_DESCRIPTION "Description"
//#define ITEM_RE_ ""

// Facility Information dialog box
#define ITEM_FI_FACILITY_INFO "FacilityInfo"
#define ITEM_FI_FACILITY_PRACTICE "FacilityPractice"
#define ITEM_FI_ADDRESS "Address"
#define ITEM_FI_CITY "City"
#define ITEM_FI_STATE "State"
#define ITEM_FI_ZIPCODE "ZipCode"
#define ITEM_FI_PHONE_NUM "PhoneNum"
#define ITEM_FI_LOGO_REQ "LogoReq"
#define ITEM_FI_LOGO_REQ2 "LogoReq2"
#define ITEM_FI_LOGO_REQ3 "LogoReq3"
#define ITEM_FI_LOGO_REQ4 "LogoReq4"

//#define ITEM_FI_ ""

// Finish the Study dialog box
#define ITEM_FS_FINISH_STUDY "FinishStudy"
//#define ITEM_FS_ ""

// Medications dialog box
#define ITEM_M_MEDICATIONS "Medications"
#define ITEM_M_DATE "Date"
#define ITEM_M_MEDICATION "Medication"
#define ITEM_M_UPD_MED "UpdMed"
#define ITEM_M_GO_TO_STUDY "GoToStudy"
//#define ITEM_M_ ""

// POST_WATER_TEST dialog box
#define ITEM_PW_STUDYPHASE "StudyPhase"
#define ITEM_PW_BEGINSTUDY "BeginStudy"
#define ITEM_PW_EXTENDTEST "ExtendTest"
#define ITEM_PW_FINISHED "Finished"
//#define ITEM_PW_ ""

// Study Type dialog box
#define ITEM_ST_STUDY_TYPE "StudyType"
#define ITEM_ST_SEL_STUDY_TYPE "SelStudyType"
#define ITEM_ST_STD_STUDY "StdStudy"
#define ITEM_ST_RSCH_STUDY "RschStudy"
//#define ITEM_ST_ ""

// Stimulation Medium and Water Load dialog boxes
#define ITEM_SM_STIM_MEDIUM "StimMedium"
#define ITEM_SM_INGEST_STIM_MEDIUM "IngestStimMedium"
#define ITEM_SM_DESCRIPTION "Description"
#define ITEM_SM_QUANTITY "Quantity"
#define ITEM_SM_OZ "oz"
#define ITEM_SM_ML "ml"
#define ITEM_SM_GRAMS "grams"
#define ITEM_SM_START_STUDY "StartStudy"
#define ITEM_SM_WATERLOAD "WaterLoad"
#define ITEM_SM_AMTWATER "AmtWater"
#define ITEM_SM_INGESTWATER "IngestWater"
//#define ITEM_SM_ ""

// What If dialog box
#define ITEM_WI_WHATIFSCENARIO "WhatIfScenario"
#define ITEM_WI_DATETIME "DateTime"
#define ITEM_WI_DESCRIPTION "Description"
#define ITEM_WI_SAVE "Save"
#define ITEM_WI_RECALL "Recall"
#define ITEM_WI_DELETE "Delete"
//#define ITEM_WI_ ""

// Baseline dialog box
#define ITEM_BL_BASELINE "Baseline"
#define ITEM_BL_START_BL "StartBL"
#define ITEM_BL_EXTEND_BL "ExtendBL"
#define ITEM_BL_PAUSE_STUDY "PauseStudy"
#define ITEM_BL_WATER_LOAD "WaterLoad"
#define ITEM_BL_SKIP_STIM_MEDIUM "SkipStimMedium"
//#define ITEM_BL_ ""

// Settings dialog box
#define ITEM_SET_SETTINGS "Settings"
#define ITEM_SET_SEL_LANG "SelLang"
#define ITEM_SET_SEL_LOC_FOR_DATA_FILES "SelLocForDataFiles"
#define ITEM_SET_BROWSE "Browse"
#define ITEM_SET_SEL_LOC_FOR_DB "SelLocForDB"
#define ITEM_SET_SEL_REMOTE_DB_PATH "SelRemoteDBPath"
#define ITEM_SET_ENTER_REMOTE_DB_PASSWD "EnterRemoteDBPasswd"
//#define ITEM_SET_ ""

// Recommendations dialog box
#define ITEM_REC_PATIENT_RECOMMENDATIONS "PatientRecommendations"
#define ITEM_REC_ADD_TO_REPORT "AddToReport"
#define ITEM_REC_SAVE_RECOMMENDATION_TO_LIST "SaveRecommendationToList"
#define ITEM_REC_REMOVE_FROM_LIST "RemoveFromList"
#define ITEM_REC_ENTER_RECOMM_KEY_PHRASE "EnterRecommKeyPhrase"
#define ITEM_REC_ADD_RECOMM_DESC "AddRecommDesc"
#define ITEM_REC_ERASE_RECOMM_DESC "EraseRecommDesc"
//#define ITEM_REC_ ""

// Frequency range dialog box
#define ITEM_FR_TITLE   "FreqRangeTitle"
#define ITEM_FR_SELECT  "SelFreqRange"
//#define ITEM_FR_

// Filter RSA dialog box
#define ITEM_FRSA_TITLE   "FilterRSATitle"
#define ITEM_FRSA_FILTER_RSA_CHKBOX  "FilterRSAChkBox"

// Study Type and Frequency Range dialog box.
#define ITEM_STFR_TITLE "StudyTypeFreqRangeTitle"
#define ITEM_STFR_LABEL "StudyTypeFreqRangeLabel"
#define ITEM_STFR_STD_0_15_CHKBOX "Std15Chkbox"
#define ITEM_STFR_RSCH_0_15_CHKBOX "Rsrch15Chkbox"
#define ITEM_STFR_RSCH_0_60_CHKBOX "Rsrch60Chkbox"
#define ITEM_STFR_RSCH_0_200_CHKBOX "Rsrch200Chkbox"

// Show/hide study dates dialog box
#define ITEM_HIDE_STUDY_DATES_TITLE "HideStudyDatesTitle"
#define ITEM_ENTER_PSWD "EnterPswd"
#define ITEM_CREATE_NEW_PSWD "CreateNewPswd"
#define ITEM_RESET_PSWD "ResetPswd"
#define ITEM_ENTER_NEW_PSWD "EnterNewPswd"
#define ITEM_NEXT "Next"
#define ITEM_REENTER_PSWD "ReenterPswd"
#define ITEM_CURRENTLY_HIDDEN "CurrentlyHidden"
#define ITEM_CURRENTLY_HIDDEN1 "CurrentlyHidden1"
#define ITEM_CURRENTLY_NOT_HIDDEN "CurrentlyNotHidden"
#define ITEM_CURRENTLY_NOT_HIDDEN1 "CurrentlyNotHidden1"
#define ITEM_PSWD_NOT_CORRECT "PswdNotCorrect"
#define ITEM_ENTER_PSWD_CANCEL "EnterPswdCancel"
#define ITEM_MUST_ENTER_PSWD "MustEnterPswd"
#define ITEM_PSWD_ENTRIES_DONT_MATCH "PswdEntriesDontMatch"
#define ITEM_PLEASE_ENTER_PSWD "PleaseEnterPswd"
#define ITEM_PLEASE_ENTER_PSWD1 "PleaseEnterPswd1"
#define ITEM_ENTER_3CPM_PSWD "Enter3CPMPswd"
#define ITEM_ENTER_PSWD_VALIDATE "EnterPswdValidate"
#define ITEM_PSWD_OUT_OF_DATE "PswdOutOfDate"
#define ITEM_INITIAL_INSTRUCTION "InitialInstruction"
#define ITEM_INITIAL_INSTRUCTION1 "InitialInstruction1"

// Search dialog box
#define ITEM_SEARCH_TITLE "SearchTitle"
#define ITEM_LAST_NAME_RSRCH_REF_NUM "LastNameRsrchRefNum"
#define ITEM_LAST_NAME "LastName"
#define ITEM_RSRCH_REFNUM "RsrchRefNum"
#define ITEM_ASC_ORDER "AscOrder"
#define ITEM_DESC_ORDER "DescOrder"
#define ITEM_NAME_HELP "NameHelp"
#define ITEM_STUDY_DATE "StudyDate"
#define ITEM_DATE_OLDER "DateOlder"
#define ITEM_DATE_NEWER "DateNewer"
#define ITEM_DATE_EQUAL "DateEqual"
#define ITEM_DATE_HELP "DateHelp"
#define ITEM_MONTH_HELP "MonthHelp"
#define ITEM_DAY_HELP "DayHelp"
#define ITEM_YEAR_HELP "YearHelp"

// Import duplicate name dialog box
#define ITEM_DUP_NAME_TITLE "DupNameTitle"
#define ITEM_DUP_NAME_TITLE1 "DupNameTitle1"
#define ITEM_SKIP "Skip"
#define ITEM_OVERWRITE "Overwrite"
#define ITEM_CHANGE "Change"
#define ITEM_LAST_NAME "LastName"
#define ITEM_RSRCH_REF_NUM "RsrchRefNum"
//#define ITEM_ ""
//#define ITEM_ ""
//#define ITEM_ ""

// General text section
#define ITEM_G_OK "Ok"
#define ITEM_G_CANCEL "Cancel"
#define ITEM_G_YES "Yes"
#define ITEM_G_NO "No"
#define ITEM_G_Help "Help"
#define ITEM_G_INFORMATION "Information"
#define ITEM_G_REQ_ACTIVATION "ReqActivation"
#define ITEM_G_PATIENT_NAME "PatientName"
#define ITEM_G_DATE_OF_STUDY "DateOfStudy"
#define ITEM_G_ERROR "Error"
#define ITEM_G_PATIENT_IN_DB_CONT "PatientInDBCont"
#define ITEM_G_WARNING "Warning"
#define ITEM_G_END_STUDY_SURE "EndStudySure"
#define ITEM_G_NOT_READING_DATA "NotReadingData"
#define ITEM_G_ERR_EGG_READ "ErrEggRead"
#define ITEM_G_CONTINUE_QUES "ContinueQues"
#define ITEM_G_MINUTES "Minutes"
#define ITEM_G_EGG "EGG"
#define ITEM_G_RAW_WAVFRM_Y_AXIS_LBL "RawWavfrmYAxisLbl"
#define ITEM_G_RESPIRATION "Respiration"
#define ITEM_G_CLOSE "Close"
#define ITEM_G_COPY_ERR_CONT "CopyErrCont"
#define ITEM_G_LOCAL_DB "LocalDB"
#define ITEM_G_NEED_RSCH_REF_NUM "NeedRschRefNum"
#define ITEM_G_NEED_FIRST_NAME "NeedFirstName"
#define ITEM_G_NEED_LAST_NAME "NeedLastName"
#define ITEM_G_INV_DATE "InvDate"
#define ITEM_G_SEL_RDR_LOC_DB "SelRdrLocDB"
#define ITEM_G_SIGNAL_NOT_STABLE1 "SignalNotStable1"
#define ITEM_G_SIGNAL_NOT_STABLE2 "SignalNotStable2"
#define ITEM_G_SIGNAL_NOT_STABLE3 "SignalNotStable3"
#define ITEM_G_BACKUP_PROGRESS "BackupProgress"
#define ITEM_G_RESTORE_PROGRESS "RestoreProgress"
#define ITEM_G_BACKUP_TO "BackupTo"
#define ITEM_G_RESTORE_FROM "RestoreFrom"
#define ITEM_G_BACKUP_SUCCESS "BackupSuccess"
#define ITEM_G_RESTORE_SUCCESS "RestoreSuccess"
#define ITEM_G_BACKUP_ERRORS "BackupErrors"
#define ITEM_G_RESTORE_ERRORS "RestoreErrors"
#define ITEM_G_RESTORE_RAILED "RestoreRailed"
#define ITEM_G_BACKUP_FAILED "BackupFailed"
#define ITEM_G_IMPORT_PROG "ImportProg"
#define ITEM_G_SEL_STUDY_FROM_LIST "SelStudyFromList"
#define ITEM_G_ADD_EXT_STUDIES_SURE "AddExtStudiesSure"
#define ITEM_G_ADD_EXT_STUDIES_SURE1 "AddExtStudiesSure1"
#define ITEM_G_IMPORTCANCELLED "ImportCancelled"
#define ITEM_G_IMPORTFAILED "ImportFailed"
#define ITEM_G_IMPORTCOMPLETE "ImportComplete"
#define ITEM_G_SEL_REMOTE_DB_LOC "SelRemoteDBLoc"
#define ITEM_G_OVERWRITE_PATIENT "OverwritePatient"
#define ITEM_G_OVERWRITE_PATIENT1 "OverwritePatient1"
#define ITEM_G_OVERWRITE_FILE "OverwriteFile"
#define ITEM_G_OVERWRITE_STUDY "OverwriteStudy"
#define ITEM_G_OVERWRITE_STUDY1 "OverwriteStudy1"
#define ITEM_G_OVERWRITE_STUDY2 "OverwriteStudy2"
#define ITEM_G_OVERWRITE_STUDY3 "OverwriteStudy3"
#define ITEM_G_FILE_COPY_ERR "FileCopyErr"
#define ITEM_G_INV_FILE_HDR "InvFileHdr"
#define ITEM_G_DATA_FILE_MISSING "DataFileMissing"
#define ITEM_G_BCKUP_DATA_FILE_MISSING "BckupDataFileMissing"
#define ITEM_G_BCKUP_ERRS_NUM "BckupErrsNum"
#define ITEM_G_BCKUP_ERRS "BckupErrs"
#define ITEM_G_RESTORE_ERRS "RestoreErrs"
#define ITEM_G_RESTORE_ERRS_NUM "RestoreErrsNum"
#define ITEM_G_CORE_RPT_SELECTED "CoreRptSelected"
#define ITEM_G_DIAG_PP_NORM "DiagPPNorm"
#define ITEM_G_DIAG_PP_PROB_NORM "DiagPPProbNorm"
#define ITEM_G_DIAG_PP_SUB_PROB "DiagPPSubProb"
#define ITEM_G_DIAG_PP_TACHY "DiagPPTachy"
#define ITEM_G_DIAG_PP_PROB_TACHY "DiagPPProbTachy"
#define ITEM_G_DIAG_PP_POS_TACHY_CC "DiagPPPosTachyCC"
#define ITEM_G_DIAG_PP_BRADY "DiagPPBrady"
#define ITEM_G_DIAG_PP_PROB_BRADY "DiagPPProbBrady"
#define ITEM_G_DIAG_PP_POS_BRADY_CC "DiagPPPosBradyCC"
#define ITEM_G_DIAG_PP_GOO "DiagPPGoo"
#define ITEM_G_DIAG_PP_MXNS "DiagPPMxns"
#define ITEM_G_DIAG_DUOD_HYPER "DiagDuodHyper"
#define ITEM_G_DIAGNOSIS "Diagnosis"
#define ITEM_G_POST_STIM_PERIOD "PostStimPd"
#define ITEM_G_BASELINE "Baseline"
#define ITEM_G_SAVE_CLOSE "SaveClose"
#define ITEM_G_MEDICATION "Medication"
#define ITEM_G_FIRST_10_MIN_PD "First10MinPD"
#define ITEM_G_SECOND_10_MIN_PD "Second10MinPD"
#define ITEM_G_THIRD_10_MIN_PD "Third10MinPD"
#define ITEM_G_FOURTH_10_MIN_PD "Fourth10MinPD"
#define ITEM_G_ENTER_TYPE_MED_QTY "EnterTypeMedQty"
#define ITEM_G_TYPE_MED_QTY "TypeMedQty"
#define ITEM_G_ENTER_WATER_AMOUNT "EnterWaterAmt"
#define ITEM_G_WATER_INGESTED "WaterIngested"
#define ITEM_G_WHATIF_NEW "WhatifNew"
#define ITEM_G_CANT_NAME_WHATIF "CantNameWhatif"
#define ITEM_G_SURE_OVERWRITE_WHATIF "SureOverwriteWhatif"
#define ITEM_G_FAILED_TO_SAVE_WHATIF "FailSaveWhatif"
#define ITEM_G_ENTER_DESCRIPTION "EnterDescrip"
#define ITEM_G_SELECT_WHATIF "SelWhatif"
#define ITEM_G_ASK_DELETE_WHATIF "AskDelWhatif"
#define ITEM_G_CANT_DELETE_ORIG_STUDY "CantDelOrigStudy"
#define ITEM_G_WHATIF_ORIGINAL_STUDY "WhatifOrigStudy"
#define ITEM_G_SELECT_WHATIF_LIST "SelWhatifList"
#define ITEM_G_STOP_BASELINE "StopBaseline"
#define ITEM_G_STIMULATION_MEDIUM "StimMedium"
#define ITEM_G_PAUSE_CONTINUE_BASELINE "PauseContBL"
#define ITEM_G_PAUSE_CONTINUE_BASELINE1 "PauseContBL1"
#define ITEM_G_RESUME_TO_CONTINUE "ResumeToCont"
#define ITEM_G_STOP_CONTINUE_BASELINE "StopContBL"
#define ITEM_G_SQL_ERROR "SQLError"
#define ITEM_G_SQL_INVALID_HANDLE "SQLInvHandle"
#define ITEM_G_SQL_ERR_UNKNOWN "SQLErrUnknown"
#define ITEM_G_FMT_DATABASE_ERROR "FmtDBErr"
#define ITEM_G_DB_NOT_FOUND "DBNotFound"
#define ITEM_G_ERR_LOAD_DLL "ErrLoadDll"
#define ITEM_G_ERR_DLL_FUNCS_NOT_FOUND "ErrDllFuncsNotFound"
#define ITEM_G_ERR_DLL "ErrDll"
#define ITEM_G_ERR_DLL_CHANNELS "ErrDllChannels"
#define ITEM_G_ERR_READ_BATTERY_TIMEOUT "ErrReadBattTimeout"
#define ITEM_G_BAD_BATTERY "BadBatt"
#define ITEM_G_LOW_BATTERY "LowBatt"
#define ITEM_G_ERR_READ_POS_IMP_TIMEOUT "ErrReadPosImpTimeout"
#define ITEM_G_BAD_POS_IMPEDANCE "BadPosImpedance"
#define ITEM_G_ERR_READ_NEG_IMP_TIMEOUT "ErrReadNegImpTimeout"
#define ITEM_G_BAD_NEG_IMPEDANCE "BadNegImpedance"
#define ITEM_G_EXPIRE_DATE "ExpireDate"
#define ITEM_G_EXPIRED "Expired"
#define ITEM_G_OPEN_SCREEN_NOTICE "OpenScreenNotice"
#define ITEM_G_OPEN_SCREEN_NOTICE1 "OpenScreenNotice1"
#define ITEM_G_DEMO_OPEN_SCREEN_NOTICE "DemoOpenScreenNotice"
#define ITEM_G_INV_STUDY_FOR_VERSION "InvStudyForVers"
#define ITEM_G_ERR_INVALID_HDR "ErrInvHdr"
#define ITEM_G_ERR_POSITION_FILE "ErrPosFile"
#define ITEM_G_ERR_READ_FILE "ErrReadFile"
#define ITEM_G_ERR_UNEXPECTED_EOF "ErrunexpectEOF"
#define ITEM_G_ERR_OPEN_FILE "ErrOpenFile"
#define ITEM_G_NO_DATA_IN_FILE "NoDataInFile"
#define ITEM_G_GOOD_MINUTES_UNCHECKED "GoodMinUnchkd"
#define ITEM_G_PERIOD_LENGTH_UNCHECKED "PeriodLenUnchkd"
#define ITEM_G_ERR_CREATE_FILE "ErrCreateFile"
#define ITEM_G_ERR_WRITE_FILE "ErrWriteFile"
#define ITEM_G_WANT_TO_SAVE_WHATIF "WantToSaveWhatif"
#define ITEM_G_WANT_TO_SAVE_PATIENT "WantToSavePatient"
#define ITEM_G_DB_VERSION_WRONG "DBverWrong"
#define ITEM_G_DB_ACCESS_ERR "DBAccessErr"
#define ITEM_G_CONTACT_SUPPORT "ContactSupport"
#define ITEM_G_FREQ_CPM "FreqCPM"
#define ITEM_G_STIMULATION_MEDIUM_PHASE "StimMedPhs"
#define ITEM_G_TIME "Time"
#define ITEM_G_WATER_INGESTION_PHASE "WaterIngestionPhs"
#define ITEM_G_ENTER_FILE_NAME "EnterFileName"
#define ITEM_G_1_25CPM "1_25CPM"
#define ITEM_G_25_375CPM "25_375CPM"
#define ITEM_G_375_100CPM "375_100CPM"
#define ITEM_G_100_150CPM "100_150CPM"
#define ITEM_G_1_15CPM "1_15CPM"
#define ITEM_G_BRADY "Brady"
#define ITEM_G_NORMAL "Normal"
#define ITEM_G_TACHY "Tachy"
#define ITEM_G_DUOD "Duod"
#define ITEM_G_ENTIRE_RANGE "EntireRange"
#define ITEM_G_PERIOD "Period"
#define ITEM_G_0_300 "0_300"
#define ITEM_G_325_600 "325_600"
#define ITEM_G_625_1500 "625_1500"
#define ITEM_G_0_15 "0_15"
#define ITEM_G_SUMM_BRADY_TITLE "SummBradyTitle"
#define ITEM_G_SUMM_NORMAL_TITLE "SummNormalTitle"
#define ITEM_G_SUMM_TACHY_TITLE "SummTachyTitle"
#define ITEM_G_SUMM_DUOD_TITLE "SummDuodTitle"
#define ITEM_G_CONTROL "Control"
#define ITEM_G_PATIENT "Patient"
#define ITEM_G_BL "BL"
#define ITEM_G_MIN_10 "Min10"
#define ITEM_G_MIN_20 "Min20"
#define ITEM_G_MIN_30 "Min30"
#define ITEM_G_DS_DIST_AVG_PWR_15_50 "DSDistAvgPwr15_50"
#define ITEM_G_DS_RATIO_AVG_PWR "DSRatioAvgPwr"
#define ITEM_G_DS_DIST_AVG_PWR_RNG "DSDistAvgPwrRng"
#define ITEM_G_DS_AVG_DOM_FREQ "DAvgDomFreq"
#define ITEM_G_DS_PCT_DOM_FREQ "DSPctDomFreq"
#define ITEM_G_NO_STIM_MEDIUM "NoStimMedium"
#define ITEM_G_REF_NUM "RefNum"
#define ITEM_G_ATTENDING_PHYSICIAN "AttendingPhysician"
#define ITEM_G_REFERING_PHYSICIAN "ReferingPhysician"
#define ITEM_G_TECHNICIAN "Technician"
#define ITEM_G_INDICATIONS "Indications"
//#define ITEM_G_NAUSEA "Nausea"
//#define ITEM_G_VOMITING "Vomiting"
//#define ITEM_G_BLOATING "Bloating"
#define ITEM_G_GERD "GERD"
#define ITEM_G_ERUCTUS "Eructus"
//#define ITEM_G_EARLY_SATEITY "EarlySateity"
#define ITEM_G_PP_FULLNESS "PPFullness"
//#define ITEM_G_DELAYED_GASTRIC_EMPTYING "DelayedGastricEmptying"
#define ITEM_G_DIAGNOSIS "Diagnosis"
#define ITEM_G_RECOMMENDATIONS "Recommendations"
#define ITEM_G_SIGNATURE "Signature"
#define ITEM_G_DATE "Date"
#define ITEM_G_ERR_PRINTER_INIT "ErrPrntrInit"
#define ITEM_G_ERR_PRINTER_START_PAGE "ErrPrntrStartPage"
#define ITEM_G_PAGE_NUMBER "PageNum"
#define ITEM_G_RESP_NOT_SAVED "RespNotSaved"
#define ITEM_G_CHG_RESP_NOT_SET "ChgRespNotSet"
#define ITEM_G_SS_LEFT_TITLE_1  "SSLeftTitle1"
#define ITEM_G_SS_LEFT_TITLE_2  "SSLeftTitle2"
#define ITEM_G_SS_LEFT_TITLE_3  "SSLeftTitle3"
#define ITEM_G_SS_LEFT_TITLE_4  "SSLeftTitle4"
#define ITEM_G_SS_LEFT_TITLE_5  "SSLeftTitle5"
#define ITEM_G_SS_LEFT_TITLE_6  "SSLeftTitle6"
#define ITEM_G_SS_LEFT_TITLE_7  "SSLeftTitle7"
#define ITEM_G_SS_LEFT_TITLE_8  "SSLeftTitle8"
#define ITEM_G_SS_LEFT_TITLE_9  "SSLeftTitle9"
#define ITEM_G_SS_LEFT_TITLE_10 "SSLeftTitle10"
#define ITEM_G_SS_LEFT_TITLE_11 "SSLeftTitle11"
#define ITEM_G_SS_FILE_CREATED "SSFileCreated"
#define ITEM_G_PM "PM"
#define ITEM_G_AM "AM"
#define ITEM_G_BACKUP_ERROR "BackupErr"
#define ITEM_G_PUT_IN_DISK "PutInDisk"
#define ITEM_G_PUT_IN_NEW_DISK "PutInNewDisk"
#define ITEM_G_DISK_TRY_AGAIN "DiskTryAgain"
#define ITEM_G_RESET_PERIODS_QUESTION "ResetPdsQues"
#define ITEM_G_RESET_PERIODS_QUESTION1 "ResetPdsQues1"
#define ITEM_G_NEED_4_MINUTES "Need4Min"
#define ITEM_G_MIN_INCL_ART "MinInclArt"
#define ITEM_G_MIN_INCL_ART1 "MinInclArt1"
#define ITEM_G_START_MINUTE_FORMAT_ERR "StartMinFmtErr"
#define ITEM_G_END_MINUTE_FORMAT_ERR "EndMinFmtErr"
#define ITEM_G_START_MIN_IN_PREV_PERIOD "StartMinInPrevPd"
#define ITEM_G_END_MIN_IN_NEXT_PERIOD "EndMinInNxtPd"
#define ITEM_G_ASK_DELETE_EVENT "AskDelEvnt"
#define ITEM_G_START_MINUTE_OUT_OF_RANGE "StartMinOutOfRange"
#define ITEM_G_END_MINUTE_OUT_OF_RANGE "EndMinOutOfRange"
#define ITEM_G_STANDARD "Standard"
#define ITEM_G_RESEARCH "Research"
#define ITEM_G_EVENT_LOG "EventLog"
#define ITEM_G_INDICATION_KEY "IndicationKey"
#define ITEM_G_COUGH_KEY "CoughKey"
#define ITEM_G_INGESTION_KEY "IngestionKey"
#define ITEM_G_MOVEMENT_KEY "MovementKey"
#define ITEM_G_NAUSEA_KEY "NauseaKey"
#define ITEM_G_PAIN_KEY "PainKey"
#define ITEM_G_TALKING_KEY "TalkingKey"
#define ITEM_G_VOMITING_KEY "VomitingKey"
#define ITEM_G_OTHER_KEY "OtherKey"
#define ITEM_G_INDICATION_COUGH "IndicationCough"
#define ITEM_G_INDICATION_INGESTION "IndicationIngestion"
#define ITEM_G_INDICATION_MOVEMENT "IndicationMovement"
#define ITEM_G_INDICATION_NAUSEA "IndicationNausea"
#define ITEM_G_INDICATION_PAIN "IndicationPain"
#define ITEM_G_INDICATION_TALKING "IndicationTalking"
#define ITEM_G_INDICATION_VOMITING "IndicationVomiting"
#define ITEM_G_INDICATION_OTHER "IndicationOther"
#define ITEM_G_SEL_DATA_PATH "SelDataPath"
#define ITEM_G_INC_IN_RPT_DESC "IncInRptDesc"
#define ITEM_G_EGG_RPT_TITLE "EGGRptTitle"
#define ITEM_G_AMTWATERINGESTED "AmtWaterIngested"
#define ITEM_G_ICDCODE "ICDCode"
#define ITEM_G_ADD_NEW_KEY_PHRASE "AddNewKeyPhrase"
#define ITEM_G_CPM "CPM"
#define ITEM_G_CPM1 "CPM1"
#define ITEM_G_GOTO_WHATIF1 "GoToWhatIF1"
#define ITEM_G_GOTO_WHATIF2 "GoToWhatIF2"
#define ITEM_G_SEL_STUDY_TYPE_FREQ_RANGE "SelStudyTypeFreqRange"
#define ITEM_G_CHANGE_IN_SETTINGS "ChangeInSettings"
//#define ITEM_G_ ""
//#define ITEM_G_ ""
//#define ITEM_G_ ""
//// End of INI item defines


class CLanguage
  {

  public:
    CLanguage(void);
    virtual ~CLanguage(void);

    CString m_cstrINISection;

    // Text for tool tips
    CString cstrNewStudy;
    CString cstrOpenPriorStudy;
    CString cstrSavePatient;
    CString cstrPrintReport;
    CString cstrStartOfStudy;
    CString cstrPrevious10MinutePeriod;
    CString cstrPreviousPeriod;
    CString cstrCurrentPeriod;
    CString cstrNext10MinutePeriod;
    CString cstrNextPeriod;
    CString cstrEndOfStudy;
    CString cstrRSA;
    CString cstrSummaryGraphs;
    CString cstrDataSheet;
    CString cstrDiagnosis;
    CString cstrRecommendations;
    CString cstrWhatIfScenario;
    CString cstrAbout;
    CString cstrHelpTopics;
    CString cstrFilterRSA;

    // These 2 arrays must match each other.
    // Array of languages (ex: US English)
    CStringArray m_cstraLang;
    // Array of cultures (ex: en-US)
    CStringArray m_cstraCultures;
    int m_iLangIndex; // Index into the above language/culture array.
    CString m_cstrCulture; // Culture for the currently selected language.

    // Methods
    void vLoad_menus(unsigned int uMenuID);
    LPSTR pstrGet_tooltip(unsigned int uID);

    void vSet_lang_file(void);
    void vStart_section(CString cstrSection);
    void vEnd_section();
    CString cstrGet_text(CString cstrID);
    CString cstrLoad_string(CString cstrID);
    CString cstrLoad_string_fmt(CString cstrID, ...);
    void vGet_languages(void);
    CString cstrGet_first_language(void);
    CString cstrGet_next_language(void);
    void vSet_language(CString cstrLanguage);
    CString cstrGet_language(void);

  };
