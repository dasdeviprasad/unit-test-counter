#pragma once

#define SECURE_INSTALL_FILE "ValidateInstall.txt"
#define SECURE_INSTALL_FILE_BACKUP "ValidateInstall.bak"
#define INSTALL_PASSWORD "3cpm"

#define DATA_ARRAY_SIZE_1 6   // size of the first dimension of the data array.
#define DATA_ARRAY_SIZE_2 50  // size of the second dimension of the data array.


class CSecureInstall
  {
  public:
    CSecureInstall(void);
    virtual ~CSecureInstall(void);

  public: // Methods
    bool bRead_file();
    bool bWrite_file();
    bool bValidate_password();

  };
