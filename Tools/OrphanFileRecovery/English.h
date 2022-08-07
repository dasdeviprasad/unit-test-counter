/********************************************************************
English.h

Copyright (C) 2010,2016 3CPM Company, Inc.  All rights reserved.

  Contains English text strings.


HISTORY:
25-JUN-10  RET  New.
08-JUN-16  RET
             Fix program crash if user selects add to database without selecting
             a study.
               Add define: IDS_PLEASE_MAKE_SELECTION
********************************************************************/

#define IDS_SQL_ERROR "SQL Error"
#define IDS_SQL_INVALID_HANDLE "Invalid SQL handle."
#define IDS_SQL_ERR_UNKNOWN "Unknown database error."
#define IDS_FMT_DATABASE_ERROR "Database error: %s"
#define IDS_DB_NOT_FOUND "The database was not found: %s"

#define IDS_ERROR1 "ERROR"
#define IDS_INFORMATION "Information"
#define IDS_STUDY_NOT_COMPLETE "Study not completed. Total minutes is %.1f"
#define IDS_INFORMATION "Information"
#define IDS_ERR_CANT_ACCESS_DB "Can't access database."
#define IDS_ERR_FILE_READ "Error reading file %s, %s"
#define IDS_ERR_UNEXPECTED_EOF "Unexpected EOF in %s"
#define IDS_ERR_FILE_OPEN "Error opening file %s, %s"
#define IDS_ERR_NO_HEADER "No header found in: %s"
#define IDS_ERR_INV_FILE_HDR "File header is invalid for %s"

#define IDS_PATIENT_NAME "Patient name"
#define IDS_DATE_OF_STUDY "Date of study"
#define IDS_TYPE_OF_STUDY "Type of Study"
#define IDS_RESEARCH_STUDY "Research"
#define IDS_STANDARD_STUDY "WaterLoad"

#define IDS_STUDY_CANCELLED "The study was cancelled."
#define IDS_NOT_ENOUGH_BASELINE "There are not enough minutes in the baseline to analyze."

#define IDS_WANT_TO_ADD_TO_DB "Do you want to add this study to the database?"
#define IDS_STUDY_CANCELLED_DURING_BASELINE "The study was cancelled during the baseline."
#define IDS_NO_POSTWATER_STUDY "There is not post-waterload study."
#define IDS_STUDY_CANCELLED_DURING_POSTWATER_STUDY "The study was cancelled during the post-waterload."
#define IDS_ADD_AS_PAUSED_STUDY "If Yes, the study will be added as a paused study."

#define IDS_MINUTES_DONT_ADDUP "The baseline and post-waterload minutes don't add up to the total minutes."
#define IDS_STUDY_ADD_TO_DB "The study for the patient (%s) and study date (%s) has been successfully added to the database."
#define IDS_NOT_ENOUGH_POSTSTIM_MIN "There are not enough minutes in the post-stimulation period.  The study was cancelled."
#define IDS_PLEASE_MAKE_SELECTION "Please select a study for recovery."
