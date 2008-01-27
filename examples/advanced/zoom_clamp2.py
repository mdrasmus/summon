#!/usr/bin/env python-i


import summon
from summon.core import *
from summon import shapes, colors

step = 50


win = summon.Window("zoom_clamp2")

vis = []
for i in xrange(101):
    for j in xrange(101):
        s = .01 * i
    
        vis.append(translate(i*step, j*step, 
                             zoom_clamp(colors.red, shapes.regular_polygon(0, 0, 10, 5),
                                        minx=s, miny=s, maxx=1, maxy=1)))

win.add_group(group(*vis))
win.add_group(group(colors.white, shapes.box(0, 0, 100*step, 100*step, fill=False)))

win.home()
