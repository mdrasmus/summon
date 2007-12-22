#!/usr/bin/env python-i
#
# demo of text within a zoom_clamp
#

from summon.core import *
import summon
from summon import colors, shapes


win = summon.Window("zoom_text")


def text_box(msg, x1, y1, x2, y2):
    """re-create a text_clip-like element with text_clip and zoom_clamp"""

    return group(colors.blue, 
                 shapes.box(x1, y1, x2, y2, fill=False),
                 zoom_clamp(
                     colors.red,
                     shapes.box(x1, y1, x2, y2, fill=False),
                     text_scale(msg, x1, y1, x2, y2,
                                "center", "middle"),
                     minx=.1, miny=.1, maxx=1, maxy=1, 
                     origin=((x1+x2)/ 2.0, (y1+y2)/2.0), 
                     clip=True, link=True, link_type="smaller"))


win.add_group(group(text_box("hello", 0, 0, 100, 100),
                    text_box("goodbye", 100, 0, 200, 100)))

win.home()
