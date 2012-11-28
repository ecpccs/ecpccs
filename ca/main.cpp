#include <iostream>

#include "CertificateAuthority.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <cstdio>

using namespace std;

int main(int argc, char* argv[]) {

  FILE* file = fopen("ca_key.pem", "r");
  if(file == NULL) {
      cerr << "Cannot open cert.pem" << endl;
      return 1;
  }
  RSA* key = PEM_read_RSAPrivateKey(file, NULL, NULL, NULL);


  CertificateAuthority * ca = new CertificateAuthority(key);
  pthread_t thread;
  pthread_create(&thread, NULL, CertificateAuthority::serverThread, ca);

  for(;;);
  return 0;

}
