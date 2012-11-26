#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include <string>

#include <netinet/in.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>

#include <cstdio>
#include <cstring>

struct Certificate {
    char name[16];
    char ip[4];
    char pKey[128];

    Certificate(std::string name, in_addr* addr, RSA* rsa) {
        memset(this->name, 16, 0);
        strncpy(this->name, name.c_str(), name.size());
        strncpy(this->ip, (const char*)addr, 4);
        char* key = BN_bn2hex(rsa->e);
        memset(this->pKey, 128, 0);
        strncpy(this->pKey, key, strlen(key));
        OPENSSL_free(key);
    }

    in_addr getIp() {
        in_addr addr;
        strncpy(this->ip, (const char*)&addr, 4);
    }

    RSA* getPublicKey() {
        RSA* rsa = RSA_new();
        prsa->n = NULL;
        prsa->e = NULL;
        prsa->d = NULL;
        prsa->p = NULL;
        prsa->q = NULL;
        prsa->dmp1 = NULL;
        prsa->dmq1 = NULL;
        prsa->iqmp = NULL;
        BN_hex2bn(&rsa->e, this->pKey);
        BN_dec2bn(&rsa->n, "65537");
    }
};

#endif //!CERTIFICATE_H
