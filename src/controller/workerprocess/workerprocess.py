from multiprocessing import shared_memory
from os import path
import subprocess
import struct
import time

class WorkerProcess():
    def __init__(self,id,args):
        self.id = id
        self.corpus_dir = args.corpus_dir
        self.binarypath = args.binary_path
        self.max_hangtime = args.max_hangtime
        self.shm_id = f'cimg_fuzz_worker_{self.id}'
        self.shm = None
        self.proc = None
        self.restarting = False
        self._last_samplecount = 0
        self.total_samplecount = 0
        self.hang_ts = 0
    
    def start(self):
        self.proc = subprocess.Popen([self.binarypath,self.id,self.corpus_dir],stdout=subprocess.PIPE,stderr=subprocess.PIPE)

        # we give the worker process 10 seconds to (re)start and setup
        # if time runs out or process exits during setup, raise an exception.

        for _ in range(int(10/.125)):
            if self.proc.poll() != None:
                raise BaseException(f'worker binary with id {self.id} exited prematurely during setup with code {self.proc.poll()} stderr={self.proc.stderr.read()}')

            try:
                self.shm = shared_memory.SharedMemory(name=self.shm_id)
                # to avoid a race condition where the worker starts fuzzing
                # the target before we detect that the shm has been established,
                # the worker waits for the shm to be written to.
                self.shm.buf[0] = 1
                return
            except FileNotFoundError:
                # setup not yet done
                pass
            time.sleep(.125)

        raise BaseException(f'worker binary with id {self.id} did not create shared memory after 10 seconds. Timeout.')

    def _samples_processed(self):
        self._last_samplecount = struct.unpack('>H',self.shm.buf[:2])[0]
        return self._last_samplecount
    
    def is_hanging(self):
        if self.hang_ts != 0:
            return time.perf_counter()-self.hang_ts >= self.max_hangtime
        return False

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

        if diff == 0:
            if self.hang_ts == 0:
                self.hang_ts = time.perf_counter()
        else:
            self.hang_ts = 0
            self.total_samplecount += diff

        return self.total_samplecount 

    def get_retcode(self):
        return self.proc.poll()

    def get_sampleinfo(self):
        data = bytes(self.shm.buf[2:])
        filename = data.rstrip(b'\x00').split(b'\x00')[-1]
        content = data.rstrip(b'\x00')[:-len(filename)-1]
        return content,path.basename(filename.decode())
    
    def save_unlink(self):
        if self.shm == None:
            return
        try:
            self.shm.unlink()
        except FileNotFoundError:
            # proc may already be dead and shm object
            # already destroyed
            pass

    def kill(self):
        self.save_unlink()
        self.proc.kill()

        #avoid race condition where we try to restart a running process
        while self.get_retcode() == None:
            pass

    def restart(self):
        assert self.proc.poll() != None, "tried to restart running worker process"
        self.restarting = True
        self._last_samplecount = 0
        self.hang_ts = 0
        self.save_unlink()
        self.start()
        self.restarting = False