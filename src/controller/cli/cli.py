import argparse

parser = argparse.ArgumentParser(description='Fuzz the Cimg library')

parser.add_argument('-num_fuzzers','-n',type=int,metavar='n',help='number of individual fuzzing processes to use. Deafault 1',default=1)
parser.add_argument('-crash_dir','-d',type=str,metavar='dir',help='directory to store crashes. Deafault ./crashes',default="./crashes")
parser.add_argument('-update_interval','-i',type=int,metavar='i',help='update interval of ui in seconds. Deafault .125',default=.125)