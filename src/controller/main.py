from controller.controller import Controller
from cli.cli import parser

if __name__ == '__main__':
    args = parser.parse_args()
    
    c = Controller(args.num_fuzzers,args.crash_dir,args.update_interval)
    c.run()