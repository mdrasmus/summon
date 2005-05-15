# SUMMON examples
# example1.py - basic commands

# make summon commands available
from summon import *

# syntax of used visdraw functions
# add_group( <group> )   : adds a group of graphics to the screen
# group( <elements> )    : creates a group from several graphical elements
# lines( <primitives> )  : an element that draws one or more lines
# quads( <primitives> )  : an element that draws one or more quadrilaterals
# vertices( <x,y ... > ) : a primitive that specifices one or more vertices
# color( <red>, <green>, <blue> ) : a primitive that specifies a color



# clear the screen of all drawing
clear_groups()

# add a line
add_group(group(lines(vertices(0,0, 30,40))))

# add a quad
add_group(group(quads(vertices(50,0, 50,70, 60,70, 60,0))))

# add a multi-colored quad
add_group(group(quads(
    color(1,0,0), vertices(100, 0),
    color(0,1,0), vertices(100, 70),
    color(0,0,1), vertices(140, 60),
    color(1,1,1), vertices(140, 0))))


# add some text below everything else
add_group(group(
    text("this is my text",    # text to appear
         30, -10, 110, -50,    # bounding box of text
         "center", "top")))    # justification of text in bounding box

# center the "camera" so that all shapes are in view
home()
