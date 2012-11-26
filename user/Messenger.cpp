#include "Messenger.h"

using namespace std;

Messenger::Messenger(std::string login, std::string ip)
 : _user(login), _authIp(ip)
{
    try {
        _user.auth(_authIp);
    }
    catch(const std::exception& e) {
        cerr << e.what() << endl;
        throw e;
    }
}

void Messenger::retrieveRemoteUser(std::string login)
{
    //if(recv(acSock, &_certificate, sizeof(_certificate), 0) < 0) {
        //throw std::exception();
    //}

    //unsigned char signature[128];
    //if(recv(acSock, &hash, 128, 0) < 0) {
        //throw std::exception();
    //}
    
    //unsigned char hash[20];
    //RSA_public_decrypt(20, &signature, &hash, acKey, RSA_PKCS1_OAEP_PADDING);

    //unsigned char testHash[20];
}
