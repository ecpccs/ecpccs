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
#include <errno.h>

#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include "ClientListener.h"
#include "Message.h"

using namespace std;

Messenger::Messenger(std::string login, std::string ip)
 : _user(login), _authIp(ip)
{    
    if(login.size() > 16) {
        throw std::exception();
    }

    try {
        _user.auth(_authIp);
    }
    catch(const std::exception& e) {
        cerr << e.what() << endl;
        throw e;
    }
}

void Messenger::listen(unsigned int port)
{
    ClientListener listener(this, port);
    listener.start();
}

void Messenger::retrieveRemoteUser(std::string login)
{
    if(login.size() > 16) {
        throw std::exception();
    }
    
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

    unsigned char* signature = new unsigned char[128];
    if(recv(acSock, signature, 128, 0) < 0) {
        throw std::exception();
    }

    unsigned char* hash = new unsigned char[20];
    RSA_public_decrypt(128, signature, hash, acKey, RSA_PKCS1_PADDING);

    unsigned char* testHash = new unsigned char[20];
    SHA1(reinterpret_cast<unsigned char*>(cert), sizeof(Certificate), testHash);
    if(strncmp((char*)hash, (char*)testHash, 20)) {
        cerr << "Bad signature !" << endl;
        return;
    }


    _remoteUsers.insert(std::make_pair(login, *cert));
    delete cert;

    //RSA* pubKey = cert->getPublicKey();

    //RSA* privKey = _user.getPrivateKey();
    ////privKey->e = pubKey->e; 
    //unsigned char b1[128], b2[13];
    //int size = RSA_public_encrypt(13, (const unsigned char*)"Hello world!", b1, pubKey, RSA_PKCS1_OAEP_PADDING); 
    //int size2 = RSA_private_decrypt(128, b1, b2, privKey, RSA_PKCS1_OAEP_PADDING);

    //ERR_load_crypto_strings();
    //cout << size << " " << ERR_error_string(ERR_get_error(), NULL) << endl;

    //cout << b2 << endl;
}

void Messenger::sendTo(std::string login, std::string message) {
    Certificate cert = this->findUser(login);
    Message msg(this->_user.getLogin(), this->_user.getPrivateKey(), login.c_str(), cert.getPublicKey(), message.c_str());

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr = cert.getIp();
    addr.sin_port = htons(65534);
    char* aip = inet_ntoa(cert.getIp());
    string ip = aip ? aip : "Invalid";

    if(connect(sock, (sockaddr*)&addr, sizeof(sockaddr)) < 0) {
        cerr << "Failed to connect to " << login << "@" <<  ip << ":" << strerror(errno) << endl;
        throw std::exception();
    }

    string data = msg.toXml();
    int size = data.size();

    if(send(sock, &size, 4, 0) < 0) {
        cerr << "Failed to send data to " << login << "@" <<  ip << ":" << strerror(errno) << endl;
        throw std::exception();
    }
    if(send(sock, data.c_str(), data.size(), 0) < 0) {
        cerr << "Failed to send data to " << login << "@" <<  ip << ":" << strerror(errno) << endl;
        throw std::exception();
    }
}

Certificate Messenger::findUser(std::string u) {
    map<string, Certificate>::const_iterator it = _remoteUsers.find(u);
    if(it == _remoteUsers.end()) {
        this->retrieveRemoteUser(u);
        map<string, Certificate>::const_iterator it = _remoteUsers.find(u);
        if(it == _remoteUsers.end()) {
            throw exception();
        }
    }
    return it->second;
}
