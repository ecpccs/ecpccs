#ifndef LOCALUSER_H
#define LOCALUSER_H

#include <string>

#include <openssl/rsa.h>

class LocalUser
{
    public:
        LocalUser(std::string login);
        
        void auth(std::string ip);

        RSA* getPrivateKey();
        inline std::string getLogin() { return _login; }

    private:
        std::string _login;
        bool _registered;
        RSA* _privKey;
};

#endif
