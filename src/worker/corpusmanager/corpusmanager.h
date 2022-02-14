#pragma once
#include <vector>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;
typedef unsigned char byte;

struct ImageBytes{
    std::string filename;
    std::string extension;
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