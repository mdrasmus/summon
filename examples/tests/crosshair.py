
import summon
from summon.core import *




win = summon.Window()

class CrossHair (object):

    def __init__(self, win, col=None):
        self.win = win
        self.grp = None
        self.timer = None
        self.color = col


    def start(self, interval=.05):
        self.timer = summon.add_timer(self.update, interval, window=win)


    def stop(self):
        self.timer.stop()
        if self.grp:
            self.win.screen.remove_group(self.grp)
            self.grp = None


    def toggle(self):
        if self.grp:
            self.stop()
        else:
            self.start()


    def is_enabled(self):
        return (self.grp is not None)


    def update(self):
        x, y = self.win.get_mouse_pos("screen")
        
        if self.grp is None:
            # TODO: add custom color

            self.grp = translate(x, y,
                                 lines(color(1,1,1), 
                                       0, -3000, 0, 3000,
                                       -3000, 0, 3000, 0))
            self.win.screen.add_group(self.grp)
        else:
            self.grp.set(x, y)

    

ch = CrossHair(win)
ch.start()

