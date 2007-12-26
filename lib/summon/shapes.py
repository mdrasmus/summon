"""
    SUMMON - common shapes
    
    This module provideds functions for building common shapes.
"""

from summon import *
import math



def box(x1, y1, x2, y2, fill=True):
    """
    Draws a rectangle (aligned with the x and y axis)
    
    x1, y1 -- one corner of rectangle
    x2, y2 -- the opposite corner of the rectangle
    fill   -- a bool indicating whether the rectangle is filled or stroked
    """

    if fill:
        return quads(x1, y1, x2, y1, x2, y2, x1, y2)
    else:
        return line_strip(x1, y1, x2, y1, x2, y2, x1, y2, x1, y1)



def regularPoly(nsides, radius, fill=True, rotate=0.0):
    """
    Draws a regular 'nsides'-sided polygon with size 'radius'
    
    fill   -- a bool indicating whether polygon is filled or stroked
    rotate -- the number of degrees to rotate the polygon counter-clockwise
    """
    
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


def arrow(headx, heady, tailx, taily, *tail, **options):
    style = options.get("style", "solid")
    head_size = options.get("head_size", 10)

    # compute arrow head vector (unit vector)
    vecx = headx - tailx
    vecy = heady - taily
    length = math.sqrt(vecx*vecx + vecy*vecy)
    vecx *= head_size / length
    vecy *= head_size / length
    
    if style == "solid":
        return group(line_strip(headx, heady, tailx, taily, *tail),
                     zoom_clamp(
                        polygon(headx, heady,
                                headx - vecx - vecy, heady - vecy + vecx,
                                headx - vecx + vecy, heady - vecy - vecx),
                        minx=1, miny=1, maxx=1, maxy=1, 
                        origin=(headx, heady),
                        axis=(tailx, taily)))
    else:
        raise Exception("unknown arrow style '%s'" % style)    


def boxStroke(x1, y1, x2, y2):
    """DEPRECATED: use box(x1, y1, x2, y2, fill=False)"""
    return box(x1, y1, x2, y2, fill=False)

def regularPolyStroke(nsides, size):
    """DEPRECATED: use regularPoly(nsides, size, fill=False)"""
    
