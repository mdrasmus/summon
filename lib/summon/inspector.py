
from summon.core import *
import summon
from summon import shapes, colors, util, treelib


def inspect_window(win):
    vis = Inspector(win.get_root(), win.get_name())
    vis.show()
    return vis




class Inspector (object):
    def __init__(self, element, title="inspect"):
        self.element = element
        self.title = title
        self.win = None
    
    def show(self):
        if self.win == None:
            self.win = summon.Window(self.title)    
        else:
            self.win.clear_groups()
        
        vis = GroupInspect(self.element)
        
        self.win.add_group(vis.draw())
        
    
class GroupInspect (object):
    def __init__(self, element, x=0, y=0, width=100, height=20):
        self.element = element
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.children_vis = None
        self.children = []
    
    def draw(self):
        x1, y1, x2, y2 = self.x, self.y, self.x+self.width, self.y+self.height
        midy = (y1 + y2) / 2.0
        
        self.vis = group(color(0, 0, 1),
                     shapes.box(x1, y1, x2, y2),
                     color(1, 1, 1),
                     shapes.box(x1, y1, x2, y2, fill=False),
                     zoom_clamp(                    
                         text_scale(self.element.__class__.__name__,
                                    x1, midy, x2, y2,
                                    "center", "middle"),
                         maxx=2, maxy=2,
                         minx=.1, miny=.1,
                         link=True, link_type="smaller",
                         origin=((x1+x2)/2.0, y2-(y1+y2)/4.0)),
                     hotspot("click", x1, midy, x2, y2, self.expand))
        return self.vis
    
    
    def expand(self):
        x1, y1, x2, y2 = self.x, self.y, self.x+self.width, self.y+self.height
        midy = (y1 + y2) / 2.0
    
    
        if self.children_vis == None:
        
            children = self.element.get_children()
            
            if len(children) > 0:
                xspacing = self.width #/ float(len(children))
                yspacing = 0
                startx = 0 #self.x #self.x - xspacing * len(children) / 2.0

                self.children_vis = group()
                for i, child in enumerate(children):
                    self.children.append(GroupInspect(child, startx + i * xspacing,
                                                      self.y + yspacing,
                                                      self.width,
                                                      self.height))
                    self.children_vis.append(self.children[-1].draw())
                self.vis.append(translate(self.x, self.y, 
                                scale(1.0 / float(len(children)),
                                      .5, self.children_vis)))
            else:
                self.vis.append(zoom_clamp(color(1, 1, 1),
                         text_scale(str(self.element.get()),
                                    x1, y1, x2, (y2 + y1)/2.0,
                                    "center", "middle"),
                         link=True, link_type="smaller",
                         origin=((x1+x2)/2.0, (y1+y2)/2.0)))
            
        else:
            self.children_vis.remove_self()
            self.children = []
            self.children_vis = None
