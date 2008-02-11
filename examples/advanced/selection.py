#!/usr/bin/env python-i

from summon.core import *
import summon
from summon import shapes, select


# experimenting with drag
# I may need to make drag a primitive...



def box(pos1, pos2):
    x1, y1 = pos1
    x2, y2 = pos2
    
    return shapes.draggable(
        summon.Region(x1, y1, x2, y2),
        group(color(0, 0, 0, .5),
             shapes.box(x1+4, y1-4, x2+4, y2-4),
             color(0, 1, 0),
             shapes.box(x1, y1, x2, y2),
             color(1, 1, 1),
             shapes.box(x1, y1, x2, y2, fill=False)))



def selected(pos1, pos2):
    win.add_group(box(pos1, pos2))


win = summon.Window("selection")
win.add_binding(input_click("left", "up", "shift"), "hotspot_drag_stop")
win.add_binding(input_click("left", "down", "shift"), "hotspot_drag_start")
win.add_binding(input_motion("left", "down", "shift"), "hotspot_drag")
win.select.set_callback(selected)
#sel = select.Select(win, selected, strokeColor=(1, .5, 1, .8), 
#                         fillColor=(0, .5, 1, .5))

