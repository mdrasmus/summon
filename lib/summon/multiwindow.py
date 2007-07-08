#
# SUMMON - Multiple Window Management
#

from summon.core import *
from summon import util
import summon


WINDOW_OFFSET = None

def get_window_offset(win):
    """determine window decoration offset"""
    global WINDOW_OFFSET
    
    # if WINDOW_OFFSET is already known, return it
    if WINDOW_OFFSET != None:
        return WINDOW_OFFSET[:]
    
    pos1 = win.get_position()
    win.set_position(*pos1)
    
    # wait for position to change
    pos2 = pos1
    while pos2 == pos1:
        pos2 = win.get_position()

    offset = [pos1[0] - pos2[0], pos1[1] - pos2[1]]
    WINDOW_OFFSET = offset[:]

    pos1 = (pos1[0] + offset[0], pos1[1] + offset[1])        
    win.set_position(*pos1)

    # TODO: this will fail if window decoration is size zero!
    # wait for last set position to take affect
    pos2 = pos1
    while pos2 == pos1:
        pos2 = win.get_position()
    
    return offset
    


class WindowEnsemble (summon.VisObject):
    def __init__(self, windows, stackx=False, stacky=False, 
                       samew=False, sameh=False,
                       tiex=False, tiey=False, pinx=False, piny=False,
                       coordsx=None, coordsy=None,
                       master=None):
        self.windows = windows[:]
        self.pos = {}
        self.sizes = {}
        self.stackx = stackx
        self.stacky = stacky
        self.samew = samew
        self.sameh = sameh
        self.closeListeners = {}
        self.ties = {}
        
        
        if master != None:
            self.master = master
        else:
            self.master = windows[0]
        
        # determine window decoration offset
        self.offset = get_window_offset(self.master)
        
        
        # setup window stacking
        if stackx or stacky:
            self.stack(self.master)
        
        
        # setup scrolling ties
        if tiex or tiey:
            self.tie(windows, tiex=tiex, tiey=tiey, pinx=pinx, piny=piny,
                     coordsx=coordsx, coordsy=coordsy, master=master)
        
        def make_close_listener(win):
            return lambda: self._on_window_close(win)
        
        # setup window close listeners
        for win in windows:
            self.closeListeners[win] = make_close_listener(win)
            win.add_close_listener(self.closeListeners[win])
        
        # record window positions and sizes
        for win in windows:
            pos = win.get_position()
            self.pos[win] = [pos[0] + self.offset[0], pos[1] + self.offset[1]]
            self.sizes[win] = win.get_size()
        
        
        # enable updating
        self.win = self.master
        self.enableUpdating(interval=.5)
    
    
    def stop(self):
        """stop the window ensemble from coordinating window movements"""
        
        # pretend all the windows have closed
        for win in list(self.windows):
            self._on_window_close(win)
        self.enableUpdating(False)
    
    
    def _on_window_close(self, win):
        """callback for when a window in the ensemble closes"""
        
        self.windows.remove(win)
        
        # remove all callbacks
        win.remove_close_listener(self.closeListeners[win])
        win.remove_view_change_listener(self.ties[win].update_scroll)
        win.remove_focus_change_listener(self.ties[win].update_focus)
        del self.closeListeners[win]
        del self.ties[win]
        
        # make sure window ties remove their callbacks
        for tie in self.ties.itervalues():
            tie.remove_window(win)
        
    
    def window_offset(self):
        """determine window decoration offset"""
        global WINDOW_OFFSET
        
        if WINDOW_OFFSET != None:
            self.offset = WINDOW_OFFSET[:]
            return
        
        pos1 = self.master.get_position()
        self.master.set_position(*pos1)
        
        # wait for position to change
        pos2 = pos1
        while pos2 == pos1:
            pos2 = self.master.get_position()
        
        self.offset = [pos1[0] - pos2[0], pos1[1] - pos2[1]]
        WINDOW_OFFSET = self.offset[:]
        
        pos1 = (pos1[0] + self.offset[0], pos1[1] + self.offset[1])        
        self.master.set_position(* pos1)
        
        # TODO: this will fail if window decoration is size zero!
        # wait for last set position to take affect
        pos2 = pos1
        while pos2 == pos1:
            pos2 = self.master.get_position()
    
    
    def stack(self, win):
        others = self.windows[:]
        others.remove(win)
        
        target_pos = win.get_position()
        target_pos = [target_pos[0] + self.offset[0],
                      target_pos[1] + self.offset[1]]
        target_size = win.get_size()
        
        # get window sizes
        widths = []
        heights = []
        x = []
        y = []
        totalx = 0
        totaly = 0
        target = []
        
        for win2 in self.windows:
            w, h = win2.get_size()
            
            if self.samew:
                w = target_size[0]
            if self.sameh:
                h = target_size[1]
            
            win2.set_size(w, h)
            self.sizes[win2] = (w, h)
            
            if win2 == win:
                target = [totalx, totaly]
            
            widths.append(w)
            heights.append(h)
            x.append(totalx)
            y.append(totaly)
            totalx += w - self.offset[0]
            totaly += h - self.offset[1]
        
        # set window positions
        for i, win2 in enumerate(self.windows):
            if self.stackx:
                newx = target_pos[0]
                newy = target_pos[1] + y[i] - target[1]
            elif self.stacky:
                newx = target_pos[0] + x[i] - target[0]
                newy = target_pos[1]
            
            self.pos[win2] = [newx, newy]
            
            win2.set_position(newx, newy)
        
            
    def align(self, win):
        now = win.get_position()
        now = [now[0] + self.offset[0], now[1] + self.offset[1]]
        pos1 = self.pos[win]
        
        # move all other windows to match moved window
        for win2 in self.windows:
            if win2 != win:
                pos2 = self.pos[win2]
                pos3 = [now[0] + pos2[0] - pos1[0],
                        now[1] + pos2[1] - pos1[1]]
                win2.set_position(*pos3)
                self.pos[win2] = pos3

        # record new position for main window
        self.pos[win] = now
    
    
    def tie(self, windows, tiex=False, tiey=False, pinx=False, piny=False,
                  coordsx=None, coordsy=None, master=None):

        if len(windows) < 2:
            return
        
        self.tiex = tiex
        self.tiey = tiey
        self.pinx = pinx
        self.piny = piny
        self.coordsx = coordsx
        self.coordsy = coordsy
        
        if master == None:
            master = windows[0]    
        
        if coordsx == None:
            coordsx = [0] * len(windows)

        if coordsy == None:
            coordsy = [0] * len(windows)

        # make coordinate lookup
        self.coords = {}
        for win, x, y in zip(windows, coordsx, coordsy):
            self.coords[win] = util.Bundle(x=x, y=y)

        # set callbacks for each window
        for win in windows:
            others = util.remove(windows, win)
            
            tie = WindowTie(win, others, self)
            self.ties[win] = tie
            
            win.add_view_change_listener(tie.update_scroll)
            win.add_focus_change_listener(tie.update_focus)

            if master == win:
                master_trans = tie.update_scroll
                master_focus = tie.update_focus
        master_focus()
        master_trans()

            

    def update(self):
        # check for moved windows
        for win in self.windows:
            pos1 = win.get_position()
            pos1 = [pos1[0] + self.offset[0], pos1[1] + self.offset[1]]
            pos2 = self.pos[win]
            size1 = win.get_size()
            size2 = self.sizes[win]
            
            if self.stackx or self.stacky:
                if pos1 != pos2 or size1 != size2:
                    self.stack(win)
                    break
            else:
                if pos1 != pos2:
                    self.align(win)
                    break


