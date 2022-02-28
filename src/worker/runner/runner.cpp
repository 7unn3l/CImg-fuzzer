#include "runner.h"
#include "../conf/CImg_conf.h"
#include "../conf/fuzzer_conf.h"
#include "../log/log.h"
#include "../../CImg/CImg.h"

void Runner::run_one_sample(Communicator& com){
    cimg_library::CImg<unsigned char> img;
    
    try{
        load_fileformat(com,img);
    }catch(cimg_library::CImgIOException){

    }catch(cimg_library::CImgInstanceException){
        
    }
    
}

void Runner::load_fileformat(Communicator& com, cimg_library::CImg<unsigned char>& img){

    FILE* vfile = fmemopen(com.shm_buf+2,com.shm_buf_sz,"rw");

    // no case statements with strings? Nice typesystem, c++

    #define EXTENSION(x) com.current_extension.compare(x) == 0
    #define LOAD(x) img._load_##x(vfile,"")

    if (EXTENSION("bmp")){
        LOAD(bmp);
    }
    else if (EXTENSION("ppm") || EXTENSION("pgm")){
        LOAD(pnm);
    }else if (EXTENSION("ascii")){
        LOAD(ascii);
    }
    else{
        LOG("[-] no extension handler defined for extension %s",com.current_extension.c_str());
        exit(1);
    }
    fclose(vfile);
}
