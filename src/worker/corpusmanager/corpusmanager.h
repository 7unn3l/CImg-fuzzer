#pragma once
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;
typedef unsigned char byte;

struct ImageBytes{
    const char* filename;
    int sz;
    byte* data;
};

class CorpusManager{
    public:
        ImageBytes* get_random_sample();
        CorpusManager();
    
    private:
        std::vector<ImageBytes*> corpus;
        void load_corpus();
        int load_file(const fs::path&);
};