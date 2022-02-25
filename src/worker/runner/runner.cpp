#include "runner.h"
#include "../conf/CImg_conf.h"
#include "../conf/fuzzer_conf.h"
#include "../log/log.h"
#include "../../CImg/CImg.h"

void Runner::run_one_sample(ImageBytes* sample){
    cimg_library::CImg<unsigned char> img;
    
    try{
        load_fileformat(sample,img);
    }catch(cimg_library::CImgIOException){

    }catch(cimg_library::CImgInstanceException){
        
    }
    
}

void Runner::load_fileformat(ImageBytes* sample, cimg_library::CImg<unsigned char>& img){

    FILE* vfile = fmemopen(sample->data,sample->sz,"rw");

    // no case statements with strings? Nice typesystem, c++

    #define EXTENSION(x) sample->fileformat.compare(x) == 0
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
        LOG("[-] no extension handler defined for extension %s",sample->fileformat.c_str());
        exit(1);
    }
    fclose(vfile);
}
