"""

    SUMMON - Mouse selection
    
    This module provides the ability to select regions on a summon window using 
    mouse clicks and drags.

"""

from summon.core import *
from summon import shapes


class Select:
    """This class allows the selecting of regions on a summon window with the
       mouse.
    """

    def __init__(self, win, func, mindrag=3, strokeColor=None, fillColor=None,
                       coords="world"):
        self.win = win    
        self.func = func
        self.mindrag = mindrag
        self.coords = coords
        
        if strokeColor == None:
            if win.get_bgcolor() == (0, 0, 0):
                self.strokeColor = (1, 1, 1, 1)
            else:
                self.strokeColor = (0, 0, 0, 0)
        else:
            self.strokeColor = strokeColor
        
        if fillColor == None:
            self.fillColor = (0, 0, 0, 0)
        else:
            self.fillColor = fillColor
        
        
        self.pos1 = None
        self.pos2 = None
        self.worldpos1 = None
        self.worldpos2 = None
        self.selbox = None    
    
        self.start()    
    
    def start(self):
        """initialize mouse bindings"""
        
        # setup bindings
        self.win.add_binding(input_click("middle", "down"), self.click)
        self.win.add_binding(input_motion("middle", "down"), self.drag)
        self.win.add_binding(input_click("middle", "up"), self.done)
        
    
    def click(self):
        """starting click"""
        
        self.pos1 = list(self.win.get_mouse_pos("screen"))
        self.worldpos1 = list(self.win.get_mouse_pos("world"))

    
    def drag(self):
        """process drag event"""
        
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
