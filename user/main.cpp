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
        user.registerf();
    }
    catch(const std::exception& e) {
        cerr << e.what() << endl;
        throw e;
    }

    MessageListener listener;
    listener.listen();

    Messenger messenger;
    std::string message;
    while(true) {
        cin >> message;
        cout << message;
        message = "";
    }
}

void usage(char* exName)
{
    cout << "Usage: " << exName << " <login>" << endl;
}
