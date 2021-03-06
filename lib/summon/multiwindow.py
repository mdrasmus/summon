"""

 SUMMON - Multiple Window Management

"""

import time

from summon.core import *
from summon import util
import summon

   


class WindowEnsemble:
    """This class coordinates the position, size, translation, and zoom of 
       multiple SUMMON Windows.
    """

    def __init__(self, windows, stackx=False, stacky=False, 
                       samew=False, sameh=False,
                       tiex=False, tiey=False, pinx=False, piny=False,
                       coordsx=None, coordsy=None,
                       master=None,
                       close_with_master=None):
        """windows -- windows to coordinate
           stackx  -- (bool) windows should stack with same x-coordinate
           stacky  -- (bool) windows should stack with same y-coordinate
           samew   -- (bool) windows should have same width
           sameh   -- (bool) windows should have same height
           tiex    -- (bool) translation along x-axis should be coordinated
           tiey    -- (bool) translation along y-axis should be coordinated
           pinx    -- (bool) translation along x-axis should be offset by window position
           piny    -- (bool) translation along x-axis should be offset by window position
           coordsx -- a list of x-offsets for translation
           coordsy -- a list of y-offsets for translation
           master  -- master window
           close_with_master -- (bool) if true, all windows close with master
        """
        
        self.windows = windows[:]
        self.pos = {}
        self.sizes = {}
        self.stackx = stackx
        self.stacky = stacky
        self.samew = samew
        self.sameh = sameh
        self.listeners = {}
        self.ties = {}
        self.lock = False
        self.recentPos = util.Dict(default=[])
        self.recentSize = util.Dict(default=[])
        
        self.tiex = tiex
        self.tiey = tiey
        self.pinx = pinx
        self.piny = piny
        self.coordsx = coordsx
        self.coordsy = coordsy
        
        # setup master window
        if master != None:
            self.master = master
            
            # close_with_master defaults to True if master is given
            if close_with_master == None:
                self.close_with_master = True
            else:
                self.close_with_master = close_with_master
        else:
            self.master = windows[0]
            
            # close_with_master defaults to False if master is not given
            if close_with_master == None:            
                self.close_with_master = False
            else:
                self.close_with_master = close_with_master
        
        
        # record window positions and sizes
        for win in windows:
            self.pos[win] = win.get_position()
            self.sizes[win] = win.get_size()
                    
                                              
        # setup window listeners
        for win in windows:
            self.init_listeners(win)
            

        # setup window stacking
        if stackx or stacky:
            self.stack(self.master)
        
        # setup scrolling ties
        if tiex or tiey:
            self.tie(windows, tiex=tiex, tiey=tiey, pinx=pinx, piny=piny,
                     coordsx=coordsx, coordsy=coordsy, master=master)
    
    

    def add_window(self, win, index=-1, coordx=0, coordy=0):
        """add a window to the existing ensemble"""
        
        if self.tiex or self.tiey:
            self.untie()
        
        if index == -1:
            index = len(self.windows)
        self.windows.insert(index, win)
        
        self.pos[win] = win.get_position()
        self.sizes[win] = win.get_size()
        
        self.init_listeners(win)
        
        self.recentPos.clear()
        self.recentSize.clear()
        
        # setup window stacking
        if self.stackx or self.stacky:
            self.stack(self.master)
        
        if self.coordsx != None:
            self.coordsx.insert(index, coordx)
        if self.coordsy != None:
            self.coordsy.insert(index, coordy)
        
        # setup scrolling ties
        if self.tiex or self.tiey:
            self.tie(self.windows, tiex=self.tiex, tiey=self.tiey, 
                     pinx=self.pinx, piny=self.piny,
                     coordsx=self.coordsx, coordsy=self.coordsy, 
                     master=self.master)

    
    
    def init_listeners(self, win):
        """initialize listeners for a window managed by the ensemble"""
        
        self.listeners[win] = util.Bundle(
            close=lambda: self._on_window_close(win),
            resize=lambda w, h: self._on_window_resize(win, w, h),
            move=lambda x, y: self._on_window_move(win, x, y))
        win.add_close_listener(self.listeners[win].close)
        win.add_resize_listener(self.listeners[win].resize)
        win.add_move_listener(self.listeners[win].move)
    
    
    def stop(self):
        """stop the window ensemble from coordinating window movements"""
        
        # pretend all the windows have closed
        for win in list(self.windows):
            self._on_window_close(win)
    
    
    def _on_window_close(self, win):
        """callback for when a window in the ensemble closes"""
        
        self.remove_window(win)
    
        # close all windows if master closes
        if self.close_with_master and win == self.master:
            for win2 in self.windows:
                win2.close()
        
    
    def remove_window(self, win):
        """removes a window from the ensemble"""
        
        # do nothing if window is not in ensemble
        if win not in self.windows:
            return    
        
        self.windows.remove(win)
        
        # remove all callbacks
        win.remove_close_listener(self.listeners[win].close)
        win.remove_resize_listener(self.listeners[win].resize)
        win.remove_move_listener(self.listeners[win].move)

        del self.listeners[win]
        
        self.untie(win)
        

    
    
    
    def _on_window_resize(self, win, width, height):
        """callback for when a window resizes"""
    
        # ignore windows that have been changed by the ensemble
        size = (width, height)
        if size in self.recentSize[win]:
            ind = self.recentSize[win].index(size)
            self.recentSize[win] = self.recentSize[win][ind+1:]
    
        # process windows that have been changed by outside forces
        elif self.sizes[win] != (width, height):
            if self.stackx or self.stacky:
                self.stack(win)
            else:
                self.align(win)
            self.raise_windows(win)
    
    
    def _on_window_move(self, win, x, y):
        """callback for when a window moves"""
    
        # ignore windows that have been changed by the ensemble
        pos = (x, y)
        if pos in self.recentPos[win]:
            ind = self.recentPos[win].index(pos)
            self.recentPos[win] = self.recentPos[win][ind+1:]
        
        # process windows that have been changed by outside forces        
        elif self.pos[win] != (x, y):
            if self.stackx or self.stacky:
                self.stack(win)
            else:
                self.align(win)
            self.raise_windows(win)
                
    
    def stack(self, win):
        """restack windows together"""
        
        target_pos = win.get_position()
        target_size = win.get_size()
        self.pos[win] = target_pos
        self.sizes[win] = target_size
        
        # get window sizes
        widths = []
        heights = []
        x = []
        y = []
        totalx = 0
        totaly = 0
        target = []
        
        for win2 in self.windows:
            # update size
            if win2 == win:
                w, h = target_size
                
                # determine destination positions
                target = [totalx, totaly]
            else:
                w2, h2 = win2.get_size()
            
                if self.samew:
                    w = target_size[0]
                else:
                    w = w2
                if self.sameh:
                    h = target_size[1]
                else:
                    h = h2
            
                if (w,h) != (w2, h2):
                    self.recentSize[win2].append((w,h))
                    self.sizes[win2] = (w, h)
                    win2.set_size(w, h)
            
            widths.append(w)
            heights.append(h)
            x.append(totalx)
            y.append(totaly)
            deco = win2.get_decoration()
            totalx += w + deco[0]
            totaly += h + deco[1]
        
        # set window positions
        for i, win2 in enumerate(self.windows):
            if win == win2:
                continue
            
            if self.stackx:
                newx = target_pos[0]
                newy = target_pos[1] + y[i] - target[1]
            elif self.stacky:
                newx = target_pos[0] + x[i] - target[0]
                newy = target_pos[1]
            
            oldpos = self.pos[win2] #win2.get_position()
            self.pos[win2] = (newx, newy)
            
            if (newx, newy) != oldpos:
                win2.set_position(newx, newy)
                self.recentPos[win2].append((newx, newy))
        
            
    def align(self, win):
        """move all windows the same amount window 'win' has moved"""
        now = win.get_position()
        now = [now[0], now[1]]
        pos1 = self.pos[win]
        
        # move all other windows to match moved window
        for win2 in self.windows:
            if win2 != win:
                pos2 = self.pos[win2]
                pos3 = [now[0] + pos2[0] - pos1[0],
                        now[1] + pos2[1] - pos1[1]]
                win2.set_position(*pos3)
                self.recentPos[win2].append(tuple(pos3))
                self.pos[win2] = pos3

        # record new position for main window
        self.pos[win] = now
    
    
    def tie(self, windows, tiex=False, tiey=False, pinx=False, piny=False,
                  coordsx=None, coordsy=None, master=None):
        """ties the scrolling and zooming of multiple windows together"""
        
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
    

    def untie(self, win=None):
        """remove a window from any ties"""
        
        if win == None:
            # untie all windows
            for win2 in self.windows:
                self.untie(win2)
        else:
            if win not in self.ties:
                return
        
            win.remove_view_change_listener(self.ties[win].update_scroll)
            win.remove_focus_change_listener(self.ties[win].update_focus)        

            del self.ties[win]

            # make sure window ties remove their callbacks
            for tie in self.ties.itervalues():
                tie.remove_window(win)


    def raise_windows(self, top=None):
        """raises all windows in ensemble above other windows on the desktop"""
        for win in self.windows:
            win.raise_window(True)
        
        if top != None:
            top.raise_window(True)
            



