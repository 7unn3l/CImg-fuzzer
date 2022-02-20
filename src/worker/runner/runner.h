#pragma once
#include <cstdio>
#include "../corpusmanager/corpusmanager.h"
#include "../conf/CImg_conf.h"
#include "../../CImg/CImg.h"

class Runner{
public:
    void run_one_sample(ImageBytes* sample);
    void load_fileformat(ImageBytes* sample, cimg_library::CImg<unsigned char>& img);
};