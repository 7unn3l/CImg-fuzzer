#include "corpusmanager.h"
#include "../conf/fuzzer_conf.h"
#include "../log/log.h"
#include "../random/random.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

CorpusManager::CorpusManager(const std::string &corpus_dir){
    load_corpus(corpus_dir);
    LOG("corpus manager initialized");
}

int CorpusManager::load_file(const fs::path &file){
    auto fullpath = fs::canonical(fs::absolute(file));
    size_t fsize = fs::file_size(file);
    
    if (fsize <= 0){
        LOG("[*] skipping empty file: %s",fullpath.c_str());
        return -1;
    }

    ImageBytes* imgbytes = new ImageBytes;

    imgbytes->data = (byte*)malloc(fsize);
    imgbytes->sz = fsize;
    imgbytes->filename = file.relative_path().string();

    int dotindex = imgbytes->filename.find_last_of(".");
    int namesize = imgbytes->filename.size();

    if (dotindex == std::string::npos || dotindex+1 >= namesize){
        LOG("[-] could not load file %s : no extension",imgbytes->filename.c_str());
        return 1;
    }

    imgbytes->fileformat = imgbytes->filename.substr(dotindex+1);
    

    std::ifstream fd(fullpath.c_str(),std::ios::binary | std::ios::ate);
    fd.seekg(0, std::ios::beg);

    if(fd.read((char*)&imgbytes->data[0],fsize)){
        corpus.push_back(imgbytes);
        LOG("[+] loaded %s",fullpath.c_str());

    }else{
        free(imgbytes->data);
        delete imgbytes;
        LOG("[-] could not load %s",fullpath.c_str());
        
    }
    fd.close();
    return 0;
}

void CorpusManager::load_corpus(const std::string &corpus_dir){
    fs::current_path(corpus_dir);
    for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator("."))
    {
        if (dir_entry.is_regular_file()){
            load_file(dir_entry);
        }
            
    }
}

ImageBytes* CorpusManager::get_random_sample(){
    uint_fast32_t ind = randomgen::xorshf96() % corpus.size();
    return corpus.at(ind);
}

int CorpusManager::get_largest_samplesize() const{
    int largest = 0;
    for (int i=0;i<corpus.size();i++){
        int sz = corpus.at(i)->sz;
        if (sz > largest){
            largest = sz;
        }
    }
    return largest;
}

int CorpusManager::get_largest_filenamesize() const{
    int largest = 0;
    for (int i=0;i<corpus.size();i++){
        int sz = corpus.at(i)->filename.size();
        if (sz > largest){
            largest = sz;
        }
    }
    return largest;
}