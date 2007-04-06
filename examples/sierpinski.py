#!/usr/bin/env summon


from summon.core import *
import math

trix = math.cos(math.pi / 2.0 - 2*math.pi / 3.0)
triy = math.sin(math.pi / 2.0 - 2*math.pi / 3.0)


def tri(x, y, size, depth):
    if depth <= 0:
        return group(triangles(x+0, y+size, 
                               x+size*trix, y+size*triy, 
                               x-size*trix, y+size*triy))
    else:
        # recurse
        
        rad = size + size*triy
        return group(tri(x+0, y+rad, rad, depth-1),
                     tri(x+rad*trix, y+rad*triy, -size*triy, depth-1),
                     tri(x-rad*trix, y+rad*triy, -size*triy, depth-1))




add_group(tri(0, 0, 100, 10))
home()
