#!/usr/bin/env python-i
#
# demo of text within a zoom_clamp
#

from summon.core import *
import summon
from summon import colors, shapes


win = summon.Window("zoom_text")


def text_box(msg, x1, y1, x2, y2, *justify):
    """re-create a text_clip-like element with text_clip and zoom_clamp"""
    
    # determine justification
    originx = (x1+x2)/ 2.0
    originy = (y1+y2)/2.0
    
    for j in justify:
        if j == "center":
            originx = (x1+x2)/ 2.0
        elif j == "left":
            originx = x1
        elif j == "right":
            originx = x2
        elif j == "middle":
            originy = (y1+y2)/2.0
        elif j == "top":
            originy = y2
        elif j == "bottom":
            originy = y1
        
        
    return group(colors.blue, 
                 shapes.box(x1, y1, x2, y2, fill=False),
                 zoom_clamp(
                     colors.red,
                     shapes.box(x1, y1, x2, y2, fill=False),
                     text_scale(msg, x1, y1, x2, y2,
                                *justify),
                     minx=.1, miny=.1, maxx=1, maxy=1, 
                     origin=(originx, originy), 
                     clip=True, link=True, link_type="smaller"))


win.add_group(group(text_box("hello", 0, 0, 100, 100, "top", "left"),
                    text_box("goodbye", 100, 0, 200, 100),
                    translate(200, 200,
                        rotate(45, text_box("hi", 0, 0, 100, 100)))))

win.home()
