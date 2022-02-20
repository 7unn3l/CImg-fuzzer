#include "runner.h"
#include "../conf/CImg_conf.h"
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

    if (EXTENSION("bmp")){
        img._load_bmp(vfile,"");
    }
    else if (EXTENSION("ppm") || EXTENSION("pgm")){
        img._load_pnm(vfile,"");
    }
    fclose(vfile);
}
