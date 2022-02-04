#pragma once
#include "cstdint"


namespace randomgen
{
    extern int_fast32_t x,y,z;
    void seedrand();
    int_fast32_t xorshf96();
}