"""

    SUMMON - Heads Up Display (HUD)

    This module contains the interface for a simple implementation of a HUD.
    
"""

import math

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


#=============================================================================

def get_ruler_auto_size(screenwidth, worldwidth, pixels=50):
    """get most appropriate unit for zoom level"""
    
    if worldwidth == 0.0:
        return 1.0
    order = min(math.ceil(math.log10(worldwidth * pixels /
                                     float(screenwidth))), 30)    
    return 10**order


def get_unit_suffix(unit):
    """get the sufffix for a unit"""

    order = math.log10(max(unit, 1e-300))

    if order < -1:
        unitstr = "e" + str(order)
        unit2 = unit
    elif order < 3:
        unitstr = ""
        unit2 = 1
    elif 3 <= order < 6:
        unitstr = "K"
        unit2 = 1000
    elif 6 <= order < 9:
        unitstr = "M"
        unit2 = 1e6
    elif 9 <= order < 12:
        unitstr = "G"
        unit2 = 1e9
    elif 12 <= order < 15:
        unitstr = "T"
        unit2 = 1e12
    elif 15 <= order:
        unitstr = "e" + str(order)
        unit2 = unit
    
    return unit2, unitstr


class RulerHud (object):
    def __init__(self, win, height=40, width=40, top=True, left=True,
                 minunit=1, 
                 bgcolor=None, fgcolor=None, fgcolor2=None):
        self.height = height
        self.width = width
        self.minunit = minunit
        self.sides = set()
        if top:
            self.sides.add("top")
        if left:
            self.sides.add("left")
            
        self.bgcolor = bgcolor if bgcolor else win.get_bgcolor()
        self.fgcolor = fgcolor if fgcolor else (
            1-self.bgcolor[0], 1-self.bgcolor[1], 1-self.bgcolor[2])

        mix = .5; mix2 = 1 - mix
        self.fgcolor2 = fgcolor2 if fgcolor2 else (
            mix*self.fgcolor[0] + mix2*.5,
            mix*self.fgcolor[1] + mix2*.5,
            mix*self.fgcolor[2] + mix2*.5, .2)
        self.visible = True
        
        self.g = group()
        self.add(win)
        

    def add(self, win):
        """Add ruler to a summon.Window 'win'"""

        self.win = win

        # install callbacks
        self.win.add_resize_listener(self.on_resize)
        self.win.add_view_change_listener(self.on_scroll)
        
        self.win.screen.add_group(self.g)
        self.update()
        

    def remove(self):
        """Remove a ruler from its window"""

        if self.win:
            # uninstall callbacks
            self.win.remove_resize_listener(self.on_resize)
            self.win.remove_view_change_listener(self.on_scroll)
            
            self.g.remove_self()
            self.g.clear()
            self.win = None
            

    def show(self):
        """Shows the ruler"""
        self.g.set_visible(True)
        self.visible = True
        
    def hide(self):
        """Hides the ruler"""
        self.g.set_visible(False)
        self.visible = False

    def toggle(self):
        """Toggles the ruler's visibility"""
        self.visible = not self.visible
        self.g.set_visible(self.visible)
    
    def on_resize(self, w, h):
        """Callback for window resize"""
        self.update()

    def on_scroll(self):
        """Callback for window scroll"""
        self.update()

    def update(self):
        """Update the display of a ruler"""

        self.g.clear()

        g = group()
        if "top" in self.sides:
            g.append(self.draw_horizontal_ruler())
        if "left" in self.sides:
            g.append(self.draw_vertical_ruler())
        if "top" in self.sides and "left" in self.sides:
            g.append(self.draw_corner())
        self.g.append(g)


    def draw_horizontal_ruler(self):
        g = group()

        # get window dimensions
        screen_width, screen_height = self.win.get_size()
        x1, y1, x2, y2 = self.win.get_visible()
        world_width = x2 - x1

        # layout ruler
        left = 0
        right = screen_width
        top = screen_height
        bot = screen_height - self.height
        xscale = world_width / float(screen_width)

        step = get_ruler_auto_size(screen_width, world_width)
        unit, unitstr = get_unit_suffix(step)

        # ruler background
        g.append(group(
            color(*self.bgcolor), quads(left, top, right, top,
                                        right, bot, left, bot),
            color(*self.fgcolor2),
            lines(left, bot, right, bot)))

        # make mini hashes        
        if step >= self.minunit*10 and world_width / step < 1000:
            step2 = step // 10
            l = []
            worldi = (x1 // step2) * step2
            while worldi < x2:
                screeni = (worldi - x1) / xscale
                l.extend([screeni, top, screeni, bot])
                worldi += step2
            g.append(lines(color(*self.fgcolor2), *l))

        # make main hashes
        if step >= self.minunit and world_width / step < 1000:
            l = []; t = group(color(*self.fgcolor))
            worldi = (x1 // step) * step
            while worldi < x2:
                screeni = (worldi - x1) / xscale
                l.extend([screeni, top, screeni, bot])
                t.append(
                    text("%s%s" % (int(worldi // unit), unitstr),
                         screeni - step/xscale, top, screeni+1, bot,
                         "middle", "right"))
                worldi += step
            g.append(group(lines(color(*self.fgcolor), *l), t))

        return g



    def draw_vertical_ruler(self):
        g = group()

        # get window dimensions
        screen_width, screen_height = self.win.get_size()
        x1, y1, x2, y2 = self.win.get_visible()
        world_height = y2 - y1

        # layout ruler
        left = 0
        right = self.width
        top = screen_height
        bot = 0
        yscale = world_height / float(screen_height)

        step = get_ruler_auto_size(screen_height, world_height)
        unit, unitstr = get_unit_suffix(step)

        # ruler background
        g.append(group(
            color(*self.bgcolor), quads(left, top, right, top,
                                        right, bot, left, bot),
            color(*self.fgcolor2),
            lines(right, top, right, bot)))

        # make mini hashes
        if step >= 10 and world_height / step < 1000:
            step2 = step // 10
            l = []
            worldi = (y1 // step2) * step2
            while worldi < y2:
                screeni = (worldi - y1) / yscale
                l.extend([left, screeni, right, screeni])
                worldi += step2
            g.append(lines(color(*self.fgcolor2), *l))

        # make main hashes
        if step >= 1 and world_height / step < 1000:
            l = []; t = group(color(*self.fgcolor))
            worldi = (y1 // step) * step
            while worldi < y2:
                screeni = (worldi - y1) / yscale
                l.extend([right, screeni, left, screeni])
                t.append(
                    text("%s%s" % (int(worldi // unit), unitstr),
                         left, screeni + 100, right, screeni,
                         "center", "bottom"))
                worldi += step
            g.append(group(lines(color(*self.fgcolor), *l), t))

        return g


    def draw_corner(self):

        # get window dimensions
        screen_width, screen_height = self.win.get_size()

        # layout ruler
        left = 0
        right = self.width
        top = screen_height
        bot = top - self.height

        # ruler background
        return group(quads(color(*self.bgcolor),
                           left, top, right, top,
                           right, bot, left, bot),
                     lines(color(*self.fgcolor2), right, top, right, bot,
                           right, bot, left, bot))
        
        
