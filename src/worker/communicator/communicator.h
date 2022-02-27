#pragma once
#include "../corpusmanager/corpusmanager.h"
#include <unistd.h>

class Communicator{
    public:
        uint8_t* shm_buf;
        int shm_buf_sz;
        void setup(int uid,const CorpusManager &cm);
};