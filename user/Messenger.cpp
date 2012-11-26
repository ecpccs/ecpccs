#include "Messenger.h"

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

#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/pem.h>

using namespace std;

Messenger::Messenger(std::string login, std::string ip)
 : _user(login), _authIp(ip)
{
    try {
        _user.auth(_authIp);
    }
    catch(const std::exception& e) {
        cerr << e.what() << endl;
        throw e;
    }
}

void Messenger::retrieveRemoteUser(std::string login)
{
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
    command << "RETR " << login;
    do {
        command.put(' ');
    } while(command.str().length() != 38);

    unsigned char* encryptedCommand = new unsigned char[RSA_size(acKey)];
    RSA_public_encrypt(38, reinterpret_cast<const unsigned char*>(command.str().c_str()), encryptedCommand, acKey, RSA_PKCS1_OAEP_PADDING);

    int acSock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(_authIp.c_str());
    serverAddr.sin_port = htons(65535);

    if(connect(acSock, (sockaddr*)&serverAddr, sizeof(sockaddr)) != 0) {
        cerr << "Connect error" << endl;
        throw std::exception();
    }

    if(send(acSock, encryptedCommand, 128, 0) < 0) {
        cerr << "Send error" << endl;
        throw std::exception();
    }

    Certificate* cert = static_cast<Certificate*>(malloc(sizeof(Certificate)));
    if(recv(acSock, cert, sizeof(Certificate), 0) < 0) {
        throw std::exception();
    }

    //cout << cert->name << endl;
    //cout << cert->pKey << endl;
    //cout << cert->ip << endl;

    unsigned char* signature = new unsigned char[128];
    if(recv(acSock, signature, 128, 0) < 0) {
        throw std::exception();
    }

    unsigned char* hash = new unsigned char[20];
    RSA_public_decrypt(128, signature, hash, acKey, RSA_PKCS1_PADDING);

    cout << hash << endl;

    unsigned char* testHash = new unsigned char[20];
    SHA1(reinterpret_cast<unsigned char*>(cert), sizeof(Certificate), testHash);
    cout << testHash << endl;
    if(strncmp((char*)hash, (char*)testHash, 20)) {
        cerr << "Bad signature !" << endl;
        return;
    }


    _remoteUsers.insert(std::make_pair(login, cert));

    cout << "success !" << endl;


    RSA* pubKey = cert->getPublicKey();
    RSA_print(NULL, pubKey, 0);

    RSA* privKey = _user.getPrivateKey();
    unsigned char b1[128], b2[13];
    RSA_public_encrypt(12, (const unsigned char*)"Hello world!", b1, pubKey, RSA_PKCS1_OAEP_PADDING);
    RSA_private_decrypt(128, b1, b2, privKey, RSA_PKCS1_OAEP_PADDING);
    cout << b2 << endl;

    

    
}
