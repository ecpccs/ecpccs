#include <iostream>

#include "CertificateAuthority.h"

int main(int argc, char* argv[]) {

  CertificateAuthority * ca = new CertificateAuthority();
  pthread_t thread;
  pthread_create(&thread, NULL, CertificateAuthority::serverThread, static_cast<void*>(ca));


  for(;;);
  return 0;

}
