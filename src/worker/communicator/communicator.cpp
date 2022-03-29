#include "communicator.h"
#include "../conf/fuzzer_conf.h"
#include "../log/log.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <cstring>

void Communicator::setup(std::string& id,const CorpusManager& cm){

    std::string shm_obj_name = "cimg_fuzz_worker_";
    shm_obj_name += id;

    /* calculate the size for the communication buffer which always is
       [2 byte unsigned] sample_counter (controller will correctly handle overflows)
       [n byte] current_sample_data
       [1 byte] NULL byte
       [n byte] current_sample_filename
    */
   
    int needed_size = 2 + 1 + cm.get_largest_samplesize() + cm.get_largest_filenamesize();
    LOG("allocating shared memory object '%s' of size %d",shm_obj_name.c_str(),needed_size);

    // create a new shared memory object with RW access and RW permissions of this process
    int fd = shm_open(shm_obj_name.c_str(), O_RDWR | O_CREAT , S_IRUSR | S_IWUSR);
    
    if (fd <= 0){
        LOG("could not create shared memory, error %d",errno);
        exit(1);
    }
   
    // set the size of the shared memory object
    int ret = ftruncate(fd,needed_size);
    if (ret){
        LOG("could not resize shared memory object, error %d",errno);
        exit(1);
    }
    
    // map the shared memory segment into our address space and set RW and shared map properties.
    // Begin at offset 0.
    uint8_t *shm_ptr = (uint8_t*)mmap(NULL, needed_size, PROT_READ | PROT_WRITE,MAP_SHARED, fd, 0);
    
    if (shm_ptr == nullptr){
        LOG("received a nullpointer from mmap");
        exit(1);
    }

    shm_buf = shm_ptr;
    shm_buf_sz = needed_size;
}

void Communicator::wait_for_controller(){
    // assuming that the shm buf is a new one and filled
    // with zeros, wait for the controller to exit the setup
    // phase, signaled to a write to index 0 in the shm buf

    while (1){
        if (shm_buf[0] != 0){
            shm_buf[0] = 0;
            break;
        }
    }
}

void Communicator::update(ImageBytes* sample){

    // update counter 16 bit unsigned big endian
    uint16_t counter = 0;
    counter = shm_buf[0];
    counter = counter << 8;
    counter = counter | shm_buf[1];
    counter += 1;
    shm_buf[0] = counter >> 8;
    shm_buf[1] = counter & 0x00FF;

    // header is 2 byte counter
    // copy sample data to shared mem buffer
    memcpy(shm_buf+2,sample->data,sample->sz);

    // clean up left over bytes from previous sample
    memset(shm_buf+2+sample->sz,0,shm_buf_sz-2-sample->sz);

    // copy filename. skip one byt for NULL byte
    memcpy(shm_buf+3+sample->sz,sample->filename.data(),sample->filename.size());

    // safe extension
    current_extension = sample->fileformat;
}