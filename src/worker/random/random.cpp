#include "random.h"
#include <fstream>

int_fast32_t randomgen::x = 1;
int_fast32_t randomgen::y = 2;
int_fast32_t randomgen::z = 3;

void randomgen::seedrand(){
    std::ifstream f("/dev/urandom",std::ios::binary);
    f.seekg(0,std::ios::beg);
    f.read((char*)&randomgen::x,4);
    f.read((char*)&randomgen::y,4);
    f.read((char*)&randomgen::z,4);
    f.close();
}

int_fast32_t randomgen::xorshf96(){
    int_fast32_t t;
    randomgen::x ^= randomgen::x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    return z;
}
