#ifndef HEX_ENCODE_H
#define HEX_ENCODE_H

#include <string>

void hex_encode(const unsigned char* from, size_t size, unsigned char* to);
void hex_decode(const unsigned char* from, size_t size, unsigned char* to);
std::string hex_encode(const char* from, size_t size);
void hex_decode(std::string in, char* to);

#endif // HEX_ENCODE_H
