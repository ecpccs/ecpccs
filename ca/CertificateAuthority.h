#ifndef CERTIFICATE_AUTHORITY_H
#define CERTIFICATE_AUTHORITY_H

#include <pthread.h>
#include <map>
#include "../common/Certificate.h"
#include <openssl/rsa.h>

class CertificateAuthority 
{

  public:
    CertificateAuthority(RSA* cert);

    static void* serverThread(void* arg);
    static void* clientThread(void *arg);

  private:
    std::map<std::string, Certificate> _register; 
    RSA* _certificate;
};

#endif //!CERTIFICATE_AUTHORITY_H
