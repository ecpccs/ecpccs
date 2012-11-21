#include "LocalUser.h"
    
#include <iostream>
#include <sstream>
#include <cstdio>

#include <openssl/pem.h>
#include <openssl/rsa.h>

#include "../common/Key.h"

using namespace std;

LocalUser::LocalUser(std::string login)
 : _login(login), _registered(false)
{
}

void LocalUser::auth()
{
    //generate temp blowfish key
    unsigned char* key = Key(16).getKey();

    //encrypt temp key with ac's public key
    FILE* pkf = fopen("ca_publickey.pem", "r");
    
    if(pkf == NULL) {
        throw std::exception();
    }

    RSA* acKey = PEM_read_RSAPublicKey(pkf, NULL, NULL, NULL);
    fclose(pkf);
    
    if(acKey == NULL) {
        throw std::exception();
    }

    stringstream command;
    command << "AUTH " << _login;
    do {
        command.put(0);
    } while(command.str().length() != 21);

    command << " " << key;

    unsigned char* encryptedCommand = new unsigned char[RSA_size(acKey)];
    RSA_public_encrypt(38, reinterpret_cast<const unsigned char*>(command.str().c_str()), encryptedCommand, acKey, RSA_PKCS1_OAEP_PADDING);

    //send command AUTH
    cout << encryptedCommand << endl;


    //retrieve own certificate, verify that it's from ac

    delete key;
}
