#include "LocalUser.h"
    
#include <iostream>

#include "../common/Key.h"

using namespace std;

LocalUser::LocalUser(std::string login)
 : _login(login), _registered(false)
{
}

void LocalUser::auth()
{
    //generate temp blowfish key
    unsigned char* key = Key(16).getKey();

    //encrypt temp key with ac's public key
    

    //send command AUTH

    //retrieve own certificate, verify that it's from ac
}
