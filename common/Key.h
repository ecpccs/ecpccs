#ifndef KEY_H
#define KEY_H

#include <fstream>

class Key
{
    public:
        Key(size_t keySize)
        {
            _key = new unsigned char[keySize];
            
            for(unsigned int i = 0 ; i < keySize ; ++i) {
                _key[i] = static_cast<double>(rand()) * 256. / (static_cast<double>(RAND_MAX)+1.); 
                //_key[i] = i+65;
            }
        }

        unsigned char* getKey()
        {
            return _key;
        }

    private:
        unsigned char* _key;
};

#endif
