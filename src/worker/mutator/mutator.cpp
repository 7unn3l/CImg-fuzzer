#include "../conf/fuzzer_conf.h"
#include "mutator.h"
#include "../corpusmanager/corpusmanager.h"
#include "../random/random.h"

Mutator::Mutator(CorpusManager* cm) : corpus_manager{cm} {}

void Mutator::set_mutated_sample(Communicator& com){
    ImageBytes* sample = corpus_manager->get_random_sample();
    
    com.update(sample);

    int num_changes =  1 + randomgen::xorshf96() % fuzzer_num_max_mutations % sample->sz;

    for (int i=0;i<num_changes;i++){
        int rindex = randomgen::xorshf96() % sample->sz;

        // ignore 2 byte header
        com.shm_buf[2+rindex] = randomgen::xorshf96() % 256;

    }

}