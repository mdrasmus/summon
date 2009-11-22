"""

    SUMMON - Heads Up Display (HUD)

    This module contains the interface for a simple implementation of a HUD.
    
"""

import summon
from summon import shapes
from summon.core import *



class MenuItem (object):
    def __init__(self, name, func, button_color=(.9, .9, .9), 
                 button_highlight=(1,1,1),
                 text_color=(0, 0, 0), height=15, width=50,
                 model=None):
        self.name = name
        self.func = func
        self.button_color = button_color
        self.button_highlight = button_highlight
        self.text_color = text_color
        self.height = height
        self.width = width
        self.model = model
        
        self.drawGroup = None
    
    def set_model(self, model):
        self.model = model
    setModel = set_model
    
    def on_click(self):
        #if self.model != None:
        #    self.model.show_group(self.drawGroup, False)
        self.func()
    onClick = on_click
        
        
    def draw(self):
        self.drawGroup = group(color(*self.button_color),
                     shapes.box(0, 0, self.width, self.height),
                     color(*self.button_highlight),
                     lines(0, self.height-1, self.width-1, self.height-1,
                           self.width, 0, self.width-1, self.height-1),
                     color(*self.text_color),
                     text(self.name, 0, 2, self.width, self.height+20, 
                          "bottom", "center"),
                     hotspot("click", 0, 0, self.width, self.height,
                                      self.onClick))

        return self.drawGroup
        

class SideBar (object):
    def __init__(self, win, width=100, base_color=(.7, .7, .7),
                 showkey=input_key(" ")):
        self.win = win
        self.width = width
        self.base_color = base_color
        self.items = []
        
        self.xmargin = 2
        self.ymargin = 2
        self.gid = None
        self.shown = False
        
        self.shadow_size = (-4, 4)
        self.shadow_color = (0, 0, 0, .5)
        
        # install callbacks
        win.add_resize_listener(self.on_resize)
        win.set_binding(showkey, self.toggle_menu)
    
    
    def delete(self):
        self.win.remove_resize_listener(self.on_resize)
        
    
    def add_item(self, item):
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
            self.gid = None
    
    
    def toggle_menu(self):
        self.shown = not self.shown
        
        if self.shown:
            self.show()
        else:
            self.hide()
    
    def draw(self, winwidth, winheight):
        height = self.get_height()
        
        vis = [color(*self.shadow_color), 
               shapes.box(winwidth, winheight,
                          winwidth-self.width-self.shadow_size[0], 
                          winheight-height-self.shadow_size[1]),
               color(*self.base_color), 
               shapes.box(winwidth, winheight,
                          winwidth-self.width, winheight-height)]
        
        x = winwidth - self.width + self.xmargin 
        y = winheight - self.ymargin
        for item in self.items:
            y -= item.height     
            item.set_model(self.win.screen)
            vis.append(translate(x, y, item.draw()))
            y -= self.ymargin
        
        return group(*vis)
    
    
    def get_height(self):
        height = self.ymargin
        for item in self.items:
            height += item.height + self.ymargin
        return height
    
    
    def on_resize(self, width, height):
        if self.shown:
            self.gid = self.win.screen.replace_group(self.gid, 
                                                     self.draw(width, height))


