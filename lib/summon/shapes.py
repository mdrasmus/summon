"""
    SUMMON - common shapes
    
    This module provideds functions for building common shapes.
"""

from summon import *
import math



def box(x1, y1, x2, y2, fill=True):
    if fill:
        return quads(x1, y1, x2, y1, x2, y2, x1, y2)
    else:
        return line_strip(x1, y1, x2, y1, x2, y2, x1, y2, x1, y1)



def regularPoly(nsides, radius, fill=True, rotate=0.0):
    rotate *= 3.14159 / 180.0
    i = rotate
    pts = []
    while i < rotate + 2 * 3.14159:
        pts.append(radius * math.cos(i))
        pts.append(radius * math.sin(i))
        i += (2 * 3.14159 / nsides)
    
    if fill:
        return polygon(*pts)
    else:
        pts.append(radius)
        pts.append(0)
        return line_strip(*pts)


def boxStroke(x1, y1, x2, y2):
    """DEPRECATED: use box(x1, y1, x2, y2, fill=False)"""
    return box(x1, y1, x2, y2, fill=False)

def regularPolyStroke(nsides, size):
    """DEPRECATED: use regularPoly(nsides, size, fill=False)"""
    
