#include "LocalUser.h"
    
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <ctime>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/blowfish.h>
#include <openssl/bn.h>

#include "../common/Key.h"

using namespace std;

LocalUser::LocalUser(std::string login)
 : _login(login), _registered(false)
{
    _privKey = RSA_new();
}


RSA* LocalUser::getPrivateKey() {
    return _privKey;
}

void LocalUser::auth(std::string ip)
{
    //generate temp blowfish key
    unsigned char* key = Key(16).getKey();

    //encrypt temp key with ac's public key
    FILE* pkf = fopen("ca_publickey.pem", "r");
    
    if(pkf == NULL) {
        throw std::exception();
    }

    RSA* acKey;
    acKey = PEM_read_RSA_PUBKEY(pkf, NULL, NULL, NULL);
    
    if(acKey == NULL) {
        throw std::exception();
    }
    fclose(pkf);

    stringstream command;
    command << "AUTH " << _login;
    do {
        command.put(' ');
    } while(command.str().length() != 21);

    command << " " << key;

    unsigned char* encryptedCommand = new unsigned char[RSA_size(acKey)];
    RSA_public_encrypt(38, reinterpret_cast<const unsigned char*>(command.str().c_str()), encryptedCommand, acKey, RSA_PKCS1_OAEP_PADDING);

    //send command AUTH

    int acSock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    serverAddr.sin_port = htons(65535);

    if(connect(acSock, (sockaddr*)&serverAddr, sizeof(sockaddr)) != 0) {
        cerr << "Connect error" << endl;
        throw std::exception();
    }

    if(send(acSock, encryptedCommand, RSA_size(acKey), 0) < 0) {
        cerr << "Send error" << endl;
        throw std::exception();
    }

    //retrieve own certificate, verify that it's from ac
    
    char* buffer = new char[512];
    if(recv(acSock, buffer, 512, 0) < 0) {
        throw std::exception();
    }
    
    char* privKey = new char[512];
    char* d = new char[257];
    char* n = new char[257];
    memset(d, 257, 0);
    memset(n, 257, 0);

    BF_KEY *blowKey = new BF_KEY;
    BF_set_key(blowKey, 16, key);
    unsigned char ivec[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int num = 0;
    BF_cfb64_encrypt(reinterpret_cast<unsigned char*>(buffer), reinterpret_cast<unsigned char*>(privKey), 512, blowKey, ivec, &num, BF_DECRYPT);
    
    strncpy(d, privKey, 256);
    strncpy(n, privKey+256, 256);

    BN_hex2bn(&_privKey->d, d);
    BN_hex2bn(&_privKey->n, n);
    BN_dec2bn(&_privKey->e, "65537");

//  cout << BN_bn2hex(_privKey->d) << endl;

    delete privKey;
    delete d;
    delete n;
    delete buffer;
    delete key;
    delete encryptedCommand;
    _registered = true;
}
