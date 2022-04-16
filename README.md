# Fuzzing CImg

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

## Motivation

found unbound malloc when overwriting width and height parameters in jpeg images while fuzzing imgcat (version https://github.com/eddieantonio/imgcat/commit/e0277fa90c1083eb0c73899098dea3b99ef85e0a).
the bug was actually in an old version of the CImg library and here we are.

The issue was addressed in https://github.com/dtschump/CImg/pull/296

The Author implemented a "fix" only for PNM Image files the with commit https://github.com/dtschump/CImg/commit/e1e48675f4c05f27cec14be8ee8b8993ba4ccf8b

The version if imgcat in question uses cimg version 293. The "fix" is available from version 297

The found crash in imgcat does however not work in the current version:

"Failed to allocate memory (15.8 Gio) for image (44556,31766,1,3)."

Interestingly, these size checks in the form of "Failed to allocate memory" were present since the initial commit back in 2018:

```
...
size_t siz = (size_x,size_y,size_z,size_c); // <- No check
      if (siz) {
        _width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c;
try { _data = new T[siz]; } catch (...) {
            _width = _height = _depth = _spectrum = 0; _data = 0;
            throw CImgInstanceException(_cimg_instance
                                        "assign(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                        cimg_instance,
                                        cimg::strbuffersize(sizeof(T)*size_x*size_y*size_z*size_c),
                                        size_x,size_y,size_z,size_c);

...
```

BUT only with commit https://github.com/dtschump/CImg/commit/4f184f89f9ab6785a6c90fd238dbaa6d901d3505,
the function ```safe_size``` was introduced, fixing size_t overflows that could allow huge size allocations,
heap overwrites etc:

```size_t siz = _safe_size(size_x,size_y,size_z,size_c);```

so what really prevents the new crash from causing huge allocations like in the old program
is the safe_size

*Overall: this library is a total mess, different filetypes are loaded in very different ways,
sometimes sizes are checked, sometimes not. Also ways of checking file differ. Also the project
is really big.*

=> Fuzzing this will be very good.

## Command injection

this library will call external programs via system. It does however
escape use supplied data and wrap it in double quotes, so command injection
does not seem feasible. See load_from_network(), system(), _system_strescape()

Old command injection vulnerability with missing sanitization was already reported: https://www.cvedetails.com/cve/CVE-2019-1010174/

## Cimg Version

Fuzzed version is 3.0.2 at commit 927fee511fe4fcc1ae5cdf2365a60048fe4ff935

## Fuzzing

Lets just use the library and directly call some functions with unsafe data
from within the compiled code. So corpus data loading and mutation will happen
inside the cpp process. It will emit to disk (or to env) what corpus sample was
picked and what mutations were done. When it crashes, the python master can
read this information and reproduce a crashing input. One Master can start
many slave cpp fuzzer workers. Slaves will also emit fuzz cases so fc/s can
be estimated.

## what functions to fuzz, and how?

CImg:assign(filename) -> cimage:load(filename) will call all subsequent loaders
with \_load\_<format>(0,filename)

possibly make this faster by mapping extensions that load() detects to _load_<format>
functions and then load files once, change in memory, detect extension that Cimg would
detect and directly call the specific function with modified file contents. We have to
use std::FILE

also interesting: some applications may decide to call assign() with a byte buffer,
size and channel information (and maybe more arguments of overloaded functions). This
could also be an interesting vector

### Native Parsers

From CImg_documentation.h:

```
  The %CImg Library can NATIVELY handle the following file formats :
  - RAW : consists in a very simple header (in ascii), then the image data.
  - ASC (Ascii)
  - HDR (Analyze 7.5)
  - INR (Inrimage)
  - PPM/PGM (Portable Pixmap)
  - BMP (uncompressed)
  - PAN (Pandore-5)
  - DLM (Matlab ASCII)
```

For other formats, additional libraries have to be installed such as libpng or libjpeg. Some
image formats are converted via external system command and then loaded

### Typical image loading Process

When CImg<T>::assign is called, the application proceeds as follows:

1) check if the filename starts with http:// or https://. If true, download the img file via either builtin curl if enabled or external curl or wget
2) check if the extension of the filename matches any predefined extensions and call load_<imgtype>(filename) on it if match. Multiple extensions
can resolve to the same load function
3) if no extension matched, try to guess file type via header check. Not all supported file types can be checked
4) if nothing loaded still, try to load via built in magick library then try with external imagemagick then try with external graphicsmagick
and at last try to load the _custom_ file format .cimg

If a library shall be used or not, is always dictated by ```#define cimg_use_XXXX 1```

### Devlog
- Cimg handles .png and jpeg in a way that causes mem leaks. This was a real pain to detect. Ive implemented user controlled occasional restarts to circumvent these statefull errros where the program exits because there is no RAM left bc of mem leaks. SOMETIMES THE PROGRAM GOES INTO AN INFINTE LOOP(?). My guess for the latter behavior is that it got something todo with either no memory left (but it is a different case then total exhaustion) or that we enter a endless loop, which I could not verify. Either way, jpeg and png parsing are totally weird and caused really many problems during development and testing, because they lead to false positive "hangs"  where if the the single sample would be ran through img.assign or img._load_X in a fresh process, it would *not* hang because the hang is statefull and only happens after processing X png/jpeg images.
      