class WindowTie:
    def __init__(self, win, others, ensemble):
        self.win = win
        self.others = others
        self.lock = False
        self.ensemble = ensemble

    def remove_window(self, win):
        """removes a window from the list of tied windows"""
        if win in self.others:
            self.others.remove(win)
    
    
    def update_scroll(self):
        """call back that sets translation and zoom"""
        
        w1 = self.win
        others = self.others
        coords = self.ensemble.coords

        # prevent infinite loops
        if self.lock:
            return
        self.lock = True
        
        pos1 = w1.get_position()
        trans1 = w1.get_trans()
        zoom1 = w1.get_zoom()
        
        
        for w2 in others:
            pos2 = w2.get_position()
            oldtrans2 = list(w2.get_trans())
            oldzoom2 = list(w2.get_zoom())
            trans2 = oldtrans2[:]
            zoom2 = oldzoom2[:]

            if self.ensemble.tiex:
                trans2[0] = trans1[0] - coords[w2].x + coords[w1].x
                zoom2[0] = zoom1[0]

                if self.ensemble.pinx:
                    trans2[0] += pos1[0] - pos2[0]

            if self.ensemble.tiey:
                trans2[1] = trans1[1] - coords[w2].y + coords[w1].y
                zoom2[1] = zoom1[1]

                if self.ensemble.piny:
                    trans2[1] -= pos1[1] - pos2[1]

            # check to see if there is a change (prevents infinite loops)
            if trans2 != oldtrans2:
                w2.set_trans(*trans2)
            if zoom2 != oldzoom2:
                w2.set_zoom(*zoom2)
        self.lock = False


        
    def update_focus(self):
        """callback that sets focus"""
        
        w1 = self.win
        others = self.others
        coords = self.ensemble.coords
        
        # prevent infinite loops     
        if self.lock:
            return
        self.lock = True
        
        fx1, fy1 = w1.get_focus()
        fx1 -= coords[w1].x
        fy1 -= coords[w1].y

        for w2 in others:
            newpos = (fx1 + coords[w2].x, fy1 + coords[w2].y)
            oldpos = w2.get_focus()

            if newpos != oldpos:
                w2.set_focus(* newpos)
        self.lock = False
