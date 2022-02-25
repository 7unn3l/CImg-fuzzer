#include "random/random.h"
#include "corpusmanager/corpusmanager.h"
#include "mutator/mutator.h"
#include "runner/runner.h"

int main(int argc, char* argv[]){
    CorpusManager cm{};
    Mutator mu{&cm};
    Runner rn{};

    randomgen::seedrand();

    // avoid printing exception messages to stdout, still allowes
    // the library to throw them.
    cimg_library::cimg::exception_mode(0);

    while (true){
        auto sample = mu.get_mutated_sample();
        rn.run_one_sample(sample);
        free(sample->data);
        delete sample;
    }

}