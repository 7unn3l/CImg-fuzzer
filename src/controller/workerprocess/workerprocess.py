from multiprocessing import shared_memory
from os import path
import subprocess
import struct
import time

class WorkerProcess():
    def __init__(self,id,corpus_dir,binarypath):
        self.id = id
        self.corpus_dir = corpus_dir
        self.binarypath = binarypath
        self.shm_id = f'cimg_fuzz_worker_{self.id}'
        self.shm = None
        self.proc = None
        self.restarting = False
        self._last_samplecount = 0
        self.total_samplecount = 0
    
    def start(self):
        self.proc = subprocess.Popen([self.binarypath,self.id,self.corpus_dir],stdout=subprocess.PIPE)

        while self.proc.poll() == None:
            time.sleep(.125)
            try:
                self.shm = shared_memory.SharedMemory(name=self.shm_id)
                return
            except FileNotFoundError:
                pass

        raise BaseException('worker binary exited during setup')

    def _samples_processed(self):
        self._last_samplecount = struct.unpack('>H',self.shm.buf[:2])[0]
        return self._last_samplecount
    
    def get_total_samples_porocessed(self):
        # thread safety
        if self.restarting:
            return self.total_samplecount

        # this frunction handels overflows in the u16 bit counter field
        saved_sc = self._last_samplecount
        new_sc = self._samples_processed()

        diff = new_sc - saved_sc

        if diff < 0:
            # handle overflow
            diff = pow(2,16)-1 + diff

        self.total_samplecount += diff

        return self.total_samplecount 

    def get_retcode(self):
        return self.proc.poll()

    def get_sampleinfo(self):
        data = bytes(self.shm.buf[2:])
        filename = data.rstrip(b'\x00').split(b'\x00')[-1]
        content = data.rstrip(b'\x00')[:-len(filename)-1]
        return content,path.basename(filename.decode())
    
    def kill(self):
        try:
            self.shm.unlink()
        except FileNotFoundError:
            # proc may already be dead and shm object
            # already destroyed
            pass

        self.proc.kill()
    
    def restart(self):
        assert self.proc.poll() != None, "tried to restart running worker process"
        self.restarting = True
        self._last_samplecount = 0
        self.shm.unlink()
        self.start()
        self.restarting = False