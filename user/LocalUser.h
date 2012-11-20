#ifndef LOCALUSER_H
#define LOCALUSER_H

#include <string>

class LocalUser
{
    public:
        LocalUser(std::string login);
        
        void auth();

    private:
        std::string _login;
        bool _registered;
};

#endif
