#!/usr/bin/env python-i

import math

from summon.core import *
import summon
from summon import shapes, colors, util



win = summon.Window("arrows")

radius = 100
for angle in util.frange(0, 2*math.pi, math.pi / 10):
    x = radius * math.cos(angle)
    y = radius * math.sin(angle)
    win.add_group(shapes.arrow(x, y, 0, 0))
    #win.add_group(rotate(180/math.pi*angle, shapes.arrow(100, 0, 0, 0)))

win.home()
