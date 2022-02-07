#include <cstdlib>
#include "string.h"
#include "mutator.h"
#include "../corpusmanager/corpusmanager.h"
#include "../random/random.h"

Mutator::Mutator(CorpusManager* cm) : corpus_manager{cm} {}

ImageBytes* Mutator::get_mutated_sample(){
    ImageBytes* sample = corpus_manager->get_random_sample();
    ImageBytes* mutated = new ImageBytes;
    
    mutated->sz = sample->sz;
    mutated->data = (byte*)malloc(sample->sz);
    
    memcpy(mutated->data,sample->data,sample->sz);

    int num_changes = randomgen::xorshf96() % 9;

    for (int i=0;i<num_changes;i++){
        int rindex = randomgen::xorshf96() % sample->sz;
        mutated->data[rindex] = randomgen::xorshf96() % 256;

    }
    return mutated;

}