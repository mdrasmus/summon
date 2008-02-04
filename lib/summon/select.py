"""

    SUMMON - Mouse selection
    
    This module provides the ability to select regions on a summon window using 
    mouse clicks and drags.

    NOTE: This is still experimental.
"""

from summon.core import *
from summon import shapes


class Select:
    """This class allows the selecting of regions on a summon window with the
       mouse.
    """

    def __init__(self, win, func, mindrag=3, stroke_color=None, fill_color=None,
                       coords="world"):
        self.win = win    
        self.func = func
        self.mindrag = mindrag
        self.coords = coords
        self.enabled = True
        
        if stroke_color == None:
            if win.get_bgcolor() == (0, 0, 0):
                self.strokeColor = (1, 1, 1, 1)
            else:
                self.strokeColor = (0, 0, 0, 1)
        else:
            self.strokeColor = stroke_color
        
        if fill_color == None:
            self.fillColor = (0, .5, 1, .2)
        else:
            self.fillColor = fill_color
        
        
        self.pos1 = None
        self.pos2 = None
        self.worldpos1 = None
        self.worldpos2 = None
        self.selbox = None
    
    
    
    def set_fill_color(self, col):
        self.fillColor = col
    
    def set_stroke_color(self, col):
        self.strokeColor = col
        
    def set_min_drag(self, mindrag):
        self.mindrag = mindrag
    
    def set_callback(self, func):
        self.func = func
    
    def set_coords(self, coords):
        self.coords = coords
    
    
    #==========================================
    # callbacks
    
    def click(self):
        """starting click"""
        
        self.pos1 = list(self.win.get_mouse_pos("screen"))
        self.worldpos1 = list(self.win.get_mouse_pos("world"))

    
    def drag(self):
        """process drag event"""
        
        # do nothing if selection is not enabled
        if not self.enabled:
            return
        
        self.pos2 = list(self.win.get_mouse_pos("screen"))
        self.worldpos2 = list(self.win.get_mouse_pos("world"))
        
        # do nothing if drag is not large enough
        if abs(self.pos1[0] - self.pos2[0]) < self.mindrag and \
           abs(self.pos1[1] - self.pos2[1]) < self.mindrag:
            return
        
        if self.selbox == None:
            self.selbox = self.win.screen.add_group(group())
        else:
            self.selbox = self.win.screen.replace_group(self.selbox,
                            group(color(*self.fillColor),
                                  shapes.box(self.pos1[0], self.pos1[1],
                                             self.pos2[0], self.pos2[1],
                                             fill=True),                          
                                  color(*self.strokeColor),
                                  shapes.box(self.pos1[0], self.pos1[1],
                                             self.pos2[0], self.pos2[1],
                                             fill=False)
                                  ))

    def done(self):
        """process a completed selection"""
        
        if self.selbox != None:
            self.win.screen.remove_group(self.selbox)
            self.selbox = None
            
            # swap positions into a consistent manner
            if self.pos1[0] > self.pos2[0]:
                self.pos1[0], self.pos2[0] = self.pos2[0], self.pos1[0]
            if self.pos1[1] > self.pos2[1]:
                self.pos1[1], self.pos2[1] = self.pos2[1], self.pos1[1]
                
            if self.worldpos1[0] > self.worldpos2[0]:
                self.worldpos1[0], self.worldpos2[0] = \
                    self.worldpos2[0], self.worldpos1[0]
            if self.worldpos1[1] > self.worldpos2[1]:
                self.worldpos1[1], self.worldpos2[1] = \
                    self.worldpos2[1], self.worldpos1[1]


            # call callback function
            if self.coords == "world":
                self.func(self.worldpos1, self.worldpos2)
            elif self.coords == "screen":
                self.func(self.pos1, self.pos2)
            else:
                raise Exception("Unknown coordinate system '%s'" % self.coords)


    def abort(self):
        """abort a selection in progress"""
        
        if self.selbox != None:
            self.win.screen.remove_group(self.selbox)
            self.selbox = None


    def enable(self, enable=True):
        """enable/disable selection"""
        self.enabled = enable
    
    def is_enabled(self):
        """returns True if selection is enabled"""
        return enabled
