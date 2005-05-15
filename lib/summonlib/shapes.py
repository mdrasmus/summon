from summon import *
import math


def boxStroke(x1, y1, x2, y2):
    return group(line_strip(vertices(x1, y1, x2, y1, x2, y2, x1, y2, x1, y1)))

def box(x1, y1, x2, y2):
    return group(quads(vertices(x1, y1, x2, y1, x2, y2, x1, y2)))


def regularPolyStroke(nsides, size):
    i = 0
    pts = []
    while i < 2 * 3.14159:
        pts.append(size * math.cos(i))
        pts.append(size * math.sin(i))
        i += (2 * 3.14159 / nsides)
    pts.append(size)
    pts.append(0)
    return group(line_strip(apply(vertices, pts)))

def regularPoly(nsides, size):
    i = 0
    pts = []
    while i < 2 * 3.14159:
        pts.append(size * math.cos(i))
        pts.append(size * math.sin(i))
        i += (2 * 3.14159 / nsides)
    return group(polygon(apply(vertices, pts)))
