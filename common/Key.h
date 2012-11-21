#ifndef KEY_H
#define KEY_H

class Key
{
    public:
        Key(size_t keySize)
        {
            _key = new unsigned char[keySize];
            
            ifstream rf("/dev/urandom");
            unsigned int seed = 0;
            rf >> seed;

            for(unsigned int i = 0 ; i < keySize ; ++i) {
                _key[i] = static_cast<double>(rand()) * 256. / (static_cast<double>(RAND_MAX)+1.); 
            }
        }

        unsigned char* getKey()
        {
            return key;
        }

    private:
        unsigned char* _key;
}

#endif
