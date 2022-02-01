#define fuzzer_log 1

#define fuzzer_corpus_dir "../../sample_corpus" 

const char* FUZZ_EXTENSIONS[8] = {
    // native support for the following extensions
    ".raw", // consists in a very simple header (in ascii), then the image data.
    ".asc", // (Ascii)
    ".hdr", // (Analyze 7.5)
    ".inr", // (Inrimage)
    ".pgm", // (Portable Pixmap)
    ".bmp", // (uncompressed)
    ".pan", // (Pandore-5)
    ".dlm"  // (Matlab ASCII)
    };
