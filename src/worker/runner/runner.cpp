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

    FILE* vfile = fmemopen(com.shm_buf+2,com.current_samplesize,"rw");

    // no case statements with strings? Nice typesystem, c++

    #define EXTENSION(x) com.current_extension.compare(x) == 0
    #define LOAD(x) img._load_##x(vfile,"")

    if (EXTENSION("ascii")){
        LOAD(ascii);
    }
    else if (EXTENSION("hdr") || EXTENSION("nii")){
        img._load_analyze(vfile,"",0);
    }
    else if(EXTENSION("inr")){
        img._load_inr(vfile,"",0);
    }
    else if (EXTENSION("pnm") || EXTENSION("ppm") || EXTENSION("pgm")){
        LOAD(pnm);
    }
    else if (EXTENSION("bmp")){
        LOAD(bmp);
    }
    else if (EXTENSION("pan") || EXTENSION("pandore")){
        LOAD(pandore);
    }
    else if (EXTENSION("dlm")){
        LOAD(dlm);
    }
    else if (EXTENSION("jpeg") || EXTENSION("jpg")){
        LOAD(jpeg);
    }
    else if (EXTENSION("png")){
        img._load_png(vfile,"",0);
    }
    else{
        LOG("[-] no extension handler defined for extension %s",com.current_extension.c_str());
        exit(1);
    }
    fclose(vfile);
}
