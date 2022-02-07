#pragma once
#include "../corpusmanager/corpusmanager.h"

class Mutator{
    public:
        Mutator(CorpusManager*);
        ImageBytes* get_mutated_sample();
    private:
        CorpusManager* corpus_manager;
};