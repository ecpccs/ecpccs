#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include <string>


struct Certificate {
    unsigned char name[64];
    unsigned char ip[4];
    unsigned char pKey[1024];
}

#endif //!CERTIFICATE_H
