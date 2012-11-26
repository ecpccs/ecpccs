#ifndef MESSENGER_H
#define MESSENGER_H

#include <iostream>
#include <string>
#include <map>

#include "LocalUser.h"
#include "MessageListener.h"
#include "../common/Certificate.h"

class Messenger
{
    public:
        Messenger(std::string login, std::string authorityIp);
        
        void retrieveRemoteUser(std::string login);
        void sendTo(std::string login, std::string message) const;

    private:
        std::map<std::string, Certificate*> _remoteUsers;
        std::string _authIp;
        LocalUser _user;
};

#endif
