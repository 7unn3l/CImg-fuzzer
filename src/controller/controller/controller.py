import os
import time
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
    def __init__(self,num_workers,crash_dir='./crashes',statistics_update_interval=1/8):
        self.num_workers = num_workers
        self.crash_dir = os.path.abspath(crash_dir)
        self.update_interval = statistics_update_interval
        self.crashes = []
        self.workers = []
        self._end = False
    
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

        # calculate total samples procssed
        Statistics.total_samples_processed = 0
        for worker in self.workers:
            Statistics.total_samples_processed += worker.get_total_samples_porocessed()

        Statistics.samples_per_second = Statistics.total_samples_processed/delta
        Statistics.samples_per_second_per_worker = Statistics.samples_per_second/self.num_workers

        Statistics.total_crashes = len(self.crashes)
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
            for worker in self.get_dead_workers():
                r = worker.get_retcode()
                content,fname = worker.get_sampleinfo()
                c = Crash(r,fname,content)
                self.crashes.append(c)
                c.safe_to_disk(self.crash_dir)                
                Statistics._last_samples_processed_per_worker[self.workers.index(worker)] = 0
                worker.restart()

    def run(self):

        self.make_crash_dir()
        self.init_statistics()
        print(f'starting fuzzing session with {self.num_workers} workers..')

        for i in range(self.num_workers):
            w = WorkerProcess(str(i),'./bin/worker')
            self.workers.append(w)
            w.start()
        
        Thread(target=self.t_watch_workers).start()

        while not self._end:
            try:
                time.sleep(self.update_interval)
                self.update_statistics()
                print(Statistics.samples_per_second,'samples /s',f'({Statistics.samples_per_second_per_worker} per worker per second) {Statistics.seconds_per_crash} seconds/crash\t[{Statistics.total_crashes} crashes total] [{self.workers[0].total_samplecount} samples total]',end='\r')
            except KeyboardInterrupt:
                self.end()
    
    def get_dead_workers(self):
        return [worker for worker in self.workers if worker.get_retcode() != None]
