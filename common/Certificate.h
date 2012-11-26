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
        char* key = BN_bn2hex(rsa->n);
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
        rsa->n = NULL;
        rsa->e = NULL;
        rsa->d = NULL;
        rsa->p = NULL;
        rsa->q = NULL;
        rsa->dmp1 = NULL;
        rsa->dmq1 = NULL;
        rsa->iqmp = NULL;
        char key[129];
        strncpy(key, pKey, 128);
        key[128] = 0;
        std::cout << key << std::endl;
        int res = BN_hex2bn(&rsa->e, key);
        std::cout << res << std::endl;
        if(res == 0) {
            std::cerr << "BN conversion error" << std::endl;
        }
        res = BN_dec2bn(&rsa->n, "65537");
        std::cout << res << std::endl;
        if(res == 0) {
            std::cerr << "BN conversion error" << std::endl;
        }
    }
};

#endif //!CERTIFICATE_H
