import os
import time
import sys
from ui.ui import Ui
from workerprocess.workerprocess import WorkerProcess
from crash.crash import Crash
from threading import Thread

class Statistics:
    starttime = None
    runtime = 0

    total_samples_processed = 0
    samples_per_second_per_worker = 0
    samples_per_second = 0
    
    # total_crashes counts both hangs and actual crashes.
    # we do not differentiate between crashes and hangs
    # for other time based statistic variables

    total_crashes = 0 
    total_hangs = 0
    crashes_by_filetype = {}
    seconds_per_crash = 0

class Controller():
    def __init__(self,args):
        self.args = args
        self.num_workers = args.num_fuzzers
        self.crash_dir = os.path.abspath(args.crash_dir)
        self.update_interval = args.update_interval
        self.corpus_dir = args.corpus_dir
        self.binary_path = args.binary_path
        self.max_hangtime = args.max_hangtime
        self.restart_interval = args.restart_interval
        self.last_restart = None
        self.crashes = []
        self.workers = []
        self.exc_info = None
        self._end = False
        self.ui = None
        self.supported_extensions = ['ascii','hdr','nii','inr','pnm','ppm','pgm','bmp','pan', \
                                    'dlm','jpeg','jpg','png','pfm']
    
    def make_crash_dir(self):
        if not os.path.exists(self.crash_dir):
            os.makedirs(self.crash_dir)
        else:
            pass


    def update_statistics(self):
        delta = time.perf_counter()-Statistics.starttime
        Statistics.runtime = delta

        Statistics.total_samples_processed = sum([worker.get_total_samples_porocessed() for worker in self.workers])
        Statistics.samples_per_second = Statistics.total_samples_processed/delta
        Statistics.samples_per_second_per_worker = Statistics.samples_per_second/self.num_workers

        Statistics.total_crashes = len(self.crashes)
        Statistics.total_hangs = len([c for c in self.crashes if c.exitcode == None])
        Statistics.crashes_by_filetype = {}
        if Statistics.total_crashes > 0:
            Statistics.seconds_per_crash = delta/Statistics.total_crashes
            for crash in self.crashes:
                if crash.extension in Statistics.crashes_by_filetype:
                    Statistics.crashes_by_filetype[crash.extension] += 1
                else:
                    Statistics.crashes_by_filetype[crash.extension] = 1

    def end(self):
        self._end = True
        for worker in self.workers:
            worker.kill()

    def t_watch_workers(self):
        while not self._end:
            try:
                for worker,reason in self.get_dead_workers():

                    r = worker.get_retcode() if reason == 'crash' else None

                    content,fname = worker.get_sampleinfo()
                    c = Crash(r,fname,content)
                    self.crashes.append(c)
                    c.safe_to_disk(self.crash_dir)

                    if reason == 'hang':
                        worker.kill()

                    worker.restart()

                if self.restart_interval != 0 and time.perf_counter() - self.last_restart >= self.restart_interval:
                    self.last_restart = time.perf_counter()
                    for worker in self.workers:
                        worker.kill()
                        worker.restart()

            except:
                self.exc_info = sys.exc_info()
                time.sleep(self.update_interval*2) # required bc of infinite lock

    def preflight(self):

        if not os.path.exists(self.corpus_dir):
            print(f'corpus directory {os.path.abspath(self.corpus_dir)} does not exist')
            exit(1)

        for file in os.listdir(self.corpus_dir):
            fullpath = os.path.abspath(os.path.join(self.corpus_dir,file))
            
            if os.path.isfile(fullpath) and not any([fullpath.lower().endswith(ext) for ext in self.supported_extensions]):
                print(f'filetype of {fullpath} is not supported')
                exit(1)

        self.make_crash_dir()
        
        print(f'starting fuzzing session with {self.num_workers} workers..')

        self.ui = Ui()

        for i in range(self.num_workers):
            w = WorkerProcess(str(i),self.args)
            self.workers.append(w)
            w.start()
        
        Statistics.starttime = time.perf_counter()
        self.last_restart = Statistics.starttime

        Thread(target=self.t_watch_workers).start()
        
        self.ui.stdscr.clear()


    def run(self,_):

        while not self._end:

            # check for exceptions in the worker watcher thread
            # and re-raise them here so curses.wrapper can properly catch them
            if self.exc_info != None:
                raise self.exc_info[1].with_traceback(self.exc_info[2])

            try:
                time.sleep(self.update_interval)
                self.update_statistics()                
                self.ui.render(Statistics,self)
            except KeyboardInterrupt:
                break
    
    def get_dead_workers(self):
        for worker in self.workers:
            reason = ''
            if worker.get_retcode() != None:
                reason = 'crash'
            elif self.args.max_hangtime != 0:
                if worker.is_hanging():
                    reason = 'hang'

            if reason:
                yield (worker,reason)