class WindowTie:
    """This class coordinates the translation and zoom of multiple SUMMON Windows.
    """

    def __init__(self, win, others, ensemble):
        self.win = win
        self.others = others
        self.ensemble = ensemble

    def remove_window(self, win):
        """removes a window from the list of tied windows"""
        if win in self.others:
            self.others.remove(win)
    
    
    def update_scroll(self):
        """call back that sets translation and zoom"""

        # prevent infinite loops
        if self.ensemble.lock:
            return
        self.ensemble.lock = True
                
        
        w1 = self.win
        others = self.others
        coords = self.ensemble.coords
        
        needpin = self.ensemble.pinx or self.ensemble.piny
        
        if needpin:
            pos1 = w1.get_position()
        trans1 = w1.get_trans()
        zoom1 = w1.get_zoom()
        
        
        for w2 in others:
            if needpin:
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
        self.ensemble.lock = False


        
    def update_focus(self):
        """callback that sets focus"""
        
        # prevent infinite loops     
        if self.ensemble.lock:
            return
        self.ensemble.lock = True
        
        coords = self.ensemble.coords        
        
        fx1, fy1 = self.win.get_focus()
        fx1 -= coords[self.win].x
        fy1 -= coords[self.win].y

        for w2 in self.others:
            newpos = (fx1 + coords[w2].x, fy1 + coords[w2].y)
            oldpos = w2.get_focus()

            if newpos != oldpos:
                w2.set_focus(* newpos)
        self.ensemble.lock = False



