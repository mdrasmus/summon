
from summon.core import *
import summon


def tie_windows(windows, tiex=False, tiey=False, pinx=False, piny=False):
    # call back that sets translation and zoom
    def tie_scroll(w1, others):
        def update_view():
            pos1 = w1.get_position()
            trans1 = w1.get_trans()
            zoom1 = w1.get_zoom()
            
            for w2 in others:
                pos2 = w2.get_position()
                trans2 = list(w2.get_trans())
                zoom2 = list(w2.get_zoom())
            
                if tiex:
                    trans2[0] = trans1[0]
                    zoom2[0] = zoom1[0]
                
                    if pinx:
                        trans2[0] += pos1[0] - pos2[0]
            
                if tiey:
                    trans2[1] = trans1[1]
                    zoom2[1] = zoom1[1]

                    if piny:
                        trans2[1] -= pos1[1] - pos2[1]

                w2.set_trans(*trans2)
                w2.set_zoom(*zoom2)
        return update_view
    
    # callback that sets focus
    def tie_focus(w1, others):
        def update_view():
            focus1 = w1.get_focus()
            for w2 in others:
                w2.set_focus(*focus1)
        return update_view
    
    # set callbacks for each window
    for win in windows:
        others = windows[:]
        others.remove(win)
        
        t1 = tie_scroll(win, others)
        t2 = tie_focus(win, others)
        win.add_binding(input_motion("left", "down"), t1)
        win.add_binding(input_click("right", "down"), t2)
        win.add_binding(input_click("right", "down", "ctrl"), t2)
        win.add_binding(input_click("right", "down", "shift"), t2)
        win.add_binding(input_motion("right", "down"), t1)
        win.add_binding(input_motion("right", "down", "ctrl"), t1)
        win.add_binding(input_motion("right", "down", "shift"), t1)



class WindowEnsembl (summon.VisObject):
    def __init__(self, windows, stackx=False, stacky=False, 
                                samew=False, sameh=False):
        self.windows = windows[:]
        self.pos = {}
        self.sizes = {}
        self.stackx = stackx
        self.stacky = stacky
        self.samew = samew
        self.sameh = sameh
        
        # determine window decoration offset
        pos1 = windows[0].get_position()
        windows[0].set_position(*pos1)
        # wait for position to change
        pos2 = pos1
        while pos2 == pos1:
            pos2 = windows[0].get_position()
        
        self.offset = [pos1[0] - pos2[0], pos1[1] - pos2[1]]
        
        windows[0].set_position(pos1[0] + self.offset[0], 
                                pos1[1] + self.offset[1])
        
        # setup stacking
        if stackx or stacky:
            self.stack(self.windows[0])

        # record window positions and sizes
        for win in windows:
            pos = win.get_position()
            self.pos[win] = [pos[0] + self.offset[0], pos[1] + self.offset[1]]
            self.sizes[win] = win.get_size()
        
        # enable updating
        self.win = windows[0]
        self.enableUpdating()
    
    
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
            