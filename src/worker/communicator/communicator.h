#pragma once
#include "../corpusmanager/corpusmanager.h"
#include <unistd.h>

class Communicator{
    public:
        uint8_t* shm_buf;
        int shm_buf_sz;
        std::string current_extension; // safes complicated extraction
        void setup(std::string& id,const CorpusManager &cm);
        void update(ImageBytes* sample);
};