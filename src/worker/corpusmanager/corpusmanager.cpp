#include "corpusmanager.h"
#include "../conf/fuzzer_conf.h"
#include "../log/log.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

CorpusManager::CorpusManager(){
    LOG("corpus manager initialized");
    load_corpus();
}

int CorpusManager::load_file(const fs::path &file){
    auto fullpath = fs::absolute(file);
    size_t fsize = fs::file_size(file);
    
    if (fsize <= 0){
        LOG("[*] skipping empty file: %s",fullpath.c_str());
        return -1;
    }

    ImageBytes* imgbytes = new ImageBytes;

    imgbytes->data = (byte*)malloc(fsize);
    imgbytes->sz = fsize;
    //imgbytes->filename = fullpath.c_str(); fix this
    
    std::ifstream fd(fullpath.c_str(),std::ios::binary | std::ios::ate);
    fd.seekg(0, std::ios::beg);

    if(fd.read((char*)&imgbytes->data[0],fsize)){
        LOG("[+] loaded %s",fullpath.c_str());
    }else{
        LOG("[-] could not load %s",fullpath.c_str());
    }
    return 0;
}

void CorpusManager::load_corpus(){
    fs::current_path(fs::path(fuzzer_corpus_dir));
    for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(fs::path("./serpent")))
    {
        if (dir_entry.is_regular_file()){
            load_file(dir_entry);
        }
            
    }
}