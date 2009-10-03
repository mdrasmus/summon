
import math


class Multiscale (object):
    """Manage detecting when the zoom and scroll of the visualization is 
       sufficently different to justify a redraw"""

    def __init__(self, marginx=.5, marginy=.5, scalex=4, scaley=4):
        self.worldx1 = None
        self.worldx2 = None
        self.worldy1 = None
        self.worldy2 = None
        self.marginx = marginx
        self.marginy = marginy
        self.scalex = scalex
        self.scaley = scaley      
        self.win    = None
        self.reseted = False
        
    
    def init(self, win, view=None):
        self.win = win
        
        if view == None:
            view = win.get_visible()
        self.worldx1, self.worldy1, self.worldx2, self.worldy2 = view
        
        # define outer bound with margins
        self.worldwidth = self.worldx2 - self.worldx1
        self.worldheight = self.worldy2 - self.worldy1
        marginx = self.worldwidth * self.marginx
        marginy = self.worldheight * self.marginx
        
        self.worldx1 -= marginx
        self.worldx2 += marginx
        self.worldy1 -= marginy
        self.worldy2 += marginy

    
    def reset(self):
        self.reseted = True
                
    
    def same_view(self, view=None):
        if self.reseted:
            self.reseted = False
            return False
    
        if view == None:
            view = self.win.get_visible()
        
        worldx1, worldy1, worldx2, worldy2 = view
        
        # test for scrolling
        if worldx1 < self.worldx1 or \
           worldx2 > self.worldx2 or \
           worldy1 < self.worldy1 or \
           worldy2 > self.worldy2:
            self.init(self.win, view=view)
            return False
        
        worldwidth = worldx2 - worldx1
        worldheight = worldy2 - worldy1
        
        if self.worldwidth == 0 or \
           self.worldheight == 0:
            return True
        
        # test for zooming
        if abs(math.log10(worldwidth / self.worldwidth)) > 1./self.scalex or \
           abs(math.log10(worldheight / self.worldheight)) > 1./self.scaley:
            self.init(self.win, view=view)
            return False
        
        return True


    def atleast(self, xminres, yminres, view=None, size=None):       
        if view == None:
            view = self.win.get_visible()
        if size == None:
            size = self.win.get_size()
        
        worldx1, worldy1, worldx2, worldy2 = view
        screenwidth, screenheight = size
        worldwidth = worldx2 - worldx1
        worldheight = worldy2 - worldy1
        
        return (worldwidth == 0 or
                screenwidth / worldwidth > xminres) and \
               (worldheight == 0 or 
                screenheight / worldheight > yminres)

