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

using namespace std;

struct ClientHandler {
    ClientHandler(int socket, CertificateAuthority* ca_) : clientSocket(socket), ca(ca_) {}
    int clientSocket;
    CertificateAuthority* ca;
};


CertificateAuthority::CertificateAuthority(RSA* cert)
 : _certificate(cert) {
    if(_certificate == NULL) {
        cerr << "Invalid certificate !" << endl;
    }
    else {
        int size = RSA_size(_certificate);
        cerr << "Loaded valid RSA with key length of " << size << endl;
    }
}

void* CertificateAuthority::serverThread(void* arg) {
    CertificateAuthority* ca = reinterpret_cast<CertificateAuthority*>(arg);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(65535);

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

    listen(serverSocket, 10);

    while(true) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        cout << "New client connected with socket " << clientSocket << endl;
        ClientHandler* handler = new ClientHandler(clientSocket, ca);
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

    char buffer[38];
    recv(clientSocket, buffer, 38, 0);
    char decryptedBuffer[38];
    int res = RSA_private_decrypt(38, (unsigned char*)buffer, (unsigned char*)decryptedBuffer, ca->_certificate, RSA_PKCS1_OAEP_PADDING);
    if(res < 38) {
        cout << "Too short request received (" << res << ")!" << endl;
        close(clientSocket);
        pthread_exit(NULL);
    }
    string request = decryptedBuffer;
    string req = request.substr(0, 4);
    string login = request.substr(5, 16);
    cout << "Received request [" << req << "] from \"" << login << "\"" << endl;
    if(req == "AUTH") {
        string key = request.substr(22, 16);
        cout << "Received blowfish key : " << key << endl;
        RSA* rsa = RSA_generate_key(1024, 65537, NULL, NULL);
        char* d = BN_bn2hex(rsa->d);

        BF_KEY* blowfish = new BF_KEY;
        BF_set_key(blowfish, 16, (unsigned char*)key.c_str());
        char cryptedKey[128];
        unsigned char ivec[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        int num = 0;
        BF_cfb64_encrypt((unsigned char*)d, (unsigned char*)cryptedKey, 128, blowfish, ivec, &num, BF_ENCRYPT);

        send(clientSocket, cryptedKey, 128, 0);


    }
    else if(req == "RETR") {
        
        map<string, Certificate>::const_iterator it = ca->_register.find(login);
        if(it == ca->_register.end()) return NULL; /* TO DO STH */
//        send(
        
    }

    return NULL;
}
