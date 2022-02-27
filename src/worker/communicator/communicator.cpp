#include "communicator.h"
#include "../conf/fuzzer_conf.h"
#include "../log/log.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

void Communicator::setup(int uid,const CorpusManager& cm){

    std::string shm_obj_name = "cimg_fuzz_worker_";
    shm_obj_name += std::to_string(uid);

    /* calculate the size for the communication buffer which always is
       [2 byte unsigned] sample_counter (controller will correctly handle overflows)
       [4 byte unsigned] current_sample_size
       [n byte] current_sample_data
       [n byte] current_sample_filename
    */
   
    int needed_size = 2 + 4 + cm.get_largest_samplesize() + cm.get_largest_filenamesize();
    LOG("allocating shared memory object '%s' of size %d",shm_obj_name.c_str(),needed_size);

    // create a new shared memory object with RW access and RW permissions of this process
    int fd = shm_open("cimg_fuzzer_shmobj", O_RDWR | O_CREAT , S_IRUSR | S_IWUSR);
    
    if (fd <= 0){
        LOG("could not create shared memory, error %d",errno);
        exit(1);
    }
   
    // set the size of the shared memory object
    int ret = ftruncate(fd,10);
    if (ret){
        LOG("could not resize shared memory object, error %d",errno);
        exit(1);
    }
    
    // map the shared memory segment into our address space and set RW and shared map properties.
    // Begin at offset 0.
    uint8_t *shm_ptr = (uint8_t*)mmap(NULL, 10, PROT_READ | PROT_WRITE,MAP_SHARED, fd, 0);
    
    if (shm_ptr == nullptr){
        LOG("received a nullpointer from mmap");
        exit(1);
    }

    shm_buf = shm_ptr;
    shm_buf_sz = needed_size;
}