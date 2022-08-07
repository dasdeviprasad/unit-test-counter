/********************************************************************
Encrypt.cpp

Copyright (C) 2010,2012, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CEncrypt class.

  This class provides methods for encrypting/decrytping the password
  and MAC address in the array that holds the data file.

HISTORY:
10-AUG-10  RET  New.
18-SEP-12  RET
             Fix problem in validating the password. It used to compare the
               password from the validate text file with what the password
               is supposed to be not what the user entered.
                 Change method: bValidate_password()
********************************************************************/

// Password creation
// Each password character is encrypted as a formula based on the GUIDs in the file.
// encrypted character = Password character + (GUID[1,5] * GUID[3,20]) - GUID[5,12]
// Password character = encrypted character + GUID[5,12] - (GUID[1,5] * GUID[3,20])
//   where GUID[X,Y]:
//      X is line number (0 based) from secure data file
//      Y is character index on the line.
//      The encrypted character is actually a word and will be placed in the secure
//        data file as 4 hex characters.
// Can't use GUID character indexes: 8,13,18,23.  These are dashes on each line.
// Can't use GUID position: [0, 0] and [0, 1], these are the file version.
//    Used GUID positions:
//      [0, 2], [0, 7], [0, 10], [0, 14], [0, 24], [0, 33]
//      [1, 3], [1,5], [1, 9], [1, 20], [1, 34]
//      [2, 3], [2, 11], [2, 25]
//      [3, 4], [3, 15], [3,20], [3, 30]
//      [4, 7], [4, 19], [4, 21], [4, 34]
//      [5, 4], [5, 10], [5,12], [5, 14], [5, 29]
// Max GUID character index is 35.
// encrypted password is stored in GUID[line, char] as follows:
//   character 1: [0, 14], [3, 4], [1, 3], [4, 34]
//   character 2: [5, 10], [1, 34], [2, 3], [0, 10]
//   character 3: [0, 2], [4, 19], [5, 4], [0, 24]
//   character 4: [2, 25], [5, 14], [1, 20], [2, 11]
// The MAC address will replace the encoded password positions.
// The MAC address will be encrypted as follows:
// MAC sample: 00-0D-56-A8-AB-A7
//   Each number will take up 1 postion in the GUID, dashes are not saved.
//   Four additional character positions are needed.
//   character 5: [5, 29], [0, 33], [3, 30], [4, 21]
//   character 6: [3, 15], [4, 7], [0, 7], [1, 9]


#include "StdAfx.h"
#include "..\common\SecureInstall.h"
#include "Encrypt.h"

/********************************************************************
Constructor

********************************************************************/
CEncrypt::CEncrypt(void)
  {
  }

/********************************************************************
Destructor

********************************************************************/
CEncrypt::~CEncrypt(void)
  {
  }

/********************************************************************
vEncrypt_password - Encrypt the password into the data array.

inputs: Data array.

return: None.
********************************************************************/
void CEncrypt::vEncrypt_password(char szData[][DATA_ARRAY_SIZE_2])
  {
  unsigned short uNum;
  char szPwd[10], szEncrypted[5];

  strcpy(szPwd, INSTALL_PASSWORD);
  // First character
  uNum = (unsigned short)szPwd[0]
         + (szData[GUID_ENCRYPT_11][GUID_ENCRYPT_12]
         * szData[GUID_ENCRYPT_21][GUID_ENCRYPT_22])
         - szData[GUID_ENCRYPT_31][GUID_ENCRYPT_32];
  sprintf(szEncrypted, "%04X", uNum);
  szData[GUID_STORE_11][GUID_STORE_12] = szEncrypted[0];
  szData[GUID_STORE_13][GUID_STORE_14] = szEncrypted[1];
  szData[GUID_STORE_15][GUID_STORE_16] = szEncrypted[2];
  szData[GUID_STORE_17][GUID_STORE_18] = szEncrypted[3];

  // Second character
  uNum = (unsigned short)szPwd[1]
         + (szData[GUID_ENCRYPT_11][GUID_ENCRYPT_12]
         * szData[GUID_ENCRYPT_21][GUID_ENCRYPT_22])
         - szData[GUID_ENCRYPT_31][GUID_ENCRYPT_32];
  sprintf(szEncrypted, "%04X", uNum);
  szData[GUID_STORE_21][GUID_STORE_22] = szEncrypted[0];
  szData[GUID_STORE_23][GUID_STORE_24] = szEncrypted[1];
  szData[GUID_STORE_25][GUID_STORE_26] = szEncrypted[2];
  szData[GUID_STORE_27][GUID_STORE_28] = szEncrypted[3];

  // Third character
  uNum = (unsigned short)szPwd[2]
         + (szData[GUID_ENCRYPT_11][GUID_ENCRYPT_12]
         * szData[GUID_ENCRYPT_21][GUID_ENCRYPT_22])
         - szData[GUID_ENCRYPT_31][GUID_ENCRYPT_32];
  sprintf(szEncrypted, "%04X", uNum);
  szData[GUID_STORE_31][GUID_STORE_32] = szEncrypted[0];
  szData[GUID_STORE_33][GUID_STORE_34] = szEncrypted[1];
  szData[GUID_STORE_35][GUID_STORE_36] = szEncrypted[2];
  szData[GUID_STORE_37][GUID_STORE_38] = szEncrypted[3];

  // Character 4
  uNum = (unsigned short)szPwd[3]
         + (szData[GUID_ENCRYPT_11][GUID_ENCRYPT_12]
         * szData[GUID_ENCRYPT_21][GUID_ENCRYPT_22])
         - szData[GUID_ENCRYPT_31][GUID_ENCRYPT_32];
  sprintf(szEncrypted, "%04X", uNum);
  szData[GUID_STORE_41][GUID_STORE_42] = szEncrypted[0];
  szData[GUID_STORE_43][GUID_STORE_44] = szEncrypted[1];
  szData[GUID_STORE_45][GUID_STORE_46] = szEncrypted[2];
  szData[GUID_STORE_47][GUID_STORE_48] = szEncrypted[3];
  }

