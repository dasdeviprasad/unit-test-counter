/********************************************************************
SecureInstall.cpp

Copyright (C) 2010, 3CPM Company, Inc.  All rights reserved.

  Implementation file for the CSecureInstall class.

  This class provides methods for creating, reading and writing the
  secure data file.


HISTORY:
10-AUG-10  RET  New.
17-SEP-10  RET
                Add method vSet_file_path().
                Change methods: bWrite_file(), bRead_file() to use
                  the full path and file name for the secure data file.
18-SEP-12  RET
             Fix problem in validating the password. It used to compare the
               password from the validate text file with what the password
               is supposed to be not what the user entered.
                 Change method: bValidate_password()
********************************************************************/

#include "StdAfx.h"
#include "Iptypes.h"
#include "Iphlpapi.h"
#include "..\common\SecureInstall.h"

/********************************************************************
Constructor

  Create the encryption/decryption class.
********************************************************************/
CSecureInstall::CSecureInstall(void)
  {
  m_pce = new CEncrypt();
  m_cstrDataFile = SECURE_INSTALL_FILE;
  }

/********************************************************************
Destructor

  Clean up any data or classes.
********************************************************************/
CSecureInstall::~CSecureInstall(void)
  {

  delete m_pce;
  }

/********************************************************************
bCreate_secure_file - Create the secure data file.
                      Encrypt the password into the file.

inputs: None.

return: true if data file was created and the encrypted password was written.
        false if there was an error.
********************************************************************/
bool CSecureInstall::bCreate_secure_file()
  {
  bool bRet;
  CString cstrGUID;
  int iIndex;

  m_cstrErr = "";
  // Generate 6 lines of GUID.
  for(iIndex = 0; iIndex < 6; ++iIndex)
    {
    if((bRet = bCreate_guid(cstrGUID)) == false)
      break;
    strcpy(m_szaGUID[iIndex], (LPCTSTR)cstrGUID);
    }
  if(bRet == true)
    {
    // Encode password in to the 6 lines.
    m_pce->vEncrypt_password(m_szaGUID);
    // Create the secure data file.
    bRet = bWrite_file();
    }
  return(bRet);
  }

/********************************************************************
bCreate_guid - Create a GUID.

inputs: Reference to a CString object that gets the GUID.

return: true if GUID was created.
        false if there was an error.
********************************************************************/
bool CSecureInstall::bCreate_guid(CString &cstrGUID)
  {
	GUID guid;    // generated GUID
  bool bRet = false;

	// create random GUID using UuidCreate so that we 
	// can check for more error codes
	guid = GUID_NULL;
	HRESULT hr = ::UuidCreate(&guid);

	// Errors
	if (HRESULT_CODE(hr) != RPC_S_OK)
		m_cstrErr = "Unable to get a GUID.";
	else if (guid == GUID_NULL)
		m_cstrErr = "Unable to create new GUID.";
	else if (HRESULT_CODE(hr) == RPC_S_UUID_NO_ADDRESS)
		m_cstrErr = "Cannot get the hardware address for this computer.";
	else if (HRESULT_CODE(hr) == RPC_S_UUID_LOCAL_ONLY)
		m_cstrErr = "Warning: Unable to determine your network address.\r\n  The UUID generated is unique on this computer only.\r\n  It should not be used on another computer.";
  else
    {
    bRet = true;
    cstrGUID.Format("%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
   		guid.Data1, guid.Data2, guid.Data3,
  		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
  		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    }
  return(bRet);
  }

/********************************************************************
bRead_file - Read the data file into a class array.

inputs: None.

return: true if the file was read.
        false if there was an error.
********************************************************************/
bool CSecureInstall::bRead_file()
  {
  bool bRet;
  FILE *pf;
  int iIndex, iLen;

  m_cstrErr = "";
  if((pf = fopen(m_cstrDataFile, "r")) == NULL)
    {
    bRet = false;
    m_cstrErr.Format("Failed to open %s.", SECURE_INSTALL_FILE);
    }
  else
    { // Got the file opened.
      // Read each line of the file into a class array.
      // Remove the trailing newline characters.
    for(iIndex = 0; iIndex < 6; ++iIndex)
      {
      fgets(m_szaGUID[iIndex], DATA_ARRAY_SIZE_2, pf);
      // Remove the trailing newline character.
      iLen = (int)strlen(m_szaGUID[iIndex]);
      m_szaGUID[iIndex][iLen - 1] = NULL;
      }
    fclose(pf);
    bRet = true;
    }
  return(bRet);
  }

/********************************************************************
bWrite_file - Write the class array to the data file.

inputs: None.

return: true if the file was written.
        false if there was an error.
********************************************************************/
bool CSecureInstall::bWrite_file()
  {
  bool bRet = false;
  FILE *pf;
  int iIndex;
  CString cstrFileVersion;

  m_cstrErr = "";
  if((pf = fopen(m_cstrDataFile, "w")) == NULL)
    {
    m_cstrErr.Format("Failed to open %s.", SECURE_INSTALL_FILE);
    bRet = false;
    }
  else
    { // Write the GUIDs to the data file.
      // Put in the file version.
    cstrFileVersion.Format("%02X", SECURE_FILE_VERSION);
    m_szaGUID[0][0] = cstrFileVersion.GetAt(0);
    m_szaGUID[0][1] = cstrFileVersion.GetAt(1);
    for(iIndex = 0; iIndex < 6; ++iIndex)
      {
      fputs(m_szaGUID[iIndex], pf);
      fputs("\n", pf);
      }
    fclose(pf);
    bRet = true;
    }
  return(bRet);
  }

/********************************************************************
bValidate_password - Validate the password.  If the password validates
                     get the MAC address and overwrite the password
                     with the MAC address then rewrite the data file.

inputs: CString object containing the password.

return: true if the password is valid.
        false if the password is not valid.
********************************************************************/
bool CSecureInstall::bValidate_password(CString cstrPassword)
  {
  bool bRet = false;

  m_pce->vDecrypt_password(m_szaGUID);
  if(m_pce->bValidate_password(cstrPassword) == true)
    { // Password is valid.
      // If correct, get the MAC address and replace the password with it.
    bChange_password();
    bRet = true;
    }
  return(bRet);
  }

/********************************************************************
bChange_password - Change the password in the file to the MAC address.

inputs: None.

return: true if the password is valid.
        false if the password is not valid.
********************************************************************/
bool CSecureInstall::bChange_password()
  {
  bool bRet = false;
  CString cstrMacAddr;
  IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information for up to 16 NICs
  DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

  // Call GetAdapterInfo ([out] buffer to receive data, [in] size of receive data buffer)
  DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);

   // Get pointer to linked list of current adapter info
  PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

  // Read in the data file.
  if((bRet = bRead_file()) == true)
    {
    // Encrypt the mac address and store in data file.
    m_pce->vEncrypt_mac_address(m_szaGUID, pAdapterInfo->Address);
    bRet = bWrite_file();
    }

  return(bRet);
  }

/********************************************************************
vSet_file_path - Set the path and file name for the secure data file.

inputs: Path name without the trailing backslash.

return: None.
********************************************************************/
void CSecureInstall::vSet_file_path(CString cstrPath)
  {

  if(cstrPath == "")
    m_cstrDataFile = SECURE_INSTALL_FILE;
  else
    m_cstrDataFile = cstrPath + "\\" + SECURE_INSTALL_FILE;
  }

