#pragma once
#include "cstdint"


namespace randomgen
{
    extern uint_fast32_t x,y,z;
    void seedrand();
    uint_fast32_t xorshf96();
}