void CEncrypt::vDecrypt_password(char szData[][DATA_ARRAY_SIZE_2])
  {
  unsigned uNum;
  char szEncrypted[10];

  // First character
  szEncrypted[0] = szData[GUID_STORE_11][GUID_STORE_12];
  szEncrypted[1] = szData[GUID_STORE_13][GUID_STORE_14];
  szEncrypted[2] = szData[GUID_STORE_15][GUID_STORE_16];
  szEncrypted[3] = szData[GUID_STORE_17][GUID_STORE_18];
  szEncrypted[4] = NULL;

  sscanf(szEncrypted, "%X", &uNum);
  m_szPwd[0] = (char)(uNum
         + szData[GUID_ENCRYPT_31][GUID_ENCRYPT_32]
         - (szData[GUID_ENCRYPT_11][GUID_ENCRYPT_12]
         * szData[GUID_ENCRYPT_21][GUID_ENCRYPT_22]));

  // Second character
  szEncrypted[0] = szData[GUID_STORE_21][GUID_STORE_22];
  szEncrypted[1] = szData[GUID_STORE_23][GUID_STORE_24];
  szEncrypted[2] = szData[GUID_STORE_25][GUID_STORE_26];
  szEncrypted[3] = szData[GUID_STORE_27][GUID_STORE_28];
  sscanf(szEncrypted, "%X", &uNum);
  m_szPwd[1] = (char)(uNum
         + szData[GUID_ENCRYPT_31][GUID_ENCRYPT_32]
         - (szData[GUID_ENCRYPT_11][GUID_ENCRYPT_12]
         * szData[GUID_ENCRYPT_21][GUID_ENCRYPT_22]));

  // Third character
  szEncrypted[0] = szData[GUID_STORE_31][GUID_STORE_32];
  szEncrypted[1] = szData[GUID_STORE_33][GUID_STORE_34];
  szEncrypted[2] = szData[GUID_STORE_35][GUID_STORE_36];
  szEncrypted[3] = szData[GUID_STORE_37][GUID_STORE_38];
  sscanf(szEncrypted, "%X", &uNum);
  m_szPwd[2] = (char)(uNum
         + szData[GUID_ENCRYPT_31][GUID_ENCRYPT_32]
         - (szData[GUID_ENCRYPT_11][GUID_ENCRYPT_12]
         * szData[GUID_ENCRYPT_21][GUID_ENCRYPT_22]));

  // Character 4
  szEncrypted[0] = szData[GUID_STORE_41][GUID_STORE_42];
  szEncrypted[1] = szData[GUID_STORE_43][GUID_STORE_44];
  szEncrypted[2] = szData[GUID_STORE_45][GUID_STORE_46];
  szEncrypted[3] = szData[GUID_STORE_47][GUID_STORE_48];
  sscanf(szEncrypted, "%X", &uNum);
  m_szPwd[3] = (char)(uNum
         + szData[GUID_ENCRYPT_31][GUID_ENCRYPT_32]
         - (szData[GUID_ENCRYPT_11][GUID_ENCRYPT_12]
         * szData[GUID_ENCRYPT_21][GUID_ENCRYPT_22]));
  m_szPwd[4] = NULL;
  }

/********************************************************************
bValidate_password - Compare the password from the data file to the
                     password required for this program.

inputs: None.

return: true it the 2 password compare.
        false if the passwords don't compare.
********************************************************************/
bool CEncrypt::bValidate_password(CString cstrPassword)
  {
  bool bRet;

  //if(strcmp(m_szPwd, INSTALL_PASSWORD) == 0)
  if(strcmp(m_szPwd, cstrPassword) == 0)
    bRet = true;
  else
    bRet = false;
  return(bRet);
  }

