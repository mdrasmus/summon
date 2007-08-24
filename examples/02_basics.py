#!/usr/bin/python -i
#!/usr/bin/env python-i
# SUMMON examples
# 02_basics.py - transforms and use of python functions

# make summon commands available
from summon.core import *
import summon


# create a new window
win = summon.Window("02_basics")


# a function for drawing a commonly used shape
def arrow():
    white = color(1,1,1)
    red  = color(1,0,0)
    blue = color(0,0,1)
    
    return group(
        quads(white, 0, 0,        # right fin
              red,   40, -40,
              red,   40, -80,
              white, 0, -40,
              
              white, 0,0,         # left fin
              blue,  -40, -40,
              blue,  -40, -80,
              white, 0, -40),
              
        lines(white, 0,0, 0, 100)) # shaft

# a function for writing text at particular position
def label(string, pos):
    return group(text_scale(string, pos-40, -80, pos+40, -100, "center"))



# draw an arrow with our function
win.add_group(arrow())
win.add_group(label("arrow", 0))

# draw the arrow again, 
# except translated to the right by 100
# NOTE: we must wrap the translate construct in a group because add_group
# expects a group construct
win.add_group(group(translate(100, 0, arrow())))
win.add_group(label("translated arrow", 100))


# draw another arrow thats translated and then rotated 
win.add_group(group(translate(200, 0, rotate(40, arrow()))))
win.add_group(label("rotated arrow", 200))


# draw another arrow thats translated and then fliped over the line (0, 100) 
win.add_group(group(translate(350, 0, 
    flip(0, 100, arrow()),
    label("fliped arrow", 0))))
# notice how the label is now also in the translation

# draw another arrow thats translated and then scaled by a 1/3
win.add_group(group(translate(450, 0, 
    scale(.3, .3, arrow()),
    label("scaled arrow", 0))))


# center the "camera" so that all shapes are in view
win.home()
