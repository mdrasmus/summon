"""
    SUMMON - common shapes
    
    This module provideds functions for building common shapes.
"""

from summon import *
import math


def boxStroke(x1, y1, x2, y2):
    """DEPRECATED: use box(x1, y1, x2, y2, fill=False)"""
    return box(x1, y1, x2, y2, fill=False)

def box(x1, y1, x2, y2, fill=True):
    if fill:
        return group(quads(x1, y1, x2, y1, x2, y2, x1, y2))
    else:
        return group(line_strip(x1, y1, x2, y1, x2, y2, x1, y2, x1, y1))


def regularPolyStroke(nsides, size):
    i = 0
    pts = []
    while i < 2 * 3.14159:
        pts.append(size * math.cos(i))
        pts.append(size * math.sin(i))
        i += (2 * 3.14159 / nsides)
    pts.append(size)
    pts.append(0)
    return group(line_strip(* pts))

def regularPoly(nsides, size):
    i = 0
    pts = []
    while i < 2 * 3.14159:
        pts.append(size * math.cos(i))
        pts.append(size * math.sin(i))
        i += (2 * 3.14159 / nsides)
    return group(polygon(* pts))
