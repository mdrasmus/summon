#!/usr/bin/env python-i
# SUMMON examples
# 01_basics.py - basic commands

# make summon commands available
from summon.core import *
import summon

# syntax of used summon functions
# add_group( <group> )   = adds a group of graphics to the screen
# group( <elements> )    = creates a group from several graphical elements
# lines( x1, y1, x2, y2, ... )  = an element that draws one or more lines
# quads( x1, y1, ..., x4, y4, ... )  = an element that draws one or more quadrilaterals
# color( <red>, <green>, <blue>, [alpha] ) = a primitive that specifies a color


# create a new window
win = summon.Window("01_basics")

# add a line from (0,0) to (30,40)
win.add_group(lines(0,0, 30,40))

# add two blue quadrilaterals inside a group
win.add_group(group(color(0, 0, 1), 
                    quads(50,0, 50,70, 60,70, 60,0),
                    quads(65,0, 65,70, 75,70, 75,0)))

# add a multi-colored quad, where each vertex has it own color
win.add_group(quads(color(1,0,0), 100, 0,
                    color(0,1,0), 100, 70,
                    color(0,0,1), 140, 60,
                    color(1,1,1), 140, 0))


# add some text below everything else
win.add_group(text("Hello, world!",     # text to appear
                   0, -10, 140, -100,   # bounding box of text
                   "center", "top"))    # justification of text in bounding box

# center the "camera" so that all shapes are in view
win.home()
