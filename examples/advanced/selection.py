#!/usr/bin/env python-i

from summon.core import *
import summon
from summon import shapes, select


class Drag (object):
    def __init__(self):
        self.state = "none"

    def start(self):
        self.state = "start"
        x, y = win.get_mouse_pos("window")
        win.hotspot_click(int(x), int(y))
    
    def done(self):
        self.state = "done"
        win.select.enable()
        x, y = win.get_mouse_pos("window")
        win.hotspot_click(int(x), int(y))
    
    def drag(self):
        self.state = "drag"
        x, y = win.get_mouse_pos("window")
        win.hotspot_click(int(x), int(y))
        

drag = Drag()

# experimenting with drag
# I may need to make drag a primitive...



def box(win, pos1, pos2):
    pos1 = list(pos1)
    pos2 = list(pos2)

    def func(event, x, y):
        if event == "drag_start":
            pos[:] = [x, y]
        
        elif event == "drag_stop":
            pos[:] = []

            
        elif event == "drag":
            if len(pos) == 0:
                return
            click = [x, y]

            vx = click[0] - pos[0]
            vy = click[1] - pos[1]
            pos[:] = click

            pos1[0] += vx
            pos1[1] += vy
            pos2[0] += vx
            pos2[1] += vy
            
            g[0] = win.replace_group(g[0], group(
                   makebox(pos1[0], pos1[1], pos2[0], pos2[1]),
                   hotspot("drag", pos1[0], pos1[1], pos2[0], pos2[1], func)))

    def makebox(x1, y1, x2, y2):
        return group(color(0, 0, 0, .5),
                     shapes.box(x1+4, y1-4, x2+4, y2-4),
                     color(0, 1, 0),
                     shapes.box(x1, y1, x2, y2),
                     color(1, 1, 1),
                     shapes.box(x1, y1, x2, y2, fill=False))

    pos = []        
    g = [None]
    g[0] = group(color(0, 1, 0),
              makebox(pos1[0], pos1[1], pos2[0], pos2[1]),
              hotspot("drag", pos1[0], pos1[1], pos2[0], pos2[1], func))
    return g[0]



def selected(pos1, pos2):
    win.add_group(box(win, pos1, pos2))


win = summon.Window("selection")
win.drag = False
win.add_binding(input_click("left", "up", "shift"), "hotspot_drag_stop")
win.add_binding(input_click("left", "down", "shift"), "hotspot_drag_start")
win.add_binding(input_motion("left", "down", "shift"), "hotspot_drag")
win.select.set_callback(selected)
#sel = select.Select(win, selected, strokeColor=(1, .5, 1, .8), 
#                         fillColor=(0, .5, 1, .5))

