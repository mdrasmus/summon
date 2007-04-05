#!/usr/bin/env summon


from summon.core import *
import math

trix = math.cos(math.pi / 2.0 - 2*math.pi / 3.0)
triy = math.sin(math.pi / 2.0 - 2*math.pi / 3.0)




def tri(size, depth):
    if depth == 0:
        return group(triangles(0, size, size*trix, size*triy, -size*trix, size*triy))
    else:
        # recurse
        
        rad = size + size*triy
        return group(
            translate(0, rad, tri(rad, depth-1)),
            translate(rad*trix, rad*triy, tri(-size*triy, depth-1)),
            translate(-rad*trix, rad*triy, tri(-size*triy, depth-1)))




add_group(tri(100, 8))
home()
