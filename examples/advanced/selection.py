#!/usr/bin/env python-i

from summon.core import *
import summon
from summon import shapes, select





def box(win, pos1, pos2):
    def func():
        win.remove_group(g)
        
    g = group(color(0, 1, 0),
              shapes.box(pos1[0], pos1[1], pos2[0], pos2[1]),
              hotspot("click", pos1[0], pos1[1], pos2[0], pos2[1], func))
    return g



def selected(pos1, pos2):
    win.add_group(box(win, pos1, pos2))


win = summon.Window("selection")
sel = select.Select(win, selected, strokeColor=(1, .5, 1, .8), 
                            fillColor=(0, .5, 1, .5))

