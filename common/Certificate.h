#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include <string>


struct Certificate {
    unsigned char name[16];
    unsigned char ip[4];
    unsigned char pKey[128];
};

#endif //!CERTIFICATE_H
