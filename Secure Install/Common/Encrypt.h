/********************************************************************
Encrypt.h

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Header file for the CEncrypt class.

HISTORY:
10-AUG-10  RET  New.
********************************************************************/

#pragma once

// Maximum password size.
#define MAX_PWD_SIZE 4

// GUID positions for encrypting the password.
#define GUID_ENCRYPT_11 1  // Line index for first number in encryption formula.
#define GUID_ENCRYPT_12 5  // Character index for first number in encryption formula.
#define GUID_ENCRYPT_21 3  // Line index for second number in encryption formula.
#define GUID_ENCRYPT_22 20 // Character index for second number in encryption formula.
#define GUID_ENCRYPT_31 5  // Line index for third number in encryption formula.
#define GUID_ENCRYPT_32 12 // Character index for third number in encryption formula.
// GUID positions for storing the encrypted password and the MAC address.
//   character 1: [0, 14], [3, 4], [1, 3], [4, 34]
#define GUID_STORE_11 0
#define GUID_STORE_12 14
#define GUID_STORE_13 3
#define GUID_STORE_14 4
#define GUID_STORE_15 1
#define GUID_STORE_16 3
#define GUID_STORE_17 4
#define GUID_STORE_18 34

//   character 2: [5, 10], [1, 34], [2, 3], [0, 10]
#define GUID_STORE_21 5
#define GUID_STORE_22 10
#define GUID_STORE_23 1
#define GUID_STORE_24 34
#define GUID_STORE_25 2
#define GUID_STORE_26 3
#define GUID_STORE_27 0
#define GUID_STORE_28 10

//   character 3: [0, 2], [4, 19], [5, 4], [0, 24]
#define GUID_STORE_31 0
#define GUID_STORE_32 2
#define GUID_STORE_33 4
#define GUID_STORE_34 19
#define GUID_STORE_35 5
#define GUID_STORE_36 4
#define GUID_STORE_37 0
#define GUID_STORE_38 24

//   character 4: [2, 25], [5, 14], [1, 20], [2, 11]
#define GUID_STORE_41 2
#define GUID_STORE_42 25
#define GUID_STORE_43 5
#define GUID_STORE_44 14
#define GUID_STORE_45 1
#define GUID_STORE_46 20
#define GUID_STORE_47 2
#define GUID_STORE_48 11

//   character 5: [5, 29], [0, 33], [3, 30], [4, 21]
#define GUID_STORE_51 5
#define GUID_STORE_52 29
#define GUID_STORE_53 0
#define GUID_STORE_54 33
#define GUID_STORE_55 3
#define GUID_STORE_56 30
#define GUID_STORE_57 4
#define GUID_STORE_58 21

//   character 6: [3, 15], [4, 7], [0, 7], [1, 9]
#define GUID_STORE_61 3
#define GUID_STORE_62 15
#define GUID_STORE_63 4
#define GUID_STORE_64 7
#define GUID_STORE_65 0
#define GUID_STORE_66 7
#define GUID_STORE_67 1
#define GUID_STORE_68 9


class CEncrypt
  {

  public:
    CEncrypt(void);
    virtual ~CEncrypt(void);

    char m_szPwd[MAX_PWD_SIZE + 1];
    void vEncrypt_password(char szData[][DATA_ARRAY_SIZE_2]);
    void vDecrypt_password(char szData[][DATA_ARRAY_SIZE_2]);
    bool bValidate_password(CString cstrPassword);
    void vEncrypt_mac_address(char szData[][DATA_ARRAY_SIZE_2], BYTE *pbytMacAddr);

  };
