# Fuzzing CImg

This project aims to fuzz the [CImg library](https://github.com/dtschump/CImg). It consits of a worker binary, written in c++, that does the heavy sample processing and an overwatcher, written in python, that reports progress to the tui and saves crashes to disk. The two components communicate via shared memory. Scaling up via multiple worker processes is supported. Im writing this project in my free time.

## Installation

```
git clone --recurse-submodules https://github.com/7unn3l/CImg-fuzzer
```

If you want to also fuzz png and jpeg handeling, which is the default you must

```
sudo apt install libjpeg-dev libpng-dev -y
```

otherwise, remove ``-lpng -ljpeg`` in ``/src/worker/Makefile`` and the defines ``cimg_use_png`` and ``cimg_use_jpeg`` in ``/src/worker/conf/CImg_conf.h``

make the worker binary

```
cd CImg-fuzzer/src/worker && make
```
You can now start fuzzing via ``python3 CImg-fuzzer/src/controller/main.py``

## Supported filetypes

The list of currently supported filetypes for fuzzing are

  - .ascii
  - .hdr/.nii (Analyze 7.5)
  - .inr (Inrimage)
  - .pnm/.ppm/.pgm (Portable Pixmap)
  - .bmp (uncompressed)
  - .pan/.pandore (Pandore-5)
  - .dlm (Matlab ASCII)
  - .jpeg/.jpg
  - .png
  - .pfm (Printer Font Metric)

## Trophies 🏆

- [CVE-2022-1325](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2022-1325): RAM exhaustion in bmp and pandore loading
