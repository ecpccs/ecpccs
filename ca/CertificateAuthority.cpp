#include "CertificateAuthority.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <ctime>
#include <iostream>

#include <openssl/rsa.h>
#include <openssl/blowfish.h>
#include <openssl/pem.h>
#include <openssl/sha.h>

using namespace std;

struct ClientHandler {
    ClientHandler(int socket, CertificateAuthority* ca_, sockaddr_in addr_) : clientSocket(socket), ca(ca_), addr(addr_) {}
    int clientSocket;
    CertificateAuthority* ca;
    sockaddr_in addr;
};


CertificateAuthority::CertificateAuthority(RSA* cert)
 : _certificate(cert) {
    if(_certificate == NULL) {
        cerr << "Invalid certificate !" << endl;
    }
    else {
        int size = RSA_size(_certificate);
        cout << "Loaded valid RSA with key length of " << size << endl;
    }
}

void* CertificateAuthority::serverThread(void* arg) {
    CertificateAuthority* ca = reinterpret_cast<CertificateAuthority*>(arg);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(65535);

    int res = bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if(res != 0) {
        cout << "Cannot bind !" << endl;
    }

    res = listen(serverSocket, 10);
    if(res != 0) {
        cout << "Cannot listen !" << endl;
    }

    while(true) {
        sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int clientSocket = accept(serverSocket, (sockaddr*)&addr, &len);
        cout << "New client connected with socket " << clientSocket << endl;
        ClientHandler* handler = new ClientHandler(clientSocket, ca, addr);
        pthread_t thread;
        pthread_create(&thread, NULL, CertificateAuthority::clientThread, static_cast<void*>(handler));

    }

    pthread_exit(NULL);

}

void* CertificateAuthority::clientThread(void *arg) {
    ClientHandler* handler = static_cast<ClientHandler*>(arg);
    int clientSocket = handler->clientSocket;
    CertificateAuthority* ca = handler->ca;

    cout << "clientThread started for socket " << clientSocket << endl;

    char buffer[128];
    recv(clientSocket, buffer, 128, 0);
    char decryptedBuffer[38];
    int res = RSA_private_decrypt(128, (unsigned char*)buffer, (unsigned char*)decryptedBuffer, ca->_certificate, RSA_PKCS1_OAEP_PADDING);
    if(res < 38) {
        cout << "The decrypted request size is too short (" << res << " < 38)!" << endl;
        close(clientSocket);
        pthread_exit(NULL);
    }
    string request = decryptedBuffer;
    string req = request.substr(0, 4);
    string login = request.substr(5, 16);
    cout << "Received request [" << req << "] from \"" << login << "\"" << endl;
    if(req == "AUTH") {
        char key[16];
        strncpy(key, decryptedBuffer+22, 16);

        cout << "Received blowfish key : " << key << endl;
        RSA* rsa = RSA_generate_key(1024, 65537, NULL, NULL);
        char* d = BN_bn2hex(rsa->d);
        char* n = BN_bn2hex(rsa->n);

        char rsaKey[512];
        memset(rsaKey, 512, 0);
        strncpy(rsaKey, d, strlen(d));
        strncpy(rsaKey+256, n, strlen(n));

        OPENSSL_free(d);
        OPENSSL_free(n);

        BF_KEY* blowfish = new BF_KEY;
        BF_set_key(blowfish, 16, (unsigned char*)key);
        char cryptedKey[512];
        unsigned char ivec[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        int num = 0;
        BF_cfb64_encrypt((unsigned char*)rsaKey, (unsigned char*)cryptedKey, 512, blowfish, ivec, &num, BF_ENCRYPT);

        send(clientSocket, cryptedKey, 512, 0);
        Certificate newCert(login, &handler->addr.sin_addr, rsa);

        map<string, Certificate>::iterator it = ca->_register.find(login);
        if(it != ca->_register.end()) {
            ca->_register.erase(it);
        }
        ca->_register.insert(std::pair<string, Certificate>(login, newCert));




    }
    else if(req == "RETR") {
        
        map<string, Certificate>::const_iterator it = ca->_register.find(login);
        if(it == ca->_register.end()) {
            cout << "User '" << login << "'not found" << endl;; /* TO DO STH */
        }
        Certificate cert = it->second;
        unsigned char digest[20];
        cout << cert.name << endl << cert.ip << endl << cert.modulus << endl;
        SHA1((unsigned char*)&cert, sizeof(cert), digest);
        cout << digest << endl;
        unsigned char encDigest[128];
        RSA_private_encrypt(20, digest, encDigest, ca->_certificate, RSA_PKCS1_PADDING);
        send(clientSocket, &cert, sizeof(cert), 0);
        send(clientSocket, encDigest, 128, 0);
    }

    close(clientSocket);
    pthread_exit(NULL);
}
