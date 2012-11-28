#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include <string>

#include <netinet/in.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>

#include <cstdio>
#include <cstring>
#include <cmath>
#include <iostream>

struct Certificate {
    char name[16];
    char ip[4];
    char modulus[257];

    Certificate(std::string name, in_addr* addr, RSA* rsa) {
        memset(this->name, 16, 0);
        strncpy(this->name, name.c_str(), std::min((int)name.size(), 16));
        strncpy(this->ip, (const char*)addr, 4);
        char* mod = BN_bn2hex(rsa->n);
        std::cout << "Modulus : " << mod << std::endl;
        memset(this->modulus, 257, 0);
        strncpy(this->modulus, mod, strlen(mod));
        std::cout  << "( size=" <<  strlen(mod) << ")" << std::endl;
        OPENSSL_free(mod);
    }

    in_addr getIp() {
        in_addr addr;
        strncpy(this->ip, (const char*)&addr, 4);
        return addr;
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
        int res = BN_hex2bn(&rsa->n, this->modulus);
        if(res == 0) {
            std::cerr << "BN conversion error" << std::endl;
        }
        res = BN_dec2bn(&rsa->e, "65537");
        if(res == 0) {
            std::cerr << "BN conversion error" << std::endl;
        }
        return rsa;
    }
};

#endif //!CERTIFICATE_H
