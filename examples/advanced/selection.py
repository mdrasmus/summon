#!/usr/bin/env python-i

from summon.core import *
import summon
from summon import shapes, select



# experimenting with drag
# I may need to make drag a primitive...

def box(win, pos1, pos2):
    pos1 = list(pos1)
    pos2 = list(pos2)

    def func():
        win.drag = True
        
        if pos == []:
            pos[:] = list(win.get_mouse_pos("world"))
        else:
            click = list(win.get_mouse_pos("world"))
            
            vx = click[0] - pos[0]
            vy = click[1] - pos[1]
            pos[:] = click
            
            pos1[0] += vx
            pos1[1] += vy
            pos2[0] += vx
            pos2[1] += vy
            
            g[0] = win.replace_group(g[0], group(color(0, 1, 0),
                   shapes.box(pos1[0], pos1[1], pos2[0], pos2[1]),
                   hotspot("click", pos1[0], pos1[1], pos2[0], pos2[1], func)))
            

    pos = []        
    g = [None]
    g[0] = group(color(0, 1, 0),
              shapes.box(pos1[0], pos1[1], pos2[0], pos2[1]),
              hotspot("click", pos1[0], pos1[1], pos2[0], pos2[1], func))
    return g[0]



def selected(pos1, pos2):
    win.add_group(box(win, pos1, pos2))


win = summon.Window("selection")
win.drag = False
win.clear_binding(input_click("middle", "up"))
win.set_binding(input_motion("middle", "down", "ctrl"), "hotspot_click")
sel = select.Select(win, selected, strokeColor=(1, .5, 1, .8), 
                            fillColor=(0, .5, 1, .5))

