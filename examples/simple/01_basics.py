#!/usr/bin/env summon
# SUMMON examples
# 01_basics.py - basic commands


# syntax of used summon functions
# add_group( <group> )   : adds a group of graphics to the screen
# group( <elements> )    : creates a group from several graphical elements
# lines( <primitives> )  : an element that draws one or more lines
# quads( <primitives> )  : an element that draws one or more quadrilaterals
# color( <red>, <green>, <blue>, [alpha] ) : a primitive that specifies a color


# add a line from (0,0) to (30,40)
add_group(group(lines(0,0, 30,40)))

# add a quadrilateral
add_group(group(quads(50,0, 50,70, 60,70, 60,0)))

# add a multi-colored quad
add_group(group(quads(
    color(1,0,0), 100, 0,
    color(0,1,0), 100, 70,
    color(0,0,1), 140, 60,
    color(1,1,1), 140, 0)))


# add some text below everything else
add_group(group(
    text("Hello, world!",    # text to appear
         0, -10, 140, -100,    # bounding box of text
         "center", "top")))    # justification of text in bounding box

# center the "camera" so that all shapes are in view
home()
