import curses
import time
from displayutils.displayutils import prettydelta

class Ui:
    def __init__(self):
        self.stdscr = curses.initscr()
        # disable cursor
        curses.curs_set(0)
        # no key press echo to stdout
        curses.noecho()
        # unbuffered input
        curses.cbreak()
        #init colors
        curses.start_color()

        self.stdscr.clear()

        # color pairs
        curses.init_pair(1, curses.COLOR_GREEN, curses.COLOR_BLACK)
        curses.init_pair(2, curses.COLOR_RED, curses.COLOR_BLACK)
        curses.init_pair(3, curses.COLOR_CYAN, curses.COLOR_BLACK)
        curses.init_pair(4, curses.COLOR_WHITE, curses.COLOR_BLACK)
    
    def add_section(self,section):
        self.stdscr.addstr(f'{section}\n',curses.A_UNDERLINE)

    def add_pair(self,key,val,col=4):
        self.stdscr.addstr(f'{key} : ',curses.A_DIM)
        self.stdscr.addstr(f'{val}\n',curses.color_pair(col))

    def render(self,stats,controller):


        self.stdscr.erase()

        self.add_section('general')
        self.add_pair('runtime',prettydelta(stats.runtime))

        if controller.crashes:
            val = prettydelta(time.perf_counter() - controller.crashes[-1].creationdate,z=False)
        else:
            val = 'n/a'
        
        self.add_pair('last crash',val)
        
        self.add_section('workers')
        self.add_pair('workers',len(stats._last_samples_processed_per_worker))
        self.add_pair('samples per second',int(stats.samples_per_second))
        self.add_pair('samples per second per worker',int(stats.samples_per_second_per_worker))
        self.add_pair('total samples',int(stats.total_samples_processed))
        
        self.add_section('crashes')

        if stats.total_crashes <= 0:
            col = 1
        else:
            col = 2

        self.add_pair('total crashes',f'{stats.total_crashes} {f"({stats.total_hangs} hangs)" if stats.total_hangs else ""}',col)
        self.add_pair('avg time to crash',prettydelta(stats.seconds_per_crash))


        if not stats.crashes_by_filetype:
            st = 'n/a'
        else:
            st = ('\n'+22*' ').join(f'{extension} : {crash}' for extension,crash in sorted(stats.crashes_by_filetype.items(), key=lambda item: item[1])[::-1])

        self.add_pair('crashes by filetype',st)

        self.stdscr.refresh()
    
    def end(self):
        curses.nocbreak()
        curses.echo()
        curses.endwin()