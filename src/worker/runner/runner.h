#pragma once
#include <cstdio>
#include "../corpusmanager/corpusmanager.h"
#include "../conf/CImg_conf.h"
#include "../../CImg/CImg.h"
#include "../communicator/communicator.h"

class Runner{
public:
    void run_one_sample(Communicator& com);
    void load_fileformat(Communicator& com, cimg_library::CImg<unsigned char>& img);
};