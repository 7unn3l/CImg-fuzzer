from controller.controller import Controller
from cli.cli import parser
import curses
import sys

if __name__ == '__main__':
    args = parser.parse_args()
    
    c = Controller(args.num_fuzzers,args.crash_dir,args.update_interval,args.corpus_dir,args.binary_path,args.max_hangtime)
    # actually curses.wrapper does already catch and reraise exceptions for us
    # but in this case we want to call c.end() before terminating to ensure
    # that we always clean up shared memory objects on the system.

    try:
        curses.wrapper(c.run)
    except:
        c.end()
        exc_inf = sys.exc_info()
        raise exc_inf[1].with_traceback(exc_inf[2])
