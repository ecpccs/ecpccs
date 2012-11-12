#ifndef CERTIFICATE_AUTHORITY_H
#define CERTIFICATE_AUTHORITY_H

#include <pthread>
#include <map>
#include "../common/Certificate.h"

class CertificateAuthority 
{

  public:
    static void* serverThread(void* arg);
    static void* clientThread(void *arg);

  private:
    std::map<std::string, Certificate> _register; 

};

#endif //!CERTIFICATE_AUTHORITY_H
