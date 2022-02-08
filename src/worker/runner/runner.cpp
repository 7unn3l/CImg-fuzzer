#include "runner.h"
#include "../conf/CImg_conf.h"
#include "../../CImg/CImg.h"

void Runner::run_one_sample(ImageBytes* sample){
    FILE* vfile = fmemopen(sample->data,sample->sz,"rw");

    cimg_library::CImg<unsigned char> img;
}