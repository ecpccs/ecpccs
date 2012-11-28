#include "ClientHandler.h"

#include <iostream>
#include <pthread.h>

#include "Message.h"

using namespace std;

void ClientHandler::start()
{
    pthread_t thread;
    pthread_create(&thread, NULL, ClientHandler::thread, this);
}

void* ClientHandler::thread(void* arg)
{
    ClientHandler* handler = reinterpret_cast<ClientHandler*>(arg);

    int clientSocket = handler->clientSocket;
    ClientListener* listener = handler->listener;

    // cout << "clientThread started for socket " << clientSocket << ". Ready to receive message." << endl;
    
    int mSize;
    if(recv(clientSocket, &mSize, sizeof(int), 0) < 0)
    {
        cerr << "Error receiving message size" << endl;
        close(clientSocket);
        pthread_exit(NULL);
    }

    char* buffer = new char[mSize];
    if(recv(clientSocket, buffer, mSize, 0) < 0)
    {
        cerr << "Error receiving message" << endl;
        close(clientSocket);
        pthread_exit(NULL);
    }

    Message* mess = Message::retrieveMessage(buffer);
    cout << mess->getContent() << endl;
    
    delete[] buffer;
    delete mess;
    close(clientSocket);
    pthread_exit(NULL);
}
