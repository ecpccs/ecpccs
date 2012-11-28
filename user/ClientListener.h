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

class ClientListener
{
    public:
        ClientListener(unsigned int port = 65535) : _port(port) {};
        void start();

    private:
        static void* thread(void* listener);
        
    private:
        unsigned int _port;
};

#endif
