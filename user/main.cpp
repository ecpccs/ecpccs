#include <iostream>

#include "LocalUser.h"
#include "MessageListener.h"
#include "Messenger.h"

void usage(char* exName);

using namespace std;

int main(int argc, char** argv)
{
    if(argc != 2) {
        usage(argv[0]);
        return 0;
    }

    LocalUser user(argv[1]);
    
    try {
        user.auth();
    }
    catch(const std::exception& e) {
        cerr << e.what() << endl;
        throw e;
    }

    MessageListener listener;
    listener.listen();

    Messenger messenger;
    std::string command;
    while(true) {
        getline(cin, command);
        size_t first_space = command.find_first_of(' ');
        std::string target = command.substr(0, first_space);
        std::string message = command.substr(first_space+1);
        
        cout << "Commande : " << command << endl;
        cout << "message : " << message << endl;
        cout << "target : " << target << endl;

        command = "";
    }
}

void usage(char* exName)
{
    cout << "Usage: " << exName << " <login>" << endl;
}