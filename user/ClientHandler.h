#ifndef MESSAGELISTENER_H
#define MESSAGELISTENER_H

#include <pthread.h>
#include <arpa/inet.h>
class ClientListener;

class ClientHandler
{
    public: 
        ClientHandler(int socket, ClientListener* listener_, sockaddr_in addr_) : clientSocket(socket), listener(listener_), addr(addr_) {}
        void start();

    private:
        static void* thread(void* arg);

    private:    
        int clientSocket;
        ClientListener* listener;
        sockaddr_in addr;
};

#endif
