#!/usr/bin/env python-i

import math

import summon
from summon.core import *
from summon import util, shapes, colors

win = summon.Window("18_hotspot_shapes")


def func(x, y):
    print "click", x, y

def func2(x, y):
    print "box", x, y

pts = [0, 0, 
       -10, 100, 
       50, 150,
       70, 120,
       65, -10]

win.add_group(
    group(
        colors.red,
        shapes.box(-10, -10, 10, 10),
        text_clip("origin", -10, -10, 10, 10, 4, 20, "middle", "center"),
        
        translate(100, 100,
            rotate(5, 
                scale(2, .5,
                    colors.green,
                    shapes.box(-50, -50, 50, 50),
                    hotspot("click", -50, -50, 50, 50, func2, True),
                    
                    colors.blue, 
                    shapes.regularPolygon(0, 0, 40, 50),
                    hotspot_circle("click", 0, 0, 50, func, give_pos=True)
                ))),
        
        translate(400, 0,
            color(.7, .3, .9),
            polygon(*pts),
            hotspot_polygon("click", pts, func, give_pos=True)
        )
    ))
win.home()

