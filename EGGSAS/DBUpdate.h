/********************************************************************
DBUpdate.h

Copyright (C) 2019 3CPM Company, Inc.  All rights reserved.

  Header file for the CDBUpdate class.

HISTORY:
09-FEB-19  RET
             New
********************************************************************/
#pragma once

class CDBUpdate
  {
  public:
    CDBUpdate(void);
    ~CDBUpdate(void);
    int iUpdate_database(CString cstrUpdDB);
    CString cstrGet_sys_err_msg(void);
  };
