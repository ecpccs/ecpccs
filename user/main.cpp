#include <iostream>

#include "Messenger.h"

void usage(char* exName);

using namespace std;

int main(int argc, char** argv)
{
    if(argc != 3) {
        usage(argv[0]);
        return 0;
    }
            
    srand(time(0));
    
    Messenger messenger(argv[1], argv[2]);
    messenger.listen();

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
    cout << "Usage: " << exName << " <login> <ca ip>" << endl;
}
