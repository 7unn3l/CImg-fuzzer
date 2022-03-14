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

    _last_samples_processed_per_worker = []
    total_samples_processed = 0
    samples_per_second_per_worker = 0
    samples_per_second = 0
    
    total_crashes = 0
    crashes_by_filetype = {}
    seconds_per_crash = 0

class Controller():
    def __init__(self,num_workers,crash_dir,update_interval,corpus_dir,binary_path):
        self.num_workers = num_workers
        self.crash_dir = os.path.abspath(crash_dir)
        self.update_interval = update_interval
        self.corpus_dir = corpus_dir
        self.binary_path = binary_path
        self.crashes = []
        self.workers = []
        self.exc_info = None
        self._end = False
        self.ui = Ui()
    
    def make_crash_dir(self):
        if not os.path.exists(self.crash_dir):
            os.makedirs(self.crash_dir)
        else:
            pass
    
    def init_statistics(self):
        Statistics._last_samples_processed_per_worker = [0]*self.num_workers
        Statistics.starttime = time.perf_counter()

    def update_statistics(self):
        delta = time.perf_counter()-Statistics.starttime
        Statistics.runtime = delta

        Statistics.total_samples_processed = sum([worker.get_total_samples_porocessed() for worker in self.workers])
        Statistics.samples_per_second = Statistics.total_samples_processed/delta
        Statistics.samples_per_second_per_worker = Statistics.samples_per_second/self.num_workers

        Statistics.total_crashes = len(self.crashes)
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
                for worker in self.get_dead_workers():
                    r = worker.get_retcode()
                    content,fname = worker.get_sampleinfo()
                    c = Crash(r,fname,content)
                    self.crashes.append(c)
                    c.safe_to_disk(self.crash_dir)
                    Statistics._last_samples_processed_per_worker[self.workers.index(worker)] = 0
                    worker.restart()
            except:
                self.exc_info = sys.exc_info()
                time.sleep(self.update_interval*2) # required bc of infinite lock

    def run(self,_):

        self.make_crash_dir()
        self.init_statistics()
        print(f'starting fuzzing session with {self.num_workers} workers..')

        for i in range(self.num_workers):
            w = WorkerProcess(str(i),self.corpus_dir,self.binary_path)
            self.workers.append(w)
            w.start()
        
        Thread(target=self.t_watch_workers).start()
        
        self.ui.stdscr.clear()

        while not self._end:

            # check for exceptions in the worker watcher thread
            # and re-raise them here so curses.wrapper can properly catch them
            if self.exc_info != None:
                self.end()
                raise self.exc_info[1].with_traceback(self.exc_info[2])

            try:
                time.sleep(self.update_interval)
                self.update_statistics()                
                self.ui.render(Statistics,self)
            except KeyboardInterrupt:
                self.end()
    
    def get_dead_workers(self):
        return [worker for worker in self.workers if worker.get_retcode() != None]
