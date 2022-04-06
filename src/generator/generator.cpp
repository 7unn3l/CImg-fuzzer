#define cimg_display 0 
#define cimg_use_jpeg
#define cimg_use_png

#include "../CImg/CImg.h"

#include <iostream>
#include <string>

#define SAVE(x) printf("saving as %s...",#x);img.save_##x((basename+std::string(#x)).c_str());printf(" DONE.\n")

int main(int argc,char* argv[]){
    if (argc != 2){
        printf("please provide an input image\n");
        exit(1);
    }

    setvbuf(stdout, NULL, _IONBF, 0);

    std::string fname = std::string(argv[1]);
    int extension_index = fname.find_last_of('.');

    if (extension_index == std::string::npos){
        printf("image file does need an extension\n");
        exit(1);
    }

    std::string basename = fname.substr(0,fname.size()-(fname.size()-extension_index-1));

    cimg_library::CImg<unsigned char> img;

    printf("loading image...\n");

    img.load(argv[1]);

    // native formats

    //SAVE(raw); taken out since Cimg wont ever call load_raw when 
    // loading usr supplied image of any type
    SAVE(ascii);
    SAVE(analyze); // alias hdr,nii
    SAVE(inr);
    SAVE(pnm); // alias pgm,ppm
    SAVE(bmp);
    SAVE(pandore); // alias pan
    SAVE(dlm);

    // formats not mentioned as supported natively but found in source code
    SAVE(pfm);

    // external formats

    SAVE(jpeg);
    SAVE(png);
}