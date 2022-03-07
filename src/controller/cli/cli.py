import argparse

parser = argparse.ArgumentParser(description='Fuzz the Cimg library')

parser.add_argument('-num_fuzzers','-n',type=int,metavar='n',help='number of individual fuzzing processes to use. Deafault 1',default=1)
parser.add_argument('-crash_dir','-c',type=str,metavar='dir',help='directory to store crashes. Deafault ./crashes',default="./crashes")
parser.add_argument('-update_interval','-i',type=int,metavar='i',help='update interval of ui in seconds. Deafault .125',default=.125)
parser.add_argument('-corpus_dir','-s',type=str,metavar='dir',help='directory to load corpus from. Deafault ./corpus',default="./corpus")
parser.add_argument('-binary_path','-b',type=str,metavar='path',help='path to the worker executable ./corpus',default="./bin/worker")