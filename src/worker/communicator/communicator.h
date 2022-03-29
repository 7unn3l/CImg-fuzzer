#pragma once
#include "../corpusmanager/corpusmanager.h"
#include <unistd.h>

class Communicator{
    public:
        uint8_t* shm_buf;
        int shm_buf_sz;
        std::string current_extension; // safes complicated extraction
        int current_samplesize;
        void setup(std::string& id,const CorpusManager &cm);
        void update(ImageBytes* sample);
        void wait_for_controller();
};