#ifndef CLIENTLISTENER_H
#define CLIENTLISTENER_H

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
#include "ClientHandler.h"

class Messenger;

class ClientListener
{
    public:
        ClientListener(Messenger* messenger, unsigned int port = 65534) : _port(port), _messenger(messenger) {}
        void start();

    private:
        static void* thread(void* listener);
        
    private:
        unsigned int _port;
        Messenger* _messenger;

    friend class ClientHandler;
};

#endif