/********************************************************************
vEncrypt_mac_address - Encrypt the MAC address into the data array.
                       This overwrites the password.

inputs: Data array.
        Byte array containing the MAC address.

return: None.
********************************************************************/
void CEncrypt::vEncrypt_mac_address(char szData[][DATA_ARRAY_SIZE_2], BYTE *pbytMacAddr)
  {
  unsigned short uNum;
  char szEncrypted[5];

  // Byte 1 of MAC address
  uNum = *pbytMacAddr
         + (szData[GUID_ENCRYPT_11][GUID_ENCRYPT_12]
         * szData[GUID_ENCRYPT_21][GUID_ENCRYPT_22])
         - szData[GUID_ENCRYPT_31][GUID_ENCRYPT_32];
  sprintf(szEncrypted, "%04X", uNum);
  szData[GUID_STORE_11][GUID_STORE_12] = szEncrypted[0];
  szData[GUID_STORE_13][GUID_STORE_14] = szEncrypted[1];
  szData[GUID_STORE_15][GUID_STORE_16] = szEncrypted[2];
  szData[GUID_STORE_17][GUID_STORE_18] = szEncrypted[3];

  // Byte 2 of MAC address
  uNum = *(pbytMacAddr + 1)
         + (szData[GUID_ENCRYPT_11][GUID_ENCRYPT_12]
         * szData[GUID_ENCRYPT_21][GUID_ENCRYPT_22])
         - szData[GUID_ENCRYPT_31][GUID_ENCRYPT_32];
  sprintf(szEncrypted, "%04X", uNum);
  szData[GUID_STORE_21][GUID_STORE_22] = szEncrypted[0];
  szData[GUID_STORE_23][GUID_STORE_24] = szEncrypted[1];
  szData[GUID_STORE_25][GUID_STORE_26] = szEncrypted[2];
  szData[GUID_STORE_27][GUID_STORE_28] = szEncrypted[3];

  // Byte 3 of MAC address
  uNum = *(pbytMacAddr + 2)
         + (szData[GUID_ENCRYPT_11][GUID_ENCRYPT_12]
         * szData[GUID_ENCRYPT_21][GUID_ENCRYPT_22])
         - szData[GUID_ENCRYPT_31][GUID_ENCRYPT_32];
  sprintf(szEncrypted, "%04X", uNum);
  szData[GUID_STORE_31][GUID_STORE_32] = szEncrypted[0];
  szData[GUID_STORE_33][GUID_STORE_34] = szEncrypted[1];
  szData[GUID_STORE_35][GUID_STORE_36] = szEncrypted[2];
  szData[GUID_STORE_37][GUID_STORE_38] = szEncrypted[3];

  // Byte 4 of MAC address
  uNum = *(pbytMacAddr + 3)
         + (szData[GUID_ENCRYPT_11][GUID_ENCRYPT_12]
         * szData[GUID_ENCRYPT_21][GUID_ENCRYPT_22])
         - szData[GUID_ENCRYPT_31][GUID_ENCRYPT_32];
  sprintf(szEncrypted, "%04X", uNum);
  szData[GUID_STORE_41][GUID_STORE_42] = szEncrypted[0];
  szData[GUID_STORE_43][GUID_STORE_44] = szEncrypted[1];
  szData[GUID_STORE_45][GUID_STORE_46] = szEncrypted[2];
  szData[GUID_STORE_47][GUID_STORE_48] = szEncrypted[3];

  // Byte 5 of MAC address
  uNum = *(pbytMacAddr + 4)
         + (szData[GUID_ENCRYPT_11][GUID_ENCRYPT_12]
         * szData[GUID_ENCRYPT_21][GUID_ENCRYPT_22])
         - szData[GUID_ENCRYPT_31][GUID_ENCRYPT_32];
  sprintf(szEncrypted, "%04X", uNum);
  szData[GUID_STORE_51][GUID_STORE_52] = szEncrypted[0];
  szData[GUID_STORE_53][GUID_STORE_54] = szEncrypted[1];
  szData[GUID_STORE_55][GUID_STORE_56] = szEncrypted[2];
  szData[GUID_STORE_57][GUID_STORE_58] = szEncrypted[3];

  // Byte 6 of MAC address
  uNum = *(pbytMacAddr + 5)
         + (szData[GUID_ENCRYPT_11][GUID_ENCRYPT_12]
         * szData[GUID_ENCRYPT_21][GUID_ENCRYPT_22])
         - szData[GUID_ENCRYPT_31][GUID_ENCRYPT_32];
  sprintf(szEncrypted, "%04X", uNum);
  szData[GUID_STORE_61][GUID_STORE_62] = szEncrypted[0];
  szData[GUID_STORE_63][GUID_STORE_64] = szEncrypted[1];
  szData[GUID_STORE_65][GUID_STORE_66] = szEncrypted[2];
  szData[GUID_STORE_67][GUID_STORE_68] = szEncrypted[3];
  }