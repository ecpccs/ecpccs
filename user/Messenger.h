#ifndef MESSENGER_H
#define MESSENGER_H

#include <string>
#include <map>

#include "RemoteUser.h"

class Messenger
{
    public:
        void retrieveRemoteUser(std::string login);
        void sendTo(std::string login, std::string message) const;
        void broadcast(std::string message) const;

    private:
        std::map<std::string, RemoteUser> _remoteUsers;
};

#endif
