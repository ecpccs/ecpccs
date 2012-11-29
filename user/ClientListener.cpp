#include "ClientListener.h"

#include <pthread.h>

#include "ClientHandler.h"

using namespace std;

void ClientListener::start()
{
    pthread_t thread;
    //cout << "port " << this->_port << endl;
    //cout << ios_base::hex << reinterpret_cast<intptr_t>(this) << endl;
    pthread_create(&thread, NULL, ClientListener::thread, this);
}

void* ClientListener::thread(void* arg)
{
    ClientListener* listener = reinterpret_cast<ClientListener*>(arg);
   
    
    //cout << ios_base::hex << reinterpret_cast<intptr_t>(listener) << endl;
    //cout << "listener started on port " << listener->_port << endl;

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(listener->_port);

    int res = bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if(res != 0) {
        cerr << "Cannot bind !" << endl;
    }

    res = listen(serverSocket, 10);
    if(res != 0) {
        cerr << "Cannot listen !" << endl;
    }

    while(true) {
        sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int clientSocket = accept(serverSocket, (sockaddr*)&addr, &len);
        //cout << "New client connected with socket " << clientSocket << endl;
        ClientHandler* handler = new ClientHandler(clientSocket, listener, addr);
        handler->start();
    }

    pthread_exit(NULL);
    
}
