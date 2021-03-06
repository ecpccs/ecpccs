#ifndef MESSENGER_H
#define MESSENGER_H

#include <iostream>
#include <string>
#include <map>

#include "LocalUser.h"
#include "../common/Certificate.h"

class Messenger
{
    public:
        Messenger(std::string login, std::string authorityIp);
       
        void listen(unsigned int port = 65534);

        void retrieveRemoteUser(std::string login);
        void sendTo(std::string login, std::string message);

        inline LocalUser getLocalUser() const { return _user; }
        Certificate findUser(std::string u);

    private:
        std::map<std::string, Certificate> _remoteUsers;
        std::string _authIp;
        LocalUser _user;
};

#endif
