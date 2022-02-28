#pragma once
#include "../corpusmanager/corpusmanager.h"
#include "../communicator/communicator.h"

class Mutator{
    public:
        Mutator(CorpusManager*);
        void set_mutated_sample(Communicator& com);
    private:
        CorpusManager* corpus_manager;
};