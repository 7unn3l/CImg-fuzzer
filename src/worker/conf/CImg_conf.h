// we dont care about displaying capabilities for now
#define cimg_display 0

#define cimg_use_png
#define cimg_use_jpeg

// prevent major RAM exhaustions, allow max 500 MB in memory buffer allocaitons
#define cimg_max_file_size ((cimg_ulong)500000000)