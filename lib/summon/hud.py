"""

    SUMMON
    Heads Up Display (HUD)

    This module contains the interface for a simple implementation of a HUD.
    
"""

import summon
from summon import shapes
from summon.core import *



class MenuItem (object):
    def __init__(self, name, func, buttonColor=color(.7, .7, .9), 
                 textColor=color(0, 0, 0), height=15, width=50):
        self.name = name
        self.func = func
        self.buttonColor = buttonColor
        self.textColor = textColor
        self.height = height
        self.width = width

    def draw(self):
        return group(self.buttonColor,
                     shapes.box(0, 0, self.width, self.height),
                     self.textColor,
                     text(self.name, 0, 2, self.width, self.height+20, 
                          "bottom", "center"),
                     hotspot("click", 0, 0, self.width, self.height,
                                      self.func))
        

class SideBar (object):
    def __init__(self, win, width=100, baseColor=color(.5, .5, .7),
                 showkey=input_key(" ")):
        self.win = win
        self.width = width
        self.baseColor = baseColor
        self.items = []
        
        self.xmargin = 2
        self.ymargin = 2
        self.gid = None
        self.shown = False
        
        # install callbacks
        win.on_resize = self.onResize
        win.set_binding(showkey, self.toggleMenu)
    
    
    def addItem(self, item):
        self.items.append(item)
        item.width = self.width - 2 * self.xmargin
    
    
    def show(self):
        self.shown = True
        if self.gid != None:
            self.win.screen.remove_group(self.gid)
        
        # show menu
        self.gid = self.win.screen.add_group(self.draw(*self.win.get_size()))
    
    
    def hide(self):
        self.shown = False
        if self.gid != None:
            self.win.screen.remove_group(self.gid)
    
    
    def toggleMenu(self):
        self.shown = not self.shown
        
        if self.shown:
            self.show()
        else:
            self.hide()
    
    
    def draw(self, winwidth, winheight):
        height = self.getHeight()
        
        vis = [self.baseColor, 
               shapes.box(winwidth, winheight,
                          winwidth-self.width, winheight-height)]
        
        x = winwidth - self.width + self.xmargin 
        y = winheight - self.ymargin
        for item in self.items:
            y -= item.height        
            vis.append(translate(x, y, item.draw()))
            y -= self.ymargin
        
        return group(*vis)
    
    
    def getHeight(self):
        height = self.ymargin
        for item in self.items:
            height += item.height + self.ymargin
        return height
    
    
    def onResize(self, width, height):
        self.gid = self.win.screen.replace_group(self.gid, self.draw(width, height))


