#!/usr/bin/env summon

from summon import *
from math import *
from summonlib import util

def fractal(depth, offset):
    if depth == 0:
        return group()
    
    branches = 5
    shrink = .38
    step = 2 * pi / 10
    c = 1 - depth / 7.0
    c2 = 1 - (depth - 1) / 7.0
    
    
    vis = []
    for i in xrange(branches):
        angle = i * 2 * pi / branches + offset
        vis.append(group(
            lines(color(1, c, 0), 0,0, color(1, c2, 0), cos(angle), sin(angle)),
            translate(cos(angle), sin(angle),
                      scale(shrink, shrink, fractal(depth - 1, offset + step)))))
    return list2group(vis)
    

util.tic("draw")
add_group(fractal(6, 0))
home()
util.toc()
