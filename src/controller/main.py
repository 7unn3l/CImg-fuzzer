from dis import findlinestarts
from controller.controller import Controller
from cli.cli import parser
import curses
import sys

if __name__ == '__main__':
    args = parser.parse_args()
    
    c = Controller(args)
    # actually curses.wrapper does already catch and reraise exceptions for us
    # but in this case we want to call c.end() before terminating to ensure
    # that we always clean up shared memory objects on the system.

    exc_inf = None

    try:
        curses.wrapper(c.run)
    except:
        exc_inf = sys.exc_info()

    finally:
        c.end()
        if exc_inf:
            raise exc_inf[1].with_traceback(exc_inf[2])
