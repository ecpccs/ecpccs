#include "CertificateAuthority.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cunistd>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <ctime>

struct clientHandler {
    int clientSocket;
    CertificateAuthority* ca;
};

static void* CertificateAuthority::serverThread(void* arg) {
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
        

    }

}

static void* CertificateAuthority::clientThread(void *arg) {
    int clientSocket = reinterpret_cast<clientHandler*>(arg)->clientSocket;
    int ca = reinterpret_cast<clientHandler*>(arg)->ca;
    
    char request[86];
    recv(clientSocket, request, 86, 0);
    if(strncmp(request, "AUTH", 4)) {
       
    }
    else if(strncmp(request, "RETR", 4)) {
        char name[64];
        strncpy(name, request + 4, 64);
        
        map<string, Certificate>::const_iterator it = ca->_register.find(name);
        if(it == ca->_register.end()) return; /* TO DO STH */
        send(
        
    }
}
