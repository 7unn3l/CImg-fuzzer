#pragma once
#include <vector>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;
typedef unsigned char byte;

struct ImageBytes{
    std::string filename;
    std::string fileformat;
    int sz;
    byte* data;
};

class CorpusManager{
    public:
        ImageBytes* get_random_sample();
        CorpusManager(const std::string &corpus_dir);
        int  get_largest_samplesize() const;
        int get_largest_filenamesize() const;
    
    private:
        std::vector<ImageBytes*> corpus;
        void load_corpus(const std::string &corpus_dir);
        int load_file(const fs::path&);
};