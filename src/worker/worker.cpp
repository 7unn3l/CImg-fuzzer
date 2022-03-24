#include "conf/fuzzer_conf.h"
#include "random/random.h"
#include "corpusmanager/corpusmanager.h"
#include "mutator/mutator.h"
#include "runner/runner.h"
#include "communicator/communicator.h"

#ifdef fuzzer_local_perf_log

#include <chrono>
int samples_processed = 0;
std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
#define perf_log() samples_processed+=1;if (samples_processed % 100 == 0){;log_performance();}

void log_performance(){
    double delta = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now()- begin).count();
    printf("%f samples/s\n",samples_processed/delta);
}

#else
#define perf_log()
#endif

void setup(){
    randomgen::seedrand();

    // avoid printing exception messages to stdout, still allowes
    // the library to throw them.    
    cimg_library::cimg::exception_mode(0);

}

int main(int argc, char* argv[]){

    if (argc < 3){
        printf("please supply id and corpus. use worker <id> <corpus dir>\n");
        exit(1);
    }
    std::string id = argv[1];
    std::string corpus_dir = argv[2];
    

    CorpusManager cm{corpus_dir};
    Mutator mu{&cm};
    Runner rn{};
    Communicator com{};
    com.setup(id,cm);

    setup();

    com.wait_for_controller();

    while (true){
        perf_log();
        mu.set_mutated_sample(com);
        rn.run_one_sample(com);
    }
}